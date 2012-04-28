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

#ifdef __cplusplus
extern "C" {
#endif

#include"commons.h"
#include<string.h>

/* ************************************************************************ */

/* ------------------------------------------------------------------------ */
/* [dim] */

size_t k_goodsize(size_t ss)
{
	size_t s = ss;
	if(s == 0) return 0;  /* added */
	if(s <= K_FASTMALLOC_SIZE) return K_FASTMALLOC_SIZE;
	if(s > 64 * 1024) return s;
	s |= s >> 1;
	s |= s >> 2;
	s |= s >> 4;
	s |= s >> 8;
	s |= s >> 16;
	return ((s + 1) == ss * 2) ? ss : s + 1;
}

size_t k_goodsize2(size_t ss, size_t wsize)
{
	return k_goodsize(ss * wsize) / wsize;
}

const kdim_t *new_dim(CTX, size_t capacity, size_t wsize)
{
	kdim_t *dim = (kdim_t*)KCALLOC(sizeof(kdim_t));
	dim->capacity = capacity;
	dim->wsize = wsize;
	dim->dim = 1;
	dim->x = dim->xy = dim->xyz = capacity;
	return (const kdim_t*)dim;
}

void knh_dimfree(CTX, void *p, const kdim_t *dim)
{
	size_t size = dim->capacity * dim->wsize;
	if(size > 0) {
		KFREE(_ctx, p, size);
		KFREE(_ctx, (void*)dim, sizeof(kdim_t));
	}
}

/* ------------------------------------------------------------------------ */

static void Bytes_checkstack(CTX, kchar_t*oldstart, kchar_t *oldend, kchar_t* newstart)
{
	kchar_t **cstack_top = (kchar_t**)(&ctx);
	kchar_t **p = (kchar_t**)ctx->cstack_bottom;
	DBG_P("cstack_bottom=%p, cstack_top=%p basedata=%p", p[0], cstack_top, &oldstart);
	if(!(p < cstack_top)) {
		kchar_t **tmp = cstack_top;
		cstack_top = p; p = tmp;
	}
	while(p <= cstack_top) {
		if((oldstart <= p[0] && p[0] < oldend)) {
			kchar_t *newc = p[0] + (newstart - oldstart);
			KNH_LOG("oldptr=%p, newptr=%p", p[0], newc);
			p[0] = newc;
		}
		p++;
	}
}

void knh_Bytes_expands(CTX, kBytes *ba, size_t newsize)
{
	if(ba->dim->capacity == 0) {
		newsize = k_goodsize(newsize);
		ba->bu.ubuf = (kchar_t*)KCALLOC(newsize);
		ba->dim = new_dim(_ctx, newsize, 1);
	}
	else {
		kchar_t *ubuf = ba->bu.ubuf;
		ba->bu.ubuf = (kchar_t*)KNH_REALLOC(_ctx, ba->DBG_name, ba->bu.ubuf, ba->dim->capacity, newsize, 1);
		((kdim_t*)ba->dim)->capacity = newsize;
		if(unlikely(_ctx->bufa == ba)) {
			Bytes_checkstack(_ctx, ubuf, ubuf + ba->bu.len, ba->bu.ubuf);
		}
	}
}

void knh_Bytes_dispose(CTX, kBytes *ba)
{
	if(ba->dim->capacity > 0) {
		//DBG_P("dispose %p %p size=%ld,%ld", ba, ba->bu.ubuf, ba->bu.len, ba->dim->capacity);
		KFREE(_ctx, ba->bu.ubuf, ba->dim->capacity);
		ba->bu.ubuf = NULL;
		ba->bu.len = 0;
		((kdim_t*)ba->dim)->capacity = 0;
	}
}

kBytes* new_Bytes(CTX, const char *name, size_t capacity)
{
	kBytes *ba = new_(Bytes);
	ba->DBG_name = name;
	if(capacity > 0) {
		knh_Bytes_expands(_ctx, ba, capacity);
	}
	return ba;
}

KNHAPI2(void) knh_Bytes_clear(kBytes *ba, size_t pos)
{
	if(pos < BA_size(ba)) {
		knh_bzero(ba->bu.ubuf + pos, BA_size(ba) - pos);
		BA_size(ba) = pos;
	}
}

void knh_Bytes_ensureSize(CTX, kBytes *ba, size_t len)
{
	size_t blen = ba->bu.len + len;
	if(ba->dim->capacity < blen) {
		size_t newsize = k_grow(ba->dim->capacity);
		if(newsize < blen) newsize = k_goodsize(blen);
		knh_Bytes_expands(_ctx, ba, newsize);
	}
}

const char *knh_Bytes_ensureZero(CTX, kBytes *ba)
{
	size_t size = BA_size(ba);
	size_t capacity = ba->dim->capacity;
	if(size == capacity) {
		knh_Bytes_expands(_ctx, ba, k_grow(capacity));
	}
	ba->bu.ubuf[BA_size(ba)] = 0;
	return ba->bu.text;
}

void knh_Bytes_putc(CTX, kBytes *ba, int ch)
{
	size_t capacity = ba->dim->capacity;
	if(BA_size(ba) == capacity) {
		knh_Bytes_expands(_ctx, ba, k_grow(capacity));
	}
	ba->bu.ubuf[BA_size(ba)] = ch;
	BA_size(ba) += 1;
}

void knh_Bytes_reduce(kBytes *ba, size_t size)
{
	if(BA_size(ba) >= size) {
		BA_size(ba) -= size;
		knh_bzero(ba->bu.ubuf + BA_size(ba), size);
	}
}

void knh_Bytes_write(CTX, kBytes *ba, kbytes_t t)
{
	size_t capacity = ba->dim->capacity;
	if(t.len == 0) return ;
	if(BA_size(ba) + t.len >= capacity) {
		size_t newsize = k_grow(capacity);
		if(newsize < BA_size(ba) + t.len) newsize = k_goodsize(BA_size(ba) + t.len);
		knh_Bytes_expands(_ctx, ba, newsize);
	}
	knh_memcpy(ba->bu.ubuf + BA_size(ba), t.utext, t.len);
	BA_size(ba) += t.len;
}

KNHAPI2(void) knh_Bytes_write2(CTX, kBytes *ba, const char *text, size_t len)
{
	size_t capacity = ba->dim->capacity;
	if(len == 0) return ;
	if(BA_size(ba) + len >= capacity) {
		size_t newsize = k_grow(capacity);
		if(newsize < BA_size(ba) + len) newsize = k_goodsize(BA_size(ba) + len);
		knh_Bytes_expands(_ctx, ba, newsize);
	}
	knh_memcpy(ba->bu.ubuf + BA_size(ba), text, len);
	BA_size(ba) += len;
}

/* ------------------------------------------------------------------------ */

kbytes_t CWB_ensure(CTX, CWB_t *cwb, kbytes_t t, size_t reqsize)
{
	if(!(cwb->ba->bu.len + reqsize < cwb->ba->dim->capacity)) {
		const char *p = cwb->ba->bu.text;
		if(p <= t.text && t.text <= p + cwb->pos) {
			size_t s = t.text - p;
			knh_Bytes_expands(_ctx, cwb->ba, reqsize);
			t.text = cwb->ba->bu.text + s;
		}
		else {
			knh_Bytes_expands(_ctx, cwb->ba, reqsize);
		}
	}
	return t;
}

KNHAPI2(kascii_t*) CWB_totext(CTX, CWB_t *cwb)
{
	return knh_Bytes_ensureZero(_ctx, cwb->ba) + cwb->pos;
}

KNHAPI2(kString*) CWB_newString(CTX, CWB_t *cwb, int pol)
{
	kString *s = TS_EMPTY;
	if(cwb->pos < (cwb->ba)->bu.len) {
		kbytes_t t = CWB_tobytes(cwb);
		s = new_kString(t.text, t.len, pol);
	}
	CWB_close(_ctx, cwb);
	return s;
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
