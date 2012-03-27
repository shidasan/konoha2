/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 *
 * You may choose one of the following two licenses when you use konoha.
 * If you want to use the latter license, please contact us.
 *
 * (1) GNU General Public License 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure License 1.0
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/* ************************************************************************ */

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
#endif/*K_USING_DEBUG*/

#define _ALWAYS SPOL_POOL
#define _NEVER  SPOL_POOL
#define _ASCII  SPOL_ASCII
#define _UTF8   SPOL_UTF8
#define _SUB(s0) (S_isASCII(s0) ? _ASCII|_ALWAYS : _ALWAYS)
#define _SUBCHAR(s0) (S_isASCII(s0) ? _ASCII : 0)
#define _CHARSIZE(len) (len==1 ? _ASCII : _UTF8)

#define knh_strncmp(s1, s2, n)         strncmp((const char*)(s1),(const char*)(s2),n)
#define knh_strncasecmp(s1, s2, n)     strncasecmp((const char*)(s1),(const char*)(s2),n)
#define knh_snprintf(buf, bufsiz, fmt, ...) snprintf(buf, bufsiz, fmt, ##__VA_ARGS__);

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif
/* ------------------------------------------------------------------------ */

static inline kbool_t knh_bytes_equals(kbytes_t v1, kbytes_t v2)
{
	return (v1.len == v2.len && knh_strncmp(v1.text, v2.text, v1.len) == 0);
}

static inline kbool_t knh_bytes_equalsIgnoreCase(kbytes_t v1, kbytes_t v2)
{
	return (v1.len == v2.len && knh_strncasecmp(v1.text, v2.text, v1.len) == 0);
}

#define knh_bytes_startsWith(t, T)   knh_bytes_startsWith_(t, STEXT(T))
static inline int knh_bytes_startsWith_(kbytes_t v1, kbytes_t v2)
{
	if(v1.len < v2.len) return 0;
	return (knh_strncmp(v1.text, v2.text, v2.len) == 0);
}

#define knh_bytes_endsWith(t, T)   knh_bytes_endsWith_(t, STEXT(T))
static inline kbool_t knh_bytes_endsWith_(kbytes_t v1, kbytes_t v2)
{
	if(v1.len < v2.len) return 0;
	const char *p = v1.text + (v1.len-v2.len);
	return (knh_strncmp(p, v2.text, v2.len) == 0);
}

static inline kindex_t knh_bytes_index(kbytes_t v, int ch)
{
	size_t i;
	for(i = 0; i < v.len; i++) {
		if(v.utext[i] == ch) return (kindex_t)i;
	}
	return -1;
}

static inline kindex_t knh_bytes_rindex(kbytes_t v, int ch)
{
	kindex_t i;
	for(i = v.len - 1; i >= 0; i--) {
		if(v.utext[i] == ch) return i;
	}
	return -1;
}

static inline kbytes_t knh_bytes_head(kbytes_t t, int ch)
{
	size_t i;
	for(i = 0; i < t.len; i++) {
		if(t.utext[i] == ch) {
			t.len = i;
			break;
		}
	}
	return t;
}

static inline kbytes_t knh_bytes_rhead(kbytes_t t, int ch)
{
	long i;
	for(i = t.len - 1; i >= 0; i--) {
		if(t.utext[i] == ch) {
			t.len = i;
			break;
		}
	}
	return t;
}

static inline kbytes_t knh_bytes_next(kbytes_t v, int ch)
{
	size_t i;
	for(i = 0; i < v.len; i++) {
		if(v.utext[i] == ch) {
			v.text = v.text + (i+1);
			v.len = v.len - (i+1);
			break;
		}
	}
	return v;
}

static inline kbytes_t knh_bytes_rnext(kbytes_t v, int ch)
{
	long i;
	for(i = v.len - 1; i >= 0; i--) {
		if(v.utext[i] == ch) {
			v.text = v.text + (i+1);
			v.len = v.len - (i+1);
			break;
		}
	}
	return v;
}

static inline kbytes_t knh_bytes_first(kbytes_t t, intptr_t loc)
{
	kbytes_t t2 = {{t.text}, loc};
	return t2;
}

static inline kbytes_t knh_bytes_last(kbytes_t t, intptr_t loc)
{
	kbytes_t t2 = {{t.text + loc}, t.len - loc};
	return t2;
}

static kbytes_t bytes_trim(kbytes_t t)
{
	while(isspace(t.utext[0])) {
		t.utext++;
		t.len--;
	}
	if(t.len == 0) return t;
	while(isspace(t.utext[t.len-1])) {
		t.len--;
		if(t.len == 0) return t;
	}
	return t;
}

static int knh_bytes_strcmp(kbytes_t v1, kbytes_t v2)
{
	int len, res1, res;
	if (v1.len == v2.len)     { len = v1.len; res1 =  0;}
	else if (v1.len < v2.len) { len = v1.len; res1 = -1;}
	else                      { len = v2.len; res1 =  1;}
	res = strncmp(v1.text, v2.text, len);
	res1 = (res == 0) ? res1 : res;
	return res1;
}

static inline size_t knh_array_index(CTX, ksfp_t *sfp, kint_t n, size_t size)
{
	size_t idx = (n < 0) ? size + n : n;
	if(unlikely(!(idx < size))) {
		//FIXME
		//THROW_OutOfRange(ctx, sfp, n, size);
	}
	return idx;
}

/* ------------------------------------------------------------------------ */
/* These utf8 functions were originally written by Shinpei Nakata */

#define utf8_isLead(c)      ((c & 0xC0) != 0x80)
#define utf8_isTrail(c)     ((0x80 <= c) && (c <= 0xBF))
#define utf8_isSingleton(c) (c <= 0x7f)

static const char _utf8len[] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0,
};

#define utf8len(c)    _utf8len[(int)c]

/* ------------------------------------------------------------------------ */

//static kbool_t knh_bytes_checkENCODING(kbytes_t v)
//{
//#ifdef K_USING_UTF8
//	const unsigned char *s = v.utext;
//	const unsigned char *e = s + v.len;
//	while (s < e) {
//		size_t ulen = utf8len(s[0]);
//		switch(ulen) {
//		case 1: s++; break;
//		case 2:
//			if(!utf8_isTrail(s[1])) return 0;
//			s+=2; break;
//		case 3:
//			if(!utf8_isTrail(s[1])) return 0;
//			if(!utf8_isTrail(s[2])) return 0;
//			s+=3; break;
//		case 4:
//			if(!utf8_isTrail(s[1])) return 0;
//			if(!utf8_isTrail(s[2])) return 0;
//			if(!utf8_isTrail(s[3])) return 0;
//			s+=4; break;
//		case 5: case 6: case 0: default:
//			return 0;
//		}
//	}
//	return (s == e);
//#else
//	return 1;
//#endif
//}

static size_t knh_bytes_mlen(kbytes_t v)
{
#ifdef K_USING_UTF8
	size_t size = 0;
	const unsigned char *s = v.utext;
	const unsigned char *e = s + v.len;
	while (s < e) {
		size_t ulen = utf8len(s[0]);
		size ++;
		s += ulen;
	}
	return size;
#else
	return v.len;
#endif
}

static kbytes_t knh_bytes_mofflen(kbytes_t v, size_t moff, size_t mlen)
{
#ifdef K_USING_UTF8
	size_t i;
	const unsigned char *s = v.utext;
	const unsigned char *e = s + v.len;
	for(i = 0; i < moff; i++) {
		s += utf8len(s[0]);
	}
	v.buf = (char*)s;
	for(i = 0; i < mlen; i++) {
		s += utf8len(s[0]);
	}
	KNH_ASSERT(s <= e);
	v.len = (const char*)s - v.text;
	return v;
#else
	return knh_bytes_subbytes(m, moff, mlen); /* if K_ENCODING is not set */
#endif
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.concat(String rhs);
//## @Const method String String.opADD(String rhs);

static KMETHOD String_opADD(CTX, ksfp_t *sfp _RIX)
{
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	kString *lhs = sfp[0].s, *rhs = sfp[1].s;
	kwb_write(&wb, S_text(lhs), S_size(lhs));
	kwb_write(&wb, S_text(rhs), S_size(rhs));
	RETURN_(new_kString(kwb_top(&wb, 1), kwb_size(&wb), SPOL_POOL));
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.opSUB(String s);
static KMETHOD String_opSUB(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t base = S_tobytes(sfp[0].s);
	kbytes_t t = S_tobytes(sfp[1].s);
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	char c = t.utext[0];
	size_t i;
	for(i = 0; i < base.len; i++) {
		if(base.utext[i] == c) {
			size_t j;
			for(j = 1; j < t.len; j++) {
				if(base.utext[i+j] != t.utext[j]) break;
			}
			if(j == t.len) {
				i += t.len - 1;
				continue;
			}
		}
		kwb_putc(&wb, base.text[i]);
	}
	if(base.len == kwb_size(&wb)) {
		kwb_free(&wb);
		RETURN_(sfp[0].o);
	}
	else {
		RETURN_(new_kString(kwb_top(&wb, 1), kwb_size(&wb), SPOL_POOL));
	}
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.equals(String s);
//## @Const method Boolean String.opEQ(String s);
static KMETHOD String_opEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_((S_size(sfp[0].s) == S_size(sfp[1].s) &&
		knh_bytes_strcmp(S_tobytes(sfp[0].s), S_tobytes(sfp[1].s)) == 0));
}

/* ------------------------------------------------------------------------ */
//## method @Const Int String.getSize();

static KMETHOD String_getSize(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[0].s;
	size_t size;
	size = (S_isASCII(s)) ? S_size(s) : knh_bytes_mlen(S_tobytes(s));
	RETURNi_(size);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.startsWith(String s);

static KMETHOD String_startsWith(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_strncmp(S_text(sfp[0].s), S_text(sfp[1].s), S_size(sfp[1].s)) == 0);
	//RETURNb_(knh_bytes_startsWith_(S_tobytes(sfp[0].s), S_tobytes(sfp[1].s))); older
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.endsWith(String s);

static KMETHOD String_endsWith(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_bytes_endsWith_(S_tobytes(sfp[0].s), S_tobytes(sfp[1].s)));
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.indexOf(String s);

static KMETHOD String_indexOf(CTX, ksfp_t *sfp _RIX)
{
	long loc = -1;
	kbytes_t base = S_tobytes(sfp[0].s);
	char *p = strstr(base.text, S_text(sfp[1].s));
	if (p != NULL) {
		loc = p - base.text;
		if(!S_isASCII(sfp[0].s)) {
			base.len = (size_t)loc;
			loc = knh_bytes_mlen(base);
		}
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.lastIndexOf(String s);

static KMETHOD String_lastIndexOf(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t base = S_tobytes(sfp[0].s);
	if (IS_NULL(sfp[1].o)) RETURNi_(-1);
	kbytes_t delim = S_tobytes(sfp[1].s);
	kindex_t loc = base.len - delim.len;
	if(delim.len == 0) loc--;
	for(; loc >= 0; loc--) {
		if(base.utext[loc] == delim.utext[0]) {
			kbytes_t sub = {{base.text + loc}, delim.len};
			if(knh_bytes_strcmp(sub, delim) == 0) break;
		}
	}
	if (loc >= 0 && !S_isASCII(sfp[0].s)) {
		base.len = (size_t)loc;
		loc = knh_bytes_mlen(base);
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.trim();

static KMETHOD String_trim(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[0].s;
	kbytes_t t = S_tobytes(s);
	kbytes_t t2 = bytes_trim(t);
	if(t.len > t2.len) {
		s = new_kString(t2.text, t2.len, _SUB(s));
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.opHAS(String s);

static KMETHOD String_opHAS(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(strstr(S_text(sfp[0].s), S_text(sfp[1].s)) != NULL);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.get(Int n);

static KMETHOD String_get(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t base = S_tobytes(sfp[0].s);
	kString *s;
	if(S_isASCII(sfp[0].s)) {
		size_t n = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), S_size(sfp[0].s));
		s = new_kString(base.text + n, 1, _ALWAYS|_ASCII);
	}
	else {
		size_t off = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), knh_bytes_mlen(base));
		kbytes_t sub = knh_bytes_mofflen(base, off, 1);
		s = new_kString(sub.text, sub.len, _ALWAYS|_CHARSIZE(sub.len));
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.substring(Int offset, Int length);

static KMETHOD String_substring(CTX, ksfp_t *sfp _RIX)
{
	kString *s;
	kbytes_t base = S_tobytes(sfp[0].s);
	kbytes_t t;
	if(S_isASCII(sfp[0].s)) {
		size_t offset = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), base.len);
		t = knh_bytes_last(base, offset);
		if(sfp[2].ivalue != 0) {
			size_t len = (size_t)sfp[2].ivalue;
			if(len < t.len) t = knh_bytes_first(t, len);
		}
	}
	else { // multibytes
		size_t mlen = knh_bytes_mlen(base);
		size_t offset = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), mlen);
		size_t length = sfp[2].ivalue == 0  ? (mlen - offset) : (size_t)sfp[2].ivalue;
		t = knh_bytes_mofflen(base, offset, length);
	}
	s = new_kString(t.text, t.len, _SUBCHAR(sfp[0].s));
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.opUNTIL(Int s, Int e);

static KMETHOD String_opUNTIL(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = S_tobytes(sfp[0].s);
	if(sfp[2].ivalue != 0) {
		if(!S_isASCII(sfp[0].s)) {
			size_t mlen = knh_bytes_mlen(t);
			size_t offset = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), mlen);
			size_t length = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), mlen) - offset;
			t = knh_bytes_mofflen(t, offset, length);
			RETURN_(new_kString(t.text, t.len, 0));
		}
		else {
			size_t offset = Int_to(size_t, sfp[1]);
			sfp[2].ivalue = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), (sfp[0].s)->str.len) - offset;
		}
	}
	String_substring(_ctx, sfp, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.opTO(Int s, Int e);

static KMETHOD String_opTO(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = S_tobytes(sfp[0].s);
	if(sfp[2].ivalue != 0) {
		if(!S_isASCII(sfp[0].s)) {
			size_t mlen = knh_bytes_mlen(t);
			size_t offset = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), mlen);
			size_t length = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), mlen) - offset + 1;
			t = knh_bytes_mofflen(t, offset, length);
			RETURN_(new_kString(t.text, t.len, 0));
		}
		else {
			size_t offset = Int_to(size_t, sfp[1]);
			sfp[2].ivalue = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), (sfp[0].s)->str.len) - offset + 1;
		}
	}
	String_substring(_ctx, sfp, K_RIX);
}



// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t String_initPackage(CTX, struct kLingo *lgo, int argc, const char**args, kline_t pline)
{
	int FN_x = FN_("x");
	int FN_y = FN_("y");
	intptr_t methoddata[] = {
		_Public|_Const, _F(String_opADD), TY_String, TY_String, MN_("opADD"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_opSUB), TY_String, TY_String, MN_("opSUB"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_opEQ),  TY_String, TY_String, MN_("opEQ"),  1, TY_String, FN_x,
		_Public|_Const, _F(String_getSize), TY_Int, TY_String, MN_("getSize"), 0,
		_Public|_Const, _F(String_startsWith),  TY_Boolean, TY_String, MN_("startsWith"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_endsWith),    TY_Boolean, TY_String, MN_("endsWith"),   1, TY_String, FN_x,
		_Public|_Const, _F(String_opHAS),       TY_Boolean, TY_String, MN_("opHAS"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_indexOf),     TY_Int, TY_String, MN_("indexOf"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_lastIndexOf), TY_Int, TY_String, MN_("lastIndexOf"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_trim),  TY_String, TY_String, MN_("trim"), 0,
		_Public|_Const, _F(String_get),   TY_String, TY_String, MN_("get"), 1, TY_Int, FN_x,
		_Public|_Const, _F(String_opUNTIL), TY_String, TY_String, MN_("opUNTIL"), 2, TY_Int, FN_x, TY_Int, FN_y,
		_Public|_Const, _F(String_opTO),    TY_String, TY_String, MN_("opTO"), 2, TY_Int, FN_x, TY_Int, FN_y,
		DEND,
	};
	kaddMethodDef(NULL, methoddata);
	return true;
}

static kbool_t String_setupPackage(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t String_initLingo(CTX,  struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t String_setupLingo(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* String_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("String", "1.0"),
		.initPackage = String_initPackage,
		.setupPackage = String_setupPackage,
		.initLingo = String_initLingo,
		.setupPackage = String_setupLingo,
	};
	return &d;
}
#ifdef __cplusplus
}
#endif

