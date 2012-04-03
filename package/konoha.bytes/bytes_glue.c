#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#define K_USING_UTF8 1 /*FIXME*/
#define Int_to(T, a)               ((T)a.ivalue)
#ifdef K_USING_DEBUG
#define TODO() do {\
    fflush(stdout); \
    fprintf(stderr, "%s*TODO(%s:%d) TODO\n", __FUNCTION__, knh_sfile(__FILE__), __LINE__); \
} while (0)
#else
#define TODO()
#define KNH_LOG(...) DBG_P(__VA_ARGS__)
#endif/*K_USING_DEBUG*/

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif
/* ------------------------------------------------------------------------ */
/* bytes module */
#define MOD_BYTES 20
#define kbytesmod   ((kbytesmod_t*)_ctx->mod[MOD_BYTES])
#define kbytesshare ((kbytesshare_t*)_ctx->modshare[MOD_BYTES])
#define CT_Bytes    kbytesshare->cBytes
#define TY_Bytes    kbytesshare->cBytes->cid
#define CT_StringDecoder    kbytesshare->cStringDecoder
#define TY_StringDecoder    kbytesshare->cStringDecoder->cid
#define CT_StringEncoder    kbytesshare->cStringEncoder
#define TY_StringEncoder    kbytesshare->cStringEncoder->cid

typedef struct {
	kmodshare_t h;
	const kclass_t *cBytes;
	const kclass_t *cStringDecoder;
	const kclass_t *cStringEncoder;
} kbytesshare_t;

typedef struct {
	kmod_t h;
} kbytesmod_t;

static void kbytesshare_setup(CTX, struct kmodshare_t *def)
{
}

static void kbytesshare_reftrace(CTX, struct kmodshare_t *baseh)
{
}

static void kbytesshare_free(CTX, struct kmodshare_t *baseh)
{
	KNH_FREE(baseh, sizeof(kbytesshare_t));
}

/* ------------------------------------------------------------------------ */
//## class Bytes Object;

typedef struct kBytes {
	kObjectHeader h;
	kbytes_t   bu;
	size_t capacity;
} kBytes;

static void Bytes_init(CTX, kRawPtr *o, void *conf)
{
}

static void knh_Bytes_dispose(CTX, kBytes *ba);
static void Bytes_free(CTX, kRawPtr *o)
{
	kBytes *ba = (kBytes *) o;
	knh_Bytes_dispose(_ctx, ba);
}
#define BA_size(o)      ((o)->bu.len)
#define BA_text(o)      ((const char*) (O_ct(o)->unbox(_ctx, (kObject*)(o))))
#define k_grow(N)       ((N)*2)

static KDEFINE_CLASS BytesDef = {
	STRUCTNAME(Bytes),
	.cflag = 0,
	.init = Bytes_init,
	.free = Bytes_free,
};

/* ------------------------------------------------------------------------ */
//## @Immutable class Converter Object;

typedef void knh_conv_t;

typedef struct knh_ConverterDPI_t {
	int  type;
	const char *name;
	knh_conv_t* (*open)(CTX, const char*, const char*);
	kbool_t  (*conv)(CTX,  knh_conv_t *, const char*, size_t, kwb_t *);
	kbool_t  (*enc)(CTX,   knh_conv_t *, const char*, size_t, kwb_t *);
	kbool_t  (*dec)(CTX,   knh_conv_t *, const char*, size_t, kwb_t *);
	kbool_t  (*sconv)(CTX, knh_conv_t *, const char*, size_t, kwb_t *);
	void (*close)(CTX, knh_conv_t*);
	void (*setparam)(CTX, knh_conv_t *, void *, void *);
} knh_ConverterDPI_t;

typedef struct kConverter {
	kObjectHeader h;
	knh_conv_t *conv;
	const struct knh_ConverterDPI_t *dpi;
} kConverter;

/* ------------------------------------------------------------------------ */
//## @Immutable class StringEncoder Converter;

typedef struct kStringEncoder {
	kObjectHeader h;
	knh_conv_t *conv;
	const struct knh_ConverterDPI_t *dpi;
} kStringEncoder;

static KDEFINE_CLASS StringEncoderDef = {
	STRUCTNAME(StringEncoder),
	.cflag = 0,
};

/* ------------------------------------------------------------------------ */
//## @Immutable class StringDecoder Converter;

typedef struct kStringDecoder {
	kObjectHeader h;
	knh_conv_t *conv;
	const struct knh_ConverterDPI_t *dpi;
} kStringDecoder;

static KDEFINE_CLASS StringDecoderDef = {
	STRUCTNAME(StringDecoder),
	.cflag = 0,
};

/* ------------------------------------------------------------------------ */
static size_t k_goodsize(size_t ss)
{
	size_t s = ss;
	if(s == 0) return 0;  /* added */
	if(s <= K_FASTMALLOC_SIZE) return K_FASTMALLOC_SIZE;
	if(s > 64 * 1024) return s;
	s |= s >> 1;
	s |= s >> 2;
	s |= s >> 4;
	s |= s >> 8;
	s |= s >> 16;
	return ((s + 1) == ss * 2) ? ss : s + 1;
}

static kString *kwb_newString(CTX, kwb_t *wb, int flg)
{
	return new_kString(kwb_top(wb, flg), kwb_size(wb), SPOL_POOL);
}
static kbytes_t kwb_tobytes(CTX, kwb_t *wb)
{
	kbytes_t b = {kwb_size(wb), {kwb_top(wb, 0)}};
	return b;
}


//static void Bytes_checkstack(CTX, char *oldstart, char *oldend, char* newstart)
//{
//	char **cstack_top = (char**)(&_ctx);
//	char **p = (char**)_ctx->stack->cstack_bottom;
//	DBG_P("cstack_bottom=%p, cstack_top=%p basedata=%p", p[0], cstack_top, &oldstart);
//	if(!(p < cstack_top)) {
//		char **tmp = cstack_top;
//		cstack_top = p; p = tmp;
//	}
//	while(p <= cstack_top) {
//		if((oldstart <= p[0] && p[0] < oldend)) {
//			char *newc = p[0] + (newstart - oldstart);
//			KNH_LOG("oldptr=%p, newptr=%p", p[0], newc);
//			p[0] = newc;
//		}
//		p++;
//	}
//}

static void knh_Bytes_expands(CTX, kBytes *ba, size_t newsize)
{
	if(ba->capacity == 0) {
		newsize = k_goodsize(newsize);
		ba->bu.ubuf = (unsigned char*)KNH_ZMALLOC(newsize);
		ba->capacity  = newsize;
	}
	else {
		unsigned char *ubuf = ba->bu.ubuf;
		ba->bu.ubuf = (unsigned char*)KNH_ZMALLOC(newsize);
		memcpy(ba->bu.ubuf, ubuf, ba->capacity);
		KNH_FREE(ubuf, ba->capacity);
		ba->capacity = newsize;
	}
}

static void knh_Bytes_dispose(CTX, kBytes *ba)
{
	if(ba->capacity > 0) {
		//DBG_P("dispose %p %p size=%ld,%ld", ba, ba->bu.ubuf, ba->bu.len, ba->capacity);
		KNH_FREE(ba->bu.ubuf, ba->capacity);
		ba->bu.ubuf = NULL;
		ba->bu.len = 0;
		ba->capacity = 0;
	}
}

static kBytes* new_Bytes(CTX, const char *name, size_t capacity)
{
	kBytes *ba = new_(Bytes, 0);
	if(capacity > 0) {
		knh_Bytes_expands(_ctx, ba, capacity);
	}
	return ba;
}

//static void knh_Bytes_clear(kBytes *ba, size_t pos)
//{
//	if(pos < BA_size(ba)) {
//		bzero(ba->bu.ubuf + pos, BA_size(ba) - pos);
//		BA_size(ba) = pos;
//	}
//}
//
//static void knh_Bytes_ensureSize(CTX, kBytes *ba, size_t len)
//{
//	size_t blen = ba->bu.len + len;
//	if(ba->capacity < blen) {
//		size_t newsize = k_grow(ba->capacity);
//		if(newsize < blen) newsize = k_goodsize(blen);
//		knh_Bytes_expands(_ctx, ba, newsize);
//	}
//}
//
//static const char *knh_Bytes_ensureZero(CTX, kBytes *ba)
//{
//	size_t size = BA_size(ba);
//	size_t capacity = ba->capacity;
//	if(size == capacity) {
//		knh_Bytes_expands(_ctx, ba, k_grow(capacity));
//	}
//	ba->bu.ubuf[BA_size(ba)] = 0;
//	return ba->bu.text;
//}
//
//static void knh_Bytes_putc(CTX, kBytes *ba, int ch)
//{
//	size_t capacity = ba->capacity;
//	if(BA_size(ba) == capacity) {
//		knh_Bytes_expands(_ctx, ba, k_grow(capacity));
//	}
//	ba->bu.ubuf[BA_size(ba)] = ch;
//	BA_size(ba) += 1;
//}
//
//static void knh_Bytes_reduce(kBytes *ba, size_t size)
//{
//	if(BA_size(ba) >= size) {
//		BA_size(ba) -= size;
//		bzero(ba->bu.ubuf + BA_size(ba), size);
//	}
//}

static void knh_Bytes_write(CTX, kBytes *ba, kbytes_t t)
{
	size_t capacity = ba->capacity;
	if(t.len == 0) return;
	if(BA_size(ba) + t.len >= capacity) {
		size_t newsize = k_grow(capacity);
		if(newsize < BA_size(ba) + t.len) newsize = k_goodsize(BA_size(ba) + t.len);
		knh_Bytes_expands(_ctx, ba, newsize);
	}
	memcpy(ba->bu.ubuf + BA_size(ba), t.utext, t.len);
	BA_size(ba) += t.len;
}

//static void knh_Bytes_write2(CTX, kBytes *ba, const char *text, size_t len)
//{
//	size_t capacity = ba->capacity;
//	if(len == 0) return ;
//	if(BA_size(ba) + len >= capacity) {
//		size_t newsize = k_grow(capacity);
//		if(newsize < BA_size(ba) + len) newsize = k_goodsize(BA_size(ba) + len);
//		knh_Bytes_expands(_ctx, ba, newsize);
//	}
//	memcpy(ba->bu.ubuf + BA_size(ba), text, len);
//	BA_size(ba) += len;
//}
//
static inline size_t knh_array_index(CTX, ksfp_t *sfp, kint_t n, size_t size)
{
	size_t idx = (n < 0) ? size + n : n;
	if(unlikely(!(idx < size))) {
		//FIXME
		//THROW_OutOfRange(_ctx, sfp, n, size);
	}
	return idx;
}

/* ------------------------------------------------------------------------ */
//## method Bytes Bytes.new(Int init);

static KMETHOD Bytes_new(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t init = sfp[1].ivalue <= K_FASTMALLOC_SIZE ? K_FASTMALLOC_SIZE : k_goodsize(Int_to(size_t, sfp[1]));
	DBG_ASSERT(ba->capacity == 0);
	ba->bu.ubuf = (unsigned char*)KNH_ZMALLOC(init);
	bzero(ba->bu.ubuf, init);
	ba->capacity = init;
	ba->bu.len = 0;
	RETURN_(ba);
}

/* ------------------------------------------------------------------------ */
//## method @Const Bytes String.encode(StringEncoder enc);

static KMETHOD String_encode(CTX, ksfp_t *sfp _RIX)
{
	kwb_t wb; kwb_init(NULL, &wb);
	kConverter *c = sfp[1].conv;
	//DBG_P("%s, c=%p, c->dpi=%p, c->dpi->enc=****", CLASS__(c->h.cid), c, c->dpi);
	c->dpi->enc(_ctx, c->conv, S_text(sfp[0].s), S_size(sfp[0].s), &wb);
	kBytes *ba = new_Bytes(_ctx, NULL, kwb_size(&wb));
	knh_Bytes_write(_ctx, ba, kwb_tobytes(_ctx, &wb));
	kwb_free(&wb);
	RETURN_(ba);
}

/* ------------------------------------------------------------------------ */
//## method @Const String Bytes.decode(StringDecoder c);

static KMETHOD Bytes_decode(CTX, ksfp_t *sfp _RIX)
{
	kwb_t wb; kwb_init(NULL, &wb);
	kConverter *c = sfp[1].conv;
	c->dpi->dec(_ctx, c->conv, BA_text(sfp[0].ba), BA_size(sfp[0].ba), &wb);
	RETURN_(kwb_newString(_ctx, &wb, 0));
}
/* ------------------------------------------------------------------------ */
//## method Int Bytes.getSize();

static KMETHOD Bytes_getSize(CTX, ksfp_t *sfp _RIX)
{
	//DBG_P("** rix=%ld sfp[K_RIX]=%p", rix, sfp + rix);
	RETURNi_((sfp[0].ba)->bu.len);
}

/* ------------------------------------------------------------------------ */
//## method Int Bytes.get(Int n);

static KMETHOD Bytes_get(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t n2 = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), ba->bu.len);
	RETURNi_(ba->bu.utext[n2]);
}

/* ------------------------------------------------------------------------ */
//## method Int Bytes.set(Int n, Int c);

static KMETHOD Bytes_set(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t n2 = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), ba->bu.len);
	ba->bu.ubuf[n2] = Int_to(char, sfp[2]);
	RETURNi_(ba->bu.utext[n2]);
}

/* ------------------------------------------------------------------------ */
//## method void Bytes.setAll(Int c);

static KMETHOD Bytes_setAll(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t i, n = Int_to(size_t, sfp[1]);
	for(i = 0; i < ba->bu.len; i++) {
		ba->bu.ubuf[i] = n;
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
/* [range] */

static void _rangeUNTIL(CTX, ksfp_t *sfp, size_t size, size_t *s, size_t *e)
{
	*s = sfp[1].ivalue == 0 ? 0 : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), size);
	*e = sfp[2].ivalue == 0 ? (size) : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), size);
}

static void _rangeTO(CTX, ksfp_t *sfp, size_t size, size_t *s, size_t *e)
{
	*s = sfp[1].ivalue == 0 ? 0 : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), size);
	*e = sfp[2].ivalue == 0 ? (size) : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), size) + 1;
}

/* ------------------------------------------------------------------------ */

static kBytes *new_BytesRANGE(CTX, kBytes *ba, size_t s, size_t e)
{
	kBytes *newa = new_(Bytes, 0);
	if(e < s) {
		size_t t = s; s = e; e = t;
	}
	DBG_ASSERT(e <= BA_size(ba));
	if(s < e) {
		size_t newsize = e - s;
		size_t capacity = newsize;
		if(newsize > 0) {
			if(capacity < 256) capacity = 256;
			newa->bu.ubuf = (unsigned char*)KNH_ZMALLOC(capacity);
			bzero(newa->bu.ubuf, capacity);
			memcpy(newa->bu.ubuf, ba->bu.utext + s, newsize);
		}
		else {
			newa->bu.ubuf = NULL;
		}
		newa->capacity = capacity;
		newa->bu.len = newsize;
	}
	return newa;
}

/* ------------------------------------------------------------------------ */
//## method Bytes Bytes.opUNTIL(Int s, Int e);

static KMETHOD Bytes_opUNTIL(CTX, ksfp_t *sfp _RIX)
{
	size_t s, e;
	_rangeUNTIL(_ctx, sfp, (sfp[0].ba)->bu.len, &s, &e);
	RETURN_(new_BytesRANGE(_ctx, sfp[0].ba, s, e));
}

/* ------------------------------------------------------------------------ */
//## method Bytes Bytes.opTO(Int s, Int e);

static KMETHOD Bytes_opTO(CTX, ksfp_t *sfp _RIX)
{
	size_t s, e;
	_rangeTO(_ctx, sfp, (sfp[0].ba)->bu.len, &s, &e);
	RETURN_(new_BytesRANGE(_ctx, sfp[0].ba, s, e));
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t Bytes_initPackage(CTX, struct kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	int FN_n = FN_("n");
	int FN_c = FN_("c");
	int FN_s = FN_("s");
	int FN_e = FN_("e");
	kbytesshare_t *base = (kbytesshare_t*)KNH_ZMALLOC(sizeof(kbytesshare_t));
	base->h.name     = "bytes";
	base->h.setup    = kbytesshare_setup;
	base->h.reftrace = kbytesshare_reftrace;
	base->h.free     = kbytesshare_free;
	ksetModule(MOD_BYTES, &base->h, pline);
	base->cBytes = kaddClassDef(NULL, &BytesDef, pline);
	base->cStringEncoder = kaddClassDef(NULL, &StringEncoderDef, pline);
	base->cStringDecoder = kaddClassDef(NULL, &StringDecoderDef, pline);

	intptr_t methoddata[] = {
		_Public, _F(Bytes_new),     TY_Bytes, TY_Bytes, MN_("new"), 1, TY_Int, MN_("init"),
		_Public, _F(Bytes_getSize), TY_Int,   TY_Bytes, MN_("getSize"), 0,
		_Public, _F(Bytes_get),     TY_Int,   TY_Bytes, MN_("get"), 1, TY_Int, FN_n,
		_Public, _F(Bytes_set),     TY_Int,   TY_Bytes, MN_("set"), 2, TY_Int, FN_n, TY_Int, FN_c,
		_Public, _F(Bytes_setAll),  TY_void,  TY_Bytes, MN_("setAll"), 1, TY_Int, FN_c,
		_Public, _F(Bytes_opUNTIL), TY_Bytes, TY_Bytes, MN_("opUNTIL"), TY_Int, FN_s, TY_Int, FN_e,
		_Public, _F(Bytes_opTO),    TY_Bytes, TY_Bytes, MN_("opTO"),    TY_Int, FN_s, TY_Int, FN_e,
		_Public|_Const, _F(String_encode), TY_Bytes,  TY_String, MN_("encode"), 1, TY_StringEncoder, MN_("enc"),
		_Public|_Const, _F(Bytes_decode),  TY_String, TY_Bytes,  MN_("decode"), 1, TY_StringDecoder, MN_("c"),
		DEND,
	};
	kloadMethodData(NULL, methoddata);
	return true;
}

static kbool_t Bytes_setupPackage(CTX, struct kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t Bytes_initKonohaSpace(CTX, struct kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t Bytes_setupKonohaSpace(CTX, struct kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KPACKDEF* bytes_init(void)
{
	static const KPACKDEF d = {
		KPACKNAME("Bytes", "1.0"),
		.initPackage = Bytes_initPackage,
		.setupPackage = Bytes_setupPackage,
		.initKonohaSpace = Bytes_initKonohaSpace,
		.setupKonohaSpace = Bytes_setupKonohaSpace,
	};
	return &d;
}
#ifdef __cplusplus
}
#endif

