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

#include "kernel_id.h"
#include "ecrobot_base.h"
#include "ecrobot_interface.h"
#include "balancer.h"

#include "tinykonoha.h"
#include <../../include/konoha2/konoha2.h>
#include "vm.h"
#include "tinyvm.h"
#include "allocate.h"
#include "../msgc/msgc.c"
#include "../../include/konoha2/sugar.h"
#include "../../src/konoha/methods.h"
#include "datatype.h"

#define K_STACK_SIZE 128
void KRUNTIME_reftraceAll(CTX)
{
	//kcontext_reftrace(_ctx, (kcontext_t*)_ctx);
}

void KONOHA_freeObjectField(CTX, struct _kObject *o)
{
	kclass_t *ct = O_ct(o);
	if(o->h.kvproto->bytemax > 0) {
		karray_t *p = o->h.kvproto;
		KFREE(p->bytebuf, p->bytemax);
		KFREE(p, sizeof(karray_t));
		o->h.kvproto = kvproto_null();
	}
	ct->free(_ctx, o);
}

void KONOHA_reftraceObject(CTX, kObject *o)
{
	kclass_t *ct = O_ct(o);
	if(o->h.kvproto->bytemax > 0) {
		size_t i, pmax = o->h.kvproto->bytemax / sizeof(kvs_t);
		kvs_t *d = o->h.kvproto->kvs;
		BEGIN_REFTRACE(pmax);
		for(i = 0; i < pmax; i++) {
			if(FN_isBOXED(d->key)) {
				KREFTRACEv(d->oval);
			}
			d++;
		}
		END_REFTRACE();
	}
	ct->reftrace(_ctx, o);
}

struct _kObject** KONOHA_reftail(CTX, size_t size)
{
	kstack_t *stack = _ctx->stack;
	size_t ref_size = stack->reftail - stack->ref.refhead;
	if(stack->ref.bytemax/sizeof(void*) < size + ref_size) {
		KARRAY_EXPAND(&stack->ref, (size + ref_size) * sizeof(kObject*));
		stack->reftail = stack->ref.refhead + ref_size;
	}
	struct _kObject **reftail = stack->reftail;
	stack->reftail = NULL;
	return reftail;
}

static kbool_t KRUNTIME_setModule(CTX, int x, kmodshare_t *d, kline_t pline)
{
	if (_ctx->modshare[x] == NULL) {
		_ctx->modshare[x] = d;
		return 1;
	} else {
		return 0;
	}
}

static void Kreport(CTX, int level, const char *msg)
{
	/* TODO */
}

static void Kreportf(CTX, int level, kline_t pline, const char *fmt, ...)
{
	/* TODO */
}

static void CT_addMethod(CTX, kclass_t *ct, kMethod *mtd)
{
	if(unlikely(ct->methods == K_EMPTYARRAY)) {
		KINITv(((struct _kclass*)ct)->methods, new_(MethodArray, 8));
	}
	kArray_add(ct->methods, mtd);
}

static void KonohaSpace_addMethod(CTX, kKonohaSpace *ks, kMethod *mtd)
{
	if(ks->methods == K_EMPTYARRAY) {
		KINITv(((struct _kKonohaSpace*)ks)->methods, new_(MethodArray, 8));
	}
	kArray_add(ks->methods, mtd);
}

static void KonohaSpace_loadMethodData(CTX, kKonohaSpace *ks, intptr_t *data)
{
	static int mn_count = 0;
	intptr_t *d = data;
	while(d[0] != -1) {
		uintptr_t flag = 0;
		knh_Fmethod f = (knh_Fmethod)d[0];
		ktype_t rtype = 0;
		kcid_t cid = (kcid_t)d[1];
		mn_count++;
		kmethodn_t mn = mn_count;
		size_t psize = 0;
		kparam_t p[1];
		d += 3;
		//kMethod *mtd = new_kMethod(flag, cid, mn, f);
		//kMethod_setParam(mtd, rtype, psize, p);
		//if(ks == NULL || kMethod_isPublic(mtd)) {
		//	CT_addMethod(_ctx, CT_(cid), mtd);
		//} else {
		//	KonohaSpace_addMethod(_ctx, ks, mtd);
		//}
	}
}

static void karray_init(CTX, karray_t *m, size_t bytemax)
{
	m->bytesize = 0;
	m->bytemax  = bytemax;
	m->bytebuf = (char*)KCALLOC(bytemax, 1);
}

static void karray_resize(CTX, karray_t *m, size_t newsize)
{
	size_t oldsize = m->bytemax;
	char *newbody = (char*)KMALLOC(newsize);
	if(oldsize < newsize) {
		memcpy(newbody, m->bytebuf, oldsize);
		bzero(newbody + oldsize, newsize - oldsize);
	}
	else {
		memcpy(newbody, m->bytebuf, newsize);
	}
	KFREE(m->bytebuf, oldsize);
	m->bytebuf = newbody;
	m->bytemax = newsize;
}

static void karray_expand(CTX, karray_t *m, size_t minsize)
{
	if(m->bytemax == 0) {
		if(minsize > 0) karray_init(_ctx, m, minsize);
	}
	else {
		size_t oldsize = m->bytemax, newsize = oldsize * 2;
		if(minsize > newsize) newsize = minsize;
		karray_resize(_ctx, m, newsize);
	}
}

static void karray_free(CTX, karray_t *m)
{
	if(m->bytemax > 0) {
		KFREE(m->bytebuf, m->bytemax);
		m->bytebuf = NULL;
		m->bytesize = 0;
		m->bytemax  = 0;
	}
}

static void klib2_init(struct _klib2 *l)
{
	l->Karray_init       = karray_init;
	l->Karray_resize     = karray_resize;
	l->Karray_expand     = karray_expand;
	l->Karray_free       = karray_free;
	l->KsetModule        = KRUNTIME_setModule;
	//l->Kreport           = Kreport;
	//l->Kreportf          = Kreportf;
	l->KS_loadMethodData = KonohaSpace_loadMethodData;
}

static void KRUNTIME_init(CTX, kcontext_t *ctx, size_t stacksize)
{
	size_t i;
	kstack_t *base = (kstack_t*)KCALLOC(sizeof(kstack_t), 1);
	base->stacksize = stacksize;
	base->stack = (ksfp_t*)KCALLOC(sizeof(ksfp_t), stacksize);
	assert(stacksize>64);
	base->stack_uplimit = base->stack + (stacksize - 64);
	for(i = 0; i < stacksize; i++) {
		KINITv(base->stack[i].o, K_NULL);
	}
	//KINITv(base->gcstack, new_(Array, K_PAGESIZE/sizeof(void*)));
	//KINITv(base->gcstack, new_(Array, 5));
	//KARRAY_INIT(&base->cwb, K_PAGESIZE * 4);
	KARRAY_INIT(&base->ref, 32);
	base->reftail = base->ref.refhead;
	ctx->esp = base->stack;
	ctx->stack = base;
}

static void KRUNTIME_reftrace(CTX, kcontext_t *ctx)
{
	ksfp_t *sp = ctx->stack->stack;
	BEGIN_REFTRACE((_ctx->esp - sp)+1);
	while(sp < ctx->esp) {
		KREFTRACEv(sp[0].o);
		sp++;
	}
	KREFTRACEv(ctx->stack->gcstack);
	END_REFTRACE();
}

static void KRUNTIME_free(CTX, kcontext_t *ctx)
{
	KARRAY_FREE(&_ctx->stack->cwb);
	KARRAY_FREE(&_ctx->stack->ref);
	KFREE(_ctx->stack->stack, sizeof(ksfp_t) * ctx->stack->stacksize);
	KFREE(_ctx->stack, sizeof(kstack_t));
}

static kcontext_t *new_context(size_t stacksize)
{
	heap_init();
	static kcontext_t _ctx;
	static kmodshare_t *modshare[MOD_MAX] = {0};
	static kmodlocal_t *modlocal[MOD_MAX] = {0};
	static struct _klib2 klib2 = {0};
	klib2_init(&klib2);
	_ctx.modshare = modshare;
	_ctx.modlocal = modlocal;
	_ctx.lib2 = &klib2;
	MODGC_init(&_ctx, &_ctx);
	KCLASSTABLE_init(&_ctx);
	KRUNTIME_init(&_ctx, &_ctx, stacksize);
	KCLASSTABLE_loadMethod(&_ctx);
	return &_ctx;
}

void cyc0(VP_INT exinf)
{
}

void TaskMain(VP_INT exinf)
{
}

void TaskDisp(VP_INT exinf)
{
	struct kcontext_t *_ctx = NULL;
	_ctx = new_context(K_STACK_SIZE);
	//new_CT(_ctx, NULL, NULL, 0);
	//VirtualMachine_run(_ctx, sfp, NULL);
	kclass_t *ct = CT_(CLASS_String);
	TDBG_s("loop start");
	while (1) {
		new_kObject(ct, NULL);
	}
}
