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
#include "kernel_id.h"
#include "ecrobot_base.h"
#include "ecrobot_interface.h"
#include "balancer.h"

#include "allocate.h"
#include "vm.h"
#include "tinyvm.h"
#include "../../include/konoha2/gc.h"
#include "../msgc/msgc.c"

ksfp_t sfp[STACK_SIZE];

void KRUNTIME_reftraceAll(CTX)
{
	//kcontext_reftrace(_ctx, (kcontext_t*)_ctx);
}

#define KVPROTO_INIT  8
#define KVPROTO_DELTA 7

static inline karray_t* kvproto_null(void)  // for proto_get safe null
{
	static kvs_t dnull[KVPROTO_DELTA] = {};
	static karray_t pnull = {
		.bytesize = sizeof(kvs_t), .bytemax = 0,
	};
	pnull.kvs = dnull;
	return &pnull;
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

static kObject *DEFAULT_fnull(CTX, kclass_t *ct)
{
	DBG_ASSERT(ct->nulvalNUL != NULL);
	return ct->nulvalNUL;
}

static void DEFAULT_init(CTX, kObject *o, void *conf)
{
	(void)_ctx;(void)o;(void)conf;
}

static void DEFAULT_reftrace(CTX, kObject *o)
{
	(void)_ctx;(void)o;
}

static void DEFAULT_free(CTX, kObject *o)
{
	(void)_ctx;(void)o;
}

static void DEFAULT_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	(void)_ctx;(void)sfp;(void)pos;(void)wb;(void)level;
}

static uintptr_t DEFAULT_unbox(CTX, kObject *o)
{
	return 0;
}

static struct _kclass *new_CT(CTX, kclass_t *bct, KDEFINE_CLASS *s, kline_t pline)
{
	static struct _kclass cts[MAX_CT];
	kshare_t *share = _ctx->share;
	kcid_t newcid = share->casize;
	assert(newcid < MAX_CT);
	share->casize++;
	struct _kclass *ct = share->ca[newcid];
	if (bct != NULL) {
		DBG_ASSERT(s == NULL);
		memcpy(ct, bct, offsetof(kclass_t, cparam));
		ct->cid = newcid;
		//if (ct->fnull == DEFAULT_fnull) ct->fnull = DEFAULT_fnullinit;
	} else {
		DBG_ASSERT(s != NULL);
		ct->cflag = s->cflag;
		ct->cid = newcid;
		ct->bcid = newcid;
		ct->supcid = (s->supcid == 0) ? CLASS_Object : s->supcid;
		ct->fields = s->fields;
		ct->fsize = s->fsize;
		ct->fallocsize = s->fallocsize;
		//ct->cstruct_size = size64(s->cstruct_size);
		if (s->cparams != NULL) {
			DBG_P("params");
			//KINITv(ct->cparam, new_kParam2(s->rtype, s->psize, s->cparams));
		}
		ct->init = (s->init != NULL) ? s->init : DEFAULT_init;
		ct->reftrace = (s->reftrace != NULL) ? s->reftrace : DEFAULT_reftrace;
		ct->p = (s->p != NULL) ? s->p : DEFAULT_p;
		ct->unbox = (s->unbox != NULL) ? s->unbox : DEFAULT_unbox;
		ct->free = (s->free != NULL) ? s->free : DEFAULT_free;
		//ct->fnull = (s->fnull != NULL) ? s->fnull : DEFAULT_fnullinit;
		//ct->realtype = (s->realtype != NULL) ? s->realtype : DEFAULT_realtype;
		//ct->isSubType = (s->isSubType != NULL) ? s->isSubType : DEFAULT_isSubType;
		ct->initdef = s->initdef;
	}
	if (ct->initdef != NULL) {
		ct->initdef(_ctx, ct, pline);
	}
	return ct;
}

static kclass_t *addClassDef(CTX, kpack_t packid, kpack_t packdom, kString *name, KDEFINE_CLASS *cdef, kline_t pline)
{
	struct _kclass *ct = new_CT(_ctx, NULL, cdef, pline);
	ct->packid = packid;
	ct->packdom = packdom;
	if (name == NULL) {
		const char *n = cdef->structname;
		//ct->nameid = kuname(n, strlen(n), SPOL_ASCII|SPOL_POOL|SPOL_TEXT, _NEWID);
	} else {
		//ct->nameid = kuname(S_text(name), S_size(name), 0, _NEWID);
	}
	//CT_setName(_ctx, ct, pline);
	return (kclass_t*)ct;
}

static void KCLASSTABLE_initklib2(struct _klib2 *l)
{
	l->KaddClassDef = addClassDef;
}

static void loadInitStructData(CTX)
{

}

static void KCLASSTABLE_init(kcontext_t *_ctx)
{
	static kshare_t share;
	_ctx->share = &share;
	static struct _kclass *ca[MAX_CT];
	_ctx->share->ca = ca;
	_ctx->share->casize = 0;
	KCLASSTABLE_initklib2((struct _klib2*)_ctx->lib2);
	loadInitStructData(_ctx);
}

static kcontext_t *new_context()
{
	static kcontext_t _ctx;
	static kmodshare_t *modshare[MOD_MAX] = {0};
	static kmodlocal_t *modlocal[MOD_MAX] = {0};
	static struct _klib2 klib2 = {0};
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
	struct kcontext_t *_ctx = NULL;
	//_ctx = new_context();
	new_CT(_ctx, NULL, NULL, 0);
	VirtualMachine_run(_ctx, sfp, NULL);
}

void TaskDisp(VP_INT exinf)
{

}
