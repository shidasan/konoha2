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

#include"commons.h"
#include"../../include/konoha2/konohalang.h"

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

#define K_SYMBOL_MAXSIZ   128

/* ------------------------------------------------------------------------ */
/* [properties] */

static void CWB_nzenvkey(CTX, CWB_t *cwb, kbytes_t t)
{
	size_t i;
	for(i = 0; i < t.len; i++) {
		kwb_putc((cwb->ba), toupper(t.utext[i]));
	}
}

/* ------------------------------------------------------------------------ */

kString* knh_getPropertyNULL(CTX, kbytes_t key)
{
	if(knh_bytes_startsWith_(key, STEXT("env."))) {
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		CWB_nzenvkey(_ctx, cwb, knh_bytes_last(key, 4));
		char *v = knh_getenv(CWB_totext(_ctx, cwb));
		CWB_close(_ctx, cwb);
		if(v == NULL) return NULL;
		return new_kString(v, knh_strlen(v), SPOL_ASCII|SPOL_POOL);
	}
	return (kString*)knh_DictMap_getNULL(_ctx,  ctx->share->props, key);
}

/* ------------------------------------------------------------------------ */

void knh_setProperty(CTX, kString *key, dynamic *value)
{
	knh_DictMap_set_(_ctx, ctx->share->props, key, value);
}

/* ------------------------------------------------------------------------ */

KNHAPI2(void) knh_setPropertyText(CTX, char *key, char *value)
{
	kString *k = new_T(key);
	kString *v = new_kString(value, knh_strlen(value), SPOL_TEXT);
	knh_DictMap_set_(_ctx, ctx->share->props, k, UPCAST(v));
}

/* ------------------------------------------------------------------------ */
/* [Constant Value] */

Object *knh_getClassConstNULL(CTX, kcid_t cid, kbytes_t name)
{
	DBG_ASSERT_cid(cid);
	if(ClassTBL(cid)->constDictCaseMapNULL == NULL) return NULL;
	kDictMap *cmap = ClassTBL(cid)->constDictCaseMapNULL;
	Object *value = NULL;
	OLD_LOCK(_ctx, LOCK_SYSTBL, NULL);
	int res = knh_DictMap_index(cmap, name);
	if(res != -1) {
		value = knh_DictMap_valueAt(cmap, res);
	}
	OLD_UNLOCK(_ctx, LOCK_SYSTBL, NULL);
	return value;
}

/* ------------------------------------------------------------------------ */

int knh_addClassConst(CTX, kcid_t cid, kString* name, Object *value)
{
	int ret;
	kDictMap *cmap = ClassTBL(cid)->constDictCaseMapNULL;
	DBG_ASSERT_cid(cid);
	if(cmap == NULL) {
		kclass_t *t = varClassTBL(cid);
		cmap = new_DictMap0(_ctx, 0, 1/*isCaseMap*/, "ClassTBL.constDictMap");
		KINITv(t->constDictCaseMapNULL, cmap);
	}
	DBG_ASSERT(IS_Map(cmap));
	OLD_LOCK(_ctx, LOCK_SYSTBL, NULL);
	int idx = knh_DictMap_index(cmap, S_tobytes(name));
	if(idx != -1) {
		ret =  0;
		goto L_UNLOCK;
	}
	knh_DictMap_set(_ctx, cmap, name, value);
	ret = 1;

	L_UNLOCK:
	OLD_UNLOCK(_ctx, LOCK_SYSTBL, NULL);
	return ret;
}

/* ------------------------------------------------------------------------ */
/* [tfieldn, tmethodn] */

ksymbol_t knh_addname(CTX, kString *s, knh_Fdictset f)
{
	size_t n = knh_Map_size(_ctx->share->symbolDictCaseSet);
	if(unlikely(!(n+1 < MN_SETTER))) {  /* Integer overflowed */
		KNH_DIE("too many names, last nameid(fn)=%d < %d", (int)(n+1), (int)MN_SETTER);
	}
	kArray_add(ctx->share->symbolList, s);
	f(_ctx, ctx->share->symbolDictCaseSet, s, n + 1);
	return (ksymbol_t)(n);
}

static ksymbol_t addSymbol(CTX, kbytes_t t)
{
	char symbuf[K_SYMBOL_MAXSIZ];
	size_t i, pos = 0;
	int toLower = isupper(t.buf[0]) ? 1 : 0;
	for(i = 0; i < t.len; i++) {
		int ch = t.buf[i];
		if(ch == '_') {
			toLower = 1; continue;
		}
		symbuf[pos] = (toLower) ? tolower(ch) : ch;
		toLower = 0;
		pos++;
		if(!(pos < sizeof(symbuf) - 2)) break;
	}
	symbuf[pos] = 0;
	return knh_addname(_ctx, new_kString((const char*)symbuf, pos, SPOL_ASCII|SPOL_POOL), knh_DictSet_set);
}

static ksymbol_t getSymbol(CTX, kbytes_t n, ksymbol_t def)
{
	OLD_LOCK(_ctx, LOCK_SYSTBL, NULL);
	kindex_t idx = knh_DictSet_index(_ctx->share->symbolDictCaseSet, n);
	if(idx == -1) {
		if(def == FN_NEWID) {
			idx = addSymbol(_ctx, n);
		}
		else {
			idx = def - MN_OPSIZE;
		}
	}
	else {
		idx = knh_DictSet_valueAt(_ctx->share->symbolDictCaseSet, idx) - 1;
	}
	OLD_UNLOCK(_ctx, LOCK_SYSTBL, NULL);
	return (ksymbol_t)idx + MN_OPSIZE;
}

/* ------------------------------------------------------------------------ */
/* [fn] */

const char* knh_getopMethodName(kmethodn_t mn);

KNHAPI2(kString*) knh_getFieldName(CTX, ksymbol_t fn)
{
	fn = FN_UNMASK(fn);
	if(fn < MN_OPSIZE) {
		return new_T(knh_getopMethodName(fn));
	}
	else {
		return ctx->share->symbolList->strings[fn - MN_OPSIZE];
	}
}

/* ------------------------------------------------------------------------ */

ksymbol_t knh_getfnq(CTX, kbytes_t tname, ksymbol_t def)
{
	ksymbol_t mask = 0;
	kindex_t idx = knh_bytes_index(tname, ':');
	if(idx > 0) {
		tname = knh_bytes_first(tname, idx);
	}
	else if(knh_bytes_startsWith_(tname, STEXT("super."))) {
		mask = (def == FN_NONAME) ? 0 : KFLAG_FN_SUPER;
		tname = knh_bytes_last(tname, 6);
	}
	else if(!knh_bytes_endsWith_(tname, STEXT("__"))) {
		if(tname.utext[0] == '_' && def != FN_NONAME) {
			mask = KFLAG_FN_U1;
			tname = knh_bytes_last(tname, 1);
		}
		if(tname.utext[0] == '_' && def != FN_NONAME) {
			mask = KFLAG_FN_U2;
			tname = knh_bytes_last(tname, 1);
		}
		while(tname.utext[0] == '_') {
			tname = knh_bytes_last(tname, 1);
		}
	}
	return getSymbol(_ctx, tname, def) | mask;
}

/* ------------------------------------------------------------------------ */
/* [methodn] */

static kbytes_t knh_bytes_skipFMTOPT(kbytes_t t)
{
	size_t i;
	for(i = 1; i < t.len; i++) {
		if(isalnum(t.utext[i])) break;
	}
	t.utext = t.utext + i;
	t.len = t.len - i;
	return t;
}

kmethodn_t knh_getmn(CTX, kbytes_t tname, kmethodn_t def)
{
	ksymbol_t mask = 0;
	if(tname.utext[0] == 'o' && tname.utext[1] == 'p') {
		kmethodn_t mn = MN_opNOT;
		for(; mn <= MN_opNEG; mn++) {
			const char *op = knh_getopMethodName(mn);
			if(knh_bytes_equals(tname, B(op))) {
				return mn;
			}
		}
	}
	if(tname.utext[0] == '%') {
		tname = knh_bytes_skipFMTOPT(tname);
		if(def != MN_NONAME) mask |= MN_FMT;
	}
	else if(tname.utext[0] == 'i' && tname.utext[1] == 's') { /* is => get */
		tname = knh_bytes_last(tname, 2);
		if(def != MN_NONAME) mask |= MN_ISBOOL;
	}
	else if(tname.utext[0] == 'g' && tname.utext[1] == 'e' && tname.utext[2] == 't') {
		tname = knh_bytes_last(tname, 3);
		if(def != MN_NONAME) mask |= MN_GETTER;
	}
	else if(tname.utext[0] == 's' && tname.utext[1] == 'e' && tname.utext[2] == 't') {
		tname = knh_bytes_last(tname, 3);
		if(def != MN_NONAME) mask |= MN_SETTER;
	}
	return getSymbol(_ctx, tname, def) | mask;
}

/* ------------------------------------------------------------------------ */

const char* knh_getmnname(CTX, kmethodn_t mn)
{
	mn = MN_toFN(mn);
	if(mn < MN_OPSIZE) {
		return knh_getopname(mn);
	}
	return S_text(knh_getFieldName(_ctx, mn));
}

/* ------------------------------------------------------------------------ */
/* [uri] */

kuri_t knh_getURI(CTX, kbytes_t t)
{
	OLD_LOCK(_ctx, LOCK_SYSTBL, NULL);
	kindex_t idx = knh_DictSet_index(_ctx->share->urnDictSet, t);
	if(idx == -1) {
		kString *s = new_kString(t.text, t.len, SPOL_POOL);
		idx = kArray_size(_ctx->share->urns);
		knh_DictSet_set(_ctx, ctx->share->urnDictSet, s, idx);
		kArray_add(ctx->share->urns, s);
		KNH_NTRACE2(_ctx, "konoha:newuri", K_OK, KNH_LDATA(LOG_s("urn", S_text(s)), LOG_i("uri", idx)));
	}
	else {
		idx = knh_DictSet_valueAt(_ctx->share->urnDictSet, idx);
	}
	OLD_UNLOCK(_ctx, LOCK_SYSTBL, NULL);
	return (kuri_t)idx;
}

/* ------------------------------------------------------------------------ */

kString *knh_getURN(CTX, kuri_t uri)
{
	size_t n = URI_UNMASK(uri);
	kArray *a = ctx->share->urns;
	if(n < kArray_size(a)) {
		return (kString*)(a)->list[n];
	}
	else {
		DBG_ASSERT(uri == URI_unknown);
		return TS_EMPTY;
	}
}

/* ------------------------------------------------------------------------ */
/* [Driver] */

static kbytes_t knh_Lingo_getDpiPath(CTX , kLingo *ns, kbytes_t path)
{
	while(DP(ns)->name2dpiNameDictMapNULL != NULL) {
		kString *s = (kString*)knh_DictMap_getNULL(_ctx, DP(ns)->name2dpiNameDictMapNULL, path);
		if(s != NULL) return S_tobytes(s);
		if(ns->parentNULL == NULL) break;
		ns = ns->parentNULL;
	}
	return path;
}

const knh_PathDPI_t *knh_Lingo_getStreamDPINULL(CTX, kLingo *ns, kbytes_t path)
{
	kbytes_t hpath = knh_Lingo_getDpiPath(_ctx, ns, knh_bytes_head(path, ':'));
	return (const knh_PathDPI_t *)knh_DictSet_get(_ctx, ctx->share->streamDpiDictSet, hpath);
}

const knh_MapDPI_t *knh_Lingo_getMapDPINULL(CTX, kLingo *ns, kbytes_t path)
{
	kbytes_t hpath = knh_Lingo_getDpiPath(_ctx, ns, knh_bytes_head(path, ':'));
	return (const knh_MapDPI_t*)knh_DictSet_get(_ctx, ctx->share->mapDpiDictSet, hpath);
}

const knh_ConverterDPI_t *knh_Lingo_getConverterDPINULL(CTX, kLingo *ns, kbytes_t path)
{
	kbytes_t bpath = knh_Lingo_getDpiPath(_ctx, ns, knh_bytes_next(path, ':'));
	void *d;
	if(path.text[0] == 'f') {
		d = (void*)knh_DictSet_get(_ctx, ctx->share->rconvDpiDictSet, bpath);
	}
	else {
		d = (void*)knh_DictSet_get(_ctx, ctx->share->convDpiDictSet, bpath);
	}
	return (const knh_ConverterDPI_t*)d;
}

/* ------------------------------------------------------------------------ */

void knh_exit(CTX, int status)
{
	exit(status);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
