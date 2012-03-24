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

// --------------------------------------------------------------------------

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
