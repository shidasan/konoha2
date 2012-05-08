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

#include <errno.h> // include this because of E2BIG
#include <string.h>
#include <langinfo.h>
#include <locale.h>
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
    kbool_t      (*encode)(const char* from, const char* to, const char* text, size_t len, kwb_t* wb);
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
	//DBG_P("%s", nl_langinfo(CODESET));
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


static kbool_t klinkDynamicIconv(CTX, kmodiconv_t *base, kline_t pline)
{
	void *handler = dlopen("libiconv" K_OSDLLEXT, RTLD_LAZY);
	void *f = NULL;
	if (handler != NULL) {
		f = dlsym(handler, "iconv_open");
		if (f != NULL) {
			base->ficonv_open = (ficonv_open)f;
			base->ficonv = (ficonv)dlsym(handler, "iconv");
			base->ficonv_close = (ficonv_close)dlsym(handler, "iconv_close");
			KNH_ASSERT(base->ficonv != NULL && base->ficonv_close != NULL);
			return true;
		}
	}
	kreportf(WARN_, pline, "cannot find libiconv");
	return false;
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

/* ------------------------------------------------------------------------ */

#define CONV_BUFSIZE 4096 // 4K
#define MAX_STORE_BUFSIZE (CONV_BUFSIZE * 1024)// 4M

//static kbool_t encodeFromTo (const char *from, const char *to, const char *text, size_t len, kwb_t *wb)
//{
//
//}

static kBytes* convFromTo(CTX, kBytes *fromBa, const char *fromCoding, const char *toCoding)
{
	kiconv_t conv;
	kwb_t wb;

	char convBuf[CONV_BUFSIZE] = {'\0'};
	const char *presentPtrFrom = fromBa->text;
	const char ** inbuf = &presentPtrFrom;
	char *presentPtrTo = convBuf;
	char ** outbuf = &presentPtrTo;
	size_t inBytesLeft, outBytesLeft;
	inBytesLeft = fromBa->bytesize;
	outBytesLeft = CONV_BUFSIZE;
	DBG_P("from='%s' inBytesLeft=%d, to='%s' outBytesLeft=%d", fromCoding, inBytesLeft, toCoding, outBytesLeft);
//	const char *fromCoding = getSystemEncoding();
	if (strncmp(fromCoding, toCoding, strlen(fromCoding)) == 0) {
		// no need to convert.
		return fromBa;
	}
	conv = kmodiconv->ficonv_open(toCoding, fromCoding);
	if (conv == (kiconv_t)(-1)) {
		ktrace(_UserInputFault,
				KEYVALUE_s("@","iconv_open"),
				KEYVALUE_s("from", fromCoding),
				KEYVALUE_s("to", toCoding)
		);
		return (kBytes*)(CT_Bytes->nulvalNUL);
	}
	size_t iconv_ret = -1;
	size_t processedSize = 0;
	size_t processedTotalSize = processedSize;
	DBG_P("start converting!");
//	karray_t *buf = new_karray(_ctx, 0, 64);
	kwb_init(&(_ctx->stack->cwb), &wb);
	while (inBytesLeft > 0 && iconv_ret == -1) {
		iconv_ret = kmodiconv->ficonv(conv, inbuf, &inBytesLeft, outbuf, &outBytesLeft);
		if (iconv_ret == -1 && errno == E2BIG) {
			DBG_P("too big");
			processedSize = CONV_BUFSIZE - outBytesLeft;
			processedTotalSize += processedSize;
			kwb_printf(&wb, "%s", convBuf);
			// reset convbuf
			presentPtrTo = convBuf;
//			outbuf = &presentPtrTo;
			memset(convBuf, '\0', CONV_BUFSIZE);
			outBytesLeft = CONV_BUFSIZE;
		} else if (iconv_ret == -1) {
			ktrace(_DataFault,
				KEYVALUE_s("@","iconv"),
				KEYVALUE_s("from", "UTF-8"),
				KEYVALUE_s("to", toCoding),
				KEYVALUE_s("error", strerror(errno))
			);
			return (kBytes*)(CT_Bytes->nulvalNUL);
		} else {
			// finished. iconv_ret != -1
			processedSize = CONV_BUFSIZE - outBytesLeft;
			processedTotalSize += processedSize;
			kwb_printf(&wb, "%s", convBuf);
		}
	} /* end of converting loop */
	kmodiconv->ficonv_close(conv);
	DBG_P("processedTotalSize=%d, inbuf='%s', outbuf='%s'", processedTotalSize, *inbuf, *outbuf);
	const char *kwb_topChar = kwb_top(&wb, 1);
	DBG_P("kwb:'%s'", kwb_topChar);
	kBytes *toBa = (kBytes*)new_kObject(CT_Bytes, (void*)processedTotalSize);
	memcpy(toBa->buf, kwb_topChar, processedTotalSize);
	return toBa;
}

//## @Const method Bytes Bytes.encodeTo(String toEncoding);
static KMETHOD Bytes_encodeTo(CTX, ksfp_t *sfp _RIX)
{
	kBytes* ba = sfp[0].ba;
	kString* toCoding = sfp[1].s;
	RETURN_(convFromTo(_ctx, ba, "UTF-8", S_text(toCoding)));
}

//## @Const method String Bytes.decodeFrom(String fromEncoding);
static KMETHOD Bytes_decodeFrom(CTX, ksfp_t *sfp _RIX)
{
	kBytes* fromBa = sfp[0].ba;
	kString*fromCoding = sfp[1].s;
	kBytes *toBa;
	if (fromCoding != (kString*)(CT_String->nulvalNUL)) {
		toBa = convFromTo(_ctx, fromBa, S_text(fromCoding), "UTF-8");
	} else {
		// conv from default encoding
		toBa = convFromTo(_ctx, fromBa, getSystemEncoding(), "UTF-8");
	}
	RETURN_(new_kString(toBa->buf,toBa->bytesize, 0));
}

//## @Const method Bytes String.toBytes();
static KMETHOD String_toBytes(CTX, ksfp_t *sfp _RIX)
{
	kString* s = sfp[0].s;
	kBytes* ba = (kBytes*)new_kObject(CT_Bytes, S_size(s));
	memcpy(ba->buf, s->utext, S_size(s));
	RETURN_(ba);
}

// this method is same as Bytes.decodeFrom(defaultencoding);
//## @Const method String Bytes.toString();
static KMETHOD Bytes_toString(CTX, ksfp_t *sfp _RIX)
{
	kBytes *from = sfp[0].ba;
	kBytes *to = convFromTo(_ctx, from, getSystemEncoding(), "UTF-8");
	RETURN_(new_kString(to->buf, to->bytesize, 0));
}

//## Int Bytes.get(Int n);
static KMETHOD Bytes_get(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t n = check_index(_ctx, sfp[1].ivalue, ba->bytesize, sfp[K_RTNIDX].uline);
	RETURNi_(ba->utext[n]);
}

//## method Int Bytes.set(Int n, Int c);
static KMETHOD Bytes_set(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t n = check_index(_ctx, sfp[1].ivalue, ba->bytesize, sfp[K_RTNIDX].uline);
	ba->ubuf[n] = (const char) sfp[2].ivalue;
	RETURNi_(ba->utext[n]);
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
	base->h.name     = 	klinkDynamicIconv(_ctx, base, pline) ? "iconv" : "noconv";
	base->h.setup    = kmodiconv_setup;
	base->h.reftrace = kmodiconv_reftrace;
	base->h.free     = kmodiconv_free;
	Konoha_setModule(MOD_iconv, &base->h, pline);

	KDEFINE_CLASS defBytes = {
		STRUCTNAME(Bytes),
		.cflag   = kClass_Final,
		.free    = Bytes_free,
		.init    = Bytes_init,
		.p       = Bytes_p,
	};
	base->cBytes = Konoha_addClassDef(ks->packid, PN_konoha, NULL, &defBytes, pline);
	int FN_encoding = FN_("encoding");
	int FN_x = FN_("x");
	int FN_c = FN_("c");
	intptr_t methoddata[] = {
		_Public|_Im|_Coercion, _F(String_toBytes), TY_Bytes,  TY_String, MN_("toBytes"),   0,
		_Public|_Const|_Im|_Coercion, _F(Bytes_toString), TY_String, TY_Bytes,  MN_("toString"),  0,
		_Public|_Const,     _F(Bytes_encodeTo),   TY_Bytes,  TY_Bytes,  MN_("encodeTo"),    1, TY_String, FN_encoding,
		_Public|_Const,     _F(Bytes_decodeFrom),   TY_String, TY_Bytes,  MN_("decodeFrom"),    1, TY_String, FN_encoding,
		_Public|_Const|_Im,     _F(Bytes_get), TY_Int, TY_Bytes, MN_("get"), 1, TY_Int, FN_x,
		_Public|_Const|_Im,     _F(Bytes_set), TY_Int, TY_Bytes, MN_("set"), 2, TY_Int, FN_x, TY_Int, FN_c,

		DEND,
	};
	kKonohaSpace_loadMethodData(NULL, methoddata);
	return true;
}

static kbool_t bytes_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}


static int parseSQUOTE(CTX, struct _kToken *tk, tenv_t *tenv, int tok_start, kMethod *thunk)
{
	USING_SUGAR;
	int ch, prev = '\'', pos = tok_start + 1;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\n') {
			break;
		}
		if(ch == '\'' && prev != '\\') {
			if(IS_NOTNULL(tk)) {
				KSETv(tk->text, new_kString(tenv->source + tok_start + 1, (pos-1)- (tok_start+1), 0));
				tk->tt = TK_CODE;
				tk->kw = KW_("$SQUOTE");
			}
			return pos;
		}
		prev = ch;
	}
	if(IS_NOTNULL(tk)) {
		kreportf(ERR_, tk->uline, "must close with \'");
	}
	return pos-1;
}


static KMETHOD ExprTyCheck_Squote(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tk = expr->tk;
	kString *s = tk->text;
	if (S_size(s) == 1) {
		int ch = S_text(s)[0];
		RETURN_(kExpr_setNConstValue(expr, TY_Int, ch));
	} else {
		kline_t uline = SUGAR Expr_uline(_ctx, expr, 1);
		int lpos = -1;/*why?*/
		SUGAR p(_ctx, ERR_, uline, lpos, "single quote doesn't accept multi characters, '%s'", S_text(s));
	}
	RETURN_(K_NULLEXPR);
}

static kbool_t bytes_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	SUGAR KonohaSpace_setTokenizer(_ctx, ks, '\'', parseSQUOTE, NULL);
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("$SQUOTE"), _TERM, ExprTyCheck_(Squote)},
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
