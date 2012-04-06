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

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef K_USING_SEMANTICS

static
kcid_t knh_addSpecializedType(CTX, kcid_t cid, kcid_t bcid, kSemantics *u);

/* ------------------------------------------------------------------------ */

kInt* new_Int_X(CTX, kcid_t cid, kint_t value)
{
	kSemantics *u = knh_getSemantics(_ctx, cid);
	if(DP(u)->fichk(u, value)) {
		kInt *n = (kInt*)new_H(_ctx, FLAG_Int, CLASS_Int, cid);
		n->n.ivalue = value;
		return n;
	}
	else {
		if(DP(u)->imin >= 0) {
			KNH_SYSLOG(_ctx, LOG_WARNING, _("out of range: %u of %s"), value, CLASS__(cid));
		}
		else {
			KNH_SYSLOG(_ctx, LOG_WARNING, _("out of range: %d of %s"), value, CLASS__(cid));
		}
		return DP(u)->ivalue;
	}
}

/* ------------------------------------------------------------------------ */

kFloat* new_Float_X(CTX, kcid_t cid, kfloat_t value)
{
	DBG_ASSERT_cid(cid);
	kSemantics *u = knh_getSemantics(_ctx, cid);
	if(DP(u)->ffchk(u, value)) {
		kFloat *f = (kFloat*)new_H(_ctx, FLAG_Float, CLASS_Float, cid);
		f->n.fvalue = value;
		return f;
	}
	else {
		KNH_SYSLOG(_ctx, LOG_WARNING, _("out of range: %f of %s"), value, CLASS__(cid));
		return DP(u)->fvalue;
	}
}

/* ------------------------------------------------------------------------ */
/* [Int] */

static int knh_fichk__range(kSemantics *u, kint_t v)
{
	return (DP(u)->imin <= v && v <= DP(u)->imax);
}

static int knh_fichk__umax(kSemantics *u, kuint_t v)
{
	return (v <= DP(u)->umax);
}

static int knh_fichk__urange(kSemantics *u, kuint_t v)
{
	return (DP(u)->umin <= v && v <= DP(u)->umax);
}

static int knh_ficmp__unsigned(kSemantics *u, kuint_t v1, kuint_t v2)
{
	if(v1 == v2) return 0;
	return (v1 > v2) ? 1 : -1;
}

static int knh_ffchk__range(kSemantics *u, kfloat_t v)
{
	return (DP(u)->fmin <= v && v <= DP(u)->fmax);
}

static int knh_ffcmp__step(kSemantics *u, kfloat_t v1, kfloat_t v2)
{
	kfloat_t v = v1 - v2;
	if(v >= DP(u)->fstep) return 1;
	if(v <= -(DP(u)->fstep)) return -1;
	return 0;
}

/* ------------------------------------------------------------------------ */

static void knh_Semantics_initIntRange(CTX, kSemantics *u, kint_t min, kint_t max)
{
	DP(u)->imin = min;
	DP(u)->fmin = (kfloat_t)min;
	DP(u)->imax = max;
	DP(u)->fmax = (kfloat_t)max;
	DP(u)->fstep = KFLOAT_ONE;
	if(min >= 0) {
		DP(u)->ficmp = (knh_Ficmp)knh_ficmp__unsigned;
		if(min == 0) {
			DP(u)->fichk = (knh_Fichk)knh_fichk__umax;
		}
		else {
			DP(u)->fichk = (knh_Fichk)knh_fichk__urange;
		}
		DP(u)->ffchk = knh_ffchk__range;
	}
	else {
		if(min != KINT_MIN || max != KINT_MAX) {
			DP(u)->fichk = knh_fichk__range;
			DP(u)->ffchk = knh_ffchk__range;
		}
	}
	DP(u)->ffcmp = knh_ffcmp__step;
}

/* ------------------------------------------------------------------------ */

void knh_write_intx(CTX, kOutputStream *w, kSemantics *u, kint_t v)
{
	char buf[KINT_FMTSIZ];
	char *fmt = KINT_FMT;
	if(DP(u)->imin >= 0) {
		fmt = KUINT_FMT;
	}
	knh_snprintf(buf, sizeof(buf), fmt, v);
	knh_write(_ctx, w, B(buf));
	kbytes_t tag = S_tobytes(DP(u)->tag);
	if(tag.len > 0) {
		kwb_putc(wb, '[');
		knh_write(_ctx, w, tag);
		kwb_putc(wb, ']');
	}
}

/* ------------------------------------------------------------------------ */

static
void knh_Semantics_initFloatRange(CTX, kSemantics *u, kfloat_t min, kfloat_t max, kfloat_t step)
{
	DP(u)->fmin = min;
	DP(u)->fmax = max;
	DP(u)->fstep = step;
	DP(u)->imax = (kint_t)max;
	DP(u)->imin = (kint_t)min;

	if(min >= KFLOAT_ZERO) {
		DP(u)->ficmp = (knh_Ficmp)knh_ficmp__unsigned;
		if(min == 0) {
			DP(u)->fichk = (knh_Fichk)knh_fichk__umax;
		}
		else {
			DP(u)->fichk = (knh_Fichk)knh_fichk__urange;
		}
		DP(u)->ffchk = knh_ffchk__range;
	}
	else {
		if(min != KFLOAT_MIN || max != KFLOAT_MAX) {
			DP(u)->fichk = knh_fichk__range;
			DP(u)->ffchk = knh_ffchk__range;
		}
	}

	if(step != KFLOAT_ZERO) {
		DP(u)->ffcmp = knh_ffcmp__step;
	}
}

/* ------------------------------------------------------------------------ */

void knh_write_floatx(CTX, kOutputStream *w, kSemantics *u, kfloat_t v)
{
	char *FMT = KFLOAT_FMT;
#if !defined(K_USING_NOFLOT)
	kfloat_t step = DP(u)->fstep;
	if(0.1 <= step && step < 1.0) {
		FMT = KFLOAT_FMT1;
	}
	else if(0.01 <= step && step < 0.1) {
		FMT = KFLOAT_FMT2;
	}
	else if(0.001 <= step && step < 0.01) {
		FMT = KFLOAT_FMT3;
	}
	else if(0.0001 <= step && step < 0.001) {
		FMT = KFLOAT_FMT4;
	}
#endif
	knh_write_ffmt(_ctx, w, FMT, v);
	kbytes_t tag = S_tobytes(DP(u)->tag);
	if(tag.len > 0) {
		kwb_putc(wb, '[');
		knh_write(_ctx, w, tag);
		kwb_putc(wb, ']');
	}
}

/* ------------------------------------------------------------------------ */

static TYPEMAP knh_IntX_FloatX(CTX, ksfp_t *sfp _RIX)
{
	kfloat_t v = (kfloat_t)sfp[0].ivalue;
	KNH_MAPPED_Float(_ctx, sfp, v);
}

/* ------------------------------------------------------------------------ */

static TYPEMAP knh_FloatX_IntX(CTX, ksfp_t *sfp _RIX)
{
	kint_t v = (kint_t)sfp[0].fvalue;
	KNH_MAPPED_Int(_ctx, sfp, v);
}

/* ------------------------------------------------------------------------ */

static
int Semantics_isVocab(kSemantics *u)
{
	return IS_DictIdx(DP(u)->vocabDictIdx);
}

/* ------------------------------------------------------------------------ */

static
kString *knh_Semantics_getVocabAt(CTX, kSemantics *u, size_t n)
{
	return knh_DictIdx_get__fast(DP(u)->vocabDictIdx, n - DP(u)->imin);
}

/* ------------------------------------------------------------------------ */

static
kint_t knh_Semantics_getVocabIdx(CTX, kSemantics *u, kString *s)
{
	return knh_DictIdx_index(_ctx, DP(u)->vocabDictIdx, S_tobytes(s)) + DP(u)->imin;
}

/* ------------------------------------------------------------------------ */

static TYPEMAP knh_IntX_Vocab(CTX, ksfp_t *sfp _RIX)
{
	kSemantics *u = (kSemantics*)DP(sfp[1].mpr)->mapdata;
	KNH_ASSERT(IS_Semantics(u));
	RETURN_(knh_Semantics_getVocabAt(_ctx, u, sfp[0].ivalue));
}

/* ------------------------------------------------------------------------ */

static TYPEMAP knh_Vocab_IntX(CTX, ksfp_t *sfp _RIX)
{
	kSemantics *u = (kSemantics*)DP(sfp[1].mpr)->mapdata;
	KNH_ASSERT(IS_Semantics(u));
	KNH_MAPPED_Int(_ctx, sfp, knh_Semantics_getVocabIdx(_ctx, u, sfp[0].s));
}

/* ------------------------------------------------------------------------ */

void knh_Semantics_reuse(CTX, kSemantics *u, kcid_t cid)
{
	kcid_t bcid = ClassTBL(cid)->bcid;
	if(bcid == CLASS_Int) {
		kint_t v = 0;
		if(!DP(u)->fichk(u, v)) {
			v = DP(u)->imin;
		}
		KNH_ASSERT(DP(u)->ivalue == NULL);
		KINITv(DP(u)->ivalue, new_Int_(_ctx, cid, v));
		if(DP(u)->fvalue != NULL) {
			kTypeMap *mpr = new_TypeMap(_ctx, FLAG_TypeMap_Affine, DP(u)->ucid, cid, knh_FloatX_IntX, (Object*)u);
			knh_addTypeMap(_ctx, mpr);
			mpr = new_TypeMap(_ctx, FLAG_TypeMap_Affine, cid, DP(u)->ucid, knh_IntX_FloatX, (Object*)u);
			knh_addTypeMap(_ctx, mpr);
		}
		if(DP(u)->svalue != NULL) {
			if(Semantics_isVocab(u)) {
				kTypeMap *mpr = new_TypeMap(_ctx, FLAG_TypeMap_Affine, cid, DP(u)->ucid, knh_IntX_Vocab, (Object*)u);
				knh_addTypeMap(_ctx, mpr);
				mpr = new_TypeMap(_ctx, FLAG_TypeMap_Affine, DP(u)->ucid, cid, knh_Vocab_IntX, (Object*)u);
				knh_addTypeMap(_ctx, mpr);
			}
		}
	}
	else if(bcid == CLASS_Float) {
		kfloat_t v = 0;
		if(!DP(u)->ffchk(u, v)) {
			v = DP(u)->fmin;
		}
		KNH_ASSERT(DP(u)->fvalue == NULL);
		KINITv(DP(u)->fvalue, new_Float_(_ctx, cid, v));
		if(DP(u)->ivalue != NULL) {
			kTypeMap *mpr = new_TypeMap(_ctx, FLAG_TypeMap_Affine, cid, DP(u)->ucid, knh_FloatX_IntX, (Object*)u);
			knh_addTypeMap(_ctx, mpr);
			mpr = new_TypeMap(_ctx, FLAG_TypeMap_Affine, DP(u)->ucid, cid, knh_IntX_FloatX, (Object*)u);
			knh_addTypeMap(_ctx, mpr);
		}
	}
	else {
		KNH_ASSERT(bcid == CLASS_String);
		TODO();

	}
}

/* ------------------------------------------------------------------------ */

kSemantics* new_Enum(CTX, char *tag, kbytes_t urn, kint_t min, kint_t max)
{
	kcid_t cid = new_ClassId(_ctx);
	kSemantics* u = (kSemantics*)new_Object_bcid(_ctx, CLASS_Semantics, (int)cid);
	DP(u)->ubcid = CLASS_Int;
	KSETv(DP(u)->urn, new_S(_ctx, urn, NULL));
	if(tag != NULL || tag[0] != 0) {
		KSETv(DP(u)->tag, new_T(tag));
	}
	knh_Semantics_initIntRange(_ctx, u, min, max);
	if(!DP(u)->fichk(u, 0)) {
		KINITv(DP(u)->ivalue, new_Int_(_ctx, cid, min));
	}
	else {
		KINITv(DP(u)->ivalue, new_Int_(_ctx, cid, 0));
	}
	knh_addSpecializedType(_ctx, cid, CLASS_Int, u);
	return u;
}

/* ------------------------------------------------------------------------ */


kSemantics* new_Unit(CTX, char *tag, kbytes_t urn, kfloat_t min, kfloat_t max, kfloat_t step)
{
	kcid_t cid = new_ClassId(_ctx);
	kSemantics* u = (kSemantics*)new_Object_bcid(_ctx, CLASS_Semantics, (int)cid);
	DP(u)->ubcid = CLASS_Float;
	KSETv(DP(u)->urn, new_S(_ctx, urn, NULL));
	if(tag != NULL || tag[0] != 0) {
		KSETv(DP(u)->tag, new_T(tag));

	}
	knh_Semantics_initFloatRange(_ctx, u, min, max, step);
#if !defined(K_USING_NOFLOT)
	if(!DP(u)->ffchk(u, 0.0)) {
		KINITv(DP(u)->fvalue, new_Float_(_ctx, cid, min));
	}
	else {
		KINITv(DP(u)->fvalue, new_Float_(_ctx, cid, 0.0));
	}
#endif
	knh_addSpecializedType(_ctx, cid, CLASS_Float, u);
	return u;
}

/* ------------------------------------------------------------------------ */
/* [String] */

static
kString *knh_fsnew__dict(CTX, kcid_t cid, kbytes_t t, kString *memoNULL, int *foundError)
{
	kSemantics *u = knh_getSemantics(_ctx, cid);
	kindex_t n = knh_DictIdx_index(_ctx, DP(u)->vocabDictIdx, t);
	if(n == -1) {
		kbytes_t tag;
		if(knh_bytes_splitTag(t, &tag, &t)) {
			n = knh_DictIdx_index(_ctx, DP(u)->vocabDictIdx, t);
		}
	}
	if(n == -1) {
		*foundError = 1;
		return DP(u)->svalue;
	}
	return knh_DictIdx_get__fast(DP(u)->vocabDictIdx, n);
}

/* ------------------------------------------------------------------------ */

static
int knh_fscmp__dict(kSemantics *u, kbytes_t v1, kbytes_t v2)
{
	return knh_DictIdx_index(NULL, DP(u)->vocabDictIdx, v1) - knh_DictIdx_index(NULL, DP(u)->vocabDictIdx, v2);
}

/* ------------------------------------------------------------------------ */

static
knh_DictIdx_t* new_DictIdx__Array(CTX, kArray *a)
{
	knh_DictIdx_t *o = (knh_DictIdx_t*)new_H(_ctx, FLAG_DictIdx, CLASS_DictIdx, CLASS_DictIdx);
	KINITv(o->terms, a);
	KINITv(o->termsDictSet, new_DictSet(_ctx, kArray_size(a)));
	o->offset = 0;
	{
		uintptr_t i = 0;
		for(i = 0; i < kArray_size(a); i++) {
			kString *s = (kString*)kArray_n(a, i);
			knh_DictSet_append(_ctx, o->termsDictSet, s, i+1);
		}
	}
	//kDictSetoIgnoreCase(o->termsDictSet);
	return o;
}

/* ------------------------------------------------------------------------ */

static
TYPEMAP knh_Ftypemap_vocabidx(CTX, ksfp_t *sfp _RIX)
{
	kSemantics *u = knh_getSemantics(_ctx, O_cid(sfp[0].o));
	kint_t n = knh_Semantics_getVocabIdx(_ctx, u, sfp[0].s);
	DBG_P("n = %d", (int)n);
	KNH_MAPPED_Int(_ctx, sfp, n);
}

/* ------------------------------------------------------------------------ */

static
TYPEMAP knh_Ftypemap_vocab(CTX, ksfp_t *sfp _RIX)
{
	kSemantics *u = (kSemantics*)DP(sfp[1].mpr)->mapdata;
	size_t n = (size_t)(sfp[0].ivalue - DP(u)->imin);
	DBG_P("n = %zd", n);
	kArray *a = (DP(u)->vocabDictIdx)->terms;
	Object *s = K_NULL;
	if(n < kArray_size(a)) {
		s = kArray_n(a, n);
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */

kSemantics* new_Vocab(CTX, char *tag, kbytes_t urn, int base, char **terms)
{
	kcid_t cid = new_ClassId(_ctx);
	kSemantics* u = (kSemantics*)new_Object_bcid(_ctx, CLASS_Semantics, (int)cid);
	DP(u)->ubcid = CLASS_String;
	KSETv(DP(u)->urn, new_S(_ctx, urn, NULL));
	if(tag != NULL || tag[0] != 0) {
		KSETv(DP(u)->tag, new_T(tag));
	}
	DP(u)->fsnew = knh_fsnew__dict;
	DP(u)->fscmp = knh_fscmp__dict;
	{
		kArray *a = new_Array0(_ctx, 0);
		while(*terms != NULL) {
			kString *s = new_T(*terms);
			kArray_add_(_ctx, a, UPCAST(s));
			s->h.cid = cid;
			terms++;
		}
		KSETv(DP(u)->vocabDictIdx, new_DictIdx__Array(_ctx, a));
		knh_Semantics_initIntRange(_ctx, u, (kint_t)base, (kint_t)(base + kArray_size(a) - 1));
		KNH_ASSERT(kArray_size(a) > 0);
		KINITv(DP(u)->svalue, kArray_n(a, 0));
	}
	knh_addSpecializedType(_ctx, cid, CLASS_String, u);
	{
		kTypeMap *mpr = new_TypeMap(_ctx, FLAG_TypeMap_Total|FLAG_TypeMap_Const|FLAG_TypeMap_Final, cid, CLASS_Int, knh_Ftypemap_vocabidx, (Object*)u);
		knh_addTypeMap(_ctx, mpr);
		mpr = new_TypeMap(_ctx, FLAG_TypeMap_Const|FLAG_TypeMap_Final, CLASS_Int, cid, knh_Ftypemap_vocab, (Object*)u);
		knh_addTypeMap(_ctx, mpr);
	}
	return u;
}

///* ------------------------------------------------------------------------ */
//
//static
//TYPEMAP knh_TypeMap__fdict(CTX, ksfp_t *sfp _RIX)
//{
//	String *s = sfp[0].s;
//	Semantics *u = knh_getSemantics(_ctx, O_cid(sfp[0].o)].cspec;
//	KNH_ASSERT(IS_Semantics(u));
//	KNH_ASSERT(IS_DictIdx(DP(u)->vocabDictIdx));
//
//	Semantics *u2 = knh_getSemantics(_ctx, DP(sfp[1].mpr)->tcid].cspec;
//	KNH_ASSERT(IS_Semantics(u2));
//	KNH_ASSERT(IS_DictIdx(DP(u2)->vocabDictIdx));
//
//	size_t n = (size_t)knh_DictIdx_index(_ctx, DP(u)->vocabDictIdx, S_tobytes(s));
//	KNH_ASSERT(n < knh_DictIdx_size(DP(u2)->vocabDictIdx));
//	RETURN_(knh_DictIdx_get__fast(DP(u2)->vocabDictIdx, n));
//}
//
///* ------------------------------------------------------------------------ */
//
//void knh_addVocabularyTypeMap(CTX, kcid_t scid, char *text)
//{
//	DBG_ASSERT_cid(scid);
//	kcid_t tcid = knh_findcid(_ctx, B(text));
//
//	if(tcid != CLASS_UNknown || ctx->share->ClassTBL[tcid].bcid != tcid) {
////		Semantics *u = knh_getSemantics(_ctx, scid].cspec;
////		if(!IS_Semantics(u) || !IS_DictIdx(DP(u)->vocabDictIdx)) {
////			TODO();
////			return;
////		}
//		Semantics *u2 = knh_getSemantics(_ctx, tcid].cspec;
//		if(!IS_Semantics(u2) || !IS_DictIdx(DP(u2)->vocabDictIdx)) {
//			TODO();
//			return;
//		}
////		if(knh_DictIdx_size(DP(u)->vocabDictIdx) != knh_DictIdx_size(DP(u2)->vocabDictIdx)) {
////			TODO();
////			return;
////		}
//		TypeMap *mpr = new_TypeMap(_ctx, FLAG_TypeMap_Affine, scid, tcid,
//				knh_TypeMap_fvocab, (Object*)K_NULL);
//		if(_ctx->share->ClassTBL[scid].cmap == NULL) {
//			kclass_t *TC = (kclass_t*)(&ctx->share->ClassTBL[scid]);
//			KINITv(TC->cmap, new_ClassMap0(_ctx, 4));
//		}
//		knh_ClassMap_add(_ctx, ctx->share->ClassTBL[scid].cmap, mpr);
//
//		mpr = new_TypeMap(_ctx, FLAG_TypeMap_Affine, tcid, scid,
//				knh_TypeMap_fvocab, (Object*)K_NULL);
//		if(_ctx->share->ClassTBL[tcid].cmap == NULL) {
//			kclass_t *TC = (kclass_t*)(&ctx->share->ClassTBL[tcid]);
//			KINITv(TC->cmap, new_ClassMap0(_ctx, 4));
//		}
//		knh_ClassMap_add(_ctx, ctx->share->ClassTBL[tcid].cmap, mpr);
//	}
//}

/* ------------------------------------------------------------------------ */
/* [urnalias] */

kbytes_t knh_getURNAlias(CTX, kbytes_t aurn)
{
	kString *s = (kString*)knh_DictMap_get(_ctx,  ctx->share->URNAliasDictMap, aurn);
	if(IS_NOTNULL(s)) {
		return S_tobytes(s);
	}
	return aurn;
}

///* ------------------------------------------------------------------------ */
//
//void knh_setAliasURN(CTX, String *alias, String *urn)
//{
//	if(CTX_isVerbose(_ctx)) {
//		String *s =(String*)knh_DictMap_get__b(_ctx,  ctx->share->URNAliasDictMap, S_tobytes(urn));
//		if(IS_NOTNULL(s)) {
//			KNH_SYSLOG(_ctx, LOG_WARNING, "Overriding %s %s", __totext(alias), __totext(s));
//		}
//	}
//	knh_DictMap_set(_ctx, ctx->share->URNAliasDictMap, alias, UPCAST(urn));
//}

/* ------------------------------------------------------------------------ */

void knh_loadScriptURNAliasData(CTX, knh_StringData_t *data)
{
	kDictMap *mapptr = ctx->share->URNAliasDictMap;
	knh_StringData_t *d = data;
	while(d->name != NULL) {
		kString *s =(kString*)knh_DictMap_get(_ctx,  mapptr, B(d->name));
		if(IS_NOTNULL(s) && !S_equals(s, B(d->value))) {
			KNH_SYSLOG(_ctx, LOG_WARNING, _("overriding alias %s %s as %s"), d->name, S_text(s), d->value);
		}
		d++;
	}
	d = data;
	while(d->name != NULL) {
		kString *n = new_T(d->name);
		knh_DictMap_append(_ctx, mapptr, n, UPCAST(new_T(d->value)));
		d++;
	}
}

/* ------------------------------------------------------------------------ */

void knh_loadScriptSemanticsFuncData(CTX, knh_FuncData_t *data)
{
	kDictSet *ds = ctx->share->SpecFuncDictSet;
	OLD_LOCK(_ctx, LOCK_SYSTBL, NULL);
	while(data->name != NULL) {
		DBG_P("adding.. '%s'", data->name);
		kString *n = new_T(data->name);
		knh_DictSet_set(_ctx, ds, n, (uintptr_t)data->ptr);
		data++;
	}
	OLD_UNLOCK(_ctx, LOCK_SYSTBL, NULL);
}

/* ------------------------------------------------------------------------ */

static
kSemantics *new_SemanticsNULL(CTX, kbytes_t urn)
{
	kSemantics *u = NULL;
	OLD_LOCK(_ctx, LOCK_SYSTBL, NULL);
	kindex_t loc = 0;
	kbytes_t p = urn;
	while(loc != -1) {
		knh_fspec func = (knh_fspec)knh_DictSet_get(_ctx->share->SpecFuncDictSet, p);
		if(func != NULL) {
			u = func(_ctx, urn);
			if(u != NULL) {
				KSETv(DP(u)->urn, new_S(_ctx, urn, NULL));
			}
			goto L_UNLOCK;
		}
		loc = knh_bytes_rindex(p, '/');
		if(loc != -1) {
			p = knh_bytes_first(p, loc);
		}
	}
L_UNLOCK:;
	OLD_UNLOCK(_ctx, LOCK_SYSTBL, NULL);
	return u;
}

/* ------------------------------------------------------------------------ */
/* [SPEC] */

kSemantics *knh_getSemantics(CTX, kcid_t cid)
{
	kSemantics *u = (kSemantics*)ClassTBL(cid)->cspec;
	KNH_ASSERT(IS_Semantics(u));
	return u;
}

/* ------------------------------------------------------------------------ */

static
Object *knh_ClassTBL_fdefault__ISPEC(CTX, kcid_t cid)
{
	kSemantics *u = knh_getSemantics(_ctx, cid);
	return UPCAST(DP(u)->ivalue);
}

/* ------------------------------------------------------------------------ */

static
Object *knh_ClassTBL_fdefault__FSPEC(CTX, kcid_t cid)
{
	kSemantics *u = knh_getSemantics(_ctx, cid);
	return UPCAST(DP(u)->fvalue);
}

/* ------------------------------------------------------------------------ */

static
Object *knh_ClassTBL_fdefault__SSPEC(CTX, kcid_t cid)
{
	kSemantics *u = knh_getSemantics(_ctx, cid);
	return UPCAST(DP(u)->svalue);
}

/* ------------------------------------------------------------------------ */

static
kcid_t knh_addSpecializedType(CTX, kcid_t cid, kcid_t supcid, kSemantics *u)
{
	kcid_t bcid = C_bcid(supcid);
	char bufcn[CLASS__AME_BUFSIZ];
	if(cid == CLASS_newid) {
		cid = new_ClassId(_ctx);
	}
	knh_snprintf(bufcn, sizeof(bufcn), KNH_SEMANTICS_FMT, CLASS__(bcid), S_text(DP(u)->urn));
	knh_setClassName(_ctx, cid, new_S(_ctx, B(bufcn), NULL));
	{
		kclass_t *t = varClassTBL(cid);
		if((DP(u)->tag)->size > 0) {
			knh_snprintf(bufcn, sizeof(bufcn), "%s:%s", CLASS__(bcid), S_text(DP(u)->tag));
			KSETv(t->sname, new_S(_ctx, B(bufcn), NULL));
		}
		DBG_P("added %d, '%s'", cid, S_text(t->lname));

		t->bcid   = bcid;
		t->supcid = bcid;
		t->cflag  = ClassTBL(bcid)->cflag | FLAG_Class_Final;
		t->oflag  = ClassTBL(bcid)->oflag;
		t->offset = ClassTBL(bcid)->offset;

		t->cspi  = ClassTBL(bcid)->cspi;
		t->size = ClassTBL(bcid)->size;
		t->bsize  = ClassTBL(bcid)->bsize;

		DBG_ASSERT(t->fields == NULL);
		KINITv(t->methods, ClassTBL(supcid).methods);

		if(t->cmap == NULL) {
			KINITv(t->typemaps, K_EMPTYARRAY);
		}
		else {
			KNH_ASSERT(IS_ClassMap(t->cmap));
		}

		DBG_ASSERT(t->cspec == NULL);
		KINITv(t->cspec, u);

		if(DP(u)->ucid != cid) {
			knh_Semantics_reuse(_ctx, u, cid);
		}
		if(bcid == CLASS_Int) {
			KNH_ASSERT(DP(u)->ivalue != NULL);
			t->fdefnull = knh_ClassTBL_fdefault__ISPEC;
		}
		else if(bcid == CLASS_Float) {
			KNH_ASSERT(DP(u)->fvalue != NULL);
			t->fdefnull = knh_ClassTBL_fdefault__FSPEC;
		}
		else {
			KNH_ASSERT(bcid == CLASS_String);
			KNH_ASSERT(DP(u)->svalue != NULL);
			t->fdefnull = knh_ClassTBL_fdefault__SSPEC;
		}
	}
	return cid;
}

/* ------------------------------------------------------------------------ */

static
kSemantics* knh_findSemanticsNULL(CTX, kbytes_t lname)
{
	kindex_t loc = knh_bytes_index(lname, '{');
	if(loc != -1) {
		char *postfix = (char*)lname.buf + loc;
		char cname[CLASS__AME_BUFSIZ];
		knh_snprintf(cname, sizeof(cname), "Int%s", postfix);
		kcid_t cid = knh_getcid(_ctx, B(cname));
		if(cid != CLASS_UNknown) {
			return knh_getSemantics(_ctx, cid);
		}
		knh_snprintf(cname, sizeof(cname), "Float%s", postfix);
		cid = knh_getcid(_ctx, B(cname));
		if(cid != CLASS_UNknown) {
			return knh_getSemantics(_ctx, cid);
		}
		knh_snprintf(cname, sizeof(cname), "String%s", postfix);
		cid = knh_getcid(_ctx, B(cname));
		if(cid != CLASS_UNknown) {
			return knh_getSemantics(_ctx, cid);
		}
	}
	return NULL;
}

/* ------------------------------------------------------------------------ */

kcid_t knh_findcidx(CTX, kbytes_t lname)
{
	kSemantics *u = 	knh_findSemanticsNULL(_ctx, lname);
	if(u != NULL) {
		kindex_t loc = knh_bytes_index(lname, '{');
		kcid_t bcid = knh_getcid(_ctx, knh_bytes_first(lname, loc));
		return knh_addSpecializedType(_ctx, CLASS_newid, bcid, u);
	}
	else {
		kindex_t loc = knh_bytes_index(lname, '{');
		kbytes_t urn = knh_bytes_last(lname, loc+1); urn.len -= 1;
		kcid_t bcid = knh_getcid(_ctx, knh_bytes_first(lname, loc));
		DBG_P("cid=%d,%s", bcid, CLASS__(bcid));
		u = new_SemanticsNULL(_ctx, urn);
		if(u != NULL) {
			kcid_t ucid = DP(u)->ucid;
			kcid_t ubcid = DP(u)->ubcid;
			DBG_P("cid=%d,%s", ubcid, CLASS__(ubcid));
			//knh_addSpecializedType(_ctx, ucid, ubcid, u);
			if(ubcid != bcid) {
				return knh_addSpecializedType(_ctx, CLASS_newid, bcid, u);
			}
			return ucid;
		}
		return bcid;
	}
}

/* ------------------------------------------------------------------------ */

#endif/*K_USING_SEMANTICS*/

#ifdef __cplusplus
}
#endif
