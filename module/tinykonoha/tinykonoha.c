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

#include "tinykonoha.h"
#include <../../include/konoha2/konoha2.h>
#include "vm.h"
#include "tinyvm.h"
#include "allocate.h"
#include "../msgc/msgc.c"
#include "../../include/konoha2/sugar.h"
#include "datatype.h"

#include "kernel_id.h"
#include "ecrobot_base.h"
#include "ecrobot_interface.h"
#include "balancer.h"

ksfp_t sfp[SFP_SIZE];

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
	intptr_t *d = data;
	while(d[0] != -1) {
		uintptr_t flag = (uintptr_t)d[0];
		knh_Fmethod f = (knh_Fmethod)d[1];
		ktype_t rtype = (ktype_t)d[2];
		kcid_t cid  = (kcid_t)d[3];
		kmethodn_t mn = (kmethodn_t)d[4];
		size_t i, psize = (size_t)d[5];
		kparam_t p[psize+1];
		d = d + 6;
		for(i = 0; i < psize; i++) {
			p[i].ty = (ktype_t)d[0];
			p[i].fn = (ksymbol_t)d[1];
			d += 2;
		}
		kMethod *mtd = new_kMethod(flag, cid, mn, f);
		kMethod_setParam(mtd, rtype, psize, p);
		if(ks == NULL || kMethod_isPublic(mtd)) {
			CT_addMethod(_ctx, CT_(cid), mtd);
		} else {
			KonohaSpace_addMethod(_ctx, ks, mtd);
		}
	}
}

static void klib2_init(struct _klib2 *l)
{
	l->KsetModule = KRUNTIME_setModule;
	l->Kreport = Kreport;
	l->Kreportf = Kreportf;
	l->KS_loadMethodData = KonohaSpace_loadMethodData;
}

static kcontext_t *new_context()
{
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
	_ctx = new_context();
	//new_CT(_ctx, NULL, NULL, 0);
	//VirtualMachine_run(_ctx, sfp, NULL);
}
