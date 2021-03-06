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

#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>
#include<konoha2/klib.h>

struct fn {
	uintptr_t  flag;
	const char *aname;
};

#define _P(T)  kMethod_##T, #T

static struct fn attrs[] = {
	{_P(Public)}, {_P(Static)}, {_P(Const)}, {_P(Abstract)},
	{_P(Virtual)}, {_P(Overloaded)},
	{_P(Coercion)},
	{_P(D)}, {_P(Restricted)}, {_P(Immutable)},
	{_P(FASTCALL)}, {_P(CALLCC)},
	{_P(Hidden)},
	{0, NULL}
};

static void MethodAttribute_p(CTX, kMethod *mtd, kwb_t *wb)
{
	uintptr_t i;
	for(i = 0; i < 30; i++) {
		if(attrs[i].aname == NULL) break;
		if((attrs[i].flag & mtd->flag) == attrs[i].flag) {
			kwb_printf(wb, "@%s ", attrs[i].aname);
		}
	}
	if(kMethod_isCast(mtd)) {
		kwb_printf(wb, "@Cast ");
	}
//	if(kMethod_isTransCast(mtd)) {
//		kwb_printf(wb, "@T ");
//	}
}

static void Method_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kMethod *mtd = sfp[pos].mtd;
	kParam *pa = kMethod_param(mtd);
	char mbuf[128];
	DBG_ASSERT(IS_Method(mtd));
	if(level != 0) {
		MethodAttribute_p(_ctx, mtd, wb);
	}
	kwb_printf(wb, "%s %s.%s", T_ty(pa->rtype), T_cid(mtd->cid), T_mn(mbuf, mtd->mn));
	if(level != 0) {
		size_t i;
		kwb_putc(wb, '(');
		for(i = 0; i < pa->psize; i++) {
			if(i > 0) {
				kwb_putc(wb, ',', ' ');
			}
			if(FN_isCOERCION(pa->p[i].fn)) {
				kwb_printf(wb, "@Coercion ");
			}
			kwb_printf(wb, "%s %s", T_ty(pa->p[i].ty), T_fn(MN_UNMASK(pa->p[i].fn)));
		}
//		if(Param_isVARGs(DP(mtd)->mp)) {
//			knh_write_delimdots(_ctx, w);
//		}
		kwb_putc(wb, ')');
	}
}

// --------------------------------------------------------------------------

static void copyMethodList(CTX, kcid_t cid, kArray *s, kArray *d)
{
	size_t i;
	for(i = 0; i < kArray_size(s); i++) {
		kMethod *mtd = s->methods[i];
		if(mtd->cid != cid) continue;
		kArray_add(d, mtd);
	}
}

static void dumpMethod(CTX, ksfp_t *sfp, kMethod *mtd)
{
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	KSETv(sfp[2].o, mtd);
	O_ct(mtd)->p(_ctx, sfp, 2, &wb, 1);
	fprintf(stdout, "%s\n", kwb_top(&wb, 1));
	kwb_free(&wb);
	return;
}

static void dumpMethodList(CTX, ksfp_t *sfp, size_t start, kArray *list)
{
	size_t i;
	for(i = start; i < kArray_size(list); i++) {
		dumpMethod(_ctx, sfp, list->methods[i]);
	}
}

KMETHOD KonohaSpace_man(CTX, ksfp_t *sfp _RIX)
{
	INIT_GCSTACK();
	kArray *list = _ctx->stack->gcstack;
	size_t start = kArray_size(list);
	kKonohaSpace *ks = sfp[0].ks;
	kclass_t *ct = O_ct(sfp[1].o);
	DBG_P("*** man %s", T_cid(ct->cid));
//	((struct _kclass*)CT_Method)->p = Method_p;
	while(ks != NULL) {
		copyMethodList(_ctx, ct->cid, ks->methods, list);
		ks = ks->parentNULL;
	}
	copyMethodList(_ctx, ct->cid, ct->methods, list);
	dumpMethodList(_ctx, sfp, start, list);
	RESET_GCSTACK();
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t i_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	kclass_t *ct = kclass(TY_Method, pline);
	KSET_CLASSFUNC(ct, p, Method, pline);
	intptr_t MethodData[] = {
		_Public, _F(KonohaSpace_man), TY_void, TY_KonohaSpace, MN_("man"), 1, TY_Object, FN_("x") | FN_COERCION,
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t i_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t i_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
//	USING_SUGAR;
//	KDEFINE_SYNTAX SYNTAX[] = {
//		{ TOKEN("float"), .type = TY_Float, },
//		{ TOKEN("double"), .type = TY_Float, },
//		{ TOKEN("$FLOAT"), .kw = KW_TK(TK_FLOAT), .ExprTyCheck = ExprTyCheck_FLOAT, },
//		{ .name = NULL, },
//	};
//	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t i_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* i_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("konoha.i", "1.0"),
		.initPackage = i_initPackage,
		.setupPackage = i_setupPackage,
		.initKonohaSpace = i_initKonohaSpace,
		.setupKonohaSpace = i_setupKonohaSpace,
	};
	return &d;
}
