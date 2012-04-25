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

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [CONST/PROPERTY DATA] */

static void loadData(CTX, kKonohaSpace *ns, const char *dname, Object *value)
{
	if(dname[0] == '$') {
		kString *n = new_T(dname + 1);
		knh_DictMap_set_(_ctx, ctx->share->props, n, value);
	}
	else {
		kbytes_t n = {{dname}, knh_strlen(dname)};
		kindex_t loc = knh_bytes_rindex(n, '.');
		kString *name = new_T(dname + (loc+1));
		kcid_t cid = CLASS_Tdynamic;
		if(loc != -1) {
			cid = knh_KonohaSpace_getCT(_ctx, ns, knh_bytes_first(n, loc));
			if(cid == TY_unknown) {
				KNH_LOG("unknown class constant: %s", dname);
				cid = CLASS_Tdynamic;
			}
		}
		knh_addClassConst(_ctx, cid, name, value);
	}
}

static void loadIntData(CTX, kKonohaSpace *ns, const knh_IntData_t *data)
{
	while(data->name != NULL) {
		Object *value = UPCAST(new_Int(_ctx, data->ivalue));
		loadData(_ctx, ns, data->name, value);
		data++;
	}
}

static void loadFloatData(CTX, kKonohaSpace *ns, const knh_FloatData_t *data)
{
	while(data->name != NULL) {
		Object *value = UPCAST(new_Float(_ctx, data->fvalue));
		loadData(_ctx, ns, data->name, value);
		data++;
	}
}

static void loadStringData(CTX, kKonohaSpace *ns, const knh_StringData_t *data)
{
	while(data->name != NULL) {
		Object *value = UPCAST(new_kString(data->value, knh_strlen(data->value), SPOL_TEXT|SPOL_ASCII));
		loadData(_ctx, ns, data->name, value);
		data++;
	}
}

static void loadClassIntConst(CTX, kcid_t cid, const knh_IntData_t *data)
{
	while(data->name != NULL) {
		Object *value = UPCAST(new_Int(_ctx, data->ivalue));
		knh_addClassConst(_ctx, cid, new_T(data->name), value);
		data++;
	}
}

static void loadClassFloatConst(CTX, kcid_t cid, const knh_FloatData_t *data)
{
	while(data->name != NULL) {
		Object *value = UPCAST(new_Float(_ctx, data->fvalue));
		knh_addClassConst(_ctx, cid, new_T(data->name), value);
		data++;
	}
}

static void loadStringClassConst(CTX, kcid_t cid, const knh_StringData_t *data)
{
	while(data->name != NULL) {
		Object *value = UPCAST(new_T(data->value));
		knh_addClassConst(_ctx, cid, new_kString(data->name, knh_strlen(data->name), SPOL_TEXT), value);
		data++;
	}
}

static void setProperty(CTX, const char *name, const char *data)
{
	char pname[256];
	const char *nsn = S_text(DP(_ctx->gma->scr->ns)->nsname);
	knh_snprintf(pname, sizeof(pname), "%s.%s", nsn, name);
	knh_DictMap_set(_ctx, ctx->share->props, new_String(_ctx, pname), new_T(data));
}

static void setIntProperty(CTX, const char *name, kint_t data)
{
	char pname[256];
	const char *nsn = S_text(DP(_ctx->gma->scr->ns)->nsname);
	knh_snprintf(pname, sizeof(pname), "%s.%s", nsn, name);
	knh_DictMap_set(_ctx, ctx->share->props, new_String(_ctx, pname), new_Int(_ctx, data));
}

static void setFloatProperty(CTX, const char *name, kfloat_t data)
{
	char pname[256];
	const char *nsn = S_text(DP(_ctx->gma->scr->ns)->nsname);
	knh_snprintf(pname, sizeof(pname), "%s.%s", nsn, name);
	knh_DictMap_set(_ctx, ctx->share->props, new_String(_ctx, pname), new_Float(_ctx, data));
}


/* ------------------------------------------------------------------------ */

#define _MAX 1024

static kbytes_t kloaddata_tobytes(kloaddata_t data)
{
	char *p = (char*)data;
	return B(p);
}

static kParam *knh_loadScriptParam(CTX, const kloaddata_t **d, uintptr_t uflag, int step)
{
	kParam *pa = new_Param(_ctx);
	const kloaddata_t *data = (*d) + step;
	long i, psize = (long)data[0];
	long rsize = (long)data[1];
	data += 2;
	for(i = 0; i < psize+rsize; i++) {
		ktype_t type = (data[0] < _MAX || (T_This <= data[0] && data[0] <= T_T3)) ?
			(ktype_t)data[0] : knh_KonohaSpace_gettype(_ctx, K_GMANS, kloaddata_tobytes(data[0]));
		ksymbol_t fn = (data[1] < _MAX) ?
			(ksymbol_t)data[1] : knh_getfnq(_ctx, kloaddata_tobytes(data[1]), FN_NEWID);
		kparam_t p = {type, fn};
		knh_Param_add(_ctx, pa, p);
		data += 2;
	}
	pa->psize = (kushort_t)psize;
	pa->rsize = (kushort_t)rsize;
	*d = data;
	pa->h.magicflag |= uflag;
	return pa;
}

#define _CID(d)  (d < _MAX) ? (kcid_t)(d) : knh_KonohaSpace_getCT(_ctx, K_GMANS, kloaddata_tobytes(d))
#define _EXPTID(d)  (d < _MAX) ? (kevent_t)(d) : knh_geteid(_ctx, kloaddata_tobytes(d))

static void knh_loadSystemData(CTX, const kloaddata_t *data, kParam **buf)
{
	size_t c = 0;
	while(1) {
		long datatype = (long)data[0]; data++;
		switch(datatype) {
		case DATA_END: return;
		case DATA_STRUCT0: {
			kcid_t cid0 = _CID(data[0]);
			kclass_t *cspi = (struct _kclass*)data[1];
			kflag_t cflag = (kflag_t)data[2];
			data += 3;
			kcid_t cid = new_ClassId(_ctx);
			kclass_t *t = varClassTBL(cid);
			KNH_ASSERT(cid == cid0);
			t->bcid = cid;
			knh_setClassDef(_ctx, t, cspi);
			t->cflag = cflag;
			t->magicflag = KNH_MAGICFLAG(cflag);
			break;
		}
		//{"Object", FLAG_Object, CLASS_ObjectField, CLASS_Object, a},
		case DATA_CLASS0: {
			kcid_t cid0 = _CID(data[0]);
			char *name = (char*)data[1];
			kclass_t *ct = varClassTBL(cid0);
			if(ct == NULL) {
				kcid_t cid = new_ClassId(_ctx);
				ct = varClassTBL(cid);
				KNH_ASSERT(cid == cid0);
			}
			ct->cflag = (kflag_t)data[2];
			ct->magicflag = KNH_MAGICFLAG(ct->cflag);
			ct->bcid = _CID(data[3]);
			ct->baseTBL = ClassTBL(ct->bcid);
			if(cid0 != ct->bcid) {
				knh_setClassDef(_ctx, ct, ClassTBL(ct->bcid)->cdef);
			}
			DBG_ASSERT(ct->cdef != NULL);
			ct->supcid = _CID(data[4]);
			ct->supTBL = ClassTBL(ct->supcid);
			knh_setClassName(_ctx, cid0, new_T(name), NULL);
			DBG_ASSERT(ct->methods == NULL);
			if(data[5] > 0/* || cid0 == CLASS_Tvoid*/) {
				KINITv(ct->methods, new_Array0(_ctx, data[5]));
			}
			else {
				KINITv(ct->methods, K_EMPTYARRAY);
			}
			if(ct->typemaps == NULL) {
				KINITv(ct->typemaps, K_EMPTYARRAY);
			}
			data += 6;
			break;
		}
		case DATA_CPARAM: {
			kcid_t cid = _CID(data[0]);
			const char *lname = (const char*)data[1];
			//const char *sname = (const char*)data[2];
			kclass_t *ct = varClassTBL(cid);
			kParam *pa = knh_loadScriptParam(_ctx, &data, 0/*hflag*/, +3);
			knh_setClassParam(_ctx, ct, pa);
			KSETv(ct->lname, new_T(lname));
			//KSETv(ct->sname, new_T(sname));
			break;
		}
		case DATA_GENERICS: {
			kcid_t bcid = _CID(data[0]);
			kParam *mp = knh_loadScriptParam(_ctx, &data, 0/*hflag*/, +1);
			knh_addGenericsClass(_ctx, CLASS_newid, bcid, mp);
			break;
		}
		case DATA_EXPT: {
			char *name = (char*)data[0];
			kflag_t flag = (kflag_t)data[1];
//			kevent_t eid = _EXPTID(data[2]);
			kevent_t pid = _EXPTID(data[3]);
			knh_addEvent(_ctx, flag, new_T(name), pid);
			data += 4;
			break;
		}
		//{Int_opPREV, FLAG_Method_Const, CLASS_Int, MN_opPREV, 0, 20, NULL},
		//{cid, mn, flag, func, mpidx, psize, rsize}
		case DATA_METHOD0:
		case DATA_METHOD : {
			kcid_t cid = _CID(data[0]);
			kmethodn_t mn = (datatype == DATA_METHOD)
				? knh_getmn(_ctx, kloaddata_tobytes(data[1]), MN_NEWID) : (kmethodn_t)data[1];
			kflag_t flag = (kflag_t)data[2];
			knh_Fmethod func = (knh_Fmethod)data[3];
			kMethod *mtd;
			if(class_isSingleton(cid)) {
				flag = flag | FLAG_Method_Static;
			}
			mtd = new_Method(_ctx, flag, cid, mn, func);
			knh_ClassTBL_addMethod(_ctx, ClassTBL(cid), mtd, 0);
			data += 4;
			if(datatype == DATA_METHOD0) {
				KSETv(DP(mtd)->mp, buf[data[0]]);
				DBG_ASSERT(IS_Param(DP(mtd)->mp));
				data++;
			}
			else {
				kParam *mp = knh_loadScriptParam(_ctx, &data, 0/*hflag*/, +0);
				KSETv(DP(mtd)->mp, mp);
			}
			break;
		}
		case DATA_TYPEMAP: { //{scid, tcid, flag, func},
			kcid_t scid = _CID(data[0]);
			kcid_t tcid = _CID(data[1]);
			kflag_t  flag = (kflag_t)data[2];
			knh_Ftypemap func = (knh_Ftypemap)data[3];
			knh_addTypeMapFunc(_ctx, flag, scid, tcid, func, K_NULL);
			data += 4;
			break;
		}
		case DATA_PARAM: {
			uintptr_t flag = (uintptr_t)data[0];
			data++;
			buf[c] = knh_loadScriptParam(_ctx, &data, flag, +0);
			c++;
			break;
		}/*case DATA_PARAM*/
		default :
			KNH_DIE("unknown datatype=%d", (int)datatype);
		}/*switch*/
	}
}

static void knh_loadFuncData(CTX, const knh_FuncData_t *d)
{
	kDictSet *ds = ctx->share->funcDictSet;
	while(d->name != NULL) {
		const char *name = d->name;
		if(name[0] == '_') name = name + 1;
		knh_DictSet_append(_ctx, ds, new_kString(name, knh_strlen(name), SPOL_TEXT|SPOL_POOLNEVER), (uintptr_t)d->ptr);
		d++;
	}
	knh_DictSet_sort(_ctx, ds);
}

/* ------------------------------------------------------------------------ */

//static void addLink(CTX, kKonohaSpace *ns, const char *scheme, kcid_t cid)
//{
//	knh_KonohaSpace_setLinkClass(_ctx, ns, B(scheme), ClassTBL(cid));
//}

static void knh_addStreamDPI(CTX, const char *scheme, const knh_PathDPI_t *d)
{
	knh_DictSet_set(_ctx, ctx->share->streamDpiDictSet, new_T(scheme), (uintptr_t)d);
}

//static void knh_addQueryDPI(CTX, const char *scheme, const knh_QueryDPI_t *d)
//{
//	knh_DictSet_set(_ctx, ctx->share->queryDpiDictSet, new_T(scheme), (uintptr_t)d);
//}

static void knh_addMapDPI(CTX, const char *scheme, const knh_MapDPI_t *d)
{
	knh_DictSet_set(_ctx, ctx->share->mapDpiDictSet, new_T(scheme), (uintptr_t)d);
}

static void knh_addConvDPI(CTX, const char *scheme, const knh_ConverterDPI_t *conv, const knh_ConverterDPI_t *rconv)
{
	knh_DictSet_set(_ctx, ctx->share->convDpiDictSet, new_T(scheme), (uintptr_t)conv);
	if(rconv != NULL) {
		knh_DictSet_set(_ctx, ctx->share->convDpiDictSet, new_T(scheme), (uintptr_t)rconv);
	}
}

const knh_LoaderAPI_t* knh_getLoaderAPI(void)
{
	static knh_LoaderAPI_t pkgapi = {
		knh_loadSystemData, knh_loadFuncData,
		loadIntData, loadFloatData, loadStringData,
		loadClassIntConst, loadClassFloatConst, loadStringClassConst,
		setProperty, setIntProperty, setFloatProperty,
//		addLink,
		knh_addStreamDPI,
//		knh_addQueryDPI,
		knh_addMapDPI,
		knh_addConvDPI,
	};
	return &pkgapi;
}

void knh_initBuiltInPackage(CTX, const knh_LoaderAPI_t *kapi)
{
	knh_initClassFuncData(_ctx, kapi);
	knh_initStreamFuncData(_ctx, kapi);
	knh_initSugarFuncData(_ctx, kapi);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
