/*
 * klib.c
 *
 *  Created on: Jan 28, 2012
 *      Author: kimio
 */

#define K_INTERNAL
#include <konoha2/konoha2.h>
#include <konoha2/klib.h>
#include "konoha2/gc.h"

#ifdef __cplusplus
extern "C" {
#endif

// library

#define KPROMAP_DELTA 3

static inline kpromap_t * kpromap_null(void)  // for proto_get safe null
{
	static kprodata_t dnull[3] = {};
	static kpromap_t pnull = {
		.datamap = dnull,
		.size = 1,
		.capacity = 0,
	};
	return &pnull;
}

static kprodata_t* kpromap_get(kpromap_t *p, ksymbol_t key)
{
	kprodata_t *d = p->datamap + ((size_t)key) % p->size;
	if(d->key == key) return d; else d++;
	if(d->key == key) return d; else d++;
	if(d->key == key) return d; else d++;
	size_t i;
	for(i = 0; i < KPROMAP_DELTA - 3; i++) {
		if(d->key == key) return d;
		d++;
	}
	return NULL;
}

static inline void kpromap_findset(kprodata_t *d, kprodata_t *newd)
{
	size_t i;
	for(i = 0; i < KPROMAP_DELTA - 1; i++) {
		if(newd->key == 0) {
			*newd = *d;
			return;
		}
		newd++;
	}
}

static void kpromap_rehash(CTX, kpromap_t *p)
{
	size_t i, newcapacity = p->capacity * 2, newsize = newcapacity - KPROMAP_DELTA;
	kprodata_t *newdatamap = (kprodata_t*)KNH_MALLOC(sizeof(kprodata_t) * newcapacity);
	for(i = 0; i < p->capacity; i++) {
		kprodata_t *d = p->datamap + i;
		if(d->key != 0) {
			kprodata_t *newd = newdatamap + ((size_t)d->key) % newsize;
			if(newd->key == 0) {
				*newd = *d;
			}
			else {
				kpromap_findset(d, newd+1);
			}
		}
	}
	KNH_FREE(p->datamap, sizeof(kprodata_t) * p->capacity);
	p->capacity = newcapacity;
	p->datamap = newdatamap;
	p->size = newsize;
}

static kpromap_t *kpromap_new(CTX)
{
	kpromap_t *p;
	p = (kpromap_t*)KNH_MALLOC(sizeof(kpromap_t));
	p->datamap = (kprodata_t*)KNH_MALLOC(sizeof(kprodata_t) * 8);
	p->capacity = 8;
	p->size = p->capacity - KPROMAP_DELTA;
	return p;
}

void kpromap_free(CTX, kpromap_t *p)
{
	if(p->size > 1) {
		KNH_FREE(p->datamap, p->capacity * sizeof(kprodata_t));
		KNH_FREE(p, sizeof(kpromap_t));
	}
}

static void kpromap_set(CTX, kpromap_t **pval, ksymbol_t key, ktype_t type, uintptr_t val)
{
	kpromap_t *p = pval[0];
	if(p->size == 1) {
		p = kpromap_new(_ctx);
		pval[0] = p;
	}
	do {
		size_t i;
		kprodata_t *d = p->datamap + ((size_t)key) % p->size;
		for(i = 0; i < KPROMAP_DELTA; i++) {
			if(d->key == key) {
				d->type = type; d->val = val;
				return;
			}
			if(d->key == 0) {
				d->key = key; d->type = type; d->val = val;
				return;
			}
			d++;
		}
		kpromap_rehash(_ctx, p);
	}
	while(1);
}

void kpromap_each(CTX, kpromap_t *p, void *arg, void (*f)(CTX, void *, kprodata_t *d))
{
	size_t i;
	kprodata_t *d = p->datamap;
	for (i = 0; i < p->capacity; ++i, ++d) {
		f(_ctx, arg, d);
	}
}

static kObject* Object_getObjectNULL(CTX, kObject *o, ksymbol_t key)
{
	kprodata_t *d = kpromap_get(o->h.proto, key | FN_BOXED);
	if(d != NULL) {
		return d->oval;
	}
	return NULL;
}

static void Object_setObject(CTX, kObject *o, ksymbol_t key, ktype_t ty, kObject *val)
{
	kpromap_set(_ctx, &o->h.proto, key | FN_BOXED, ty, (uintptr_t)val);
}

void kpromap_reftrace(CTX, kpromap_t *p)
{
	size_t i;
	kprodata_t *d = p->datamap;
	BEGIN_REFTRACE(p->capacity);
	for(i = 0; i < p->capacity; i++) {
		if((d->key & FN_BOXED) == FN_BOXED) {
			KREFTRACEv(d->oval);
		}
		d++;
	}
	END_REFTRACE();
}

static size_t size64(size_t s)
{
	size_t base = sizeof(kObject);
	while(base < s) {
		base *= 2;
	}
	return base;
}

static void DEFAULT_free(CTX, kRawPtr *o)
{
	(void)_ctx;(void)o;
}

static void DEFAULT_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kwb_printf(wb, "%s:&%p", T_cid(O_cid(sfp[pos].o)), sfp[pos].o);
}

static uintptr_t DEFAULT_unbox(CTX, kObject *o)
{
	return 0;
}

static kObject* DEFAULT_fnull(CTX, const kclass_t *ct)
{
	DBG_ASSERT(ct->nulvalNUL != NULL);
	return ct->nulvalNUL;
}

static kObject* DEFAULT_fnullinit(CTX, const kclass_t *ct)
{
	assert(ct->nulvalNUL == NULL);
	KINITv(((kclass_t*)ct)->nulvalNUL, new_kObject(ct, 0));
	kObject_setNullObject(ct->nulvalNUL, 1);
	((kclass_t*)ct)->fnull = DEFAULT_fnull;
	return ct->nulvalNUL;
}

static kObject *CT_null(CTX, const kclass_t *ct)
{
	return ct->fnull(_ctx, ct);
}

static kclass_t* new_CT(CTX, KCLASSDEF *s, kline_t pline)
{
	kshare_t *share = _ctx->share;
	kcid_t newid = share->ca.size;
	if(share->ca.size == share->ca.max) {
		KARRAY_EXPAND(share->ca, newid + 1, kclass_t);
	}
	share->ca.size = newid + 1;
	kclass_t *ct = (kclass_t*)KNH_ZMALLOC(sizeof(kclass_t));
	_ctx->share->ca.ClassTBL[newid] = (const kclass_t*)ct;
	ct->cid = newid;
	if(s != NULL) {
		ct->cflag  = s->cflag;
		ct->bcid   = s->bcid;
		ct->supcid = s->supcid == 0 ? CLASS_Object : s->supcid;
		ct->fields = s->fields;
		ct->fsize  = s->fsize;
		ct->fallocsize = s->fallocsize;
		ct->cstruct_size = size64(s->cstruct_size);
		ct->DBG_NAME = (s->structname != NULL) ? s->structname : "N/A";
		// function
		if(ct->bcid == 0) {
			ct->bcid = ct->cid;
			ct->init = s->init;
			ct->reftrace = s->reftrace;
			ct->p     = (s->p != NULL) ? s->p : DEFAULT_p;
			ct->unbox = (s->unbox != NULL) ? s->unbox : DEFAULT_unbox;
			ct->free = (s->free != NULL) ? s->free : DEFAULT_free;
			ct->fnull = (s->fnull != NULL) ? s->fnull : DEFAULT_fnullinit;
			ct->initdef = s->initdef;
		}
		else {
			const kclass_t *bct = kclass(ct->bcid, pline);
			ct->init =  bct->init;
			ct->reftrace = bct->reftrace;
			ct->p     =  bct->p;
			ct->unbox =  bct->unbox;
			ct->free  =  bct->free;
			ct->fnull =  bct->fnull ;
			ct->initdef = bct->initdef;
		}
		if(s->initdef != NULL) {
			s->initdef(_ctx, ct, pline);
		}
	}
	return ct;
}

static const kclass_t* Kclass(CTX, kcid_t cid, kline_t pline)
{
	kshare_t *share = _ctx->share;
	if(cid < share->ca.size) {
		return share->ca.ClassTBL[cid];
	}
	kreportf(ERR_, pline, "invalid cid=%d", (int)cid);
	kraise(0);
	return share->ca.ClassTBL[0];
}


static void casehash_add(CTX, kmap_t *kmp, kString *skey, uintptr_t uvalue)
{
	uintptr_t hcode = casehash(S_text(skey), S_size(skey));
	kmape_t *e = kmap_newentry(kmp, hcode);
	KINITv(e->skey, skey);
	e->uvalue = uvalue;
	kmap_add(kmp, e);
}

static void CT_setName(CTX, kclass_t *ct, kString *name, kline_t pline)
{
	DBG_ASSERT(ct->name == NULL);
	kreportf(DEBUG_, pline, "new class name='%s'", S_text(name));
	KINITv(ct->name, name);
	if(ct->packdom == 0) {
		casehash_add(_ctx, _ctx->share->classnameMapNN, name, ct->cid);
	}
	if(ct->methods == NULL) {
		KINITv(ct->methods, new_(Array, 0));
	}
	if(ct->cparam == NULL) {
		KINITv(ct->cparam, K_NULLPARAM);
	}
}

static const kclass_t *CT_body(CTX, const kclass_t *ct, size_t head, size_t body)
{
	while(ct->cstruct_size < sizeof(kObjectHeader) + head + body) {
		if(ct->simbody == NULL) {
			kclass_t *newct = new_CT(_ctx, NULL, 0);
			memcpy(newct, ct, sizeof(kclass_t));
			newct->cstruct_size *= 2;
			newct->fallocsize = 0; // for safety
			newct->constNameMapSO = NULL;
			newct->constPoolMapNO = NULL;  // for safety
			((kclass_t*)ct)->simbody = (const kclass_t*)newct;
		}
		ct = ct->simbody;
	}
	return ct;
}

static ksymbol_t KMAP_getcode(CTX, kmap_t *symmap, kArray *list, const char *name, size_t len, uintptr_t hcode, ksymbol_t def)
{
	kmape_t *e = kmap_get(symmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && len == S_size(e->skey) && strncmp(S_text(e->skey), name, len) == 0) {
			return (ksymbol_t)e->uvalue;
		}
		e = e->next;
	}
	if(def == FN_NEWID) {
		kString *skey = new_kString(name, len, SPOL_ASCII|SPOL_POOL);
		uintptr_t sym = kArray_size(list);
		kArray_add(list, skey);
		e = kmap_newentry(symmap, hcode);
		KINITv(e->skey, skey);
		e->uvalue = sym;
		kmap_add(symmap, e);
		return (ksymbol_t)e->uvalue;
	}
	return def;
}

static kuri_t Kuri(CTX, const char *name, size_t len)
{
	uintptr_t hcode = strhash(name, len);
	return KMAP_getcode(_ctx, _ctx->share->uriMapNN, _ctx->share->uriList, name, len, hcode, FN_NEWID);
}

static const char* ksymbol_norm(char *buf, const char *t, size_t *lenR, uintptr_t *hcodeR, ksymbol_t *mask, int pol)
{
	int i, toup = 0, len = (*lenR > 128) ? 128 : *lenR;
	char *w = buf;
	for(i = 0; i < len; i++) {
		int ch = t[i];
		if(ch == 0) break;
		if(ch == '_') {
			if(toup > 0) toup = 2;
			continue;
		}
		ch = (toup == 2) ? toupper(ch) : ch;
		toup = 1;
		*w = ch; w++;
	}
	*w = 0;
	*lenR = w - buf;
	t = buf;
	if(pol == SYMPOL_METHOD) {
		if(buf[1] == 'e' && buf[2] == 't') {
			if(buf[0] == 'g') {
				*lenR -= 3; *mask = MN_GETTER;
				t = buf + 3;
			}
			else if(buf[0] == 's') {
				*lenR -= 3; *mask = MN_SETTER;
				t = buf + 3;
			}
		}
		else if(buf[0] == 'i' && buf[1] == 's') {
			*lenR -= 2; *mask = MN_ISBOOL;
			t = buf + 2;
		}
	}
	w = (char*)t;
	uintptr_t hcode = 0;
	while(*w != 0) {
		int ch = *w;
		hcode = tolower(ch) + (31 * hcode);
		w++;
	}
	*hcodeR = hcode;
	return t;
}

static ksymbol_t Ksymbol(CTX, const char *name, size_t len, ksymbol_t def, int pol)
{
	uintptr_t hcode = 0;
	if(pol == SYMPOL_RAW) {
		size_t i;
		for(i = 0; i < len; i++) {
			int ch = name[i];
			if(ch == 0) {
				len = i; break;
			}
			hcode = ch + (31 * hcode);
		}
		return KMAP_getcode(_ctx, _ctx->share->symbolMapNN, _ctx->share->symbolList, name, len, hcode, def);
	}
	else {
		char buf[256];
		ksymbol_t sym, mask = 0;
		name = ksymbol_norm(buf, name, &len, &hcode, &mask, pol);
		sym = KMAP_getcode(_ctx, _ctx->share->symbolMapNN, _ctx->share->symbolList, name, len, hcode, def);
		if(def == sym) return def;
		return sym | mask;
	}
}

static const char* usymbol_norm(char *buf, size_t bufsiz, const char *t, size_t len, uintptr_t *hcodeR)
{
	uintptr_t i, hcode = 0;
	char *p = buf;
	for(i = 0; i < len; i++) {
		if(t[i]==0) break;
		int ch = toupper(t[i]);     // Int_MAX and INT_MAX are same
		if(ch == '_') continue;   // INT_MAX and INTMAX  are same
		*p = ch; p++;
		hcode = ch + (31 * hcode);
	}
	*p=0;
	*hcodeR=hcode;
	return (const char*)buf;
}

static ksymbol_t Kusymbol(CTX, const char *name, size_t len)
{
	uintptr_t hcode = 0;
	char buf[len+1];
	name = usymbol_norm(buf, len+1, name, len, &hcode);
	len = strlen(name);
	return KMAP_getcode(_ctx, _ctx->share->usymbolMapNN, _ctx->share->usymbolList, name, len, hcode, FN_NEWID);
}

// -------------------------------------------------------------------------

#define TYPENAME(C) \
	.structname = #C,\
	.cid = CLASS_T##C,\
	.cflag = CFLAG_T##C\

#define CLASSNAME(C) \
	.structname = #C,\
	.cid = CLASS_##C,\
	.cflag = CFLAG_##C,\
	.cstruct_size = sizeof(k##C)\

static void DEFAULT_init(CTX, kRawPtr *o, void *conf)
{

}

static KCLASSDEF TvoidDef = {
	TYPENAME(void),
	.init = DEFAULT_init,
};

static KCLASSDEF TvarDef = {
	TYPENAME(var),
	.init = DEFAULT_init,
};

static void Object_init(CTX, kRawPtr *o, void *conf)
{
	kObject *of = (kObject*)o;
	of->ndata[0] = 0;
	of->ndata[1] = 0;
}

static void Object_reftrace(CTX, kRawPtr *o)
{
	kObject *of = (kObject*)o;
	const kclass_t *ct = O_ct(of);
	BEGIN_REFTRACE(ct->fsize);
	size_t i;
	for(i = 0; i < ct->fsize; i++) {
		if(ct->fields[i].isobj) {
			KREFTRACEv(of->fields[i]);
		}
	}
	END_REFTRACE();
}

static void ObjectX_init(CTX, kRawPtr *o, void *conf)
{
	kObject *of = (kObject*)o;
	const kclass_t *ct = O_ct(of);
	assert(ct->nulvalNUL != NULL);
	memcpy(of->fields, ct->nulvalNUL->fields, ct->cstruct_size - sizeof(kObjectHeader));
}

static void Object_initdef(CTX, kclass_t *ct, kline_t pline)
{
	if(ct->cid == TY_Object) return;
	DBG_P("new object initialization");
	const kclass_t *supct = kclass(ct->cid, pline);
	if(CT_isUNDEF(supct)) {
		kreportf(ERR_, pline, "%s's fields are not all set", T_cid(ct->cid));
		kraise(0);
	}
	size_t fsize = supct->fsize + ct->fsize;
	ct->cstruct_size = size64((fsize * sizeof(void*)) + sizeof(kObjectHeader));
	KSETv(ct->nulvalNUL, new_kObject(ct, NULL));
	if(fsize > 0) {
		ct->init = ObjectX_init;
	}
	ct->fnull = DEFAULT_fnull;
}

static KCLASSDEF ObjectDef = {
	CLASSNAME(Object),
	.init = Object_init,
	.reftrace = Object_reftrace,
	.initdef = Object_initdef,
};

// Object API

static kObject *new_Object(CTX, const kclass_t *ct, void *conf)
{
	DBG_ASSERT(ct->cstruct_size > 0);
	kObject *o = (kObject*) MODGC_omalloc(_ctx, ct->cstruct_size);
	o->h.magicflag = ct->magicflag;
	o->h.ct = ct;
	o->h.proto = kpromap_null();
	ct->init(_ctx, (kRawPtr*)o, conf);
	return o;
}

static uintptr_t Number_unbox(CTX, kObject *o)
{
	kInt *n = (kInt*)o;
	return (uintptr_t) n->n.data;
}

// Boolean
static void Boolean_init(CTX, kRawPtr *o, void *conf)
{
	kBoolean *n = (kBoolean*)o;
	n->n.bvalue = (kbool_t)conf;
}

static void Boolean_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kwb_printf(wb, sfp[pos].bvalue ? "true" : "false");
}

static KCLASSDEF BooleanDef = {
	CLASSNAME(Boolean),
	.init = Boolean_init,
	.unbox = Number_unbox,
	.p    = Boolean_p,
};

// Int
static void Int_init(CTX, kRawPtr *o, void *conf)
{
	kInt *n = (kInt*)o;
	n->n.ivalue = (kint_t)conf;
}

static void Int_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kwb_printf(wb, KINT_FMT, sfp[pos].ivalue);
}

static KCLASSDEF IntDef = {
	CLASSNAME(Int),
	.init = Int_init,
	.unbox = Number_unbox,
	.p    = Int_p,
};

// String

static void String_init(CTX, kRawPtr *o, void *conf)
{
	kString *s = (kString*)o;
	s->str.text = "";
	s->str.len = 0;
	s->hashCode = 0;
}

static void String_free(CTX, kRawPtr *o)
{
	kString *s = (kString*)o;
	if(S_isMallocText(s)) {
		KNH_FREE(s->str.buf, S_size(s) + 1);
	}
}

static void String_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	if(level == 0) {
		kwb_printf(wb, "%s", S_text(sfp[pos].o));
	}
	else {
		kwb_printf(wb, "\"%s\"", S_text(sfp[pos].o));
	}
}

static uintptr_t String_unbox(CTX, kObject *o)
{
	kString *s = (kString*)o;
	return (uintptr_t) s->str.text;
}

//static int String_compareTo(kRawPtr *o, kRawPtr *o2)
//{
//	return knh_bytes_strcmp(S_tobytes((kString*)o) ,S_tobytes((kString*)o2));
//}

static KCLASSDEF StringDef = {
	CLASSNAME(String),
	.init = String_init,
	.free = String_free,
	.p    = String_p,
	.unbox = String_unbox
};

static void String_checkASCII(CTX, kString *s)
{
	unsigned char ch = 0;
	long len = S_size(s), n = (len + 3) / 4;
	const unsigned char*p = (const unsigned char *)S_text(s);
	switch(len % 4) { /* Duff's device written by ide */
		case 0: do{ ch |= *p++;
		case 3:     ch |= *p++;
		case 2:     ch |= *p++;
		case 1:     ch |= *p++;
		} while(--n>0);
	}
	S_setASCII(s, (ch < 128));
}

static kString* new_String(CTX, const char *text, size_t len, int policy)
{
	const kclass_t *ct = CT_(CLASS_String);
	kString *s = NULL; //knh_PtrMap_getS(_ctx, ct->constPoolMapNULL, text, len);
	if(s != NULL) return s;
	if(TFLAG_is(int, policy, SPOL_TEXT)) {
		s = (kString*)new_Object(_ctx, ct, NULL);
		s->str.text = text;
		s->str.len = len;
		s->hashCode = 0;
		S_setTextSgm(s, 1);
	}
	else if(len + 1 < sizeof(void*) * 2) {
		s = (kString*)new_Object(_ctx, ct, NULL);
		s->str.buf = (char*)(&(s->hashCode));
		s->str.len = len;
		memcpy(s->str.ubuf, text, len);
		s->str.ubuf[len] = '\0';
		S_setTextSgm(s, 1);
	}
	else {
		s = (kString*)new_Object(_ctx, ct, NULL);
		s->str.len = len;
		s->str.buf = (char*)KNH_MALLOC(len+1);
		memcpy(s->str.ubuf, text, len);
		s->str.ubuf[len] = '\0';
		s->hashCode = 0;
		S_setMallocText(s, 1);
	}
	if(TFLAG_is(int, policy, SPOL_ASCII)) {
		S_setASCII(s, 1);
	}
	else if(TFLAG_is(int, policy, SPOL_UTF8)) {
		S_setASCII(s, 0);
	}
	else {
		String_checkASCII(_ctx, s);
	}
//	if(TFLAG_is(int, policy, SPOL_POOL)) {
//		kmapSN_add(_ctx, ct->constPoolMapNO, s);
//		S_setPooled(s, 1);
//	}
	return s;
}

// Array

typedef struct {
	kObjectHeader h;
	karray_t astruct;
} kArray_;

static void Array_init(CTX, kRawPtr *o, void *conf)
{
	kArray_ *a = (kArray_*)o;
	a->astruct.body     = NULL;
	a->astruct.size     = 0;
	a->astruct.max = (size_t)conf;
	if(a->astruct.max > 0) {
		KARRAY_INIT(a->astruct, a->astruct.max, void*);
	}
	if(TY_isUnbox(O_p1(a))) {
		kArray_setUnboxData(a, 1);
	}
}

static void Array_reftrace(CTX, kRawPtr *o)
{
	kArray *a = (kArray*)o;
	if(!kArray_isUnboxData(a)) {
		size_t i;
		BEGIN_REFTRACE(a->size);
		for(i = 0; i < a->size; i++) {
			KREFTRACEv(a->list[i]);
		}
		END_REFTRACE();
	}
}

static void Array_free(CTX, kRawPtr *o)
{
	kArray_ *a = (kArray_*)o;
	if(a->astruct.max > 0) {
		KARRAY_FREE(a->astruct, void*);
	}
}

static KCLASSDEF ArrayDef = {
	CLASSNAME(Array),
	.init = Array_init,
	.reftrace = Array_reftrace,
	.free = Array_free,
};

static void Array_expand(CTX, kArray_ *a, size_t min)
{
	if(a->astruct.max == 0) {
		KARRAY_INIT(a->astruct, 8, void*);
	}
	else {
		KARRAY_EXPAND(a->astruct, min, void*);
	}
}

static void Array_add(CTX, kArray *a, kObject *value)
{
	if(a->size == a->capacity) {
		Array_expand(_ctx, (kArray_*)a, a->size + 1);
		//DBG_P("ARRAY EXPAND %d=>%d", a->size, a->capacity);
	}
	DBG_ASSERT(a->list[a->size] == NULL);
	KINITv(a->list[a->size], value);
	a->size++;
}

static void Array_insert(CTX, kArray *a, size_t n, kObject *v)
{
	if(!(n < a->size)) {
		Array_add(_ctx, a, v);
	}
	else {
		if(a->size == a->capacity) {
			Array_expand(_ctx, (kArray_*)a, a->size + 1);
		}
		memmove(a->list+(n+1), a->list+n, sizeof(kObject*) * (a->size - n));
		KINITv(a->list[n], v);
		a->size++;
	}
}

//KNHAPI2(void) kArray_remove_(CTX, kArray *a, size_t n)
//{
//	DBG_ASSERT(n < a->size);
//	if (kArray_isUnboxData(a)) {
//		knh_memmove(a->nlist+n, a->nlist+(n+1), sizeof(kunbox_t) * (a->size - n - 1));
//	} else {
//		KNH_FINALv(_ctx, a->list[n]);
//		knh_memmove(a->list+n, a->list+(n+1), sizeof(kObject*) * (a->size - n - 1));
//	}
//	a->size--;
//}
//
//KNHAPI2(void) kArray_swap(CTX, kArray *a, size_t n, size_t m)
//{
//	DBG_ASSERT(n < a->size);
//	DBG_ASSERT(m < a->size);
//#ifdef K_USING_RCGC
//	Object *temp = a->list[n];
//	a->list[n] = a->list[m];
//	a->list[m] = temp;
//#else
//	ksfp_t *esp1 = ctx->esp + 1;
//	esp1[0].o = a->list[n];
//	a->list[n] = a->list[m];
//	a->list[m] = esp1[0].o;
//#endif
//}
//
static void Array_clear(CTX, kArray *a, size_t n)
{
//	if(!kArray_isUnboxData(a)) {
	if(a->size > n) {
		bzero(a->list + n, sizeof(void*) * (a->size - n));
	}
	a->size = n;
}

// ---------------
// Param

static void Param_init(CTX, kRawPtr *o, void *conf)
{
	kParam *pa = (kParam*)o;
	pa->psize = 0;
	pa->rtype = TY_void;
}

static KCLASSDEF ParamDef = {
	CLASSNAME(Param),
	.init = Param_init,
};

static kParam *new_Param(CTX, ktype_t rtype, int psize, kparam_t *p)
{
	const kclass_t *ct = CT_(CLASS_Param);
	ct = CT_body(_ctx, ct, sizeof(void*), psize * sizeof(kparam_t));
	kParam *pa = (kParam*)new_Object(_ctx, ct, (void*)0);
	pa->rtype = rtype;
	pa->psize = psize;
	if(psize > 0) {
		memcpy(pa->p, p, sizeof(kparam_t) * psize);
	}
	return pa;
}

/* --------------- */
/* Method */

static void Method_init(CTX, kRawPtr *o, void *conf)
{
	kMethod *mtd = (kMethod*)o;
	kParam *pa = (conf == NULL) ? K_NULLPARAM : (kParam*)conf;
	KINITv(mtd->pa, pa);
	KINITv(mtd->tcode, (struct kToken*)K_NULL);
	KINITv(mtd->kcode, K_NULL);
//	mtd->paramsNULL = NULL;
}

static void Method_reftrace(CTX, kRawPtr *o)
{
	BEGIN_REFTRACE(3);
	kMethod *mtd = (kMethod*)o;
	KREFTRACEv(mtd->pa);
	KREFTRACEv(mtd->tcode);
	KREFTRACEv(mtd->kcode);
//	KREFTRACEn(mtd->paramsNULL);
	END_REFTRACE();
}

static KCLASSDEF MethodDef = {
	CLASSNAME(Method),
	.init = Method_init,
	.reftrace = Method_reftrace,
};


static kMethod* new_Method(CTX, uintptr_t flag, kcid_t cid, kmethodn_t mn, kParam *paN, knh_Fmethod func)
{
	kMethod* mtd = new_(Method, paN);
	mtd->flag  = flag;
	mtd->cid     = cid;
	mtd->mn      = mn;
	kMethod_setFunc(mtd, func);
	return mtd;
}

// ---------------

static KCLASSDEF TdynamicDef = {
	TYPENAME(dynamic),
	.init = DEFAULT_init,
};

// ---------------
// System

#define CT_System               CT_(CLASS_System)

static KCLASSDEF SystemDef = {
	CLASSNAME(System),
	.init = DEFAULT_init,
};

// ---------------

static KCLASSDEF *DATATYPES[] = {
	&TvoidDef,
	&TvarDef,
	&ObjectDef,
	&BooleanDef,
	&IntDef,
	&StringDef,
	&ArrayDef,
	&ParamDef,
	&MethodDef,
	&SystemDef,
	&TdynamicDef,
	NULL,
};

static void initStructData(CTX)
{
	kclass_t **ctt = (kclass_t**)_ctx->share->ca.ClassTBL;
	size_t i, size = _ctx->share->ca.size;
	for(i = 0; i < size; i++) {
		kclass_t *ct = ctt[i];
		const char *name = ct->DBG_NAME;
		kString *cname = new_kString(name, strlen(name), SPOL_ASCII|SPOL_POOL|SPOL_TEXT);
		CT_setName(_ctx, ct, cname, 0);
	}
}

static const kclass_t *addClassDef(CTX, kString *name, KCLASSDEF *cdef, kline_t pline)
{
	kclass_t *ct = new_CT(_ctx, cdef, pline);
	if(name == NULL) {
		const char *n = cdef->structname;
		assert(n != NULL); // structname must be set;
		name = new_kString(n, strlen(n), SPOL_ASCII|SPOL_POOL|SPOL_TEXT);
	}
	ct->packid   = cdef->packid;
	ct->packdom  = cdef->packdom;
	CT_setName(_ctx, ct, name, pline);
	return (const kclass_t*)ct;
}

static void kshare_initklib2(klib2_t *l)
{
	l->Kclass   = Kclass;
	l->Kuri     = Kuri;
	l->Ksymbol  = Ksymbol;
	l->Kusymbol = Kusymbol;
	l->Knew_Object = new_Object;
	l->KObject_getObjectNULL = Object_getObjectNULL;
	l->KObject_setObject = Object_setObject;
	l->Knew_String   = new_String;
	l->KArray_add    = Array_add;
	l->KArray_insert = Array_insert;
	l->KArray_clear  = Array_clear;
	l->Knew_Param    = new_Param;
	l->Knew_Method   = new_Method;
	l->KaddClassDef  = addClassDef;
	l->Knull = CT_null;
}

void kshare_init(CTX, kcontext_t *ctx)
{
	kshare_t *share = (kshare_t*)KNH_ZMALLOC(sizeof(kshare_t));
	ctx->share = share;
	kshare_initklib2(_ctx->lib2);
	KARRAY_INIT(share->ca, K_CLASSTABLE_INIT, kclass_t);
	share->classnameMapNN = kmap_init(0);
	KCLASSDEF **dd = DATATYPES;
	while(*dd != NULL) {
		new_CT(_ctx, *dd, 0);
		dd++;
	}
//	knh_ClassTBL_setConstPool(_ctx, ClassTBL(CLASS_Int));
//	knh_ClassTBL_setConstPool(_ctx, ClassTBL(CLASS_String));
	//
	KINITv(share->constNull, new_(Object, NULL));
	kObject_setNullObject(share->constNull, 1);
	KINITv(share->constTrue, new_(Boolean, 1));
	KINITv(share->constFalse, new_(Boolean, 0));
	KINITv(share->nullParam,  new_(Param, NULL));
	KINITv(share->emptyString, new_(String, NULL));
	KINITv(share->emptyArray, new_(Array, 0));
	initStructData(_ctx);

	KINITv(share->uriList, new_(Array, 8));
	share->uriMapNN = kmap_init(0);
	KINITv(share->pkgList, new_(Array, 8));
	share->pkgMapNN = kmap_init(0);
	KINITv(share->symbolList, new_(Array, 32));
	share->symbolMapNN = kmap_init(0);
	KINITv(share->usymbolList, new_(Array, 32));
	share->usymbolMapNN = kmap_init(0);
}

//static void key_reftrace(CTX, kmape_t *p)
//{
//	BEGIN_REFTRACE(1);
//	KREFTRACEv(p->skey);
//	END_REFTRACE();
//}

static void val_reftrace(CTX, kmape_t *p)
{
	BEGIN_REFTRACE(1);
	KREFTRACEv(p->ovalue);
	END_REFTRACE();
}

static void keyval_reftrace(CTX, kmape_t *p)
{
	BEGIN_REFTRACE(2);
	KREFTRACEv(p->skey);
	KREFTRACEv(p->ovalue);
	END_REFTRACE();
}

void kshare_reftrace(CTX, kcontext_t *ctx)
{
	kshare_t *share = ctx->share;

	kclass_t **ctt = (kclass_t**)_ctx->share->ca.ClassTBL;
	size_t i, size = _ctx->share->ca.size;
	for(i = 0; i < size; i++) {
		kclass_t *ct = ctt[i];
		{
			BEGIN_REFTRACE(6);
			KREFTRACEn(ct->typeNUL);
			KREFTRACEv(ct->cparam);
			KREFTRACEv(ct->name);
			KREFTRACEn(ct->fullnameNUL);
			KREFTRACEv(ct->methods);
			/* TODO(imasahiro) cls->defnull is nullable? */
			KREFTRACEn(ct->nulvalNUL);
			END_REFTRACE();
		}
		if (ct->constNameMapSO) kmap_reftrace(ct->constNameMapSO, keyval_reftrace);
		if (ct->constPoolMapNO) kmap_reftrace(ct->constPoolMapNO, val_reftrace);
	}
	//kmap_reftrace(share->symbolMapNN, key_reftrace);
	//kmap_reftrace(share->usymbolMapNN, key_reftrace);
	//kmap_reftrace(share->classnameMapNN, key_reftrace);
	//kmap_reftrace(share->uriMapNN, key_reftrace);
	//kmap_reftrace(share->pkgMapNN, key_reftrace);

	BEGIN_REFTRACE(10);
	KREFTRACEv(share->constNull);
	KREFTRACEv(share->constTrue);
	KREFTRACEv(share->constFalse);
	KREFTRACEv(share->emptyString);
	KREFTRACEv(share->emptyArray);
	KREFTRACEv(share->nullParam);
	KREFTRACEv(share->uriList);
	KREFTRACEv(share->pkgList);
	KREFTRACEv(share->symbolList);
	KREFTRACEv(share->usymbolList);
	END_REFTRACE();
}

static void kshare_freeCT(CTX)
{
	kclass_t **ct = (kclass_t**)_ctx->share->ca.ClassTBL;
	size_t i, size = _ctx->share->ca.size;
	for(i = 0; i < size; i++) {
//		const char *name = ct[i]->s->structname;
//		DBG_P("name='%s'", name);
		if(ct[i]->fallocsize > 0) {
			KNH_FREE(ct[i]->fields, ct[i]->fallocsize);
		}
		KNH_FREE(ct[i], sizeof(kclass_t));
	}
}

void kshare_free(CTX, kcontext_t *ctx)
{
	kshare_t *share = ctx->share;
	kmap_free(share->classnameMapNN, NULL);
	kmap_free(share->uriMapNN, NULL);
	kmap_free(share->pkgMapNN, NULL);
	kmap_free(share->symbolMapNN, NULL);
	kmap_free(share->usymbolMapNN, NULL);

	kshare_freeCT(_ctx);
	KARRAY_FREE(share->ca, kclass_t);
	KNH_FREE(share, sizeof(kshare_t));
}

/* operator */
#include "methods.h"

#define _Public kMethod_Public
#define _Const kMethod_Const
#define _F(F)   (intptr_t)(F)

void kshare_init_methods(CTX)
{
	int FN_x = FN_("x");
	intptr_t methoddata[] = {
		_Public, _F(Boolean_opNOT), TY_Boolean, TY_Boolean, MN_("opNOT"), 0,
		_Public, _F(Int_opADD), TY_Int, TY_Int, MN_("opADD"), 1, TY_Int, FN_x,
		_Public, _F(Int_opSUB), TY_Int, TY_Int, MN_("opSUB"), 1, TY_Int, FN_x,
		_Public, _F(Int_opMUL), TY_Int, TY_Int, MN_("opMUL"), 1, TY_Int, FN_x,
		_Public, _F(Int_opDIV), TY_Int, TY_Int, MN_("opDIV"), 1, TY_Int, FN_x,
		_Public, _F(Int_opMOD), TY_Int, TY_Int, MN_("opMOD"), 1, TY_Int, FN_x,
		_Public, _F(Int_opEQ),  TY_Boolean, TY_Int, MN_("opEQ"),  1, TY_Int, FN_x,
		_Public, _F(Int_opNEQ), TY_Boolean, TY_Int, MN_("opNEQ"), 1, TY_Int, FN_x,
		_Public, _F(Int_opLT),  TY_Boolean, TY_Int, MN_("opLT"),  1, TY_Int, FN_x,
		_Public, _F(Int_opLTE), TY_Boolean, TY_Int, MN_("opLTE"), 1, TY_Int, FN_x,
		_Public, _F(Int_opGT),  TY_Boolean, TY_Int, MN_("opGT"),  1, TY_Int, FN_x,
		_Public, _F(Int_opGTE), TY_Boolean, TY_Int, MN_("opGTE"), 1, TY_Int, FN_x,
		_Public|_Const, _F(Int_toString), TY_String, TY_Int, MN_to(TY_String), 0,
		_Public|_Const, _F(String_toInt), TY_Int, TY_String, MN_to(TY_Int), 0,
		_Public, _F(System_p), TY_void, TY_System, MN_("p"), 1, TY_String, FN_("s") | FN_COERCION,
		DEND,
	};
	kloadMethodData(NULL, methoddata);
}

#ifdef __cplusplus
}
#endif
