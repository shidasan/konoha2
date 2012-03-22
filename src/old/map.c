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

/* ------------------------------------------------------------------------ */
/* [HashMap] */

#define K_HASH_INITSIZE 83

typedef struct knh_kmape_t {
	kuint_t hcode;
	struct knh_kmape_t *next;
	union {
		Object       *key;
		kString *skey;
		kunbox_t   nkey;
		void         *pkey;
	};
	union {
		Object       *value;
		void         *pvalue;
		kunbox_t   nvalue;
	};
} knh_kmape_t;

typedef struct knh_kmap_t {
	knh_kmape_t *arena;
	knh_kmape_t *unused;
	knh_kmape_t **hentry;
	size_t arenasize;
	size_t size;
	size_t hmax;
	size_t stat_total;
	size_t stat_hit;
} knh_kmap_t;

static void kmap_makeFreeList(knh_kmap_t *kmap, size_t s, size_t e)
{
	DBG_ASSERT(kmap->unused == NULL);
	knh_bzero(kmap->arena + s, (e - s) * sizeof(knh_kmape_t));
	kmap->unused = kmap->arena + s;
	size_t i;
	for(i = s; i < e - 1; i++) {
		kmap->arena[i].hcode = ((kuint_t)-1);
		kmap->arena[i].nvalue = 0;
		kmap->arena[i].next = kmap->arena + i + 1;
	}
	kmap->arena[e-1].hcode = ((kuint_t)-1);
	kmap->arena[e-1].nvalue = 0;
	DBG_ASSERT(kmap->arena[e-1].next == NULL);
}

static void kmap_rehash(CTX, knh_kmap_t *kmap)
{
	size_t i, newhmax = kmap->hmax * 2 + 1;
	knh_kmape_t **newhentry = (knh_kmape_t**)KNH_ZMALLOC(newhmax * sizeof(knh_kmape_t*));
	knh_bzero(newhentry, newhmax * sizeof(knh_kmape_t*));
	for(i = 0; i < kmap->arenasize / 2; i++) {
		knh_kmape_t *e = kmap->arena + i;
		kuint_t ni = e->hcode % newhmax;
		e->next = newhentry[ni];
		newhentry[ni] = e;
	}
	KNH_FREE(_ctx, kmap->hentry, kmap->hmax * sizeof(knh_kmape_t*));
	kmap->hentry = newhentry;
	kmap->hmax = newhmax;
}

static void kmap_shiftptr(knh_kmap_t *kmap, intptr_t shift)
{
	size_t i, size = kmap->arenasize / 2;
	for(i = 0; i < size; i++) {
		knh_kmape_t *e = kmap->arena + i;
		if(e->next != NULL) {
			e->next = (knh_kmape_t*)(((char*)e->next) + shift);
			DBG_ASSERT(kmap->arena <= e->next && e->next < kmap->arena + size);
		}
	}
}

static knh_kmape_t *new_kmape(CTX, knh_kmap_t *kmap, kuint_t hcode)
{
	knh_kmape_t *e;
	if(kmap->unused == NULL) {
		size_t oarenasize = kmap->arenasize;
		char *oarena = (char*)kmap->arena;
		kmap->arenasize *= 2;
		kmap->arena = (knh_kmape_t*)KNH_REALLOC(_ctx, path, kmap->arena, oarenasize, kmap->arenasize, sizeof(knh_kmape_t));
		DBG_P("extend arena: %p %p size=%d", oarena, kmap->arena, kmap->arenasize);
		if((void*)kmap->arena != oarena) {
			kmap_shiftptr(kmap, (char*)kmap->arena - oarena);
		}
		kmap_makeFreeList(kmap, oarenasize, kmap->arenasize);
		kmap_rehash(_ctx, kmap);
	}
	e = kmap->unused;
	kmap->unused = e->next;
	e->hcode = hcode;
	e->next = NULL;
	kmap->size++;
	return e;
}

static void kmap_unuse(knh_kmap_t *kmap, knh_kmape_t *e)
{
	e->next = kmap->unused;
	kmap->unused = e;
	e->hcode = ((kuint_t)-1);
	e->nvalue  = 0;
	kmap->size--;
}

static kmapptr_t *kmap_init(CTX, size_t init, const char *path, struct kDictMap *opt)
{
	knh_kmap_t *kmap = (knh_kmap_t*)KNH_ZMALLOC(sizeof(knh_kmap_t));
	knh_bzero(kmap, sizeof(knh_kmap_t));
	if(init < K_HASH_INITSIZE) init = K_HASH_INITSIZE;
	kmap->arenasize = (init * 3) / 4;
	kmap->arena = (knh_kmape_t*)KNH_ZMALLOC(kmap->arenasize * sizeof(knh_kmape_t));
	kmap_makeFreeList(kmap, 0, kmap->arenasize);
	kmap->hentry = (knh_kmape_t**)KNH_ZMALLOC(init * sizeof(knh_kmape_t*));
	knh_bzero(kmap->hentry, init * sizeof(knh_kmape_t*));
	kmap->hmax = init;
	kmap->size = 0;
	return (kmapptr_t*)kmap;
}

static void kmapSO_reftrace(CTX, kmapptr_t *m)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	size_t i;
	kref_ensureSize(kmap->size * 2);
	for(i = 0; i < kmap->hmax; i++) {
		knh_kmape_t *e = kmap->hentry[i];
		while(e != NULL) {
			KREFTRACEv(e->key);
			KREFTRACEv(e->value);
			e = e->next;
		}
	}
	kref_setSize();
}

static void kmapNO_reftrace(CTX, kmapptr_t *m)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	size_t i;
	kref_ensureSize(kmap->size);
	for(i = 0; i < kmap->hmax; i++) {
		knh_kmape_t *e = kmap->hentry[i];
		while(e != NULL) {
			KREFTRACEv(e->value);
			e = e->next;
		}
	}
	kref_setSize();
}

static void kmap_reftraceON(CTX, kmapptr_t *m)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	size_t i;
	kref_ensureSize(kmap->size);
	for(i = 0; i < kmap->hmax; i++) {
		knh_kmape_t *e = kmap->hentry[i];
		while(e != NULL) {
			KREFTRACEv(e->key);
			e = e->next;
		}
	}
	kref_setSize();
}

static void kmapNN_reftrace(CTX, kmapptr_t *m)
{
	kref_setSize();
}

static void kmap_free(CTX, kmapptr_t *m)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	KNH_FREE(_ctx, kmap->arena, sizeof(knh_kmape_t)*(kmap->arenasize));
	KNH_FREE(_ctx, kmap->hentry, sizeof(knh_kmape_t*)*(kmap->hmax));
	KNH_FREE(_ctx, kmap, sizeof(knh_kmap_t));
}

static knh_kmape_t *kmap_getentry(knh_kmap_t* kmap, kuint_t hcode)
{
	knh_kmape_t **hlist = kmap->hentry;
	size_t idx = hcode % kmap->hmax;
	knh_kmape_t *e = hlist[idx];
	while(e != NULL) {
		if(e->hcode == hcode) return e;
		e = e->next;
	}
	return NULL;
}

static void kmap_add(knh_kmap_t* kmap, knh_kmape_t *ne)
{
	DBG_ASSERT(ne->next == NULL);
	knh_kmape_t **hlist = kmap->hentry;
	size_t idx = ne->hcode % kmap->hmax;
	ne->next = hlist[idx];
	hlist[idx] = ne;
}

static void kmap_remove(knh_kmap_t* kmap, knh_kmape_t *oe)
{
	knh_kmape_t **hlist = kmap->hentry;
	size_t idx = oe->hcode % kmap->hmax;
	knh_kmape_t *e = hlist[idx];
	while(e != NULL) {
		if(e->next == oe) {
			e->next = oe->next;
			return;
		}
		e = e->next;
	}
	hlist[idx] = oe->next;
}

static void kmap_top(knh_kmap_t* kmap, knh_kmape_t *oe)
{
	DBG_ASSERT(oe->next == NULL);
	knh_kmape_t **hlist = kmap->hentry;
	size_t idx = oe->hcode % kmap->hmax;
	if(hlist[idx] != oe) {
		kmap_remove(kmap, oe);
		oe->next = hlist[idx];
		hlist[idx] = oe;
	}
}

static kbool_t kmap_getOO(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kObject *key = ksfp[0].o;
	kuint_t hcode = O_ct(key)->cdef->hashCode(_ctx, ksfp[0].p);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_Object_compareTo(key, e->key) == 0) {
			KSETv(rsfp[0].o, e->value);
			return 1;
		}
		e = e->next;
	}
	return 0;
}

static kbool_t kmap_getON(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kObject *key = ksfp[0].o;
	kuint_t hcode = O_ct(key)->cdef->hashCode(_ctx, ksfp[0].p);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_Object_compareTo(key, e->key) == 0) {
			rsfp[0].ndata = e->nvalue;
			return 1;
		}
		e = e->next;
	}
	return 0;
}

#define knh_String_equals(STR, t)   (knh_bytes_equals(S_tobytes(STR), t))

static kbool_t kmap_getSO(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp)
{
	DBG_ASSERT(IS_bString(ksfp[0].s));
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kbytes_t k = S_tobytes(ksfp[0].s);
	kuint_t hcode = knh_hash(0, k.text, k.len);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_String_equals(e->skey, k)) {
			KSETv(rsfp[0].o, e->value);
			return 1;
		}
		e = e->next;
	}
	return 0;
}

static kbool_t kmap_getSN(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp)
{
	DBG_ASSERT(IS_bString(ksfp[0].s));
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kbytes_t k = S_tobytes(ksfp[0].s);
	kuint_t hcode = knh_hash(0, k.text, k.len);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_String_equals(e->skey, k)) {
			rsfp[0].ndata = e->nvalue;
			return 1;
		}
		e = e->next;
	}
	return 0;
}

static kbool_t kmap_getNO(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kuint_t hcode = (kuint_t)ksfp[0].ndata;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode) {
			KSETv(rsfp[0].o, e->value);
			return 1;
		}
		e = e->next;
	}
	return 0;
}

static kbool_t kmap_getNN(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kuint_t hcode = (kuint_t)ksfp[0].ndata;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode) {
			rsfp[0].ndata = e->nvalue;
			return 1;
		}
		e = e->next;
	}
	return 0;
}

static void kmap_setOO(CTX, kmapptr_t* m, ksfp_t *kvsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kObject *key = kvsfp[0].o;
	kuint_t hcode = O_ct(key)->cdef->hashCode(_ctx, kvsfp[0].p);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_Object_compareTo(key, e->key) == 0) {
			KSETv(e->value, kvsfp[1].o);
			return;
		}
		e = e->next;
	}
	e = new_kmape(_ctx, kmap, hcode);
	KINITv(e->key, kvsfp[0].o);
	KINITv(e->value, kvsfp[1].o);
	kmap_add(kmap, e);
}

static void kmap_setON(CTX, kmapptr_t* m, ksfp_t *kvsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kObject *key = kvsfp[0].o;
	kuint_t hcode = O_ct(key)->cdef->hashCode(_ctx, kvsfp[0].p);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_Object_compareTo(key, e->key) == 0) {
			e->nvalue = kvsfp[0].ndata;
			return;
		}
		e = e->next;
	}
	e = new_kmape(_ctx, kmap, hcode);
	KINITv(e->key, kvsfp[0].o);
	e->nvalue = kvsfp[1].ndata;
	kmap_add(kmap, e);
}

static void kmap_setSO(CTX, kmapptr_t* m, ksfp_t *kvsfp)
{
	DBG_ASSERT(IS_bString(kvsfp[0].s));
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kbytes_t k = S_tobytes(kvsfp[0].s);
	kuint_t hcode = knh_hash(0, k.text, k.len);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_String_equals(e->skey, k)) {
			KSETv(e->value, kvsfp[1].o);
			return;
		}
		e = e->next;
	}
	e = new_kmape(_ctx, kmap, hcode);
	KINITv(e->key, kvsfp[0].o);
	KINITv(e->value, kvsfp[1].o);
	kmap_add(kmap, e);
}

static void kmap_setSN(CTX, kmapptr_t* m, ksfp_t *kvsfp)
{
	DBG_ASSERT(IS_bString(kvsfp[0].s));
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kbytes_t k = S_tobytes(kvsfp[0].s);
	kuint_t hcode = knh_hash(0, k.text, k.len);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_String_equals(e->skey, k)) {
			e->nvalue = kvsfp[0].ndata;
			return;
		}
		e = e->next;
	}
	e = new_kmape(_ctx, kmap, hcode);
	KINITv(e->key, kvsfp[0].o);
	e->nvalue = kvsfp[1].ndata;
	kmap_add(kmap, e);
}

static void kmap_setNO(CTX, kmapptr_t* m, ksfp_t *kvsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kuint_t hcode = (kuint_t)kvsfp[0].ndata;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode) {
			KSETv(e->value, kvsfp[1].o);
			return;
		}
		e = e->next;
	}
	e = new_kmape(_ctx, kmap, hcode);
	KINITv(e->key, kvsfp[0].o);
	KINITv(e->value, kvsfp[1].o);
	kmap_add(kmap, e);
}

static void kmap_setNN(CTX, kmapptr_t* m, ksfp_t *kvsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kuint_t hcode = (kuint_t)kvsfp[0].ndata;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode) {
			e->nvalue = kvsfp[0].ndata;
			return;
		}
		e = e->next;
	}
	e = new_kmape(_ctx, kmap, hcode);
	KINITv(e->key, kvsfp[0].o);
	e->nvalue = kvsfp[1].ndata;
	kmap_add(kmap, e);
}

static void kmap_removeOO(CTX, kmapptr_t* m, ksfp_t *ksfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kObject *key = ksfp[0].o;
	kuint_t hcode = O_ct(key)->cdef->hashCode(_ctx, ksfp[0].p);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_Object_compareTo(key, e->key) == 0) {
			KNH_FINALv(_ctx, e->key);
			KNH_FINALv(_ctx, e->value);
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
}

static void kmap_removeON(CTX, kmapptr_t* m, ksfp_t *ksfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kObject *key = ksfp[0].o;
	kuint_t hcode = O_ct(key)->cdef->hashCode(_ctx, ksfp[0].p);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_Object_compareTo(key, e->key) == 0) {
			KNH_FINALv(_ctx, e->key);
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
}

#define knh_String_equals(STR, t)   (knh_bytes_equals(S_tobytes(STR), t))

static void kmap_removeSO(CTX, kmapptr_t* m, ksfp_t *ksfp)
{
	DBG_ASSERT(IS_bString(ksfp[0].s));
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kbytes_t k = S_tobytes(ksfp[0].s);
	kuint_t hcode = knh_hash(0, k.text, k.len);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_String_equals(e->skey, k)) {
			KNH_FINALv(_ctx, e->key);
			KNH_FINALv(_ctx, e->value);
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
}

static void kmap_removeSN(CTX, kmapptr_t* m, ksfp_t *ksfp)
{
	DBG_ASSERT(IS_bString(ksfp[0].s));
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kbytes_t k = S_tobytes(ksfp[0].s);
	kuint_t hcode = knh_hash(0, k.text, k.len);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && knh_String_equals(e->skey, k)) {
			KNH_FINALv(_ctx, e->key);
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
}

static void kmap_removeNO(CTX, kmapptr_t* m, ksfp_t *ksfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kuint_t hcode = (kuint_t)ksfp[0].ndata;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode) {
			KNH_FINALv(_ctx, e->value);
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
}

static void kmap_removeNN(CTX, kmapptr_t* m, ksfp_t *ksfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	kuint_t hcode = (kuint_t)ksfp[0].ndata;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode) {
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
}

static size_t kmap_size(CTX, kmapptr_t* m)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	return kmap->size;
}

static kbool_t kmap_nextOO(CTX, kmapptr_t *m, knitr_t *mitr, ksfp_t *rsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	size_t i;
	for(i = mitr->index; i < kmap->arenasize; i++) {
		knh_kmape_t *e = kmap->arena + i;
		if(e->hcode == ((kuint_t)-1) && e->nvalue == 0) continue;
		KSETv(rsfp[0].o, e->key);
		KSETv(rsfp[1].o, e->value);
		mitr->index = i + 1;
		return 1;
	}
	mitr->index = kmap->arenasize;
	return 0;
}

static kbool_t kmap_nextON(CTX, kmapptr_t *m, knitr_t *mitr, ksfp_t *rsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	size_t i;
	for(i = mitr->index; i < kmap->arenasize; i++) {
		knh_kmape_t *e = kmap->arena + i;
		if(e->hcode == ((kuint_t)-1) && e->nvalue == 0) continue;
		KSETv(rsfp[0].o, e->key);
		rsfp[1].ndata = e->nvalue;
		mitr->index = i + 1;
		return 1;
	}
	mitr->index = kmap->arenasize;
	return 0;
}

static kbool_t kmap_nextNO(CTX, kmapptr_t *m, knitr_t *mitr, ksfp_t *rsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	size_t i;
	for(i = mitr->index; i < kmap->arenasize; i++) {
		knh_kmape_t *e = kmap->arena + i;
		if(e->hcode == ((kuint_t)-1) && e->nvalue == 0) continue;
		rsfp[0].ndata = e->nkey;
		KSETv(rsfp[1].o, e->value);
		mitr->index = i + 1;
		return 1;
	}
	mitr->index = kmap->arenasize;
	return 0;
}

static kbool_t kmap_nextNN(CTX, kmapptr_t *m, knitr_t *mitr, ksfp_t *rsfp)
{
	knh_kmap_t *kmap = (knh_kmap_t*)m;
	size_t i;
	for(i = mitr->index; i < kmap->arenasize; i++) {
		knh_kmape_t *e = kmap->arena + i;
		if(e->hcode == ((kuint_t)-1) && e->nvalue == 0) continue;
		rsfp[0].ndata = e->nkey;
		rsfp[1].ndata = e->nvalue;
		mitr->index = i + 1;
		return 1;
	}
	mitr->index = kmap->arenasize;
	return 0;
}

static const knh_MapDPI_t* kmap_config(CTX, kcid_t p1, kcid_t p2);

static const knh_MapDPI_t HMAP_OO = {
	K_DSPI_MAP, "hash",
	kmap_config, kmap_init, kmapSO_reftrace, kmap_free,
	kmap_getOO, kmap_setOO, kmap_removeOO, kmap_size, kmap_nextOO,
};
static const knh_MapDPI_t HMAP_ON = {
	K_DSPI_MAP, "hash",
	kmap_config, kmap_init, kmap_reftraceON, kmap_free,
	kmap_getON, kmap_setON, kmap_removeON, kmap_size, kmap_nextON,
};
static const knh_MapDPI_t HMAP_SO = {
	K_DSPI_MAP, "hash",
	kmap_config, kmap_init, kmapSO_reftrace, kmap_free,
	kmap_getSO, kmap_setSO, kmap_removeSO, kmap_size, kmap_nextOO,
};
static const knh_MapDPI_t HMAP_SN = {
	K_DSPI_MAP, "hash",
	kmap_config, kmap_init, kmap_reftraceON, kmap_free,
	kmap_getSN, kmap_setSN, kmap_removeSN, kmap_size, kmap_nextON,
};
static const knh_MapDPI_t HMAP_NO = {
	K_DSPI_MAP, "hash",
	kmap_config, kmap_init, kmapNO_reftrace, kmap_free,
	kmap_getNO, kmap_setNO, kmap_removeNO, kmap_size, kmap_nextNO,
};
static const knh_MapDPI_t HMAP_NN = {
	K_DSPI_MAP, "hash",
	kmap_config, kmap_init, kmapNN_reftrace, kmap_free,
	kmap_getNN, kmap_setNN, kmap_removeNN, kmap_size, kmap_nextNN,
};

static const knh_MapDPI_t* kmap_config(CTX, kcid_t p1, kcid_t p2)
{
	if(TY_isUnbox(p2)) {
		if(IS_Tstr(p1)) {
			return &HMAP_SN;
		}
		else if(TY_isUnbox(p1)) {
			return &HMAP_NN;
		}
		return &HMAP_ON;
	}
	else {
		if(IS_Tstr(p1)) {
			return &HMAP_SO;
		}
		else if(TY_isUnbox(p1)) {
			return &HMAP_NO;
		}
		return &HMAP_OO;
	}
}

kPtrMap* new_PtrMap(CTX, size_t max)
{
	kMap *m = new_H(Map);
	m->spi = &HMAP_NN;
	m->mapptr = m->spi->init(_ctx, max, NULL, NULL);
	return (kPtrMap*)m;
}

void* knh_PtrMap_get(CTX, kPtrMap *pm, void *keyptr)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	kuint_t hcode = (kuint_t)keyptr;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	if(e != NULL) {
		kmap_top(kmap, e);
		return e->pvalue;
	}
	return NULL;
}

void knh_PtrMap_add(CTX, kPtrMap *pm, void *keyptr, void *valueptr)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	kuint_t hcode = (kuint_t)keyptr;
	knh_kmape_t *e = new_kmape(_ctx, kmap, hcode);
	e->pvalue = valueptr;
	kmap_add(kmap, e);
}

void knh_PtrMap_rm(CTX, kPtrMap *pm, void *keyptr)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	kuint_t hcode = (kuint_t)keyptr;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	DBG_ASSERT(e != NULL);
	kmap_remove(kmap, e);
	kmap_unuse(kmap, e);
}

size_t knh_PtrMap_size(kPtrMap *pm)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	return kmap->size;
}

void knh_PtrMap_stat(CTX, kPtrMap *pm, const char *name)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	if(kmap->stat_total > 9) {
		knh_logprintf("STAT", knh_isVerbosePref(), "size=%lu, name=%s count=%d %f%%", kmap->size, name, kmap->stat_total, 100.0 * kmap->stat_hit / kmap->stat_total);
//		knh_ldata_t ldata[] = {LOG_s("name", name), /*fDATA("rate", kmap->stat_hit / kmap->stat_total),*/ LOG_i("count", kmap->stat_total), LOG_END};
//		LIB_STAT("konoha.PtrMap");
	}
}

kString* knh_PtrMap_getS(CTX, kPtrMap *pm, const char *k, size_t len)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	kuint_t hcode = knh_hash(0, k, len);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	kmap->stat_total++;
	while(e != NULL) {
		const char *es = (const char*)e->pkey;
		if(e->hcode == hcode && es[len] == 0 && strncmp(k, es, len) == 0) {
			kmap->stat_hit++;
			return (kString*)e->pvalue;
		}
		e = e->next;
	}
	return NULL;
}

void knh_PtrMap_addS(CTX, kPtrMap *pm, kString *v)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	const char *k = S_totext(v);
	size_t len = S_size(v);
	kuint_t hcode = knh_hash(0, k, len);
	knh_kmape_t *e = new_kmape(_ctx, kmap, hcode);
	DBG_ASSERT(IS_bString(v));
	e->pkey = (void*)k;
	e->pvalue = (void*)v;
	kmap_add(kmap, e);
}

void knh_PtrMap_rmS(CTX, kPtrMap *pm, kString *s)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	kbytes_t t = S_tobytes(s);
	kuint_t hcode = knh_hash(0, t.text, t.len);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	DBG_ASSERT(IS_bString(s));
	while(e != NULL) {
		if(e->hcode == hcode && e->pvalue == (void*)s) {
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
	DBG_P("not found removed %x '%s' %p", hcode, t.text, s);
	//KNH_ASSERT(_ctx == NULL);
}

kInt* knh_PtrMap_getI(CTX, kPtrMap *pm, kunbox_t k)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	kuint_t hcode = (kuint_t)k;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	kmap->stat_total++;
	while(e != NULL) {
		if(e->hcode == hcode DBLNDATA_(&& e->nkey == k)) {
			kmap->stat_hit++;
			return (kInt*)e->pvalue;
		}
		e = e->next;
	}
	return NULL;
}

void knh_PtrMap_addI(CTX, kPtrMap *pm, kInt *v)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	kunbox_t k = v->n.data;
	kuint_t hcode = (kuint_t)k;
	knh_kmape_t *e = new_kmape(_ctx, kmap, hcode);
	e->nkey = k;
	e->pvalue = (void*)v;
	kmap_add(kmap, e);
}

void knh_PtrMap_rmI(CTX, kPtrMap *pm, kInt *v)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	knh_kmape_t *e = kmap_getentry(kmap, (kuint_t)v->n.data);
	while(e != NULL) {
		if(e->pvalue == (void*)v) {
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
	DBG_P("not found removed %d %p", (kuint_t)v->n.data, v);
	//KNH_ASSERT(_ctx == NULL);
}

kMethod* knh_PtrMap_getM(CTX, kPtrMap *pm, kuint_t hcode)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	kmap->stat_total++;
	while(e != NULL) {
		if(e->hcode == hcode) {
			kmap->stat_hit++;
			return (kMethod*)e->pvalue;
		}
		e = e->next;
	}
	return NULL;
}

void knh_PtrMap_addM(CTX, kPtrMap *pm, kuint_t hcode, kMethod *v)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	knh_kmape_t *e = new_kmape(_ctx, kmap, hcode);
	DBG_ASSERT(IS_bMethod(v));
	e->pvalue = (void*)v;
	kmap_add(kmap, e);
}

void knh_PtrMap_rmM(CTX, kPtrMap *pm, kMethod *mtd)
{
	knh_kmap_t *kmap = (knh_kmap_t*)pm->mapptr;
	kuint_t hcode = hashcode_mtd(mtd->cid, mtd->mn, K_MTDCACHE_SIZE);
	knh_kmape_t *e = kmap_getentry(kmap, hcode);
	DBG_ASSERT(IS_bMethod(mtd));
	while(e != NULL) {
		if(e->hcode == hcode && e->pvalue == (void*)mtd) {
			kmap_remove(kmap, e);
			kmap_unuse(kmap, e);
			return;
		}
		e = e->next;
	}
	DBG_P("not found removed %x %d.%d", hcode, mtd->cid, mtd->mn);
	//KNH_ASSERT(_ctx == NULL);
}

/* ------------------------------------------------------------------------ */
/* DictMap */

#define K_USE_FASTDMAP(STMT)  STMT

typedef struct knh_dentry_t {
K_USE_FASTDMAP(uint64_t ukey;)
	union {
		kString  *key;
		intptr_t   ikey;
		kfloatptr_t fkey;
		kunbox_t    nkey;
	};
	union {
		Object         *value;
		kunbox_t    nvalue;
	};
} knh_dentry_t;

typedef struct knh_dmap_t {
	knh_dentry_t *dentry;
	size_t size;
	size_t capacity;
K_USE_FASTDMAP(uint64_t (*strkeyuint)(kbytes_t);)
	int (*dentrycmpr)(const void *, const void *);
	int (*strcmpr)(kbytes_t, kbytes_t);
	size_t sorted;
	const char *DBGNAME;
} knh_dmap_t ;

#define UNSORTED 8
#define knh_map_dmap(m)        ((knh_dmap_t*)m)
#define knh_map_dentry(m)      (((knh_dmap_t*)m)->dentry)

static uint64_t knh_struint64(kbytes_t t)
{
	const unsigned char *p = (const unsigned char*)t.text;
	uint64_t n = 0;
	if(t.len > 0) {
		switch(t.len) {
		default: n |= (((uint64_t)p[7]) << (64-64)); /* FALLTHROUGH */
		case 7 : n |= (((uint64_t)p[6]) << (64-56)); /* FALLTHROUGH */
		case 6 : n |= (((uint64_t)p[5]) << (64-48)); /* FALLTHROUGH */
		case 5 : n |= (((uint64_t)p[4]) << (64-40)); /* FALLTHROUGH */
		case 4 : n |= (((uint64_t)p[3]) << (64-32)); /* FALLTHROUGH */
		case 3 : n |= (((uint64_t)p[2]) << (64-24)); /* FALLTHROUGH */
		case 2 : n |= (((uint64_t)p[1]) << (64-16)); /* FALLTHROUGH */
		case 1 : n |= (((uint64_t)p[0]) << (64- 8));
		}
	}
	return n;
}

static int dentry_strcmp(const void *p, const void *p2)
{
	knh_dentry_t *e = (knh_dentry_t*)p;
	knh_dentry_t *e2 = (knh_dentry_t*)p2;
	K_USE_FASTDMAP(if(e->ukey == e2->ukey))
		return knh_bytes_strcmp(S_tobytes(e->key), S_tobytes(e2->key));
	K_USE_FASTDMAP(return (e->ukey < e2->ukey) ? -1 : 1;)
}

static kmapptr_t *dmap_init(CTX, size_t init, const char *path, struct kDictMap *opt)
{
	knh_dmap_t *dmap = (knh_dmap_t*)KNH_ZMALLOC(sizeof(knh_dmap_t));
	if(init < K_HASH_INITSIZE) init = 4;
	dmap->dentry = (knh_dentry_t*)KNH_REALLOC(_ctx, NULL, NULL, 0, init, sizeof(knh_dentry_t));
	dmap->capacity = init;
	dmap->size = 0;
	dmap->sorted = 0;
	dmap->strcmpr = knh_bytes_strcmp;
	K_USE_FASTDMAP(dmap->strkeyuint = knh_struint64;)
	dmap->dentrycmpr = dentry_strcmp;
	return dmap;
}

static void dmap_reftraceOO(CTX, kmapptr_t *m)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	knh_dentry_t *dentry = knh_map_dentry(m);
	size_t i;
	kref_ensureSize(dmap->size * 2);
	for(i = 0; i < dmap->size; i++) {
		KREFTRACEv(dentry[i].key);
		KREFTRACEv(dentry[i].value);
	}
	kref_setSize();
}

static void dmap_free(CTX, kmapptr_t *m)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	//DBG_P("DBGNAME=%s", dmap->DBGNAME);
	KNH_FREE(_ctx, dmap->dentry, sizeof(knh_dentry_t)*dmap->capacity);
	KNH_FREE(_ctx, dmap, sizeof(knh_dmap_t));
}

static size_t dmap_size(CTX, kmapptr_t* m)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	return dmap->size;
}

static kbool_t dmap_nextOO(CTX, kmapptr_t *m, knitr_t* mitr, ksfp_t *rsfp)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	if(mitr->index < dmap->size) {
		knh_dentry_t *dentry = knh_map_dentry(m);
		KSETv(rsfp[0].o, dentry[mitr->index].key);
		KSETv(rsfp[1].o, dentry[mitr->index].value);
		mitr->index += 1;
		return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */
/* String */

static kindex_t dmap_index(knh_dmap_t *dmap, size_t sp, size_t ep, kbytes_t key)
{
	knh_dentry_t *a = dmap->dentry;
	uint64_t ukey = dmap->strkeyuint(key);
	L_TAIL:;
	if(ep - sp < UNSORTED) {
		size_t i;
		for(i = sp; i < ep; i++) {
			if(a[i].ukey == ukey) {
				kbytes_t k = S_tobytes(a[i].key);
				if(key.len < 8) {
					//DBG_ASSERT(key.len == k.len);
					return i;
				}
				if(dmap->strcmpr(k, key) == 0) return i;
			}
		}
		return -1;
	}
	else {
		size_t cp = KNH_MID(sp, ep);
		if(a[cp].ukey < ukey) {
			sp = cp + 1;
		}
		else if(a[cp].ukey > ukey) {
			ep = cp;
		}
		else {
			kbytes_t k = S_tobytes(a[cp].key);
			int res = dmap->strcmpr(k, key);
			if(res == 0) {return cp; }
			else if(res > 0) { ep = cp; }
			else { sp = cp + 1; }
		}
		goto L_TAIL;
	}
}

static kbool_t dmap_getSO(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	kbytes_t key = S_tobytes(ksfp[0].s);
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
		if(loc == -1) return 0;
	}
	KSETv(rsfp[0].o, dmap->dentry[loc].value);
	return 1;
}

#define dmap_grow(_ctx, dmap) {\
		if(!(dmap->size < dmap->capacity)) {\
			size_t newsize = k_grow(dmap->capacity);\
			dmap->dentry = (knh_dentry_t*)KNH_REALLOC(_ctx, dmap->DBGNAME, dmap->dentry, dmap->capacity, newsize, sizeof(knh_dentry_t));\
			dmap->capacity = newsize;\
		}\
	}\

#define dmap_sort(dmap) \
	if(!((dmap->size - dmap->sorted) < UNSORTED)) {\
		/*DBG_P("SORTED %s sorted=%d, size=%d", dmap->DBGNAME, dmap->sorted, dmap->size); */\
		knh_qsort(dmap->dentry, dmap->size, sizeof(knh_dentry_t), dmap->dentrycmpr);\
		dmap->sorted = dmap->size;\
	}\


//static void dmap_sort_(knh_dmap_t *dmap, int isforced)
//{
//	if(isforced || dmap->size != dmap->sorted) {
//		//DBG_P("SORTED* %s, sorted=%d, size=%d", dmap->DBGNAME, dmap->sorted, dmap->size);
//		knh_qsort(dmap->dentry, dmap->size, sizeof(knh_dentry_t), dmap->dentrycmpr);
//		dmap->sorted = dmap->size;
//	}
//}

static void dmap_addSO(CTX, knh_dmap_t *dmap, ksfp_t *kvsfp)
{
	size_t loc = dmap->size;
	dmap_grow(_ctx, dmap);
	KINITv(dmap->dentry[loc].key, kvsfp[0].s);
	KINITv(dmap->dentry[loc].value, kvsfp[1].o);
	K_USE_FASTDMAP(dmap->dentry[loc].ukey = dmap->strkeyuint(S_tobytes(kvsfp[0].s)));
	dmap->size++;
}

static void dmap_setSO(CTX, kmapptr_t* m, ksfp_t* kvsfp)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	kbytes_t key = S_tobytes(kvsfp[0].s);
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
		if(loc == -1) {
			dmap_addSO(_ctx, dmap, kvsfp);
			dmap_sort(dmap);
			return;
		}
	}
	KSETv(dmap->dentry[loc].value, kvsfp[1].o);
}

static void dmap_removeSO(CTX, kmapptr_t* m, ksfp_t *kvsfp)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	kbytes_t key = S_tobytes(kvsfp[0].s);
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
		if(loc == -1) {
			return;
		}
	}
	KNH_FINALv(_ctx, dmap->dentry[loc].key);
	KNH_FINALv(_ctx, dmap->dentry[loc].value);
	if((size_t)loc < dmap->sorted) {
		dmap->sorted -= 1;
	}
	memmove(dmap->dentry + loc, dmap->dentry + loc + 1, (dmap->size - loc - 1)*sizeof(knh_dentry_t));
	dmap->size--;
}

static const knh_MapDPI_t* dmap_config(CTX, kcid_t p1, kcid_t p2);

static const knh_MapDPI_t DMAP_SO = {
	K_DSPI_MAP, "dictionary",
	dmap_config, dmap_init, dmap_reftraceOO, dmap_free,
	dmap_getSO, dmap_setSO, dmap_removeSO, dmap_size, dmap_nextOO,
};

static void dmap_reftraceON(CTX, kmapptr_t *m)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	knh_dentry_t *dentry = knh_map_dentry(m);
	size_t i;
	kref_ensureSize(dmap->size);
	for(i = 0; i < dmap->size; i++) {
		KREFTRACEv(dentry[i].key);
	}
	kref_setSize();
}

static kbool_t dmap_nextON(CTX, kmapptr_t *m, knitr_t* mitr, ksfp_t *rsfp)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	if(mitr->index < dmap->size) {
		knh_dentry_t *dentry = knh_map_dentry(m);
		KSETv(rsfp[0].o, dentry[mitr->index].key);
		rsfp[1].ndata = dentry[mitr->index].nvalue; /* thanks, ide */
		mitr->index += 1;
		return 1;
	}
	return 0;
}

static kbool_t dmap_getSN(CTX, kmapptr_t* m, ksfp_t *ksfp, ksfp_t *rsfp)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	kbytes_t key = S_tobytes(ksfp[0].s);
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
		if(loc == -1) return 0;
	}
	rsfp[0].ndata = dmap->dentry[loc].nvalue;
	return 1;
}

static void dmap_addSN(CTX, knh_dmap_t *dmap, ksfp_t *kvsfp)
{
	size_t loc = dmap->size;
	dmap_grow(_ctx, dmap);
	KINITv(dmap->dentry[loc].key, kvsfp[0].s);
	dmap->dentry[loc].nvalue = kvsfp[1].ndata;
	K_USE_FASTDMAP(dmap->dentry[loc].ukey = dmap->strkeyuint(S_tobytes(kvsfp[0].s)));
	dmap->size++;
}

static void dmap_setSN(CTX, kmapptr_t* m, ksfp_t* kvsfp)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	kbytes_t key = S_tobytes(kvsfp[0].s);
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
		if(loc == -1) {
			dmap_addSN(_ctx, dmap, kvsfp);
			dmap_sort(dmap);
			return;
		}
	}
	dmap->dentry[loc].nvalue = kvsfp[1].ndata;
}

static void dmap_removeSN(CTX, kmapptr_t* m, ksfp_t *kvsfp)
{
	knh_dmap_t *dmap = knh_map_dmap(m);
	kbytes_t key = S_tobytes(kvsfp[0].s);
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
		if(loc == -1) {
			return;
		}
	}
	KNH_FINALv(_ctx, dmap->dentry[loc].key);
	if((size_t)loc < dmap->sorted) {
		dmap->sorted -= 1;
	}
	memmove(dmap->dentry + loc, dmap->dentry + loc + 1, (dmap->size - loc - 1)*sizeof(knh_dentry_t));
	dmap->size--;
}

static const knh_MapDPI_t DMAP_SN = {
	K_DSPI_MAP, "dictionary",
	dmap_config, dmap_init, dmap_reftraceON, dmap_free,
	dmap_getSN, dmap_setSN, dmap_removeSN, dmap_size, dmap_nextON,
};

static const knh_MapDPI_t* dmap_config(CTX, kcid_t p1, kcid_t p2)
{
	if(IS_Tstr(p1)) {
		if(TY_isUnbox(p2)) {
			return &DMAP_SN;
		}
		return &DMAP_SO;
	}
	return NULL;
}

/* ------------------------------------------------------------------------- */
/* [casecmp] */

static uint64_t knh_strcaseuint64(kbytes_t t)
{
	int ch[8] = {0};
	uint64_t n = 0;
	size_t i, c = 0;
	for(i = 0; i < 8; i++) {
		while(t.text[c] == '_') c++;
		if(c >= t.len) break;
		ch[i] = toupper(t.text[c]); c++;
	}
	if(i > 0) {
		switch(i) {
		default: n |= ((uint64_t)ch[7]);
		case 7: n |= (((uint64_t)ch[6]) << (64-56));
		case 6: n |= (((uint64_t)ch[5]) << (64-48));
		case 5: n |= (((uint64_t)ch[4]) << (64-40));
		case 4: n |= (((uint64_t)ch[3]) << (64-32));
		case 3: n |= (((uint64_t)ch[2]) << (64-24));
		case 2: n |= (((uint64_t)ch[1]) << (64-16));
		case 1: n |= (((uint64_t)ch[0]) << (64-8));
		}
	}
	return n;
}

int knh_bytes_strcasecmp2(kbytes_t t1, kbytes_t t2)
{
	const char *p1 = t1.text, *e1 = t1.text + t1.len;
	const char *p2 = t2.text, *e2 = t2.text + t2.len;
	int ch1, ch2;
	while(1) {
		while(*p1 == '_') p1++;
		while(*p2 == '_') p2++;
		if(p1 == e1) return (p2 == e2) ? 0 : 1;
		if(p2 == e2) return -1;
		ch1 = toupper((unsigned char)*p1);
		ch2 = toupper((unsigned char)*p2);
		if(ch1 == ch2) {
			p1++; p2++; continue;
		}
		return (ch1 < ch2) ? -1 : 1;
	}
}

static int dentry_strcasecmp(const void *p, const void *p2)
{
	knh_dentry_t *e = (knh_dentry_t*)p;
	knh_dentry_t *e2 = (knh_dentry_t*)p2;
	K_USE_FASTDMAP(if(e->ukey == e2->ukey))
		return knh_bytes_strcasecmp2(S_tobytes(e->key), S_tobytes(e2->key));
	K_USE_FASTDMAP(return (e->ukey < e2->ukey) ? -1 : 1;)
}

static void dmap_case(knh_dmap_t *dmap)
{
	dmap->strkeyuint = knh_strcaseuint64;
	dmap->dentrycmpr = dentry_strcasecmp;
	dmap->strcmpr = knh_bytes_strcasecmp2;
}

/* ------------------------------------------------------------------------ */
/* DictMap */

kDictMap* new_DictMap0_(CTX, size_t capacity, int isCaseMap, const char *DBGNAME)
{
	kMap *m = new_H(Map);
	m->spi = &DMAP_SO;
	m->mapptr = m->spi->init(_ctx, capacity, NULL, NULL);
	DBG_ASSERT(m->mapptr != NULL);
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	if(isCaseMap) dmap_case(dmap);
	dmap->DBGNAME = DBGNAME;
	return (kDictMap*)m;
}

kDictMap* knh_toDictMap(CTX, kMap *m, int isCreation)
{
	if(m->spi != &DMAP_SO) {
		return (isCreation) ? new_DictMap0(_ctx, 0, 1/*isCaseMap*/, __FUNCTION__) : NULL;
	}
	return (kDictMap*)m;
}

kDictSet* new_DictSet0_(CTX, size_t capacity, int isCaseMap, const char *DBGNAME)
{
	kMap *m = new_H(Map);
	m->spi = &DMAP_SN;
	m->mapptr = m->spi->init(_ctx, capacity, NULL, NULL);
	DBG_ASSERT(m->mapptr != NULL);
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	if(isCaseMap) dmap_case(dmap);
	dmap->DBGNAME = DBGNAME;
	return (kDictSet*)m;
}

// @see ClassCONST_man

KNHAPI2(kString*) knh_DictMap_keyAt(kDictMap *m, size_t n)
{
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	DBG_ASSERT(n < knh_Map_size(m));
	return dmap->dentry[n].key;
}

KNHAPI2(Object*) knh_DictMap_valueAt(kDictMap *m, size_t n)
{
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	DBG_ASSERT(n < knh_Map_size(m));
	return dmap->dentry[n].value;
}

uintptr_t knh_DictSet_valueAt(kDictSet *m, size_t n)
{
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	DBG_ASSERT(n < knh_Map_size(m));
	return (uintptr_t)dmap->dentry[n].nvalue;
}

kindex_t knh_DictMap_index(kDictMap *m, kbytes_t key)
{
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
	}
	return loc;
}

Object *knh_DictMap_getNULL(CTX, kDictMap *m, kbytes_t key)
{
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
	}
	return (loc == -1) ? NULL : dmap->dentry[loc].value;
}

uintptr_t knh_DictSet_get(CTX, kDictSet *m, kbytes_t key)
{
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	kindex_t loc = dmap_index(dmap, 0, dmap->sorted, key);
	if(loc == -1) {
		loc = dmap_index(dmap, dmap->sorted, dmap->size, key);
	}
	return (loc == -1) ? 0 : (uintptr_t)dmap->dentry[loc].nvalue;
}

void knh_DictMap_set_(CTX, kDictMap *m, kString *key, dynamic *v)
{
	ksfp_t* kvsfp = ctx->esp;
	KSETv(kvsfp[0].o, key);
	KSETv(kvsfp[1].o, v);
	m->spi->set(_ctx, m->mapptr, kvsfp);
}

void knh_DictSet_set(CTX, kDictSet *m, kString *key, uintptr_t n)
{
	ksfp_t* kvsfp = ctx->esp;
	KSETv(kvsfp[0].o, key);
	kvsfp[1].ivalue = n;
	m->spi->set(_ctx, m->mapptr, kvsfp);
}

void knh_DictMap_append(CTX, kDictMap *m, kString *key, kObject *v)
{
	ksfp_t* kvsfp = ctx->esp;
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	KSETv(kvsfp[0].o, key);
	KSETv(kvsfp[1].o, v);
	dmap_addSO(_ctx, dmap, kvsfp);
}

//void knh_DictMap_sort(CTX, kDictMap *m)
//{
//	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
//	dmap_sort(dmap);
//}

void knh_DictSet_append(CTX, kDictSet *m, kString *key, uintptr_t n)
{
	ksfp_t* kvsfp = ctx->esp;
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	KSETv(kvsfp[0].o, key);
	kvsfp[1].ivalue = n;
	dmap_addSN(_ctx, dmap, kvsfp);
}

void knh_DictSet_sort(CTX, kDictSet *m)
{
	knh_dmap_t *dmap = (knh_dmap_t*)m->mapptr;
	dmap_sort(dmap);
}

/* ------------------------------------------------------------------------ */
/* API2 */

KNHAPI2(kMap*) new_DataMap(CTX)
{
	kMap *m = new_H(Map);
	m->spi = &DMAP_SO;
	m->mapptr = m->spi->init(_ctx, 4, NULL, NULL);
	DBG_ASSERT(m->mapptr != NULL);
	return m;
}

KNHAPI2(void) knh_DataMap_set(CTX, kMap *m, kString *key, kObject *value)
{
	ksfp_t* kvsfp = ctx->esp;
	KSETv(kvsfp[0].o, key);
	KSETv(kvsfp[1].o, value);
	m->spi->set(_ctx, m->mapptr, kvsfp);
}

KNHAPI2(void) knh_DataMap_setString(CTX, kMap *m, const char *key, const char *value)
{
	ksfp_t* kvsfp = ctx->esp;
	KSETv(kvsfp[0].o, new_kString(key, strlen(key), SPOL_POOL));
	KSETv(kvsfp[1].o, new_kString(value, strlen(value), 0));
	m->spi->set(_ctx, m->mapptr, kvsfp);
}

KNHAPI2(void) knh_DataMap_setInt(CTX, kMap *m, const char *key, kint_t value)
{
	ksfp_t* kvsfp = ctx->esp;
	KSETv(kvsfp[0].o, new_kString(key, strlen(key), SPOL_POOL));
	KSETv(kvsfp[1].o, new_Int_(_ctx, CLASS_Int, value));
	m->spi->set(_ctx, m->mapptr, kvsfp);
}

KNHAPI2(void) knh_DataMap_setFloat(CTX, kMap *m, const char *key, kfloat_t value)
{
	ksfp_t* kvsfp = ctx->esp;
	KSETv(kvsfp[0].o, new_kString(key, strlen(key), SPOL_POOL));
	KSETv(kvsfp[1].o, new_Float_(_ctx, CLASS_Float, value));
	m->spi->set(_ctx, m->mapptr, kvsfp);
}

KNHAPI2(void) knh_DataMap_setBlob(CTX, kMap *m, const char *key, const char *value, size_t size)
{
	ksfp_t* kvsfp = ctx->esp;
	kBytes *ba = new_Bytes(_ctx, NULL, size);
	knh_Bytes_write2(_ctx, ba, value, size);
	KSETv(kvsfp[0].o, new_kString(key, strlen(key), SPOL_POOL));
	KSETv(kvsfp[1].o, ba);
	m->spi->set(_ctx, m->mapptr, kvsfp);
}

/* ------------------------------------------------------------------------ */

void knh_loadScriptDefaultMapDSPI(CTX, kLingo *ns)
{
	const knh_LoaderAPI_t* ploader = knh_getLoaderAPI();
	ploader->addMapDPI(_ctx, "hash", &HMAP_SO);
	ploader->addMapDPI(_ctx, "dict", &DMAP_SO);
}

/* ------------------------------------------------------------------------ */

const knh_MapDPI_t *knh_getDefaultMapDSPI(CTX, kcid_t p1, kcid_t p2)
{
	return kmap_config(_ctx, p1, p2);
}

const knh_MapDPI_t *knh_getDictMapDSPI(CTX, kcid_t p1, kcid_t p2)
{
	if(IS_Tstr(p1)) {
		if(TY_isUnbox(p2)) {
			return &DMAP_SN;
		}
		return &DMAP_SO;
	}
	return NULL;
}

#ifdef __cplusplus
}
#endif

