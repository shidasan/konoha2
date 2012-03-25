#include "konoha2/konoha2.h"
#include "../src/gc/gc_api.h"
#include "loader/mod_sample.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef PACKAGE_BUILD_DIR
#define BUILD_DIR ""
#else
#define BUILD_DIR PACKAGE_BUILD_DIR "/"
#endif

void test_module_load(CTX)
{
    module_load(_ctx, (kcontext_t*)_ctx, BUILD_DIR "mod_sample.dylib", "ksample");
    ksampleshare_t *sharemod = (ksampleshare_t*) _ctx->modshare[MOD_SAMPLE];
    assert(sharemod);

    sharemod->h.setup(_ctx, (kmodshare_t*)sharemod);
    ksamplemod_t *mod = (ksamplemod_t*) _ctx->mod[MOD_SAMPLE];

    assert(mod);
    assert(IS_NULL(mod->array) == 0);
    knh_System_gc(_ctx, 0);
    assert(IS_NULL(mod->array) == 0);
}

static kcid_t sample_cid;
void test_classdef_load(CTX)
{
    const kclass_t *ct;
    ct = class_load(_ctx, BUILD_DIR "classdef_sample.dylib", "Sample");
    assert(ct);
    assert(strcmp(S_totext(ct->name), "Sample") == 0);
    assert(ct->cflag == 2);
    assert(ct->cstruct_size == 64/*sizeof(struct Sample)*/);
    sample_cid = ct->cid;
}

void test_mtd_load(CTX)
{
    kParam  *pa;
    kMethod *mtd;
    knh_Fmethod fmtd;
    kmethodn_t mn;
    kparam_t p[] = {{TY_Int, 0}, {TY_Int, 0}};
    p[0].fn = _ctx->lib2->Ksymbol(_ctx, "arg0", 4, FN_NEWID, 0);
    p[1].fn = _ctx->lib2->Ksymbol(_ctx, "arg1", 4, FN_NEWID, 0);
    mn = _ctx->lib2->Ksymbol(_ctx, "f", 1, MN_NEWID, 0);

    pa   = _ctx->lib2->Knew_Param(_ctx, TY_Int, 2, p);
    mtd  = _ctx->lib2->Knew_Method(_ctx, 0, sample_cid, mn, pa, 0);
    fmtd = method_load(_ctx, BUILD_DIR "mtd_sample.dylib", mtd);
    assert(fmtd != NULL);
}

int main(int argc, const char *argv[])
{
    konoha_ginit(argc, argv);
    konoha_t konoha = konoha_open();
    test_module_load(konoha);
    test_classdef_load(konoha);
    test_mtd_load(konoha);
    konoha_close(konoha);
    klib2_check_malloced_size();
    return 0;
}
