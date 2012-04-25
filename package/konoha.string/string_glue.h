/****************************************************************************
 * KONOHA2 COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
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


#ifndef STRING_GLUE_H_
#define STRING_GLUE_H_

//#define _SUB(s0) (S_isASCII(s0) ? SPOL_ASCII|SPOL_POOL : SPOL_POOL)
//#define _SUBCHAR(s0) (S_isASCII(s0) ? SPOL_ASCII : 0)
//#define _CHARSIZE(len) (len==1 ? SPOL_ASCII : SPOL_UTF8)

/* ************************************************************************ */

#define USE_STRINGLIB
#include <konoha2/klib.h>

/* ------------------------------------------------------------------------ */

static size_t text_mlen(const char *s_text, size_t s_size)
{
#ifdef K_USING_UTF8
	size_t size = 0;
	const unsigned char *start = (const unsigned char*)s_text;
	const unsigned char *end = start + s_size;
	while (start < end) {
		size_t ulen = utf8len(start[0]);
		size++;
		start += ulen;
	}
	return size;
#else
	return s_size;
#endif
}

//static kString *knh_mofflen(CTX, kString *s, size_t moff, size_t mlen)
//{
//#ifdef K_USING_UTF8
//	if (0 <= (int)moff) {
//		const unsigned char *start = (unsigned char *)S_text(s);
//		const unsigned char *itr = start;
//		size_t i;
//		for(i = 0; i < moff; i++) {
//			itr += utf8len(itr[0]);
//		}
//		start = itr;
//		for(i = 0; i < mlen; i++) {
//			itr += utf8len(itr[0]);
//		}
//		size_t len = itr - start;
//		if (mlen == 1) {
//			s = new_kString((const char *)start, len, SPOL_POOL|_CHARSIZE(len));
//		}
//		else {
//			s = new_kString((const char *)start, len, _SUB(s));
//		}
//	}
//	else {
//		s = new_kString("", 0, 0);
//	}
//	return s;
//#else
//	KTODO(you must set K_USING_UTF8);
////	return knh_bytes_subbytes(m, moff, mlen); /* if K_ENCODING is not set */
//#endif
//}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.equals(String s);
//## @Const method Boolean String.opEQ(String s);

static KMETHOD String_opEQ(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	kString *s1 = sfp[1].s;
	TODO_ASSERT(IS_NOTNULL(s1));
	if(S_size(s0) == S_size(s1)) {
		RETURNb_(strncmp(S_text(s0), S_text(s1), S_size(s0)) == 0);
	}
	RETURNb_(0);
}

/* ------------------------------------------------------------------------ */
//## method @Const Int String.getSize();

static KMETHOD String_getSize(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[0].s;
	size_t size = (S_isASCII(s) ? S_size(s) : text_mlen(S_text(s), S_size(s)));
	RETURNi_(size);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.startsWith(String s);

static KMETHOD String_startsWith(CTX, ksfp_t *sfp _RIX)
{
	// @TEST "A".startsWith("ABC");
	RETURNb_(strncmp(S_text(sfp[0].s), S_text(sfp[1].s), S_size(sfp[1].s)) == 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.endsWith(String s);

static KMETHOD String_endsWith(CTX, ksfp_t *sfp _RIX)
{
	kString *base = sfp[0].s;
	kString *arg =  sfp[1].s;
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
	kString *s0 = sfp[0].s, *s1 = sfp[1].s;
	long loc = -1;
	const char *base = S_text(s0);
	const char *arg =  S_text(s1);
	char *p = strstr(base, arg);
	if (p != NULL) {
		loc = p - base;
		if (!S_isASCII(s0)) {
			loc = text_mlen(base, (size_t)loc);
		}
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.lastIndexOf(String s);

static KMETHOD String_lastIndexOf(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	kString *s1 = sfp[1].s;
	const char *c_base = S_text(s0);
	const char *c_delim = S_text(s1);
	kindex_t loc = S_size(s0) - S_size(s1);
	int len = S_size(s1);
	if(S_size(s1) == 0) loc--;
	for(; loc >= 0; loc--) {
		if(c_base[loc] == c_delim[0]) {
			if(strncmp(c_base + loc, c_delim, len) == 0) break;
		}
	}
	if (loc >= 0 && !S_isASCII(s0)) {
		loc = text_mlen(c_base, (size_t)loc);
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
	kString *s = sfp[0].s;
	if (S_isASCII(s)) {
		size_t n = check_index(_ctx, sfp[1].ivalue, S_size(s), sfp[K_RTNIDX].uline);//		size_t n = check_index(_ctx, sfp[1].ivalue sfp[1].ivalue, S_size(s));
		s = new_kString(S_text(s) + n, 1, SPOL_POOL|SPOL_ASCII);
	}
	else { // FIXME NOW DEFINITELY IMMIDEATELY
		size_t mlen = text_mlen(S_text(s), S_size(s));
		size_t moff = check_index(_ctx, sfp[1].ivalue, mlen, sfp[K_RTNIDX].uline);
//		s = knh_mofflen(_ctx, s, moff, 1); // TODO:
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.substring(Int offset, Int length);

static KMETHOD String_substring(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	size_t offset = (size_t)sfp[1].ivalue;
	size_t length = (size_t)sfp[2].ivalue;
	kString *ret = NULL;
	if (S_isASCII(s0)) {
		offset = check_index(_ctx, offset, S_size(s0), sfp[K_RTNIDX].uline);
		const char *new_text = S_text(s0) + offset;
		size_t new_size = S_size(s0) - offset;
		if (length != 0 && length < new_size) {
			new_size = length;
		}
		ret = new_kString(new_text, new_size, SPOL_ASCII); // FIXME SPOL
	}
	else {
		size_t mlen = text_mlen(S_text(s0), S_size(s0));
		size_t moff = check_index(_ctx, offset,  mlen, sfp[K_RTNIDX].uline);
		length = (mlen - moff);
//		ret = knh_mofflen(_ctx, s0, moff, length); // TODO!!
	}
	RETURN_(ret);
}

/* ------------------------------------------------------------------------ */

static kString* S_toupper(CTX, kString *s0, size_t start)
{
	size_t i, size = S_size(s0);
	kString *s = new_kString(NULL, size, SPOL_sub(s0)|SPOL_NOCOPY);
	memcpy(s->buf, s0->buf, size);
	for(i = start; i < size; i++) {
		int ch = s->buf[i];
		if('a' <= ch && ch <= 'z') {
			s->buf[i] = toupper(ch);
		}
	}
	return s;
}

//## @Const method String String.toLower()
static KMETHOD String_toUpper(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	size_t i, size = S_size(s0);
	for(i = 0; i < size; i++) {
		int ch = s0->buf[i];
		if('a' <= ch && ch <= 'z') {
			RETURN_(S_toupper(_ctx, s0, i));
		}
	}
	RETURN_(s0);
}

/* ------------------------------------------------------------------------ */

//## @Const method String String.toLower()
static KMETHOD String_toLower(CTX, ksfp_t *sfp _RIX)
{
	TODO_ASSERT();
}

// --------------------------------------------------------------------------

//#include "methods.h"

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _Immutable kMethod_Immutable
#define _F(F)   (intptr_t)(F)

static kbool_t String_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	int FN_s = FN_("s");
	int FN_n = FN_("n");
	intptr_t MethodData[] = {
//		_Public|_Const, _F(String_opADD),       TY_String,  TY_String, MN_("opADD"), 1, TY_String, FN_s,
//		_Public|_Const, _F(String_opSUB),       TY_String,  TY_String, MN_("opSUB"), 1, TY_String, FN_s,
//		_Public|_Const, _F(String_opUNTIL),     TY_String,  TY_String, MN_("opUNTIL"), 2, TY_Int, FN_s, TY_Int, FN_n,
//		_Public|_Const, _F(String_opTO),        TY_String,  TY_String, MN_("opTO"),  2, TY_Int, FN_x, TY_Int, FN_y,
		_Public|_Const, _F(String_opEQ),        TY_Boolean, TY_String, MN_("opEQ"),  1, TY_String, FN_s,
		_Public|_Const, _F(String_opEQ),        TY_Boolean, TY_String, MN_("equals"),  1, TY_String, FN_s,
		_Public|_Const, _F(String_opHAS),       TY_Boolean, TY_String, MN_("opHAS"), 1, TY_String, FN_s,
		_Public|_Const, _F(String_trim),        TY_String, TY_String, MN_("trim"), 0,
		_Public|_Const, _F(String_get),         TY_String, TY_String, MN_("get"), 1, TY_Int, FN_n,
		_Public|_Const, _F(String_startsWith),  TY_Boolean, TY_String, MN_("startsWith"), 1, TY_String, FN_s,
		_Public|_Const, _F(String_endsWith),    TY_Boolean, TY_String, MN_("endsWith"),   1, TY_String, FN_s,
		_Public|_Const, _F(String_getSize),     TY_Int, TY_String, MN_("getSize"), 0,
		_Public|_Const, _F(String_indexOf),     TY_Int, TY_String, MN_("indexOf"), 1, TY_String, FN_n,
		_Public|_Const, _F(String_lastIndexOf), TY_Int, TY_String, MN_("lastIndexOf"), 1, TY_String, FN_n,
		_Public|_Const, _F(String_toUpper),     TY_String, TY_String, MN_("toUpper"), 0,
		_Public|_Const, _F(String_toLower),     TY_String, TY_String, MN_("toLower"), 0,
		_Public|_Const, _F(String_substring),   TY_String, TY_String, MN_("substring"), 2, TY_Int, FN_n, TY_Int, FN_n,
		DEND,
	};
	kloadMethodData(NULL, MethodData);
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


#endif /* STRING_GLUE_H_ */

