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
		int ret = fclose(file->fp);
		if (ret != 0) {
			ktrace(_SystemFault,
					KEYVALUE_s("@", "fclose"),
					KEYVALUE_s("errstr", strerror(errno))
			);
		}
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
	if (fp == NULL) {
		ktrace(_SystemFault|_ScriptFault,
				KEYVALUE_s("@", "fopen"),
				KEYVALUE_s("path", S_text(s)),
				KEYVALUE_u("mode", mode),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
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
		if (size == 0 && ferror(fp) != 0){
			ktrace(_SystemFault,
					KEYVALUE_s("@", "fread"),
					KEYVALUE_s("errstr", strerror(errno))
			);
			clearerr(fp);
		}
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
		if (size < len) {
			// error
			ktrace(_SystemFault,
					KEYVALUE_s("@", "fwrite"),
					KEYVALUE_s("errstr", strerror(errno))
			);
		}
	}
	RETURNi_(size);
}

//## @Native void File.close();
static KMETHOD File_close(CTX, ksfp_t *sfp _RIX)
{
	struct _kFile *file = (struct _kFile*)sfp[0].o;
	FILE *fp = file->fp;
	if(fp != NULL) {
		int ret = fclose(fp);
		if (ret != 0) {
			ktrace(_SystemFault,
					KEYVALUE_s("@", "fclose"),
					KEYVALUE_s("errstr", strerror(errno))
			);
		}
		file->fp = NULL;
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
		if (ch == EOF && ferror(fp) != 0) {
			ktrace(LOGPOL_DEBUG | _DataFault,
					KEYVALUE_s("@", "fgetc"),
					KEYVALUE_s("errstr", strerror(errno))
			);
		}
	}
	RETURNi_(ch);
}

//## @Native boolean File.putC(int ch);
static KMETHOD File_putC(CTX, ksfp_t *sfp _RIX)
{
	FILE *fp = ((kFile*)sfp[0].o)->fp;
	if (fp != NULL) {
		int ch = fputc(sfp[1].ivalue, fp);
		if (ch == EOF) {
			ktrace(LOGPOL_DEBUG | _DataFault,
					KEYVALUE_s("@", "fputc"),
					KEYVALUE_s("errstr", strerror(errno))
			);
		}
		RETURNb_(ch != EOF);
	}
	RETURNb_(0);
}
/* ======================================================================== */
/* FILE low-level*/

// TODO: functions below will return integer which indecates file descriptor

//## @Native int File.lseek(int offset, int whence)
static KMETHOD File_lseek(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	// fileno only returns EBADF
	if (fp == NULL) RETURNi_(-1);
	int fd = fileno(fp);
	if (fd == -1) {
		ktrace(_DataFault,
				KEYVALUE_s("@", "fileno"),
				KEYVALUE_p("fp", fp)
		);
		RETURNi_(-1);
	}
	int offset = sfp[1].ivalue;
	int whence = sfp[2].ivalue;
	off_t ret_offset = lseek(fd, offset, whence);
	if (ret_offset == -1) {
		ktrace(_DataFault,
				KEYVALUE_s("@", "lseek"),
				KEYVALUE_u("offset", offset),
				KEYVALUE_u("whence", whence),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNi_((int)ret_offset);
}

//## boolean File.truncate(int length)
static KMETHOD File_truncate(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	int length = sfp[1].ivalue;
	if (fp == NULL) RETURNb_(0);
	int fd = fileno(fp);
	if (fd == -1) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "fileno"),
				KEYVALUE_p("fp", fp)
		);
		RETURNb_(0);
	}
	int ret = ftruncate(fd, length);
	if (ret != 0) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "ftruncate"),
				KEYVALUE_u("length", length),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

//## boolean DFile.chmod(int length)
static KMETHOD File_chmod(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	mode_t mode = (mode_t)sfp[1].ivalue;
	if (fp == NULL) RETURNb_(0);
	int fd = fileno(fp);
	if (fd == -1) {
		ktrace(_DataFault,
				KEYVALUE_s("@", "fileno"),
				KEYVALUE_p("fp", fp)
		);
		RETURNb_(0);
	}

	int ret = fchmod(fd, mode);
	if (ret != -1) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "fchmod"),
				KEYVALUE_u("mode", mode),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

//## boolean File.chown(int owner, int group)
static KMETHOD File_chown(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	uid_t owner = (uid_t)sfp[1].ivalue;
	gid_t group = (gid_t)sfp[2].ivalue;
	if (fp == NULL) RETURNb_(0);
	int fd = fileno(fp);
	if (fd == -1) {
		ktrace(_DataFault,
				KEYVALUE_s("@", "fileno"),
				KEYVALUE_p("fp", fp)
		);
		RETURNb_(0);
	}
	int ret = fchown(fd, owner, group);
	if (ret != -1) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "fchown"),
				KEYVALUE_u("owner", owner),
				KEYVALUE_u("group", group),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

// TODO: isn't ioctl difficult for script users? should we support this?
//## @Native int File.ioctl(int request, String[] args)
//staic KMETHOD File_ioctl(CTX, ksfp_t *sfp _RIX)
//{
//	kFile *file = (kFile*)sfp[0].o;
//	FILE *fp = file->fp;
//	int request  = Int_to(int, sfp[1]);
//	char *argp = String_to(char*, sfp[2]);
//	if (fp == NULL) RETURNb_(0);
//	int fd = fileno(fp);
//	if (fd == -1) {
//		KNH_NTRACE2(ctx, "fileno", K_PERROR, KNH_LDATA(LOG_p("fp", fp)));
//		RETURNb_(0);
//	}
//	int ret = ioctl(fd, request, argp);
//	KNH_NTRACE2(ctx, "ioctl", ret != -1 ? K_OK : K_PERROR, KNH_LDATA(
//				LOG_p("fp", fp), LOG_i("request", request), LOG_s("arg", argp)
//				));
//	RETURNb_(ret != -1);
//}

// NOTE: sys_flock can use for a file, only for
//## @Native boolean File.flock(int opretaion);
static KMETHOD File_flock(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	int operation  = sfp[1].ivalue;
	if (fp == NULL) RETURNb_(0);
	int fd = fileno(fp);
	if (fd == -1) {
		ktrace(_DataFault,
				KEYVALUE_s("@", "fileno"),
				KEYVALUE_p("fp", fp)
		);
		RETURNb_(0);
	}
	int ret = flock(fd, operation);
	if (ret == -1) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "flock"),
				KEYVALUE_u("operation", operation),
				KEYVALUE_p("fp", fp),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

//## @Native boolean File.sync();
static KMETHOD File_sync(CTX, ksfp_t *sfp _RIX)
{
	kFile *file = (kFile*)sfp[0].o;
	FILE *fp = file->fp;
	int ret = 1;
	if(fp != NULL) {
		int fd = fileno(fp);
		if (fd == -1) {
			ktrace(_DataFault,
					KEYVALUE_s("@", "fileno"),
					KEYVALUE_p("fp", fp)
			);
			RETURNb_(0);
		}
		ret =  fsync(fd);
		if (ret == -1) {
			ktrace(_SystemFault,
					KEYVALUE_s("@", "fsync"),
					KEYVALUE_p("errstr", strerror(errno))
			);
		}
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
		_Public|_Const|_Im, _F(File_lseek), TY_Int, TY_File, MN_("lseek"), 2, TY_Int, FN_("offset"), TY_Int, FN_("whence"),
		_Public|_Const|_Im, _F(File_chmod), TY_Boolean, TY_File, MN_("chmod"), 1, TY_Int, FN_("length"),
		_Public|_Const|_Im, _F(File_chown), TY_Boolean, TY_File, MN_("chown"), 2, TY_Int, FN_("owner"), TY_Int, FN_("group"),
		_Public|_Const|_Im, _F(File_truncate), TY_Boolean, TY_File, MN_("truncate"), 1, TY_Int, FN_("length"),
		_Public|_Const|_Im, _F(File_flock), TY_Boolean, TY_File, MN_("flock"), 1, TY_Int, FN_("operation"),
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
