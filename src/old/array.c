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
/* Array */

static const kdim_t dimINIT = {
	0, 0, 0, 0, 0, 0, 0,
};

KNHAPI2(kArray*) new_Array(CTX, kcid_t p1, size_t capacity)
{
	kcid_t cid = knh_class_P1(_ctx, CLASS_Array, p1);
	kArray *a = (kArray*)new_Object_init2(_ctx, ClassTBL(cid));
	if(capacity > 0) {
		kArray_grow(_ctx, a, capacity, 8);
	}
	return a;
}

kArray* new_ArrayG(CTX, kcid_t cid, size_t capacity)
{
	kArray *a = (kArray*)new_Object_init2(_ctx, ClassTBL(cid));
	if(capacity > 0) {
		kArray_grow(_ctx, a, capacity, 8);
	}
	return a;
}

void kArray_grow(CTX, kArray *a, size_t newsize, size_t reqsize)
{
	size_t capacity = a->dim->capacity;
	if(newsize < reqsize) newsize = reqsize;
	if(newsize == 0) return;
	if(capacity == 0) {
		size_t wsize = (kArray_isUnboxData(a)) ? sizeof(kunbox_t) : sizeof(Object*);
		a->dim = new_dim(_ctx, newsize, wsize);
		DBG_ASSERT(a->list == NULL);
	}
	else {
		((kdim_t*)a->dim)->capacity = newsize;
	}
	a->list = (kObject**)KNH_REALLOC(_ctx, "Array", a->list, capacity, newsize, a->dim->wsize);
}

KNHAPI2(void) kArray_add_(CTX, kArray *a, kObject *value)
{
	size_t capacity = a->dim->capacity;
	if(a->size == capacity) {
		kArray_grow(_ctx, a, k_grow(capacity), a->size + 1);
	}
	KINITv(a->list[a->size], value);
	a->size++;
}

KNHAPI2(void) kArray_remove_(CTX, kArray *a, size_t n)
{
	DBG_ASSERT(n < a->size);
	if (kArray_isUnboxData(a)) {
		knh_memmove(a->nlist+n, a->nlist+(n+1), sizeof(kunbox_t) * (a->size - n - 1));
	} else {
		KNH_FINALv(_ctx, a->list[n]);
		knh_memmove(a->list+n, a->list+(n+1), sizeof(kObject*) * (a->size - n - 1));
	}
	a->size--;
}

KNHAPI2(void) kArray_swap(CTX, kArray *a, size_t n, size_t m)
{
	DBG_ASSERT(n < a->size);
	DBG_ASSERT(m < a->size);
#ifdef K_USING_RCGC
	Object *temp = a->list[n];
	a->list[n] = a->list[m];
	a->list[m] = temp;
#else
	ksfp_t *esp1 = ctx->esp + 1;
	esp1[0].o = a->list[n];
	a->list[n] = a->list[m];
	a->list[m] = esp1[0].o;
#endif
}

void kArray_clear(CTX, kArray *a, size_t n)
{
	if(!kArray_isUnboxData(a)) {
#ifdef K_USING_RCGC
		size_t i;
		for(i = n; i < a->size; i++) {
			KNH_FINALv(_ctx, a->list[i]);
		}
#else
		knh_bzero(a->list + n, sizeof(void*) * (a->size - n));
#endif
	}
	a->size = n;
}

/* ------------------------------------------------------------------------ */
/* [api] */


/* ------------------------------------------------------------------------ */
/* Iterator */

static ITRNEXT Fitrnext_end(CTX, ksfp_t *sfp _RIX)
{
	ITREND_();
}

void knh_Iterator_close(CTX, kIterator *it)
{
	DBG_ASSERT(IS_bIterator(it));
	if(it->fnext_1 != Fitrnext_end) {
		KSETv(DP(it)->source, K_NULL);
		if(DP(it)->m.nfree != NULL) {
			if(DP(it)->m.nptr != NULL) {
				DP(it)->m.nfree(DP(it)->m.nptr);
			}
			DP(it)->m.nfree = NULL;
		}
		DP(it)->m.nptr = NULL;
		it->fnext_1   = Fitrnext_end;
	}
}

KNHAPI2(kIterator*) new_IteratorG(CTX, kcid_t cid, kObject *source, knh_Fitrnext fnext)
{
	DBG_ASSERT(C_bcid(cid) == CLASS_Iterator);
	kIterator *it = new_O(Iterator, cid);
	if(IS_NULL(source)) fnext = Fitrnext_end;
	KSETp( it, DP(it)->source, source);
	it->fnext_1 = fnext;
	return it;
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

