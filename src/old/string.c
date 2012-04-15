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

#include"commons.h"
#include <errno.h>

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

int knh_bytes_parseint(kbytes_t t, kint_t *value)
{
	kuint_t n = 0, prev = 0, base = 10;
	size_t i = 0;
	if(t.len > 1) {
		if(t.utext[0] == '0') {
			if(t.utext[1] == 'x') {
				base = 16; i = 2;
			}
			else if(t.utext[1] == 'b') {
				base = 2;  i = 2;
			}
			else {
				base = 8;  i = 1;
			}
		}else if(t.utext[0] == '-') {
			base = 10; i = 1;
		}
	}
	for(;i < t.len; i++) {
		int c = t.utext[i];
		if('0' <= c && c <= '9') {
			prev = n;
			n = n * base + (c - '0');
		}else if(base == 16) {
			if('A' <= c && c <= 'F') {
				prev = n;
				n = n * 16 + (10 + c - 'A');
			}else if('a' <= c && c <= 'f') {
				prev = n;
				n = n * 16 + (10 + c - 'a');
			}else {
				break;
			}
		}else if(c == '_') {
			continue;
		}else {
			break;
		}
		if(!(n >= prev)) {
			*value = 0;
			return 0;
		}
	}
	if(t.utext[0] == '-') n = -((kint_t)n);
	*value = n;
	return 1;
}

int knh_bytes_parsefloat(kbytes_t t, kfloat_t *value)
{
#if defined(K_USING_NOFLOAT)
	{
		kint_t v = 0;
		knh_bytes_parseint(t, &v);
		*value = (kfloat_t)v;
	}
#else
	*value = strtod(t.text, NULL);
#endif
	return 1;
}

//kindex_t knh_bytes_indexOf(kbytes_t base, kbytes_t sub)
//{
//	const char *const str0 = base.text;  /* ide version */
//	const char *const str1 = sub.text;
//	kindex_t len  = sub.len;
//	kindex_t loop = base.len - len;
//	kindex_t index = -1;
//	if (loop >= 0) {
//		kindex_t i;
//		const char *s0 = str0, *s1 = str1;
//		const char *const s0end = s0 + loop;
//		while(s0 <= s0end) {
//			for (i = 0; i < len; i++) {
//				if (s0[i] != s1[i]) {
//					goto L_END;
//				}
//			}
//			if (i == len) {
//				return s0 - str0;
//			}
//			L_END:
//			s0++;
//		}
//	}
//	return index;
//}

//kindex_t knh_bytes_indexOf_new(kbytes_t *base, kbytes_t *sub)
//{
//	const char *const str0 = base->text;
//	const char *const str1 = sub->text;
//	kindex_t len  = sub->len;
//	kindex_t loop = base->len - len;
//	kindex_t index = -1;
//	if (loop >= 0) {
//		kindex_t i;
//		const char *s0 = str0, *s1 = str1;
//		const char *const s0end = s0 + loop;
//		while(s0 <= s0end) {
//			for (i = 0; i < len; i++) {
//				if (s0[i] != s1[i]) {
//					goto L_END;
//				}
//			}
//			if (i == len) {
//				return s0 - str0;
//			}
//			L_END:
//			s0++;
//		}
//	}
//	return index;
//}

int knh_bytes_strcmp(kbytes_t v1, kbytes_t v2)
{
	int len, res1, res;
	if (v1.len == v2.len)     { len = v1.len; res1 =  0;}
	else if (v1.len < v2.len) { len = v1.len; res1 = -1;}
	else                      { len = v2.len; res1 =  1;}
	res = knh_strncmp(v1.text, v2.text, len);
	res1 = (res == 0) ? res1 : res;
	return res1;
}


/* ------------------------------------------------------------------------ */
/* These utf8 functions were originally written by Shinpei Nakata */

#define utf8_isLead(c)      ((c & 0xC0) != 0x80)
#define utf8_isTrail(c)     ((0x80 <= c) && (c <= 0xBF))
#define utf8_isSingleton(c) (c <= 0x7f)

static const kchar_t _utf8len[] = {
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

#define utf8len(c)    _utf8len[(kchar_t)c]

int knh_utf8len(int c)
{
	return _utf8len[c];
}

/* ------------------------------------------------------------------------ */

kbool_t knh_bytes_checkENCODING(kbytes_t v)
{
#ifdef K_USING_UTF8
	const unsigned char *s = v.utext;
	const unsigned char *e = s + v.len;
	while (s < e) {
		size_t ulen = utf8len(s[0]);
		switch(ulen) {
		case 1: s++; break;
		case 2:
			if(!utf8_isTrail(s[1])) return 0;
			s+=2; break;
		case 3:
			if(!utf8_isTrail(s[1])) return 0;
			if(!utf8_isTrail(s[2])) return 0;
			s+=3; break;
		case 4:
			if(!utf8_isTrail(s[1])) return 0;
			if(!utf8_isTrail(s[2])) return 0;
			if(!utf8_isTrail(s[3])) return 0;
			s+=4; break;
		case 5: case 6: case 0: default:
			return 0;
		}
	}
	return (s == e);
#else
	return 1;
#endif
}

size_t knh_bytes_mlen(kbytes_t v)
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

kbytes_t knh_bytes_mofflen(kbytes_t v, size_t moff, size_t mlen)
{
#ifdef K_USING_UTF8
	size_t i;
	const unsigned char *s = v.utext;
	const unsigned char *e = s + v.len;
	for(i = 0; i < moff; i++) {
		s += utf8len(s[0]);
	}
	v.ubuf = (kchar_t*)s;
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

kint_t kchar_toucs4(kutext_t *utf8)   /* utf8 -> ucs4 */
{
#if defined(K_USING_UTF8)
	kint_t ucs4 = 0;
	int i= 0;
	kchar_t ret = 0;
	if (!utf8_isSingleton(utf8[0])) {
		kushort_t length_utf8 = utf8len(utf8[i]);
		kchar_t mask = (kchar_t)(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);

		switch(length_utf8){
		case 2:
			/* 110xxxxx 10xxxxxx */
			TODO();
			break;
		case 3:
			/* format 1110xxxx 10xxxxxx 10xxxxxx */
			// first 4 bits
			ucs4 = 0;
			ret = utf8[0] & mask;
			ucs4 = ucs4 | ret;
			// second bit
			ucs4 = ucs4 << 6;
			mask = mask | 1 << 4 | 1 << 5;
			ret = utf8[1] & mask;
			ucs4 = ucs4  | ret;
			// third bit
			ucs4 = ucs4 << 6;
			ret = mask & utf8[2];
			ucs4 = ucs4 | ret;
			break;
		default:
			/* TODO: */
			break;
		}
	} else {
		/* ASCII, let it goes...*/
		ucs4 = utf8[0];
	}
	return ucs4;
#else
	return (kint_t)utf8[0];
#endif
}

/* ------------------------------------------------------------------------ */
/* ucs4 -> utf8 */

char *knh_format_utf8(char *buf, size_t bufsiz, kuint_t ucs4)
{
	/* TODO: here, we assume that BOM bigEndian
	 and only 3 bytes or 1 byte UTF
	 */
	kuint_t mask = 0x0;
	kuint_t byte1 = 0x7F;
	kuint_t byte2 = 0x7FF;
	kuint_t byte3 = 0xFFFF;

	char *ret = buf;
	unsigned char utf8[8];
	if (ucs4 <= byte1) {
		/* 7 bits */
		knh_snprintf(buf, bufsiz, "%c", (int)(0xffff & ucs4));
		ret = buf;
	} else if (ucs4 <= byte2) {
		/* cut last 6 bits */
		TODO();
		/* first 5 bits */
	} else if (ucs4 <= byte3) {
		/* cut last 6 bits */
		mask = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3| 1 << 4 | 1 << 5;
		utf8[2] = (unsigned char)(ucs4 & mask);
		utf8[2] = utf8[2] | 1 << 7;
		/* cut next 6 bits */
		ucs4 = ucs4 >> 6;
		utf8[1] = (unsigned char)(ucs4 & mask);
		utf8[1] = utf8[1] | 1 << 7;
		/* first 4 bits */
		mask = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;
		ucs4 = ucs4 >> 6;
		utf8[0] = (unsigned char)(ucs4 & mask);
		utf8[0] = utf8[0] | 1 << 7 | 1 << 6 | 1 << 5;
		utf8[3] = '\0';
		knh_snprintf(buf, bufsiz, "%s", utf8);
	} else {
		TODO();
	}
	return ret;
}
/* ------------------------------------------------------------------------ */
/* [String] */

static void knh_String_checkASCII(kString *o)
{
	unsigned char ch = 0;
	long len = S_size(o);
	const kchar_t *p = (const kchar_t *) S_text(o);
#ifdef K_USING_FASTESTFASTMODE /* written by ide */
	int len = S_size(o), n = (len + 3) / 4;
	/* Duff's device */
	switch(len%4){
	case 0: do{ ch |= *p++;
	case 3:     ch |= *p++;
	case 2:     ch |= *p++;
	case 1:     ch |= *p++;
	} while(--n>0);
	}
#else
	const kchar_t *const e = p + len;
	while(p < e) {
		int n = len % 8;
		switch(n) {
			case 0: ch |= *p++;
			case 7: ch |= *p++;
			case 6: ch |= *p++;
			case 5: ch |= *p++;
			case 4: ch |= *p++;
			case 3: ch |= *p++;
			case 2: ch |= *p++;
			case 1: ch |= *p++;
		}
		len -= n;
	}
#endif
	String_setASCII(o, (ch < 128));
}

/* ------------------------------------------------------------------------ */

#ifdef K_USING_STRINGPOOL

#define CHECK_CONST(_ctx, V, S, L) \
	if(ct->constPoolMapNULL != NULL) {     \
		V = knh_PtrMap_getS(_ctx, ct->constPoolMapNULL, S, L); \
		if(V != NULL) {           \
			return V;             \
		}                         \
	}                             \

#define SET_CONST(_ctx, V) \
	if(ct->constPoolMapNULL != NULL) {     \
		knh_PtrMap_addS(_ctx, ct->constPoolMapNULL, V); \
	}                             \

#else

#define CHECK_CONST(_ctx, V, S, L)
#define SET_CONST(_ctx, V)

#endif

#ifndef K_USING_STRINGPOOL_MAXSIZ
#define K_USING_STRINGPOOL_MAXSIZ 100000
#endif

static kbool_t checkStringPooling(const char *text, size_t len)
{
	size_t i;
	for(i = 0; i < len; i++) {
		if(text[i] == ' ' || text[i] == '\n' || text[i] == '\t') return 0;
		if(len > 40) return 0;
	}
	return 1;
}


kString* new_String2(CTX, kcid_t cid, const char *text, size_t len, int policy)
{
	kclass_t *ct = ClassTBL(cid);
	int isPooling = 0;
#ifdef K_USING_STRINGPOOL
	if(!TFLAG_is(int, policy, SPOL_POOLNEVER) && ct->constPoolMapNULL != NULL) {
		kString *s = knh_PtrMap_getS(_ctx, ct->constPoolMapNULL, text, len);
		if(s != NULL) return s;
		isPooling = 1;
	}
#endif
	kString *s = (kString*)new_hObject_(_ctx, ct);
	if(TFLAG_is(int, policy, SPOL_TEXT)) {
		s->str.text = text;
		s->str.len = len;
		s->hashCode = 0;
		String_setTextSgm(s, 1);
	}
	else if(len + 1 < sizeof(void*) * 2) {
		s->str.ubuf = (kchar_t*)(&(s->hashCode));
		s->str.len = len;
		knh_memcpy(s->str.ubuf, text, len);
		s->str.ubuf[len] = '\0';
		String_setTextSgm(s, 1);
	}
	else {
		s->str.len = len;
		s->str.ubuf = (kchar_t*)KNH_ZMALLOC(KNH_SIZE(len+1));
		knh_memcpy(s->str.ubuf, text, len);
		s->str.ubuf[len] = '\0';
		s->hashCode = 0;
	}
	if(TFLAG_is(int, policy, SPOL_ASCII)) {
		String_setASCII(s, 1);
	}
	else if(TFLAG_is(int, policy, SPOL_UTF8)) {
		String_setASCII(s, 0);
	}
	else {
		knh_String_checkASCII(s);
	}
	if(isPooling) {
		if(!TFLAG_is(int, policy, SPOL_POOL)) {
			if(!checkStringPooling(s->str.text, s->str.len)) {
				return s; // not pooling
			}
		}
		if(knh_PtrMap_size(ct->constPoolMapNULL) < K_USING_STRINGPOOL_MAXSIZ) {
			knh_PtrMap_addS(_ctx, ct->constPoolMapNULL, s);
			String_setPooled(s, 1);
		}
	}
	return s;
}

KNHAPI2(kString*) new_String(CTX, const char *str)
{
	if(str == NULL) {
		return KNH_TNULL(String);
	}
	else if(str[0] == 0) {
		return TS_EMPTY;
	}
	else {
		return new_kString(str, knh_strlen(str), 0);
	}
}

/* ------------------------------------------------------------------------ */

static knh_conv_t* strconv_open(CTX, const char* to, const char *from)
{
	knh_iconv_t rc = ctx->spi->iconv_openSPI(to, from);
	if(rc == (knh_iconv_t)-1){
		KNH_NTRACE2(_ctx, "iconv_open", K_FAILED, KNH_LDATA(LOG_msg("unknown codec"),
					LOG_s("spi", ctx->spi->iconvspi), LOG_s("from", from), LOG_s("to", to)));
		return NULL;
	}
	return (knh_conv_t*)rc;
}

static kbool_t strconv(CTX, knh_conv_t *iconvp, const char *text, size_t len, kBytes *to)
{
	char buffer[4096], *ibuf = (char*)text;
	size_t ilen = len, rsize = 0;//, ilen_prev = ilen;
	knh_iconv_t cd = (knh_iconv_t)iconvp;
	kbytes_t bbuf = {{(const char*)buffer}, 0};
	while(ilen > 0) {
		char *obuf = buffer;
		size_t olen = sizeof(buffer);
		size_t rc = ctx->spi->iconvSPI(cd, &ibuf, &ilen, &obuf, &olen);
		olen = sizeof(buffer) - olen; rsize += olen;
		if(rc == (size_t)-1 && errno == EILSEQ) {
			KNH_NTRACE2(_ctx, "iconv", K_FAILED, KNH_LDATA(LOG_msg("invalid sequence"), LOG_s("spi", ctx->spi->iconvspi)));
			return 0;
		}
		bbuf.len = olen;
		knh_Bytes_write(_ctx, to, bbuf);
	}
	return 1;
}
static void strconv_close(CTX, knh_conv_t *conv)
{
	ctx->spi->iconv_closeSPI((knh_iconv_t)conv);
}

static knh_ConverterDPI_t SCONV = {
	K_DSPI_CONVTO, "md5",
	strconv_open, // open,
	strconv,  // byte->byte     :conv
	strconv,  // String->byte   :enc
	strconv,   // byte->String   :dec
	NULL,  // String->String :sconv
	strconv_close,
	NULL
};

kStringDecoder* new_StringDecoderNULL(CTX, kbytes_t t)
{
	if(knh_bytes_strcasecmp(t, STEXT(K_ENCODING)) == 0) {
		return KNH_TNULL(StringDecoder);
	}
	else {
		knh_iconv_t id = ctx->spi->iconv_openSPI(K_ENCODING, t.text);
		if(id != (knh_iconv_t)(-1)) {
			kStringDecoder *c = new_(StringDecoder);
			c->conv = (knh_conv_t*)id;
			c->dpi = &SCONV;
			return c;
		}
	}
	return NULL;
}

kStringEncoder* new_StringEncoderNULL(CTX, kbytes_t t)
{
	if(knh_bytes_strcasecmp(t, STEXT(K_ENCODING)) == 0) {
		return KNH_TNULL(StringEncoder);
	}
	else {
		knh_iconv_t id = ctx->spi->iconv_openSPI(K_ENCODING, t.text);
		if(id != (knh_iconv_t)(-1)) {
			kStringEncoder *c = new_(StringEncoder);
			c->conv = (knh_conv_t*)id;
			c->dpi = &SCONV;
			return c;
		}
	}
	return NULL;
}

/* ------------------------------------------------------------------------ */

kString *CWB_newStringDECODE(CTX, CWB_t *cwb, kStringDecoder *c)
{
	BEGIN_LOCAL(_ctx, lsfp, 1);
	LOCAL_NEW(_ctx, lsfp, 0, kString*, s, CWB_newString(_ctx, cwb, 0));
	if(!String_isASCII(s)) {
		kbytes_t t = S_tobytes(s);
		c->dpi->dec(_ctx, c->conv, t.text, t.len, cwb->ba);
		s = CWB_newString(_ctx, cwb, SPOL_UTF8);
		KSETv(lsfp[0].o, K_NULL); //
	}
	END_LOCAL(_ctx, lsfp);
	return s;
}

/* ------------------------------------------------------------------------ */

int knh_bytes_strcasecmp(kbytes_t v1, kbytes_t v2)
{
	if(v1.len < v2.len) {
		int res = knh_strncasecmp(v1.text, v2.text, v1.len);
		return (res == 0) ? -1 : res;
	}
	else if(v1.len > v2.len) {
		int res = knh_strncasecmp(v1.text, v2.text, v2.len);
		return (res == 0) ? 1 : res;
	}
	else {
		return knh_strncasecmp(v1.text, v2.text, v1.len);
	}
}

/* ------------------------------------------------------------------------ */
/* regex */

static kregex_t* strregex_malloc(CTX, kString *pattern)
{
	return (kregex_t*)pattern;
}
static int strregex_parsecflags(CTX, const char *opt)
{
	return 0;
}
static int strregex_parseeflags(CTX, const char *opt)
{
	return 0;
}
static int strregex_nmatchsize(CTX, kregex_t *reg)
{
	return 1;
}
static int strregex_regcomp(CTX, kregex_t *reg, const char *pattern, int cflags)
{
	return 0;
}
static size_t strregex_regerror(int errcode, kregex_t *reg, char *ebuf, size_t ebuf_size)
{
	ebuf[0] = 0;
	return 0;
}
static int strregex_regexec(CTX, kregex_t *reg, const char *str, size_t nmatch, kregmatch_t p[], int flags)
{
	size_t e = 0;
	kString *ptn = (kString*)reg;
	const char *po = strstr(str, S_text(ptn));
	if(po != NULL) {
		p[e].rm_so = po - str;
		p[e].rm_eo = p[e].rm_so + S_size(ptn);
		p[e].rm_name.ubuf = NULL;
		p[e].rm_name.len = 0;
		e++;
	}
	DBG_ASSERT(e < nmatch);
	p[e].rm_so = -1;
	p[e].rm_eo = -1;
	p[e].rm_name.ubuf = NULL;
	p[e].rm_name.len = 0;
	return (po) ? 0 : -1;
}

static void strregex_regfree(CTX, kregex_t *reg) { }

static const knh_RegexSPI_t REGEX_STR = {
	"strregex",
	strregex_malloc, strregex_parsecflags, strregex_parseeflags,
	strregex_regcomp, strregex_nmatchsize, strregex_regexec, strregex_regerror,
	strregex_regfree
};

static const knh_RegexSPI_t* REGEX_DEFAULT = &REGEX_STR;

const knh_RegexSPI_t* knh_getStrRegexSPI(void)
{
	return &REGEX_STR;
}

kbool_t Regex_isSTRREGEX(kRegex *re)
{
	return (re->spi == &REGEX_STR);
}

const knh_RegexSPI_t* knh_getRegexSPI(void)
{
	return REGEX_DEFAULT;
}

/* ------------------------------------------------------------------------ */
/* [pcre] */

#ifdef HAVE_PCRE_H
#include <pcre.h>
#else
//#include "../ext/pcre.h"
struct real_pcre;
typedef struct real_pcre pcre;
typedef void pcre_extra;
/* Request types for pcre_fullinfo() */
#define PCRE_CASELESS           0x00000001  /* Compile */
#define PCRE_MULTILINE          0x00000002  /* Compile */
#define PCRE_DOTALL             0x00000004  /* Compile */
#define PCRE_EXTENDED           0x00000008  /* Compile */
#define PCRE_ANCHORED           0x00000010  /* Compile, exec, DFA exec */
#define PCRE_DOLLAR_ENDONLY     0x00000020  /* Compile */
#define PCRE_EXTRA              0x00000040  /* Compile */
#define PCRE_NOTBOL             0x00000080  /* Exec, DFA exec */
#define PCRE_NOTEOL             0x00000100  /* Exec, DFA exec */
#define PCRE_UNGREEDY           0x00000200  /* Compile */
#define PCRE_NOTEMPTY           0x00000400  /* Exec, DFA exec */
#define PCRE_UTF8               0x00000800  /* Compile */
#define PCRE_NO_AUTO_CAPTURE    0x00001000  /* Compile */
#define PCRE_NO_UTF8_CHECK      0x00002000  /* Compile, exec, DFA exec */
#define PCRE_AUTO_CALLOUT       0x00004000  /* Compile */
#define PCRE_PARTIAL_SOFT       0x00008000  /* Exec, DFA exec */
#define PCRE_PARTIAL            0x00008000  /* Backwards compatible synonym */
#define PCRE_DFA_SHORTEST       0x00010000  /* DFA exec */
#define PCRE_DFA_RESTART        0x00020000  /* DFA exec */
#define PCRE_FIRSTLINE          0x00040000  /* Compile */
#define PCRE_DUPNAMES           0x00080000  /* Compile */
#define PCRE_NEWLINE_CR         0x00100000  /* Compile, exec, DFA exec */
#define PCRE_NEWLINE_LF         0x00200000  /* Compile, exec, DFA exec */
#define PCRE_NEWLINE_CRLF       0x00300000  /* Compile, exec, DFA exec */
#define PCRE_NEWLINE_ANY        0x00400000  /* Compile, exec, DFA exec */
#define PCRE_NEWLINE_ANYCRLF    0x00500000  /* Compile, exec, DFA exec */
#define PCRE_BSR_ANYCRLF        0x00800000  /* Compile, exec, DFA exec */
#define PCRE_BSR_UNICODE        0x01000000  /* Compile, exec, DFA exec */
#define PCRE_JAVASCRIPT_COMPAT  0x02000000  /* Compile */
#define PCRE_NO_START_OPTIMIZE  0x04000000  /* Compile, exec, DFA exec */
#define PCRE_NO_START_OPTIMISE  0x04000000  /* Synonym */
#define PCRE_PARTIAL_HARD       0x08000000  /* Exec, DFA exec */
#define PCRE_NOTEMPTY_ATSTART   0x10000000  /* Exec, DFA exec */
#define PCRE_UCP                0x20000000  /* Compile */


#define PCRE_INFO_OPTIONS            0
#define PCRE_INFO_SIZE               1
#define PCRE_INFO_CAPTURECOUNT       2
#define PCRE_INFO_BACKREFMAX         3
#define PCRE_INFO_FIRSTBYTE          4
#define PCRE_INFO_FIRSTCHAR          4  /* For backwards compatibility */
#define PCRE_INFO_FIRSTTABLE         5
#define PCRE_INFO_LASTLITERAL        6
#define PCRE_INFO_NAMEENTRYSIZE      7
#define PCRE_INFO_NAMECOUNT          8
#define PCRE_INFO_NAMETABLE          9
#define PCRE_INFO_STUDYSIZE         10
#define PCRE_INFO_DEFAULT_TABLES    11
#define PCRE_INFO_OKPARTIAL         12
#define PCRE_INFO_JCHANGED          13
#define PCRE_INFO_HASCRORLF         14
#define PCRE_INFO_MINLENGTH         15

/* Request types for pcre_config(). Do not re-arrange, in order to remain
compatible. */

#define PCRE_CONFIG_UTF8                    0
#define PCRE_CONFIG_NEWLINE                 1
#define PCRE_CONFIG_LINK_SIZE               2
#define PCRE_CONFIG_POSIX_MALLOC_THRESHOLD  3
#define PCRE_CONFIG_MATCH_LIMIT             4
#define PCRE_CONFIG_STACKRECURSE            5
#define PCRE_CONFIG_UNICODE_PROPERTIES      6
#define PCRE_CONFIG_MATCH_LIMIT_RECURSION   7
#define PCRE_CONFIG_BSR                     8


#endif

static const char* (*_pcre_version)(void);
static void  (*_pcre_free)(void *);
static int  (*_pcre_fullinfo)(const pcre *, const pcre_extra *, int, void *);
static pcre* (*_pcre_compile)(const char *, int, const char **, int *, const unsigned char *);
static int  (*_pcre_exec)(const pcre *, const pcre_extra *, const char*, int, int, int, int *, int);

static kbool_t knh_linkDynamicPCRE(CTX)
{
	void *h = knh_dlopen(_ctx, "libpcre" K_OSDLLEXT);
	if(h == NULL) return 0;
	_pcre_version = (const char* (*)(void))knh_dlsym(_ctx, h, "pcre_version", NULL, 0/*isTest*/);
	_pcre_free = free; // same as pcre_free
	_pcre_fullinfo = (int (*)(const pcre*, const pcre_extra*, int, void*))knh_dlsym(_ctx, h, "pcre_fullinfo", NULL, 0/*isTest*/);
	_pcre_compile = (pcre* (*)(const char *, int, const char **, int *, const unsigned char *))knh_dlsym(_ctx, h, "pcre_compile", NULL, 0/*isTest*/);
	_pcre_exec = (int  (*)(const pcre *, const pcre_extra *, const char*, int, int, int, int *, int))knh_dlsym(_ctx, h, "pcre_exec", NULL, 0/*isTest*/);
	if(_pcre_free == NULL || _pcre_fullinfo == NULL || _pcre_compile == NULL || _pcre_exec == NULL) return 0;
	return 1;
}

/* This part was implemented by Yutaro Hiraoka */

typedef struct {
	pcre *re;
	const char *err;
	int erroffset;
} PCRE_regex_t;


static kregex_t* pcre_regmalloc(CTX, kString* s)
{
	PCRE_regex_t *preg = (PCRE_regex_t*) KNH_MALLOC(_ctx,sizeof(PCRE_regex_t));
	return (kregex_t *) preg;
}

static void pcre_regfree(CTX, kregex_t *reg)
{
	PCRE_regex_t *preg = (PCRE_regex_t*)reg;
	_pcre_free(preg->re);
	KNH_FREE(_ctx, preg, sizeof(PCRE_regex_t));
}

static int pcre_nmatchsize(CTX, kregex_t *reg)
{
	PCRE_regex_t *preg = (PCRE_regex_t*)reg;
	int capsize = 0;
	if (_pcre_fullinfo(preg->re, NULL, PCRE_INFO_CAPTURECOUNT, &capsize) != 0) {
		return KREGEX_MATCHSIZE;
	}
	return capsize + 1;
}

static int pcre_parsecflags(CTX, const char *option)
{
	int i, cflags = 0;
	int optlen = strlen(option);
	for (i = 0; i < optlen; i++) {
		switch(option[i]) {
		case 'i': // caseless
			cflags |= PCRE_CASELESS;
			break;
		case 'm': // multiline
			cflags |= PCRE_MULTILINE;
			break;
		case 's': // dotall
			cflags |= PCRE_DOTALL;
			break;
		case 'x': //extended
			cflags |= PCRE_EXTENDED;
			break;
		case 'u': //utf
			cflags |= PCRE_UTF8;
			break;
		default: break;
		}
	}
	return cflags;
}

static int pcre_parseeflags(CTX, const char *option)
{
	int i, eflags = 0;
	int optlen = strlen(option);
	for (i = 0; i < optlen; i++) {
		switch(option[i]){
		default: break;
		}
	}
	return eflags;
}

static size_t pcre_regerror(int res, kregex_t *reg, char *ebuf, size_t ebufsize)
{
	PCRE_regex_t *pcre = (PCRE_regex_t*)reg;
	snprintf(ebuf, ebufsize, "[%d]: %s", pcre->erroffset, pcre->err);
	return (pcre->err != NULL) ? strlen(pcre->err) : 0;
}

static int pcre_regcomp(CTX, kregex_t *reg, const char *pattern, int cflags)
{
	PCRE_regex_t* preg = (PCRE_regex_t*)reg;
	preg->re = _pcre_compile(pattern, cflags, &preg->err, &preg->erroffset, NULL);
	return (preg->re != NULL) ? 0 : -1;
}

static int pcre_regexec(CTX, kregex_t *reg, const char *str, size_t nmatch, kregmatch_t p[], int eflags)
{
	PCRE_regex_t *preg = (PCRE_regex_t*)reg;
	int res, nm_count, nvector[nmatch*3];
	nvector[0] = 0;
	size_t idx, matched = nmatch;
	if (strlen(str) == 0) return -1;
	if ((res = _pcre_exec(preg->re, NULL, str, strlen(str), 0, eflags, nvector, nmatch*3)) >= 0) {
		matched = (res > 0 && res < nmatch) ? res : nmatch;
		res = 0;
		for (idx = 0; idx < matched; idx++) {
			p[idx].rm_so = nvector[2*idx];
			p[idx].rm_eo = nvector[2*idx+1];
		}
		p[idx].rm_so = -1;
		nm_count = 0;
		_pcre_fullinfo(preg->re, NULL, PCRE_INFO_NAMECOUNT, &nm_count);
		if (nm_count > 0) {
			unsigned char *nm_table;
			int nm_entry_size = 0;
			_pcre_fullinfo(preg->re, NULL, PCRE_INFO_NAMETABLE, &nm_table);
			_pcre_fullinfo(preg->re, NULL, PCRE_INFO_NAMEENTRYSIZE, &nm_entry_size);
			unsigned char *tbl_ptr = nm_table;
			for (idx = 0; idx < nm_count; idx++) {
				int n_idx = (tbl_ptr[0] << 8) | tbl_ptr[1];
				unsigned char *n_name = tbl_ptr + 2;
				p[n_idx].rm_name.utext = n_name;
				p[n_idx].rm_name.len = strlen((char*)n_name);
				tbl_ptr += nm_entry_size;
			}
		}
	}
	return res;
}

static const knh_RegexSPI_t REGEX_PCRE = {
	"pcre",
	pcre_regmalloc,
	pcre_parsecflags,
	pcre_parseeflags,
	pcre_regcomp,
	pcre_nmatchsize,
	pcre_regexec,
	pcre_regerror,
	pcre_regfree
};

/* ------------------------------------------------------------------------ */
/* [re2] */

static kbool_t knh_linkDynamicRe2(CTX)
{
#if defined(__cplusplus) && defined(K_USING_RE2)
	void *h = knh_dlopen(_ctx, "libre2" K_OSDLLEXT);
	if(h != NULL) return 1;
#endif
	return 0;
}

#if defined(__cplusplus) && defined(K_USING_RE2)
} /* cancel extern "C" */

#include <re2/re2.h>
#include <vector>

extern "C" {

#define RE2_CASELESS           0x00000001
#define RE2_MULTILINE          0x00000002
#define RE2_DOTALL             0x00000004

typedef struct {
	re2::RE2 *r;
} RE2_regex_t;

static kregex_t* re2_regex_malloc(CTX, kString* s)
{
	RE2_regex_t *reg = (RE2_regex_t*)KNH_ZMALLOC(sizeof(RE2_regex_t));
	reg->r = NULL;
	return (kregex_t*)reg;
}

static void re2_regex_regfree(CTX, kregex_t *reg)
{
	RE2_regex_t *re = (RE2_regex_t*)reg;
	if (re->r != NULL) {
		re2::RE2 *r = static_cast<re2::RE2*>(re->r);
		delete r;
	}
	KNH_FREE(_ctx, reg, sizeof(RE2_regex_t));
}

static size_t re2_regex_regerror(int res, kregex_t *reg, char *ebuf, size_t ebufsize)
{
	re2::RE2 *r = static_cast<re2::RE2*>(((RE2_regex_t*)reg)->r);
	const char *err = (*r).error().c_str();
	snprintf(ebuf, ebufsize, "%s", err);
	return (err != NULL) ? strlen(err) : 0;
}

static int re2_regex_parsecflags(CTX, const char *option)
{
	int i, cflags = 0;
	int optlen = strlen(option);
	for (i = 0; i < optlen; i++) {
		switch(option[i]) {
		case 'i': // caseless
				cflags |= RE2_CASELESS;
				break;
		case 'm': // multiline
			cflags |= RE2_MULTILINE;
			break;
		case 's': // dotall
			cflags |= RE2_DOTALL;
			break;
		default: break;
		}
	}
	return cflags;
}

static int re2_regex_regcomp(CTX, kregex_t *reg, const char *pattern, int cflags)
{
	RE2_regex_t* re = (RE2_regex_t*)reg;
	re2::RE2::Options opt;
	opt.Copy(re2::RE2::Quiet);
	opt.set_perl_classes(true);
	if (cflags != 0) {
		if (cflags & RE2_CASELESS) {
			opt.set_case_sensitive(false);
		}
		if (cflags & RE2_MULTILINE) {
			opt.set_never_nl(true);
		}
		if (cflags & RE2_DOTALL) {
			opt.set_one_line(true);
		}
	}
	re->r = new re2::RE2(pattern, opt);
	return ((*(re->r)).ok()) ? 0 : 1;
}

static int re2_regex_nmatchsize(CTX, kregex_t *reg)
{
	re2::RE2 *r = static_cast<re2::RE2*>(((RE2_regex_t*)reg)->r);
	return 1 + (*r).NumberOfCaptfileidngGroups(); // patern + groups
}

static int re2_regex_parseeflags(CTX, const char *option)
{
	int i, eflags = 0;
	int optlen = strlen(option);
	for (i = 0; i < optlen; i++) {
		switch(option[i]){
		default: break;
		}
	}
	return eflags;
}

static int re2_regex_regexec(CTX, kregex_t *reg, const char *str, size_t nmatch, kregmatch_t p[], int eflags)
{
	re2::RE2 *r = static_cast<re2::RE2*>(((RE2_regex_t*)reg)->r);
	re2::StringPiece base(str);
	re2::StringPiece s[nmatch], *sp = s;
	size_t remain = nmatch;
	p[0].rm_so = -1;
	if ((*r).Match(base, 0, re2::RE2::UNANCHORED, sp, nmatch)) {
		size_t grpcount = (*r).NumberOfCaptfileidngGroups();
		std::vector<std::string> names(grpcount+1);
		if (grpcount > 0) {
			std::map<std::string, int> m = (*r).NamedCaptfileidngGroups();
			std::map<std::string, int>::iterator it, m_end = m.end();
			for (it = m.begin(); it != m_end; it++) {
				names[(*it).second] = (*it).first;
			}
		}
		size_t i, spoffset = sp->data() - base.data();
		p[0].rm_so = spoffset;
		p[0].rm_eo = spoffset + sp->length();
		p[0].rm_name.len = 0; // clear name
		for (i = 1, remain--, sp++; (!sp->empty() && remain > 0); i++, remain--, sp++) {
			spoffset = sp->data() - base.data();
			p[i].rm_so = spoffset;
			p[i].rm_eo = spoffset + sp->length();
			p[i].rm_name.len = 0; // clear name
			std::string *name = &names[i];
			if (name != NULL && !name->empty()) {
				p[i].rm_name = B(name->c_str());
			}
		}
		if (i < nmatch) p[i].rm_so = -1;
	}
	return (remain < nmatch && r->ok()) ? 0 : 1;
}

#endif /* __cplusplus */

static knh_RegexSPI_t REGEX_RE2 = {
	"re2",
#if defined(__cplusplus) && defined(K_USING_RE2)
	re2_regex_malloc, re2_regex_parsecflags, re2_regex_parseeflags, re2_regex_regcomp,
	re2_regex_nmatchsize, re2_regex_regexec, re2_regex_regerror, re2_regex_regfree
#else
	// dummy (never used)
	strregex_malloc, strregex_parsecflags, strregex_parseeflags, strregex_regcomp,
	strregex_nmatchsize, strregex_regexec, strregex_regerror, strregex_regfree
#endif
};

/* ------------------------------------------------------------------------ */
/* [onig] */

// from oniguruma.h
#ifdef HAVE_ONIG_H
#include "oniguruma.h"
#else
#include "../ext/oniguruma.h"
#endif

static int (*_onig_error_code_to_str)(OnigUChar*, int, ...);
static int (*_onig_new)(OnigRegex*, OnigUChar*, OnigUChar*, OnigOptionType, OnigEncoding, OnigSyntaxType*, OnigErrorInfo*);
static int (*_onig_number_of_captures)(OnigRegex);
static OnigRegion* (*_onig_region_new)(void);
static int (*_onig_search)(OnigRegex, OnigUChar*, OnigUChar*, OnigUChar*, OnigUChar*, OnigRegion*, OnigOptionType);
static int (*_onig_foreach_name)(OnigRegex, int (*)(const OnigUChar*, const OnigUChar*, int, int*, OnigRegex, void*), void*);
static void (*_onig_region_free)(OnigRegion*, int);
static void (*_onig_free)(OnigRegex);
static OnigEncoding encutf8;
static OnigSyntaxType** defaultsyntax;

static kbool_t knh_linkDynamicOnig(CTX)
{
	void *h = knh_dlopen(_ctx, "libonig" K_OSDLLEXT);
	if(h == NULL) return 0;
	_onig_error_code_to_str = (int (*)(OnigUChar*, int, ...))knh_dlsym(_ctx, h, "onig_error_code_to_str", NULL, 0/*isTest*/);
	_onig_new = (int (*)(OnigRegex*, OnigUChar*, OnigUChar*, OnigOptionType, OnigEncoding, OnigSyntaxType*, OnigErrorInfo*))knh_dlsym(_ctx, h, "onig_new", NULL, 0/*isTest*/);
	_onig_number_of_captures = (int (*)(OnigRegex))knh_dlsym(_ctx, h, "onig_number_of_captures", NULL, 0/*isTest*/);
	_onig_region_new = (OnigRegion* (*)(void))knh_dlsym(_ctx, h, "onig_region_new", NULL, 0/*isTest*/);
	_onig_search = (int (*)(OnigRegex, OnigUChar*, OnigUChar*, OnigUChar*, OnigUChar*, OnigRegion*, OnigOptionType))knh_dlsym(_ctx, h, "onig_search", NULL, 0/*isTest*/);
	_onig_foreach_name = (int (*)(OnigRegex, int (*)(const OnigUChar*, const OnigUChar*, int, int*, OnigRegex, void*), void*))knh_dlsym(_ctx, h, "onig_foreach_name", NULL, 0/*isTest*/);
	_onig_region_free = (void (*)(OnigRegion*, int))knh_dlsym(_ctx, h, "onig_region_free", NULL, 0/*isTest*/);
	_onig_free = (void (*)(OnigRegex))knh_dlsym(_ctx, h, "onig_free", NULL, 0/*isTest*/);

	encutf8 = (OnigEncoding)knh_dlsym(_ctx, h, "OnigEncodingUTF8", NULL, 0/*isTest*/);
	defaultsyntax = (OnigSyntaxType**)knh_dlsym(_ctx, h, "OnigDefaultSyntax", NULL, 0/*isTest*/);
	if(_onig_error_code_to_str == NULL || _onig_new == NULL || 
			_onig_number_of_captures == NULL || _onig_region_new == NULL || 
			_onig_search == NULL || _onig_foreach_name == NULL || 
			_onig_region_free == NULL || _onig_free == NULL || encutf8 == NULL ||
			defaultsyntax == NULL)
		return 0;
	return 1;
}

typedef struct {
	OnigRegex reg;
	OnigErrorInfo *einfo;
} ONIG_regex_t;

static kregex_t* onig_regex_malloc(CTX, kString *s)
{
	ONIG_regex_t *r = (ONIG_regex_t*)KNH_ZMALLOC(sizeof(ONIG_regex_t));
	r->einfo = NULL;
	r->reg = NULL;
	return (kregex_t*) r;
}

static int onig_regex_parse_cflags(CTX, const char *option)
{
	OnigOptionType cflags = ONIG_OPTION_DEFAULT;
	int i, optlen = strlen(option);
	for (i = 0; i < optlen; i++) {
		switch(option[i]) {
		case 'i':
			ONIG_OPTION_ON(cflags, ONIG_OPTION_IGNORECASE);
			break;
		case 'm':
			ONIG_OPTION_ON(cflags, ONIG_OPTION_MULTILINE);
			break;
		case 'x':
			ONIG_OPTION_ON(cflags, ONIG_OPTION_EXTEND);
			break;
		default: break;
		}
	}
	return (int)cflags;
}

static int onig_regex_parse_eflags(CTX, const char *option)
{
	OnigOptionType eflags = ONIG_OPTION_NONE;
	int i, optlen = strlen(option);
	for (i = 0; i < optlen; i++) {
		switch(option[i]) {
		default: break;
		}
	}
	return eflags;
}

static size_t onig_regex_regerror(int res, kregex_t *reg, char* ebuf, size_t ebuf_size)
{
	ONIG_regex_t *oreg = (ONIG_regex_t*)reg;
	return _onig_error_code_to_str((OnigUChar*)ebuf, res, oreg->einfo);
}

static int onig_regex_regcomp(CTX, kregex_t *reg, const char *pattern, int cflag)
{
	ONIG_regex_t *oreg = (ONIG_regex_t*)reg;
	OnigUChar* upatt = (OnigUChar*) pattern;
	OnigUChar* end = upatt + strlen(pattern);
	int cmp = _onig_new(&(oreg->reg), upatt, end, cflag, encutf8, *defaultsyntax, oreg->einfo);
	return (cmp == ONIG_NORMAL) ? 0 : 1;
}

static int onig_regex_nmatchsize(CTX, kregex_t *reg)
{
	ONIG_regex_t *oreg = (ONIG_regex_t*)reg;
	int cap = _onig_number_of_captures(oreg->reg);
	return 1 + cap; // pattern & groups
}

static int knh_regex_onig_setNames(const OnigUChar* name, const OnigUChar* name_end, int num_of_group, int* num_of_group_list, OnigRegex reg, void *arg)
{
	kregmatch_t *p = (kregmatch_t*)arg;
	int i, len = name_end - name;
	for (i = 0; i < num_of_group; i++) {
		int j = num_of_group_list[i];
		p[j].rm_name.buf = (char*)name;
		p[j].rm_name.len = len;
	}
	return 0;
}

static int onig_regex_regexec(CTX, kregex_t *reg, const char *str, size_t nmatch, kregmatch_t p[], int eflag)
{
	ONIG_regex_t *oreg = (ONIG_regex_t*)reg;
	OnigUChar *head = (OnigUChar*)str;
	OnigUChar *end = head + strlen(str);
	OnigRegion* region = _onig_region_new();
	int res = 0;
	p[0].rm_so = -1;
	if ((res = _onig_search(oreg->reg, head, end, head, end, region, eflag)) >= 0) {
		size_t i, matched = region->num_regs;
		for (i = 0; i < nmatch && i < matched; i++) {
			p[i].rm_so = region->beg[i];
			p[i].rm_eo = region->end[i];
		}
		if (i < nmatch) p[i].rm_so = -1;
		_onig_foreach_name(oreg->reg, knh_regex_onig_setNames, p);
	}
	_onig_region_free(region, 1); /* 1:free self, 0:free contents only */
	return (res >= 0) ? 0 : res; /* >=0: not error(matched bytes), <0:error */
}

static void onig_regex_regfree(CTX, kregex_t *reg)
{
	ONIG_regex_t *oreg = (ONIG_regex_t*)reg;
	OnigRegex r = oreg->reg;
	_onig_free(r);
	KNH_FREE(_ctx, oreg, sizeof(ONIG_regex_t));
}

static const knh_RegexSPI_t REGEX_ONIG = {
	"oniguruma",
	onig_regex_malloc,
	onig_regex_parse_cflags,
	onig_regex_parse_eflags,
	onig_regex_regcomp,
	onig_regex_nmatchsize,
	onig_regex_regexec,
	onig_regex_regerror,
	onig_regex_regfree
};

/* ------------------------------------------------------------------------ */
/* [regex] */

void knh_linkDynamicRegex(CTX)
{
	if(REGEX_DEFAULT == &REGEX_STR) {
		if(knh_linkDynamicRe2(_ctx)) {
			REGEX_DEFAULT = &REGEX_RE2;
			return;
		}
		if(knh_linkDynamicPCRE(_ctx)) {
			REGEX_DEFAULT = &REGEX_PCRE;
			return;
		}
		if(knh_linkDynamicOnig(_ctx)) {
			REGEX_DEFAULT = &REGEX_ONIG;
			return;
		}
	}
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

