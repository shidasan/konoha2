/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include "pcre_glue.h"

/* ------------------------------------------------------------------------ */

//#define K_USING_UTF8
/* copy from string_glue.c */
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

static const char* (*_pcre_version)(void);
static void  (*_pcre_free)(void *);
static int  (*_pcre_fullinfo)(const pcre *, const pcre_extra *, int, void *);
static pcre* (*_pcre_compile)(const char *, int, const char **, int *, const unsigned char *);
static int  (*_pcre_exec)(const pcre *, const pcre_extra *, const char*, int, int, int, int *, int);

static kbool_t knh_linkDynamicPCRE(CTX)
{
	void *h = dlopen("libpcre" K_OSDLLEXT, RTLD_LAZY);
	if(h == NULL) return 0;
	_pcre_version = (const char* (*)(void))dlsym(h, "pcre_version");
	_pcre_free = free; // same as pcre_free
	_pcre_fullinfo = (int (*)(const pcre*, const pcre_extra*, int, void*))dlsym(h, "pcre_fullinfo");
	_pcre_compile = (pcre* (*)(const char *, int, const char **, int *, const unsigned char *))dlsym(h, "pcre_compile");
	_pcre_exec = (int  (*)(const pcre *, const pcre_extra *, const char*, int, int, int, int *, int))dlsym(h, "pcre_exec");
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
	PCRE_regex_t *preg = (PCRE_regex_t*) KMALLOC(sizeof(PCRE_regex_t));
	return (kregex_t *) preg;
}

static void pcre_regfree(CTX, kregex_t *reg)
{
	PCRE_regex_t *preg = (PCRE_regex_t*)reg;
	_pcre_free(preg->re);
	KFREE(preg, sizeof(PCRE_regex_t));
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
static void kregexshare_setup(CTX, struct kmodshare_t *def, int newctx)
{
}

static void kregexshare_reftrace(CTX, struct kmodshare_t *baseh)
{
}

static void kregexshare_free(CTX, struct kmodshare_t *baseh)
{
	KFREE(baseh, sizeof(kregexshare_t));
}

///* ------------------------------------------------------------------------ */
//static void knh_Regex_setGlobalOption(kRegex *re, const char *opt)
//{
//	const char *p = opt;
//	while(*p != 0) {
//		if(*p == 'g') {
//			Regex_setGlobalOption(re, 1);
//			break;
//		}
//		p++;
//	}
//}
//

static size_t knh_regex_matched(kregmatch_t* r, size_t maxmatch)
{
	size_t n = 0;
	for (; n < maxmatch && r[n].rm_so != -1; n++) ;
	return n;
}

static void WB_write_regexfmt(CTX, kwb_t *wb, kbytes_t *fmt, const char *base, kregmatch_t *r, size_t matched)
{
	const char *ch = fmt->text;
	const char *eof = ch + fmt->len; // end of fmt
	for (; ch < eof; ch++) {
		if (*ch == '\\') {
			kwb_putc(wb, *ch);
			ch++;
		} else if (*ch == '$' && isdigit(ch[1])) {
			size_t grpidx = (size_t)ch[1] - '0'; // get head of grourp_index
			if (grpidx < matched) {
				ch++;
				while (isdigit(ch[1])) {
					size_t nidx = grpidx * 10 + (ch[1] - '0');
					if (nidx < matched) {
						grpidx = nidx;
						ch++;
						if (ch < eof) {
							continue;
						}
					}
				}
				kregmatch_t *rp = &r[grpidx];
				kwb_write(wb, base + rp->rm_so, rp->rm_eo - rp->rm_so);
				continue; // skip putc
			}
		}
		kwb_putc(wb, *ch);
	}
}

static kString *kwb_newString(CTX, kwb_t *wb, int flg)
{
	return new_kString(kwb_top(wb, flg), kwb_bytesize(wb), SPOL_POOL);
}

#define _ALWAYS SPOL_POOL
#define _NEVER  SPOL_POOL
#define _ASCII  SPOL_ASCII
#define _UTF8   SPOL_UTF8
#define _SUB(s0) (S_isASCII(s0) ? _ASCII|_ALWAYS : _ALWAYS)
#define _SUBCHAR(s0) (S_isASCII(s0) ? _ASCII : 0)
#define _CHARSIZE(len) (len==1 ? _ASCII : _UTF8)

static kArray *kStringToCharArray(CTX, kString *bs, int istrim)
{
	kbytes_t base = {S_size(bs), {S_text(bs)}};
	size_t i, n = base.len;
	kArray *a = new_(Array, n); //TODO new_Array(_ctx, CLASS_String, n);
	if(S_isASCII(bs)) {
		for(i = 0; i < n; i++) {
			if(istrim && isspace(base.utext[i])) continue;
			kArray_add(a, new_kString(base.text+i, 1, _ALWAYS|_ASCII));
		}
	}
	else {
		n = knh_bytes_mlen(base);
		for(i = 0; i < n; i++) {
			if(istrim && isspace(base.utext[i])) continue;
			kbytes_t sub = knh_bytes_mofflen(base, i, 1);
			kArray_add(a, new_kString(sub.text, sub.len, _ALWAYS|((sub.len == 1) ? _ASCII:_UTF8)));
		}
	}
	return a;
}

static void Regex_init(CTX, kObject *o, void *conf)
{
	kRegex *re = (kRegex*)o;
	kString *ptns = conf != NULL ? (kString *)conf : KNULL(String);
	const char *ptn = S_text(ptns);
	const char *opt = "";//IS_NULL(sfp[2].o) ? "" : S_text(sfp[2].s);
	//knh_Regex_setGlobalOption(re, opt);
	KSETv(re->pattern, ptns);
	re->spi = &REGEX_PCRE;
	re->reg = re->spi->regmalloc(_ctx, ptns);
	re->spi->regcomp(_ctx, re->reg, ptn, re->spi->parse_cflags(_ctx, opt));
	re->eflags = re->spi->parse_eflags(_ctx, opt);
}

static void Regex_free(CTX, kObject *o)
{
	kRegex *re = (kRegex*)o;
	re->spi->regfree(_ctx, re->reg);
}

static void Regex_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kwb_printf(wb, "/%s/", S_text(sfp[pos].re->pattern));
}

/* ------------------------------------------------------------------------ */
//## @Const method Regex Regex.new(String pattern, String option);

static KMETHOD Regex_new(CTX, ksfp_t *sfp _RIX)
{
	kRegex *re = (kRegex*)sfp[0].o;
	const char *ptn = S_text(sfp[1].s);
	const char *opt = IS_NULL(sfp[2].o) ? "" : S_text(sfp[2].s);
	//knh_Regex_setGlobalOption(re, opt);
	KSETv(re->pattern, sfp[1].s);
	re->spi = &REGEX_PCRE;
	re->reg = re->spi->regmalloc(_ctx, sfp[1].s);
	re->spi->regcomp(_ctx, re->reg, ptn, re->spi->parse_cflags(_ctx, opt));
	re->eflags = re->spi->parse_eflags(_ctx, opt);
	RETURN_(sfp[0].o);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.search(Regex re);

static KMETHOD String_search(CTX, ksfp_t *sfp _RIX)
{
	kRegex *re = sfp[1].re;
	kindex_t loc = -1;
	if(!IS_NULL(re) && S_size(re->pattern) > 0) {
		kregmatch_t pmatch[2]; // modified by @utrhira
		const char *str = S_text(sfp[0].s);  // necessary
		int res = re->spi->regexec(_ctx, re->reg, str, 1, pmatch, re->eflags);
		if(res == 0) {
			loc = pmatch[0].rm_so;
			if (loc != -1 && !S_isASCII(sfp[0].s)) {
				kbytes_t base = {loc, {str}};
				loc = knh_bytes_mlen(base);
			}
		}
		else {
			//TODO
			//LOG_regex(_ctx, sfp, res, re, str);
		}
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method String[] String.match(Regex re);

static KMETHOD String_match(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	kRegex *re = sfp[1].re;
	kArray *a = NULL;
	if(IS_NOTNULL(re) && S_size(re->pattern) > 0) {
		const char *str = S_text(s0);  // necessary
		const char *base = str;
		const char *eos = base + S_size(s0);
		size_t nmatch = re->spi->regnmatchsize(_ctx, re->reg);
		kregmatch_t *p, pmatch[nmatch+1];
		int i, isGlobalOption = Regex_isGlobalOption(re);
		a = new_(Array, nmatch);/*TODO new_Array(CLASS_String)*/
		BEGIN_LOCAL(lsfp, 1);
		KSETv(lsfp[0].o, a);
		do {
			int res = re->spi->regexec(_ctx, re->reg, str, nmatch, pmatch, re->eflags);
			if(res != 0) {
				// FIXME
				//LOG_regex(_ctx, sfp, res, re, str);
				break;
			}
			for(p = pmatch, i = 0; i < nmatch; p++, i++) {
				if (p->rm_so == -1) break;
				//DBG_P("[%d], rm_so=%d, rm_eo=%d", i, p->rm_so, p->rm_eo);
				kbytes_t sub = {((p->rm_eo) - (p->rm_so)), {str + (p->rm_so)}};
				kArray_add(a, new_kString(sub.text, sub.len, _SUB(s0)));
			}
			if(isGlobalOption) {
				size_t eo = pmatch[0].rm_eo; // shift matched pattern
				str += (eo > 0) ? eo : 1;
				if(!(str < eos)) isGlobalOption = 0; // stop iteration
			}
		} while(isGlobalOption);
		END_LOCAL();
	}
	else {
		a = new_(Array, 0);/*TODO new_Array(CLASS_String)*/
	}
	RETURN_(a);
/*new_(O_ct(sfp[K_RTNIDX].o), 0);  // USE THIS; */
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.replace(Regex re, String s);

static KMETHOD String_replace(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	kRegex *re = sfp[1].re;
	kbytes_t fmt = {S_size(sfp[2].s), {S_text(sfp[2].s)}};
	kString *s = s0;
	if(IS_NOTNULL(re) && S_size(re->pattern) > 0) {
		kwb_t wb;
		kwb_init(&(_ctx->stack->cwb), &wb);
		const char *str = S_text(s0);  // necessary
		const char *base = str;
		const char *eos = str + S_size(s0); // end of str
		kregmatch_t pmatch[KREGEX_MATCHSIZE+1];
		while (str < eos) {
			int res = re->spi->regexec(_ctx, re->reg, str, KREGEX_MATCHSIZE, pmatch, re->eflags);
			if(res != 0) {
				// TODO
				//LOG_regex(_ctx, sfp, res, re, str);
				break;
			}
			size_t len = pmatch[0].rm_eo;
			if (pmatch[0].rm_so > 0) {
				kwb_write(&wb, str, pmatch[0].rm_so);
			}
			size_t matched = knh_regex_matched(pmatch, KREGEX_MATCHSIZE);
			if (len > 0) {
				WB_write_regexfmt(_ctx, &wb, &fmt, base, pmatch, matched);
				str += len;
			} else {
				if (str == base) { // 0-length match at head of string
					WB_write_regexfmt(_ctx, &wb, &fmt, base, pmatch, matched);
				}
				break;
			}
		}
		kwb_write(&wb, str, strlen(str)); // write out remaining string
		s = kwb_newString(_ctx, &wb, 0); // close cwb
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method String[] String.split(Regex re);

static KMETHOD String_split(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	kRegex *re = sfp[1].re;
	kArray *a = NULL;
	if (IS_NOTNULL(re) && S_size(re->pattern) > 0) {
		const char *str = S_text(s0);  // necessary
		const char *eos = str + S_size(s0);
		kregmatch_t pmatch[KREGEX_MATCHSIZE+1];
		if (str < eos) {
			a = new_(Array, 0); // TODO new_Array(_ctx, CLASS_String, 0);
			BEGIN_LOCAL(lsfp, 1);
			KSETv(lsfp[0].o, a);
			while (str <= eos) {
				int res = re->spi->regexec(_ctx, re->reg, str, KREGEX_MATCHSIZE, pmatch, re->eflags);
				if (res == 0) {
					size_t len = pmatch[0].rm_eo;
					if (len > 0) {
						kbytes_t sub = {pmatch[0].rm_so, {str}};
						kArray_add(a, new_kString(sub.text, sub.len, _SUB(s0)));
						str += len;
						continue;
					}
				}
				kArray_add(a, new_kString(str, strlen(str), SPOL_POOL)); // append remaining string to array
				break;
			}
			END_LOCAL();
		} else { // for 0-length patterh
			a = kStringToCharArray(_ctx, new_kString(str, S_size(s0), SPOL_POOL), 0);
		}
	}
	else {
		a = kStringToCharArray(_ctx, s0, 0);
	}
	RETURN_(a);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t pcre_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	knh_linkDynamicPCRE(_ctx);
	kregexshare_t *base = (kregexshare_t*)KCALLOC(sizeof(kregexshare_t), 1);
	base->h.name     = "regex";
	base->h.setup    = kregexshare_setup;
	base->h.reftrace = kregexshare_reftrace;
	base->h.free     = kregexshare_free;
	Konoha_setModule(MOD_REGEX, &base->h, pline);

	KDEFINE_CLASS RegexDef = {
		STRUCTNAME(Regex),
		.cflag = 0,
		.init = Regex_init,
		.free = Regex_free,
		.p    = Regex_p,
	};
	base->cRegex = Konoha_addClassDef(ks->packid, PN_konoha, NULL, &RegexDef, pline);

	kparam_t p = { .ty = TY_String, .fn = FN_("i"), };
	kclass_t *cStrArray = kClassTable_Generics(CT_(TY_Array), 1, &p);
#define TY_StrArray (cStrArray->cid)

	int FN_x = FN_("x");
	int FN_y = FN_("y");
	intptr_t MethodData[] = {
		_Public|_Const, _F(Regex_new),     TY_Regex,  TY_Regex,  MN_("new"), 2, TY_String, FN_x, TY_String, FN_y,
		_Public|_Const, _F(String_search), TY_Int,    TY_String, MN_("search"),  1, TY_Regex, FN_x,
		_Public|_Const, _F(String_replace),TY_String, TY_String, MN_("replace"), 2, TY_Regex, FN_x, TY_String, FN_y,
		_Public|_Const, _F(String_match),  TY_StrArray, TY_String, MN_("match"), 1, TY_Regex, FN_x,
		_Public|_Const, _F(String_split),  TY_StrArray, TY_String, MN_("split"), 1, TY_Regex, FN_x,
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t pcre_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static Ftokenizer parseSLASH;

static int parseREGEX(CTX, struct _kToken *tk, tenv_t *tenv, int tok_start, kMethod *thunk)
{
	USING_SUGAR;
	int ch, prev = '/', pos = tok_start + 1;
	if(tenv->source[pos] == '*' || tenv->source[pos] == '/') {
		return parseSLASH(_ctx, tk, tenv, tok_start, thunk);
	}
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\n') {
			break;
		}
		if(ch == '/' && prev != '\\') {
			if(IS_NOTNULL(tk)) {
				KSETv(tk->text, new_kString(tenv->source + tok_start + 1, (pos-1) - (tok_start+1), 0));
				tk->tt = TK_CODE;
				tk->kw = KW_("$REGEX");
			}
			return pos;
		}
		prev = ch;
	}
	if(IS_NOTNULL(tk)) {
		return parseSLASH(_ctx, tk, tenv, tok_start, thunk);
		//kreportf(ERR_, tk->uline, "must close with /");
	}
	return pos-1;
}

//  /.*/ or /.*/g

//static KMETHOD ParseExpr_Regex(CTX, ksfp_t *sfp _RIX)
//{
//	VAR_ParseExpr(stmt, syn, tls, s, c, e);
//	kToken *tk = tls->toks[c];      // first token
//	kToken *tkOp = tls->toks[c+1];  // second token
//	assert(s == c);
//	// new Regex(, )
//	syn = SYN_(kStmt_ks(stmt), KW_ExprMethodCall);  // switch type checker
//	expr = SUGAR new_TypedMethodCall(_ctx, syn, 2, tk, rexpr);
//	RETURN_(expr);
//}

static KMETHOD ExprTyCheck_Regex(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tk = expr->tk;
	kString *s = tk->text;
	kRegex *r = new_(Regex, s);
	RETURN_(kExpr_setConstValue(expr, TY_Regex, r));
}

#define _SLASH     30//FIXME (from src/sugar/token.h)
static kbool_t pcre_initKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	parseSLASH = ks->fmat[_SLASH];
	SUGAR KonohaSpace_setTokenizer(_ctx, ks, '/', parseREGEX, NULL);
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("$REGEX"), _TERM, ExprTyCheck_(Regex), },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t pcre_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* pcre_init(void)
{
	static const KDEFINE_PACKAGE d = {
		KPACKNAME("regex", "1.0"),
		.initPackage = pcre_initPackage,
		.setupPackage = pcre_setupPackage,
		.initKonohaSpace = pcre_initKonohaSpace,
		.setupKonohaSpace = pcre_setupKonohaSpace,
	};
	return &d;
}
#ifdef __cplusplus
}
#endif
