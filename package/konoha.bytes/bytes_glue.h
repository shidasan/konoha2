/*
 * bytes_glue.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kimio, yoan
 */

#ifndef BYTES_GLUE_H_
#define BYTES_GLUE_H_
#include <iconv.h>

#define kbytesmod        ((kbytesmod_t*)_ctx->mod[MOD_bytes])
#define kmodbytes        ((kmodbytes_t*)_ctx->modshare[MOD_bytes])
#define IS_defineBytes() (_ctx->modshare[MOD_bytes] != NULL)
#define CT_Bytes         kmodbytes->cBytes
#define TY_Bytes         kmodbytes->cBytes->cid

#define IS_Bytes(O)      ((O)->h.ct == CT_Bytes)

typedef struct {
    kmodshare_t h;
    kclass_t *cBytes;
    void* (*encode)(const char* from, const char* to, const char* text, size_t len, kwb_t* wb);
    const char* fmt;
    const char* locale;
} kmodbytes_t;

typedef struct {
    kmodlocal_t h;
} kbytesmod_t;

// Int
static void Bytes_init(CTX, kObject *o, void *conf)
{
	//struct _kNumber *n = (struct _kNumber*)o;  // kFloat has the same structure
	//n->ndata = (uintptr_t)conf;  // conf is unboxed data
}

static void Bytes_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	//kwb_printf(wb, "%f", sfp[pos].fvalue);
}

static void kmodbytes_setup(CTX, struct kmodshare_t *def, int newctx)
{
}

static void kmodbytes_reftrace(CTX, struct kmodshare_t *baseh)
{
}

static void kmodbytes_free(CTX, struct kmodshare_t *baseh)
{
	KNH_FREE(baseh, sizeof(kmodbytes_t));
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

#define K_BYTES_BUFSIZE 256

static inline kbytes_t _S_tobytes(CTX, kString *s)
{
	kbytes_t b;
	b.text = S_text(s);
	b.bytesize  = S_size(s);
	return b;
}
#define S_tobytes(s) _S_tobytes(_ctx, s)
#define S_totext(s) (s->text)
//#define S_size(s) (s->bytesize)

static kBytes* new_Bytes(CTX, const char *text, size_t len)
{
	kclass_t *ct = CT_Bytes;
	struct _kBytes *b = NULL; //knh_PtrMap_getS(_ctx, ct->constPoolMapNULL, text, len);
	if(b != NULL) return b;
	b = (struct _kBytes*)new_kObject(ct, NULL);
	b->text = text;
	b->bytesize = len;
	return b;
}

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
	kBytes* src = sfp[0].ba;
	kString* to = sfp[1].s;
	iconv_t c;
	size_t len, olen;
	char ret[K_BYTES_BUFSIZE] = {'\0'};
	char *r = ret;
	len = olen = src->bytesize+1;

	c = iconv_open("UTF-8", to);
	if (c == (iconv_t)(-1)) {
		perror("ERROR: iconv open");
		return NULL;
	}
	olen = iconv(c, &S_totext(src), &len, &r, &olen);
	if (olen == (size_t)-1) {
		perror("ERROR: iconv");
		return NULL;
	}
	iconv_close(c);
	RETURN_(new_Bytes(_ctx, ret, src->bytesize));
}

//## @Const method String Bytes.decode(String fmt);
static KMETHOD Bytes_decode(CTX, ksfp_t *sfp _RIX)
{
	kBytes* src = sfp[0].ba;
	kString* from = sfp[1].s;
	//fprintf(stderr, "(decode)from: %s, text: %s, size: %lu\n", S_totext(from), S_totext(src), src->bytesize);
	iconv_t c;
	size_t len, olen;
	char ret[K_BYTES_BUFSIZE] = {'\0'};
	char *r = ret;
	len = olen = src->bytesize+1;

	c = iconv_open("UTF-8", from);
	if (c == (iconv_t)(-1)) {
		perror("ERROR: iconv open");
		return NULL;
	}
	olen = iconv(c, &S_totext(src), &len, &r, &olen);
	if (olen == (size_t)-1) {
		perror("ERROR: iconv");
		return NULL;
	}
	iconv_close(c);
	//fprintf(stderr, "(decode)ret: %s\n", ret);

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
	kmodbytes_t *base = (kmodbytes_t*)KNH_ZMALLOC(sizeof(kmodbytes_t));
	base->h.name     = "bytes";
	base->h.setup    = kmodbytes_setup;
	base->h.reftrace = kmodbytes_reftrace;
	base->h.free     = kmodbytes_free;
	Konoha_setModule(MOD_bytes, &base->h, pline);

	KDEFINE_CLASS BytesDef = {
		STRUCTNAME(Bytes),
		.packid  = ks->packid,
		.packdom = 0,
		.cflag   = CFLAG_Int,
		.init    = Bytes_init,
		.p       = Bytes_p,
	};
	base->cBytes = Konoha_addClassDef(NULL, &BytesDef, pline);
	int FN_x = FN_("x");
	intptr_t methoddata[] = {
		//_Public|_Const|_Im, _F(String_toBytes), TY_Bytes,  TY_String, MN_to(TY_Bytes),  0,
		//_Public|_Const|_Im, _F(Bytes_toString), TY_String, TY_Bytes,  MN_to(TY_String), 0,
		_Public|_Const|_Im, _F(String_toBytes), TY_Bytes,  TY_String, MN_("toBytes"),   0,
		_Public|_Const|_Im, _F(Bytes_toString), TY_String, TY_Bytes,  MN_("toString"),  0,
		_Public|_Const,     _F(Bytes_encode),   TY_Bytes,  TY_Bytes,  MN_("encode"),    1, TY_String, FN_x,
		_Public|_Const,     _F(Bytes_decode),   TY_String, TY_Bytes,  MN_("decode"),    1, TY_String, FN_x,
		DEND,
	};
	Konoha_loadMethodData(NULL, methoddata);
	return true;
}
#endif /* BYTES_GLUE_H_ */
