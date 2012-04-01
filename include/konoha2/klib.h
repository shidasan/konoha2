/*
 * konoha2_inlinelibs.h
 *
 *  Created on: Mar 15, 2012
 *      Author: kimio
 */

#ifndef KONOHA2_INLINELIBS_H_
#define KONOHA2_INLINELIBS_H_

static inline uintptr_t strhash(const char *name, size_t len)
{
	uintptr_t i, hcode = 0;
	for(i = 0; i < len; i++) {
		hcode = name[i] + (31 * hcode);
	}
	return hcode;
}

static inline uintptr_t casehash(const char *name, size_t len)
{
	uintptr_t i, hcode = 0;
	for(i = 0; i < len; i++) {
		hcode = tolower(name[i]) + (31 * hcode);
	}
	return hcode;
}

// --------------------------------------------------------------------------

#define S_cid(X)  S_cid_(_ctx, X)
#define T_cid(X)  S_text(S_cid(X))

static inline kString* S_cid_(CTX, ktype_t ty)
{
	return CT_(ty)->name;
}

#define S_ty(X)  S_ty_(_ctx, X)
#define T_ty(X)  S_text(S_ty(X))

static inline kString* S_ty_(CTX, ktype_t ty)
{
	if(ty < _ctx->share->ca.max) {
		return CT_(ty)->name;
	}
	DBG_P("undefined type sym=%d", ty);
	return TS_EMPTY;
}

#define S_fn(fn)   S_fn_(_ctx, fn)
#define T_fn(fn)   S_text(S_fn_(_ctx, fn))
static inline kString* S_fn_(CTX, ksymbol_t sym)
{
	size_t index = (size_t) MN_UNMASK(sym);
	if(index < kArray_size(_ctx->share->symbolList)) {
		return _ctx->share->symbolList->strings[index];
	}
	DBG_P("undefined symbol sym=%d", sym);
	return TS_EMPTY;
}

static inline const char* filename(const char *str)
{
	char *p = strrchr(str, '/');
	return (p == NULL) ? str : (const char*)p+1;
}

#define S_uri(I)  Suri(_ctx, I)
static inline kString* Suri(CTX, kline_t uri)
{
	kline_t n = (uri >> (sizeof(kshort_t) * 8));
	if(n < kArray_size(_ctx->share->uriList)) {
		return _ctx->share->uriList->strings[n];
	}
	DBG_P("unknown uri=%d", n);
	abort();
	return TS_EMPTY;
}

#endif /* KONOHA2_INLINELIBS_H_ */
