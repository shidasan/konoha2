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
#include"../../include/konoha2/konoha_code_.h"
#include"../../include/konoha2/konohalang.h"

#ifndef T_BytesIm
#define CLASS_BytesIm CLASS_Bytes
#define T_BytesIm  T_Bytes
#endif

#ifndef TY_ArrayIm
#define CLASS_ArrayIm CLASS_Array
#define TY_ArrayIm  TY_Array
#endif

#ifndef T_MapIm
#define CLASS_MapIm CLASS_Map
#define T_MapIm  T_Map
#endif

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* --------------- */
/* Data */
/* --------------- */

#define K_INCLUDE_BUILTINAPI
#include "dspi.c"

#ifdef K_USING_BMGC
#define knh_bodyfree(_ctx, p, C)
#else
#define knh_bodymalloc(_ctx, C)   (knh_##C##EX_t*)KCALLOC(sizeof(knh_##C##EX_t))
#define knh_bodyfree(_ctx, p, C)  KFREE(_ctx, p, sizeof(knh_##C##EX_t))
#endif

/* ------------------------------------------------------------------------ */
/* DEFAULT */

static void DEFAULT_init(CTX, kObject *o)
{
//	DBG_ASSERT((sizeof(kObjectUnused) - sizeof(kObjectHeader)) == sizeof(intptr_t) * 4);
//	intptr_t *p = (intptr_t*)&(o->rawptr);
//	p[0] = KINT0; p[1] = KINT0; p[2] = KINT0; p[3] = KINT0;
}

static void DEFAULT_initcopy(CTX, kObject *dst, kObject *src)
{
	KNH_TODO("copy operation");
}

kbool_t knh_class_canObjectCopy(CTX, kcid_t cid)
{
	kclass_t *ct = ClassTBL(cid);
	return (ct->cdef->initcopy != DEFAULT_initcopy);
}

static void DEFAULT_reftrace(CTX, kObject *o)
{
}

static void DEFAULT_free(CTX, kObject *o)
{
}

static void DEFAULT_checkin(CTX, ksfp_t *sfp, kObject *o)
{
}

static void DEFAULT_checkout(CTX, kObject *o, int isFailed)
{
}

static int DEFAULT_compareTo(kObject *o1, kObject *o2)
{
	return (int)((intptr_t)o1 - (intptr_t)o2);
}

static void DEFAULT_p(CTX, kOutputStream *w, kObject *o, int level)
{
	KNH_LOG("TODO: must be defined %s_p", O__(o));
	knh_write_type(_ctx, w, O_cid(o));
	kwb_putc(wb, ':');
	knh_write_ptr(_ctx, w, (void*)o);
}

static void knh_write_TObject(CTX, kOutputStream *w, ktype_t type, Object **v, size_t i, int level)
{
	switch(type) {
		case CLASS_Boolean: {
			kbool_t *d = (kbool_t*)(v+i);
			knh_write_bool(_ctx, w, d[0]);
			break;
		}
		case CLASS_Int: {
			kint_t *d = (kint_t*)(v+i);
			knh_write_ifmt(_ctx, w, KINT_FMT, d[0]);
			break;
		}
		case CLASS_Float: {
			kfloat_t *d = (kfloat_t*)(v+i);
			knh_write_ffmt(_ctx, w, KFLOAT_FMT, d[0]);
			break;
		}
		default:{
			knh_write_InObject(_ctx, w, v[i], level);
		}
	}
}

static kString* DEFAULT_getkey(CTX, ksfp_t *sfp)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	knh_write_type(_ctx, cwb->w, O_cid(sfp[0].o));
	knh_putc(_ctx, cwb->w, ':');
	knh_write_ptr(_ctx, cwb->w, sfp[0].o);
	return CWB_newString(_ctx, cwb, 0);
}

static kuint_t DEFAULT_hashCode(CTX, kObject *o)
{
	return ((kuint_t)o) / (sizeof(void*) * 8);
}

static void DEFAULT_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{

}

#define DEFAULT_0 NULL
#define DEFAULT_1 NULL
#define DEFAULT_findTypeMapNULL NULL
#define DEFAULT_2 NULL
#define DEFAULT_3 NULL
#define DEFAULT_4 NULL
#define DEFAULT_5 NULL
//#define DEFAULT_6 NULL
#define sizeof_O(T) (sizeof(k##T))

static kclass_t TvoidDef = {
	DEFAULT_init, DEFAULT_initcopy, DEFAULT_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"void", CFLAG_Tvoid, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
};

static kclass_t TvarDef = {
	DEFAULT_init, DEFAULT_initcopy, DEFAULT_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"var", CFLAG_Tvar, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
};

static kclass_t TdynamicDef = {
	DEFAULT_init, DEFAULT_initcopy, DEFAULT_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"dynamic", CFLAG_Tvar, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
};

kclass_t* knh_getDefaultClassDef(void)
{
	return &TdynamicDef;
}

void knh_ClassTBL_setConstPool(CTX, kclass_t *ct)
{
	if(ct->constPoolMapNULL == NULL) {
		KINITv(((struct _kclass*)ct)->constPoolMapNULL, new_PtrMap(_ctx, 0));
	}
}

/* --------------- */
/* Object */

static void ObjectField_init(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
	kclass_t *ct = O_ct(o);
	if(ct->fsize > 0) {
		Object **v = &(of->smallobject);
		if(ct->fsize > K_SMALLOBJECT_FIELDSIZE) {
			v = (Object**)KCALLOC(ct->fsize * sizeof(kObject*));
		}
		of->fields = v;
		knh_memcpy(v, ct->protoNULL->fields, ct->fsize * sizeof(kObject*));
#ifdef K_USING_RCGC
		size_t i;
		for(i = 0; i < ct->fsize; i++) {
			if(!ct->fields[i].israw) {
				knh_Object_RCinc(v[i]);
			}
		}
#endif
	}
	else {
		of->fields = NULL;
	}
}

static void CppObject_init(CTX, kObject *o)
{
	kclass_t *ct = O_ct(o);
	o->rawptr = NULL;
	if(ct->fsize > 0) {
		o->kfields = (Object**)KCALLOC(ct->fsize * sizeof(kObject*));
		knh_memcpy(o->kfields, ct->protoNULL->fields, ct->fsize * sizeof(kObject*));
#ifdef K_USING_RCGC
		size_t i;
		for(i = 0; i < ct->fsize; i++) {
			if(!ct->fields[i].israw) {
				knh_Object_RCinc(o->kfields[i]);
			}
		}
#endif
	}
	else {
		o->kfields = NULL;
	}
}

static void ObjectField_initcopy(CTX, kObject *o, kObject *src)
{
	kObject *of = (kObject*)o;
	kclass_t *t = O_ct(o);
	if(t->fsize > 0) {
		Object **v = &(of->smallobject);
		if(t->fsize > K_SMALLOBJECT_FIELDSIZE) {
			v = (Object**)KCALLOC(t->fsize * sizeof(kObject*));
		}
		of->fields = v;
		knh_memcpy(v, src->rawptr, t->fsize * sizeof(kObject*));
#ifdef K_USING_RCGC
		size_t i;
		for(i = 0; i < t->fsize; i++) {
			if(!t->fields[i].israw) {
				knh_Object_RCinc(v[i]);
			}
		}
#endif
	}
	else {
		of->fields = NULL;
	}
}

static void ObjectField_reftrace(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
	kclass_t *ct = O_ct(o);
	size_t i;
	for(i = 0; i < ct->fsize; i++) {
		if(ct->fields[i].israw == 0) {
			KREFTRACEv(of->fields[i]);
		}
	}
	kref_setSize();
}

static void CppObject_reftrace(CTX, kObject *o)
{
	kclass_t *ct = O_ct(o);
	size_t i;
	for(i = 0; i < ct->fsize; i++) {
		if(ct->fields[i].israw == 0) {
			KREFTRACEv(o->kfields[i]);
		}
	}
	kref_setSize();
}

static void ObjectField_free(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
	kclass_t *ct = O_ct(o);
	if(ct->fsize > K_SMALLOBJECT_FIELDSIZE) {
		KFREE(_ctx, of->fields, ct->fsize * sizeof(kObject*));
	}
	DBG_(of->fields = NULL);
}

static void CppObject_free(CTX, kObject *o)
{
	kclass_t *ct = O_ct(o);
	if(ct->fsize > 0) {
		KFREE(_ctx, o->kfields, ct->fsize * sizeof(kObject*));
		o->kfields = NULL;
	}
	if(o->rawptr != NULL) {
//		DBG_P("freeing %s %p", o->DBG_NAME, o->rawptr);
		o->rawfree(o->rawptr);
		o->rawptr = NULL;
	}
}

static void CppObject_checkout(CTX, kObject *o, int isFailed)
{
//	if(o->rawfree != NULL) {
//		DBG_P("freeing %s %p", o->DBG_NAME, o->rawptr);
//		o->rawfree(o->rawptr);
//		o->rawptr = NULL;
//		o->rawfree = NULL;
//	}
}

static int ObjectField_compareTo(kObject *o, kObject *o2)
{
	return o - o2;
}

static void ObjectField_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kwb_putc(wb, '{');
	{
		kfieldinfo_t *tf = O_ct(o)->fields;
		size_t i, fsize = O_ct(o)->fsize;
		Object **v = (Object**)o->rawptr;
		if(fsize > 0) {
			knh_write_fn(_ctx, w, tf[0].fn);
			knh_write(_ctx, w, STEXT(": "));
			knh_write_TObject(_ctx, w, tf[0].type, v, 0, level);
		}
		for(i = 1; i < fsize; i++) {
			if(tf[i].type == TY_void) continue;
			knh_write_delim(_ctx, w);
			knh_write_fn(_ctx, w, tf[i].fn);
			knh_write(_ctx, w, STEXT(": "));
			knh_write_TObject(_ctx, w, tf[i].type, v, i, level);
		}
	}
	kwb_putc(wb, '}');
}

static kString* ObjectField_getkey(CTX, ksfp_t *sfp)
{
	int kwx = O_ct(sfp[0].o)->kwx;
	if(kwx != -1) {
		kObject *of = (kObject*)sfp[0].o;
		DBG_ASSERT(IS_bString(of->fields[kwx]));
		return (kString*)of->fields[kwx];
	}
	return DEFAULT_getkey(_ctx, sfp);
}

static void pack_unbox(CTX, void *pkr, kcid_t cid, kObject **v, const knh_PackSPI_t *packspi)
{
	knbody_t n = ((knbody_t*)(v))[0];
	if (IS_Tint(cid)) {
		packspi->pack_int(_ctx, pkr, n.ivalue);
	} else if (IS_Tfloat(cid)) {
		packspi->pack_float(_ctx, pkr, n.fvalue);
	} else {
		packspi->pack_bool(_ctx, pkr, n.bvalue);
	}
}

static void Object_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{
	kclass_t *ct = O_ct(o);
	kObject *of = (kObject*) o;
	Object **v = of->fields;
	size_t i = 0, field_count = ct->fsize;
	DBLNDATA_(
			for (i = 0; i < ct->fsize; i++) {
			kfieldinfo_t *field = ct->fields + i;
			if (field->type == CLASS_Tvoid) {
			field_count--;
			}
	});

	// { "ks:class" : "main.Person", 
	packspi->pack_beginmap(_ctx, pkr, field_count + 1);
	packspi->pack_string(_ctx, pkr, "ks:class", knh_strlen("ks:class"));
	packspi->pack_putc(_ctx, pkr, ':');
	packspi->pack_string(_ctx, pkr, S_text(ct->sname), S_size(ct->sname));
	packspi->pack_putc(_ctx, pkr, ',');

	for (i = 0; i < ct->fsize; i++) {
		kfieldinfo_t *field = ct->fields + i;
		ktype_t type = field->type;
		kString *key;

		if (type == CLASS_Tvoid) continue;
		if (i != 0) {
			packspi->pack_putc(_ctx, pkr, ',');
		}
		key = knh_getFieldName(_ctx, field->fn);
		packspi->pack_string(_ctx, pkr, S_text(key), S_size(key));
		packspi->pack_putc(_ctx, pkr, ':');
		if (TY_isUnbox(type)) {
			pack_unbox(_ctx, pkr, type, v + i, packspi);
		} else {
			kObject *obj = v[i];
			O_ct(obj)->cdef->wdata(_ctx, RAWPTR(obj), pkr, packspi);
		}
	}
	packspi->pack_endmap(_ctx, pkr);
}

static kclass_t ObjectDef = {
	ObjectField_init, ObjectField_initcopy, ObjectField_reftrace, ObjectField_free,
	DEFAULT_checkin, DEFAULT_checkout, ObjectField_compareTo, ObjectField_p,
	ObjectField_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, Object_wdata, DEFAULT_2, DEFAULT_3,
	"Object", CFLAG_Object, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
};

static void ObjectFieldN_init(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
	kclass_t *t = O_ct(o);
	if(t->fsize > 0) {
		Object **v = &(of->smallobject);
		if(t->fsize > K_SMALLOBJECT_FIELDSIZE) {
			v = (Object**)KCALLOC(t->fsize * sizeof(kObject*));
		}
		of->fields = v;
		knh_memcpy(v, t->protoNULL->fields, t->fsize * sizeof(kObject*));
	}
	else {
		of->fields = NULL;
	}
}

static void ObjectFieldN_initcopy(CTX, kObject *o, kObject *src)
{
	kObject *of = (kObject*)o;
	kclass_t *ct = O_ct(o);
	if(ct->fsize > 0) {
		Object **v = &(of->smallobject);
		if(ct->fsize > K_SMALLOBJECT_FIELDSIZE) {
			v = (Object**)KCALLOC(ct->fsize * sizeof(kObject*));
		}
		of->fields = v;
		knh_memcpy(v, src->rawptr, ct->fsize * sizeof(kObject*));
	}
	else {
		of->fields = NULL;
	}
}

static void ObjectField1_reftrace(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
#ifdef K_USING_FASTREFS_
	KNH_SETREF(_ctx, of->fields, 1);
#else
	KREFTRACEv(of->fields[0]);
	kref_setSize();
#endif
}
static void ObjectField2_reftrace(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
#ifdef K_USING_FASTREFS_
	KNH_SETREF(_ctx, of->fields, 2);
#else
	KREFTRACEv(of->fields[0]);
	KREFTRACEv(of->fields[1]);
	kref_setSize();
#endif
}
static void ObjectField3_reftrace(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
#ifdef K_USING_FASTREFS_
	KNH_SETREF(_ctx, of->fields, 3);
#else
	KREFTRACEv(of->fields[0]);
	KREFTRACEv(of->fields[1]);
	KREFTRACEv(of->fields[2]);
	kref_setSize();
#endif
}
static void ObjectField4_reftrace(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
#ifdef K_USING_FASTREFS_
	KNH_SETREF(_ctx, of->fields, 4);
#else
	KREFTRACEv(of->fields[0]);
	KREFTRACEv(of->fields[1]);
	KREFTRACEv(of->fields[2]);
	KREFTRACEv(of->fields[3]);
	kref_setSize();
#endif
}

static kclass_t ObjectNDef[] = {
	{
		ObjectFieldN_init,
		ObjectFieldN_initcopy,
		DEFAULT_reftrace,
		ObjectField_free,
		DEFAULT_checkin, DEFAULT_checkout, ObjectField_compareTo, ObjectField_p,
		ObjectField_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
		DEFAULT_findTypeMapNULL, Object_wdata, DEFAULT_2, DEFAULT_3,
		"Object", CFLAG_Object, 0, NULL,
		NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
	},
	{
		ObjectField_init, ObjectField_initcopy, ObjectField1_reftrace, ObjectField_free,
		DEFAULT_checkin, DEFAULT_checkout, ObjectField_compareTo, ObjectField_p,
		ObjectField_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
		DEFAULT_findTypeMapNULL, Object_wdata, DEFAULT_2, DEFAULT_3,
		"Object", CFLAG_Object, 0, NULL,
		NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
	},
	{
		ObjectField_init, ObjectField_initcopy, ObjectField2_reftrace, ObjectField_free,
		DEFAULT_checkin, DEFAULT_checkout, ObjectField_compareTo, ObjectField_p,
		ObjectField_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
		DEFAULT_findTypeMapNULL, Object_wdata, DEFAULT_2, DEFAULT_3,
		"Object", CFLAG_Object, 0, NULL,
		NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
	},
	{
		ObjectField_init, ObjectField_initcopy, ObjectField3_reftrace, ObjectField_free,
		DEFAULT_checkin, DEFAULT_checkout, ObjectField_compareTo, ObjectField_p,
		ObjectField_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
		DEFAULT_findTypeMapNULL, Object_wdata, DEFAULT_2, DEFAULT_3,
		"Object", CFLAG_Object, 0, NULL,
		NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
	},
	{
		ObjectField_init, ObjectField_initcopy, ObjectField4_reftrace, ObjectField_free,
		DEFAULT_checkin, DEFAULT_checkout, ObjectField_compareTo, ObjectField_p,
		ObjectField_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
		DEFAULT_findTypeMapNULL, Object_wdata, DEFAULT_2, DEFAULT_3,
		"Object", CFLAG_Object, 0, NULL,
		NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
	}
};

void knh_ClassTBL_setObjectCSPI(CTX, kclass_t *ct)
{
	size_t c, i;
	ct->struct_size = sizeof(void*) * ct->fsize;
	for(c = 0; c < ct->fsize; c++) {
		if(ct->fields[c].israw == 1) break;
	}
	for(i = c + 1; i < ct->fsize; i++) {
		if(ct->fields[i].israw == 0) {
			knh_setClassDef(_ctx, ct, &ObjectDef);
			return;
		}
	}
	if(c <= 4) {
		DBG_P("%s: SIZE OF OBJECT FIELD: %d", S_text(ct->lname), c);
		knh_setClassDef(_ctx, ct, ObjectNDef + c);
	}
	else {
		knh_setClassDef(_ctx, ct, &ObjectDef);
	}
}

static kclass_t CppObjectDef = {
	CppObject_init, DEFAULT_initcopy, CppObject_reftrace, CppObject_free,
	DEFAULT_checkin, CppObject_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"dynamic", 0, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Object), 0,
};

kclass_t* knh_getCppClassDef(void)
{
	return &CppObjectDef;
}

/* --------------- */
/* Boolean */

static void NDATA_init(CTX, kObject *o)
{
	kNumber *no = (kNumber*)o;
	no->n.ivalue = 0;
}

static void NDATA_initcopy(CTX, kObject *o, kObject *src)
{
	kNumber *no = (kNumber*)o;
	kNumber *so = (kNumber*)src;
	no->n.data = so->n.data;
}

static void NDATA_free(CTX, kObject *o)
{
	if(O_ct(o)->constPoolMapNULL != NULL) {
		knh_PtrMap_rmI(_ctx, O_ct(o)->constPoolMapNULL, (kInt*)o);
	}
}

static kint_t Int_toint(CTX, ksfp_t *sfp)
{
	return sfp[0].ivalue;
}

static kfloat_t Int_tofloat(CTX, ksfp_t *sfp)
{
	return (kfloat_t)sfp[0].ivalue;
}

static kint_t Float_toint(CTX, ksfp_t *sfp)
{
	return (kint_t)sfp[0].fvalue;
}

static kfloat_t Float_tofloat(CTX, ksfp_t *sfp)
{
	return sfp[0].fvalue;
}

static kuint_t NDATA_hashCode(CTX, kObject *o)
{
	return (kuint_t)((kInt*)o)->n.data;
}

static int Int_compareTo(kObject *o, kObject *o2)
{
	kint_t f = ((kNumber*)o)->n.ivalue;
	kint_t f2 = ((kNumber*)o2)->n.ivalue;
	return (f < f2) ? -1 : ((f == f2) ? 0 : 1);
}

static int Float_compareTo(kObject *o, kObject *o2)
{
	kfloat_t f = ((kNumber*)o)->n.fvalue;
	kfloat_t f2 = ((kNumber*)o2)->n.fvalue;
	return (f < f2) ? -1 : ((f == f2) ? 0 : 1);
}

static void Boolean_p(CTX, kOutputStream *w, kObject *o, int level)
{
	knh_write_bool(_ctx, w, ((kNumber*)o)->n.bvalue);
}

static void Int_p(CTX, kOutputStream *w, kObject *o, int level)
{
#if defined(K_USING_SEMANTICS)
	kSemantics *u = knh_getSemantics(_ctx, O_cid(sfp[1].o));
	knh_write_intx(_ctx, sfp[0].w, u, sfp[1].ivalue);
#else
	knh_write_ifmt(_ctx, w, KINT_FMT, ((kNumber*)o)->n.ivalue);
#endif
}

static void Float_p(CTX, kOutputStream *w, kObject *o, int level)
{
#if defined(K_USING_SEMANTICS)
	kSemantics *u = knh_getSemantics(_ctx, O_cid(sfp[1].o));
	knh_write_floatx(_ctx, sfp[0].w, u, sfp[1].fvalue);
#else
	knh_write_ffmt(_ctx, w, KFLOAT_FMT, ((kNumber*)o)->n.fvalue);
#endif
}

static void Boolean_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{
	packspi->pack_bool(_ctx, pkr, ((kBoolean *)o)->n.bvalue);
}

static void Int_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{
	packspi->pack_int(_ctx, pkr, ((kInt *)o)->n.ivalue);
}

static void Float_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{
	packspi->pack_float(_ctx, pkr, ((kFloat *)o)->n.fvalue);
}

static kclass_t BooleanDef = {
	NDATA_init, NDATA_initcopy, DEFAULT_reftrace, NDATA_free,
	DEFAULT_checkin, DEFAULT_checkout, Int_compareTo, Boolean_p,
	ObjectField_getkey, NDATA_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, Boolean_wdata, DEFAULT_2, DEFAULT_3,
	"Boolean", CFLAG_Boolean, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Boolean), 0,
};

static kclass_t NumberDef = {
	NDATA_init, NDATA_initcopy, DEFAULT_reftrace, NDATA_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	ObjectField_getkey, NDATA_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Number", CFLAG_Number, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Number), 0,
};

static kclass_t IntDef = {
	NDATA_init, NDATA_initcopy, DEFAULT_reftrace, NDATA_free,
	DEFAULT_checkin, DEFAULT_checkout, Int_compareTo, Int_p,
	ObjectField_getkey, NDATA_hashCode, Int_toint, Int_tofloat,
	DEFAULT_findTypeMapNULL, Int_wdata, DEFAULT_2, DEFAULT_3,
	"Int", CFLAG_Int, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Int), 0,
};

static kclass_t FloatDef = {
	NDATA_init, NDATA_initcopy, DEFAULT_reftrace, NDATA_free,
	DEFAULT_checkin, DEFAULT_checkout, Float_compareTo, Float_p,
	ObjectField_getkey, NDATA_hashCode, Float_toint, Float_tofloat,
	DEFAULT_findTypeMapNULL, Float_wdata, DEFAULT_2, DEFAULT_3,
	"Float", CFLAG_Float, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Float), 0,
};

/* --------------- */
/* Date */

static void Date_init(CTX, kObject *o)
{
	kDate *dt = (kDate*)o;
	time_t t;
	struct tm tm;
	time(&t);
#if defined(K_USING_WINDOWS_)
#if defined(K_USING_MINGW_)
	tm = *localtime(&t);
#else
	localtime_s(&tm, &t);
#endif /* defined(K_USING_MINGW_) */
#else
	localtime_r(&t, &tm);
#endif /* defined(K_USING_WINDOWS_) */
	dt->dt.year  = (kshort_t)(tm.tm_year + 1900);
	dt->dt.month = (kshort_t)(tm.tm_mon + 1);
	dt->dt.day   = (kshort_t)(tm.tm_mday);
	dt->dt.hour  = (kshort_t)(tm.tm_hour);
	dt->dt.min   = (kshort_t)(tm.tm_min);
	dt->dt.sec   = (kshort_t)(tm.tm_sec);
#if defined(K_USING_MINGW_)
	_tzset();
	dt->dt.gmtoff = (kshort_t)(_timezone / (60 * 60));
#else
	dt->dt.gmtoff = (kshort_t)(tm.tm_gmtoff / (60 * 60));
#endif /* defined(K_USING_MINGW_) */
	dt->dt.isdst = (kshort_t)(tm.tm_isdst);
}

static int Date_compareTo(kObject *o, kObject *o2)
{
	kDate *dt1 = (kDate*)o;
	kDate *dt2 = (kDate*)o2;
	intptr_t res = dt1->dt.year - dt2->dt.year;
	if(res != 0) return res;
	res = dt1->dt.month - dt2->dt.month;
	if(res == 0) { // TODO: adjustment of summer time
		intptr_t n1 = ((((intptr_t)dt1->dt.day * 24) + dt1->dt.hour) * 60) * dt1->dt.min;
		intptr_t n2 = ((((intptr_t)dt2->dt.day * 24) + dt2->dt.hour) * 60) * dt2->dt.min;
		n1 = (n1 + dt1->dt.gmtoff) * 60 + dt1->dt.sec;
		n2 = (n2 + dt2->dt.gmtoff) * 60 + dt2->dt.sec;
		return (n1 - n2);
	}
	return res;
}

static void Date_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kDate *dt = (kDate*)o;
	char buf[80];
	knh_snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d%+02d:%02d",
		(int)(dt->dt.year), (int)(dt->dt.month), (int)dt->dt.day,
		(int)(dt->dt.hour), (int)(dt->dt.min), (int)dt->dt.sec, (int)(dt->dt.gmtoff / 60), (int)(dt->dt.gmtoff % 60));
	knh_write_ascii(_ctx, w, buf);
}

static kuint_t Date_hashCode(CTX, kObject *o)
{
	kDate *dt = (kDate*)o;
	kuint_t n1 = ((((kuint_t)dt->dt.day * 24) + dt->dt.hour) * 60) * dt->dt.min;
	n1 = (n1 + dt->dt.gmtoff) * 60 + dt->dt.sec;
	n1 = n1 + ((dt->dt.year) * 365) + dt->dt.month;
	return n1;
}

static void Date_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{
//	kDate *s = (kDate *)o;
//	packspi->pack_string(_ctx, pkr, S_text(s), S_size(s));
}

static kclass_t DateDef = {
	Date_init, DEFAULT_initcopy, DEFAULT_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, Date_compareTo, Date_p,
	DEFAULT_getkey, Date_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, Date_wdata, DEFAULT_2, DEFAULT_3,
	"Date", CFLAG_Date, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Date), 0,
};


/* --------------- */
/* String */

static void String_init(CTX, kObject *o)
{
	kString *s = (kString*)o;
	s->str.text = "";
	s->str.len = 0;
	s->hashCode = 0;
	String_setTextSgm(s, 1);
}

static void String_free(CTX, kObject *o)
{
	kString *s = (kString*)o;
#ifdef K_USING_STRINGPOOL
	if(String_isPooled(s) && O_ct(o)->constPoolMapNULL != NULL) {
		knh_PtrMap_rmS(_ctx, O_ct(o)->constPoolMapNULL, s);
	}
#endif
	if(!String_isTextSgm(s)) {
		KFREE(_ctx, s->str.ubuf, KNH_SIZE(S_size(s) + 1));
	}
}

static int String_compareTo(kObject *o, kObject *o2)
{
	kString *s1 = (kString*)o;
	kString *s2 = (kString*)o2;
#if defined(K_USING_SEMANTICS)
	if(s1->h.cid != CLASS_String && s2->h.cid != CLASS_String) {
		kSemantics *u = knh_getSemantics(_ctx, s1->h.cid);
		return DP(u)->fscmp(u, S_tobytes(s1), S_tobytes(s2));
	}
#else
	return knh_bytes_strcmp(S_tobytes(s1) ,S_tobytes(s2));
#endif
}

static void String_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kString *s = (kString*)o;
	if(IS_FMTs(level)) {
		knh_write_utf8(_ctx, w, S_tobytes(s), !String_isASCII(s));
	}
	else {
		knh_write_quote(_ctx, w, '"', S_tobytes(s), !String_isASCII(s));
	}
}

static kString* String_getkey(CTX, ksfp_t *sfp)
{
	return sfp[0].s;
}

static kuint_t String_hashCode(CTX, kObject *o)
{
	return ((kString*)o)->hashCode;
}

static void String_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{
	kString *s = (kString *)o;
	packspi->pack_string(_ctx, pkr, S_text(s), S_size(s));
}

static kclass_t StringDef = {
	String_init, DEFAULT_initcopy, DEFAULT_reftrace, String_free,
	DEFAULT_checkin, DEFAULT_checkout, String_compareTo, String_p,
	String_getkey, String_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, String_wdata, DEFAULT_2, DEFAULT_3,
	"String", CFLAG_String, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(String), 0,
};

/* --------------- */
/* Bytes */

static const kdim_t dimINIT = {
	0, 0, 0, 0, 0, 0, 0,
};

static void Bytes_init(CTX, kObject *o)
{
	kBytes *ba = (kBytes*)o;
	ba->bu.len = 0;
	ba->bu.ubuf = NULL;
	ba->dim = &dimINIT;
	ba->DBG_name = NULL;
}

static const kdim_t* dim_copy(CTX, const kdim_t *dim_src)
{
	if(dim_src->capacity == 0) {
		return dim_src;
	}
	else {
		kdim_t *dim = (kdim_t*)KCALLOC(sizeof(kdim_t));
		/* copy dim_src to dim. @imasahiro */
		dim->capacity = dim_src->capacity;
		dim->wsize    = dim_src->wsize;
		dim->sorted   = dim_src->sorted;
		dim->dim      = dim_src->dim;
		dim->x        = dim_src->x;
		dim->xy       = dim_src->xy;
		dim->xyz      = dim_src->xyz;
		return (const kdim_t*)dim;
	}
}

static void Bytes_initcopy(CTX, kObject *o, kObject *src)
{
	kBytes *ba = (kBytes*)o, *ba_src = (kBytes*)src;
	if(ba_src->dim->capacity > 0) {
		ba->bu.len = ba_src->bu.len;
		ba->bu.ubuf = (kchar_t*)KCALLOC(ba_src->dim->capacity);
		knh_memcpy(ba->bu.ubuf, ba_src->bu.ubuf, ba_src->dim->capacity);
	}
	else {
		ba->bu.len = 0;
		ba->bu.ubuf = NULL;
	}
	ba->dim = dim_copy(_ctx, ba_src->dim);
}

static void Bytes_free(CTX, kObject *o)
{
	kBytes *ba = (kBytes*)o;
	knh_dimfree(_ctx, ba->bu.ubuf, ba->dim);
}

static int Bytes_compareTo(kObject *o, kObject *o2)
{
	kBytes *s1 = (kBytes*)o;
	kBytes *s2 = (kBytes*)o2;
	return knh_bytes_strcmp(BA_tobytes(s1) , BA_tobytes(s2));
}

static void Bytes_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kBytes *ba = (kBytes*)o;
	if(IS_FMTs(level)) {
		knh_printf(_ctx, w, "byte[%d]", BA_size(ba));
	}
	else if(IS_FMTdump(level)) {
		size_t i, j, n;
		char buf[40];
		for(j = 0; j * 16 < ba->bu.len; j++) {
			knh_snprintf(buf, sizeof(buf), "%08x", (int)(j*16));
			knh_write(_ctx, w, B(buf));
			for(i = 0; i < 16; i++) {
				n = j * 16 + i;
				if(n < ba->bu.len) {
					knh_snprintf(buf, sizeof(buf), " %2x", (int)ba->bu.utext[n]);
					knh_write(_ctx, w, B(buf));
				}
				else {
					knh_write(_ctx, w, STEXT("   "));
				}
			}
			knh_write(_ctx, w, STEXT("    "));
			for(i = 0; i < 16; i++) {
				n = j * 16 + i;
				if(n < ba->bu.len && isprint(ba->bu.utext[n])) {
					knh_snprintf(buf, sizeof(buf), "%c", (int)ba->bu.utext[n]);
					knh_write(_ctx, w, B(buf));
				}
				else {
					knh_write(_ctx, w, STEXT(" "));
				}
			}
			knh_write_EOL(_ctx, w);
		}
	}
}

static kuint_t Bytes_hashCode(CTX, kObject *o)
{
	kBytes *ba = (kBytes*)o;
	return knh_hash(0, ba->bu.text, ba->bu.len);
}

static kclass_t BytesDef = {
	Bytes_init, Bytes_initcopy, DEFAULT_reftrace, Bytes_free,
	DEFAULT_checkin, DEFAULT_checkout, Bytes_compareTo, Bytes_p,
	DEFAULT_getkey, Bytes_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Bytes", CFLAG_Bytes, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Bytes), 0,
};

/* --------------- */
/* Pointer */

static void Pointer_init(CTX, kObject *o)
{
	kPointer *p = (kPointer*)o;
	p->ptr = NULL;
	p->size = 0;
	p->wsize = 0;
	KINITv(p->gcref, K_NULL);
}

static void Pointer_reftrace(CTX, kObject *o)
{
	kPointer *p = (kPointer*)o;
	KREFTRACEv(p->gcref);
	kref_setSize();
}

static int Pointer_compareTo(kObject *o, kObject *o2)
{
	kPointer *p1 = (kPointer*)o;
	kPointer *p2 = (kPointer*)o2;
	return (int)((intptr_t)(p1)->ptr - (intptr_t)p2->ptr);
}

static void Pointer_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kPointer *p = (kPointer*)o;
	if(IS_FMTs(level)) {
		knh_write_ptr(_ctx, w, p->ptr);
	}
	else if(IS_FMTdump(level)) {
		knh_write_ptr(_ctx, w, p->ptr);
	}
}

static kuint_t Pointer_hashCode(CTX, kObject *o)
{
	kPointer *p = (kPointer*)o;
	return (kuint_t)(p->ptr) / sizeof(void*);
}

static kclass_t PointerDef = {
	Pointer_init, DEFAULT_initcopy, Pointer_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, Pointer_compareTo, Pointer_p,
	DEFAULT_getkey, Pointer_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Pointer", CFLAG_Pointer, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Pointer), 0,
};

/* --------------------------------------------------------------------------*/
/* Tuple */

static void Tuple_init(CTX, kObject *o)
{
	kTuple *of = (kTuple*)o;
	kclass_t *t = O_ct(o);
	if(t->fsize > 0) {
		Object **v = &(of->smallobject);
		if(t->fsize > K_SMALLOBJECT_FIELDSIZE) {
			v = (Object**)KCALLOC(t->fsize * sizeof(kObject*));
		}
		of->fields = v;
		knh_memcpy(v, t->defnull->fields, t->fsize * sizeof(kObject*));
#ifdef K_USING_RCGC
		size_t i;
		for(i = 0; i < t->fsize; i++) {
			if(t->fields[i].israw == 0) {
				knh_Object_RCinc(of->fields[i]);
			}
		}
#endif
	}
	else {
		of->fields = NULL;
	}
}

static void TUPLE_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kwb_putc(wb, '(');
	{
		kfieldinfo_t *tf = O_ct(o)->fields;
		size_t i, fsize = O_ct(o)->fsize;
		Object **v = (Object**)o->rawptr;
		knh_write_TObject(_ctx, w, tf[0].type, v, 0, level);
		for(i = 1; i < fsize; i++) {
			if(tf[i].type == TY_void) continue;
			knh_write_delim(_ctx, w);
			knh_write_TObject(_ctx, w, tf[i].type, v, i, level);
		}
	}
	kwb_putc(wb, ')');
}

static kclass_t TupleDef = {
	Tuple_init, ObjectField_initcopy, ObjectField_reftrace, ObjectField_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, TUPLE_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Tuple", 0, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Tuple), 0,
};

/* --------------- */
/* Range */

static void Range_init(CTX, kObject *o)
{
	kRange *rng = (kRange*)o;
	kcid_t p1 = O_p0(rng);
	if(TY_isUnbox(p1)) {
		Range_setNDATA(rng, 1);
		rng->nstart = 0;
		rng->nend = 0;
	}
	else {
		KINITv(rng->ostart, KNH_NULVAL(O_p0(rng)));
		KINITv(rng->oend, rng->ostart);
	}
}

static void Range_reftrace(CTX, kObject *o)
{
	kRange *rng = (kRange*)o;
	if(!Range_isNDATA(o)) {
		KREFTRACEv(rng->ostart);
		KREFTRACEv(rng->oend);
		kref_setSize();
	}
}

static void Range_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kRange *range = (kRange*)o;
	kcid_t p1 = O_p0(range);
	kwb_putc(wb, '[');
	knh_write_TObject(_ctx, w, p1, &range->ostart, 0, level);
	knh_write(_ctx, w, STEXT(" to "));
	knh_write_TObject(_ctx, w, p1, &range->oend, 0, level);
	kwb_putc(wb, ']');
}

static kclass_t RangeDef = {
	Range_init, DEFAULT_initcopy, Range_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Range_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Range", CFLAG_Range, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Range), 0,
};

/* --------------- */
/* Array */

static void Array_init(CTX, kObject *o)
{
	kArray *a = (kArray*)o;
	kArray_initAPI(_ctx, a);
	a->dim = &dimINIT;
	a->list = NULL;
	a->size = 0;
}

static void Array_initcopy(CTX, kObject *o, kObject *src)
{
	kArray *a = (kArray*)o, *a_src = (kArray*)src;
	a->api = a_src->api;
	if(a_src->dim->capacity > 0) {
		a->size = a_src->size;
		a->list = (kObject**)KCALLOC(a_src->dim->capacity * a_src->dim->wsize);
		knh_memcpy(a->list, a_src->list, a_src->dim->capacity * a_src->dim->wsize);
		kArray_setUnboxData(a, kArray_isUnboxData(a_src));
		if(!kArray_isUnboxData(a)) {
			size_t i;
			for(i = 0; i < a->size; i++) {
				knh_Object_RCinc(a->list[i]);
			}
		}
	}
	else {
		a->list = NULL;
		a->size = 0;
	}
	a->dim = dim_copy(_ctx, a_src->dim);
}

static void Array_reftrace(CTX, kObject *o)
{
	kArray *a = (kArray*)o;
	if(!kArray_isUnboxData(a)) {
#ifdef K_USING_FASTREFS_
		KNH_SETREF(_ctx, a->list, a->size);
#else
		size_t i;
		for(i = 0; i < a->size; i++) {
			KREFTRACEv(a->list[i]);
		}
		kref_setSize();
#endif
	}
}

static void Array_free(CTX, kObject *o)
{
	kArray *a = (kArray*)o;
	knh_dimfree(_ctx, a->list, a->dim);
}

static int Array_compareTo(kObject *o, kObject *o2)
{
	if(O_ct(o) == O_ct(o2)) {
		kArray *a = (kArray*)o;
		kArray *a2 = (kArray*)o2;
		size_t i, asize = kArray_size(a), asize2 = kArray_size(a2);
		if(kArray_isUnboxData(a)) {
			for(i = 0; i < asize; i++) {
				if(!(i < asize2)) return -1;
				if(a->nlist[i] == a2->nlist[i]) continue;
				if(O_ct(a)->p1 == CLASS_Float) {
					kfloat_t dim = a->flist[i] - a2->flist[i];
					if(dim < 0) return -1;
					return (dim > 0) ? 1: 0;
				}
				else {
					kint_t dim = a->ilist[i] - a2->ilist[i];
					if(dim < 0) return -1;
					return (dim > 0) ? 1: 0;
				}
			}
		}
		else {
			for(i = 0; i < asize; i++) {
				if(!(i < asize2)) return -1;
				if(a->list[i] == a2->list[i]) continue;
				int res = knh_Object_compareTo(a->list[i], a2->list[i]);
				if(res != 0) return res;
			}
		}
		return (asize == asize2) ? 0 : 1;
	}
	return (int)(o - o2);
}

static void Array_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kwb_putc(wb, '[');
	{
		kArray *a = (kArray*)o;
		kcid_t p1 = O_p0(a);
		size_t c, size = kArray_size(a);
		if(size > 0) {
			if(TY_isUnbox(p1)) {
				if(IS_Tint(p1)) {
					knh_write_ifmt(_ctx, w, KINT_FMT, a->ilist[0]);
					if(IS_FMTline(level)) {
						knh_write_delimdots(_ctx, w);
					}
					else {
						for(c = 1; c < size; c++) {
							knh_write_delim(_ctx, w);
							knh_write_ifmt(_ctx, w, KINT_FMT, a->ilist[c]);
						}
					}
				}
				else if(p1 == TY_Boolean) {
					knh_write_bool(_ctx, w, (int)a->ilist[0]);
					if(IS_FMTline(level)) {
						knh_write_delimdots(_ctx, w);
					}
					else {
						for(c = 1; c < kArray_size(a); c++) {
							knh_write_delim(_ctx, w);
							knh_write_bool(_ctx, w, (int)a->ilist[c]);
						}
					}
				}
				else { /* IS_Tfloat(p1) */
					knh_write_ffmt(_ctx, w, KFLOAT_FMT, a->flist[0]);
					if(IS_FMTline(level)) {
						knh_write_delimdots(_ctx, w);
					}
					else {
						for(c = 1; c < kArray_size(a); c++) {
							knh_write_delim(_ctx, w);
							knh_write_ffmt(_ctx, w, KFLOAT_FMT, a->flist[c]);
						}
					}
				}
			}
			else {
				knh_write_InObject(_ctx, w, a->list[0], level);
				if(IS_FMTline(level)) {
					knh_write_delimdots(_ctx, w);
				}
				else {
					for(c = 1; c < size; c++) {
						knh_write_delim(_ctx, w);
						knh_write_InObject(_ctx, w, a->list[c], level);
					}
				}
			}
		}
	}
	kwb_putc(wb, ']');
}

static void Array_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{
	kArray *a = (kArray *)o;
	packspi->pack_beginarray(_ctx, pkr, a->size);
	size_t i = 0;
	kcid_t p1 = O_p0(a);
	if (kArray_isUnboxData(a)) {
		for (i = 0; i < a->size; i++) {
			if (i != 0)
				packspi->pack_putc(_ctx, pkr, ',');
			pack_unbox(_ctx, pkr, p1, (kObject**)(a->ilist+i), packspi);
		}
	} else {
		for (i = 0; i < a->size; i++) {
			if (i != 0)
				packspi->pack_putc(_ctx, pkr, ',');
			O_ct(a->list[i])->cdef->wdata(_ctx, RAWPTR(a->list[i]), pkr, packspi);
		}
	}
	packspi->pack_endarray(_ctx, pkr);
}

static kclass_t ArrayDef = {
	Array_init, Array_initcopy, Array_reftrace, Array_free,
	DEFAULT_checkin, DEFAULT_checkout, Array_compareTo, Array_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, Array_wdata, DEFAULT_2, DEFAULT_3,
	"Array", CFLAG_Array, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Array), 0,
};

/* --------------- */
/* Iterator */

static ITRNEXT Fitrnext_single(CTX, ksfp_t *sfp _RIX)
{
	kIterator *itr = ITR(sfp);
	if(DP(itr)->m.index == 0) {
		DP(itr)->m.index = 1;
		ITRNEXT_(DP(itr)->source);
	}
	ITREND_();
}

static void Iterator_init(CTX, kObject *o)
{
	kIterator *itr = (kIterator*)o;
	knh_IteratorEX_t *b;
#ifdef K_USING_BMGC
	b = DP(itr);
#else
	b = knh_bodymalloc(_ctx, Iterator);
	itr->b = b;
#endif
	itr->fnext_1  =  Fitrnext_single;
	KINITv(b->source, K_NULL);
	b->mtdNULL  =  NULL;
	b->m.nptr   =  NULL;
	b->m.index  = 0;
	b->m.max    = 0;
	b->m.nfree = NULL;
}

static void Iterator_reftrace(CTX, kObject *o)
{
	kIterator *itr = (kIterator*)o;
	KREFTRACEv(DP(itr)->source);
	KREFTRACEn(DP(itr)->mtdNULL);
	kref_setSize();
}

static void Iterator_free(CTX, kObject *o)
{
	kIterator *itr = (kIterator*)o;
	knh_Iterator_close(_ctx, itr);
#ifndef K_USING_BMGC
	knh_bodyfree(_ctx, itr->b, Iterator);
#endif
}

static void Iterator_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kIterator *it = (kIterator*)o;
	kcid_t p1 = O_p0(it);
	if(IS_FMTdump(level)) {
		ksfp_t *lsfp = ctx->esp;
		KSETv(lsfp[1].o, it);
		klr_setesp(_ctx, lsfp + 2);
		size_t c = 0;
		while(it->fnext_1(_ctx, lsfp+1, -1)) {
			if(c > 0) {
				knh_write_EOL(_ctx, w);
			}
			if(IS_Tint(p1)) {
				knh_write_ifmt(_ctx, w, KINT_FMT, lsfp[0].ivalue);
			}
			else if(IS_Tfloat(p1)) {
				knh_write_ffmt(_ctx, w, KFLOAT_FMT, lsfp[0].fvalue);
			}
			else {
				knh_write_Object(_ctx, w, lsfp[0].o, FMT_data);
			}
			c++;
			KSETv(lsfp[1].o, it);
			klr_setesp(_ctx, lsfp + 2);
		}
	}
}

static kclass_t IteratorDef = {
	Iterator_init, DEFAULT_initcopy, Iterator_reftrace, Iterator_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Iterator_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Iterator", CFLAG_Iterator, sizeof(knh_IteratorEX_t), NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Iterator), 0,
};

/* --------------- */
/* Map */

static const knh_MapDPI_t* NULLMAP_config(CTX, kcid_t p1, kcid_t p2);
static kmapptr_t *NULLMAP_init(CTX, size_t init, const char *path, struct kDictMap *opt) { return NULL; }
static void NULLMAP_reftrace(CTX, kmapptr_t *m){}
static void NULLMAP_free(CTX, kmapptr_t *m){}
static kbool_t NULLMAP_get(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp) { return 0; }
static void NULLMAP_set(CTX, kmapptr_t* m, ksfp_t *ksfp) {}
static void NULLMAP_remove(CTX, kmapptr_t* m, ksfp_t *ksfp) {}
static size_t NULLMAP_size(CTX, kmapptr_t* m) { return 0; }
static kbool_t NULLMAP_next(CTX, kmapptr_t* m, knitr_t *mitr, ksfp_t *rsfp) { return 0; }

static const knh_MapDPI_t NULLMAP = {
	K_DSPI_MAP, "NULL",
	NULLMAP_config, NULLMAP_init, NULLMAP_reftrace, NULLMAP_free,
	NULLMAP_get, NULLMAP_set, NULLMAP_remove, NULLMAP_size, NULLMAP_next,
};

static const knh_MapDPI_t* NULLMAP_config(CTX, kcid_t p1, kcid_t p2)
{
	return &NULLMAP;
}

static void Map_init(CTX, kObject *o)
{
	kMap *m = (kMap*)o;
	m->spi = &NULLMAP;
	m->mapptr = NULL;
}

static void TODO_initcopy(CTX, kObject *d, kObject *s)
{
	KNH_TODO(__FUNCTION__);
}

static void Map_reftrace(CTX, kObject *o)
{
	kMap *m = (kMap*)o;
	m->spi->reftrace(_ctx, m->mapptr FTRDATA);
}

static void Map_free(CTX, kObject *o)
{
	kMap *m = (kMap*)o;
	m->spi->freemap(_ctx, m->mapptr);
}

static void Map_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kMap *m = (kMap*)o;
	size_t n = m->spi->size(_ctx, m->mapptr);
	kwb_putc(wb, '{');
	if(n > 0) {
		BEGIN_LOCAL(_ctx, lsfp, 2);
		kcid_t p1 = O_ct(o)->p1, p2 = O_ct(o)->p2;
		knitr_t mitrbuf = K_NITR_INIT, *mitr = &mitrbuf;
		if(m->spi->next(_ctx, m->mapptr, mitr, lsfp)) {
			knh_write_sfp(_ctx, w, p1, lsfp, FMT_line);
			knh_write(_ctx, w, STEXT(": "));
			knh_write_sfp(_ctx, w, p2, lsfp+1, FMT_data);
			if(!IS_FMTline(level)) {
				while(m->spi->next(_ctx, m->mapptr, mitr, lsfp)) {
					knh_write(_ctx, w, STEXT(", "));
					knh_write_sfp(_ctx, w, p1, lsfp, FMT_line);
					knh_write(_ctx, w, STEXT(": "));
					knh_write_sfp(_ctx, w, p2, lsfp+1, FMT_data);
				}
			}
			else {
				knh_write_delimdots(_ctx, w);
			}
		}
		END_LOCAL(_ctx, lsfp);
	}
	kwb_putc(wb, '}');
}

static void pack_sfp(CTX, void *pkr, kcid_t cid, ksfp_t *sfp, const knh_PackSPI_t *packspi)
{
	if (TY_isUnbox(cid)) {
		if (IS_Tint(cid)) {
			packspi->pack_int(_ctx, pkr, sfp[0].ivalue);
		} else if (IS_Tfloat(cid)) {
			packspi->pack_float(_ctx, pkr, sfp[0].fvalue);
		} else {
			packspi->pack_bool(_ctx, pkr, sfp[0].bvalue);
		}
	} else {
		O_ct(sfp[0].o)->cdef->wdata(_ctx, RAWPTR(sfp[0].o), pkr, packspi);
	}
}


static void Map_wdata(CTX, kObject *o, void *pkr, const knh_PackSPI_t *packspi)
{
	kMap *m = (kMap*)o;
	size_t i = 0, n = m->spi->size(_ctx, m->mapptr);

	packspi->pack_beginmap(_ctx, pkr, n + 1);
	if(n > 0) {
		BEGIN_LOCAL(_ctx, lsfp, 2);
		kcid_t p1 = O_ct(o)->p1, p2 = O_ct(o)->p2;
		knitr_t mitrbuf = K_NITR_INIT, *mitr = &mitrbuf;
		while(m->spi->next(_ctx, m->mapptr, mitr, lsfp)) {
			if (i++ != 0) {
				packspi->pack_putc(_ctx, pkr, ',');
			}
			pack_sfp(_ctx, pkr, p1, lsfp, packspi);
			packspi->pack_putc(_ctx, pkr, ':');
			pack_sfp(_ctx, pkr, p2, lsfp+1, packspi);
		}
		END_LOCAL(_ctx, lsfp);
	}
	packspi->pack_endmap(_ctx, pkr);
}


static kclass_t MapDef = {
	Map_init, TODO_initcopy, Map_reftrace, Map_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Map_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, Map_wdata, DEFAULT_2, DEFAULT_3,
	"Map", CFLAG_Map, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Map), 0,
};

/* --------------- */
/* Class */

static int Class_compareTo(kObject *o, kObject *o2)
{
	kClass *c = (kClass*)o;
	kClass *c2 = (kClass*)o2;
	return knh_strcmp(S_text(c->ct->lname), S_text(c2->ct->lname));
}

static kString *Class_getkey(CTX,ksfp_t *sfp)
{
	kClass *c = (kClass*)sfp[0].o;
	return ClassTBL(c->cid)->lname;
}

static kuint_t Class_hashCode(CTX, kObject *o)
{
	kClass *c = (kClass*)o;
	return (kuint_t)c->cid;
}

static void Class_p(CTX, kOutputStream *w, kObject *o, int level)
{
	knh_write_cid(_ctx, w, knh_Class_cid((kClass*)o));
}

static kclass_t ClassDef = {
	DEFAULT_init, DEFAULT_initcopy, DEFAULT_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, Class_compareTo, Class_p,
	Class_getkey, Class_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Class", CFLAG_Class, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Class), 0,
};

/* --------------- */
/* Param */

static void Param_init(CTX, kObject *o)
{
	kParam *pa = (kParam*)o;
	pa->psize = 0;
	pa->rsize = 0;
	pa->params = NULL;
	pa->capacity = 0;
}

static void Param_free(CTX, kObject *o)
{
	kParam *pa = (kParam*)o;
	if(pa->psize + pa->rsize > 3) {
		KFREE(_ctx, pa->params, pa->capacity * sizeof(kparam_t));
	}
}

static void Param_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kParam *pa = (kParam*)o;
	size_t i;
	for(i = 0; i < pa->psize; i++) {
		kparam_t *p = knh_Param_get(pa, i);
		if(i > 0) knh_write_delim(_ctx, w);
		knh_write_type(_ctx, w, p->type);
		if(IS_FMTdump(level)) {
			kwb_putc(wb, ' ');
			knh_write_fn(_ctx, w, p->fn);
		}
	}
	if(pa->rsize > 0) {
		knh_write(_ctx, w, STEXT(" => "));
		for(i = 0; i < pa->rsize; i++) {
			kparam_t *p = knh_Param_rget(pa, i);
			knh_write_type(_ctx, w, p->type);
		}
	}
}

static kclass_t ParamDef = {
	Param_init, TODO_initcopy, DEFAULT_reftrace, Param_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Param_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Param", CFLAG_Param, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Param), 0,
};

/* --------------- */
/* Method */

static void Method_init(CTX, kObject *o)
{
	kMethod *mtd = (kMethod*)o;
	knh_MethodEX_t *b;
#ifndef K_USING_BMGC
	b = knh_bodymalloc(_ctx, Method);
	mtd->b = b;
#else
	b = DP(mtd);
#endif
	KINITv(b->mp, KNH_NULVAL(CLASS_Param));
	KINITv(b->kcode, K_NULL);
	KINITv(b->tsource, K_NULL);
	b->paramsNULL = NULL;
//	b->flag   = 0;
//	b->delta  = 0;
//	b->fileid  = 0;  b->domain = 0;
}

static void Method_reftrace(CTX, kObject *o)
{
	kMethod *mtd = (kMethod*)o;
	knh_MethodEX_t *b = DP(mtd);
	KREFTRACEv(b->mp);
	KREFTRACEv(b->kcode);
	KREFTRACEv(b->tsource);
	KREFTRACEn(b->paramsNULL);
	kref_setSize();
}

static void BODY_free(CTX, kObject *o)
{
#ifndef K_USING_BMGC
	kclass_t *ct = O_ct(o);
	DBG_ASSERT(ct->cdef->struct_size > 0);
	KFREE(_ctx, o->rawptr, ct->cdef->struct_size);
#endif
}

static void Method_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kMethod *mtd = (kMethod*)o;
	if(!(IS_FMTline(level))) {
		if(Method_isAbstract(mtd)) {
			knh_write(_ctx, w, STEXT("@Abstract")); kwb_putc(wb, ' ');
		}
		if(Method_isPrivate(mtd)) {
			knh_write(_ctx, w, STEXT("@Private"));  kwb_putc(wb, ' ');
		}
		if(Method_isStatic(mtd)) {
			knh_write(_ctx, w, STEXT("@Static"));   kwb_putc(wb, ' ');
		}
		knh_write_type(_ctx, w, knh_Param_rtype(DP(mtd)->mp));
		kwb_putc(wb, ' ');
	}
	knh_write_cname(_ctx, w, (mtd)->cid);
	kwb_putc(wb, '.');
	knh_write_mn(_ctx, w, (mtd)->mn);
	if(!(IS_FMTline(level))) {
		size_t i;
		kwb_putc(wb, '(');
		for(i = 0; i < knh_Method_psize(mtd); i++) {
			kparam_t *p = knh_Param_get(DP(mtd)->mp, i);
			if(i > 0) {
				knh_write_delim(_ctx, w);
			}
			knh_write_type(_ctx, w, p->type);
			kwb_putc(wb, ' ');
			knh_write(_ctx, w, B(FN__(p->fn)));
		}
		if(Param_isVARGs(DP(mtd)->mp)) {
			knh_write_delimdots(_ctx, w);
		}
		kwb_putc(wb, ')');
	}
	if(IS_FMTdump(level)) {
		if(!IS_NULL(DP(mtd)->objdata)) {
			knh_write_EOL(_ctx, w);
			knh_write_InObject(_ctx, w, DP(mtd)->objdata, level);
		}
	}
}

static kclass_t MethodDef = {
	Method_init, TODO_initcopy, Method_reftrace, BODY_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Method_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Method", CFLAG_Method, sizeof(knh_MethodEX_t), NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Method), 0,
};

/* --------------- */
/* TypeMap */

static void TypeMap_init(CTX, kObject *o)
{
	kTypeMap *tmr = (kTypeMap*)o;
	tmr->scid = 0;
	tmr->tcid = 0;
	KINITv(tmr->mapdata, K_NULL);
	KINITv(tmr->tmr2, K_NULL);
}

static void TypeMap_reftrace(CTX, kObject *o)
{
	kTypeMap *tmr = (kTypeMap*)o;
	KREFTRACEv(tmr->mapdata);
	KREFTRACEv(tmr->tmr2);
	kref_setSize();
}

static void TypeMap_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kTypeMap *tmr = (kTypeMap*)o;
	knh_write_type(_ctx, w, tmr->scid);
	knh_write(_ctx, w, STEXT("=>"));
	knh_write_type(_ctx, w, tmr->tcid);
}

static kclass_t TypeMapDef = {
	TypeMap_init, TODO_initcopy, TypeMap_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, TypeMap_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"TypeMap", CFLAG_TypeMap, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(TypeMap), 0,
};

/* --------------- */
/* Func */

static KMETHOD Fmethod_funcRTYPE(CTX, ksfp_t *sfp _RIX)
{
	ktype_t rtype = knh_Param_rtype(DP(sfp[K_MTDIDX].mtdNC)->mp);
	if(rtype != TY_void) {
		if(TY_isUnbox(rtype)) {
			RETURNi_(KINT0);  // same results in Float, Boolean
		}
		else {
			RETURN_(KNH_NULVAL(CLASS_t(rtype)));
		}
	}
}

static void Func_init(CTX, kObject *o)
{
	kFunc *fo = (kFunc*)o;
	kclass_t *t = O_ct(o);
	kMethod *mtd;
	if(t->defnull == NULL) {
		mtd = new_Method(_ctx, 0, O_cid(o), MN_LAMBDA, Fmethod_funcRTYPE);
		KSETv(DP(mtd)->mp, t->cparam);
	}
	else {
		mtd = t->deffunc->mtd;
	}
	KINITv(fo->mtd, mtd);
	fo->baseNULL = NULL;
}

static void Func_reftrace(CTX, kObject *o)
{
	kFunc *fo = (kFunc*)o;
	KREFTRACEv((fo->mtd));
	KREFTRACEn(fo->baseNULL);
//		size_t i, stacksize = (fo)->hstacksize[-1];
//		for(i = 0; i < stacksize; i++) {
//			KREFTRACEv((fo)->envsfp[i].o);
//		}
	kref_setSize();
}

static void Func_free(CTX, kObject *o)
{
//	kFunc *cc = (kFunc*)o;
//	if(Func_isStoredEnv(cc)) {
//		size_t stacksize = (cc)->hstacksize[-1];
//		KFREE(_ctx, (cc)->envsfp, (sizeof(ksfp_t) * stacksize) + sizeof(size_t));
//		(cc)->envsfp = NULL;
//		Func_setStoredEnv(cc, 0);
//	}
}

static void Func_p(CTX, kOutputStream *w, kObject *o, int level)
{

}

static kclass_t FuncDef = {
	Func_init, TODO_initcopy, Func_reftrace, Func_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Func_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Func", CFLAG_Func, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Func), 0,
};

/* --------------- */
/* Thunk */

static void Thunk_newenv(CTX, kThunk *thk, size_t envsize)
{
	size_t i;
	thk->envsize = envsize;
	thk->envsfp = (ksfp_t*)KCALLOC(sizeof(ksfp_t) * envsize);
	for(i = 0; i < envsize; i++) {
		KINITv((thk)->envsfp[i].o, K_NULL);
		(thk)->envsfp[i].ndata = 0;
	}
}

static void Thunk_init(CTX, kObject *o)
{
	kThunk *thk = (kThunk*)o;
	kMethod *mtd = ClassTBL_getMethodNULL(_ctx, ClassTBL(CLASS_Thunk), MN_);
	DBG_ASSERT(mtd != NULL);
	Thunk_newenv(_ctx, thk, K_CALLDELTA);
	KSETv(thk->envsfp[0].o, KNH_NULVAL(O_p0(thk)));
	thk->envsfp[K_CALLDELTA+K_MTDIDX].mtdNC = mtd;
}

kThunk* new_Thunk(CTX, kcid_t p1, size_t envsize)
{
	kcid_t cid = knh_class_P1(_ctx, CLASS_Thunk, p1);
	kThunk *thk = (kThunk*)new_hObject_(_ctx, ClassTBL(cid));
	Thunk_newenv(_ctx, thk, envsize);
	return thk;
}

static void Thunk_reftrace(CTX, kObject *o)
{
	kThunk *thk = (kThunk*)o;
	size_t i;
	for(i = 0; i < (thk)->envsize; i++) {
		KREFTRACEv((thk)->envsfp[i].o);
	}
	kref_setSize();
}

static void Thunk_free(CTX, kObject *o)
{
	kThunk *thunk = (kThunk*)o;
	KFREE(_ctx, thunk->envsfp, sizeof(ksfp_t) * thunk->envsize);
	thunk->envsfp = NULL;
	thunk->envsize = 0;
}

static kclass_t ThunkDef = {
	Thunk_init, TODO_initcopy, Thunk_reftrace, Thunk_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Thunk", CFLAG_Thunk, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Thunk), 0,
};

/* --------------- */
/* Exception */

static void Exception_init(CTX, kObject *o)
{
	kException *e = (kException*)o;
	KINITv(e->emsg, TS_EMPTY);
	e->tracesNULL = NULL;
	e->uline = 0;
}

static void Exception_reftrace(CTX, kObject *o)
{
	kException *e = (kException*)o;
	KREFTRACEv(e->emsg);
	KREFTRACEn(e->tracesNULL);
	kref_setSize();
}

static void Exception_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kException *e = (kException*)o;
	if(e->uline != 0 && IS_FMTdump(level)) {
		knh_write_uline(_ctx, w, e->uline);
	}
	knh_write(_ctx, w, S_tobytes(e->emsg));
	if(e->tracesNULL != NULL && IS_FMTdump(level)) {
		kArray *a = e->tracesNULL;
		size_t i, size = kArray_size(a), c = 0;
		kbytes_t prev = STEXT("?");
		knh_write_EOL(_ctx, w);
		for(i = 0; i < size; i++) {
			kString *s = a->strings[i];
			if(S_startsWith(s, prev)) {
				c++; continue;
			}
			if(c > 0) {
				knh_write_EOL(_ctx, w);
				knh_printf(_ctx, w, "  *** called %d times recursively ***", c);
				c = 0;
			}
			knh_write_EOL(_ctx, w);
			knh_printf(_ctx, w, "  at %s", S_text(s));
			prev = S_tobytes(s);
			prev = knh_bytes_first(prev, knh_bytes_rindex(prev, '('));
		}
		knh_write_EOL(_ctx, w);
	}
}

static kclass_t ExceptionDef = {
	Exception_init, TODO_initcopy, Exception_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Exception_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Exception", CFLAG_Exception, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Exception), 0,
};

/* --------------- */
/* ExceptionHandler */

static void ExceptionHandler_init(CTX, kObject *o)
{
	kExceptionHandler *hdr = (kExceptionHandler*)o;
	knh_ExceptionHandlerEX_t *b;
#ifdef K_USING_BMGC
	b = DP(hdr);
#else
	b = knh_bodymalloc(_ctx, ExceptionHandler);
	o->rawptr = b;
#endif
	knh_bzero(b, sizeof(knh_ExceptionHandlerEX_t));
	KINITv(hdr->stacklist, new_Array0(_ctx, 0));
}

static void ExceptionHandler_reftrace(CTX, kObject *o)
{
	kExceptionHandler *hdr = (kExceptionHandler*)o;
	KREFTRACEv(hdr->stacklist);
	kref_setSize();
}

static kclass_t ExceptionHandlerDef = {
	ExceptionHandler_init, TODO_initcopy, ExceptionHandler_reftrace, BODY_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"ExceptionHandler", CFLAG_ExceptionHandler, sizeof(knh_ExceptionHandlerEX_t), NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(ExceptionHandler), 0,
};

/* --------------- */
/* Regex */

static void Regex_init(CTX, kObject *o)
{
	kRegex *re = (kRegex*)o;
	KINITv(re->pattern, TS_EMPTY);
	re->spi = knh_getStrRegexSPI();
	re->reg = (kregex_t*)TS_EMPTY;
}

static void Regex_reftrace(CTX, kObject *o)
{
	kRegex *re = (kRegex*)o;
	KREFTRACEv((re->pattern));
	kref_setSize();
}

static void Regex_free(CTX, kObject *o)
{
	kRegex *re = (kRegex*)o;
	if(re->reg != NULL) {
		re->spi->regfree(_ctx, re->reg);
		re->spi = NULL;
		re->reg = NULL;
	}
}

static void Regex_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kRegex *re = (kRegex*)o;
	kbytes_t t = S_tobytes(re->pattern);
	size_t i;
	kwb_putc(wb, '/');
	for(i = 0; i < t.len; i++) {
		int ch = t.buf[i];
		if(ch == '/') {
			kwb_putc(wb, '\\');
		}
		kwb_putc(wb, ch);
	}
	kwb_putc(wb, '/');
}

static kclass_t RegexDef = {
	Regex_init, DEFAULT_initcopy, Regex_reftrace, Regex_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Regex_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Regex", CFLAG_Regex, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Regex), 0,
};

/* --------------- */
/* StringEncoder */

static kbool_t conv_NOCONV(CTX, knh_conv_t *c, const char *t, size_t s, kBytes *tobuf)
{
	knh_Bytes_write2(_ctx, tobuf, t, s);  // this is necessary for default StringEncoder
	return 1;
}

static knh_ConverterDPI_t NOCONV_DSPI = {
	K_DSPI_CONVTO, "NOP",
	NULL,
	conv_NOCONV,
	conv_NOCONV,
	conv_NOCONV,
	conv_NOCONV,
	NULL/*knh_close_NOCONV*/,
	NULL/*knh_conv_NOSET*/,
};

static void Converter_init(CTX, kObject *o)
{
	kConverter *bc = (kConverter*)o;
	bc->conv = NULL;
	bc->dpi = &NOCONV_DSPI;
}

static void Converter_free(CTX, kObject *o)
{
	kConverter *bc = (kConverter*)o;
	if(bc->conv != NULL) {
		bc->dpi->close(_ctx, bc->conv);
		bc->conv = NULL;
		bc->dpi = &NOCONV_DSPI;
	}
}

static kclass_t ConverterDef = {
	Converter_init, DEFAULT_initcopy, DEFAULT_reftrace, Converter_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Converter", CFLAG_Converter, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Converter), 0,
};

static kclass_t StringEncoderDef = {
	Converter_init, DEFAULT_initcopy, DEFAULT_reftrace, Converter_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"StringEncoder", CFLAG_StringEncoder, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(StringEncoder), 0,
};

static kclass_t StringDecoderDef = {
	Converter_init, DEFAULT_initcopy, DEFAULT_reftrace, Converter_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"StringDecoder", CFLAG_StringDecoder, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(StringDecoder), 0,
};

static kclass_t StringConverterDef = {
	Converter_init, DEFAULT_initcopy, DEFAULT_reftrace, Converter_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"StringConverter", CFLAG_StringConverter, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(StringConverter), 0,
};

/* --------------- */
/* Semantics */

#ifdef K_USING_SEMANTICS

static int knh_fichk__nop(kSemantics *u, kint_t v)
{
	return 1;
}

static int knh_ficmp__signed(kSemantics *u, kint_t v1, kint_t v2)
{
	return (int)(v1 - v2);
}

static int knh_ffchk__default(kSemantics *u, kfloat_t v)
{
	return 1;
}

static int knh_ffcmp__default(kSemantics *u, kfloat_t v1, kfloat_t v2)
{
	kfloat_t delta = v1 - v2;
	if(delta == KFLOAT_ZERO) return 0;
	return delta < 0 ? -1 : 1;
}

static kString *knh_fsnew__default(CTX, kcid_t cid, kbytes_t t, kString *orig, int *foundError)
{
	return new_String_(_ctx, cid, t, orig);
}

static int knh_fscmp__default(kSemantics *u, kbytes_t v1, kbytes_t v2)
{
	return knh_bytes_strcmp(v1, v2);
}

#define FUNC(f)     f
#else
#define FUNC(f)     NULL

#endif

static void Semantics_init(CTX, kObject *o)
{
	knh_SemanticsEX_t *b;
#ifdef K_USING_BMGC
	b = DP((kSemantics*)o);
#else
	b = knh_bodymalloc(_ctx, Semantics);
	o->rawptr = b;
#endif
	// common
	b->flag = 0;
	b->ucid  = 0;
	KINITv(b->urn, TS_EMPTY);
	KINITv(b->tag,  TS_EMPTY);
	b->ivalue = NULL;
	b->fvalue = NULL;
	b->svalue = NULL;

	// int
	b->imax  = KINT_MAX;
	b->imin  = KINT_MIN;
	b->fichk = FUNC(knh_fichk__nop);
	b->ficmp = FUNC(knh_ficmp__signed);

	// float
#ifndef K_USED_TO_BE_ON_LKM
	b->fstep = 0.001;
#else
	b->fstep = 1;
#endif
	b->fmax  = KFLOAT_MAX;
	b->fmin  = KFLOAT_MIN;
	b->ffchk = FUNC(knh_ffchk__default);
	b->ffcmp = FUNC(knh_ffcmp__default);
//	b->ffmt = knh_funitfmt__default;
//	b->FMT  = KFLOAT_FMT;

	// String
	b->fsnew = FUNC(knh_fsnew__default);
	b->fscmp = FUNC(knh_fscmp__default);
	//b->fsconv = NULL;
	KINITv(b->bconv, K_NULL);
	b->charlen = 0;
	b->bytelen = 0;
	KINITv(b->pattern, K_NULL);
//	KINITv(b->vocabDictIdx, K_NULL);
}

static void Semantics_reftrace(CTX, kObject *o)
{
	kSemantics *u = (kSemantics*)o;
	knh_SemanticsEX_t *b = DP(u);
	KREFTRACEv((b->urn));
	KREFTRACEv((b->tag));
	KREFTRACEn((b->ivalue));
	KREFTRACEn((b->fvalue));
	KREFTRACEn((b->svalue));
	KREFTRACEv((b->bconv));
	KREFTRACEv((b->pattern));
//	KREFTRACEv((b->vocabDictIdx));
	kref_setSize();
}

static kclass_t SemanticsDef = {
	Semantics_init, TODO_initcopy, Semantics_reftrace, BODY_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Semantics", CFLAG_Semantics, sizeof(knh_SemanticsEX_t), NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Semantics), 0,
};

/* Path */

static void Path_init(CTX, kObject *o)
{
	kPath *pth = (kPath*)o;
	KINITv(pth->urn, TS_EMPTY);
	pth->ospath = S_text(pth->urn);
	pth->asize = 0;
	pth->dpi = knh_getDefaultPathStreamDPI();
}

static void Path_reftrace(CTX, kObject *o)
{
	kPath *pth = (kPath*)o;
	KREFTRACEv(pth->urn);
	kref_setSize();
}

static void Path_free(CTX, kObject *o)
{
	kPath *pth = (kPath*)o;
	if(pth->asize > 0) {
		KFREE(_ctx, (void*)pth->ospath, pth->asize);
		pth->ospath = NULL;
		pth->asize = 0;
	}
}

static void Path_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kPath *pth = (kPath*)o;
	knh_write(_ctx, w, S_tobytes(pth->urn));
}

static kclass_t PathDef = {
	Path_init, TODO_initcopy, Path_reftrace, Path_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Path_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Path", CFLAG_Path, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Path), 0,
};

/* --------------- */
/* InputStream */

static void InputStream_init(CTX, kObject *o)
{
	kInputStream *in = (kInputStream*)o;
	in->decNULL = NULL;
	in->io2 = io2_null();
	KINITv(in->path, ctx->share->cwdPath);
}

static void InputStream_reftrace(CTX, kObject *o)
{
	kInputStream *in = (kInputStream*)o;
	KREFTRACEv(  in->path);
	KREFTRACEn(in->decNULL);
	kref_setSize();
}

static void InputStream_free(CTX, kObject *o)
{
	kInputStream *in = (kInputStream*)o;
	io2_free(_ctx, in->io2);
}

static void InputStream_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kInputStream *in = (kInputStream*)o;
	knh_write_quote(_ctx, w, '\'', S_tobytes(in->path->urn), !String_isASCII(in->path->urn));
}

static kclass_t InputStreamDef = {
	InputStream_init, DEFAULT_initcopy, InputStream_reftrace, InputStream_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, InputStream_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"InputStream", CFLAG_InputStream, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(InputStream), 0,
};

/* --------------- */
/* OutputStream */

static void OutputStream_init(CTX, kObject *o)
{
	kOutputStream *w = (kOutputStream*)o;
	w->io2 = io2_null();
	KINITv(w->path, ctx->share->cwdPath);
	w->encNULL = NULL;
	w->bufferNULL = NULL;
}

static void OutputStream_reftrace(CTX, kObject *o)
{
	kOutputStream *w = (kOutputStream*)o;
	KREFTRACEn((w->encNULL));
	KREFTRACEn((w->bufferNULL));
	KREFTRACEv((w->path));
	kref_setSize();
}

static void OutputStream_free(CTX, kObject *o)
{
	kOutputStream *w = (kOutputStream*)o;
	io2_free(_ctx, w->io2);
}

static void OutputStream_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kOutputStream *ous = (kOutputStream*)o;
	knh_write_quote(_ctx, w, '\'', S_tobytes(ous->path->urn) , !String_isASCII(ous->path->urn));
}

static kclass_t OutputStreamDef = {
	OutputStream_init, DEFAULT_initcopy, OutputStream_reftrace, OutputStream_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, OutputStream_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"OutputStream", CFLAG_OutputStream, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(OutputStream), 0,
};

/* --------------- */
/* Connection */

static void View_init(CTX, kObject *o)
{
	kView *rel = (kView*)rel;
	KINITv(rel->path,  K_NULL);
	KINITv(rel->conf, K_NULL);
}

static void View_reftrace(CTX, kObject *o)
{
	kView *rel = (kView*)rel;
	KREFTRACEv(rel->path);
	KREFTRACEv(rel->conf);
	kref_setSize();
}

static kclass_t ViewDef = {
	View_init, DEFAULT_initcopy, View_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"View", CFLAG_View, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(View), 0,
};

/* --------------- */
/* Script */

static void Script_init(CTX, kObject *o)
{
	kScript *scr = (kScript*)o;
	kcid_t cid = new_ClassId(_ctx);
	kclass_t *ct = varClassTBL(cid);
	scr->h.ct = (kclass_t*)ct;
	DBG_ASSERT(ct->cdef == NULL);
	ct->cflag  = CFLAG_Script;
	ct->magicflag = KNH_MAGICFLAG(ct->cflag);
	ct->bcid   = CLASS_Script;
	ct->baseTBL = ClassTBL(CLASS_Script);
	ct->supcid = CLASS_Script;
	ct->supTBL = ClassTBL(CLASS_Script);
	knh_setClassDef(_ctx, ct, ClassTBL(CLASS_Script)->cdef);
	KINITv(ct->methods, K_EMPTYARRAY);
	KINITv(ct->typemaps, K_EMPTYARRAY);
	knh_setClassName(_ctx, cid, ClassTBL(CLASS_Script)->sname, ClassTBL(CLASS_Script)->sname);
	DBG_ASSERT(ct->defnull == NULL);
	scr->fields = NULL;
	knh_setClassDefaultValue(_ctx, cid, scr, NULL);
	KINITv(scr->ns, new_KonohaSpace(_ctx, ctx->share->rootks));
}

static void Script_p(CTX, kOutputStream *w, kObject *o, int level)
{
	if(IS_FMTdump(level)) {
		kclass_t *ct = O_ct(o);
		kScript *scr = (kScript*)o;
		size_t i;
		for(i = 0; i < ct->fsize; i++) {
			kfieldinfo_t *cf = ct->fields + i;
			if(cf->type == TY_void) continue;
			{
				ktype_t type = ktype_tocid(_ctx, cf->type, ct->cid);
				knh_printf(_ctx, w, "[%d] %T %s=", i, type, FN__(cf->fn));
				knh_write_TObject(_ctx, w, type, scr->fields, i, FMT_line);
				knh_write_EOL(_ctx, w);
			}
		}
	}
	else {
		ObjectField_p(_ctx, w, o, level);
	}
}

static void Script_reftrace(CTX, kObject *o)
{
	kScript *scr = (kScript*)o;
	KREFTRACEv(scr->ns);
	ObjectField_reftrace(_ctx, o FTRDATA);
}

static kclass_t ScriptDef = {
	Script_init, DEFAULT_initcopy, Script_reftrace, ObjectField_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Script_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Script", CFLAG_Script, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Script), 0,
};

/* --------------- */
/* KonohaSpace */

static void KonohaSpace_init(CTX, kObject *o)
{
	kKonohaSpace *ns = (kKonohaSpace*)o;
	knh_KonohaSpaceEX_t *b;
#ifdef K_USING_BMGC
	b = DP(ns);
#else
	b = knh_bodymalloc(_ctx, KonohaSpace);
	ns->b = b;
#endif
	knh_bzero(b, sizeof(knh_KonohaSpaceEX_t));
	KINITv(b->nsname, TS_main);
	KINITv(ns->path, ctx->share->cwdPath);
	ns->parentNULL          = NULL;
	b->ffilinksNULL         = NULL;
	b->linkDictMapNULL      = NULL;
	b->name2ctDictSetNULL  = NULL;
	b->constDictCaseMapNULL = NULL;
	b->formattersNULL       = NULL;
	b->methodsNULL          = NULL;
	ns->gluehdr = NULL;
}

static void KonohaSpace_reftrace(CTX, kObject *o)
{
	kKonohaSpace *ns = (kKonohaSpace*)o;
	knh_KonohaSpaceEX_t *b = DP(ns);
	KREFTRACEv(b->nsname);
	KREFTRACEv(ns->path);
	KREFTRACEn(ns->parentNULL);
	KREFTRACEn(b->ffilinksNULL);
	KREFTRACEn(b->linkDictMapNULL);
	KREFTRACEn(b->name2ctDictSetNULL);
	KREFTRACEn(b->name2dpiNameDictMapNULL);
	KREFTRACEn(b->constDictCaseMapNULL);
	KREFTRACEn(b->formattersNULL);
	KREFTRACEn(b->methodsNULL);
	kref_setSize();
}

static void KonohaSpace_free(CTX, kObject *o)
{
	BODY_free(_ctx, o);
}

static void KonohaSpace_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kKonohaSpace *ns = (kKonohaSpace*)o;
	knh_write_ascii(_ctx, w, "ns:");
	knh_write(_ctx, w, S_tobytes(ns->path->urn));
}

static kclass_t KonohaSpaceDef = {
	KonohaSpace_init, TODO_initcopy, KonohaSpace_reftrace, KonohaSpace_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, KonohaSpace_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"KonohaSpace", CFLAG_KonohaSpace, sizeof(knh_KonohaSpaceEX_t), NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(KonohaSpace), 0,
};

/* --------------- */
/* System */

#ifdef K_USING_NOFILE
#define stdin  NULL
#define stdout NULL
#define stderr NULL
#endif

static kclass_t SystemDef = {
	DEFAULT_init, DEFAULT_initcopy, DEFAULT_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"System", CFLAG_System, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(System), 0,
};

/* --------------- */
/* Context */

static void Context_init(CTX, kObject *o)
{
	KNH_TODO(__FUNCTION__);
}

static void Context_reftrace(CTX, kObject *o)
{
	//KNH_TODO(__FUNCTION__);
}

static void Context_free(CTX, kObject *o)
{
	//KNH_TODO(__FUNCTION__);
}

static kclass_t ContextDef = {
	Context_init, DEFAULT_initcopy, Context_reftrace, Context_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Context", CFLAG_Context, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Context), 0,
};


/* --------------- */
/* Assurance */

static void Assurance_init(CTX, kObject *o)
{
	kAssurance *g = (kAssurance*)o;
	KINITv(g->msg, TS_EMPTY);
	g->aid = 0;
	g->stime = 0;
}

static void Assurance_reftrace(CTX, kObject *o)
{
	kAssurance *g = (kAssurance*)o;
	KREFTRACEv(g->msg);
	kref_setSize();
}

static void Assurance_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kAssurance *g = (kAssurance*)o;
	knh_write_quote(_ctx, w, '\'', S_tobytes(g->msg), !String_isASCII(g->msg));
}

static void Assurance_checkin(CTX, ksfp_t *sfp, kObject *o)
{
	static uintptr_t uid = 0;
	kAssurance *g = (kAssurance*)o;
	g->aid = uid++;
	g->sfp = sfp;
	g->stime = (knh_getTimeMilliSecond() / 1000);
	KNH_NTRACE2(_ctx, "konoha:checkin", K_OK, KNH_LDATA(LOG_i("id", g->aid),
				LOG_s("case", S_text(g->msg))));
	Assurance_setCheckedIn(g, 1);
}

static void Assurance_checkout(CTX, kObject *o, int isFailed)
{
	kAssurance *g = (kAssurance*)o;
	//ksfp_t *sfp = g->sfp;
	intptr_t t = (knh_getTimeMilliSecond() / 1000) - g->stime;
	if(isFailed) {
		KNH_NTRACE2(_ctx, "konoha:assure", K_FAILED, KNH_LDATA(LOG_i("id", g->aid),
					LOG_s("case", S_text(g->msg)), LOG_i("elapsed_time:s", t)));
		knh_logprintf("ac", 0, "FAILED @%s", S_text(g->msg));
	}
	else {
		KNH_NTRACE2(_ctx, "konoha:assure", K_NOTICE, KNH_LDATA(LOG_i("id", g->aid),
					LOG_s("case", S_text(g->msg)), LOG_i("elapsed_time:s", t)));
		knh_logprintf("ac", 0, "PASSED @%s", S_text(g->msg));
	}
	Assurance_setCheckedIn(g, 0);
}

static kclass_t AssuranceDef = {
	Assurance_init, DEFAULT_initcopy, Assurance_reftrace, DEFAULT_free,
	Assurance_checkin, Assurance_checkout, DEFAULT_compareTo, Assurance_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Assurance", CFLAG_Assurance, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Assurance), 0,
};


#include "../sugar/struct.h"

/* --------------- */
/* Term */

static void Term_init(CTX, kObject *o)
{
	kTerm *tk = (kTerm*)o;
	tk->tt        =  TT_ASIS;
	tk->type      =  TY_var;
	tk->uline     =   0;
	tk->flag0 = 0;
	tk->index = 0;
	KINITv(tk->data, K_NULL);
}

static void Term_reftrace(CTX, kObject *o)
{
	kTerm *tk = (kTerm*)o;
	KREFTRACEv(tk->data);
	kref_setSize();
}

const char* TT__(kterm_t tt);

static void Term_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kTerm *tk = (kTerm*)o;
	kterm_t tt = tk->tt;
	if(tt < TT_NUM) {
		knh_write_ascii(_ctx, w, TT__(tt));
		if(tt == TT_PARENTHESIS || tt == TT_BRACE || tt == TT_BRANCET) {
			if(IS_Term(tk->data)) {
				knh_write_InObject(_ctx, w, tk->data, level);
			}
			else if(IS_Array(tk->data)) {
				size_t i;
				for(i = 0; i < kArray_size(tk->list); i++) {
					if(i > 0) kwb_putc(wb, ' ');
					knh_write_InObject(_ctx, w, tk->list->list[i], level);
				}
			}
			if(tt == TT_PARENTHESIS) {
				kwb_putc(wb, ']');
			}else if(tt == TT_BRACE) {
				kwb_putc(wb, '}');
			}else {
				kwb_putc(wb, ']');
			}
		}
	}
	else {
		kbytes_t t = S_tobytes(tk->text);
		int hasUTF8 = !(String_isASCII(tk->text));
		switch(tt) {
		case TT_NUM: knh_write(_ctx, w, t); break;
		case TT_STR:  knh_write_quote(_ctx, w, '"', t, hasUTF8); break;
		case TT_TSTR: knh_write_quote(_ctx, w, '\'', t, hasUTF8); break;
		case TT_ESTR: knh_write_quote(_ctx, w, '`', t, hasUTF8); break;
		case TT_REGEX: knh_write_quote(_ctx, w, '/', t, hasUTF8); break;
//		case TT_DOC:
		case TT_METAN: kwb_putc(wb, '@'); knh_write(_ctx, w, t); break;
		case TT_PROPN: kwb_putc(wb, '$'); knh_write(_ctx, w, t); break;
		case TT_URN: case TT_TLINK:
			knh_write_utf8(_ctx, w, t, hasUTF8); break;
		case TT_NAME: case TT_UNAME:
			if(Term_isDOT(tk)) kwb_putc(wb, '.');
			if(Term_isGetter(tk)) knh_write(_ctx, w, STEXT("get_"));
			else if(Term_isSetter(tk)) knh_write(_ctx, w, STEXT("set_"));
			else if(Term_isISBOOL(tk)) knh_write(_ctx, w, STEXT("is_"));
			knh_write(_ctx, w, t); //break;
			if(Term_isExceptionType(tk)) {
				knh_write(_ctx, w, STEXT("!!"));
			}
			break;
		case TT_FUNCNAME: case TT_UFUNCNAME:
			knh_write(_ctx, w, t); break;
		case TT_PTYPE: {
			kArray *a = tk->list;
			size_t i;
			knh_write_Object(_ctx, w, a->list[0], FMT_line);
			kwb_putc(wb, '<');
			for(i = 1; i < a->size; i++) {
				if(i > 1) kwb_putc(wb, ',');
				knh_write_Object(_ctx, w, a->list[i], FMT_line);
			}
			kwb_putc(wb, '>');
			break;
		}
		case TT_CID:
			knh_write_cid(_ctx, w, tk->cid); break;
		case TT_MN:
			knh_write_mn(_ctx, w, tk->mn); break;
		case TT_CONST:
			knh_write_Object(_ctx, w, tk->data, FMT_line);
			break;
		case TT_SYSVAL: case TT_FVAR: case TT_LVAR:
			if(IS_FMTdump(level) || !IS_String(tk->data)) {
				knh_write_ascii(_ctx, w, TT__(tt));
				kwb_putc(wb, '=');
				knh_write_ifmt(_ctx, w, KINT_FMT, (kint_t)tk->index);
				break;
			}
		case TT_FIELD: case TT_LFIELD:
			if(IS_FMTdump(level) || !IS_String(tk->data)) {
				knh_write_ascii(_ctx, w, TT__(tt));
				kwb_putc(wb, '=');
				if(IS_Term(tk->data)) {
					knh_write_ifmt(_ctx, w, KINT_FMT, (kint_t)tk->index);
				}
				else {
					kwb_putc(wb, '0');
				}
				kwb_putc(wb, '+');
				knh_write_ifmt(_ctx, w, KINT_FMT, (kint_t)tk->index);
				break;
			}
		case TT_ERR:
			knh_write(_ctx, w, t); break;
		default:
			fprintf(stderr, "DEFINE %s in Term_stmt", TT__(tt));
		}
	}
	if(IS_FMTdump(level)) {
		if(SP(tk)->uline != 0) {
			knh_write(_ctx, w, STEXT("+L"));
			knh_write_ifmt(_ctx, w, KINT_FMT, ULINE_line(tk->uline));
		}
		if(SP(tk)->type != TY_var) {
			knh_write(_ctx, w, STEXT("+:")); knh_write_type(_ctx, w, SP(tk)->type);
		}
	}
}

static kclass_t TermDef = {
	Term_init, TODO_initcopy, Term_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, Term_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Term", CFLAG_Term, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Term), 0,
};

/* --------------- */
/* Stmt */

static void StmtExpr_init(CTX, kObject *o)
{
	kStmtExpr *stmt = (kStmtExpr*)o;
	knh_StmtEX_t *b;
#ifdef K_USING_BMGC
	b = DP(stmt);
#else
	b = knh_bodymalloc(_ctx, Stmt);
	o->rawptr = b;
#endif
	SP(stmt)->uline = 0;
	SP(stmt)->stt   = STT_DONE;
	SP(stmt)->type = TY_var;
	(stmt)->terms = NULL;
	b->flag0 = 0;
	b->espidx = 0;
	b->size = 0;
	b->capacity = 0;
	KINITv(b->metaDictCaseMap,  K_NULL);
	b->nextNULL = NULL;
}

static void StmtExpr_reftrace(CTX, kObject *o)
{
	kStmtExpr *stmt = (kStmtExpr*)o;
	knh_StmtEX_t *b = DP((kStmtExpr*)o);
	KREFTRACEv((b->metaDictCaseMap));
	KREFTRACEn((b->nextNULL));
	if(stmt->terms != NULL) {
		size_t i;
		kref_ensureSize(b->size);
		for(i = 0; i < b->size; i++) {
			KREFTRACEv(stmt->terms[i]);
		}
	}
	kref_setSize();
}

static void StmtExpr_free(CTX, kObject *o)
{
	kStmtExpr *stmt = (kStmtExpr*)o;
	knh_StmtEX_t *b = DP((kStmtExpr*)o);
	if(stmt->terms != NULL) {
		KFREE(_ctx, stmt->terms, sizeof(kTerm*) * b->capacity);
		stmt->terms = NULL;
	}
	knh_bodyfree(_ctx, b, Stmt);
}

static void StmtExpr_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kStmtExpr *stmt = (kStmtExpr*)o;
	intptr_t i, size;
	L_TAILCALLED:;
	kwb_putc(wb, '(');
	if(!IS_FMTs(level)) {
		if(IS_Map(DP(stmt)->metaDictCaseMap)) {
			size = knh_Map_size(DP(stmt)->metaDictCaseMap);
			for(i = 0; i < size; i++) {
				kString *k = knh_DictMap_keyAt(DP(stmt)->metaDictCaseMap, i);
				kString *v = (kString*)knh_DictMap_valueAt(DP(stmt)->metaDictCaseMap, i);
				if(k == v) {
					knh_printf(_ctx, w, "@%s ", S_text(k));
				}
				else {
					knh_printf(_ctx, w, "@%s(%O) ", S_text(k), v);
				}
			}
		}
	}
	knh_write_ascii(_ctx, w, TT__(stmt->stt));
	for(i = 0; i < DP(stmt)->size; i++) {
		kwb_putc(wb, ' ');
		knh_write_InObject(_ctx, w, (Object*)(stmt->terms[i]), FMT_line);
	}
	kwb_putc(wb, ')');
	if(IS_FMTdump(level)) {
		if(SP(stmt)->type != TY_var) {
			knh_write(_ctx, w, STEXT("+:")); knh_write_type(_ctx, w, SP(stmt)->type);
		}
	}
	if(DP(stmt)->nextNULL != NULL) {
		if(IS_FMTdump(level)) {
			knh_write_EOL(_ctx, w);
			stmt = DP(stmt)->nextNULL;
			goto L_TAILCALLED;
		}
		else {
			knh_write_dots(_ctx, w);
		}
	}
}

static kclass_t StmtExprDef = {
	StmtExpr_init, TODO_initcopy, StmtExpr_reftrace, StmtExpr_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, StmtExpr_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"StmtExpr", CFLAG_StmtExpr, sizeof(knh_StmtEX_t), NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(StmtExpr), 0,
};

/* GammaBuilder */

static void GammaBuilder_init(CTX, kObject *o)
{
	knh_GammaBuilderEX_t *b;
#ifdef K_USING_BMGC
	b = DP((kGammaBuilder*)o);
#else
	b = knh_bodymalloc(_ctx, GammaBuilder);
	o->rawptr = b;
#endif
	knh_bzero(b, sizeof(knh_GammaBuilderEX_t));
	b->cflag = FLAG_GammaBuilder_InlineFunction | FLAG_GammaBuilder_TailRecursion;
	KINITv(b->mtd, K_NULL);
	KINITv(b->stmt, K_NULL);
	KINITv(b->lstacks, new_Array0(_ctx, 0));
	KINITv(b->insts, new_Array0(_ctx, 0));
	KINITv(b->errmsgs, new_Array0(_ctx, 0));
	KINITv(b->finallyStmt, K_NULL);
	KINITv(((kGammaBuilder*)o)->scr, ctx->script);
}

static void GammaBuilder_reftrace(CTX, kObject *o)
{
	size_t i;
	knh_GammaBuilderEX_t *b = DP((kGammaBuilder*)o);
	kref_ensureSize(b->gcapacity * 3);
	for(i = 0; i < b->gcapacity; i++) {
		KREFTRACEv(b->gf[i].tkIDX);
		KREFTRACEv(b->gf[i].tk);
	}
	KREFTRACEv((b->mtd));
	KREFTRACEv((b->stmt));
	KREFTRACEv((b->lstacks));
	KREFTRACEv((b->insts));
	KREFTRACEv((b->errmsgs));
	KREFTRACEv((b->finallyStmt));
	KREFTRACEv(((kGammaBuilder*)o)->scr);
	kref_setSize();
}

static void GammaBuilder_free(CTX, kObject *o)
{
	knh_GammaBuilderEX_t *b = DP((kGammaBuilder*)o);
	if(b->gcapacity) {
		KFREE(_ctx, b->gf, b->gcapacity * sizeof(knh_gamma2_t));
	}
	knh_bodyfree(_ctx, b, GammaBuilder);
}

static kclass_t GammaBuilderDef = {
	GammaBuilder_init, TODO_initcopy, GammaBuilder_reftrace, GammaBuilder_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"GammaBuilder", CFLAG_GammaBuilder, sizeof(knh_GammaBuilderEX_t), NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(GammaBuilder), 0,
};

/* --------------- */
/* BasicBlock */

static void BasicBlock_init(CTX, kObject *o)
{
	kBasicBlock *bb = (kBasicBlock*)o;
#ifndef K_USING_BMGC
	bb->b = knh_bodymalloc(_ctx, BasicBlock);
#endif
	knh_bzero(DP(bb), sizeof(knh_BasicBlockEX_t));
	bb->listNC  = NULL;
	bb->nextNC  = NULL;
	bb->jumpNC  = NULL;
}

static void BasicBlock_reftrace(CTX, kObject *o)
{
	kBasicBlock *bb = (kBasicBlock*)o;
	size_t i;
	kref_ensureSize(DP(bb)->size);
	for(i = 0; i < DP(bb)->size; i++) {
		tail_ = kopl_reftrace(_ctx, DP(bb)->opbuf + i FTRDATA);
	}
	kref_setSize();
}

static void BasicBlock_free(CTX, kObject *o)
{
	kBasicBlock *bb = (kBasicBlock*)o;
	if(DP(bb)->capacity > 0) {
		KFREE(_ctx, DP(bb)->opbuf, DP(bb)->capacity * sizeof(kopl_t));
	}
	knh_bodyfree(_ctx, DP(bb), BasicBlock);
}

static kclass_t BasicBlockDef = {
	BasicBlock_init, TODO_initcopy, BasicBlock_reftrace, BasicBlock_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"BasicBlock", CFLAG_BasicBlock, sizeof(knh_BasicBlockEX_t), NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(BasicBlock), 0,
};

/* --------------- */
/* KonohaCode */

static void KonohaCode_init(CTX, kObject *o)
{
	kKonohaCode *b = (kKonohaCode*)o;
	b->codesize = 0;
	b->code = NULL;
	b->fileid = 0;
	KINITv(b->source, TS_EMPTY);
}

static void KonohaCode_reftrace(CTX, kObject *o)
{
	kKonohaCode *b = (kKonohaCode*)o;
	kopl_t *pc = b->code;
	KREFTRACEv(b->source);
	kref_ensureSize(b->codesize / sizeof(kopl_t));
	while(pc->opcode != OPCODE_RET) {
		tail_ = kopl_reftrace(_ctx, pc FTRDATA);
		pc++;
	}
	kref_setSize();
}

static void KonohaCode_free(CTX, kObject *o)
{
	kKonohaCode *b = (kKonohaCode*)o;
	KFREE(_ctx, b->code, b->codesize);
}

static void KonohaCode_p(CTX, kOutputStream *w, kObject *o, int level)
{
	kKonohaCode *kcode = (kKonohaCode*)o;
	kopl_t *pc = kcode->code + 1;
	while(1) {
		knh_opcode_dump(_ctx, pc, w, kcode->code + 1);
		if(pc->opcode == OPCODE_RET) break;
		pc++;
	}
}

static kclass_t KonohaCodeDef = {
	KonohaCode_init, TODO_initcopy, KonohaCode_reftrace, KonohaCode_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, KonohaCode_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"KonohaCode", CFLAG_KonohaCode, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(KonohaCode), 0,
};

static kclass_t ImmutableDef = {
	DEFAULT_init, DEFAULT_initcopy, DEFAULT_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"Immutable", CFLAG_Immutable, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(Immutable), 0,
};

static kclass_t KindOfDef = {
	DEFAULT_init, DEFAULT_initcopy, DEFAULT_reftrace, DEFAULT_free,
	DEFAULT_checkin, DEFAULT_checkout, DEFAULT_compareTo, DEFAULT_p,
	DEFAULT_getkey, DEFAULT_hashCode, DEFAULT_0, DEFAULT_1,
	DEFAULT_findTypeMapNULL, DEFAULT_wdata, DEFAULT_2, DEFAULT_3,
	"KindOf", CFLAG_KonohaCode, 0, NULL,
	NULL, DEFAULT_4, DEFAULT_5, sizeof_O(KindOf), 0,
};

/* --------------- */
/* @data*/

typedef struct {
	const char   *name;
	int           index;
} knh_StringData0_t ;

typedef struct {
	const char *name;
	ksymbol_t fn;
} kloadsymbol_t ;


#include"operator.c"
#include"structdata.h"

/* ------------------------------------------------------------------------ */

static Object *knh_Context_fdefault(CTX, kcid_t cid)
{
	KNH_TODO(__FUNCTION__);
	return (Object*)ctx;
}

static void knh_setDefaultValues(CTX)
{
	knh_setClassDefaultValue(_ctx, CLASS_Object, K_NULL, NULL);
	knh_setClassDefaultValue(_ctx, CLASS_Tdynamic, K_NULL, NULL);
	knh_setClassDefaultValue(_ctx, CLASS_Boolean, K_FALSE, NULL);
	{
		kInt *io = new_H(Int);
		(io)->n.ivalue = 0;
		Object_setNullObject(io, 1);
		knh_setClassDefaultValue(_ctx, CLASS_Int, io, NULL);
	}
	{
		kFloat *fo = new_H(Float);
		(fo)->n.fvalue = KFLOAT_ZERO;
		Object_setNullObject(fo, 1);
		knh_setClassDefaultValue(_ctx, CLASS_Float, fo, NULL);
	}
	{
		kString *so = new_H(String);
		so->str.text = "";
		so->str.len = 0;
		so->hashCode = 0;
		String_setASCII(so, 1);
		String_setTextSgm(so, 1);
		Object_setNullObject(so, 1);
		knh_setClassDefaultValue(_ctx, CLASS_String, so, NULL);
	}
	knh_setClassDefaultValue(_ctx, CLASS_Path, ctx->share->cwdPath, NULL);
	knh_setClassDefaultValue(_ctx, CLASS_Class, new_Type(_ctx, CLASS_Tvoid), NULL);

#if defined(K_USING_SEMANTICS)
	{
		kSemantics *u = new_(Semantics);
		KINITv(DP(u)->ivalue, KNH_INT0);
		KINITv(DP(u)->fvalue, KNH_FLOAT0);
		KINITv(DP(u)->svalue, TS_EMPTY);
		knh_setClassDefaultValue(_ctx, CLASS_Semantics, u, NULL);
	}
#endif
	// load file/Channel/regex/db drivers
	knh_setClassDefaultValue(_ctx, CLASS_Context, K_NULL, knh_Context_fdefault);
	knh_setClassDefaultValue(_ctx, CLASS_KonohaSpace, UPCAST(_ctx->share->rootks), NULL);
	knh_setClassDefaultValue(_ctx, CLASS_Lang, UPCAST(_ctx->share->corelang), NULL);
//	knh_setClassDefaultValue(_ctx, CLASS_System, UPCAST(_ctx->sys), NULL);
	knh_loadSystemDriver(_ctx, ctx->share->rootks);
	{
		kTerm *tk = KNH_TNULL(Term);
		tk->tt = TT_FVAR;
		(tk)->index = 0;
	}
}

static void knh_loadScriptFieldNameData0(CTX, kloadsymbol_t *data)
{
	while(data->name != NULL) {
		kString *name = new_T(data->name);
#if defined(K_USING_DEBUG)
		ksymbol_t fn = knh_addname(_ctx, name, knh_DictSet_append);
		DBG_ASSERT(fn == data->fn - MN_OPSIZE);
#else
		knh_addname(_ctx, name, knh_DictSet_append);
#endif
		data++;
	}
}

/* ------------------------------------------------------------------------ */
/* @data */

static const knh_IntData_t IntConstData0[] = {
	{"Int.MAX", KINT_MAX},
	{"Int.MIN", KINT_MIN},
	{NULL, 0}
};

static const knh_FloatData_t FloatConstData0[] = {
	{"Float.MAX", KFLOAT_MAX},
	{"Float.MIN", KFLOAT_MIN},
	{"Float.EPSLON", KFLOAT_EPSILON},
	{NULL, KFLOAT_ZERO}
};

static const knh_StringData_t StringConstData0[] = {
	{"$konoha.version", K_VERSION},
	{NULL, NULL},
};

#define FN_K FN_k
#define FN_V FN_v
#define FN_T FN_t
#define FN_U FN_u
#define FN_P FN_p
#define FN_R FN_r

#define _D(s)  (kloaddata_t)s
static const kloaddata_t CParamData0[] = {
	DATA_CPARAM, CLASS_Iterator,  _D("konoha.Iterator<dynamic>"), _D("dynamic" PT_Iterator), 1, 0, T_dyn, FN_V,
	DATA_CPARAM, CLASS_Range,     _D("konoha.Range<dynamic>"), _D("Range<dynamic>"), 1, 0, T_dyn, FN_V,
	DATA_CPARAM, CLASS_Array,     _D("konoha.Array<dynamic>"), _D("dynamic" PTY_Array), 1, 0, T_dyn, FN_V,
	DATA_CPARAM, CLASS_Map,       _D("konoha.Map<konoha.String,dynamic>"), _D("Map<String,dynamic>"), 2, 0, TY_String, FN_K, T_dyn, FN_V,
	DATA_CPARAM, CLASS_Tuple,     _D("konoha.Map<konoha.String,dynamic>"), _D("Tuple<String,dynamic>"), 2, 0, TY_String, FN_K, T_dyn, FN_V,
	DATA_CPARAM, CLASS_Func,      _D("konoha.Func<void>"), _D("Func<void>"), 0, 0,
	DATA_CPARAM, CLASS_Thunk,     _D("konoha.Thunk<dynamic>"), _D("Thunk<dynamic>"), 1, 0, T_dyn, FN_V,
	DATA_CPARAM, CLASS_Immutable, _D("konoha.Immutable<dynamic>"), _D("dynamic" PT_Immutable), 1, 0, T_dyn, FN_V,
	DATA_CPARAM, CLASS_KindOf,    _D("konoha.KindOf<void>"), _D("dynamic" PT_KindOf), 1, 0, TY_void, FN_V,
	0,
};

void kshare_loadStructData(CTX, const knh_LoaderAPI_t *kapi)
{
	kapi->loadData(_ctx, StructData0, NULL);
	{
		kclass_t *t = varClassTBL(CLASS_Tvoid);
		t->supcid = CLASS_Object;
		t->supTBL = ClassTBL(CLASS_Object);
		t = varClassTBL(CLASS_Tvar);
		t->supcid = CLASS_Object;
		t->supTBL = ClassTBL(CLASS_Object);
	}
}

void knh_loadScriptSystemString(CTX)
{
	const char **data = StringData0;
	size_t i = 0;
	for(i = 0; *data != NULL; i++) {
		DBG_ASSERT(_ctx->share->tString[i] == NULL);
		DBG_ASSERT(i < (SIZEOF_TSTRING / sizeof(kString*)));
		KINITv(_ctx->share->tString[i], new_T(*data));
		data++;
	}
}

void knh_loadScriptSystemData(CTX, kKonohaSpace *ns, const knh_LoaderAPI_t *kapi)
{
	kapi->loadData(_ctx, ClassData0, NULL);
	kapi->loadData(_ctx, CParamData0, NULL);
	kapi->loadIntData(_ctx, ns, IntConstData0);
	kapi->loadFloatData(_ctx, ns, FloatConstData0);
	kapi->loadStringData(_ctx, ns, StringConstData0);
	knh_getURI(_ctx, STEXT("(eval)"));  // URI_EVAL
	knh_setDefaultValues(_ctx);
	knh_loadScriptDefaultMapDSPI(_ctx, ns);
}

void knh_loadScriptSystemMethod(CTX, const knh_LoaderAPI_t *kapi)
{
	kParam *pools[K_PARAM0_SIZE];
	knh_loadScriptFieldNameData0(_ctx, FieldNameData0);
	kapi->loadData(_ctx, ParamData0, pools);
	kapi->loadData(_ctx, APIData0, pools);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
