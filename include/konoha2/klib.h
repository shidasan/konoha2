/*
 * konoha2_inlinelibs.h
 *
 *  Created on: Mar 15, 2012
 *      Author: kimio
 */

#ifndef KONOHA2_INLINELIBS_H_
#define KONOHA2_INLINELIBS_H_

static inline size_t size64(size_t s)
{
	size_t base = sizeof(kObject);
	while(base < s) {
		base *= 2;
	}
	return base;
}

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

static inline const char* filename(const char *str)
{
	/*XXX g++ 4.4.5 need char* cast to compile it. */
	char *p = (char *) strrchr(str, '/');
	return (p == NULL) ? str : (const char*)p+1;
}

#define S_file(X)  S_fileid(_ctx, X)
#define T_file(X)  S_text(S_fileid(_ctx, X))
static inline kString* S_fileid(CTX, kline_t fileid)
{
	kline_t n = (fileid >> (sizeof(kshort_t) * 8));
	DBG_ASSERT(n < kArray_size(_ctx->share->fileidList));
	return _ctx->share->fileidList->strings[n];
}

#define S_pack(X)  S_pack_(_ctx, X)
#define T_pack(X)  S_text(S_pack_(_ctx, X))
static inline kString* S_pack_(CTX, kpack_t packid)
{
	DBG_ASSERT(packid < kArray_size(_ctx->share->packList));
	return _ctx->share->packList->strings[packid];
}

#define S_un(X)  S_un_(_ctx, X)
#define T_un(X)  S_text(S_un_(_ctx, X))
static inline kString* S_un_(CTX, kuname_t un)
{
	DBG_ASSERT(un < kArray_size(_ctx->share->unameList));
	return _ctx->share->unameList->strings[un];
}

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


#endif /* KONOHA2_INLINELIBS_H_ */
