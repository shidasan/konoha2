#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MASK_LINER    ((S_FLAG_LINER   ) << S_FLAG_MASK_BASE)
#define MASK_NOFREE   ((S_FLAG_NOFREE  ) << S_FLAG_MASK_BASE)
#define MASK_ROPE     ((S_FLAG_ROPE    ) << S_FLAG_MASK_BASE)
#define MASK_INLINE   ((S_FLAG_INLINE  ) << S_FLAG_MASK_BASE)
#define MASK_EXTERNAL ((S_FLAG_EXTERNAL) << S_FLAG_MASK_BASE)

#define S_len(s) ((s)->length)
typedef struct StringBase {
	kObjectHeader h;
	size_t length;
} StringBase;

typedef struct LinerString {
	StringBase base;
	char *text;
} LinerString;

typedef struct ExternalString {
	struct StringBase base;
	char *text;
} ExternalString;

typedef struct RopeString {
	struct StringBase base;
	struct StringBase *left;
	struct StringBase *right;
} RopeString;

typedef struct InlineString {
	struct StringBase base;
	char *text;
	char inline_text[SIZEOF_INLINETEXT];
} InlineString;

static inline uint32_t S_flag(StringBase *s)
{
	uint32_t flag = ((~0U) & (s)->h.magicflag) >> S_FLAG_MASK_BASE;
	assert(flag <= S_FLAG_ROPE);
	return flag;
}

static inline int StringBase_isRope(StringBase *s)
{
	return S_flag(s) == S_FLAG_ROPE;
}

static inline int StringBase_isLiner(StringBase *s)
{
	return S_flag(s) == S_FLAG_LINER;
}

static void StringBase_setFlag(StringBase *s, uint32_t mask)
{
	s->h.magicflag |= mask;
}

static void StringBase_unsetFlag(StringBase *s, uint32_t mask)
{
	s->h.magicflag ^= mask;
}

static StringBase *new_StringBase(CTX, uint32_t mask)
{
	StringBase *s = (StringBase *) new_(String, 0);
	StringBase_setFlag(s, mask);
	return s;
}

static StringBase *InlineString_new(CTX, StringBase *base,
		const char *text, size_t len)
{
	size_t i;
	InlineString *s = (InlineString *) base;
	StringBase_setFlag(base, MASK_INLINE);
	s->base.length = len;
	for (i = 0; i < len; ++i) {
		s->inline_text[i] = text[i];
	}
	s->inline_text[len] = '\0';
	s->text = s->inline_text;
	return base;
}

static StringBase *ExternalString_new(CTX, StringBase *base,
		const char *text, size_t len)
{
	ExternalString *s = (ExternalString *) base;
	StringBase_setFlag(base, MASK_EXTERNAL);
	s->base.length = len;
	s->text = (char *) text;
	return base;
}

static StringBase *LinerString_new(CTX, StringBase *base,
		const char *text, size_t len)
{
	LinerString *s = (LinerString *) base;
	StringBase_setFlag(base, MASK_LINER);
	s->base.length = len;
	s->text = (char *) KNH_MALLOC(len+1);
	memcpy(s->text, text, len);
	s->text[len] = '\0';
	return base;
}

static StringBase *RopeString_new(CTX, StringBase *left,
		StringBase *right, size_t len)
{
	RopeString *s = (RopeString *) new_StringBase(_ctx, MASK_ROPE);
	s->base.length = len;
	s->left  = left;
	s->right = right;
	return (StringBase *) s;
}

static LinerString *RopeString_toLinerString(RopeString *o, char *text, size_t len)
{
	LinerString *s = (LinerString *) o;
	StringBase_unsetFlag((StringBase*)s, MASK_ROPE);
	StringBase_setFlag((StringBase*)s, MASK_LINER);
	s->base.length = len;
	s->text = text;
	return s;
}

static kString *Knew_String(CTX, const char *text, size_t len, int policy)
{
	StringBase *s = (StringBase *) new_StringBase(_ctx, 0);
	if (len < SIZEOF_INLINETEXT)
		return (kString*) InlineString_new(_ctx, s, text, len);
	if(TFLAG_is(int, policy, SPOL_TEXT))
		return (kString*) ExternalString_new(_ctx, s, text, len);
	return (kString*) LinerString_new(_ctx, s, text, len);
}

static void String2_free(CTX, kObject *o)
{
	StringBase *base = (StringBase*) o;
	if (S_isMallocText(base)) {
		KNH_FREE(((LinerString *)base)->text, S_len(base)+1);
	}
}

static void write_text(StringBase *base, char *dest, int size)
{
	RopeString *str;
	size_t len;
	while (1) {
		switch (S_flag(base)) {
			case S_FLAG_LINER:
			case S_FLAG_EXTERNAL:
			case S_FLAG_INLINE:
				memcpy(dest, ((InlineString *) base)->text, size);
				return;
			case S_FLAG_ROPE:
				str = (RopeString *) base;
				len = S_len(str->left);
				write_text(str->left, dest, len);
				base = str->right;
				dest += len;
				break;
		}
	}
}

static LinerString *RopeString_flatten(CTX, RopeString *rope)
{
	size_t length = S_len((StringBase *) rope);
	char *dest = (char *) KNH_MALLOC(length+1);
	size_t len = S_len(rope->left);
	write_text(rope->left,  dest, len);
	write_text(rope->right, dest+len, length - len);
	return RopeString_toLinerString(rope, dest, length);
}

static char *String_getReference(CTX, StringBase *s)
{
	uint32_t flag = S_flag(s);
	switch (flag) {
		case S_FLAG_LINER:
		case S_FLAG_EXTERNAL:
		case S_FLAG_INLINE:
			return ((LinerString*)s)->text;
		case S_FLAG_ROPE:
			return RopeString_flatten(_ctx, (RopeString*)s)->text;
		default:
			/*unreachable*/
			assert(0);
	}
	return NULL;
}

static void StringBase_reftrace(CTX, StringBase *s)
{
	while (1) {
		if (unlikely(!StringBase_isRope(s)))
			break;
		BEGIN_REFTRACE(1);
		RopeString *rope = (RopeString *) s;
		StringBase_reftrace(_ctx, rope->left);
		KREFTRACEv(rope->right);
		s = rope->right;
		END_REFTRACE();
	}
}

static void String2_reftrace(CTX, kObject *o)
{
	StringBase_reftrace(_ctx, (StringBase *) o);
}

static uintptr_t String2_unbox(CTX, kObject *o)
{
	StringBase *s = (StringBase*)o;
	return (uintptr_t) String_getReference(_ctx, s);
}

static StringBase *StringBase_concat(CTX, kString *s0, kString *s1)
{
	StringBase *left = (StringBase*) s0, *right = (StringBase*) s1;
	size_t llen, rlen, length;

	llen = S_len(left);
	if (llen == 0)
		return right;

	rlen = S_len(right);
	if (rlen == 0)
		return left;

	length = llen + rlen;
	if (length < SIZEOF_INLINETEXT) {
		InlineString *ret = (InlineString *) new_StringBase(_ctx, MASK_INLINE);
		char *s0 = String_getReference(_ctx, left);
		char *s1 = String_getReference(_ctx, right);
		ret->base.length = length;
		memcpy(ret->inline_text, s0, llen);
		memcpy(ret->inline_text + llen, s1, rlen);
		ret->inline_text[length] = '\0';
		ret->text = ret->inline_text;
		return (StringBase *) ret;
	}
	return RopeString_new(_ctx, left, right, length);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.concat(String rhs);
//## @Const method String String.opADD(String rhs);

static KMETHOD Rope_opADD(CTX, ksfp_t *sfp _RIX)
{
	kString *lhs = sfp[0].s, *rhs = sfp[1].s;
	RETURN_(StringBase_concat(_ctx, lhs, rhs));
}

/* ------------------------------------------------------------------------ */

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t Rope_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	int FN_x = FN_("x");
	intptr_t methoddata[] = {
		_Public|_Const, _F(Rope_opADD),       TY_String,  TY_String, MN_("opADD"), 1, TY_String, FN_x,
		DEND,
	};
	kloadMethodData(ks, methoddata);
//	{
//		kclass_t *cString = (struct _kclass*) CT_String;
//		cString->unbox = String2_unbox;
//		cString->free  = String2_free;
//		cString->reftrace = String2_reftrace;
//	}
	KSET_CLASSFUNC(CT_String, unbox, String2, pline);
	KSET_CLASSFUNC(CT_String, free, String2, pline);
	KSET_CLASSFUNC(CT_String, reftrace, String2, pline);
	KSET_KLIB2(new_String, pline);
	{
		struct _klib2 *l = (struct _klib2*)_ctx->lib2;
		l->Knew_String = Knew_String;
	}
	return true;
}

static kbool_t Rope_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t Rope_initKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t Rope_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* rope_init(void)
{
	static const KDEFINE_PACKAGE d = {
		KPACKNAME("Rope", "1.0"),
		.initPackage = Rope_initPackage,
		.setupPackage = Rope_setupPackage,
		.initKonohaSpace = Rope_initKonohaSpace,
		.setupKonohaSpace = Rope_setupKonohaSpace,
	};
	assert(sizeof(StringBase) <= sizeof(kString));
	assert(sizeof(ExternalString) <= sizeof(kString));
	assert(sizeof(InlineString) <= sizeof(kString));
	assert(sizeof(LinerString) <= sizeof(kString));
	assert(sizeof(RopeString) <= sizeof(kString));
	return &d;
}

#ifdef __cplusplus
}
#endif
