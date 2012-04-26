/*
 * bytes_glue.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kimio, yoan
 */

#ifndef BYTES_GLUE_H_
#define BYTES_GLUE_H_
#include <iconv.h>

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

/* ------------------------------------------------------------------------ */
/* [class defs] */
#define ctxiconv        ((ctxiconv_t*)_ctx->mod[MOD_iconv])
#define kmodiconv        ((kmodiconv_t*)_ctx->modshare[MOD_iconv])
#define IS_defineBytes() (_ctx->modshare[MOD_iconv] != NULL)
#define CT_Bytes         kmodiconv->cBytes
#define TY_Bytes         kmodiconv->cBytes->cid

#define IS_Bytes(O)      ((O)->h.ct == CT_Bytes)

#define BYTES_BUFSIZE 256

typedef struct {
    kmodshare_t h;
    kclass_t *cBytes;
    void* (*encode)(const char* from, const char* to, const char* text, size_t len, kwb_t* wb);
    const char* fmt;
    const char* locale;
} kmodiconv_t;

typedef struct {
    kmodlocal_t h;
} ctxiconv_t;

// Int
static void Bytes_init(CTX, kObject *o, void *conf)
{
	struct _kBytes *ba = (struct _kBytes*)o;
	ba->byteptr = NULL;
	ba->bytesize = (size_t)conf;
	if(ba->bytesize > 0) {
		ba->byteptr = (const char *)KNH_ZMALLOC(ba->bytesize);
	}
}

static void Bytes_free(CTX, kObject *o)
{
	struct _kBytes *ba = (struct _kBytes*)o;
	if (ba->byteptr != NULL) {
		KNH_FREE(ba->buf, ba->bytesize);
		ba->byteptr = NULL;
		ba->bytesize = 0;
	}
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
	KNH_FREE(baseh, sizeof(kmodiconv_t));
}

static KMETHOD ExprTyCheck_BYTES(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tk = expr->tk;
	RETURN_(kExpr_setConstValue(expr, TY_Bytes, tk->text));
}

static kbool_t local_initbytes(CTX, kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("Bytes"),  .type = TY_Bytes, },
		{ TOKEN("$BYTES"), .kw = KW_TK(TK_BYTES), .ExprTyCheck = ExprTyCheck_BYTES, },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t share_initbytes(CTX, kKonohaSpace *ks, kline_t pline);


/* ------------------------------------------------------------------------ */

//## @Const method Bytes String.toBytes();
static KMETHOD String_toBytes(CTX, ksfp_t *sfp _RIX)
{
	kString* s = sfp[0].s;
	kBytes* b = new_Bytes(_ctx, S_totext(s), S_size(s));
	RETURN_(b);
	//RETURN_(new_Bytes(_ctx, S_totext(s), S_size(s));
}

//## @Const method String Bytes.toString();
static KMETHOD Bytes_toString(CTX, ksfp_t *sfp _RIX)
{
	kBytes* dst = sfp[0].ba;
	RETURN_(new_kString(S_totext(dst), S_size(dst), SPOL_ASCII));
}


//## @Const method Bytes Bytes.encode(String fmt);
static KMETHOD Bytes_encode(CTX, ksfp_t *sfp _RIX)
{
	kBytes* ba = sfp[0].ba;
	kString* to = sfp[1].s;
	iconv_t c;
	size_t len, olen;
	char ret[BYTES_BUFSIZE] = {'\0'};
	char *r = ret;
	len = olen = ba->bytesize + 1;
	c = iconv_open("UTF-8", to);
	if (c == (iconv_t)(-1)) {
		perror("ERROR: iconv open");
		return NULL;
	}
//	olen = iconv(c, &S_totext(ba), &len, &r, &olen);
	if (olen == (size_t)-1) {
		perror("ERROR: iconv");
		return NULL;
	}
	iconv_close(c);
	RETURN_(Bytes_init(_ctx, ret, ba->bytesize));
}

//## @Const method String Bytes.decode(String fmt);
static KMETHOD Bytes_decode(CTX, ksfp_t *sfp _RIX)
{
	kBytes* src = sfp[0].ba;
	kString* from = sfp[1].s;
	//fprintf(stderr, "(decode)from: %s, text: %s, size: %lu\n", S_totext(from), S_totext(src), src->bytesize);
	iconv_t c;
	size_t len, olen;
	char ret[BYTES_BUFSIZE] = {'\0'};
	char *r = ret;
	len = olen = src->bytesize+1;

	c = iconv_open("UTF-8", from);
	if (c == (iconv_t)(-1)) {
		perror("ERROR: iconv open");
		return NULL;
	}
//	olen = iconv(c, &S_totext(src), &len, &r, &olen);
	if (olen == (size_t)-1) {
		perror("ERROR: iconv");
		return NULL;
	}
	iconv_close(c);
	RETURN_(new_kString(ret, src->bytesize, SPOL_ASCII));
}

/* ------------------------------------------------------------------------ */

static kbool_t bytes_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	return share_initbytes(_ctx, ks, pline);
}

static kbool_t bytes_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t bytes_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return local_initbytes(_ctx, ks, pline);
}

static kbool_t bytes_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}


// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Im       kMethod_Immutable
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t share_initbytes(CTX, kKonohaSpace *ks, kline_t pline)
{
	kmodiconv_t *base = (kmodiconv_t*)KNH_ZMALLOC(sizeof(kmodiconv_t));
	base->h.name     = "bytes";
	base->h.setup    = kmodiconv_setup;
	base->h.reftrace = kmodiconv_reftrace;
	base->h.free     = kmodiconv_free;
	Konoha_setModule(MOD_iconv, &base->h, pline);

	KDEFINE_CLASS BytesDef = {
		STRUCTNAME(Bytes),
		.packid  = ks->packid,
		.packdom = 0,
		.cflag   = CFLAG_Int,
		.init    = Bytes_init,
		.p       = Bytes_p,
	};
	base->cBytes = Konoha_addClassDef(NULL, &BytesDef, pline);
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
	Konoha_loadMethodData(NULL, methoddata);
	return true;
}
#endif /* BYTES_GLUE_H_ */
