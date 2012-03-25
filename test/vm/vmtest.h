#include "konoha2/konoha2.h"
#include "../../src/vm/vm.h"
#include "../../src/vm/minivm.h"
#include "../../src/gc/gc_api.h"

static void thcode(CTX, struct kopl_t *pc, void **codeaddr)
{
#ifdef K_USING_THCODE_
    while(1) {
        pc->codeaddr = codeaddr[pc->opcode];
        if(pc->opcode == OPCODE_RET) break;
        pc++;
    }
#endif
}

static kopl_t *opinit(kopl_t *op, kopcode_t opcode)
{
    op->opcode = opcode;
    return op;
}

#define OPCAST(opcode, op) ((klr_##opcode##_t *) opinit(op++, OPCODE_##opcode))
