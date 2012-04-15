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

#define _SUB(s0) (S_isASCII(s0) ? SPOL_ASCII|SPOL_POOL : SPOL_POOL)
#define _SUBCHAR(s0) (S_isASCII(s0) ? SPOL_ASCII : 0)
#define _CHARSIZE(len) (len==1 ? SPOL_ASCII : SPOL_UTF8)

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* static inline kbytes_t _S_tobytes(CTX, kString *s) */
/* { */
/* 	kbytes_t b; */
/* 	b.text = S_text(s); */
/* 	b.len  = S_size(s); */
/* 	return b; */
/* } */
/* #define S_tobytes(s) _S_tobytes(_ctx, s) */

/* /\* ------------------------------------------------------------------------ *\/ */

/* static inline kbool_t knh_bytes_endsWith_(kbytes_t v1, kbytes_t v2) */
/* { */
/* 	if(v1.len < v2.len) return 0; */
/* 	const char *p = v1.text + (v1.len-v2.len); */
/* 	return (strncmp(p, v2.text, v2.len) == 0); */
/* } */

/* static inline kbytes_t knh_bytes_first(kbytes_t t, intptr_t loc) */
/* { */
/* 	kbytes_t t2 = {loc, {t.text}}; */
/* 	return t2; */
/* } */

/* static inline kbytes_t knh_bytes_last(kbytes_t t, intptr_t loc) */
/* { */
/* 	kbytes_t t2 = {t.len - loc, {t.text + loc}}; */
/* 	return t2; */
/* } */

/* static kbytes_t bytes_trim(kbytes_t t) */
/* { */
/* 	while(isspace(t.utext[0])) { */
/* 		t.utext++; */
/* 		t.len--; */
/* 	} */
/* 	if(t.len == 0) return t; */
/* 	while(isspace(t.utext[t.len-1])) { */
/* 		t.len--; */
/* 		if(t.len == 0) return t; */
/* 	} */
/* 	return t; */
/* } */

/* static int knh_bytes_strcmp(kbytes_t v1, kbytes_t v2) */
/* { */
/* 	int len, res1, res; */
/* 	if (v1.len == v2.len)     { len = v1.len; res1 =  0;} */
/* 	else if (v1.len < v2.len) { len = v1.len; res1 = -1;} */
/* 	else                      { len = v2.len; res1 =  1;} */
/* 	res = strncmp(v1.text, v2.text, len); */
/* 	res1 = (res == 0) ? res1 : res; */
/* 	return res1; */
/* } */

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

/* static size_t knh_bytes_mlen(kbytes_t v) */
/* { */
/* #ifdef K_USING_UTF8 */
/* 	size_t size = 0; */
/* 	const unsigned char *s = v.utext; */
/* 	const unsigned char *e = s + v.len; */
/* 	while (s < e) { */
/* 		size_t ulen = utf8len(s[0]); */
/* 		size ++; */
/* 		s += ulen; */
/* 	} */
/* 	return size; */
/* #else */
/* 	return v.len; */
/* #endif */
/* } */

/* static kbytes_t knh_bytes_mofflen(kbytes_t v, size_t moff, size_t mlen) */
/* { */
/* #ifdef K_USING_UTF8 */
/* 	size_t i; */
/* 	const unsigned char *s = v.utext; */
/* 	const unsigned char *e = s + v.len; */
/* 	for(i = 0; i < moff; i++) { */
/* 		s += utf8len(s[0]); */
/* 	} */
/* 	v.buf = (char*)s; */
/* 	for(i = 0; i < mlen; i++) { */
/* 		s += utf8len(s[0]); */
/* 	} */
/* 	KNH_ASSERT(s <= e); */
/* 	v.len = (const char*)s - v.text; */
/* 	return v; */
/* #else */
/* 	return knh_bytes_subbytes(m, moff, mlen); /\* if K_ENCODING is not set *\/ */
/* #endif */
/* } */

/* ------------------------------------------------------------------------ */
//## @Const method String String.concat(String rhs);
//## @Const method String String.opADD(String rhs);

static KMETHOD String_opADD(CTX, ksfp_t *sfp _RIX)
{
	//kString *lhs = sfp[0].s, *rhs = sfp[1].s;
	//RETURN_(StringBase_concat(_ctx, lhs, rhs));
}

/* ------------------------------------------------------------------------ */
/* //## @Const method String String.opSUB(String s); */
/* static KMETHOD String_opSUB(CTX, ksfp_t *sfp _RIX) */
/* { */
/* 	kbytes_t base = S_tobytes(sfp[0].s); */
/* 	kbytes_t t = S_tobytes(sfp[1].s); */
/* 	kwb_t wb; */
/* 	kwb_init(&(_ctx->stack->cwb), &wb); */
/* 	char c = t.utext[0]; */
/* 	size_t i; */
/* 	for(i = 0; i < base.len; i++) { */
/* 		if(base.utext[i] == c) { */
/* 			size_t j; */
/* 			for(j = 1; j < t.len; j++) { */
/* 				if(base.utext[i+j] != t.utext[j]) break; */
/* 			} */
/* 			if(j == t.len) { */
/* 				i += t.len - 1; */
/* 				continue; */
/* 			} */
/* 		} */
/* 		kwb_putc(&wb, base.text[i]); */
/* 	} */
/* 	if(base.len == kwb_size(&wb)) { */
/* 		kwb_free(&wb); */
/* 		RETURN_(sfp[0].o); */
/* 	} */
/* 	else { */
/* 		RETURN_(new_kString(kwb_top(&wb, 1), kwb_size(&wb), SPOL_POOL)); */
/* 	} */
/* } */

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.equals(String s);
//## @Const method Boolean String.opEQ(String s);
static KMETHOD String_opEQ(CTX, ksfp_t *sfp _RIX)
{
	if (IS_NULL(sfp[1].o)) RETURNb_(0);
	kString *s0 = sfp[0].s;
	kString *s1 = sfp[1].s;
	size_t len0 = S_size(s0);
	size_t len1 = S_size(s1);
	const char *t0, *t1;
	if (len0 != len1) {
		RETURNb_(0);
	}
	t0 = S_text(s0);
	t1 = S_text(s1);
	RETURNb_(strncmp(t0, t1, len0) == 0);
}

/* ------------------------------------------------------------------------ */
//## method @Const Int String.getSize();

static KMETHOD String_getSize(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(S_size(sfp[0].s));
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.startsWith(String s);

static KMETHOD String_startsWith(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(strncmp(S_text(sfp[0].s), S_text(sfp[1].s), S_size(sfp[1].s)) == 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.endsWith(String s);

static KMETHOD String_endsWith(CTX, ksfp_t *sfp _RIX)
{
	kString *base = (kString *)sfp[0].s;
	kString *arg = (kString *)sfp[1].s;
	int ret;
	if (S_size(base) < S_size(arg)) {
		ret = 0;
	}
	else {
		const char *p = S_text(base) + (S_size(base) - S_size(arg));
		ret = (strncmp(p, S_text(arg), S_size(arg)) == 0);
	}
	RETURNb_(ret);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.indexOf(String s);

static KMETHOD String_indexOf(CTX, ksfp_t *sfp _RIX)
{
	if (IS_NULL(sfp[1].o)) RETURNi_(-1);
	long loc = -1;
	const char *base = S_text(sfp[0].s);
	const char *arg = S_text(sfp[1].s);
	char *p = strstr(base, arg);
	if (p != NULL) {
		loc = p - base;
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.lastIndexOf(String s);

static KMETHOD String_lastIndexOf(CTX, ksfp_t *sfp _RIX)
{
	kString *base = (kString *)sfp[0].s;
	if (IS_NULL(sfp[1].o)) RETURNi_(-1);
	kString *delim = (kString *)sfp[1].s;
	kindex_t loc = S_size(base) - S_size(delim);
	int len = S_size(delim);
	const char *c_base = S_text(base);
	const char *c_delim = S_text(delim);
	if(S_size(delim) == 0) loc--;
	for(; loc >= 0; loc--) {
		if(c_base[loc] == c_delim[0]) {
			if(strncmp(c_base + loc, c_delim, len) == 0) break;
		}
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.trim();

static KMETHOD String_trim(CTX, ksfp_t *sfp _RIX)
{
	const char *s = S_text(sfp[0].s);
	int len = S_size(sfp[0].s);
	kString *ret = NULL;
	while(isspace(s[0])) {
		s++;
		len--;
	}
	if(len != 0) {
		while(isspace(s[len-1])) {
			len--;
			if(len == 0) break;
		}
	}
	if(S_size(sfp[0].s) > len) {
		ret = new_kString(s, len, _SUB(sfp[0].s));
	}
	else {
		ret = (kString *)sfp[0].s;
	}
	RETURN_(ret);
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
	kString *s = (kString *)sfp[0].s;
	size_t n = knh_array_index(_ctx, sfp, sfp[1].ivalue, S_size(sfp[0].s));
	s = new_kString(S_text(s) + n, 1, SPOL_POOL|SPOL_ASCII);
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.substring(Int offset, Int length);

static KMETHOD String_substring(CTX, ksfp_t *sfp _RIX)
{
	kString *base = (kString *)sfp[0].s;
	size_t length = (size_t)sfp[2].ivalue;
	size_t offset = knh_array_index(_ctx, sfp, sfp[1].ivalue, S_size(base));
	kString *ret = NULL;
	if (S_size(base) < offset) {
		ret = new_kString("", 0, 0);
	}
	else {
		const char *new_text = S_text(base) + offset;
		size_t new_size = S_size(base) - offset;
		if (length != 0) {
			if (length < new_size) {
				new_size = length;
			}
		}
		ret = new_kString(new_text, new_size, _SUBCHAR(base));
	}
	RETURN_(ret);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.opUNTIL(Int s, Int e);

/* static KMETHOD String_opUNTIL(CTX, ksfp_t *sfp _RIX) */
/* { */
/* 	kbytes_t t = S_tobytes(sfp[0].s); */
/* 	if(sfp[2].ivalue != 0) { */
/* 		if(!S_isASCII(sfp[0].s)) { */
/* 			size_t mlen = knh_bytes_mlen(t); */
/* 			size_t offset = knh_array_index(_ctx, sfp, sfp[1].ivalue, mlen); */
/* 			size_t length = knh_array_index(_ctx, sfp, sfp[2].ivalue, mlen) - offset; */
/* 			t = knh_bytes_mofflen(t, offset, length); */
/* 			RETURN_(new_kString(t.text, t.len, 0)); */
/* 		} */
/* 		else { */
/* 			size_t offset = (size_t)sfp[1].ivalue; */
/* 			sfp[2].ivalue = knh_array_index(_ctx, sfp, sfp[2].ivalue, (sfp[0].s)->str.len) - offset; */
/* 		} */
/* 	} */
/* 	String_substring(_ctx, sfp K_RIXPARAM); */
/* } */

/* ------------------------------------------------------------------------ */
//## @Const method String String.opTO(Int s, Int e);

/* static KMETHOD String_opTO(CTX, ksfp_t *sfp _RIX) */
/* { */
/* 	kbytes_t t = S_tobytes(sfp[0].s); */
/* 	if(sfp[2].ivalue != 0) { */
/* 		if(!S_isASCII(sfp[0].s)) { */
/* 			size_t mlen = knh_bytes_mlen(t); */
/* 			size_t offset = knh_array_index(_ctx, sfp, sfp[1].ivalue, mlen); */
/* 			size_t length = knh_array_index(_ctx, sfp, sfp[2].ivalue, mlen) - offset + 1; */
/* 			t = knh_bytes_mofflen(t, offset, length); */
/* 			RETURN_(new_kString(t.text, t.len, 0)); */
/* 		} */
/* 		else { */
/* 			size_t offset = (size_t) sfp[1].ivalue; */
/* 			sfp[2].ivalue = knh_array_index(_ctx, sfp, sfp[2].ivalue, (sfp[0].s)->str.len) - offset + 1; */
/* 		} */
/* 	} */
/* 	String_substring(_ctx, sfp K_RIXPARAM); */
/* } */

/* ------------------------------------------------------------------------ */
//## @Const method String String.opUNTIL(Int s, Int e);

static KMETHOD String_toUpper(CTX, ksfp_t *sfp _RIX)
{
	kString *ret = NULL;
	kString *s = (kString *)sfp[0].s;
	size_t size = S_size(s);
	const char *text = S_text(s);
	if (size > 0) {
		char buf[size + 1];
//		memset(buf, 0, size + 1);
		buf[size] = '\0';
		int i = 0;
		while (size > i) {
			buf[i] = toupper(text[i]);
			i++;
		}
		ret = new_kString(buf, size, _SUB(s)); // is it ok using _SUB ?
	}
	else {
		ret = new_kString(text, size, _SUB(s));
	}
	RETURN_(ret);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.opUNTIL(Int s, Int e);

static KMETHOD String_toLower(CTX, ksfp_t *sfp _RIX)
{
	kString *ret = NULL;
	kString *s = (kString *)sfp[0].s;
	size_t size = S_size(s);
	const char *text = S_text(s);
	if (size > 0) {
		char buf[size + 1];
//		memset(buf, 0, size + 1);
		buf[size] = '\0';
		int i = 0;
		while (size > i) {
			buf[i] = tolower(text[i]);
			i++;
		}
		ret = new_kString(buf, size, _SUB(s)); // is it ok using _SUB ?
	}
	else {
		ret = new_kString(text, size, _SUB(s));
	}
	RETURN_(ret);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t String_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	int FN_x = FN_("x");
	int FN_y = FN_("y");
	intptr_t methoddata[] = {
		_Public|_Const, _F(String_opADD),       TY_String,  TY_String, MN_("opADD"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_opADD),       TY_String,  TY_String, MN_("concat"), 1, TY_String, FN_x,
//		_Public|_Const, _F(String_opSUB),       TY_String,  TY_String, MN_("opSUB"), 1, TY_String, FN_x,
//		_Public|_Const, _F(String_opUNTIL),     TY_String,  TY_String, MN_("opUNTIL"), 2, TY_Int, FN_x, TY_Int, FN_y,
//		_Public|_Const, _F(String_opTO),        TY_String,  TY_String, MN_("opTO"),  2, TY_Int, FN_x, TY_Int, FN_y,
		_Public|_Const, _F(String_opEQ),        TY_Boolean, TY_String, MN_("opEQ"),  1, TY_String, FN_x,
		_Public|_Const, _F(String_opEQ),        TY_Boolean, TY_String, MN_("equals"),  1, TY_String, FN_x,
		_Public|_Const, _F(String_opHAS),       TY_Boolean, TY_String, MN_("opHAS"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_trim),        TY_String, TY_String, MN_("trim"), 0,
		_Public|_Const, _F(String_get),         TY_String, TY_String, MN_("get"), 1, TY_Int, FN_x,
		_Public|_Const, _F(String_startsWith),  TY_Boolean, TY_String, MN_("startsWith"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_endsWith),    TY_Boolean, TY_String, MN_("endsWith"),   1, TY_String, FN_x,
		_Public|_Const, _F(String_getSize),     TY_Int, TY_String, MN_("getSize"), 0,
		_Public|_Const, _F(String_indexOf),     TY_Int, TY_String, MN_("indexOf"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_lastIndexOf), TY_Int, TY_String, MN_("lastIndexOf"), 1, TY_String, FN_x,
		_Public|_Const, _F(String_toUpper),     TY_String, TY_String, MN_("toUpper"), 0,
		_Public|_Const, _F(String_toLower),     TY_String, TY_String, MN_("toLower"), 0,
		_Public|_Const, _F(String_substring),   TY_String, TY_String, MN_("substring"), 2, TY_Int, FN_x, TY_Int, FN_y,
		DEND,
	};
	kloadMethodData(NULL, methoddata);
	return true;
}

static kbool_t String_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t String_initKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t String_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* string_init(void)
{
	static const KDEFINE_PACKAGE d = {
		KPACKNAME("String", "1.0"),
		.initPackage = String_initPackage,
		.setupPackage = String_setupPackage,
		.initKonohaSpace = String_initKonohaSpace,
		.setupKonohaSpace = String_setupKonohaSpace,
	};
	return &d;
}
#ifdef __cplusplus
}
#endif

