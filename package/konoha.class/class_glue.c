#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

// --------------------------------------------------------------------------

//const char *structname;
//kcid_t     cid;         kflag_t    cflag;
//kcid_t     bcid;        kcid_t     supcid;
//size_t     cstruct_size;
//kfield_t   *fields;
//kushort_t  fsize;        kushort_t fallocsize;
//KCLASSSPI;

//static KMETHOD Fmethod_getter(CTX, ksfp_t *sfp _RIX)
//{
//	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
//	RETURN_((sfp[0].ox)->fields[delta]);
//}
//static KMETHOD Fmethod_ngetter(CTX, ksfp_t *sfp _RIX)
//{
//	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
//	kunbox_t *data = (kunbox_t*)(&(sfp[0].ox->fields[delta]));
//	RETURNd_(data[0]);
//}
//static KMETHOD Fmethod_setter(CTX, ksfp_t *sfp _RIX)
//{
//	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
//	KSETv((sfp[0].ox)->fields[delta], sfp[1].o);
//	RETURN_(sfp[1].o);
//}
//static KMETHOD Fmethod_nsetter(CTX, ksfp_t *sfp _RIX)
//{
//	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
//	kunbox_t *ndata = (kunbox_t*)(&((sfp[0].ox)->fields[delta]));
//	ndata[0] = sfp[1].ndata;
//	RETURNd_(ndata[0]);
//}
//static KMETHOD Fmethod_kgetter(CTX, ksfp_t *sfp _RIX)
//{
//	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
//	RETURN_((sfp[0].p)->kfields[delta]);
//}
//static KMETHOD Fmethod_kngetter(CTX, ksfp_t *sfp _RIX)
//{
//	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
//	kunbox_t *data = (kunbox_t*)(&(sfp[0].p->kfields[delta]));
//	RETURNd_(data[0]);
//}
//static KMETHOD Fmethod_ksetter(CTX, ksfp_t *sfp _RIX)
//{
//	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
//	KSETv((sfp[0].p)->kfields[delta], sfp[1].o);
//	RETURN_(sfp[1].o);
//}
//static KMETHOD Fmethod_knsetter(CTX, ksfp_t *sfp _RIX)
//{
//	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
//	kunbox_t *ndata = (kunbox_t*)(&((sfp[0].p)->kfields[delta]));
//	ndata[0] = sfp[1].ndata;
//	RETURNd_(ndata[0]);
//}
//
//static knh_Fmethod accessors[8] = {
//	Fmethod_getter, Fmethod_setter, Fmethod_ngetter, Fmethod_nsetter,
//	Fmethod_kgetter, Fmethod_ksetter, Fmethod_kngetter, Fmethod_knsetter,
//};
//
//#define _SETTER  1
//#define _NDATA   2
//#define _CPPOBJ  4
//
//static kMethod *new_GetterMethod(CTX, kcid_t cid, kmethodn_t mn, ktype_t type, int idx)
//{
//	//knh_Fmethod f = (TY_isUnbox(type)) ? Fmethod_ngetter : Fmethod_getter;
//	knh_Fmethod f = accessors[(TY_isUnbox(type)?_NDATA:0)|((ClassTBL(cid)->bcid==CLASS_CppObject)?_CPPOBJ:0)];
//	kMethod *mtd = new_Method(_ctx, 0, cid, mn, f);
//	DP(mtd)->delta = idx;
//	KSETv(DP(mtd)->mp, new_ParamR0(_ctx, type));
//	return mtd;
//}
//
//static kMethod *new_SetterMethod(CTX, kcid_t cid, kmethodn_t mn, ktype_t type, int idx)
//{
//	//knh_Fmethod f = (TY_isUnbox(type)) ? Fmethod_nsetter : Fmethod_setter;
//	knh_Fmethod f = accessors[_SETTER|(TY_isUnbox(type)?_NDATA:0)|((ClassTBL(cid)->bcid==CLASS_CppObject)?_CPPOBJ:0)];
//	kMethod *mtd = new_Method(_ctx, 0, cid, mn, f);
//	DP(mtd)->delta = idx;
//	KSETv(DP(mtd)->mp, new_ParamP1(_ctx, RT_set(type), type, FN_UNMASK(mn)));
//	return mtd;
//}
//
//kindex_t knh_Method_indexOfGetterField(kMethod *o)
//{
//	knh_Fmethod f = SP(o)->fcall_1;
//	if(f== Fmethod_getter || f == Fmethod_ngetter) {
//		return (kindex_t)DP(o)->delta;
//	}
//	return -1;
//}
//
//kindex_t knh_Method_indexOfSetterField(kMethod *o)
//{
//	knh_Fmethod f = SP(o)->fcall_1;
//	if(f == Fmethod_setter || f == Fmethod_nsetter) {
//		return (kindex_t)DP(o)->delta;
//	}
//	return -1;
//}

// --------------------------------------------------------------------------

// int Lingo.getCid(String name, int defval)
static KMETHOD Lingo_getCid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(kLingo_getcid(sfp[0].lgo, S_text(sfp[1].s), S_size(sfp[1].s), (kcid_t)sfp[2].ivalue));
}

// int Lingo.defineClass(int flag, String name, int supcid, int fieldsize);
static KMETHOD Lingo_defineClass(CTX, ksfp_t *sfp _RIX)
{
	kLingo *lgo = sfp[0].lgo;
	KCLASSDEF cdef = {};
	cdef.cstruct_size = sfp[4].ivalue * sizeof(void*);
	cdef.cflag  = (kflag_t)sfp[1].ivalue | ((cdef.cstruct_size > 0) ? kClass_UNDEF : 0);
	cdef.cid    = CLASS_newid;
	cdef.bcid   = CLASS_Object;
	cdef.supcid = (kcid_t)sfp[3].ivalue;
	cdef.packid = lgo->packid;
	cdef.packdom = lgo->packdom;
	if(cdef.supcid == 0) cdef.supcid = TY_Object;
	const kclass_t *supct = kclass(cdef.supcid, sfp[K_RTNIDX].uline);
	if(CT_isFinal(supct)) {
		kreportf(ERR_, sfp[K_RTNIDX].uline, "%s is @Final", T_cid(cdef.supcid));
		kraise(0);
	}
	const kclass_t *c = kaddClassDef(sfp[2].s, &cdef, sfp[K_RTNIDX].uline);
	RETURNi_(c->cid);
}

// int Lingo.defineClassField(int cid, int ty, String name, Object *value);
static KMETHOD Lingo_defineClassField(CTX, ksfp_t *sfp _RIX)
{
	kclass_t *ct = (kclass_t*)kclass((kcid_t)sfp[1].ivalue, sfp[K_RTNIDX].uline);
	if(!CT_isUNDEF(ct) || !(ct->fsize < ct->fallocsize)) {
		kreportf(ERR_, sfp[K_RTNIDX].uline, "all fields are defined: %s", T_cid(ct->cid));
		kraise(0);
	}
	int pos = ct->fsize;
	ktype_t ty = (ktype_t)sfp[2].ivalue;
	ct->fsize += 1;
	ct->fields[pos].ty = ty;
	ct->fields[pos].fn = ksymbol(S_text(sfp[3].s), S_size(sfp[3].s), FN_NEWID, SYMPOL_NAME);
	if(TY_isUnbox(ty)) {
		ct->nulvalNUL->ndata[pos] = O_unbox(sfp[4].o);
	}
	else {
		kObject *v = (IS_NULL(sfp[4].o)) ? knull(O_ct(sfp[4].o)) : sfp[4].o;
		KSETv(ct->nulvalNUL->ndata[pos], v);
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

static	kbool_t class_initPackage(CTX, struct kLingo *lgo, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	int FN_flag = FN_("flag"), FN_cid = FN_("cid"), FN_name = FN_("name"), FN_defval = FN_("defval");
	intptr_t methoddata[] = {
		_Public, _F(Lingo_getCid), TY_Int, TY_Lingo, MN_("cid"), 2, TY_String, FN_name, TY_Int, FN_defval,
		_Public, _F(Lingo_defineClass), TY_Int, TY_Lingo, MN_("defineClass"), 4, TY_Int, FN_flag, TY_String, FN_name, TY_Int, FN_("supcid"), TY_Int, FN_("fieldSize"),
		_Public, _F(Lingo_defineClassField), TY_Int, TY_Lingo, MN_("defineClassField"), 4, TY_Int, FN_cid, TY_Int, FN_("type"), TY_String, FN_name, TY_Object, FN_defval,
		DEND,
	};
	kloadMethodData(lgo, methoddata);
	return true;
}

static kbool_t class_setupPackage(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t class_initLingo(CTX,  struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t class_setupLingo(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* class_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("class", "1.0"),
		.initPackage = class_initPackage,
		.setupPackage = class_setupPackage,
		.initLingo = class_initLingo,
		.setupPackage = class_setupLingo,
	};
	return &d;
}
