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

#ifndef BYTES_GLUE_H_
#define BYTES_GLUE_H_

#include <konoha2/logger.h>
#include <konoha2/bytes.h>


#ifdef K_USING_ICONV
#include <iconv.h>
typedef iconv_t knh_iconv_t;
#else
typedef long    knh_iconv_t;
#endif


typedef knh_iconv_t (*ficonv_open)(const char *, const char *);
typedef size_t (*ficonv)(knh_iconv_t, const char **, size_t *, char **, size_t *);
typedef int    (*ficonv_close)(knh_iconv_t);

typedef struct {
    kmodshare_t h;
    kclass_t     *cBytes;
    void*        (*encode)(const char* from, const char* to, const char* text, size_t len, kwb_t* wb);
    const char*  fmt;
    const char*  locale;
    ficonv_open  iconv_openSPI;
    ficonv       iconvSPI;
    ficonv_close iconv_closeSPI;
} kmodiconv_t;

typedef struct {
    kmodlocal_t h;
} ctxiconv_t;


/* ------------------------------------------------------------------------ */
/* [util] */

static const char *getSystemEncoding(void)
{
	//TODO!! check LC_CTYPE compatibility with iconv
	char *enc = getenv("LC_CTYPE");
	if(enc != NULL) {
		return enc;
	}
#if defined(K_USING_WINDOWS_)
	static char codepage[64];
	knh_snprintf(codepage, sizeof(codepage), "CP%d", (int)GetACP());
	return codepage;
#else
	return "UTF-8";
#endif
}


static void *kdlopen(CTX, const char* path){
//    const char *func = __FUNCTION__;
    void *handler = NULL;
#if defined(K_USING_WINDOWS_)
//    func = "LoadLibrary";
    handler = (void*)LoadLibraryA((LPCTSTR)path);
#elif defined(K_USING_POSIX_)
//    func = "dlopen";
    handler = dlopen(path, RTLD_LAZY);
#else

#endif
    return handler;
}

static void *kdlsym(CTX, void* handler, const char* symbol, const char *another, int isTest)
{
//    const char *func = __FUNCTION__,
	const char *emsg = NULL;
    void *p = NULL;
#if defined(K_USING_WINDOWS_)
//    func = "GetProcAddress";
    p = GetProcAddress((HMODULE)handler, (LPCSTR)symbol);
    if(p == NULL && another != NULL) {
        symbol = another;
        p = GetProcAddress((HMODULE)handler, (LPCSTR)symbol);
    }
    return p;
#elif defined(K_USING_POSIX_)
//    func = "dlsym";
    p = dlsym(handler, symbol);
    if(p == NULL && another != NULL) {
        symbol = another;
        p = dlsym(handler, symbol);
    }
    if(p == NULL) {
        emsg = dlerror();
    }
#else
#endif
    return p;
}

static int kdlclose(CTX, void* handler)
{
#if defined(K_USING_WINDOWS_)
    return (int)FreeLibrary((HMODULE)handler);
#elif defined(K_USING_POSIX_)
    return dlclose(handler);
#else
    return 0;
#endif
}

const char *knh_dlerror()
{
#if defined(K_USING_POSIX_)
    return dlerror();
#else
    return "unknown dlerror";
#endif
}


/* ------------------------------------------------------------------------ */

static void klinkDynamicIconv(CTX)
{
	void *handler = kdlopen(_ctx, "libiconv" K_OSDLLEXT);
	void *f = NULL;
	if (handler != NULL) {
		f = kdlsym(_ctx, handler, "iconv_open", "libiconv_open", 1/*isTest*/);
		if (f != NULL) {
			kmodiconv->iconv_openSPI = (ficonv_open)f;
			kmodiconv->iconvSPI = (ficonv)kdlsym(_ctx, handler, "iconv", "libiconv", 0/*isTest*/);
			kmodiconv->iconv_closeSPI = (ficonv_close)kdlsym(_ctx, handler, "iconv_close", "libiconv_close", 0);
			KNH_ASSERT(kmodiconv->iconvSPI != NULL && kmodiconv->iconv_closeSPI != NULL);
			return ; // OK
		}
	} else {
		//TODO: need to insert function for kmodiconv
		DBG_P("cannot find libiconv");
	}
}

/* ------------------------------------------------------------------------ */


#define BYTES_BUFSIZE 256

// Bytes_init
static void Bytes_init(CTX, kObject *o, void *conf)
{
	struct _kBytes *ba = (struct _kBytes*)o;
	ba->byteptr = NULL;
	ba->bytesize = (size_t)conf;
	if(ba->bytesize > 0) {
		ba->byteptr = (const char *)KCALLOC(ba->bytesize, 1);
	}
}

static void Bytes_free(CTX, kObject *o)
{
	struct _kBytes *ba = (struct _kBytes*)o;
	if (ba->byteptr != NULL) {
		KFREE(ba->buf, ba->bytesize);
		ba->byteptr = NULL;
		ba->bytesize = 0;
	}
}

static kBytes* new_Bytes(CTX, size_t capacity)
{
	kclass_t *ct = CT_Bytes;
	kBytes *ba = (kBytes*)new_kObject(ct, NULL);
	if(capacity > 0) {
		Bytes_init(_ctx, (kObject*)ba, (void*)capacity);
	}
	return ba;
}

//TODO!! yoan
//static void Bytes_p(CTX, kOutputStream *w, kObject *o, int level)
//{
//	kBytes *ba = (kBytes*)o;
//	if(IS_FMTs(level)) {
//		knh_printf(_ctx, w, "byte[%d]", BA_size(ba));
//	}
//	else if(IS_FMTdump(level)) {
//		size_t i, j, n;
//		char buf[40];
//		for(j = 0; j * 16 < ba->bu.len; j++) {
//			knh_snprintf(buf, sizeof(buf), "%08x", (int)(j*16));
//			knh_write(_ctx, w, B(buf));
//			for(i = 0; i < 16; i++) {
//				n = j * 16 + i;
//				if(n < ba->bu.len) {
//					knh_snprintf(buf, sizeof(buf), " %2x", (int)ba->bu.utext[n]);
//					knh_write(_ctx, w, B(buf));
//				}
//				else {
//					knh_write(_ctx, w, STEXT("   "));
//				}
//			}
//			knh_write(_ctx, w, STEXT("    "));
//			for(i = 0; i < 16; i++) {
//				n = j * 16 + i;
//				if(n < ba->bu.len && isprint(ba->bu.utext[n])) {
//					knh_snprintf(buf, sizeof(buf), "%c", (int)ba->bu.utext[n]);
//					knh_write(_ctx, w, B(buf));
//				}
//				else {
//					knh_write(_ctx, w, STEXT(" "));
//				}
//			}
//			knh_write_EOL(_ctx, w);
//		}
//	}
//}

static void Bytes_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	//kwb_printf(wb, "%f", sfp[pos].fvalue);
}

static void kmodiconv_setup(CTX, struct kmodshare_t *def, int newctx)
{
}

static void kmodiconv_reftrace(CTX, struct kmodshare_t *baseh)
{
}

static void kmodiconv_free(CTX, struct kmodshare_t *baseh)
{
	KFREE(baseh, sizeof(kmodiconv_t));
}

static KMETHOD ExprTyCheck_BYTES(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tk = expr->tk;
	RETURN_(kExpr_setConstValue(expr, TY_Bytes, tk->text));
}

/* ------------------------------------------------------------------------ */

//## @Const method Bytes String.toBytes();
static KMETHOD String_toBytes(CTX, ksfp_t *sfp _RIX)
{
	kString* s = sfp[0].s;
	kBytes* ba = new_Bytes(_ctx, S_size(s));
	RETURN_(ba);
}

//## @Const method String Bytes.toString();
static KMETHOD Bytes_toString(CTX, ksfp_t *sfp _RIX)
{
	kBytes* dst = sfp[0].ba;
	RETURN_(new_kString(S_text(dst), S_size(dst), SPOL_ASCII));
}

//## @Const method Bytes Bytes.encode(String fmt);
static KMETHOD Bytes_encode(CTX, ksfp_t *sfp _RIX)
{
	kBytes* ba = sfp[0].ba;
	kString* to = sfp[1].s;
	knh_iconv_t conv;
	size_t len, olen;
	char ret[BYTES_BUFSIZE] = {'\0'};
	char *r = ret;
	len = olen = ba->bytesize + 1;
	conv = kmodiconv->iconv_openSPI("UTF-8", S_text(to));
	if (conv == (knh_iconv_t)(-1)) {
		ktrace(_UserInputFault,
				KEYVALUE_s("@","iconv_open"),
				KEYVALUE_s("from", "UTF-8"),
				KEYVALUE_s("to", S_text(to))
		);
		return; // TODO!!: kthrow is better
	}
	const char *inbuf = S_text(ba);
	olen = kmodiconv->iconvSPI(conv, &inbuf, &len, &r, &olen);
	if (olen == (size_t)-1) {
		ktrace(_DataFault,
			KEYVALUE_s("@","iconv"),
			KEYVALUE_s("from", "UTF-8"),
			KEYVALUE_s("to", S_text(to))
		);
		return; // TODO!!: kthrow is better
	}
	kmodiconv->iconv_closeSPI(conv);
	Bytes_init(_ctx, (kObject*)ba, (void*)ba->bytesize);
	RETURN_(ba);
}


//## @Const method String Bytes.decode(String fmt);
static KMETHOD Bytes_decode(CTX, ksfp_t *sfp _RIX)
{
	kBytes* src = sfp[0].ba;
	kString* from = sfp[1].s;
	knh_iconv_t conv;
	size_t len, olen;
	char ret[BYTES_BUFSIZE] = {'\0'};
	char *r = ret;
	len = olen = src->bytesize+1;

	conv = kmodiconv->iconv_openSPI("UTF-8", S_text(from));
	if (conv == (knh_iconv_t)(-1)) {
		// @See old/evidence.c
		ktrace(_ScriptFault,
				KEYVALUE_s("@","iconv_open"),
				KEYVALUE_s("from", S_text(src)),
				KEYVALUE_s("to", "UTF-8")
		);
	}
	const char *inbuf = S_text(src);
	olen = kmodiconv->iconvSPI(conv, &inbuf, &len, &r, &olen);
	if (olen == (size_t)-1) {
		ktrace(_DataFault,
			KEYVALUE_s("@","iconv"),
			KEYVALUE_s("from", S_text(src)),
			KEYVALUE_s("to", "UTF-8")
		);
	}
	kmodiconv->iconv_closeSPI(conv);
	RETURN_(new_kString(ret, src->bytesize, SPOL_ASCII));
}

/* ------------------------------------------------------------------------ */
#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Im       kMethod_Immutable
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t bytes_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{

	kmodiconv_t *base = (kmodiconv_t*)KCALLOC(sizeof(kmodiconv_t), 1);
	base->h.name     = "bytes";
	base->h.setup    = kmodiconv_setup;
	base->h.reftrace = kmodiconv_reftrace;
	base->h.free     = kmodiconv_free;
	Konoha_setModule(MOD_iconv, &base->h, pline);

	KDEFINE_CLASS defBytes = {
		STRUCTNAME(Bytes),
		.cflag   = CFLAG_Int,
		.free    = Bytes_free,
		.init    = Bytes_init,
		.p       = Bytes_p,
	};
	base->cBytes = Konoha_addClassDef(PN_sugar, PN_sugar, NULL, &defBytes, pline);
	int FN_encoding = FN_("encoding");
	intptr_t methoddata[] = {
		//_Public|_Const|_Im, _F(String_toBytes), TY_Bytes,  TY_String, MN_to(TY_Bytes),  0,
		//_Public|_Const|_Im, _F(Bytes_toString), TY_String, TY_Bytes,  MN_to(TY_String), 0,
		_Public|_Const|_Im, _F(String_toBytes), TY_Bytes,  TY_String, MN_("toBytes"),   0,
		_Public|_Const|_Im, _F(Bytes_toString), TY_String, TY_Bytes,  MN_("toString"),  0,
		_Public|_Const,     _F(Bytes_encode),   TY_Bytes,  TY_Bytes,  MN_("encode"),    1, TY_String, FN_encoding,
		_Public|_Const,     _F(Bytes_decode),   TY_String, TY_Bytes,  MN_("decode"),    1, TY_String, FN_encoding,
		DEND,
	};
	kKonohaSpace_loadMethodData(NULL, methoddata);
	klinkDynamicIconv(_ctx);
	return true;
}

static kbool_t bytes_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t bytes_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("Bytes"),  .type = TY_Bytes, },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t bytes_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}


#endif /* BYTES_GLUE_H_ */
