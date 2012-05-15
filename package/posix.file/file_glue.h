/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

/* ************************************************************************ */

#ifndef FILE_GLUE_H_
#define FILE_GLUE_H_

#define MOD_file 22

typedef struct {
    kmodshare_t h;
    kclass_t     *cFile;
} kmodfile_t;

typedef struct {
    kmodlocal_t h;
} ctxfile_t;


typedef const struct _kFile kFile;
struct _kFile {
	kObjectHeader h;
	FILE *fp;
};

/* ------------------------------------------------------------------------ */

static void File_init(CTX, kObject *o, void *conf)
{
	struct _kFile *file = (struct _kFile*)o;
	file->fp = (conf != NULL) ? conf : NULL;
}

static void File_free(CTX, kObject *o)
{
	struct _kFile *file = (struct _kFile*)o;
	if (file->fp != NULL) {
		fclose(file->fp);
		file->fp = NULL;
	}
}


static void File_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	//TODO
}

/* ------------------------------------------------------------------------ */
//## @Native @Throwable File System.fopen(String path, String mode);
static KMETHOD System_fopen(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	const char *mode = IS_NULL(sfp[2].s) ? "r" : S_text(sfp[2].s);
	FILE *fp = fopen(S_text(s), mode);
	DBG_P("fopen=%p", fp);
	RETURN_(new_kObject(O_ct(sfp[K_RTNIDX].o), fp));
}
//## @Native int File.read(Bytes buf, int offset, int len);
static KMETHOD File_read(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	size_t size = 0;
	if(fp != NULL) {
		kBytes *ba = sfp[1].ba;
		size_t offset = (size_t)sfp[2].ivalue;
		size_t len = (size_t)sfp[3].ivalue;
		size = ba->bytesize;
		if(!(offset < size)) {
			kline_t uline = sfp[K_RTNIDX].uline;
			kreportf(CRIT_, uline, "OutOfRange!!, offset=%d, size=%d", offset, size);
		}
		if(len == 0) len = size - offset;
		size = fread(ba->buf + offset, 1, len, fp);
		DBG_P("FileRead size=%d", size);
	}
	RETURNi_(size);
}

//## @Native int File.write(Bytes buf, int offset, int len);
static KMETHOD File_write(CTX , ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	size_t size = 0;
	if(fp != NULL) {
		kBytes *ba = sfp[1].ba;
		size_t offset = (size_t)sfp[2].ivalue;
		size_t len = (size_t)sfp[3].ivalue;
		size = ba->bytesize;
		if(len == 0) len = size - offset;
		size = fwrite(ba->buf + offset, 1, len, fp);
	}
	RETURNi_(size);
}

//## @Native void File.close();
static KMETHOD File_close(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	if(fp != NULL) {
		fclose(fp);
//		file->fp = NULL;
	}
	RETURNvoid_();
}

//## @Native int File.getC();
static KMETHOD File_getC(CTX, ksfp_t *sfp _RIX)
{
	FILE *fp = ((kFile*)sfp[0].o)->fp;
	int ch = EOF;
	if (fp != NULL) {
		ch = fgetc(fp);
	}
	RETURNi_(ch);
}

//## @Native boolean File.putC(int ch);
static KMETHOD File_putC(CTX, ksfp_t *sfp _RIX)
{
	FILE *fp = ((kFile*)sfp[0].o)->fp;
	if (fp != NULL) {
		int ch = fputc(sfp[1].ivalue, fp);
		RETURNb_(ch != EOF);
	}
	RETURNb_(0);
}

//## @Native boolean File.sync();
KMETHOD File_sync(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	int ret = 1;
	if(fp != NULL) {
		int fd = fileno(fp);
		if (fd == -1) {
			ktrace(LOGPOL_DEBUG | _DataFault,
					KEYVALUE_s("@", "fileno")
//					KEYVALUE_p("fp", fp)
					);
			RETURNb_(0);
		}
		ret =  fsync(fd);
	}
	RETURNb_(ret == 0);
}
// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _Im kMethod_Immutable
#define _F(F)   (intptr_t)(F)

#define CT_File         cFile
#define TY_File         cFile->cid
#define IS_File(O)      ((O)->h.ct == CT_File)

static kbool_t file_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	KDEFINE_CLASS defFile = {
		STRUCTNAME(File),
		.cflag = kClass_Final,
		.init  = File_init,
		.free  = File_free,
		.p     = File_p,
	};

	kclass_t *cFile = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &defFile, pline);
	intptr_t MethodData[] = {
		_Public|_Const|_Im, _F(System_fopen), TY_File, TY_System, MN_("fopen"), 2, TY_String, FN_("path"), TY_String, FN_("mode"),
		_Public|_Const|_Im, _F(File_close), TY_void, TY_File, MN_("close"), 0,
		_Public|_Const|_Im, _F(File_sync), TY_Boolean, TY_File, MN_("sync"), 0,
		_Public|_Const|_Im, _F(File_getC), TY_Int, TY_File, MN_("getC"), 0,
		_Public|_Const|_Im, _F(File_putC), TY_Boolean, TY_File, MN_("putC"), 1, TY_Int, FN_("ch"),
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	if (IS_defineBytes()) {
		// the function below uses Bytes
		intptr_t MethodData2[] = {
			_Public|_Const, _F(File_write), TY_Int, TY_File, MN_("write"), 3, TY_Bytes, FN_("buf"), TY_Int, FN_("offset"), TY_Int, FN_("len"),
			_Public|_Const, _F(File_read), TY_Int, TY_File, MN_("read"), 3, TY_Bytes, FN_("buf"), TY_Int, FN_("offset"), TY_Int, FN_("len"),
			DEND,
		};
		kKonohaSpace_loadMethodData(ks, MethodData2);
	} else {
		kreportf(INFO_, pline, "konoha.bytes package hasn't imported. Some features are still disabled.");
	}
	return true;
}

static kbool_t file_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t file_initKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t file_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#endif /* FILE_GLUE_H_ */
