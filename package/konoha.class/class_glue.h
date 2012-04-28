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

#ifndef CLASS_GLUE_H_
#define CLASS_GLUE_H_

static KMETHOD Fmethod_FieldGetter(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	RETURN_((sfp[0].o)->fields[delta]);
}
static KMETHOD Fmethod_FieldGetterN(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	RETURNd_((sfp[0].o)->ndata[delta]);
}
static KMETHOD Fmethod_FieldSetter(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	KSETv((sfp[0].Wo)->fields[delta], sfp[1].o);
	RETURN_(sfp[1].o);
}
static KMETHOD Fmethod_FieldSetterN(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	(sfp[0].Wo)->ndata[delta] = sfp[1].ndata;
	RETURNd_(sfp[1].ndata);
}

static kMethod *new_FieldGetter(CTX, kcid_t cid, ksymbol_t sym, ktype_t ty, int idx)
{
	kmethodn_t mn = ty == TY_Boolean ? MN_toISBOOL(sym) : MN_toGETTER(sym);
	knh_Fmethod f = (TY_isUnbox(ty)) ? Fmethod_FieldGetterN : Fmethod_FieldGetter;
	kParam *pa = new_kParam(ty, 0, NULL);
	kMethod *mtd = new_kMethod(kMethod_Public|kMethod_Immutable|kMethod_Const, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = idx;  // FIXME
	return mtd;
}

static kMethod *new_FieldSetter(CTX, kcid_t cid, kmethodn_t mn, ktype_t ty, int idx)
{
	knh_Fmethod f = (TY_isUnbox(ty)) ? Fmethod_FieldSetterN : Fmethod_FieldSetter;
	kparam_t p = {ty, FN_("x")};
	kParam *pa = new_kParam(ty, 1, &p);
	kMethod *mtd = new_kMethod(kMethod_Public, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = idx;   // FIXME
	return mtd;
}

static intptr_t KLIB2_Method_indexOfField(kMethod *mtd)
{
	knh_Fmethod f = mtd->fcall_1;
	if(f== Fmethod_FieldGetter || f == Fmethod_FieldGetterN || f == Fmethod_FieldSetter || f == Fmethod_FieldSetterN) {
		return (intptr_t)mtd->delta;
	}
	return -1;
}

static void KLIB2_setGetterSetter(CTX, kclass_t *ct)
{
	size_t i, fsize = ct->fsize;
	for(i=0; i < fsize; i++) {
		if(FLAG_is(ct->fields[i].flag, kField_Getter)) {
			FLAG_unset(ct->fields[i].flag, kField_Getter);
			kMethod *mtd = new_FieldGetter(_ctx, ct->cid, ct->fields[i].fn, ct->fields[i].ty, i);
			kArray_add(ct->methods, mtd);
		}
		if(FLAG_is(ct->fields[i].flag, kField_Setter)) {
			FLAG_unset(ct->fields[i].flag, kField_Setter);
			kMethod *mtd = new_FieldSetter(_ctx, ct->cid, ct->fields[i].fn, ct->fields[i].ty, i);
			kArray_add(ct->methods, mtd);
		}
	}
}

static KMETHOD Fmethod_ProtoGetter(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	RETURN_(kObject_getObject(sfp[0].o, key, sfp[K_RTNIDX].o));
}

static KMETHOD Fmethod_ProtoGetterN(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	RETURNd_(kObject_getUnboxedValue(sfp[0].o, key, 0));
}

static KMETHOD Fmethod_ProtoSetter(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	kObject_setObject(sfp[0].o, key, sfp[1].o);
	RETURN_(sfp[1].o);
}

static KMETHOD Fmethod_ProtoSetterN(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	kObject_setUnboxedValue(sfp[0].o, key, mtd->pa->p[0].ty, sfp[1].ndata);
	RETURNd_(sfp[1].ndata);
}

static kMethod *new_ProtoGetter(CTX, kcid_t cid, ksymbol_t sym, ktype_t ty)
{
	kmethodn_t mn = ty == TY_Boolean ? MN_toISBOOL(sym) : MN_toGETTER(sym);
	knh_Fmethod f = (TY_isUnbox(ty)) ? Fmethod_ProtoGetterN : Fmethod_ProtoGetter;
	kParam *pa = new_kParam(ty, 0, NULL);
	kMethod *mtd = new_kMethod(kMethod_Public|kMethod_Immutable|kMethod_Const, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = sym;
	return mtd;
}

static kMethod *new_ProtoSetter(CTX, kcid_t cid, ksymbol_t sym, ktype_t ty)
{
	kmethodn_t mn = MN_toSETTER(sym);
	knh_Fmethod f = (TY_isUnbox(ty)) ? Fmethod_ProtoSetterN : Fmethod_ProtoSetter;
	kparam_t p = {ty, FN_("x")};
	kParam *pa = new_kParam(ty, 1, &p);
	kMethod *mtd = new_kMethod(kMethod_Public, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = sym;
	return mtd;
}

//static kMethod *KLIB2_getGetterNULL(CTX, kKonohaSpace *ks, kclass_t *ct, ksymbol_t sym)
//{
//	kMethod *mtd = kKonohaSpace_getMethodNULL(ks, ct, MN_toGETTER(sym));
//	if(mtd != NULL) return mtd;
//	mtd = kKonohaSpace_getMethodNULL(ks, ct, MN_toISBOOL(sym));
//	return mtd;
//}
//
//static kMethod *KLIB2_getSetterNULL(CTX, kKonohaSpace *ks, kclass_t *ct, ksymbol_t sym, ktype_t ty)
//{
//	kMethod *mtd = kKonohaSpace_getMethodNULL(ks, ct, MN_toSETTER(sym));
//	if(mtd == NULL && ty == TY_void) {
//		mtd = new_ProtoGetter(_ctx, ct->cid, sym, ty);
//		kArray_add(ct->methods, mtd);
//		mtd = new_ProtoSetter(_ctx, ct->cid, sym, ty);
//		kArray_add(ct->methods, mtd);
//	}
//	return mtd;
//}

// --------------------------------------------------------------------------

// int KonohaSpace.getCid(String name, int defval)
static KMETHOD KonohaSpace_getCid(CTX, ksfp_t *sfp _RIX)
{
	kclass_t *ct = kKonohaSpace_getCT(sfp[0].ks, NULL/*fixme*/, S_text(sfp[1].s), S_size(sfp[1].s), (kcid_t)sfp[2].ivalue);
	kint_t cid = ct != NULL ? ct->cid : sfp[2].ivalue;
	RETURNi_(cid);
}

// int KonohaSpace.defineClass(int flag, String name, int supcid, int fieldsize);
static KMETHOD KonohaSpace_defineClass(CTX, ksfp_t *sfp _RIX)
{
	kKonohaSpace *ks = sfp[0].ks;
	KDEFINE_CLASS cdef = {};
	cdef.cstruct_size = sfp[4].ivalue * sizeof(void*);
	cdef.cflag  = (kflag_t)sfp[1].ivalue | ((cdef.cstruct_size > 0) ? kClass_UNDEF : 0);
	cdef.cid    = CLASS_newid;
	cdef.bcid   = CLASS_Object;
	cdef.supcid = (kcid_t)sfp[3].ivalue;
//	cdef.packid = ks->packid;
//	cdef.packdom = ks->packdom;
	if(cdef.supcid == 0) cdef.supcid = TY_Object;
	kclass_t *supct = kclass(cdef.supcid, sfp[K_RTNIDX].uline);
	if(CT_isFinal(supct)) {
		kreportf(CRIT_, sfp[K_RTNIDX].uline, "%s is @Final", T_cid(cdef.supcid));
	}
	kclass_t *c = Konoha_addClassDef(ks->packid, ks->packdom, sfp[2].s, &cdef, sfp[K_RTNIDX].uline);
	RETURNi_(c->cid);
}

// int KonohaSpace.defineClassField(int cid, int flag, int ty, String name, Object *value);
static KMETHOD KonohaSpace_defineClassField(CTX, ksfp_t *sfp _RIX)
{
	kcid_t cid = (kcid_t)sfp[1].ivalue;
	kflag_t flag = (kflag_t)sfp[2].ivalue;
	ktype_t ty = (ktype_t)sfp[3].ivalue;
	kString *name = sfp[4].s;
	kObject *value = sfp[5].o;
	struct _kclass *ct = (struct _kclass*)kclass(cid, sfp[K_RTNIDX].uline);
	if(!CT_isUNDEF(ct) || !(ct->fsize < ct->fallocsize)) {
		kreportf(CRIT_, sfp[K_RTNIDX].uline, "all fields are defined: %s", T_cid(ct->cid));
	}
	int pos = ct->fsize;
	ct->fsize += 1;
	ct->fields[pos].flag = flag;
	ct->fields[pos].ty = ty;
	ct->fields[pos].fn = ksymbol(S_text(name), S_size(name), FN_NEWID, SYMPOL_NAME);
	if(TY_isUnbox(ty)) {
		ct->WnulvalNUL->ndata[pos] = O_unbox(value);
	}
	else {
		kObject *v = (IS_NULL(value)) ? knull(O_ct(value)) : value;
		KSETv(ct->WnulvalNUL->ndata[pos], v);
		ct->fields[pos].isobj = 1;
	}
	if(!(ct->fsize < ct->fallocsize)) {
		DBG_P("all fields are set");
		KLIB2_setGetterSetter(_ctx, ct);
		CT_setUNDEF(ct, 0);
	}
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t class_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	int FN_flag = FN_("flag"), FN_cid = FN_("cid"), FN_name = FN_("name"), FN_defval = FN_("defval");
	intptr_t MethodData[] = {
		_Public, _F(KonohaSpace_getCid), TY_Int, TY_KonohaSpace, MN_("cid"), 2, TY_String, FN_name, TY_Int, FN_defval,
		_Public, _F(KonohaSpace_defineClass), TY_Int, TY_KonohaSpace, MN_("defineClass"), 4, TY_Int, FN_flag, TY_String, FN_name, TY_Int, FN_("supcid"), TY_Int, FN_("fieldSize"),
		_Public, _F(KonohaSpace_defineClassField), TY_Int, TY_KonohaSpace, MN_("defineClassField"), 5, TY_Int, FN_cid, TY_Int, FN_flag, TY_Int, FN_("type"), TY_String, FN_name, TY_Object, FN_defval,
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t class_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t class_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t class_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#endif /* CLASS_GLUE_H_ */
