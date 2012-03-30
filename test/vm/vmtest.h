#include "konoha2/konoha2.h"
#include "konoha2/gc.h"
#include "../../src/vm/vm.h"
#include "../../src/vm/minivm.h"

#ifndef VMTEST_H_
#define VMTEST_H_

static void thcode(CTX, struct kopl_t *pc, void **codeaddr)
{
#ifdef K_USING_THCODE_
    while(1) {
        pc->codeaddr = codeaddr[pc->opcode];
        fprintf(stderr, "%p, %d\n", pc, pc->opcode);
        if(pc->opcode == OPCODE_RET) break;
        if(pc->opcode == OPCODE_EXIT) break;
        pc++;
    }
#endif
}

static kopl_t *opinit(kopl_t *op, kopcode_t opcode)
{
    op->opcode = opcode;
    return op;
}

kopl_t* VirtualMachine_run(CTX, ksfp_t *sfp0, kopl_t *pc);

#define OPCAST(opcode, op) ((klr_##opcode##_t *) opinit(op++, OPCODE_##opcode))
#define emit_ret(pc) do {\
    /*klr_RET_t *op = OPCAST(RET, pc);(void)op;*/\
} while (0)
#endif /* end of include guard */
