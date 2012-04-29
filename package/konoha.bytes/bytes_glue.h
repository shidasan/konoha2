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
typedef iconv_t kiconv_t;
#else
typedef long    kiconv_t;
#endif

typedef kiconv_t (*ficonv_open)(const char *, const char *);
typedef size_t (*ficonv)(kiconv_t, const char **, size_t *, char **, size_t *);
typedef int    (*ficonv_close)(kiconv_t);

typedef struct {
    kmodshare_t h;
    kclass_t     *cBytes;
    void*        (*encode)(const char* from, const char* to, const char* text, size_t len, kwb_t* wb);
    const char*  fmt;
    const char*  locale;
    kiconv_t     (*ficonv_open)(const char *, const char*);
    size_t       (*ficonv)(kiconv_t, const char **, size_t *, char**, size_t *);
    int          (*ficonv_close)(kiconv_t);
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

/* ------------------------------------------------------------------------ */

static void klinkDynamicIconv(CTX)
{
	void *handler = dlopen("libiconv" K_OSDLLEXT, RTLD_LAZY);
	void *f = NULL;
	if (handler != NULL) {
//		f = kdlsym(_ctx, handler, "iconv_open", "libiconv_open", 1/*isTest*/);
		f = dlsym(handler, "iconv_open");
		if (f != NULL) {
			kmodiconv->ficonv_open = (ficonv_open)f;
			kmodiconv->ficonv = (ficonv)dlsym(handler, "iconv");
			kmodiconv->ficonv_close = (ficonv_close)dlsym(handler, "iconv_close");
			KNH_ASSERT(kmodiconv->ficonv != NULL && kmodiconv->ficonv_close != NULL);
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

static kBytes* _new_Bytes(CTX, size_t capacity)
{
	kclass_t *ct = CT_Bytes;
	kBytes *ba = (kBytes*)new_kObject(ct, NULL);
	if(capacity > 0) {
		Bytes_init(_ctx, (kObject*)ba, (void*)capacity);
	}
	return ba;
}

static void Bytes_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kBytes *ba = (kBytes*)sfp[pos].o;
	DBG_P("level:%d", level);
	if(level == 0) {
		kwb_printf(wb, "byte[%d]", ba->bytesize);
	}
	else if(level == 1) {
		size_t i, j, n;
		for(j = 0; j * 16 < ba->bytesize; j++) {
			kwb_printf(wb, "%08x", (int)(j*16));
			for(i = 0; i < 16; i++) {
				n = j * 16 + i;
				if(n < ba->bytesize) {
					kwb_printf(wb, " %2x", (int)ba->utext[n]);
				}
				else {
					kwb_printf(wb, "%s", "   ");
				}
			}
			kwb_printf(wb, "%s", "    ");
			for(i = 0; i < 16; i++) {
				n = j * 16 + i;
				if(n < ba->bytesize && isprint(ba->utext[n])) {
					kwb_printf(wb, "%c", (int)ba->utext[n]);
				}
				else {
					kwb_printf(wb, "%s", " ");
				}
			}
			kwb_printf(wb, "\n");
		}
	}
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
#include <errno.h>
static kBytes* convTo(CTX, kBytes *fromBa, const char *toCoding)
{
	kiconv_t conv;
	kBytes *toBa = _new_Bytes(_ctx, BYTES_BUFSIZE);
	const char *fromBuf = fromBa->text;
	const char ** inbuf = &fromBuf;
	char *toBuf = toBa->buf;
	char ** outbuf = &toBuf;
	size_t fromLen, toLen;
	fromLen = strlen(fromBuf)+ 1;
	toLen = toBa->bytesize;
	DBG_P("to='%s', from='%s'", toCoding, getSystemEncoding());
	conv = kmodiconv->ficonv_open(toCoding, getSystemEncoding());
	if (conv == (kiconv_t)(-1)) {
		ktrace(_UserInputFault,
				KEYVALUE_s("@","iconv_open"),
				KEYVALUE_s("from", "UTF-8"),
				KEYVALUE_s("to", toCoding),
				LOG_STRERROR()
		);
		return (kBytes*)(CT_Bytes->nulvalNUL);
	}
	toLen = kmodiconv->ficonv(conv, inbuf, &fromLen, outbuf, &toLen);
	if (toLen == (size_t)-1) {
		ktrace(_DataFault,
			KEYVALUE_s("@","iconv"),
			KEYVALUE_s("from", "UTF-8"),
			KEYVALUE_s("to", toCoding),
			LOG_STRERROR()
		);
		return (kBytes*)(CT_Bytes->nulvalNUL);
	}
	kmodiconv->ficonv_close(conv);
	return toBa;
}
//## @Const method Bytes Bytes.encode(String fmt);
static KMETHOD Bytes_encode(CTX, ksfp_t *sfp _RIX)
{
	kBytes* ba = sfp[0].ba;
	kString* toCoding = sfp[1].s;
	RETURN_(convTo(_ctx, ba, S_text(toCoding)));
}

//## @Const method String Bytes.decode(String fmt);
static KMETHOD Bytes_decode(CTX, ksfp_t *sfp _RIX)
{
	kBytes* fromBa = sfp[0].ba;
	kString* toCoding = sfp[1].s;
	kBytes *toBa;
	if (toCoding == (kString*)(CT_String->nulvalNUL)) {
		toBa = convTo(_ctx, fromBa, getSystemEncoding());
	} else {
		toBa = convTo(_ctx, fromBa, S_text(toCoding));
	}
	RETURN_(new_kString(S_text(toBa), S_size(toBa), 0));
}

//## @Const method Bytes String.toBytes();
static KMETHOD String_toBytes(CTX, ksfp_t *sfp _RIX)
{
	kString* s = sfp[0].s;
	kBytes* ba = _new_Bytes(_ctx, S_size(s));
	DBG_ASSERT(ba->bytesize >= s->bytesize);
	memcpy(ba->buf, s->utext, S_size(s) + 1);
	DBG_P("%s", ba->buf);
	RETURN_(ba);
}

//## @Const method String Bytes.toString();
static KMETHOD Bytes_toString(CTX, ksfp_t *sfp _RIX)
{
	kBytes *from = sfp[0].ba;
	kBytes *to = convTo(_ctx, from, getSystemEncoding());
	RETURN_(new_kString(S_text(to), S_size(to), 0));
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
