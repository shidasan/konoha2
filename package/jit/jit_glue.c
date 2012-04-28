#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include "jit.h"

//typedef struct {
//	kmodshare_t h;
//	kMethod *GenCode;
//} kmodjit_t;
//
//typedef struct {
//	kmodlocal_t h;
//} kjitmod_t;
//
//
//static void kmodjit_setup(CTX, struct kmodshare_t *def, int newctx)
//{
//	(void)_ctx;(void)def;(void)newctx;
//}
//
//static void kmodjit_reftrace(CTX, struct kmodshare_t *baseh)
//{
//	(void)_ctx;(void)baseh;
//}
//
//static void kmodjit_free(CTX, struct kmodshare_t *baseh)
//{
//	KFREE(baseh, sizeof(kmodjit_t));
//}
//
//
//static void KMethod_genCode(CTX, kMethod *mtd, kBlock *bk)
//{
//	DBG_P("START CODE GENERATION..");
//	BEGIN_LOCAL(lsfp, 8);
//	KSETv(lsfp[K_CALLDELTA+0].o, mtd);
//	KSETv(lsfp[K_CALLDELTA+1].o, bk);
//	KCALL(lsfp, 0, MtdGenCode, 2, K_NULL);
//	END_LOCAL();
//}
//
//static kbool_t jit_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
//{
//	KSET_KLIB2(Method_genCode, pline);
//	kmodjit_t *base  = (kmodjit_t*)KCALLOC(sizeof(kmodjit_t));
//	base->h.name     = "jit";
//	base->h.setup    = kmodjit_setup;
//	base->h.reftrace = kmodjit_reftrace;
//	base->h.free     = kmodjit_free;
//	ksetModule(MOD_jit, &base->h, pline);
//	return true;
//}

static kbool_t jit_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t jit_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{

	return true;
}

static kbool_t jit_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* jit_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("jit", "1.0"),
		.initPackage = llvm_jit_initPackage,
		.setupPackage = jit_setupPackage,
		.initKonohaSpace = jit_initKonohaSpace,
		.setupKonohaSpace = jit_setupKonohaSpace,
	};
	return &d;
}
