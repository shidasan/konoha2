#include "konoha2/konoha2.h"
#include "konoha2/sugar.h"
#include "konoha2/gc.h"

/* operator */
#define DEFINE_KMETHOD 1
#include "../src/konoha/methods.h"
/* from ../src/minivm/datatype.c */
#ifndef DEND
#define PTRSIZE  (sizeof(void*))
#define DEND     ((const char*)-1)
#define DT_(X)   ((const char*)((TY_##X)    | (1UL<<(PTRSIZE*8-1))))
#define DC_(X)   ((const char*)((CLASS_##X) | (1UL<<(PTRSIZE*8-1))))
#define DS_(X)   ((const char*)((FN_##X)    | (1UL<<(PTRSIZE*8-1))))
#define DM_(X)   ((const char*)((MN_##X)    | (1UL<<(PTRSIZE*8-1))))
#define DF_(X)   ((const char*)(X))
#define FF_(X)   (kMethod_##X)
#endif

static const char *Pkeyword_(CTX, keyword_t kw) { return NULL; }
static kString *Skw_(CTX, keyword_t kw) { return NULL; }

static const char *methoddata[] = {
	DF_(FF_(Static)), DF_(Int_opADD), DT_(Int), DC_(Int), "opAdd", DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opSUB), DT_(Int), DC_(Int), "opSUB", DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opMUL), DT_(Int), DC_(Int), "opMUL", DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opDIV), DT_(Int), DC_(Int), "opDIV", DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opMOD), DT_(Int), DC_(Int), "opMOD", DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opEQ),  DT_(Int), DC_(Int), "opEQ",  DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opNEQ),  DT_(Int), DC_(Int), "opNEQ",  DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opLT),  DT_(Int), DC_(Int), "opLT",  DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opGT),  DT_(Int), DC_(Int), "opGT",  DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opLTE), DT_(Int), DC_(Int), "opLTE", DF_(2), "Int", "a", "Int", "b",
	DF_(FF_(Static)), DF_(Int_opGTE), DT_(Int), DC_(Int), "opGTE", DF_(2), "Int", "a", "Int", "b",
	DEND
};

void test_KloadMethodData(CTX)
{
    kmodsugar->h.setup(_ctx, (kmodshare_t*)kmodsugar);
    _ctx->lib2->KloadMethodData(_ctx, kmodsugar->rootns, methoddata);
}

int main(int argc, const char *argv[])
{
    konoha_t konoha = konoha_open();
    test_KloadMethodData(konoha);
    konoha_close(konoha);
    MODGC_check_malloced_size();
    return 0;
}
