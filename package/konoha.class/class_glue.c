#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

// --------------------------------------------------------------------------

static KMETHOD Fmethod_getter(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	RETURN_((sfp[0].o)->fields[delta]);
}
static KMETHOD Fmethod_ngetter(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	RETURNd_((sfp[0].o)->ndata[delta]);
}
static KMETHOD Fmethod_setter(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	KSETv((sfp[0].Wo)->fields[delta], sfp[1].o);
	RETURN_(sfp[1].o);
}
static KMETHOD Fmethod_nsetter(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	(sfp[0].Wo)->ndata[delta] = sfp[1].ndata;
	RETURNd_(sfp[1].ndata);
}

static KMETHOD Fmethod_getterP(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	kObject *v = kObject_getObjectNULL(sfp[0].o, key);
	if(v == NULL) v = knull(CT_(mtd->pa->rtype));
	RETURN_(v);
}

static kMethod *new_GetterMethod(CTX, kcid_t cid, kmethodn_t mn, ktype_t type, int idx)
{
	knh_Fmethod f = (TY_isUnbox(type)) ? Fmethod_ngetter : Fmethod_getter;
	kParam *pa = new_kParam(type, 0, NULL);
	kMethod *mtd = new_kMethod(0, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = idx;  // FIXME
	return mtd;
}

static kMethod *new_SetterMethod(CTX, kcid_t cid, kmethodn_t mn, ktype_t type, int idx)
{
	knh_Fmethod f = (TY_isUnbox(type)) ? Fmethod_nsetter : Fmethod_setter;
	kparam_t p = {type, FN_("x")};
	kParam *pa = new_kParam(type, 1, &p);
	kMethod *mtd = new_kMethod(0, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = idx;   // FIXME
	return mtd;
}

static intptr_t Method_indexOfGetterSetterField(kMethod *mtd)
{
	knh_Fmethod f = mtd->fcall_1;
	if(f== Fmethod_getter || f == Fmethod_ngetter || f == Fmethod_setter || f == Fmethod_nsetter) {
		return (intptr_t)mtd->delta;
	}
	return -1;
}

//static KMETHOD Fmethod_ngetterP(CTX, ksfp_t *sfp _RIX)
//{
//	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
//	ksymbol_t key = (ksymbol_t)mtd->delta;
//	uintptr_t u = kObject_getNDATA(sfp[0].o, key);
//	RETURNd_(v);
//}
//static KMETHOD Fmethod_setterP(CTX, ksfp_t *sfp _RIX)
//{
//	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
//	ksymbol_t key = (ksymbol_t)mtd->delta;
//	kObject_setObject(sfp[0].o, key, sfp[1].o);
//	RETURN_(sfp[1].o);
//}
//static KMETHOD Fmethod_nsetterP(CTX, ksfp_t *sfp _RIX)
//{
//	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
//	ksymbol_t key = (ksymbol_t)mtd->delta;
//	kObject_setDATA(sfp[0].o, key, sfp[1].ndata);
//	RETURNd_(sfp[1].ndata);
//}

static intptr_t CT_indexOfClassField(kclass_t *ct, ksymbol_t fn)
{
	intptr_t i = ct->fsize;
	for(i = ct->fsize - 1; i >=0; i--) {
		if(ct->fields[i].fn == fn) break;
	}
	return i;
}

// --------------------------------------------------------------------------

// int KonohaSpace.getCid(String name, int defval)
static KMETHOD KonohaSpace_getCid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(kKonohaSpace_getcid(sfp[0].ks, S_text(sfp[1].s), S_size(sfp[1].s), (kcid_t)sfp[2].ivalue));
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
	cdef.packid = ks->packid;
	cdef.packdom = ks->packdom;
	if(cdef.supcid == 0) cdef.supcid = TY_Object;
	kclass_t *supct = kclass(cdef.supcid, sfp[K_RTNIDX].uline);
	if(CT_isFinal(supct)) {
		kreportf(ERR_, sfp[K_RTNIDX].uline, "%s is @Final", T_cid(cdef.supcid));
		kraise(0);
	}
	kclass_t *c = kaddClassDef(sfp[2].s, &cdef, sfp[K_RTNIDX].uline);
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
	if(!CT_iS_UNDEF(ct) || !(ct->fsize < ct->fallocsize)) {
		kreportf(ERR_, sfp[K_RTNIDX].uline, "all fields are defined: %s", T_cid(ct->cid));
		kraise(0);
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
	if(!ct->fsize < ct->fallocsize) {
		DBG_P("all fields are set");
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
		_Public, _F(KonohaSpace_defineClassField), TY_Int, TY_KonohaSpace, MN_("defineClassField"), 4, TY_Int, FN_cid, TY_Int, FN_("type"), TY_String, FN_name, TY_Object, FN_defval,
		DEND,
	};
	kloadMethodData(ks, MethodData);
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

KDEFINE_PACKAGE* class_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("class", "1.0"),
		.initPackage = class_initPackage,
		.setupPackage = class_setupPackage,
		.initKonohaSpace = class_initKonohaSpace,
		.setupKonohaSpace = class_setupKonohaSpace,
	};
	return &d;
}
