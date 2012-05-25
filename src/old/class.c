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
#include"../../include/konoha2/konohalang.h"

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [object] */

int knh_Object_compareTo(Object *o1, Object *o2)
{
	kcid_t cid1 = O_cid(o1);
	kcid_t cid2 = O_cid(o2);
	int res;
	if(cid1 == cid2) {
		res = O_ct(o1)->cdef->compareTo(RAWPTR(o1), RAWPTR(o2));
	}
	else {
		res = (int)(o1 - o2);
		DBG_P("compared %s %s res=%d", O__(o1), O__(o2), res);
	}
	return res;
}

KNHAPI2(void) kObjectoNULL_(CTX, Object *o)
{
	Object_setNullObject(o, 1);
}

/* ------------------------------------------------------------------------ */

kbool_t knh_invokeMethod0(CTX, Object *o, kKonohaSpace *ns, unsigned long mnd, void **retval, ...)
{
	kmethodn_t mn = mnd < 10000 ? (kmethodn_t)mnd : knh_getmn(_ctx, B((char*)mnd), MN_NONAME);
	kMethod *mtd = knh_KonohaSpace_getMethodNULL(_ctx, ns, O_cid(o), mn);
	if(mtd != NULL) {
		BEGIN_LOCAL(_ctx, lsfp, 0);
		size_t i, psize = knh_Method_psize(mtd), rtnidx = 0, thisidx = rtnidx + K_CALLDELTA;
		kcid_t this_cid = O_cid(o);
		va_list ap;
		va_start(ap, retval);
		KSETv(lsfp[thisidx].o, o);
		for(i = 0; i < psize; i++) {
			ktype_t ptype = knh_Method_ptype(_ctx, mtd, i, this_cid);
			switch(ptype) {
				case CLASS_Boolean: {
					lsfp[thisidx+1+i].bvalue = va_arg(ap, int);
					break;
				}
				case CLASS_Int : {
					lsfp[thisidx+1+i].ivalue = va_arg(ap, long);
					break;
				}
				case CLASS_Float : {
					lsfp[thisidx+1+i].ivalue = va_arg(ap, double);
					break;
				}
				case CLASS_String :  {
					const char *s = va_arg(ap, const char*);
					if(s == NULL) {
						KSETv(lsfp[thisidx+1+i].o, KNH_TNULL(String));
					}
					else if(knh_isObject(_ctx, (void*)s)) {
						DBG_ASSERT(IS_bString((Object*)s));
						KSETv(lsfp[thisidx+1+i].o, (Object*)s);
					}
					else {
						KSETv(lsfp[thisidx+1+i].o, new_String(_ctx, s));
					}
					break;
				}
				default : {
					Object *v = va_arg(ap, Object *);
					if(v == NULL) {
						v = KNH_NULVAL(ptype);
					}
					else {
						DBG_ASSERT(O_cid(v) == ptype || ClassTBL_isa_(_ctx, O_ct(v), ClassTBL(ptype)));
					}
					KSETv(lsfp[thisidx+1+i].o, v);
				}
			}/*switch*/
		}/*for*/
		va_end(ap);
		KNH_SCALL(_ctx, lsfp, rtnidx, mtd, psize);
		if(retval != NULL) {
			ktype_t rtype = knh_Method_rtype(_ctx, mtd, this_cid);
			switch(rtype) {
				case CLASS_Tvoid: retval[0] = NULL; break;
				case CLASS_Boolean: ((kbool_t*)retval)[0] = lsfp[rtnidx].bvalue; break;
				case CLASS_Int:     ((kint_t*)retval)[0] = lsfp[rtnidx].ivalue; break;
				case CLASS_Float:   ((kfloat_t*)retval)[0] = lsfp[rtnidx].fvalue; break;
				default: retval[0] = (void*)lsfp[rtnidx].o;
			}
		}
		END_LOCAL(_ctx, lsfp);
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

static void nofree(void *p)
{
}

KNHAPI2(kObject*) new_RawPtr(CTX, kclass_t *ct, void *rawptr)
{
	kObject *npo = (kObject*)new_hObject_(_ctx, ct);
	npo->rawptr = rawptr;
	if(rawptr == NULL) {
		kObjectoNULL(_ctx, npo);
	}
	npo->rawfree = nofree;
	return npo;
}

KNHAPI2(kObject*) new_ReturnObject(CTX, ksfp_t *sfp)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ktype_t rtype = knh_Param_rtype(DP(mtd)->mp);
	return new_Object_init2(_ctx, ClassTBL(rtype));
}

KNHAPI2(kObject*) new_ReturnCppObject(CTX, ksfp_t *sfp, void *rawptr, knh_Frawfree pfree)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ktype_t rtype = knh_Param_rtype(DP(mtd)->mp);
	kObject *npo = (kObject*)new_hObject_(_ctx, ClassTBL(rtype));
	npo->rawptr = rawptr;
	if(rawptr == NULL) {
		kObjectoNULL(_ctx, npo);
	}
	else {
		npo->rawfree = (pfree != NULL) ? pfree : nofree;
	}
	return npo;
}

kObject *new_Pointer(CTX, const char *name, void *rawptr, knh_Frawfree pfree)
{
	kObject *npo = (kObject*)new_hObject_(_ctx, ClassTBL(CLASS_Tdynamic));
	npo->rawptr = rawptr;
	npo->DBG_NAME = name;
	npo->rawfree = (pfree != NULL) ? pfree : nofree;
	return npo;
}

KNHAPI2(void) knh_addConstPool(CTX, kObject *o)
{
	kArray_add(ctx->share->constPools, o);
}

/* ------------------------------------------------------------------------ */
/* [ClassTBL] */

const char *SAFESTRUCT__(CTX, kcid_t bcid)
{
	if(bcid < ctx->share->sizeClassTBL) {
		return ClassTBL(bcid)->cdef->name;
	}
	KNH_P("unknown bcid=%d", bcid);
	return "UNKNOWN";
}

const char *SAFET__(CTX, ktype_t type)
{
	kcid_t cid = CLASS_t(type);
	if(type == TY_void) return "void";
	if(type == TY_var)  return "var";
	if(cid < ctx->share->sizeClassTBL) {
		return S_text(ClassTBL(cid)->sname);
	}
	else {
		static char unsafebuf[20];
		knh_snprintf(unsafebuf, sizeof(unsafebuf), "type=%d", (int)type);
		return (const char*)unsafebuf;
	}
}

const char *SAFECLASS__(CTX, kcid_t cid)
{
	if(cid < ctx->share->sizeClassTBL) {
		return S_text(ClassTBL(cid)->lname);
	}
	else {
		return "unknown";
	}
}

#ifdef K_USING_DEBUG
kclass_t* DBG_ClassTBL(CTX, kcid_t cid)
{
	if(cid < ctx->share->sizeClassTBL) {
		return ctx->share->ClassTBL[cid];
	}
	DBG_ABORT("NOT CORRECT: cid=%d", cid);
	return NULL;
}
#endif

/* ------------------------------------------------------------------------ */

kClass *new_Type(CTX, ktype_t type)
{
	kcid_t cid = CLASS_t(type);
	kclass_t *ct = ClassTBL(cid);
	if(ct->typeNULL == NULL) {
		kClass *o = new_(Class);
		o->cid = cid;
		o->type = cid;
		o->ct = ct;
		KINITv(((struct _kclass*)ct)->typeNULL, o);
		if(cid == CLASS_Tvoid) {
			Object_setNullObject(o, 1);
		}
	}
	return ct->typeNULL;
}

KNHAPI2(void) knh_write_cid(CTX, kOutputStream *w, kcid_t cid)
{
	const char *tname = NULL;
	switch(cid) {
	case TY_dynamic:  tname = "dynamic";    break;
	case TY_void: tname = "void";       break;
	case TY_var:  tname = "var";        break;
	case T_This: tname = "This";       break;
	default :{
			if(cid > T_This) {
				knh_write(_ctx, w, STEXT("T"));
				knh_write_ifmt(_ctx, w, KINT_FMT, cid - T_This);
			}
			else {
				knh_write(_ctx, w, S_tobytes(ClassTBL(cid)->lname));
			}
			return ;
		}
	}
	knh_write_ascii(_ctx, w, tname);
}

void knh_write_cname(CTX, kOutputStream *w, kcid_t cid)
{
	const char *tname = NULL;
	switch(cid) {
	case TY_dynamic:  tname = "dynamic";    break;
	case TY_void: tname = "void";       break;
	case TY_var:  tname = "var";        break;
	case T_This: tname = "This";       break;
	default :{
			if(cid > T_This) {
				knh_write(_ctx, w, STEXT("T"));
				knh_write_ifmt(_ctx, w, KINT_FMT, cid - T_This);
			}
			else {
				knh_write(_ctx, w, S_tobytes(ClassTBL(cid)->sname));
			}
			return ;
		}
	}
	knh_write_ascii(_ctx, w, tname);
}

KNHAPI2(void) knh_write_type(CTX, kOutputStream *w, ktype_t type)
{
	const char *tname = NULL;
	if(type > T_This) {
		knh_write(_ctx, w, STEXT("T"));
		knh_write_ifmt(_ctx, w, KINT_FMT, type - T_This);
		return;
	}
	else if(type == T_This) {
		knh_write_ascii(_ctx, w, "This");
		return;
	}
	switch(type) {
	case TY_Boolean: tname = "boolean"; break;
	case TY_Int: tname = "int"; break;
	case T_Float: tname = "float"; break;
	case T_Bytes:  tname = "byte[]";    break;
	case TY_Array: tname = "dynamic" PTY_Array;  break;
	case T_Iterator:  tname = "dynamic" PT_Iterator;  break;
	case T_KindOf: tname = "dynamic" PT_KindOf; break;
	case T_Immutable: tname = "dynamic" PT_Immutable; break;
	default :
		tname = NULL;
	}
	if(tname != NULL) {
		knh_write_ascii(_ctx, w, tname);
	}
	else {
		knh_write(_ctx, w, S_tobytes(ClassTBL(CLASS_t(type))->sname));
	}
}

/* ------------------------------------------------------------------------ */

static kObject *Fdefnull_newValue(CTX, kcid_t cid);

void knh_setClassName(CTX, kcid_t cid, kString *lname, kString *snameNULL)
{
	DBG_ASSERT_cid(cid);
	kclass_t *ct = varClassTBL(cid);
	DBG_ASSERT(ct->lname == NULL);
	KINITv(ct->lname, lname);
	if(snameNULL != NULL) {
		KINITv(ct->sname, snameNULL);
	}
	else if(ct->bcid == cid) {
		KINITv(ct->sname, new_T(ct->cdef->name));
	}
	if(ct->sname == NULL) {
		KINITv(ct->sname, ct->lname);
	}
	OLD_LOCK(_ctx, LOCK_SYSTBL, NULL);
	{
		kbytes_t t = S_tobytes(lname);
		if(t.buf[t.len-1] != '>') {
			knh_DictSet_set(_ctx, ctx->share->classNameDictSet, lname, (uintptr_t)ct);
		}
		if(!class_isPrivate(cid) && S_startsWith(lname, STEXT("konoha."))) {
			knh_DictSet_append(_ctx, ctx->share->classNameDictSet, ct->sname, (uintptr_t)ct);
		}
	}
	OLD_UNLOCK(_ctx, LOCK_SYSTBL, NULL);
	if(ct->fdefnull == NULL) {
		ct->fdefnull = Fdefnull_newValue;
	}
}

/* ------------------------------------------------------------------------ */
/* [name] */

kcid_t knh_getcid(CTX, kbytes_t lname)
{
	kclass_t *ct = (kclass_t *)knh_DictSet_get(_ctx, ctx->share->classNameDictSet, lname);
	if(ct != NULL) return ct->cid;
#if defined(K_USING_SEMANTICS)
	if(lname.buf[lname.len-1] == '}') {
		return knh_findcidx(_ctx, lname);
	}
#endif/*K_USING_SEMANTICS*/
	return TY_unknown;
}

/* ------------------------------------------------------------------------ */
/* [default] */

static kObject *Fdefnull_getConst(CTX, kcid_t cid)
{
	return ClassTBL(cid)->defnull;
}

static kObject *Fdefnull_newValue(CTX, kcid_t cid)
{
	kclass_t *ct = varClassTBL(cid);
	Object *v = new_Object_init2(_ctx, (kclass_t*)ct);
	Object_setNullObject(v, 1);
	KNH_ASSERT(ct->defnull == NULL);
	KINITv(ct->defnull, v);
	ct->fdefnull = Fdefnull_getConst;
	DBG_P("create new default value of %s", CLASS__(cid));
	DBG_ASSERT(cid != CLASS_Class);  // for DEBUG
	return ct->defnull;
}

void knh_setClassDefaultValue_(CTX, kcid_t cid, Object *value, knh_Fdefnull f)
{
	DBG_ASSERT_cid(cid);
	kclass_t *t = varClassTBL(cid);
	if(value == NULL) {
		if(f == NULL) f = Fdefnull_newValue;
	}
	else {
		KNH_ASSERT(t->defnull == NULL);
		KINITv(t->defnull, value);
		if(f == NULL) f = Fdefnull_getConst;
	}
	t->fdefnull = f;
}

KNHAPI2(Object*) knh_getClassDefaultValue(CTX, kcid_t cid)
{
	return ClassTBL(cid)->fdefnull(_ctx, cid);
}

void knh_setClassDef(CTX, kclass_t *ct, kclass_t *cdef)
{
	//DBG_P("setClassDef(%s)", cdef->name);
	ct->cdef = cdef;
	if(cdef->fields != NULL) {
		KNH_ASSERT(ct->fields == NULL);
		KWRITE_BARRIER(ct, cdef->fields);
		ct->fields = cdef->fields;
		ct->fsize = cdef->struct_size / sizeof(void*);
		ct->fcapacity = 0;
	}
	if(cdef->getDefaultNull != NULL) {
		ct->fdefnull = cdef->getDefaultNull;
	}
	ct->struct_size = cdef->c_struct_size;
}

/* ------------------------------------------------------------------------ */
/* [instaceof] */

kbool_t ClassTBL_isa_(CTX, kclass_t *ct, kclass_t *ct2)
{
	kcid_t reqt = ct2->cid;
	if(reqt == CLASS_Object || reqt == CLASS_Tdynamic || ct->cid == reqt) return 1;
	DBG_ASSERT(ct->cid != CLASS_Tvoid); DBG_ASSERT(ct->cid != CLASS_Tvar);
	//if(cid == CLASS_Tvoid) return 0;
//  if(ClassTBL(scid)->bcid == tcid) return 1; /* Int:km Int */
	if(ct->bcid == CLASS_Tuple && ct2->bcid == CLASS_Tuple) {
		if(ct2->cparam->psize < ct->cparam->psize) {
			size_t i;
			for(i = 0; i < ct2->cparam->psize; i++) {
				kparam_t *p = knh_Param_get(ct->cparam, i);
				kparam_t *p2 = knh_Param_get(ct2->cparam, i);
				if(p->type != p2->type) return 0;
			}
			return 1;
		}
		return 0;
	}
	if(ct->bcid == CLASS_Func && ct2->bcid == CLASS_Func) {
		if(ct->cparam->psize <= ct2->cparam->psize) {
			size_t i;
			for(i = 0; i < ct->cparam->psize; i++) {
				kparam_t *p = knh_Param_get(ct->cparam, i);
				kparam_t *p2 = knh_Param_get(ct2->cparam, i);
				if(p->type != p2->type) return 0;
			}
			if(ct->cparam->rsize == ct2->cparam->rsize) {
				if(ct->cparam->rsize == 1) {
					kparam_t *r = knh_Param_rget(ct->cparam, 0);
					kparam_t *r2 = knh_Param_rget(ct2->cparam, 0);
					if(r->type != r2->type) return 0;
				}
				return 1;
			}
		}
		return 0;
	}
	while(ct->cid != CLASS_Object) {
		kArray *a = ct->typemaps;
		size_t i;
		for(i = 0; i < kArray_size(a); i++) {
			kTypeMap *tmr = a->trans[i];
			if(tmr->tcid == reqt && TypeMap_isInterface(tmr)) return 1;
		}
		if(ct == ct->supTBL) return 0;
		ct = ct->supTBL;
		if(ct->cid == reqt) return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */
/* [class param] */

int knh_class_isGenerics(CTX, kcid_t cid)
{
	kclass_t *ct = ClassTBL(cid);
	return (ct->cparam != NULL);
}

kbytes_t knh_class_bname(CTX, kcid_t bcid)
{
	return knh_bytes_head(S_tobytes(ClassTBL(bcid)->lname), '<');
}

static void knh_make_cname(CTX, kOutputStream *w, kcid_t cid)
{
	DBG_ASSERT(cid < ctx->share->sizeClassTBL);
	DBG_ASSERT(C_isGenerics(cid));
	kcid_t bcid = C_bcid(cid);
	size_t i;
	kParam *pa = ClassTBL(cid)->cparam;
	kbytes_t bname = knh_class_bname(_ctx, bcid);
	knh_write(_ctx, w, bname);
	kwb_putc(wb, '<');
	for(i = 0; i < pa->psize; i++) {
		kparam_t *p = knh_Param_get(pa, i);
		if(i > 0) kwb_putc(wb, ',');
		knh_write_cid(_ctx, w, p->type);
	}
	if(pa->rsize > 0) {
		if(pa->psize == 0) knh_write(_ctx, w, STEXT("void"));
		knh_write(_ctx, w, STEXT("=>"));
	}
	for(i = 0; i < pa->rsize; i++) {
		kparam_t *p = knh_Param_rget(pa, i);
		if(i > 0) kwb_putc(wb, ',');
		knh_write_cid(_ctx, w, p->type);
	}
	kwb_putc(wb, '>');
}

static void knh_make_tname(CTX, kOutputStream *w, kcid_t cid)
{
	DBG_ASSERT(cid < ctx->share->sizeClassTBL);
	DBG_ASSERT(C_isGenerics(cid));
	kcid_t bcid = C_bcid(cid);
	kParam *pa = ClassTBL(cid)->cparam;
	kcid_t p1   = knh_Param_get(pa, 0)->type;
	if(bcid == CLASS_Array) {
		knh_write_type(_ctx, w, p1);
		knh_write(_ctx, w, STEXT(PTY_Array));
	}
	else if(bcid == CLASS_Iterator) {
		knh_write_type(_ctx, w, p1);
		knh_write(_ctx, w, STEXT(PT_Iterator));
	}
	else if(bcid == CLASS_KindOf) {
		knh_write_type(_ctx, w, p1);
		knh_write(_ctx, w, STEXT(PT_KindOf));
	}
	else if(bcid == CLASS_Immutable) {
		knh_write_type(_ctx, w, p1);
		knh_write(_ctx, w, STEXT(PT_Immutable));
	}
	else if(C_isGenerics(cid)) {
		size_t i;
		kbytes_t bname = knh_bytes_head(S_tobytes(ClassTBL(bcid)->sname), '<');
		knh_write(_ctx, w, bname);
		kwb_putc(wb, '<');
		for(i = 0; i < pa->psize; i++) {
			kparam_t *p = knh_Param_get(pa, i);
			if(i > 0) kwb_putc(wb, ',');
			knh_write_type(_ctx, w, p->type);
		}
		if(pa->rsize > 0) {
			if(pa->psize == 0) knh_write(_ctx, w, STEXT("void"));
			knh_write(_ctx, w, STEXT("=>"));
		}
		for(i = 0; i < pa->rsize; i++) {
			kparam_t *p = knh_Param_rget(pa, i);
			if(i > 0) kwb_putc(wb, ',');
			knh_write_type(_ctx, w, p->type);
		}
		kwb_putc(wb, '>');
	}
}

/* ------------------------------------------------------------------------ */

static void SimTBL_append(CTX, kcid_t cid, kclass_t *newt)
{
	kclass_t *t = varClassTBL(cid);
	while(t->simTBL != NULL) {
		t = (struct _kclass*)t->simTBL;
	}
	t->simTBL = (kclass_t*)newt;
}

void knh_setClassParam(CTX, kclass_t *t, kParam *pa)
{
	size_t i, size = pa->psize + pa->rsize;
	KINITv(t->cparam, pa);
	if(pa->psize > 0) {
		kparam_t *p = knh_Param_get(pa, 0);
		t->p1 = CLASS_t(p->type);
	}
	if(pa->psize > 1) {
		kparam_t *p = knh_Param_get(pa, 1);
		t->p2 = CLASS_t(p->type);
	}
	for(i = 0; i < size; i++) {
		kparam_t *p = knh_Param_get(pa, i);
		if((p->type) >= T_T1) {
			t->cflag |= FLAG_Class_TypeVariable;
		}
	}
}

static void ClassTBL_addTuple(CTX, kclass_t *ct, kclass_t *bct, kParam *pa)
{
	size_t i, fi = 0;
	ct->magicflag  = bct->magicflag;
	ct->cflag  = bct->cflag;
	knh_setClassDef(_ctx, ct, bct->cdef);
	ct->bcid   = bct->cid;
	ct->baseTBL = bct;
	ct->supcid = bct->supcid;
	ct->supTBL = ClassTBL(ct->supcid);
	knh_setClassParam(_ctx, ct, pa);
	SimTBL_append(_ctx, bct->cid, ct);

	kTuple *tpl = (kTuple*)new_hObject_(_ctx, (struct _kclass*)ct);
	Object_setNullObject(tpl, 1);
	knh_setClassDefaultValue(_ctx, ct->cid, tpl, NULL);
	ct->fcapacity = pa->psize;
	DBLNDATA_(for(i = 0; i < pa->psize; i++) {
		kparam_t *p = knh_Param_get(pa, i);
		if(TY_isUnbox(p->type)) ct->fcapacity += 1;
	})
	ct->fsize = ct->fcapacity;
	ct->fields = (kfieldinfo_t*)KCALLOC(sizeof(kfieldinfo_t) * ct->fcapacity);
	if(ct->fcapacity <= K_SMALLOBJECT_FIELDSIZE) {
		KWRITE_BARRIER(tpl, &(tpl->smallobject));
		tpl->fields = &(tpl->smallobject);
	}
	else {
		tpl->fields = (kObject**)KCALLOC(sizeof(kObject*) * ct->fcapacity);
	}
	knh_bzero(tpl->fields, sizeof(kObject*) * ct->fcapacity);
	for(i = 0; i < pa->psize; i++) {
		kparam_t *p = knh_Param_get(pa, i);
		ct->fields[fi].flag = kField_Getter;
		ct->fields[fi].type = p->type;
		ct->fields[fi].fn = FN_key;
		ct->fields[fi].israw = 0;
		if(TY_isUnbox(p->type)) {
			ct->fields[fi].israw = 1;
			DBLNDATA_(fi++;
				ct->fields[fi].flag = 0;
				ct->fields[fi].fn = FN_NONAME;
				ct->fields[fi].type = TY_void;
				ct->fields[fi].israw = 1;)
		}
		else {
			KWRITE_BARRIER(tpl, KNH_NULVAL(CLASS_t(p->type)));
			KINITv(tpl->fields[fi], KNH_NULVAL(CLASS_t(p->type)));
		}
		fi++;
	}
	KWRITE_BARRIER(ct, bct->methods);
	KINITv(ct->methods, bct->methods);
	KINITv(ct->typemaps, K_EMPTYARRAY);
}

kcid_t knh_addGenericsClass(CTX, kcid_t cid, kcid_t bcid, kParam *pa)
{
	if(cid == CLASS_newid) cid = new_ClassId(_ctx);
	DBG_ASSERT(bcid < cid);
	kclass_t *ct = varClassTBL(cid);
	if(bcid == CLASS_Tuple) {
		ClassTBL_addTuple(_ctx, ct, ClassTBL(bcid), pa);
	}
	else {
		kclass_t *bct = ClassTBL(bcid);
		ct->magicflag  = bct->magicflag;
		ct->cflag  = bct->cflag;
		knh_setClassDef(_ctx, ct, bct->cdef);
		ct->bcid   = bcid;
		ct->baseTBL = bct;
		ct->supcid = bct->supcid;
		ct->supTBL = ClassTBL(ct->supcid);
		knh_setClassParam(_ctx, ct, pa);
		SimTBL_append(_ctx, bcid, ct);
		ct->fields = bct->fields;
		ct->fsize  = bct->fsize;
		ct->fcapacity = bct->fcapacity;
		KINITv(ct->methods, bct->methods);
		KINITv(ct->typemaps, K_EMPTYARRAY);
	}
	{
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		kString *lname, *sname;
		knh_make_cname(_ctx, cwb->w, cid);
		lname = CWB_newString(_ctx, cwb, SPOL_POOLNEVER|SPOL_ASCII);
		knh_make_tname(_ctx, cwb->w, cid);
		sname = CWB_newString(_ctx, cwb, SPOL_POOLNEVER|SPOL_ASCII);
		knh_setClassName(_ctx, cid, lname, sname);
	}
	return cid;
}

/* ------------------------------------------------------------------------ */

kcid_t knh_class_Generics(CTX, kcid_t bcid, kParam *pa)
{
	kclass_t *t = ClassTBL(bcid);
	while(t != NULL) {
		if(t->cparam != NULL && knh_Param_equalsType(pa, t->cparam)) {
			return t->cid;
		}
		t = t->simTBL;
	}
	if(bcid == CLASS_Immutable) {
		kcid_t p1 = knh_Param_get(pa,0)->type;
		if(p1 < ctx->share->sizeClassTBL && class_isImmutable(p1)) return p1;
	}
	return knh_addGenericsClass(_ctx, CLASS_newid, bcid, pa);
}

kcid_t knh_class_P1(CTX, kcid_t bcid, ktype_t p1)
{
	kclass_t *ct = ClassTBL(bcid);
	while(ct != NULL) {
		if(ct->p1 == p1) return ct->cid;
		ct = ct->simTBL;
	}
	if(bcid == CLASS_Immutable && p1 < ctx->share->sizeClassTBL && class_isImmutable(p1)) {
		return p1;
	}
	{
		kParam *bpa = ClassTBL(bcid)->cparam;
		kparam_t *bp = knh_Param_get(bpa, 0);
		kParam *pa = new_Param(_ctx);
		kparam_t p = {p1, bp->fn};
		knh_Param_add(_ctx, pa, p);
		return knh_addGenericsClass(_ctx, CLASS_newid, bcid, pa);
	}
}

kcid_t knh_class_P2(CTX, kcid_t bcid, ktype_t p1, ktype_t p2)
{
	kclass_t *ct = ClassTBL(bcid);
	while(ct != NULL) {
		if(ct->p2 == p2 && ct->p1 == p1) return ct->cid;
		ct = ct->simTBL;
	}
	{
		kParam *bpa = ClassTBL(bcid)->cparam;
		kparam_t *bp = knh_Param_get(bpa, 0);
		kParam *pa = new_Param(_ctx);
		kparam_t p = {p1, bp->fn};
		knh_Param_add(_ctx, pa, p);
		bp = knh_Param_get(bpa, 1);
		p.type = p2; p.fn = bp->fn;
		knh_Param_add(_ctx, pa, p);
		return knh_addGenericsClass(_ctx, CLASS_newid, bcid, pa);
	}
}

/* ------------------------------------------------------------------------ */

kbool_t knh_Param_hasTypeVar(kParam *pa)
{
	size_t i, size = pa->psize + pa->rsize;
	for(i = 0; i < size; i++) {
		kparam_t *p = knh_Param_get(pa, i);
		if(!(p->type < TY_T0)) return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

void kParamocid(CTX, kParam *pa, kcid_t this_cid, kParam *npa)
{
	size_t i, size = pa->psize + pa->rsize;
	for(i = 0; i < size; i++) {
		kparam_t *cp = knh_Param_get(pa, i);
		kparam_t np = *cp;
		np.type = ktype_tocid(_ctx, np.type, this_cid);
		knh_Param_add(_ctx, npa, np);
	}
	npa->psize = pa->psize;
	npa->rsize = pa->rsize;
}

/* ------------------------------------------------------------------------ */

KNHAPI2(kcid_t) ktype_tocid(CTX, ktype_t ptype, kcid_t this_cid)
{
	if(ptype == T_This) {
		return this_cid;
	}
	else if(ptype > T_This) {
		long ti = (long)ptype - (long)T_T1;
		kParam *tpa = ClassTBL(this_cid)->cparam;
		if(tpa != NULL && ti < (tpa)->psize + (tpa)->rsize) {
			kparam_t *p = knh_Param_get(tpa, ti);
			//DBG_P("this_cid=%d, p%d=%d", this_cid, ti+1, p->type);
			return CLASS_t(p->type);
		}
		return CLASS_Tvoid;
	}
	else {
		kcid_t pcid = CLASS_t(ptype);
		if(C_isGenerics(pcid)) { /* Iterator<This> */
			BEGIN_LOCAL(_ctx, lsfp, 1);
			kParam *npa = new_Param(_ctx);
			KSETv(lsfp[0].o, npa);
			kParamocid(_ctx, ClassTBL(pcid)->cparam, this_cid, npa);
			pcid = knh_class_Generics(_ctx, C_bcid(pcid), npa);
			END_LOCAL(_ctx, lsfp);
		}
		return pcid;
	}
}

/* ------------------------------------------------------------------------ */
/* [Param] */

kParam *new_ParamR0(CTX, ktype_t t)
{
	kParam *pa = new_Param(_ctx);
	knh_Param_addReturnType(_ctx, pa, t);
	return pa;
}

#define RT_set(t)    TY_void

kParam *new_ParamP1(CTX, ktype_t rtype, ktype_t p1, ksymbol_t fn1)
{
	kParam *pa = new_Param(_ctx);
	knh_Param_addParam(_ctx, pa, p1, fn1);
	knh_Param_addReturnType(_ctx, pa, rtype);
	return pa;
}

/* ------------------------------------------------------------------------ */

void knh_Param_add(CTX, kParam *pa, kparam_t p)
{
	size_t size = pa->psize + pa->rsize;
	if(size == 0) {
		pa->p0 = p;
		pa->p2 = p;  // last
	}
	else if(size == 1) {
		pa->p1 = p;
		pa->p2 = p;  // last
	}
	else if(size == 2) {
		pa->p2 = p;
	}
	else if(size == 3) {
		kparam_t *params = (kparam_t*)KCALLOC(K_FASTMALLOC_SIZE);
		params[0] = pa->p0;
		params[1] = pa->p1;
		params[2] = pa->p2;
		params[3] = p;
		pa->capacity = K_FASTMALLOC_SIZE / sizeof(kparam_t);
		pa->params = params;
	}
	else {
		if(pa->capacity == size) {
			pa->params = (kparam_t*)KNH_REALLOC(_ctx, NULL, pa->params, pa->capacity, pa->capacity*2, sizeof(kparam_t));
			pa->capacity *= 2;
		}
		pa->params[size] = p;
	}
	pa->psize += 1;
}

void knh_Param_addParam(CTX, kParam *pa, ktype_t type, ksymbol_t fn)
{
	kparam_t p = {type, fn};
	knh_Param_add(_ctx, pa, p);
}

void knh_Param_addReturnType(CTX, kParam *pa, ktype_t type)
{
	if(type != TY_void) {
		kparam_t p = {type, FN_return};
		knh_Param_add(_ctx, pa, p);
		pa->psize -= 1;
		pa->rsize += 1;
	}
}

/* ------------------------------------------------------------------------ */

KNHAPI2(kparam_t*) knh_Param_get(kParam *pa, size_t n)
{
	size_t size = pa->psize + pa->rsize;
	DBG_ASSERT(n <= size);
	if(size <= 3) {
		switch(n) {
			case 0: return &pa->p0;
			case 1: return &pa->p1;
			case 2: return &pa->p2;
			default: return &pa->p2;
		}
	}
	else {
		return pa->params + n;
	}
}

//ktype_t knh_Param_getptype(kParam *pa, size_t n)
//{
//	if(!(n < pa->psize)) {
//		if(pa->psize == 0) return T_dyn;
//		n = pa->psize-1;
//	}
//	return knh_Param_get(pa, n)->type;
//}

KNHAPI2(ktype_t) knh_Method_rtype(CTX, kMethod *mtd, kcid_t this_cid)
{
	if(DP(mtd)->mp->rsize != 0) {
		ktype_t rtype = knh_Param_rget(DP(mtd)->mp, 0)->type;
		if(rtype >= T_This) {
			return ktype_tocid(_ctx, rtype, this_cid);
		}
		return rtype;
	}
	return TY_void;
}

KNHAPI2(size_t) knh_Method_psize(kMethod *mtd)
{
	return DP(mtd)->mp->psize;
}

KNHAPI2(ktype_t) knh_Method_ptype(CTX, kMethod *mtd, size_t n, kcid_t this_cid)
{
	ktype_t ptype = knh_Param_get(DP(mtd)->mp, n)->type;
	if(ptype >= T_This) {
		return ktype_tocid(_ctx, ptype, this_cid);
	}
	return ptype;
}


/* ------------------------------------------------------------------------ */

kbool_t knh_Param_equalsType(kParam *pa, kParam *pa2)
{
	size_t i;
	if(pa->psize != pa2->psize || pa->rsize != pa2->rsize) return 0;
	for(i = 0; i < pa->psize; i++) {
		kparam_t *p = knh_Param_get(pa, i);
		kparam_t *p2 = knh_Param_get(pa2, i);
		if(p->type != p2->type) return 0;
	}
	for(i = 0; i < pa->rsize; i++) {
		kparam_t *p = knh_Param_rget(pa, i);
		kparam_t *p2 = knh_Param_rget(pa2, i);
		if(p->type != p2->type) return 0;
	}
	return 1;
}

/* ------------------------------------------------------------------------ */

KNHAPI2(ktype_t) knh_Param_rtype(kParam *pa)
{
	if(pa->rsize != 0) {
		kparam_t *p = knh_Param_rget(pa, 0);
		return p->type;
	}
	return (Param_isRVAR(pa)) ? TY_var : TY_void;
}

/* ------------------------------------------------------------------------ */

KNHAPI2(void) knh_write_mn(CTX, kOutputStream *w, kmethodn_t mn)
{
	kbytes_t t = B(MN__(mn));
	if(MN_isFMT(mn)) {
		kwb_putc(wb, '%');
		knh_write(_ctx, w, t);
	}
	else if(MN_isISBOOL(mn)) {
		knh_write(_ctx, w, STEXT("is"));
		knh_write_cap(_ctx, w, t, 0);
	}
	else if(MN_isGETTER(mn)) {
		knh_write(_ctx, w, STEXT("get"));
		knh_write_cap(_ctx, w, t, 0);
	}
	else if(MN_isSETTER(mn)) {
		knh_write(_ctx, w, STEXT("set"));
		knh_write_cap(_ctx, w, t, 0);
	}
	else {
		knh_write(_ctx, w, t);
	}
}

/* ------------------------------------------------------------------------ */
/* [Method] */

static KMETHOD Fmethod_abstract(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	char mbuf[128];
	kreportf(WARN_, sfp[K_RTNIDX].uline, "calling abstract method: %s.%s", T_cid(mtd->cid), T_mn(mbuf, mtd->mn));
	RETURNi_(0); //necessary
}

KNHAPI2(kbool_t) Method_isAbstract(kMethod *mtd)
{
	return (mtd->fcall_1 == Fmethod_abstract);
}

void knh_Method_setFunc(CTX, kMethod *mtd, knh_Fmethod func)
{
	func = (func == NULL) ? Fmethod_abstract : func;
	(mtd)->fcall_1 = func;
	(mtd)->pc_start = CODE_NCALL;
	DBG_ASSERT((mtd)->pc_start != NULL);
}

void kMethodoAbstract(CTX, kMethod *mtd)
{
	KSETv(DP(mtd)->kcode, K_NULL);
	DP(mtd)->cfunc = NULL;
	knh_Method_setFunc(_ctx, mtd, Fmethod_abstract);
}

/* ------------------------------------------------------------------------ */

kMethod* new_Method(CTX, kflag_t flag, kcid_t cid, kmethodn_t mn, knh_Fmethod func)
{
	kMethod* mtd = new_(Method);
	DP(mtd)->flag  = flag;
	(mtd)->cid     = cid;
	(mtd)->mn      = mn;
	knh_Method_setFunc(_ctx, mtd, func);
	return mtd;
}


/* ------------------------------------------------------------------------ */
/* [VirtualField] */

static KMETHOD Fmethod_getter(CTX, ksfp_t *sfp _RIX)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	RETURN_((sfp[0].ox)->fields[delta]);
}
static KMETHOD Fmethod_ngetter(CTX, ksfp_t *sfp _RIX)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	kunbox_t *data = (kunbox_t*)(&(sfp[0].ox->fields[delta]));
	RETURNd_(data[0]);
}
static KMETHOD Fmethod_setter(CTX, ksfp_t *sfp _RIX)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	KWRITE_BARRIER((sfp[0].ox), sfp[1].o);
	KSETv((sfp[0].ox)->fields[delta], sfp[1].o);
	RETURN_(sfp[1].o);
}
static KMETHOD Fmethod_nsetter(CTX, ksfp_t *sfp _RIX)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	kunbox_t *ndata = (kunbox_t*)(&((sfp[0].ox)->fields[delta]));
	ndata[0] = sfp[1].ndata;
	RETURNd_(ndata[0]);
}
static KMETHOD Fmethod_kgetter(CTX, ksfp_t *sfp _RIX)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	RETURN_((sfp[0].p)->kfields[delta]);
}
static KMETHOD Fmethod_kngetter(CTX, ksfp_t *sfp _RIX)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	kunbox_t *data = (kunbox_t*)(&(sfp[0].p->kfields[delta]));
	RETURNd_(data[0]);
}
static KMETHOD Fmethod_ksetter(CTX, ksfp_t *sfp _RIX)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	KWRITE_BARRIER((sfp[0].p), sfp[1].o);
	KSETv((sfp[0].p)->kfields[delta], sfp[1].o);
	RETURN_(sfp[1].o);
}
static KMETHOD Fmethod_knsetter(CTX, ksfp_t *sfp _RIX)
{
	int delta = DP(sfp[K_MTDIDX].mtdNC)->delta;
	kunbox_t *ndata = (kunbox_t*)(&((sfp[0].p)->kfields[delta]));
	ndata[0] = sfp[1].ndata;
	RETURNd_(ndata[0]);
}

static knh_Fmethod accessors[8] = {
	Fmethod_getter, Fmethod_setter, Fmethod_ngetter, Fmethod_nsetter,
	Fmethod_kgetter, Fmethod_ksetter, Fmethod_kngetter, Fmethod_knsetter,
};

#define _SETTER  1
#define _NDATA   2
#define _CPPOBJ  4

static kMethod *new_GetterMethod(CTX, kcid_t cid, kmethodn_t mn, ktype_t type, int idx)
{
	//knh_Fmethod f = (TY_isUnbox(type)) ? Fmethod_ngetter : Fmethod_getter;
	knh_Fmethod f = accessors[(TY_isUnbox(type)?_NDATA:0)|((ClassTBL(cid)->bcid==CLASS_CppObject)?_CPPOBJ:0)];
	kMethod *mtd = new_Method(_ctx, 0, cid, mn, f);
	DP(mtd)->delta = idx;
	KSETv(DP(mtd)->mp, new_ParamR0(_ctx, type));
	KWRITE_BARRIER(DP(mtd), DP(mtd)->mp);
	return mtd;
}

static kMethod *new_SetterMethod(CTX, kcid_t cid, kmethodn_t mn, ktype_t type, int idx)
{
	//knh_Fmethod f = (TY_isUnbox(type)) ? Fmethod_nsetter : Fmethod_setter;
	knh_Fmethod f = accessors[_SETTER|(TY_isUnbox(type)?_NDATA:0)|((ClassTBL(cid)->bcid==CLASS_CppObject)?_CPPOBJ:0)];
	kMethod *mtd = new_Method(_ctx, 0, cid, mn, f);
	DP(mtd)->delta = idx;
	KSETv(DP(mtd)->mp, new_ParamP1(_ctx, RT_set(type), type, FN_UNMASK(mn)));
	KWRITE_BARRIER(DP(mtd), DP(mtd)->mp);
	return mtd;
}

kindex_t knh_Method_indexOfGetterField(kMethod *o)
{
	knh_Fmethod f = SP(o)->fcall_1;
	if(f== Fmethod_getter || f == Fmethod_ngetter) {
		return (kindex_t)DP(o)->delta;
	}
	return -1;
}

kindex_t knh_Method_indexOfSetterField(kMethod *o)
{
	knh_Fmethod f = SP(o)->fcall_1;
	if(f == Fmethod_setter || f == Fmethod_nsetter) {
		return (kindex_t)DP(o)->delta;
	}
	return -1;
}

/* ------------------------------------------------------------------------ */

kDictMap* knh_Object_getXData(CTX, kObject *o)
{
	kDictMap *m;
	kclass_t *ct = O_ct(o);
	if(ct->xdataidx != -1) {
		kObject **v = (ct->bcid == CLASS_Object) ? ((kObject*)o)->fields : ((kObject*)o)->kfields;
		m = (kDictMap*)v[ct->xdataidx];
		if(IS_NULL(m)) {
			m = new_DictMap0(_ctx, 0, 1/*isCaseMap*/, "xdata");
			KWRITE_BARRIER(o, m);
			KSETv(v[ct->xdataidx], m);
		}
	}
	else {
		if(!Object_isXData(o)) {
			m = new_DictMap0(_ctx, 0, 1/*isCaseMap*/, "xdata");
			Object_setXData(o, 1);
			knh_PtrMap_add(_ctx, ctx->share->xdataPtrMap, o, m);
		}
		else {
			m = (kDictMap*)knh_PtrMap_get(_ctx, ctx->share->xdataPtrMap, o);
			DBG_ASSERT(m != NULL);
		}
	}
	return m;
}

static KMETHOD Fmethod_xgetter(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	if(Object_isXData(sfp[0].o) || O_ct(sfp[0].o)->xdataidx != -1) {
		ksymbol_t fn = DP(mtd)->delta;
		kString *n = knh_getFieldName(_ctx, fn);
		kDictMap *m = knh_Object_getXData(_ctx, sfp[0].o);
		Object *v = knh_DictMap_getNULL(_ctx, m, S_tobytes(n));
		if(v != NULL) {
			RETURN_(v);
		}
	}
	ktype_t rtype = knh_Param_rtype(DP(mtd)->mp);
	RETURN_(KNH_NULVAL(rtype));
}

static KMETHOD Fmethod_xngetter(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	if(Object_isXData(sfp[0].o) || O_ct(sfp[0].o)->xdataidx != -1) {
		ksymbol_t fn = DP(mtd)->delta;
		kString *n = knh_getFieldName(_ctx, fn);
		kDictMap *m = knh_Object_getXData(_ctx, sfp[0].o);
		Object *v = knh_DictMap_getNULL(_ctx, m, S_tobytes(n));
		if(v != NULL) {
			RETURNd_(O_ndata(v));
		}
	}
	RETURNd_(0);
}

static KMETHOD Fmethod_xsetter(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t fn = DP(mtd)->delta;
	kString *n = knh_getFieldName(_ctx, fn);
	kDictMap *m = knh_Object_getXData(_ctx, sfp[0].o);
	knh_DictMap_set(_ctx, m, n, sfp[1].o);
	RETURN_(sfp[1].o);
}

static KMETHOD Fmethod_xnsetter(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t  fn = DP(mtd)->delta;
	kString  *n = knh_getFieldName(_ctx, fn);
	kDictMap *m = knh_Object_getXData(_ctx, sfp[0].o);
	ktype_t p1 = knh_Param_get(DP(mtd)->mp, 0)->type;
	knh_DictMap_set(_ctx, m, n, new_Boxing(_ctx, sfp+1, ClassTBL(p1)));
	RETURNd_(sfp[1].ndata);
}

void knh_ClassTBL_addXField(CTX, kclass_t *ct, ktype_t type, ksymbol_t fn)
{
	knh_Fmethod f = (TY_isUnbox(type)) ? Fmethod_xngetter : Fmethod_xgetter;
	kMethod *mtd = new_Method(_ctx, 0, ct->cid, (type == CLASS_Boolean) ? MN_toISBOOL(fn) : MN_toGETTER(fn), f);
	DP(mtd)->delta = fn;
	KSETv(DP(mtd)->mp, new_ParamR0(_ctx, type));
	KWRITE_BARRIER(DP(mtd), DP(mtd)->mp);
	knh_ClassTBL_addMethod(_ctx, ct, mtd, 0/*isCheck*/);

	f = (TY_isUnbox(type)) ? Fmethod_xnsetter : Fmethod_xsetter;
	mtd = new_Method(_ctx, 0, ct->cid, MN_toSETTER(fn), f);
	DP(mtd)->delta = fn;
	KSETv(DP(mtd)->mp, new_ParamP1(_ctx, RT_set(type), type, fn));
	KWRITE_BARRIER(DP(mtd), DP(mtd)->mp);
	knh_ClassTBL_addMethod(_ctx, ct, mtd, 0/*isCheck*/);
}

kMethod *knh_KonohaSpace_addXSetter(CTX, kKonohaSpace *ns, kclass_t *ct, ktype_t type, kmethodn_t mn_setter)
{
	if(FLAG_is(ct->cflag, FLAG_Class_Expando)) {
		ksymbol_t fn = FN_UNMASK(mn_setter);
		kmethodn_t mn = (type == CLASS_Boolean) ? MN_toISBOOL(fn) : MN_toGETTER(fn);
		kMethod *mtd = knh_KonohaSpace_getMethodNULL(_ctx, ns, ct->cid, mn);
		if(mtd == NULL) {
			knh_Fmethod f = (TY_isUnbox(type)) ? Fmethod_xngetter : Fmethod_xgetter;
			mtd = new_Method(_ctx, 0, ct->cid, mn, f);
			DP(mtd)->delta = fn;
			KSETv(DP(mtd)->mp, new_ParamR0(_ctx, type));
			KWRITE_BARRIER(DP(mtd), DP(mtd)->mp);
			knh_ClassTBL_addMethod(_ctx, ct, mtd, 0/*isCheck*/);

			f = (TY_isUnbox(type)) ? Fmethod_xnsetter : Fmethod_xsetter;
			mtd = new_Method(_ctx, 0, ct->cid, mn_setter, f);
			DP(mtd)->delta = fn;
			KSETv(DP(mtd)->mp, new_ParamP1(_ctx, RT_set(type), type, fn));
			KWRITE_BARRIER(DP(mtd), DP(mtd)->mp);
			knh_ClassTBL_addMethod(_ctx, ct, mtd, 0/*isCheck*/);
			return mtd;
		}
	}
	return NULL;
}


/* ------------------------------------------------------------------------ */

static KMETHOD Fmethod_NoSuchMethod(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	KNH_ASSERT(IS_Method(mtd));
	kcid_t cid = Method_isStatic(mtd) ? (mtd)->cid : O_cid(sfp[0].o);
	THROW_NoSuchMethod(_ctx, sfp, cid, (mtd)->mn);
}

static kMethod* new_NoSuchMethod(CTX, kcid_t cid, kmethodn_t mn)
{
	kMethod *mtd = new_Method(_ctx, 0, cid, mn, Fmethod_NoSuchMethod);
	return mtd;
}

/* ------------------------------------------------------------------------ */

static inline kMethod *Cache_setMethod(knh_mtdcache_t *mcache, kcid_t cid, kmethodn_t mn, kMethod *mtd)
{
	DBG_ASSERT(mtd != NULL);
	kuint_t h = hashcode_mtd(cid, mn, K_MTDCACHE_SIZE);
	mcache = mcache + h;
	DBG_(
	if(mcache->cid != cid || mcache->mn != mn) {
		if(mcache->cid != 0 && mcache->mn != 0) {
			//DBG_P("MISSED h=%d mcache=%d.%d %d.%d", h, mcache->cid, mcache->mn, cid, mn);
		}
	});
	mcache->cid = cid;
	mcache->mn  = mn;
	mcache->mtd = mtd;
	return mtd;
}

void knh_ClassTBL_addMethod(CTX, kclass_t *t, kMethod *mtd, int isCHECK)
{
	DBG_ASSERT(IS_Method(mtd));
	kArray *a = t->methods;
	if(unlikely(a == K_EMPTYARRAY)) {
		KNH_ASSERT(kArray_size(a) == 0);
		a = new_Array0(_ctx, K_FASTMALLOC_SIZE/sizeof(kMethod*));
		KSETv(((struct _kclass*)t)->methods, a);
		KWRITE_BARRIER((struct _kclass*)t, a);
	}
//	if(isCHECK) {
//		size_t i;
//		for(i = 0; i < kArray_size(a); i++) {
//			kMethod *mtd2 = a->methods[i];
//			if((mtd2)->mn == (mtd)->mn) {
//				knh_ldata_t ldata[] = {LOG_s("msg", "redefined method"), MDATA("method", CLASS__((mtd)->cid), MN__((mtd)->mn)), __TRACE__};
//				KNH_NTRACE(_ctx, ("konoha");
//			}
//		}
//	}
	if(FLAG_is(t->cflag, FLAG_Class_Singleton)) {
		DP(mtd)->flag = DP(mtd)->flag | FLAG_Method_Static;
	}
	if(FLAG_is(t->cflag, FLAG_Class_Immutable)) {
		DP(mtd)->flag = DP(mtd)->flag | FLAG_Method_Immutable;
	}
	knh_enforceSecfileidty(_ctx, mtd);
	kArray_add(a, mtd);
	Cache_setMethod(_ctx->mtdcache, mtd->cid, mtd->mn, mtd);
}



kMethod* knh_ClassTBL_findMethodNULL(CTX, kclass_t *ct, kmethodn_t mn, int isGEN)
{
	{
		kcid_t cid = ct->cid;
		kuint_t h = hashcode_mtd(cid, mn, K_MTDCACHE_SIZE);
		knh_mtdcache_t *mcache = ctx->mtdcache + h;
		if(mcache->cid == ct->cid && mcache->mn == mn) {
			//DBG_P("HIT h=%d mcache=%d.%d %d.%d", h, mcache->cid, mcache->mn, t->cid, mn);
			KNH_ASSERT(IS_Method(mcache->mtd));
			return mcache->mtd;
		}
	}
	{
		kclass_t *p, *t0 = ct;
		do {
			size_t i;
			kArray *a = t0->methods;
			for(i = 0; i < kArray_size(a); i++) {
				kMethod *mtd = a->methods[i];
				if((mtd)->mn == mn) {
					return Cache_setMethod(_ctx->mtdcache, ct->cid, mn, mtd);
				}
			}
			p = t0;
			t0 = t0->supTBL;
		}
		while(p != t0);
	}
	if(MN_isSETTER(mn)) {
		kindex_t idx;
		ksymbol_t fn = MN_toFN(mn);
		for(idx = (kindex_t)ct->fsize - 1; idx >= 0 ; idx--) {
			kfieldinfo_t *cf = ct->fields + idx;
			if(cf->fn == fn) {
				if(!FLAG_is(cf->flag, kField_Setter)) {
					continue;
				}else {
					kMethod *mtd = new_SetterMethod(_ctx, ct->cid, mn, cf->type, idx);
					knh_ClassTBL_addMethod(_ctx, ct, mtd, 0);
					return Cache_setMethod(_ctx->mtdcache, ct->cid, mn, mtd);
				}
			}
		}
		if(FLAG_is(ct->cflag, FLAG_Class_Expando)) {

		}
	}
	if(MN_isGETTER(mn) || MN_isISBOOL(mn)) {
		kindex_t idx;
		ksymbol_t fn = MN_toFN(mn);
		for(idx = (kindex_t)ct->fsize - 1; idx >= 0 ; idx--) {
			kfieldinfo_t *cf = ct->fields + idx;
			if(cf->fn == fn) {
				if(MN_isISBOOL(mn) && !IS_Tbool(cf->type)) {
					continue;
				}
				if(!FLAG_is(cf->flag, kField_Getter)) {
					continue;
				}
				kMethod *mtd = new_GetterMethod(_ctx, ct->cid, mn, cf->type, idx);
				knh_ClassTBL_addMethod(_ctx, ct, mtd, 0);
				return Cache_setMethod(_ctx->mtdcache, ct->cid, mn, mtd);
			}
		}
		goto L_NoSuchMethod;
	}
	L_NoSuchMethod:;
	if(isGEN) {
		if(MN_isFMT(mn)) {
			DBG_P("Generating %%empty: cid=%s mn=%%%s", CLASS__(ct->cid), MN__(mn));
			return ClassTBL_getMethodNULL(_ctx, ct, MN__empty);
		}
		else {
			kMethod *mtd = new_NoSuchMethod(_ctx, ct->cid, mn);
			knh_ClassTBL_addMethod(_ctx, ct, mtd, 0);
			return Cache_setMethod(_ctx->mtdcache, ct->cid, mn, mtd);
		}
	}
	else {
		return NULL;
	}
}

kMethod *knh_ClassTBL_getFmtNULL(CTX, kclass_t *t, kmethodn_t mn0)
{
	kmethodn_t mn = mn0;
	kMethod *mtd = knh_ClassTBL_findMethodNULL(_ctx, t, mn, 0);
	if(mtd != NULL) {
		return Cache_setMethod(_ctx->mtdcache, t->cid, mn0, mtd);
	}
	return NULL;
}

kMethod *knh_ClassTBL_getFmt(CTX, kclass_t *t, kmethodn_t mn0)
{
	kMethod *mtd = knh_ClassTBL_getFmtNULL(_ctx, t, mn0);
	if(mtd == NULL) {
		mtd = knh_ClassTBL_findMethodNULL(_ctx, ClassTBL(CLASS_Object), MN__empty, 1);
		mtd = Cache_setMethod(_ctx->mtdcache, t->cid, mn0, mtd);
	}
	//DBG_P("%s.%s => %s.%s", CLASS__(t->cid), MN__(mn0), CLASS__(mtd->cid), MN__(mtd->mn));
	return mtd;
}

/* ------------------------------------------------------------------------ */


kMethod* knh_KonohaSpace_getMethodNULL(CTX, kKonohaSpace *ns, kcid_t cid, kmethodn_t mn)
{
	while(ns != NULL) {
		if(DP(ns)->methodsNULL != NULL) {
			size_t i;
			kArray *methods = DP(ns)->methodsNULL;
			for(i = 0; i < kArray_size(methods); i++) {
				kMethod *mtd = methods->methods[i];
				if(mtd->cid == cid && mtd->mn == mn) {
					return mtd;
				}
			}
		}
		ns = ns->parentNULL;
	}
	return ClassTBL_getMethodNULL(_ctx, ClassTBL(cid), mn);
}

kMethod* knh_KonohaSpace_getFmtNULL(CTX, kKonohaSpace *ns, kcid_t cid, kmethodn_t mn)
{
	L_TAIL:;
	if(DP(ns)->formattersNULL != NULL) {
		kArray *a = DP(ns)->formattersNULL;
		size_t i;
		for(i = 0; i < kArray_size(a); i++) {
			kMethod *mtd = a->methods[i];
			if((mtd)->cid == cid && (mtd)->mn == mn) {
				return mtd;
			}
		}
	}
	if(ns->parentNULL != NULL) {
		ns = ns->parentNULL;
		goto L_TAIL;
	}
	return knh_ClassTBL_getFmtNULL(_ctx, ClassTBL(cid), mn);
}

void knh_KonohaSpace_addFmt(CTX, kKonohaSpace *ns, kMethod *mtd)
{
	if(DP(ns)->formattersNULL == NULL) {
		KINITv(DP(ns)->formattersNULL, new_Array0(_ctx, 0));
	}
	kArray_add(DP(ns)->formattersNULL, mtd);
}

KNHAPI2(void) knh_Func_invoke(CTX, kFunc *fo, ksfp_t *rtnsfp, int argc)
{
	ksfp_t *sfp = rtnsfp + K_CALLDELTA;
	if(fo->baseNULL != NULL) {
		KSETv(sfp[0].o, fo->baseNULL);
	}
	klr_setmtdNC(_ctx, sfp[K_MTDIDX], fo->mtd);
	((kcontext_t*)ctx)->esp = sfp + (argc + 1);
	(fo->mtd)->fcall_1(_ctx, sfp, -K_CALLDELTA);
	((kcontext_t*)ctx)->esp = rtnsfp;
}

/* ------------------------------------------------------------------------ */
/* [MapType] */

static inline kTypeMap *Cache_getTypeMap(CTX, kcid_t scid, kcid_t tcid)
{
	kuint_t h = hashcode_tmr(scid, tcid, K_TMAPCACHE_SIZE);
	knh_tmrcache_t *cache = ctx->tmrcache + h;
	if(cache->scid  == scid && cache->tcid == tcid) {
		return cache->tmr;
	}
	return NULL;
}

static inline kTypeMap *Cache_setTypeMap(CTX, kcid_t scid, kcid_t tcid, kTypeMap *tmr)
{
	kuint_t h = hashcode_tmr(scid, tcid, K_TMAPCACHE_SIZE);
	knh_tmrcache_t *cache = ctx->tmrcache + h;
	cache->scid = scid;
	cache->tcid = tcid;
	cache->tmr = tmr;
	DBG_P("SETCACHE (%s=>%s) TMR %s=>%s",CLASS__(scid), CLASS__(tcid), T__(tmr->scid), T__(tmr->tcid));
	return tmr;
}

static void knh_readyTransMapList(CTX, kcid_t cid)
{
	DBG_ASSERT_cid(cid);
	kclass_t *t = varClassTBL(cid);
	if(t->typemaps == K_EMPTYARRAY) {
		KNH_ASSERT(kArray_size(t->typemaps) == 0);
		KSETv(t->typemaps, new_Array0(_ctx, 0));
		KWRITE_BARRIER(t, t->typemaps);
	}
}

KNHAPI2(void) knh_addTypeMap(CTX, kTypeMap *tmr, int initCache)
{
	kcid_t cid = tmr->scid;
	knh_readyTransMapList(_ctx, cid);
	kArray_add(ClassTBL(cid)->typemaps, tmr);
	if(initCache) {
		Cache_setTypeMap(_ctx, tmr->scid, tmr->tcid, tmr);
	}
}

KNHAPI2(void) knh_TypeMap_exec(CTX, kTypeMap *tmr, ksfp_t *sfp _RIX)
{
	if(TypeMap_isNDATA(tmr)) {
		KSETv(sfp[K_TMRIDX].tmr, tmr);
		tmr->ftypemap_1(_ctx, sfp, K_RIX);
	}
	else {
		KNH_SAFEPOINT(_ctx, sfp);
		if(IS_NULL(sfp[0].o)) {
			KSETv(sfp[K_RIX].o, KNH_NULVAL(tmr->tcid))
			sfp[K_RIX].ivalue = 0;
		}
		else {
			sfp[K_TMRIDX].tmrNC = tmr;
			tmr->ftypemap_1(_ctx, sfp, K_RIX);
		}
	}
}

/* ------------------------------------------------------------------------ */

void knh_addTypeMapFunc(CTX, kflag_t flag, ktype_t stype, ktype_t ttype, knh_Ftypemap fTYPEMAP, Object *mapdata)
{
	kcid_t cid = CLASS_t(stype);
	DBG_ASSERT_cid(cid);
	knh_readyTransMapList(_ctx, cid);
	kArray_add(ClassTBL(cid)->typemaps, new_TypeMap(_ctx, flag, CLASS_t(stype), CLASS_t(ttype), fTYPEMAP));
}

/* ------------------------------------------------------------------------ */

static TYPEMAP Ftypemap_null(CTX, ksfp_t *sfp _RIX)
{
	KNH_TODO("null translator");
}

/* ------------------------------------------------------------------------ */

KNHAPI2(kTypeMap*) new_TypeMap(CTX, kflag_t flag, kcid_t scid, kcid_t tcid, knh_Ftypemap func)
{
	kTypeMap* tmr = new_(TypeMap);
	tmr->h.magicflag |= (uintptr_t)flag;
	tmr->scid = scid;
	tmr->tcid = tcid;
	tmr->ftypemap_1 = (func == NULL) ? Ftypemap_null : func;
	if(TY_isUnbox(scid)) {
		TypeMap_setNDATA(tmr, 1);
	}
	return tmr;
}

KNHAPI2(kTypeMap*) new_TypeMapData(CTX, kflag_t flag, kcid_t scid, kcid_t tcid, knh_Ftypemap func, Object *mapdata)
{
	kTypeMap* tmr = new_(TypeMap);
	tmr->h.magicflag |= (uintptr_t)flag;
	tmr->scid = scid;
	tmr->tcid = tcid;
	tmr->ftypemap_1 = (func == NULL) ? Ftypemap_null : func;
	KSETv(tmr->mapdata, mapdata);
	KWRITE_BARRIER(tmr, mapdata);
	if(TY_isUnbox(scid)) {
		TypeMap_setNDATA(tmr, 1);
	}
	return tmr;
}

static TYPEMAP Ftypemap_method(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr = sfp[K_TMRIDX].tmrNC;
	kMethod *mtd = tmr->mtd;
	ksfp_t *lsfp = ctx->esp;
	DBG_ASSERT(IS_Method(mtd));
	DBG_ASSERT(sfp < lsfp);
	KSETv(lsfp[0+K_CALLDELTA+1].o, sfp[0].o);
//	lsfp[0+K_CALLDELTA+1].ndata = sfp[0].ndata;  // no necessary
	KNH_SCALL(_ctx, lsfp, 0, mtd, 1);
	KSETv(sfp[K_RIX].o, lsfp[0].o);
	sfp[K_RIX].ndata = lsfp[0].ndata;
}

static TYPEMAP Ftypemap_methodN(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr = sfp[K_TMRIDX].tmr;
	kMethod *mtd = tmr->mtd;
	ksfp_t *lsfp = ctx->esp;
	DBG_ASSERT(IS_Method(mtd));
	DBG_ASSERT(sfp < lsfp);
//	KSETv(lsfp[0+K_CALLDELTA+1].o, sfp[0].o); // no necessary
	lsfp[0+K_CALLDELTA+1].ndata = sfp[0].ndata;
	KNH_SCALL(_ctx, lsfp, 0, mtd, 1);
	KSETv(sfp[K_RIX].o, lsfp[0].o);
	sfp[K_RIX].ndata = lsfp[0].ndata;
}

kTypeMap *new_TypeMapMethod(CTX, kflag_t flag, kMethod *mtd)
{
	kcid_t scid = knh_Param_get(DP(mtd)->mp, 0)->type, tcid = knh_Param_rtype(DP(mtd)->mp);
	knh_Ftypemap f = TY_isUnbox(scid) ? Ftypemap_methodN : Ftypemap_method;
	kTypeMap *tmr = new_TypeMap(_ctx, flag, scid, tcid, f);
	KSETv(tmr->mtd, mtd);
	KWRITE_BARRIER(tmr, mtd);
	return tmr;
}

static TYPEMAP Ftypemap_asis(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(sfp[0].o);
}
static TYPEMAP FtypemapN_asis(CTX, ksfp_t *sfp _RIX)
{
	RETURNd_(sfp[0].ndata);
}
static TYPEMAP Ftypemap_unbox(CTX, ksfp_t *sfp _RIX)
{
	RETURNd_(O_ndata(sfp[0].o));
}
static TYPEMAP FtypemapN_box(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr = sfp[K_TMRIDX].tmr; // N
	RETURN_(new_Boxing(_ctx, sfp, ClassTBL(tmr->scid)));
}
static TYPEMAP Ftypemap_dcast(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr0= sfp[K_TMRIDX].tmrNC;
	kTypeMap *tmr = knh_findTypeMapNULL(_ctx, O_cid(sfp[0].o), tmr0->tcid);
	if(tmr != NULL /*&& TypeMap_isSemantic(tmr)*/) {
		sfp[0].ndata = O_ndata(sfp[0].o);
		knh_TypeMap_exec(_ctx, tmr, sfp, K_RIX);
	}
	else {
		sfp[K_RIX].ndata = 0;
		RETURN_(KNH_NULVAL(tmr0->scid));
	}
}

static kTypeMap *knh_getSubtypeMap(CTX, kclass_t *sct, kclass_t *tct)
{
	knh_Ftypemap f = NULL;
	if(sct->cid == tct->cid || ClassTBL_isa_(_ctx, sct, tct) || tct->cid == CLASS_Tdynamic) {
		if(TY_isUnbox(sct->cid)) {
			f = (TY_isUnbox(tct->cid)) ? FtypemapN_asis : FtypemapN_box;
		}
		else {
			f = (TY_isUnbox(tct->cid)) ? Ftypemap_unbox : Ftypemap_asis;
		}
	}
	else if(sct->cid == CLASS_Tdynamic) {
		f = Ftypemap_dcast;
	}
	if(f != NULL) {
		return new_TypeMap(_ctx, FLAG_TypeMap_Semantic|FLAG_TypeMap_Interface, sct->cid, tct->cid, f);
	}
	return NULL;
}

///* ------------------------------------------------------------------------ */
///* [MapMap] */
//
//static TYPEMAP knh_Ftypemap_mapmap(CTX, ksfp_t *sfp _RIX)
//{
//	kTypeMap *trl = sfp[K_TMRIDX].trlNC;
//	DBG_ASSERT(IS_TypeMap(trl));
//	kTypeMap *m1 = DP(trl)->m1, *m2 = DP(trl)->m2;
//	DBG_ASSERT(IS_TypeMap(m1));
//	KSETv(sfp[1].o, m1);
//	(m1)->ftypemap_1(_ctx, sfp);
//	if(IS_NOTNULL(sfp[0].o)) {
//		DBG_ASSERT(IS_TypeMap(m2));
//		KSETv(sfp[1].o, m2);
//		(m2)->ftypemap_1(_ctx, sfp);
//	}
//}
//
///* ------------------------------------------------------------------------ */
//
//static kTypeMap* new_MapMap(CTX, kTypeMap *m1, kTypeMap *m2)
//{
//	kTypeMap* trl = (kTypeMap*)new_Object_bcid(_ctx, CLASS_TypeMap, 0);
//	KNH_SYSLOG(_ctx, LOG_NOTICE, "generated mapper: %C => %C => %C", DP(m1)->scid, DP(m1)->tcid, DP(m2)->tcid);
//	DP(trl)->size = 0;
//	DP(trl)->flag = DP(m1)->flag | DP(m2)->flag;
//	SP(trl)->scid = DP(m1)->scid;
//	SP(trl)->tcid = DP(m2)->tcid;
//	DBG_ASSERT(IS_TypeMap(m1));
//	KSETv(DP(trl)->m1, m1);
//	DBG_ASSERT(IS_TypeMap(m2));
//	KSETv(DP(trl)->m2, m2);
//	trl->h.flag = m1->h.flag & m2->h.flag;
//	trl->ftypemap_1 = knh_Ftypemap_mapmap;
//	return trl;
//	return NULL;
//}


//
//
///* ------------------------------------------------------------------------ */
///* [RangeInt] */
//
////## @Semantic @Const mapper RangeInt ArrayInt;
//
//static TYPEMAP RangeInt_ArrayInt(CTX, ksfp_t *sfp _RIX)
//{
//	kRange *rng = (kRange*)sfp[0].o;
//	kArray *a = new_Array(_ctx, CLASS_Int, (rng->nend - rng->nstart) + 1);
//	intptr_t i = 0, n;
//	for(n = (intptr_t)rng->nstart; n <= (intptr_t)rng->nend; n++) {
//		a->nlist[i] = n;
//		i++;
//	}
//	RETURN_(a);
//}


kbool_t TypeMap_isNoSuchMapping(kTypeMap *tmr)
{
	return ((tmr)->ftypemap_1 == Ftypemap_null);
}

void knh_addTypeMapRule(CTX, kcid_t scid, kcid_t tcid, knh_Ftypemaprule func)
{
	kuint_t hkey = (kuint_t)scid;
	hkey = (hkey << (sizeof(kcid_t) * 8)) + tcid;
	void *f = knh_PtrMap_get(_ctx, ctx->share->inferPtrMap, (void*)hkey);
	if(f != NULL) {

	}
	knh_PtrMap_add(_ctx, ctx->share->inferPtrMap, (void*)hkey, (void*)func);
}

static knh_Ftypemaprule knh_getTypeMapRule(CTX, kcid_t scid, kcid_t tcid)
{
	kuint_t hkey = (kuint_t)scid;
	hkey = (hkey << (sizeof(kcid_t) * 8)) + tcid;
	return (knh_Ftypemaprule)knh_PtrMap_get(_ctx, ctx->share->inferPtrMap, (void*)hkey);
}

static kTypeMap *knh_findTypeMap1NULL(CTX, kclass_t *sct, kcid_t tcid)
{
	kcid_t scid = sct->cid;
	kTypeMap *tmr = Cache_getTypeMap(_ctx, scid, tcid);
	if(tmr == NULL) {
		while(1) {
			size_t i;
			kArray *a = sct->typemaps;
			for(i = 0; i < (a)->size; i++) {
				tmr = (a)->trans[i];
				//DBG_P("?(%s,%s) looking %s=>%s..", T__(scid), T__(tcid), T__(sct->cid), T__(tmr->tcid));
				if(SP(tmr)->tcid == tcid) {
					return Cache_setTypeMap(_ctx, scid, tcid, tmr);
				}
			}
			if(sct->supTBL == sct) return NULL;
			sct = sct->supTBL;
		}
		tmr = NULL;
	}
	return tmr;
}

KNHAPI2(kTypeMap*) knh_findTypeMapNULL(CTX, kcid_t scid0, kcid_t tcid0)
{
	kTypeMap *tmr = NULL;
	DBG_P("finding.. %s ==> %s",CLASS__(scid0), CLASS__(tcid0));
	DBG_ASSERT(scid0 != tcid0);
	//if(scid0 == CLASS_Tvoid) return NULL;
	DBG_ASSERT(scid0 != CLASS_Tvar);
	{
		kuint_t h = hashcode_tmr(scid0, tcid0, K_TMAPCACHE_SIZE);
		knh_tmrcache_t *cache = ctx->tmrcache + h;
		if(cache->scid  == scid0 && cache->tcid == tcid0) {
			DBG_P("?(%s=>%s) HIT %s=>%s",CLASS__(scid0), CLASS__(tcid0), T__(cache->tmr->scid), T__(cache->tmr->tcid));
			return cache->tmr;
		}
	}
	DBG_ASSERT_cid(scid0); DBG_ASSERT_cid(tcid0);
	kclass_t *sct = ClassTBL(scid0), *tct = ClassTBL(tcid0);
	while(1) {
		//DBG_P("finding3.. %s ==> %s",CLASS__(sct->cid), CLASS__(tct->cid));
		tmr = knh_findTypeMap1NULL(_ctx, sct, tct->cid);
		if(tmr != NULL) return tmr;
		if(tct == tct->supTBL) break;
		tct = tct->supTBL;
	}

	tct = ClassTBL(tcid0);
	knh_Ftypemaprule frule = knh_getTypeMapRule(_ctx, sct->bcid, tct->bcid);
	if(frule != NULL) {
		tmr = frule(_ctx, sct, tct);
		if(tmr != NULL) {
			knh_addTypeMap(_ctx, tmr, 0/*initCache*/);
			return Cache_setTypeMap(_ctx, sct->cid, tct->cid, tmr);
		}
	}
	tmr = knh_getSubtypeMap(_ctx, sct, tct);
	if(tmr != NULL) {
		return Cache_setTypeMap(_ctx, sct->cid, tct->cid, tmr);
	}
//	size_t i;
//	kArray *a = sct->typemaps;
//	for(i = 0; i < (a)->size; i++) {
//		kTypeMap *tmr = (a)->trans[i], *tmr2;
//		if(!TypeMap_isSemantic(tmr)) continue;
//		tmr2 = knh_findTypeMapNULL(_ctx, tmr->tcid, tcid0, isT);
//		if(tmr2 != NULL && TypeMap_isSemantics(tmr2)) {
//			tmr = new_TypeMap2(_ctx, tmr, tmr2);
//			knh_addTypeMap(_ctx, tmr);
//			goto L_SETCACHE;
//		}
//	}
	return NULL;
}

/* ------------------------------------------------------------------------ */
/* [TypeMapRule] */

/* Iterator => Iterator */

static ITRNEXT Iterator_Onext(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(IS_bIterator(sfp[0].it));
	kIterator *this_itr = ITR(sfp);
	kTypeMap *tmr = DP(this_itr)->tmrNULL;
	kIterator *src_itr = (kIterator*)DP(this_itr)->source;
	KSETv(sfp[1].o, src_itr);
	while(1) {
		klr_setesp(_ctx, sfp+2);
		if(src_itr->fnext_1(_ctx, sfp + 1, 1)) { // => sfp[2]
			klr_setesp(_ctx, sfp+3);
			sfp[2].ndata = O_ndata(sfp[2].o);   // unbox
			KSETv(sfp[3].o, TS_EMPTY);  // set nonnull
			knh_TypeMap_exec(_ctx, tmr, sfp+2, +1);
			if(IS_NULL(sfp[3].o)) continue;
			sfp[K_RIX].ndata = O_ndata(sfp[3].o);
			ITRNEXT_(sfp[3].o);
		}
		break;
	}
	ITREND_();
}

static ITRNEXT Iterator_Nnext(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(IS_bIterator(sfp[0].it));
	kIterator *this_itr = ITR(sfp);
	kTypeMap *tmr = DP(this_itr)->tmrNULL;
	kIterator *src_itr = (kIterator*)DP(this_itr)->source;
	KSETv(sfp[1].o, src_itr);
	while(1) {
		klr_setesp(_ctx, sfp+2);
		if(src_itr->fnext_1(_ctx, sfp + 1, 1)) { // => sfp[2]
			klr_setesp(_ctx, sfp+3);
			knh_TypeMap_exec(_ctx, tmr, sfp+2, +1);
			sfp[K_RIX].ndata = sfp[3].ndata;
			KSETv(sfp[3].o, new_Boxing(_ctx, sfp+3, ClassTBL(tmr->scid)));
			ITRNEXT_(sfp[3].o);
		}
	}
	ITREND_();
}

static kIterator* new_IteratorIterator(CTX, kcid_t cid, kIterator *s)
{
	kIterator *it = new_O(Iterator, cid);
	KSETv(DP(it)->source, s);
	KWRITE_BARRIER(DP(it), s);
	it->fnext_1 = Iterator_isNDATA(s) ? Iterator_Nnext : Iterator_Onext;
	kcid_t sp1 = O_p0(s), tp1 = O_p0(it);
	kTypeMap *tmr = knh_findTypeMapNULL(_ctx, sp1, tp1);
	KNH_ASSERT(tmr != NULL);
	KINITv(DP(it)->tmrNULL, tmr);
	return it;
}

static TYPEMAP Iterator_Iterator(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr0 = sfp[K_TMRIDX].tmrNC;
	RETURN_(new_IteratorIterator(_ctx, tmr0->scid, sfp[0].it));
}

static kTypeMap *knh_inferIteratorIterator(CTX, kclass_t *sct, kclass_t *tct)
{
	kcid_t sp1 = sct->p1, tp1 = tct->p1;
	kTypeMap *tmr = knh_findTypeMapNULL(_ctx, sp1, tp1);
	if(tmr != NULL) {
		return new_TypeMap(_ctx, 0, sct->cid, tct->cid, Iterator_Iterator);
	}
	return NULL;
}

/* Array => Array */

static TYPEMAP Array_Array(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr = sfp[K_TMRIDX].tmrNC;
	kArray *sa = sfp[0].a;
	kArray *ta = (kArray*)new_Object_init2(_ctx, ClassTBL(tmr->tcid));
	size_t i, tsize = kArray_size(sa);
	if(tsize > 0) {
		kArray_grow(_ctx, ta, tsize, 8);
	}
	ksfp_t *lsfp = ctx->esp + 1;
	tmr = tmr->tmr1;
	//KSETv(lsfp[0].o, TS_EMPTY);  // set nonnull for unbox map
	for(i = 0; i < tsize; i++) {
		sa->api->get(_ctx, sa, i, lsfp);
		klr_setesp(_ctx, lsfp+1); //added
		knh_TypeMap_exec(_ctx, tmr, lsfp, +1);
		ta->api->add(_ctx, ta, lsfp+1);
	}
	RETURN_(ta);
}

static kTypeMap *knh_inferArrayArray(CTX, kclass_t *sct, kclass_t *tct)
{
	kcid_t sp1 = sct->p1, tp1 = tct->p1;
	kTypeMap *tmr = knh_findTypeMapNULL(_ctx, sp1, tp1);
	if(tmr != NULL) {
		return new_TypeMapData(_ctx, 0, sct->cid, tct->cid, Array_Array, UPCAST(tmr));
	}
	return NULL;
}

/* Array => Iterator */

static kArray* new_ArrayClass(CTX, kcid_t cid, size_t capacity)
{
	kArray *a = (kArray*)new_Object_init2(_ctx, ClassTBL(cid));
	if(capacity > 0) {
		kArray_grow(_ctx, a, capacity, 8);
	}
	return a;
}

static ITRNEXT Array_nextO(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(IS_bIterator(sfp[0].it));
	kIterator *itr = ITR(sfp);
	kArray *a = (kArray*)DP(itr)->source;
	size_t pos = (size_t)DP(itr)->m.index;
	if(pos < a->size) {
		DP(itr)->m.index = pos + 1;
		ITRNEXT_(a->list[pos]);
	}
	ITREND_();
}

static ITRNEXT Array_nextN(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(IS_bIterator(sfp[0].it));
	kIterator *itr = ITR(sfp);
	kArray *a = (kArray*)DP(itr)->source;
	size_t pos = (size_t)DP(itr)->m.index;
	if(pos < a->size) {
		DP(itr)->m.index = pos+1;
		ITRNEXTd_(a->nlist[pos]);
	}
	ITREND_();
}

kIterator* new_ArrayIterator(CTX, kArray *a)
{
	kcid_t cid = O_p0(a);
	knh_Fitrnext fnext = kArray_isUnboxData(a) ? Array_nextN : Array_nextO;
	return new_IteratorG(_ctx, knh_class_P1(_ctx, CLASS_Iterator, cid), UPCAST(a), fnext);
}

kbool_t knh_isArrayIterator(kIterator *itr)
{
	return (itr->fnext_1 == Array_nextO || itr->fnext_1 == Array_nextN);
}

static TYPEMAP Array_Iterator(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr0 = sfp[K_TMRIDX].tmrNC;
	knh_Fitrnext fnext = kArray_isUnboxData(sfp[0].a) ? Array_nextN : Array_nextO;
	RETURN_(new_IteratorG(_ctx, tmr0->tcid, sfp[0].o, fnext));
}

static TYPEMAP Array_MIterator(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr0 = sfp[K_TMRIDX].tmrNC;
	knh_Fitrnext fnext = kArray_isUnboxData(sfp[0].a) ? Array_nextN : Array_nextO;
	RETURN_(new_IteratorIterator(_ctx, tmr0->scid, new_IteratorG(_ctx, tmr0->tcid, sfp[0].o, fnext)));
}

static kTypeMap *knh_inferArrayIterator(CTX, kclass_t *sct, kclass_t *tct)
{
	kcid_t sp1 = sct->p1, tp1 = tct->p1;
	if(sp1 == tp1) {
		return new_TypeMap(_ctx, 0, sct->cid, tct->cid, Array_Iterator);
	}
	kTypeMap *tmr = knh_findTypeMapNULL(_ctx, sp1, tp1);
	if(tmr != NULL) {
		return new_TypeMap(_ctx, 0, sct->cid, tct->cid, Array_MIterator);
	}

	DBG_P("Rule(%s=>%s)", T__(sct->cid), T__(tct->cid));
	return NULL;
}

/* Iterator => Array */

static kArray* Iterator_toArray(CTX, kcid_t tcid, kIterator *itr)
{
	BEGIN_LOCAL(_ctx, lsfp, 2);
	kArray *a = new_ArrayClass(_ctx, tcid, 0);
	KSETv(lsfp[0].o, a);
	KSETv(lsfp[1].o, itr);
	klr_setesp(_ctx, lsfp+2);
	while(itr->fnext_1(_ctx, lsfp + 1, 1)) {
		a->api->add(_ctx, a, lsfp + 2);
		klr_setesp(_ctx, lsfp+2);
	}
	END_LOCAL(_ctx, lsfp);
	return a;
}

static kArray* Iterator_toMArray(CTX, kcid_t tcid, kIterator *itr, kTypeMap *tmr)
{
	BEGIN_LOCAL(_ctx, lsfp, 2);
	kArray *a = new_ArrayClass(_ctx, tcid, 0);
	KSETv(lsfp[0].o, a);
	KSETv(lsfp[1].o, itr);
	klr_setesp(_ctx, lsfp+2);
	while(itr->fnext_1(_ctx, lsfp + 1, 1)) {
		klr_setesp(_ctx, lsfp+3);
		knh_TypeMap_exec(_ctx, tmr, lsfp+2, +1);
		a->api->add(_ctx, a, lsfp+3);
		klr_setesp(_ctx, lsfp+2);
	}
	END_LOCAL(_ctx, lsfp);
	return a;
}

static TYPEMAP Iterator_Array(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmrNC = sfp[K_TMRIDX].tmrNC;
	RETURN_(Iterator_toArray(_ctx, tmrNC->tcid, sfp[0].it));
}

static TYPEMAP Iterator_MArray(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmrNC = sfp[K_TMRIDX].tmrNC;
	DBG_ASSERT(IS_TypeMap(tmrNC->tmr1));
	RETURN_(Iterator_toMArray(_ctx, tmrNC->tcid, sfp[0].it, tmrNC->tmr1));
}

static kTypeMap *knh_inferIteratorArray(CTX, kclass_t *sct, kclass_t *tct)
{
	kcid_t sp1 = sct->p1, tp1 = tct->p1;
	if(sp1 == tp1) {
		return new_TypeMap(_ctx, 0, sct->cid, tct->cid, Iterator_Array);
	}
	kTypeMap *tmr = knh_findTypeMapNULL(_ctx, sp1, tp1);
	if(tmr != NULL) {
		return new_TypeMapData(_ctx, 0, sct->cid, tct->cid, Iterator_MArray, UPCAST(tmr));
	}
	DBG_P("Rule(%s=>%s)", T__(sct->cid), T__(tct->cid));
	return NULL;
}

static ITRNEXT Map_nextO(CTX, ksfp_t *sfp _RIX)
{
	kIterator *itr = ITR(sfp);
	kMap *m = (kMap*)DP(itr)->source;
	ksfp_t *lsfp = ctx->esp;
	klr_setesp(_ctx, lsfp+1);
	if(m->spi->next(_ctx, m->mapptr, &(DP(itr)->m), lsfp)) {
		ITRNEXT_(lsfp[0].o);
	}
	else {
		ITREND_();
	}
}

static ITRNEXT Map_nextN(CTX, ksfp_t *sfp _RIX)
{
	kIterator *itr = ITR(sfp);
	kMap *m = (kMap*)DP(itr)->source;
	ksfp_t *lsfp = ctx->esp;
	klr_setesp(_ctx, lsfp+1);
	if(m->spi->next(_ctx, m->mapptr, &(DP(itr)->m), lsfp)) {
		ITRNEXTd_(lsfp[0].ndata);
	}
	else {
		ITREND_();
	}
}

static TYPEMAP Map_Iterator(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr0 = sfp[K_TMRIDX].tmrNC;
	kcid_t p1 = O_p0(sfp[0].m);
	knh_Fitrnext fnext = TY_isUnbox(p1) ? Map_nextN : Map_nextO;
	RETURN_(new_IteratorG(_ctx, tmr0->tcid, sfp[0].o, fnext));
}

static kTypeMap *knh_inferMapIterator(CTX, kclass_t *sct, kclass_t *tct)
{
	kcid_t sp1 = sct->p1, tp1 = tct->p1;
	if(sp1 == tp1) {
		return new_TypeMap(_ctx, 0, sct->cid, tct->cid, Map_Iterator);
	}
	return NULL;
}

/* ------------------------------------------------------------------------- */

static kbool_t knh_KonohaSpace_dataCheck(CTX, kKonohaSpace *ns, kcid_t cid, ksfp_t *sfp)
{
	kString *key = sfp[0].s;
	kObject *value = sfp[1].o;
	kmethodn_t mn = knh_getmn(_ctx, S_tobytes(key), MN_NEWID);
	DBG_P("key=%s", S_text(key));
	kMethod *mtd = knh_KonohaSpace_getMethodNULL(_ctx, ns, cid, MN_toSETTER(mn));
	if(mtd == NULL) {
		mtd = knh_KonohaSpace_addXSetter(_ctx, ns, ClassTBL(cid), O_cid(value), MN_toSETTER(mn));
		if(mtd == NULL) return 0;
	}
	KSETv(sfp[0].o, mtd); //
	if(knh_Method_psize(mtd) == 1) {
		ktype_t ptype = knh_Method_ptype(_ctx, mtd, 0, cid);
		if(ptype == O_cid(value) || ClassTBL_isa_(_ctx, O_ct(value), ClassTBL(ptype))) {
			return 1; // ok
		}
		kTypeMap *tmr = knh_findTypeMapNULL(_ctx, O_cid(value), ptype);
		if(tmr != NULL) {
			klr_setesp(_ctx, sfp+2);
			sfp[1].ndata = O_ndata(value); // unbox
			knh_TypeMap_exec(_ctx, tmr, sfp+1, 0);
			knh_boxing(_ctx, sfp+1, ptype);
			return 1;
		}
	}
	else {
		KNH_TODO("multi arguments");
	}
	return 0;
}

void knh_Object_fastset(CTX, kObject *o, kMethod *mtd, kObject *v)
{
	KNH_ASSERT(IS_Method(mtd));
	kindex_t idx = knh_Method_indexOfSetterField(mtd);
	if(idx != -1) {
		kObject *of = (kObject*)o;
		if(TY_isUnbox(O_cid(v))) {
			kunbox_t *df = (kunbox_t*)(of->fields + idx);
			df[0] = O_ndata(v);
		}
		else {
			KSETv(of->fields[idx], v);
			KWRITE_BARRIER(of, v);
		}
		return;
	}
	if(knh_Method_psize(mtd) == 1) {
		BEGIN_LOCAL(_ctx, lsfp, 0);
		size_t rtnidx = 0, thisidx = rtnidx + K_CALLDELTA;
		KSETv(lsfp[thisidx].o, o);
		KSETv(lsfp[thisidx+1].o, v);
		lsfp[thisidx+1].ndata = O_ndata(v);
		KNH_SCALL(_ctx, lsfp, rtnidx, mtd, 1);
		END_LOCAL(_ctx, lsfp)
	}
}

void knh_Object_setData(CTX, kObject *o, kMap *m, kKonohaSpace *ns, int Checked)
{
	ksfp_t *lsfp = ctx->esp;
	knitr_t mitrbuf = K_NITR_INIT, *mitr = &mitrbuf;
	klr_setesp(_ctx, lsfp+1);
	if(Checked) {
		while(m->spi->next(_ctx, m->mapptr, mitr, lsfp)) {
			klr_setesp(_ctx, lsfp+2);
			knh_Object_fastset(_ctx, o, lsfp[0].mtd, lsfp[1].o);
			klr_setesp(_ctx, lsfp+1);
		}
	}
	else {
		kcid_t cid = O_cid(o);
		while(m->spi->next(_ctx, m->mapptr, mitr, lsfp)) {
			if(knh_KonohaSpace_dataCheck(_ctx, ns, cid, lsfp)) {
				klr_setesp(_ctx, lsfp+2);
				knh_Object_fastset(_ctx, o, lsfp[0].mtd, lsfp[1].o);
			}
			klr_setesp(_ctx, lsfp+1);
		}
	}
	klr_setesp(_ctx, lsfp);
}

static TYPEMAP Map_Object(CTX, ksfp_t *sfp _RIX)
{
	kTypeMap *tmr0 = sfp[K_TMRIDX].tmrNC;
	BEGIN_LOCAL(_ctx, lsfp, 1);
	KSETv(lsfp[0].o, new_Object_init2(_ctx, ClassTBL(tmr0->tcid)));
	klr_setesp(_ctx, lsfp+1);
	knh_Object_setData(_ctx, lsfp[0].o, sfp[0].m, NULL, 0/*Checked*/);
	END_LOCAL(_ctx, lsfp);
	RETURN_(lsfp[0].o);
}

static kTypeMap *knh_inferMapObject(CTX, kclass_t *sct, kclass_t *tct)
{
	if(sct->p1 == CLASS_String) {
		return new_TypeMap(_ctx, 0, sct->cid, tct->cid, Map_Object);
	}
	return NULL;
}

static TYPEMAP Object_Map(CTX, ksfp_t *sfp _RIX)
{
	BEGIN_LOCAL(_ctx, lsfp, 1);
	kDictMap *dm = new_DictMap0(_ctx, 0, 1/*isCaseMap*/, NULL);
	KSETv(lsfp[0].o, dm);
	size_t rtnidx = 1, thisidx = rtnidx + K_CALLDELTA;
	kclass_t *ct = O_ct(sfp[0].o);
	kcid_t this_cid = ct->cid;
	while(ct->cid != CLASS_Object) {
		size_t i, size = kArray_size(ct->methods);
		for(i = 0; i < size; i++) {
			kMethod *mtd = ct->methods->methods[i];
			if(knh_Method_psize(mtd) == 0 && (MN_isGETTER(mtd->mn) || MN_isISBOOL(mtd->mn))) {
				kString *key = knh_getFieldName(_ctx, FN_UNMASK(mtd->mn));
				DBG_P("key=%s", S_text(key));
				KSETv(lsfp[thisidx].o, sfp[0].o);
				KNH_SCALL(_ctx, lsfp, rtnidx, mtd, 0);
				ktype_t rtype = knh_Method_rtype(_ctx, mtd, this_cid);
				knh_boxing(_ctx, lsfp+rtnidx, rtype);
				knh_DictMap_append(_ctx, dm, key, lsfp[rtnidx].o);
			}
		}
		ct = ct->supTBL;
	}
//	knh_DictMap_sort(_ctx, dm);
	END_LOCAL(_ctx, lsfp);
	RETURN_(dm);
}

static void ClassTBL_checkGetter(CTX, kclass_t *sct)
{
	size_t i;
	for(i = 0; i < sct->fsize; i++) {
		ktype_t ftype = sct->fields[i].type;
		if(!FLAG_is(sct->fields[i].flag, kField_Getter)) continue;
		if(ftype == TY_void) continue;
		kmethodn_t mn = (ftype == TY_Boolean) ? MN_toISBOOL(sct->fields[i].fn) : MN_toGETTER(sct->fields[i].fn);
		knh_KonohaSpace_getMethodNULL(_ctx, NULL, sct->cid, mn);
	}
}

static kTypeMap *knh_inferObjectMap(CTX, kclass_t *sct, kclass_t *tct)
{
	if(tct->p1 == CLASS_String && tct->p2 == CLASS_Tdynamic) {
		ClassTBL_checkGetter(_ctx, sct);

		return new_TypeMap(_ctx, 0, sct->cid, tct->cid, Object_Map);
	}
	return NULL;
}

/* ------------------------------------------------------------------------ */

void knh_loadSystemTypeMapRule(CTX)
{
	knh_addTypeMapRule(_ctx, CLASS_Iterator, CLASS_Array,    knh_inferIteratorArray);
	knh_addTypeMapRule(_ctx, CLASS_Iterator, CLASS_Iterator, knh_inferIteratorIterator);
//	knh_addTypeMapRule(_ctx, CLASS_Tdynamic, CLASS_Iterator, knh_inferDynamicIterator);
	knh_addTypeMapRule(_ctx, CLASS_Array,    CLASS_Iterator, knh_inferArrayIterator);
	knh_addTypeMapRule(_ctx, CLASS_Array,    CLASS_Array,    knh_inferArrayArray);
	knh_addTypeMapRule(_ctx, CLASS_Map,      CLASS_Iterator,    knh_inferMapIterator);
	knh_addTypeMapRule(_ctx, CLASS_Map,      CLASS_Object,   knh_inferMapObject);
	knh_addTypeMapRule(_ctx, CLASS_Object,   CLASS_Map,      knh_inferObjectMap);

}

/* ------------------------------------------------------------------------ */

void knh_KonohaSpace_setLinkClass(CTX, kKonohaSpace *ns, kbytes_t linkname, kclass_t *ct)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	knh_Bytes_write(_ctx, cwb->ba, knh_bytes_head(linkname, ':'));
	kwb_putc(cwb->ba, ':');
	if(DP(ns)->name2ctDictSetNULL == NULL) {
		KINITv(DP(ns)->name2ctDictSetNULL, new_DictSet0(_ctx, 0, 1/*isCaseMap*/, "KonohaSpace.name2cid"));
	}
	knh_DictSet_set(_ctx, DP(ns)->name2ctDictSetNULL, CWB_newString(_ctx, cwb, SPOL_POOLNEVER|SPOL_ASCII), (uintptr_t)ct);
}

kclass_t *knh_KonohaSpace_getLinkClassTBLNULL(CTX, kKonohaSpace *ns, kbytes_t path, kcid_t tcid)
{
	if(path.text[0] == 't' && path.text[1] == 'o' && (path.text[2] == ':' || path.text[2] == 0)) {
		if(CLASS_Converter <= tcid && tcid <= CLASS_StringConverter) return ClassTBL(tcid);
		return ClassTBL(CLASS_Converter);
	}
	kbytes_t scheme = knh_bytes_head(path, ':');
	kcid_t cid = TY_unknown; /* = knh_KonohaSpace_getCT(_ctx, ns, scheme);*/
	if(islower(scheme.buf[0]) && scheme.len < 81) {
		char buf[128] = {0}; // zero clear
		knh_memcpy(buf, scheme.text, scheme.len);
		buf[scheme.len] = ':';
		cid = knh_KonohaSpace_getCT(_ctx, ns, B(buf));
	}
	if(cid == TY_unknown) {
		cid = knh_KonohaSpace_getCT(_ctx, ns, scheme);
	}
	if(cid != TY_unknown) {
		kMethod *mtd = knh_KonohaSpace_getMethodNULL(_ctx, ns, cid, MN_opLINK);
		if(mtd != NULL) return ClassTBL(cid);
	}
	return NULL;
}

kcid_t knh_ClassTBL_linkType(CTX, kclass_t *ct, kcid_t tcid)
{
	if(tcid == CLASS_String || tcid == CLASS_Boolean) return tcid;
	if(ct->cid == tcid || ClassTBL_isa_(_ctx, ct, ClassTBL(tcid))) {
		return ct->cid;
	}
	kTypeMap *tmr = knh_findTypeMapNULL(_ctx, ct->cid, tcid);
	return (tmr != NULL) ? tmr->tcid : TY_unknown;
}

kObject *knh_KonohaSpace_newObject(CTX, kKonohaSpace *ns, kString *path, kcid_t tcid)
{
	if(tcid == CLASS_String) return UPCAST(path);
	kclass_t *ct = knh_KonohaSpace_getLinkClassTBLNULL(_ctx, ns, S_tobytes(path), tcid);
	Object *value = NULL;
	if(ct == NULL) {
		if(tcid == CLASS_Boolean) return K_FALSE;
		LANG_LOG("link not found: %s as %s", S_text(path), CLASS__(tcid));
		return KNH_NULVAL(tcid);
	}
	else {
		BEGIN_LOCAL(_ctx, lsfp, 3 + K_CALLDELTA);
		kMethod *mtd = knh_KonohaSpace_getMethodNULL(_ctx, ns, ct->cid, MN_opLINK);
		DBG_ASSERT(mtd != NULL);
		long rtnidx = 0, thisidx = rtnidx + K_CALLDELTA;
		KSETv(lsfp[thisidx+1].o, path);
		KSETv(lsfp[thisidx+2].o, ns);
		KNH_SCALL(_ctx, lsfp, rtnidx, mtd, (3));
		knh_boxing(_ctx, lsfp + rtnidx, ct->cid);
		value = lsfp[rtnidx].o;
		END_LOCAL(_ctx, lsfp);
	}
	if(O_cid(value) == tcid || ClassTBL_isa_(_ctx, O_ct(value), ClassTBL(tcid))) {
		return value;
	}
	kTypeMap *tmr = knh_findTypeMapNULL(_ctx, O_cid(value), tcid);
	if(tmr == NULL) {
		if(tcid == CLASS_Boolean) {
			DBG_P("IS_NULL(value)=%d", IS_NULL(value));
			value = (IS_NULL(value)) ? K_FALSE : K_TRUE;
			return value;
		}
		return KNH_NULVAL(tcid);
	}
	else {
		BEGIN_LOCAL(_ctx, lsfp, 1);
		KSETv(lsfp[0].o, value);
		lsfp[0].ndata = O_ndata(value);
		klr_setesp(_ctx, lsfp+1);
		knh_TypeMap_exec(_ctx, tmr, lsfp, 1);
		knh_boxing(_ctx, lsfp, tmr->tcid);
		value = lsfp[1].o;
		END_LOCAL(_ctx, lsfp);
	}
	return value;
}

/* ------------------------------------------------------------------------ */

// boolean Object.hasMethod();
static KMETHOD Object_hasMethod(CTX, ksfp_t *sfp _RIX)
{
	kmethodn_t mn = knh_getmn(_ctx, S_tobytes(sfp[1].s), MN_NONAME);
	kMethod *mtd = knh_KonohaSpace_getMethodNULL(_ctx, NULL, O_cid(sfp[0].o), mn);
	RETURNb_(mtd != NULL);
}

// dynamic Object.invokeMethod(String method, dynamic a, ...);
static KMETHOD Object_invokeMethod(CTX, ksfp_t *sfp _RIX)
{
	kmethodn_t mn = knh_getmn(_ctx, S_tobytes(sfp[1].s), MN_NONAME);
	kMethod *mtd = knh_KonohaSpace_getMethodNULL(_ctx, NULL, O_cid(sfp[0].o), mn);
	if(mtd == NULL) {
		LANG_LOG("Object.invokeMethod: no such method %s.%s", S_text(O_ct(sfp[0].o)->lname), S_text(sfp[1].s));
		RETURN_(K_NULL);
	}
	size_t i, psize = knh_Method_psize(mtd), rtnidx = K_RIX;
	kcid_t this_cid = O_cid(sfp[0].o);
	ksfp_t *esp = ctx->esp;
	for(i = 0; i < psize; i++) {
		ktype_t ptype = knh_Method_ptype(_ctx, mtd, i, this_cid);
		Object *v = sfp + (2+i) < esp ? sfp[2+i].o: KNH_NULVAL(ptype);
		ktype_t vtype = O_cid(v);
		if(vtype == ptype || ClassTBL_isa_(_ctx, O_ct(v), ClassTBL(ptype))) {
			if(TY_isUnbox(ptype)) {
				sfp[1+i].ndata = O_ndata(v);
			}
			else {
				KSETv(sfp[1+i].o, v);
			}
			continue;
		}
		kTypeMap *tmr = knh_findTypeMapNULL(_ctx, vtype, ptype);
		if(tmr != NULL) {
			if(TY_isUnbox(vtype)) {
				esp[0].ndata = O_ndata(v);
			}
			else {
				KSETv(esp[0].o, v);
			}
			knh_TypeMap_exec(_ctx, tmr, esp, &sfp[i+1] - esp);
			continue;
		}
		LANG_LOG("Object.invokeMethod: type error %s.%s(#%d)", S_text(O_ct(sfp[0].o)->lname), MN__(mn), i+1);
		RETURN_(K_NULL);
	}
	KNH_SCALL(_ctx, sfp, rtnidx, mtd, psize);
}

static kbool_t ClassTBL_addXField(CTX, kclass_t *ct, ktype_t type, kString *name)
{
	ksymbol_t fn = knh_getmn(_ctx, S_tobytes(name), FN_NEWID);  // FIXME: NOIZE
	kMethod *mtd = knh_ClassTBL_findMethodNULL(_ctx, ct, MN_toSETTER(fn), 0);
	if(mtd != NULL) {
		LANG_LOG("already defined setter: %s.%s", S_text(ct->lname), S_text(name));
		return 0;
	}
	mtd = knh_ClassTBL_findMethodNULL(_ctx, ct, (type == CLASS_Boolean) ? MN_toISBOOL(fn) : MN_isGETTER(fn), 0);
	if(mtd != NULL) {
		LANG_LOG("already defined getter: %s.%s", S_text(ct->lname), S_text(name));
		return 0;
	}
	knh_ClassTBL_addXField(_ctx, ct, type, fn);
	return 1;
}

// boolean System.addClassField(Class c, Class type, String name)
static KMETHOD System_addClassField(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(ClassTBL_addXField(_ctx, (sfp[1].c)->ct, (sfp[2].c)->type, sfp[3].s));
}

static kArray *new_MethodList(CTX, kclass_t *ct, kKonohaSpace *ns)
{
	kArray *ma = new_Array(_ctx, CLASS_Method, kArray_size(ct->methods));
	size_t i;
	for(i = 0; i < kArray_size(ct->methods); i++) {
		kMethod *mtd = ct->methods->methods[i];
		if(MN_isFMT(mtd->mn) || Method_isHidden(mtd)) continue;
		kArray_add(ma, mtd);
	}
	while(ns != NULL) {
		if(DP(ns)->methodsNULL != NULL) {
			kArray *methods = DP(ns)->methodsNULL;
			for(i = 0; i < kArray_size(methods); i++) {
				kMethod *mtd = methods->methods[i];
				if(mtd->cid != ct->cid) continue;
				if(MN_isFMT(mtd->mn) || Method_isHidden(mtd)) continue;
				kArray_add(ma, mtd);
			}
		}
		ns = ns->parentNULL;
	}
	return ma;
}

// Method[] Object.getMethods(KonohaSpace _)
static KMETHOD Object_getMethods(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_MethodList(_ctx, O_ct(sfp[0].o), sfp[1].ns));
}

// Method[] Object.getMethods(KonohaSpace _)
static KMETHOD Class_getMethods(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_MethodList(_ctx, (sfp[0].c)->ct, sfp[1].ns));
}

// Class Class.opLINK(String path, KonohaSpace _)
static KMETHOD Class_opLINK(CTX, ksfp_t *sfp _RIX)
{
//	kKonohaSpace *ns = sfp[2].ns;
	kbytes_t bpath = knh_bytes_next(S_tobytes(sfp[1].s), ':');
	kcid_t cid = knh_getcid(_ctx, bpath);
	if(cid == TY_unknown) {
		bpath = knh_bytes_head(bpath, '<');
		cid = knh_getcid(_ctx, bpath);
		if(cid == TY_unknown) {
			cid = CLASS_Tvoid;
		}
	}
	RETURN_(new_Type(_ctx, cid));
}

// @Static Class[] Class.query(String query)
static KMETHOD Class_query(CTX, ksfp_t *sfp _RIX)
{
	size_t i, csize = ctx->share->sizeClassTBL;
	kArray *ca = new_Array(_ctx, CLASS_Class, csize);
	kbytes_t q = S_tobytes(sfp[1].s);
	for(i = 0; i < csize; i++) {
		kcid_t cid = (kcid_t)i;
		kclass_t *ct = ClassTBL(cid);
		DBG_P("lname=%s", S_text(ct->lname));
		if(!knh_bytes_startsWith_(S_tobytes(ct->lname), q)) continue;
		kArray_add(ca, new_Type(_ctx, cid));
	}
	RETURN_(ca);
}

// Method Method.opLINK(String path, KonohaSpace _)
static KMETHOD Method_opLINK(CTX, ksfp_t *sfp _RIX)
{
	kKonohaSpace *ns = sfp[2].ns;
	kbytes_t bpath = knh_bytes_next(S_tobytes(sfp[1].s), ':');
	kindex_t loc = knh_bytes_rindex(bpath, '.');
	kMethod *mtd = NULL;
	if(loc != -1) {
		kbytes_t cpath = knh_bytes_first(bpath, loc);
		kbytes_t mpath = knh_bytes_last(bpath, loc+1);
		kcid_t cid = knh_KonohaSpace_getCT(_ctx, ns, cpath);
		kmethodn_t mn = knh_getmn(_ctx, mpath, MN_NONAME);
		if(cid != TY_unknown && mn != MN_NONAME) {
			mtd = knh_KonohaSpace_getMethodNULL(_ctx, ns, cid, mn);
		}
	}
	if(mtd == NULL) {
		mtd = KNH_TNULL(Method);
	}
	RETURN_(mtd);
}

// @Const Boolean Method.isAbstract();
static KMETHOD Method_isAbstract_(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(Method_isAbstract(sfp[0].mtd));
}

// @Const Boolean Method.isPrivate();
static KMETHOD Method_isPrivate_(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(Method_isPrivate(sfp[0].mtd));
}

// @Const String Method.getName();
static KMETHOD Method_getName(CTX, ksfp_t *sfp _RIX)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	knh_write_mn(_ctx, cwb->w, (sfp[0].mtd)->mn);
	RETURN_(CWB_newString(_ctx, cwb, SPOL_POOLNEVER|SPOL_ASCII));
}

// @Const Class Method.getReturnType();
static KMETHOD Method_getReturnType(CTX, ksfp_t *sfp _RIX)
{
	ktype_t rtype = knh_Param_rtype(DP(sfp[0].mtd)->mp);
	if(rtype >= T_This) {
		rtype = ktype_tocid(_ctx, rtype, (sfp[0].mtd)->cid);
	}
	RETURN_(new_Type(_ctx, rtype));
}

// @Const Class[] Method.getParamTypes();
static KMETHOD Method_getParamTypes(CTX, ksfp_t *sfp _RIX)
{
	kParam *pa = DP(sfp[0].mtd)->mp;
	kArray *ma = new_Array(_ctx, CLASS_Method, pa->psize);
	size_t i;
	for(i = 0; i < pa->psize; i++) {
		ktype_t ptype = knh_Param_get(pa, i)->type;
		if(ptype >= T_This) {
			ptype = ktype_tocid(_ctx, ptype, (sfp[0].mtd)->cid);
		}
		kArray_add(ma, new_Type(_ctx, ptype));
	}
	RETURN_(ma);
}

// @Const String[] Method.getParamNames();
static KMETHOD Method_getParamNames(CTX, ksfp_t *sfp _RIX)
{
	kParam *pa = DP(sfp[0].mtd)->mp;
	kArray *ma = new_Array(_ctx, CLASS_String, pa->psize);
	size_t i;
	for(i = 0; i < pa->psize; i++) {
		ksymbol_t fn = knh_Param_get(pa, i)->fn;
		kArray_add(ma, knh_getFieldName(_ctx, fn));
	}
	RETURN_(ma);
}

// @Const String Method.getSourceCode();
static KMETHOD Method_getSourceCode(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[0].mtd;
	kTerm *tk = DP(mtd)->tsource;
	RETURN_(IS_Term(tk)
			? tk->text : KNH_TNULL(String));
}

#define FuncData(X) {#X , X}
static knh_FuncData_t FuncData[] = {
	FuncData(Object_hasMethod),
	FuncData(Object_invokeMethod),
	FuncData(System_addClassField),
	FuncData(Object_getMethods),
	FuncData(Class_getMethods),
	FuncData(Class_opLINK),
	FuncData(Class_query),
	FuncData(Method_opLINK),
	{"Method_isAbstract", Method_isAbstract_},
	{"Method_isPrivate", Method_isPrivate_},
	FuncData(Method_getName),
	FuncData(Method_getReturnType),
	FuncData(Method_getParamTypes),
	FuncData(Method_getParamNames),
	FuncData(Method_getSourceCode),
	{NULL, NULL},
};

void knh_initClassFuncData(CTX, const knh_LoaderAPI_t *kapi)
{
	kapi->loadFuncData(_ctx, FuncData);
}


/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
