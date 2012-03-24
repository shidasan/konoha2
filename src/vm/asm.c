/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 * You may choose one of the following two licenses when you use konoha.
 * If you want to use the latter license, please contact us.
 *
 * (1) GNU General Public License 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure License 1.
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

#include "vm.h"
#include "minivm.h"

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* kcode */

#define DP(O)           (O)
static void EXPR_asm(CTX, int a, kExpr *expr, int espidx);

#define GammaBuilderLabel(n)   (kBasicBlock*)(kcodemod->lstacks->list[n])

kBasicBlock* new_BasicBlockLABEL(CTX)
{
	kBasicBlock *bb = new_(BasicBlock, 0);
	bb->listNC = kcodemod->insts;
	DP(bb)->id = kArray_size(bb->listNC);
	kArray_add(kcodemod->insts, bb);
	return bb;
}

#if defined(K_USING_THCODE_)
#define TADDR   NULL, 0/*counter*/
#else
#define TADDR   0/*counter*/
#endif/*K_USING_THCODE_*/

#define ASMLINE  0

#define ASM(T, ...) { \
		klr_##T##_t op_ = {TADDR, OPCODE_##T, ASMLINE, ## __VA_ARGS__}; \
		BUILD_asm(_ctx, (kopl_t*)(&op_), sizeof(klr_##T##_t)); \
	}\

#define ASMop(T, OP, ...) { \
		klr_##T##_t op_ = {TADDR, OP, ASMLINE, ## __VA_ARGS__}; \
		BUILD_asm(_ctx, (kopl_t*)(&op_), sizeof(klr_##T##_t)); \
	}\

#define ASMbranch(T, lb, ...) { \
		klr_##T##_t op_ = {TADDR, OPCODE_##T, ASMLINE, NULL, ## __VA_ARGS__}; \
		ASM_BRANCH_(_ctx, lb, (kopl_t*)(&op_), sizeof(klr_##T##_t)); \
	}\

#define kBasicBlock_add(bb, T, ...) { \
		klr_##T##_t op_ = {TADDR, OPCODE_##T, ASMLINE, ## __VA_ARGS__};\
		BasicBlock_add(_ctx, bb, 0, (kopl_t*)(&op_), sizeof(klr_##T##_t));\
	}\

#define NC_(sfpidx)    (((sfpidx) * 2) + 1)
#define OC_(sfpidx)    ((sfpidx) * 2)
#define SFP_(sfpidx)   ((sfpidx) * 2)
#define RIX_(rix)      rix

static void BasicBlock_expand(CTX, kBasicBlock *bb, size_t newsize)
{
	kopl_t* newbuf = (kopl_t*)KNH_ZMALLOC(sizeof(kopl_t) * newsize);
	memcpy(newbuf, DP(bb)->opbuf, DP(bb)->capacity * sizeof(kopl_t));
	KNH_FREE(DP(bb)->opbuf, DP(bb)->capacity * sizeof(kopl_t));
	DP(bb)->opbuf = newbuf;
	DP(bb)->capacity = newsize;
}

static void BasicBlock_add(CTX, kBasicBlock *bb, kushort_t line, kopl_t *op, size_t size)
{
	if(DP(bb)->capacity == 0) {
		DP(bb)->opbuf = (kopl_t*)KNH_ZMALLOC(sizeof(kopl_t));
		DP(bb)->capacity = 1;
	}
	else if(DP(bb)->capacity == 1) {
		BasicBlock_expand(_ctx, bb, 4);
	}
	else if(DP(bb)->size == DP(bb)->capacity) {
		BasicBlock_expand(_ctx, bb, DP(bb)->capacity * 2);
	}
	{
		kopl_t *pc = DP(bb)->opbuf + DP(bb)->size;
		memcpy(pc, op, size == 0 ? sizeof(kopl_t) : size);
#ifdef K_USING_RCGC
		kopl_reftrace(_ctx, op FTRDATA);
		knh_traverse_refs(_ctx, RCinc);
#endif
		pc->line = line;
		DP(bb)->size += 1;
	}
}

static void BUILD_asm(CTX, kopl_t *op, size_t opsize)
{
	kBasicBlock *bb = kcodemod->bbNC;
	DBG_ASSERT(op->opcode != OPCODE_JMPF);
	BasicBlock_add(_ctx, bb, kcodemod->uline, op, opsize);
}

static int BUILD_asmJMPF(CTX, klr_JMPF_t *op)
{
	kBasicBlock *bb = kcodemod->bbNC;
	DBG_ASSERT(op->opcode == OPCODE_JMPF);
	int swap = 0;
	while(DP(bb)->size > 0) {
#ifdef _CLASSICVM
		kopl_t *opP = DP(bb)->opbuf + (DP(bb)->size - 1);
		if(opP->opcode == OPbNOT) {
			kopbNOT_t *opN = (kopbNOT_t*)opP;
//			DBG_P("REWRITE JMPF index %d => %d", op->a, opN->a);
			op->a = opN->a;
			swap = (swap == 0) ? 1 : 0;
			DP(bb)->size -= 1;
			continue;
		}
		if(OPiEQ <= opP->opcode && opP->opcode <= OPiGTE && HAS_OPCODE(iJEQ)) {
			kopiJEQ_t *opN = (kopiJEQ_t*)opP;
			ksfpidx_t a = ((kopiEQ_t*)opP)->a;
			ksfpidx_t b = ((kopiEQ_t*)opP)->b;
			opN->jumppc = (op)->jumppc;
			opN->a = a; opN->b = b;
			opP->opcode = OPiJEQ + ((opP)->opcode - OPiEQ);
			return swap;
		}
		if(OPiEQC <= opP->opcode && opP->opcode <= OPiGTEC && HAS_OPCODE(iJEQC)) {
			kopiJEQC_t *opN = (kopiJEQC_t*)opP;
			ksfpidx_t a = ((kopiEQC_t*)opP)->a;
			kint_t n = ((kopiEQC_t*)opP)->n;
			opN->jumppc = (op)->jumppc;
			opN->a = a; opN->n = n;
			opP->opcode = OPiJEQC + ((opP)->opcode - OPiEQC);
			return swap;
		}
		if(OPfEQ <= opP->opcode && opP->opcode <= OPfGTE && HAS_OPCODE(fJEQ)) {
			kopfJEQ_t *opN = (kopfJEQ_t*)opP;
			ksfpidx_t a = ((kopfEQ_t*)opP)->a;
			ksfpidx_t b = ((kopfEQ_t*)opP)->b;
			opN->jumppc = (op)->jumppc;
			opN->a = a; opN->b = b;
			opP->opcode = OPfJEQ + ((opP)->opcode - OPfEQ);
			return swap;
		}
		if(OPfEQC <= opP->opcode && opP->opcode <= OPfGTEC && HAS_OPCODE(fJEQC)) {
			kopfJEQC_t *opN = (kopfJEQC_t*)opP;
			ksfpidx_t a = ((kopfEQC_t*)opP)->a;
			kfloat_t n = ((kopfEQC_t*)opP)->n;
			opN->jumppc = (op)->jumppc;
			opN->a = a; opN->n = n;
			opP->opcode = OPfJEQC + ((opP)->opcode - OPfEQC);
			return swap;
		}
		break;
#endif
	}
	BasicBlock_add(_ctx, bb, kcodemod->uline, (kopl_t*)op, 0);
	return swap;
}

/* ------------------------------------------------------------------------ */
/* new_KonohaCode */

static inline kopcode_t BasicBlock_opcode(kBasicBlock *bb)
{
	if(DP(bb)->size == 0) return OPCODE_NOP;
	return DP(bb)->opbuf->opcode;
}

//#define BB(bb)   T_opcode(BasicBlock_opcode(bb))
//
//static void dumpBB(kBasicBlock *bb, const char *indent)
//{
//	size_t i;
////	DBG_P("%sid=%i, size=%d", indent, DP(bb)->id, DP(bb)->size);
//	if(bb->nextNC != NULL) {
////		DBG_P("%s\tnext=%d", indent, DP(bb->nextNC)->id);
//		if(indent[0] == 0) dumpBB(bb->nextNC, "\t");
//	}
//	if(bb->jumpNC != NULL) {
////		DBG_P("%s\tjump=%d", indent, DP(bb->jumpNC)->id);
//		if(indent[0] == 0) dumpBB(bb->jumpNC, "\t");
//	}
//	for(i = 0; i < DP(bb)->size; i++) {
//		kopl_t *op = DP(bb)->opbuf + i;
////		DBG_P("%s\t opcode=%s", indent, T_opcode(op->opcode));
//		(void)op;
//	}
//}

static void BasicBlock_strip0(CTX, kBasicBlock *bb)
{
	L_TAIL:;
	if(BasicBlock_isVisited(bb)) return;
	BasicBlock_setVisited(bb, 1);
	if(bb->jumpNC != NULL) {
		L_JUMP:;
		kBasicBlock *bbJ = bb->jumpNC;
		if(DP(bbJ)->size == 0 && bbJ->jumpNC != NULL && bbJ->nextNC == NULL) {
			//DBG_P("DIRECT JMP id=%d JMP to id=%d", DP(bbJ)->id, DP(bbJ->jumpNC)->id);
			DP(bbJ)->incoming -= 1;
			bb->jumpNC = bbJ->jumpNC;
			DP(bb->jumpNC)->incoming += 1;
			goto L_JUMP;
		}
		if(DP(bbJ)->size == 0 && bbJ->jumpNC == NULL && bbJ->nextNC != NULL) {
			//DBG_P("DIRECT JMP id=%d NEXT to id=%d", DP(bbJ)->id, DP(bbJ->nextNC)->id);
			DP(bbJ)->incoming -= 1;
			bb->jumpNC = bbJ->nextNC;
			DP(bb->jumpNC)->incoming += 1;
			goto L_JUMP;
		}
		if(bb->nextNC == NULL) {
			if(DP(bbJ)->incoming == 1 ) {
				//DBG_P("REMOVED %d JMP TO %d", DP(bb)->id, DP(bbJ)->id);
				bb->nextNC = bbJ;
				bb->jumpNC = NULL;
				goto L_NEXT;
			}
		}
		BasicBlock_strip0(_ctx, bbJ);
	}
	if(bb->jumpNC != NULL && bb->nextNC != NULL) {
		bb = bb->nextNC;
		goto L_TAIL;
	}
	L_NEXT:;
	if(bb->nextNC != NULL) {
		kBasicBlock *bbN = bb->nextNC;
		if(DP(bbN)->size == 0 && bbN->nextNC != NULL && bbN->jumpNC == NULL) {
			//DBG_P("DIRECT NEXT id=%d to NEXT id=%d", DP(bbN)->id, DP(bbN->nextNC)->id);
			DP(bbN)->incoming -= 1;
			bb->nextNC = bbN->nextNC;
			DP(bb->nextNC)->incoming += 1;
			goto L_NEXT;
		}
		if(DP(bbN)->size == 0 && bbN->nextNC == NULL && bbN->jumpNC != NULL) {
			//DBG_P("DIRECT NEXT id=%d to JUMP id=%d", DP(bbN)->id, DP(bbN->jumpNC)->id);
			DP(bbN)->incoming -= 1;
			bb->nextNC = NULL;
			bb->jumpNC = bbN->jumpNC;
			DP(bb->jumpNC)->incoming += 1;
			goto L_JUMP;
		}
		bb = bb->nextNC;
		goto L_TAIL;
	}
}

static void BasicBlock_freebuf(CTX, kBasicBlock *bb)
{
	KNH_FREE(DP(bb)->opbuf, sizeof(kopl_t) * DP(bb)->capacity);
	DP(bb)->capacity = 0;
	DP(bb)->size = 0;
	DP(bb)->opbuf = NULL;
}

static void BasicBlock_join(CTX, kBasicBlock *bb, kBasicBlock *bbN)
{
	//DBG_P("join %d(%s) size=%d and %d(%s) size=%d", DP(bb)->id, BB(bb), DP(bb)->size, DP(bbN)->id, BB(bbN), DP(bbN)->size);
	bb->nextNC = bbN->nextNC;
	bb->jumpNC = bbN->jumpNC;
	if(DP(bbN)->size == 0) {
		return;
	}
	if(DP(bb)->size == 0) {
		DBG_ASSERT(DP(bb)->capacity == 0);
		DP(bb)->opbuf = DP(bbN)->opbuf;
		DP(bb)->capacity = DP(bbN)->capacity;
		DP(bb)->size = DP(bbN)->size;
		DP(bbN)->opbuf = NULL;
		DP(bbN)->capacity = 0;
		DP(bbN)->size = 0;
		return;
	}
	if(DP(bb)->capacity < DP(bb)->size + DP(bbN)->size) {
		BasicBlock_expand(_ctx, bb, DP(bb)->size + DP(bbN)->size);
	}
	memcpy(DP(bb)->opbuf + DP(bb)->size, DP(bbN)->opbuf, sizeof(kopl_t) * DP(bbN)->size);
	DP(bb)->size += DP(bbN)->size;
	BasicBlock_freebuf(_ctx, bbN);
}

static void BasicBlock_strip1(CTX, kBasicBlock *bb)
{
	L_TAIL:;
	if(!BasicBlock_isVisited(bb)) return;
	BasicBlock_setVisited(bb, 0);  // MUST call after strip0
	if(bb->jumpNC != NULL) {
		if(bb->nextNC == NULL) {
			bb = bb->jumpNC;
			goto L_TAIL;
		}
		else {
			//DBG_P("** branch next=%d, jump%d", DP(bb->nextNC)->id, DP(bb->jumpNC)->id);
			BasicBlock_strip1(_ctx, bb->jumpNC);
			bb = bb->nextNC;
			goto L_TAIL;
		}
	}
	if(bb->nextNC != NULL) {
		kBasicBlock *bbN = bb->nextNC;
		if(DP(bbN)->incoming == 1 && BasicBlock_opcode(bbN) != OPCODE_RET) {
			BasicBlock_join(_ctx, bb, bbN);
			BasicBlock_setVisited(bb, 1);
			goto L_TAIL;
		}
		bb = bb->nextNC;
		goto L_TAIL;
	}
}

#define _REMOVE(opX)   opX->opcode = OPNOP; bbsize--; continue;
#define _REMOVE2(opX, opX2)   opX->opcode = OPNOP; opX2->opcode = OPNOP; bbsize -= 2; continue;
#define _REMOVE3(opX, opX2, opX3)   opX->opcode = OPNOP; opX2->opcode = OPNOP; opX3->opcode = OPNOP; bbsize -= 3; continue;

static size_t BasicBlock_peephole(CTX, kBasicBlock *bb)
{
	size_t i, bbsize = DP(bb)->size;
	for(i = 0; i < DP(bb)->size; i++) {
		kopl_t *op = DP(bb)->opbuf + i;
		if(op->opcode == OPCODE_NOP) {
			bbsize--;
		}
	}
#ifdef _CLASSICVM
	for(i = 1; i < DP(bb)->size; i++) {
		kopl_t *opP = DP(bb)->opbuf + (i - 1);
		kopl_t *op = DP(bb)->opbuf + i;
		if((op->opcode == OPfCAST || op->opcode == OPiCAST) && opP->opcode == OPNMOV) {
			kopfCAST_t *opCAST = (kopfCAST_t*)op;
			kopNMOV_t *opNMOV = (kopNMOV_t*)opP;
			if(opNMOV->a == opCAST->b && opCAST->a == opCAST->b) {
				opCAST->b = opNMOV->b;
				_REMOVE(opP);
			}
		}
		if (op->opcode == OPNMOV || op->opcode == OPOMOV) {
			kopNMOV_t *opNMOV = (kopNMOV_t *) op;
			if(opNMOV->a == opNMOV->b) {
				_REMOVE(op);
			}
		}
		if(opP->opcode == OPNSET && op->opcode == OPNSET) {
			kopNSET_t *op1 = (kopNSET_t*)opP;
			kopNSET_t *op2 = (kopNSET_t*)op;
			if(op1->a + K_NEXTIDX != op2->a) continue;
			if(sizeof(uintptr_t) == sizeof(kuint_t)) {
				kopNSET_t *op3 = (kopNSET_t*)(DP(bb)->opbuf + i + 1);
				kopNSET_t *op4 = (kopNSET_t*)(DP(bb)->opbuf + i + 2);
				if(op3->opcode != OPNSET || op2->a + K_NEXTIDX != op3->a) goto L_NSET2;
				if(op4->opcode == OPNSET && op3->a + K_NEXTIDX == op4->a) {
					kopNSET4_t *opNSET = (kopNSET4_t*)opP;
					opNSET->opcode = OPNSET4;
					opNSET->n2 = op2->n;
					opNSET->n3 = op3->n;
					opNSET->n4 = op4->n;
					_REMOVE3(op2, op3, op4);
				}
				else {
					kopNSET3_t *opNSET = (kopNSET3_t*)opP;
					opNSET->opcode = OPNSET3;
					opNSET->n2 = op2->n;
					opNSET->n3 = op3->n;
					_REMOVE2(op2, op3);
				}
			}
			L_NSET2:;
			kopNSET2_t *opNSET = (kopNSET2_t*)opP;
			opNSET->opcode = OPNSET2;
			opNSET->n2 = op2->n;
			_REMOVE(op2);
		}
		if(opP->opcode == OPOSET && op->opcode == OPOSET) {
			kopOSET_t *op1 = (kopOSET_t*)opP;
			kopOSET_t *op2 = (kopOSET_t*)op;
			if(op1->a + K_NEXTIDX != op2->a) continue;
			{
				kopOSET_t *op3 = (kopOSET_t*)(DP(bb)->opbuf + i + 1);
				kopOSET_t *op4 = (kopOSET_t*)(DP(bb)->opbuf + i + 2);
				if(op3->opcode != OPOSET || op2->a + K_NEXTIDX != op3->a) goto L_OSET2;
				if(op4->opcode == OPOSET && op3->a + K_NEXTIDX == op4->a) {
					kopOSET4_t *opOSET = (kopOSET4_t*)opP;
					opOSET->opcode = OPOSET4;
					opOSET->v2 = op2->o;
					opOSET->v3 = op3->o;
					opOSET->v4 = op4->o;
					_REMOVE3(op2, op3, op4);
				}
				else {
					kopOSET3_t *opOSET = (kopOSET3_t*)opP;
					opOSET->opcode = OPOSET3;
					opOSET->v2 = op2->o;
					opOSET->v3 = op3->o;
					_REMOVE2(op2, op3);
				}
			}
			L_OSET2:;
			kopOSET2_t *opOSET = (kopOSET2_t*)opP;
			opOSET->opcode = OPOSET2;
			opOSET->v2 = op2->o;
			_REMOVE(op2);
		}
		if(op->opcode == OPNMOV) {
#ifdef OPNNMOV
			if(opP->opcode == OPNMOV && HAS_OPCODE(NNMOV)) {
				kopNNMOV_t *opMOV = (kopNNMOV_t*)opP;
				opMOV->c = ((kopNMOV_t*)op)->a;
				opMOV->d = ((kopNMOV_t*)op)->b;
				opP->opcode = OPNNMOV;
				_REMOVE(op);
			}
			if(opP->opcode == OPOMOV && HAS_OPCODE(ONMOV)) {
				kopONMOV_t *opMOV = (kopONMOV_t *)opP;
				opMOV->c = ((kopNMOV_t*)op)->a;
				opMOV->d = ((kopNMOV_t*)op)->b;
				opP->opcode = OPONMOV;
				_REMOVE(op);
			}
#endif
		}
		if(op->opcode == OPOMOV) {
#ifdef OPOOMOV
			if(opP->opcode == OPOMOV && HAS_OPCODE(OOMOV)) {
				kopOOMOV_t *opMOV = (kopOOMOV_t*)opP;
				opMOV->c = ((kopOMOV_t*)op)->a;
				opMOV->d = ((kopOMOV_t*)op)->b;
				opP->opcode = OPOOMOV;
				_REMOVE(op);
			}
			if(opP->opcode == OPOMOV && HAS_OPCODE(ONMOV)) {
				kopONMOV_t *opMOV = (kopONMOV_t *)opP;
				opMOV->c = opMOV->a;
				opMOV->d = opMOV->b;
				opMOV->a = ((kopOMOV_t*)op)->a;
				opMOV->b = ((kopOMOV_t*)op)->b;
				opP->opcode = OPONMOV;
				_REMOVE(op);
			}
#endif
		}
	}
#endif
	if(bbsize < DP(bb)->size) {
		kopl_t *opD = DP(bb)->opbuf;
		for(i = 0; i < DP(bb)->size; i++) {
			kopl_t *opS = DP(bb)->opbuf + i;
			if(opS->opcode == OPCODE_NOP) continue;
			if(opD != opS) {
				*opD = *opS;
			}
			opD++;
		}
		DP(bb)->size = bbsize;
	}
	return DP(bb)->size; /*bbsize*/;
}

#define BB_(bb)   (bb != NULL) ? DP(bb)->id : -1

static size_t BasicBlock_size(CTX, kBasicBlock *bb, size_t c)
{
	L_TAIL:;
	if(bb == NULL || BasicBlock_isVisited(bb)) return c;
	BasicBlock_setVisited(bb, 1);
	if(bb->nextNC != NULL) {
		if(BasicBlock_isVisited(bb) || BasicBlock_opcode(bb->nextNC) == OPCODE_RET) {
			kBasicBlock *bb2 = new_BasicBlockLABEL(_ctx);
			bb2->jumpNC = bb->nextNC;
			bb->nextNC = bb2;
		}
	}
	if(bb->jumpNC != NULL && bb->nextNC != NULL) {
		DBG_ASSERT(bb->jumpNC != bb->nextNC);
		c = BasicBlock_size(_ctx, bb->nextNC, c + BasicBlock_peephole(_ctx, bb));
		bb = bb->jumpNC; goto L_TAIL;
	}
	if(bb->jumpNC != NULL) { DBG_ASSERT(bb->nextNC == NULL);
//		if(BasicBlock_opcode(bb->jumpNC) == OPRET) {
//			kBasicBlock_add(bb, JMP_);
//		}
//		else {
			kBasicBlock_add(bb, JMP);
//		}
		c = BasicBlock_peephole(_ctx, bb) + c;
		bb = bb->jumpNC;
		goto L_TAIL;
	}
	c = BasicBlock_peephole(_ctx, bb) + c;
	bb = bb->nextNC;
	goto L_TAIL;
}

static kopl_t* BasicBlock_copy(CTX, kopl_t *dst, kBasicBlock *bb, kBasicBlock **prev)
{
	BasicBlock_setVisited(bb, 0);
	DBG_ASSERT(!BasicBlock_isVisited(bb));
//	DBG_P("BB%d: asm nextNC=BB%d, jumpNC=BB%d", BB_(bb), BB_(bb->nextNC), BB_(bb->jumpNC));
	if(DP(bb)->code != NULL) {
		//DBG_P("BB%d: already copied", BB_(bb));
		return dst;
	}
	if(prev[0] != NULL && prev[0]->nextNC == NULL && prev[0]->jumpNC == bb) {
		dst -= 1;
		//DBG_P("BB%d: REMOVE unnecessary JMP/(?%s)", BB_(bb), T_opcode(dst->opcode));
		DBG_ASSERT(dst->opcode == OPCODE_JMP/* || dst->opcode == OPJMP_*/);
		prev[0]->jumpNC = NULL;
		prev[0]->nextNC = bb;
	}
	DP(bb)->code = dst;
	if(DP(bb)->size > 0) {
		memcpy(dst, DP(bb)->opbuf, sizeof(kopl_t) * DP(bb)->size);
		if(bb->jumpNC != NULL) {
			DP(bb)->opjmp = (dst + (DP(bb)->size - 1));
//			DBG_ASSERT(kOPhasjump(DP(bb)->opjmp->opcode));
		}
#ifdef _CLASSICVM
		size_t i;
		for(i = 0; i < DP(bb)->size; i++) {
			kopl_t *op = dst + i;
			if(op->opcode == OPVCALL) {
				if(BasicBlock_isStackChecked(bb)) {
					op->opcode = OPVCALL_;
				}
				else {
					BasicBlock_setStackChecked(bb, 1);
				}
			}
			if(op->opcode == OPiADDC) {
				kopiADDC_t *opN = (kopiADDC_t*)op;
				if(opN->a == opN->c && opN->n == 1) {
					op->opcode = OPiINC;
				}
			}
			if(op->opcode == OPiSUBC) {
				kopiSUBC_t *opN = (kopiSUBC_t*)op;
				if(opN->a == opN->c && opN->n == 1) {
					op->opcode = OPiDEC;
				}
			}
//			DBG_P("BB%d: [%ld] %s", BB_(bb), i, T_opcode(op->opcode));
		}
#endif
		dst = dst + DP(bb)->size;
		BasicBlock_freebuf(_ctx, bb);
		prev[0] = bb;
	}
	if(bb->nextNC != NULL) {
		//DBG_P("BB%d: NEXT=BB%d", BB_(bb), BB_(bb->nextNC));
		DBG_ASSERT(DP(bb->nextNC)->code == NULL);
//		if(BasicBlock_isStackChecked(bb) && DP(bb->nextNC)->incoming == 1) {
//			BasicBlock_setStackChecked(bb->nextNC, 1);
//		}
		dst = BasicBlock_copy(_ctx, dst, bb->nextNC, prev);
	}
	if(bb->jumpNC != NULL) {
		//DBG_P("BB%d: JUMP=%d", DP(bb)->id, BB_(bb->jumpNC));
//		if(BasicBlock_isStackChecked(bb) && DP(bb->jumpNC)->incoming == 1) {
//			BasicBlock_setStackChecked(bb->jumpNC, 1);
//		}
		dst = BasicBlock_copy(_ctx, dst, bb->jumpNC, prev);
	}
	return dst;
}

static void BasicBlock_setjump(kBasicBlock *bb)
{
	while(bb != NULL) {
		BasicBlock_setVisited(bb, 1);
		if(bb->jumpNC != NULL) {
			kBasicBlock *bbJ = bb->jumpNC;
			klr_JMP_t *j = (klr_JMP_t*)DP(bb)->opjmp;
			j->jumppc = DP(bbJ)->code;
			//DBG_P("jump from id=%d to id=%d %s jumppc=%p", DP(bb)->id, DP(bbJ)->id, T_opcode(j->opcode), DP(bbJ)->code);
			bb->jumpNC = NULL;
			if(!BasicBlock_isVisited(bbJ)) {
				BasicBlock_setVisited(bbJ, 1);
				BasicBlock_setjump(bbJ);
			}
		}
		bb = bb->nextNC;
	}
}

static kKonohaCode* new_KonohaCode(CTX, kBasicBlock *bb, kBasicBlock *bbRET)
{
	kKonohaCode *kcode = new_(KonohaCode, NULL);
	kBasicBlock *prev[1] = {};
	kcode->uri = kcodemod->uline; //TODO
	kcode->codesize = BasicBlock_size(_ctx, bb, 0) * sizeof(kopl_t);
	kcode->code = (kopl_t*)KNH_ZMALLOC(kcode->codesize);
	DP(bbRET)->code = kcode->code; // dummy
	{
		kopl_t *op = BasicBlock_copy(_ctx, kcode->code, bb, prev);
		DBG_ASSERT(op - kcode->code > 0);
		DP(bbRET)->code = NULL;
		BasicBlock_copy(_ctx, op, bbRET, prev);
		BasicBlock_setjump(bb);
	}
	return kcode;
}

/* ------------------------------------------------------------------------ */

static void dumpOPCODE(CTX, kopl_t *c, kopl_t *pc_start)
{
	size_t i, size = OPDATA[c->opcode].size;
	const kushort_t *vmt = OPDATA[c->opcode].types;
	if(pc_start == NULL) {
		fprintf(stdout, "[%p:%d]\t%s(%d)", c, c->line, T_opcode(c->opcode), (int)c->opcode);
	}
	else {
		fprintf(stdout, "[L%d:%d]\t%s(%d)", (int)(c - pc_start), c->line, T_opcode(c->opcode), (int)c->opcode);
	}
	for(i = 0; i < size; i++) {
		fprintf(stdout, " ");
		switch(vmt[i]) {
		case VMT_VOID: break;
		case VMT_ADDR:
			if(pc_start == NULL) {
				fprintf(stdout, "%p", c->p[i]);
			}
			else {
				fprintf(stdout, "L%d", (int)((kopl_t*)c->p[i] - pc_start));
			}
			break;
		case VMT_R:
			fprintf(stdout, "sfp[%d,r=%d]", (int)c->data[i]/2, (int)c->data[i]);
			break;
		case VMT_U:
			fprintf(stdout, "u%lu", c->data[i]); break;
		case VMT_I:
			fprintf(stdout, "i%ld", c->data[i]); break;
		case VMT_F:
			fprintf(stdout, "function(%p)", c->p[i]); break;
		case VMT_CID:
			fprintf(stdout, "CT(%s)", S_text(((kclass_t*)c->data[i])->name)); break;
//		case VMT_HCACHE: {
//			kcachedata_t *hc = (kcachedata_t*)&(c->p[i]);
//			knh_write_cname(_ctx, w, hc->cid);
//			kwb_putc(wb, '/');
//			knh_write_mn(_ctx, w, hc->mn);
//		}
//		break;
//		case VMT_OBJECT: {
//			knh_write_Object(_ctx, w, UPCAST(c->p[i]), FMT_line);
//			break;
//		}
//		case VMT_INT: {
//			kint_t n = ((kint_t*)(&(c->p[i])))[0];
//			knh_write_ifmt(_ctx, w, KINT_FMT, n); break;
//		}
//		case VMT_FLOAT:
//			knh_write_ffmt(_ctx, w, KFLOAT_FMT, *((kfloat_t*)&(c->p[i]))); break;
//		}
		}/*switch*/
	}
	fprintf(stdout, "\n");
}

KMETHOD Fmethod_runVM(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(K_RIX == K_RTNIDX);
	DBG_ASSERT(IS_Method(sfp[K_MTDIDX].mtdNC));
	VirtualMachine_run(_ctx, sfp, CODE_ENTER);
}

static void Method_threadCode(CTX, kMethod *mtd, kKonohaCode *kcode)
{
	kMethod_setFunc(mtd, Fmethod_runVM);
	KSETv(mtd->kcode, kcode);
	(mtd)->pc_start = VirtualMachine_run(_ctx, _ctx->esp + 1, kcode->code);
	DBG_P("DUMP CODE");
	kopl_t *pc = mtd->pc_start;
	while(pc->opcode != OPCODE_RET) {
		dumpOPCODE(_ctx, pc, mtd->pc_start);
		pc++;
	}
}

static void BUILD_compile(CTX, kMethod *mtd, kBasicBlock *bb, kBasicBlock *bbRET)
{
	BasicBlock_strip0(_ctx, bb);
	BasicBlock_strip1(_ctx, bb);
	kKonohaCode *kcode = new_KonohaCode(_ctx, bb, bbRET);
	Method_threadCode(_ctx, mtd, kcode);
	kArray_clear(kcodemod->lstacks, 0);
	kArray_clear(kcodemod->insts, 0);
}

static void ASM_LABEL(CTX, kBasicBlock *label)
{
	if(label != NULL) {
		kBasicBlock *bb = kcodemod->bbNC;
		if(bb != NULL) {
			bb->nextNC = label;
			DP(label)->incoming += 1;
			bb->jumpNC = NULL;
		}
		kcodemod->bbNC = label;
	}
}

static void ASM_JMP(CTX, kBasicBlock *label)
{
	kBasicBlock *bb = kcodemod->bbNC;
	if(bb != NULL) {
		bb->nextNC = NULL;
		bb->jumpNC = label;  DP(label)->incoming += 1;
	}
	kcodemod->bbNC = NULL; /*KNH_TNULL(BasicBlock);*/
}

static kBasicBlock* ASM_JMPF(CTX, int flocal, kBasicBlock *lbJUMP)
{
	kBasicBlock *bb = kcodemod->bbNC;
	kBasicBlock *lbNEXT = new_BasicBlockLABEL(_ctx);
	klr_JMPF_t op = {TADDR, OPCODE_JMPF, ASMLINE, NULL, NC_(flocal)};
	if(BUILD_asmJMPF(_ctx, &op)) {
		bb->jumpNC = lbNEXT;
		bb->nextNC = lbJUMP;
	}
	else {
		bb->jumpNC = lbJUMP;
		bb->nextNC = lbNEXT;
	}
	DP(lbJUMP)->incoming += 1;
	DP(lbNEXT)->incoming += 1;
	kcodemod->bbNC = lbNEXT;
	return lbJUMP;
}

static kBasicBlock* EXPR_asmJMPIF(CTX, int a, kExpr *expr, int isTRUE, kBasicBlock* label, int espidx)
{
	EXPR_asm(_ctx, a, expr, espidx);
	if(isTRUE) {
		ASM(BNOT, NC_(a), NC_(a));
	}
	return ASM_JMPF(_ctx, a, label);
}

//static void ASM_BRANCH_(CTX, kBasicBlock *jump, kopl_t *op, size_t size)
//{
//	kBasicBlock *bb = kcodemod->bbNC;
//	kBasicBlock *newbb = new_BasicBlockLABEL(_ctx);
//	BUILD_asm(_ctx, op, size);
//	bb->jumpNC = jump;  DP(jump)->incoming += 1;
//	bb->nextNC = newbb;  DP(newbb)->incoming += 1;
//	kcodemod->bbNC = newbb;
//}

//static void _LOOKUPMTD(CTX, ksfp_t *sfp, struct kopLDMTD_t *op)
//{
//	size_t thisidx = op->thisidx / 2;
//	const kclass_t *ct = O_ct(sfp[thisidx].o);
//	if(op->cache.cid != ct->cid) {
//		kMethod *mtd = ClassTBL_getMethod(_ctx, ct, op->cache.mn);
//		op->mtdNC = mtd;
//		op->cache.cid = ct->cid;
//	}
//	sfp[thisidx+K_MTDIDX].mtdNC = op->mtdNC;
//}


/* ------------------------------------------------------------------------ */
/* [GammaBuilder] */

//static void ASM_SAFEPOINT(CTX, int espidx)
//{
//#ifdef K_USING_SAFEPOINT
//	kBasicBlock *bb = kcodemod->bbNC;
//	size_t i;
//	for(i = 0; i < DP(bb)->size; i++) {
//		kopl_t *op = DP(bb)->opbuf + i;
//		if(op->opcode == OPSAFEPOINT) return;
//	}
//	ASM(SAFEPOINT, SFP_(espidx));
//#endif
//}

/* ------------------------------------------------------------------------ */
/* CALL */

//static inline kint_t Tn_int(kStmtExpr *stmt, size_t n)
//{
//	kTerm *tk = tkNN(stmt, n);
//	DBG_ASSERT(TT_(tk) == TT_CONST);
//	return ((tk)->num)->n.ivalue;
//}
//
//static inline kfloat_t Tn_float(kStmtExpr *stmt, size_t n)
//{
//	kTerm *tk = tkNN(stmt, n);
//	DBG_ASSERT(TT_(tk) == TT_CONST);
//	return ((tk)->num)->n.fvalue;
//}

//static int CALLPARAMs_asm(CTX, kStmtExpr *stmt, size_t s, int local, kcid_t cid, kMethod *mtd)
//{
//	size_t i;
//	if(s == 1 && Method_isStatic(mtd)) s = 2;   // ignoring static caller, like Script
//	if(DP(stmt)->size + DP(stmt)->espidx > 32) {
//		ASM(CHKSTACK, SFP_(DP(stmt)->size + DP(stmt)->espidx));
//	}
//	for(i = s; i < DP(stmt)->size; i++) {
//		Tn_asm(_ctx, stmt, i, local + i + (K_CALLDELTA-1));
//	}
//	// TODO(@imasahiro)
//	if(Stmt_isTAILRECURSION(stmt) && BUILD_isTailRecursion(_ctx->gma)) {
//		kBasicBlock *lbBEGIN = GammaBuilderLabel(_ctx, 1);
//		for(i = s; i < DP(stmt)->size; i++) {
//			ktype_t reqt = Tn_type(stmt, i); //Tn_ptype(_ctx, stmt, i, cid, mtd);
//			if(TY_isUnbox(reqt)) {
//				ASM(NMOV, NC_(i-1), NC_(local + i + (K_CALLDELTA-1)));
//			}
//			else {
//				ASM(OMOV, OC_(i-1), OC_(local + i + (K_CALLDELTA-1)));
//			}
//		}
//		ASM_JMP(_ctx, lbBEGIN);
//		return 0;
//	}
//	return 1;
//}

//static void ASM_INLINE(CTX, int sfpshift, kopl_t *code, size_t isize)
//{
//	size_t i, last = isize;
//	kBasicBlock* bb[K_INLINECODE];
//	for(i = 0; i < isize; i++) {
//		bb[i] = new_BasicBlockLABEL(_ctx);
//		bb[i]->nextNC = NULL;
//		bb[i]->jumpNC = NULL;
//		if(code[i].opcode == OPRET) {
//			last = i; break;
//		}
//	}
//	KNH_ASSERT(last != isize);
//	ASM_LABEL(_ctx, bb[0]);
//	for(i = 0; i < last; i++) {
//		kopl_t opbuf, *op;
//		opbuf = code[i]; op = &opbuf;
//		knh_OPshift(op, sfpshift);
//		if(op->opcode == OPJMP_) {
//			op->opcode = OPJMP;
//		}
//		if(op->opcode != OPJMP) {
//			BasicBlock_add(_ctx, bb[i], op->line, op, 0);
//			bb[i]->nextNC = bb[i+1];
//			DP(bb[i+1])->incoming += 1;
//		}
//		if(kOPhasjump(op->opcode)) {
//			int jmpidx = code - (kopl_t*)(op->p[0]);
//			if (jmpidx < 0) jmpidx = -jmpidx;
//			DBG_ASSERT(jmpidx < (int)isize);
//			bb[i]->jumpNC = bb[jmpidx];
//			DP(bb[jmpidx])->incoming += 1;
//			op->p[0] = NULL;
//		}
//	}
//	kcodemod->bbNC = bb[last];
//	DBG_ASSERT(DP(bb[last])->incoming > 0);
//}

#define ESP_(sfpidx, args)   SFP_(sfpidx + args + K_CALLDELTA + 1)

//static kreg_t RTNIDX_(CTX, int sfpidx, ktype_t rtype)
//{
//	if(rtype == TY_void) return -1;
//	if(TY_isUnbox(rtype)) return NC_(sfpidx);
//	return OC_(sfpidx);
//}

//static void ASM_CALL(CTX, int espidx, ktype_t rtype, kMethod *mtd, int isStatic, size_t argc)
//{
//	int thisidx = espidx + K_CALLDELTA;
//	DBG_ASSERT(IS_Method(mtd));
//	if(Method_isFinal(mtd) || isStatic) {
//		if(Method_isKonohaCode(mtd) || DP(_ctx->gma)->mtd == mtd) {
//			if(BUILD_isInlineFunction(_ctx->gma) && DP(_ctx->gma)->mtd != mtd) {
//				kKonohaCode *kcode = DP(mtd)->kcode;
//				size_t isize = kcode->codesize / sizeof(kopl_t);
//				if(isize < K_INLINECODE) {
//					NoticeInliningMethod(_ctx, mtd);
//					if(isize-1 > 0) {
//						ASM_INLINE(_ctx, espidx + K_CALLDELTA, kcode->code + 1, isize - 1);
//					}
//					return;
//				}
//			}
//			if(argc == 0) {
//				ASM(VCALL_, RTNIDX_(_ctx, espidx, rtype), SFP_(thisidx), ESP_(espidx, argc), mtd);
//			}
//			else {
//				ASM(VCALL, RTNIDX_(_ctx, espidx, rtype), SFP_(thisidx), ESP_(espidx, argc), mtd);
//			}
//		}
//		else {
//			ASM(SCALL, RTNIDX_(_ctx, espidx, rtype), SFP_(thisidx), ESP_(espidx, argc), mtd);
//		}
//	}
//	else {
//		ASM(LDMTD, SFP_(thisidx), _LOOKUPMTD, {mtd->cid, mtd->mn}, mtd);
//		ASM(CALL, RTNIDX_(_ctx, espidx, rtype), SFP_(thisidx), ESP_(espidx, argc));
//	}
//}

//static void ASM_CHKIDX(CTX, int aidx, int nidx)
//{
#ifdef OPCHKIDX
	long i;
	kBasicBlock *bb = kcodemod->bbNC;
	for(i = (long)DP(bb)->size - 1; i >= 0; i--) {
		kopCHKIDX_t *op = (kopCHKIDX_t*)(DP(bb)->opbuf + i);
		kOPt opcode = op->opcode;
		if(opcode == OPCHKIDXC && op->a == aidx && op->n == nidx) {
			return;
		}
		if(OPSCALL <= opcode && opcode <= OPVCALL_) break;
	}
	ASM(CHKIDX, aidx, nidx);
#endif
//}

//static void ASM_CHKIDXC(CTX, int aidx, int n)
//{
#ifdef OPCHKIDX
	kBasicBlock *bb = kcodemod->bbNC;
	long i;
	for(i = (long)DP(bb)->size - 1; i >= 0; i--) {
		kopCHKIDXC_t *op = (kopCHKIDXC_t*)(DP(bb)->opbuf + i);
		kOPt opcode = op->opcode;
		if(opcode == OPCHKIDXC && op->a == aidx) {
			if(op->n < (kuint_t) n) op->n = n;
			return;
		}
		if(OPSCALL <= opcode && opcode <= OPVCALL_) break;
	}
	ASM(CHKIDXC, aidx, n);
#endif
//}

//static int Tuple_index(CTX, kParam *pa, size_t n, size_t psize)
//{
#if defined(K_USING_DBLNDATA_)
	size_t i = 0, ti = 0;
	for(i = 0; i < psize; i++) {
		kparam_t *p = knh_Param_get(pa, i);
		if(i == n) return ti;
		if(TY_isUnbox(p->type)) ti+=2; else ti++;
	}
	return ti;
#else
//	return (int)n;
#endif
//}

//static void CALL_asmq(CTX, kExpr *expr, int espidx)
//{
//	kTerm *tkMTD = tkNN(expr, 0);
//	kMethod *mtd = (tkMTD)->mtd;
//	kcid_t cid = Tn_cid(expr, 1);
#ifdef _CLASSICVM
	if(!IS_Method(mtd) && C_bcid(cid) == CLASS_Tuple && (tkMTD->mn == MN_get || tkMTD->mn == MN_set)) {
		DBG_ASSERT(Tn_isCONST(expr, 2));
		kParam *pa = ClassTBL(cid)->cparam;
		int a = Tn_put(_ctx, expr, 1, espidx + 1);
		size_t psize = pa->psize;
		size_t n = tkNN(expr, 2)->index;
		int ti = Tuple_index(_ctx, pa, n, psize);
		kparam_t *p = knh_Param_get(pa, n);
		ksfx_t tx = {OC_(a), ti};
		if(tkMTD->mn == MN_get) {
			ASM_SMOVx(_ctx, espidx, p->type, tx);
		}
		else { /* mtd_mn == MN_set */
			int v = Tn_put(_ctx, expr, 3, espidx + 3);
			if(TY_isUnbox(p->type)) {
				ASM(XNMOV, tx, NC_(v));
			}
			else {
				ASM(XMOV, tx, OC_(v));
			}
		}
		return;
	}
	if(!IS_Method(mtd)) {
		size_t i;
		for(i = 1; i < DP(expr)->size; i++) {
			Tn_asm(_ctx, expr, i, espidx + i + (K_CALLDELTA-1));
		}
		ASM(LDMTD, SFP_(espidx+K_CALLDELTA), _DYNMTD, {TY_void, tkMTD->mn}, NULL);
		ASM(CALL, SFP_(espidx), SFP_(espidx+K_CALLDELTA), ESP_(espidx, DP(expr)->size - 2));
		ASM(PROBE, SFP2_(espidx), _PBOX, 0, 0);
		ASM_SAFEPOINT(_ctx, espidx+1);
		return;
	}
	kcid_t mtd_cid = (mtd)->cid;
	kmethodn_t mtd_mn = (mtd)->mn;
	if(mtd_cid == CLASS_Array) {
		kcid_t p1 = C_p1(cid);
		if(mtd_mn == MN_get) {
			int a = Tn_put(_ctx, expr, 1, espidx + 1);
			if(Tn_isCONST(expr, 2)) {
				intptr_t n = (intptr_t)Tn_int(expr, 2);
				if(n < 0) {
					goto L_USECALL;
				}
				ASM_CHKIDXC(_ctx, OC_(a), n);
				if(TY_isUnbox(p1)) {
					ASM(NGETIDXC, NC_(espidx), OC_(a), n);
				}
				else {
					ASM(OGETIDXC, OC_(espidx), OC_(a), n);
				}
			}
			else {
				int an = Tn_put(_ctx, expr, 2, espidx + 2);
				ASM_CHKIDX(_ctx, OC_(a), NC_(an));
				if(TY_isUnbox(p1)) {
					ASM(NGETIDX, NC_(espidx), OC_(a), NC_(an));
				}
				else {
					ASM(OGETIDX, OC_(espidx), OC_(a), NC_(an));
				}
			}
			return;
		}
		if(mtd_mn == MN_set) {
			int a = Tn_put(_ctx, expr, 1, espidx + 1);
			int v = Tn_put(_ctx, expr, 3, espidx + 3);
			if(Tn_isCONST(expr, 2)) {
				intptr_t n = (intptr_t)Tn_int(expr, 2);
				if(n < 0) {
					goto L_USECALL;
				}
				kcid_t p1 = C_p1(cid);
				ASM_CHKIDXC(_ctx, OC_(a), n);
				if(TY_isUnbox(p1)) {
					ASM(NSETIDXC, NC_(espidx), OC_(a), n, NC_(v));
				}
				else {
					ASM(OSETIDXC, OC_(espidx), OC_(a), n, OC_(v));
				}
			}
			else {
				int an = Tn_put(_ctx, expr, 2, espidx + 2);
				ASM_CHKIDX(_ctx, OC_(a), NC_(an));
				if(TY_isUnbox(p1)) {
					ASM(NSETIDX, NC_(espidx), OC_(a), NC_(an), NC_(v));
				}
				else {
					ASM(OSETIDX, OC_(espidx), OC_(a), NC_(an), OC_(v));
				}
			}
			return;
		}
	}
#if defined(OPBGETIDX)
	if(mtd_cid == CLASS_Bytes) {
		if(mtd_mn == MN_get) {
			int a = Tn_put(_ctx, expr, 1, espidx + 1);
			if(Tn_isCONST(expr, 2)) {
				intptr_t n = (intptr_t)Tn_int(expr, 2);
				ASM_CHKIDXC(_ctx, OC_(a), n);
				ASM(BGETIDXC, NC_(espidx), OC_(a), n);
			}
			else {
				int an = Tn_put(_ctx, expr, 2, espidx + 2);
				ASM_CHKIDX(_ctx, OC_(a), NC_(an));
				ASM(BGETIDX, NC_(espidx), OC_(a), NC_(an));
			}
			return;
		}
		if(mtd_mn == MN_set) {
			int a = Tn_put(_ctx, expr, 1, espidx + 1);
			int v = Tn_put(_ctx, expr, 3, espidx + 3);
			if(Tn_isCONST(expr, 2)) {
				intptr_t n = (intptr_t)Tn_int(expr, 2);
				if(n < 0) {
					goto L_USECALL;
				}
				ASM_CHKIDXC(_ctx, OC_(a), n);
				ASM(BSETIDXC, NC_(espidx), OC_(a), n, NC_(v));
			}
			else {
				int an = Tn_put(_ctx, expr, 2, espidx + 2);
				ASM_CHKIDX(_ctx, OC_(a), NC_(an));
				ASM(BSETIDX, NC_(espidx), OC_(a), NC_(an), NC_(v));
			}
			return;
		}
	}
#endif

#ifdef OPbNUL
	if(mtd_cid == CLASS_Object) {
		if(mtd_mn == MN_isNull) {
			int a = Tn_put(_ctx, expr, 1, espidx + 1);
			ASM(bNUL, NC_(espidx), OC_(a));
			return;
		}
		else if(mtd_mn == MN_isNotNull) {
			int a = Tn_put(_ctx, expr, 1, espidx + 1);
			ASM(bNN, NC_(espidx), OC_(a));
			return;
		}
	}
#endif
	{
		kindex_t deltaidx = knh_Method_indexOfGetterField(mtd);
		if(deltaidx != -1) {
			int b = Tn_put(_ctx, expr, 1, espidx + 1);
			ktype_t type = knh_Param_rtype(DP(mtd)->mp);
			ksfx_t bx = {OC_(b), deltaidx};
			ASM_SMOVx(_ctx, espidx, type, bx);
			return;
		}
		deltaidx = knh_Method_indexOfSetterField(mtd);
		if(deltaidx != -1) {
			int b = Tn_put(_ctx, expr, 1, espidx + 1);
			kTerm *tkV = Tn_putTK(_ctx, expr, 2, espidx + 2);
			ASM_XMOV(_ctx, OC_(b), deltaidx, tkV, espidx+3);
//			if(reqt != TY_void) {
//				ASM_SMOV(_ctx, espidx, tkV);
//			}
			return;
		}
	}
	L_USECALL:;
#endif
//	L_USECALL:;
//	{
#ifdef OPFASTCALL0
		ktype_t rtype = ktype_tocid(_ctx, knh_Param_rtype(DP(mtd)->mp), cid);
		if(DP(expr)->size == 2 && Method_isFastCall(mtd)) {
			int a = espidx;
			if(!Method_isStatic(mtd)) {
				a = Tn_put(_ctx, expr, 1, espidx + 1);
			}
			ASM(FASTCALL0, RTNIDX_(_ctx, espidx, rtype), SFP_(a), RIX_(espidx - a), SFP_(espidx + 2), SP(mtd)->fcall_1);
			return;
		}
		if(DP(expr)->size == 3 && Method_isStatic(mtd) && Method_isFastCall(mtd)) {
			int a = Tn_put(_ctx, expr, 2, espidx + 2);
			ASM(FASTCALL0, RTNIDX_(_ctx, espidx, rtype), SFP_(a - 1), RIX_(espidx - (a - 1)), SFP_(espidx + 2), SP(mtd)->fcall_1);
			return;
		}
#endif
//		if(CALLPARAMs_asm(_ctx, expr, 1, espidx, cid, mtd)) {
//			ktype_t rtype = ktype_tocid(_ctx, knh_Param_rtype(DP(mtd)->mp), cid);
//			ASM_CALL(_ctx, espidx, rtype, mtd, Method_isStatic(mtd), DP(expr)->size - 2);
//		}
//	}
//}

//static void FUNCCALL_asm(CTX, kStmtExpr *stmt, int espidx)
//{
//	kMethod *mtd = (tkNN(stmt, 0))->mtd;
//	kcid_t cid = Tn_cid(stmt, 1);
//	kParam *pa = ClassTBL(cid)->cparam;
//	size_t i;
//	for(i = 0; i < pa->psize; i++) {
////		kparam_t *p = knh_Param_get(pa, i);
////		ktype_t reqt = ktype_tocid(_ctx, p->type, DP(_ctx->gma)->this_cid);
//		Tn_asm(_ctx, stmt, i+2, espidx + i + (K_CALLDELTA+1));
//	}
//	Tn_asm(_ctx, stmt, 1, espidx + K_CALLDELTA);
//	if(Stmt_isDYNCALL(stmt)) {
//		int a = espidx + K_CALLDELTA;
//		ASM(TR, OC_(a), SFP_(a), RIX_(a-a), ClassTBL(cid), _TCHECK);
//	}
//	ktype_t rtype = ktype_tocid(_ctx, knh_Param_rtype(DP(mtd)->mp), cid);
//	ASM_CALL(_ctx, espidx, rtype, mtd, Method_isStatic(mtd), DP(stmt)->size - 2);
//}


//static kOPt OPimn(kmethodn_t mn, int diff)
//{
//	switch(mn) {
//	case MN_opNEG: return OPiNEG;
//	case MN_opADD: return OPiADD + diff;
//	case MN_opSUB: return OPiSUB + diff;
//	case MN_opMUL: return OPiMUL + diff;
//	case MN_opDIV: return OPiDIV+ diff;
//	case MN_opMOD: return OPiMOD+ diff;
//	case MN_opEQ:  return OPiEQ+ diff;
//	case MN_opNOTEQ: return OPiNEQ+ diff;
//	case MN_opLT:  return OPiLT+ diff;
//	case MN_opLTE: return OPiLTE+ diff;
//	case MN_opGT:  return OPiGT+ diff;
//	case MN_opGTE: return OPiGTE+ diff;
//#ifdef OPiAND
//	case MN_opLAND :  return OPiAND  + diff;
//	case MN_opLOR  :  return OPiOR   + diff;
//	case MN_opLXOR :  return OPiXOR  + diff;
//	case MN_opLSFT:   return OPiLSFT  + diff;
//	case MN_opRSFT:   return OPiRSFT  + diff;
//#endif
//	}
//	return OPNOP;
//}
//
//static kOPt OPfmn(kmethodn_t mn, int diff)
//{
//	switch(mn) {
//	case MN_opNEG: return OPfNEG;
//	case MN_opADD: return OPfADD + diff;
//	case MN_opSUB: return OPfSUB + diff;
//	case MN_opMUL: return OPfMUL + diff;
//	case MN_opDIV: return OPfDIV + diff;
//	case MN_opEQ:  return OPfEQ + diff;
//	case MN_opNOTEQ: return OPfNEQ + diff;
//	case MN_opLT:  return OPfLT + diff;
//	case MN_opLTE: return OPfLTE + diff;
//	case MN_opGT:  return OPfGT + diff;
//	case MN_opGTE: return OPfGTE + diff;
//	}
//	return OPNOP;
//}
//
//static kbool_t OPR_hasCONST(CTX, kStmtExpr *stmt, kmethodn_t *mn, int swap)
//{
//	int isCONST = (TT_(tmNN(stmt, 2)) == TT_CONST);
//	if(swap == 1 && TT_(tmNN(stmt, 1)) == TT_CONST) {
//		kmethodn_t newmn = *mn;
//		knh_Stmt_swap(_ctx, stmt, 1, 2);
//		if(*mn == MN_opLT) newmn = MN_opGT;  /* 1 < n ==> n > 1 */
//		else if(*mn == MN_opLTE) newmn = MN_opGTE; /* 1 <= n => n >= 1 */
//		else if(*mn == MN_opGT) newmn = MN_opLT;
//		else if(*mn == MN_opGTE) newmn = MN_opLTE;
//		//DBG_P("swap %s ==> %s", MN__(*mn), MN__(newmn));
//		*mn = newmn;
//		isCONST = 1;
//	}
//	return isCONST;
//}

//static void OPR_asm(CTX, kStmtExpr *stmt, int espidx)
//{
//	kMethod *mtd = (tkNN(stmt, 0))->mtd;
//	if(IS_NULL(mtd)) {
//		CALL_asm(_ctx, stmt, espidx); return ;
//	}
//	kmethodn_t mn = (mtd)->mn;
//	kcid_t cid = CLASS_t(SP(tkNN(stmt, 1))->type);
//	kOPt opcode;
//	if(cid == CLASS_Boolean && mn == MN_opNOT) {
//		int a = Tn_put(_ctx, stmt, 1, espidx + 1);
//		ASM(bNOT, NC_(espidx), NC_(a));
//		return;
//	}
//	if(cid == CLASS_Int && ((opcode = OPimn(mn, 0)) != OPNOP)) {
//		int swap = 1;
//		if(mn == MN_opNEG) {
//			int a = Tn_put(_ctx, stmt, 1, espidx + 1);
//			ASM(iNEG, NC_(espidx), NC_(a));
//			return;
//		}
//		if(mn == MN_opSUB || mn == MN_opDIV || mn == MN_opMOD ||
//				mn == MN_opLSFT || mn == MN_opRSFT) swap = 0;
//		if(OPR_hasCONST(_ctx, stmt, &mn, swap)) {
//			int a = Tn_put(_ctx, stmt, 1, espidx + 1);
//			kint_t b = Tn_int(stmt, 2);
//			if(b == 0 && (mn == MN_opDIV || mn == MN_opMOD)) {
//				b = 1;
//				WARN_DividedByZero(_ctx);
//			}
//			opcode = OPimn(mn, (OPiADDC - OPiADD));
//			ASMop(iADDC, opcode, NC_(espidx), NC_(a), b);
//		}
//		else {
//			int a = Tn_put(_ctx, stmt, 1, espidx + 1);
//			int b = Tn_put(_ctx, stmt, 2, espidx + 2);
//			ASMop(iADD, opcode, NC_(espidx), NC_(a), NC_(b));
//		}
//		return;
//	} /* CLASS_Int */
//	if(cid == CLASS_Float && ((opcode = OPfmn(mn, 0)) != OPNOP)) {
//		int swap = 1;
//		if(mn == MN_opNEG) {
//			int a = Tn_put(_ctx, stmt, 1, espidx + 1);
//			ASM(fNEG, NC_(espidx), NC_(a)); return;
//		}
//		if(mn == MN_opSUB || mn == MN_opDIV || mn == MN_opMOD) swap = 0;
//		if(OPR_hasCONST(_ctx, stmt, &mn, swap)) {
//			int a = Tn_put(_ctx, stmt, 1, espidx + 1);
//			kfloat_t b = Tn_float(stmt, 2);
//			if(b == KFLOAT_ZERO && mn == MN_opDIV) {
//				b = KFLOAT_ONE;
//				WARN_DividedByZero(_ctx);
//			}
//			opcode = OPfmn(mn, (OPfADDC - OPfADD));
//			ASMop(fADDC, opcode, NC_(espidx), NC_(a), b);
//		}
//		else {
//			int a = Tn_put(_ctx, stmt, 1, espidx + 1);
//			int b = Tn_put(_ctx, stmt, 2, espidx + 2);
//			ASMop(fADD, opcode, NC_(espidx), NC_(a), NC_(b));
//		}
//		return;
//	} /* CLASS_Float */
//	CALL_asm(_ctx, stmt, espidx);
//}

static kObject* BUILD_addConstPool(CTX, kObject *o)
{
	kArray_add(kcodemod->constPools, o);
	return o;
}

/* ------------------------------------------------------------------------ */

static void CALL_asm(CTX, int a, kExpr *expr, int espidx);
static void AND_asm(CTX, int a, kExpr *expr, int espidx);
static void OR_asm(CTX, int a, kExpr *expr, int espidx);
static void LETEXPR_asm(CTX, int a, kExpr *expr, int espidx);

static void NMOV_asm(CTX, int a, ktype_t ty, int b)
{
	if(TY_isUnbox(ty)) {
		ASM(NMOV, NC_(a), NC_(b), CT_(ty));
	}
	else {
		ASM(NMOV, OC_(a), OC_(b), CT_(ty));
	}
}

static void EXPR_asm(CTX, int a, kExpr *expr, int espidx)
{
	switch(expr->build) {
	case TEXPR_CONST : {
		kObject *v = expr->dataNUL;
		if(TY_isUnbox(expr->ty)) {
			ASM(NSET, NC_(a), (uintptr_t)N_toint(v), CT_(expr->ty));
		}
		else {
			v = BUILD_addConstPool(_ctx, v);
			ASM(NSET, OC_(a), (uintptr_t)v, CT_(expr->ty));
		}
		break;
	}
	case TEXPR_NCONST : {
		ASM(NSET, NC_(a), expr->ndata, CT_(expr->ty));
		break;
	}
	case TEXPR_LOCAL : {
		NMOV_asm(_ctx, a, expr->ty, expr->index);
		break;
	}
	case TEXPR_FIELD : {
		if(TY_isUnbox(expr->ty)) {
			ASM(NMOVx, NC_(a), OC_(expr->index), expr->xindex, CT_(expr->ty));
		}
		else {
			ASM(NMOVx, OC_(a), OC_(expr->index), expr->xindex, CT_(expr->ty));
		}
		break;
	}
	case TEXPR_NULL  : {
		if(TY_isUnbox(expr->ty)) {
			ASM(NSET, NC_(a), 0, CT_(expr->ty));
		}
		else {
			ASM(NULL, OC_(a), CT_(expr->ty));
		}
		break;
	}
	case TEXPR_NEW   : {
		ASM(NEW, OC_(a), expr->index, CT_(expr->ty));
		break;
	}
	case TEXPR_BOX   : {
		ASM(BOX, OC_(a), NC_(a), CT_(expr->ty));
		break;
	}
	case TEXPR_UNBOX   : {
		ASM(UNBOX, NC_(a), OC_(a), CT_(expr->ty));
		break;
	}
	case TEXPR_CALL  :
		CALL_asm(_ctx, a, expr, espidx);
		if(a != espidx) {
			NMOV_asm(_ctx, a, expr->ty, espidx);
		}
		break;
	case TEXPR_AND  :
		AND_asm(_ctx, a, expr, espidx);
		break;
	case TEXPR_OR  :
		OR_asm(_ctx, a, expr, espidx);
		break;
	case TEXPR_LET  :
		LETEXPR_asm(_ctx, a, expr, espidx);
		break;
	default:
		DBG_P("unknown expr=%d", expr->build);
		abort();
	}
}

static void CALL_asm(CTX, int a, kExpr *expr, int espidx)
{
	kMethod *mtd = expr->consNUL->methods[0];
	DBG_ASSERT(IS_Method(mtd));
	int i, s = kMethod_isStatic(mtd) ? 2 : 1, thisidx = espidx + K_CALLDELTA;
	for(i = s; i < kArray_size(expr->consNUL); i++) {
		kExpr *exprN = expr->consNUL->exprs[i];
		DBG_ASSERT(IS_Expr(exprN));
		EXPR_asm(_ctx, thisidx + i - 1, exprN, thisidx + i - 1);
	}
	int argc = kArray_size(expr->consNUL) - 2;
	if(kMethod_isVirtual(mtd)) {
		//ASM(LDMTD, SFP_(thisidx), _LOOKUPMTD, {mtd->cid, mtd->mn}, mtd);
		ASM(NSET, NC_(thisidx-1), (intptr_t)mtd, CT_Method);
		ASM(CALL, kcodemod->uline, SFP_(thisidx), ESP_(espidx, argc), CT_(expr->ty));
	}
	else {
//		ASM(NSET, NC_(thisidx-1), (intptr_t)mtd/*, FIXME CT_Method*/);
		ASM(SCALL, kcodemod->uline, SFP_(thisidx), ESP_(espidx, argc), mtd);
	}
}

static void OR_asm(CTX, int a, kExpr *expr, int espidx)
{
	int i, size = kArray_size(expr->consNUL);
	kBasicBlock*  lbTRUE = new_BasicBlockLABEL(_ctx);
	kBasicBlock*  lbFALSE = new_BasicBlockLABEL(_ctx);
	for(i = 0; i < size; i++) {
		EXPR_asmJMPIF(_ctx, a, expr->consNUL->exprs[i], 1/*TRUE*/, lbTRUE, espidx);
	}
	ASM(NSET, NC_(a), 0/*O_data(K_FALSE)*/, CT_Boolean);
	ASM_JMP(_ctx, lbFALSE);
	ASM_LABEL(_ctx, lbTRUE);
	ASM(NSET, NC_(a), 1/*O_data(K_TRUE)*/, CT_Boolean);
	ASM_LABEL(_ctx, lbFALSE); // false
}

static void AND_asm(CTX, int a, kExpr *expr, int espidx)
{
	int i, size = kArray_size(expr->consNUL);
	kBasicBlock*  lbTRUE = new_BasicBlockLABEL(_ctx);
	kBasicBlock*  lbFALSE = new_BasicBlockLABEL(_ctx);
	for(i = 1; i < size; i++) {
		EXPR_asmJMPIF(_ctx, a, expr->consNUL->exprs[i], 0/*FALSE*/, lbFALSE, espidx);
	}
	ASM(NSET, NC_(a), 1/*O_data(K_TRUE)*/, CT_Boolean);
	ASM_JMP(_ctx, lbTRUE);
	ASM_LABEL(_ctx, lbFALSE); // false
	ASM(NSET, NC_(a), 0/*O_data(K_FALSE)*/, CT_Boolean);
	ASM_LABEL(_ctx, lbTRUE);   // TRUE
}

static void LETEXPR_asm(CTX, int a, kExpr *expr, int espidx)
{
	kExpr *exprL = expr->consNUL->exprs[1];
	kExpr *exprR = expr->consNUL->exprs[2];
	if(expr->build == TEXPR_LOCAL) {
		EXPR_asm(_ctx, exprL->index, exprR, espidx);
		if(a != espidx) {
			NMOV_asm(_ctx, a, exprL->ty, espidx);
		}
	}
	else{
		assert(expr->build == TEXPR_FIELD);
		EXPR_asm(_ctx, espidx, exprR, espidx);
		if(TY_isUnbox(exprR->ty)) {
			ASM(XNMOV, OC_(exprL->index), exprL->xindex, NC_(espidx));
		}
		else {
			ASM(XNMOV, OC_(exprL->index), exprL->xindex, OC_(espidx));
		}
		if(a != espidx) {
			NMOV_asm(_ctx, a, exprL->ty, espidx);
		}
	}
}

/* ------------------------------------------------------------------------ */
/* [LABEL]  */

static void BUILD_pushLABEL(CTX, kBlock *bk, kBasicBlock *lbC, kBasicBlock *lbB)
{
	kObject *tkL = NULL;
//	if(IS_Map(DP(bk)->metaDictCaseMap)) {
//		tkL = knh_DictMap_getNULL(_ctx, DP(bk)->metaDictCaseMap, S_tobytes(TS_ATlabel));
//	}
	if(tkL == NULL) {
		tkL = K_NULL;
	}
	kArray_add(kcodemod->lstacks, tkL);
	kArray_add(kcodemod->lstacks, lbC);
	kArray_add(kcodemod->lstacks, lbB);
	kArray_add(kcodemod->lstacks, K_NULL);
}

static void BUILD_popLABEL(CTX)
{
	kArray *a = kcodemod->lstacks;
	DBG_ASSERT(kArray_size(a) - 4 >= 0);
	kArray_clear(a, kArray_size(a) - 4);
}

#ifdef ASM_STMT

/* ------------------------------------------------------------------------ */
/* [IF, WHILE, DO, FOR, FOREACH]  */

static inline kTerm *Tn_it(kStmtExpr *stmt, size_t n)
{
	DBG_ASSERT(n < DP(stmt)->size);
	return tkNN(stmt, n);
}

static inline void Tn_asmBLOCK(CTX, kStmtExpr *stmt, size_t n)
{
	DBG_ASSERT(IS_StmtExpr(stmtNN(stmt, n)));
	BLOCK_asm(_ctx, stmtNN(stmt, n));
}

static void ASM_PMOV(CTX, int isUNBOX, int a, int b)
{
	kBasicBlock *bb = kcodemod->bbNC;
	if(DP(bb)->size > 0) {
		kopl_t *opP = DP(bb)->opbuf + (DP(bb)->size - 1);
		kopTR_t *opTR = (kopTR_t*)opP;
		int defidx = opTR->a;
		if(isUNBOX) {
			int r0 = NC_(a), r1 = NC_(b);
			if(r1 == defidx) {
				kOPt opcode = opP->opcode;
				DBG_P("r0=%d, r1=%d, def=%d", r0, r1, defidx);
				if((OPbNOT <= opcode && opcode <= OPfGTEC)
				  || (OPiCAST <= opcode && opcode <= OPfCAST)
				  || (OPBGETIDX <= opcode && opcode <= OPNSETIDXC)
				  || (OPNSET == opcode)) {
					opTR->a = r0;
					return ;
				}
			}
			ASM(NMOV, r0, r1);
		}
		else {
			int r0 = OC_(a), r1 = OC_(b);
			if(r1 == defidx) {
				kOPt opcode = opP->opcode;
				DBG_P("r0=%d, r1=%d, def=%d", r0, r1, defidx);
				if(opcode == OPTR) {
					opTR->a = r0;
					opTR->rix = (r0 - opTR->b) / 2;
					return ;
				}
				if((OPOGETIDX <= opcode && opcode <= OPOSETIDXC)) {
					opTR->a = r0;
					return ;
				}
			}
			ASM(OMOV, r0, r1);
		}
	}
	else {
		if(isUNBOX) {
			ASM(NMOV, NC_(a), NC_(b));
		}
		else {
			ASM(OMOV, OC_(a), OC_(b));
		}
	}
}

static void LET_asm(CTX, kStmtExpr *stmt)
{
	kTerm *tkL = tkNN(stmt, 1);
	kTerm *tkV = tkNN(stmt, 2);
	ktype_t atype = tkL->type;
	if(IS_Term(tkV)) {
		ASM_MOV(_ctx, tkL, tkV, DP(stmt)->espidx);
	}
	else {
		Tn_asm(_ctx, stmt, 2, DP(stmt)->espidx);
		if(TT_(tkL) == TT_FVAR) {
			ASM_PMOV(_ctx, TY_isUnbox(atype), Term_index(tkL), DP(stmt)->espidx);
		}
		else {
			DBG_ASSERT(TT_(tkL) == TT_FIELD);
			ktype_t atype = tkL->type;
			ksfx_t ax;
			Term_setsfx(_ctx, tkL, &ax);
			if(TY_isUnbox(atype)) {
				ASM(XNMOV, ax, NC_(DP(stmt)->espidx));
			}
			else {
				ASM(XMOV, ax, OC_(DP(stmt)->espidx));
			}
		}
	}
}

static void SWITCH_asm(CTX, kStmtExpr *stmt)
{
	kStmtExpr *stmtCASE;
	kTerm *tkIT = Tn_it(stmt, 2);
	kBasicBlock* lbCONTINUE = new_BasicBlockLABEL(_ctx);
	kBasicBlock* lbBREAK = new_BasicBlockLABEL(_ctx);
	kBasicBlock *lbNEXT = NULL;
	BUILD_pushLABEL(_ctx, stmt, lbCONTINUE, lbBREAK);
	ASM_LABEL(_ctx, lbCONTINUE);
	//switch(it)
	Tn_asm(_ctx, stmt, 0, Term_index(tkIT));
	stmtCASE = stmtNN(stmt, 1);
	while(stmtCASE != NULL) {
		// case 'a' :
		if(STT_(stmtCASE) == STT_CASE && !Tn_isASIS(stmtCASE, 0)) {
			kBasicBlock *lbEND = new_BasicBlockLABEL(_ctx);
			//@@DP(stmt)->espidx = DP(stmtCASE)->espidx + DP(_ctx->gma)->ebpidx;
			EXPR_asmJMPIF(_ctx, stmtCASE, 0, 0/*FALSE*/, lbEND, DP(stmt)->espidx);
			if(lbNEXT != NULL) {
				ASM_LABEL(_ctx, lbNEXT); lbNEXT = NULL;
			}
			Tn_asmBLOCK(_ctx, stmtCASE, 1);
			lbNEXT = new_BasicBlockLABEL(_ctx);
			ASM_JMP(_ctx, lbNEXT);
			ASM_LABEL(_ctx, lbEND);
		}
		stmtCASE = DP(stmtCASE)->nextNULL;
	}
	if(lbNEXT != NULL) {
		ASM_LABEL(_ctx, lbNEXT); lbNEXT = NULL;
	}
	stmtCASE = stmtNN(stmt, 1);
	while(stmtCASE !=NULL) {
		if(STT_(stmtCASE) == STT_CASE && Tn_isASIS(stmtCASE, 0)) {
			Tn_asmBLOCK(_ctx, stmtCASE, 1);
		}
		stmtCASE = DP(stmtCASE)->nextNULL;
	}
	ASM_LABEL(_ctx, lbBREAK);
	BUILD_popLABEL(_ctx);
}

static void ASM_JUMPLABEL(CTX, kStmtExpr *stmt, int delta)
{
	size_t s = kArray_size(kcodemod->lstacks);
	if(s < 4) {
		kStmtExproERR(_ctx, stmt, ERROR_OnlyTopLevel(_ctx, Stmt__(stmt)));
	}
	else {
		kTerm *tkL = NULL;
		kBasicBlock *lbBLOCK = NULL;
		if(DP(stmt)->size == 1) {
			tkL = tkNN(stmt, 0);
			if(TT_(tkL) == TT_ASIS) tkL = NULL;
		}
		if(tkL != NULL) {
			int i;
			kbytes_t lname = S_tobytes((tkL)->text);
			for(i = s - 4; i >= 0; i -= 4) {
				kTerm *tkSTACK = kcodemod->lstacks->terms[i];
				if(IS_NOTNULL(tkSTACK) && S_equals((tkSTACK)->text, lname)) {
					lbBLOCK = GammaBuilderLabel(_ctx,  i + delta);
					goto L_JUMP;
				}
			}
			ErrorUndefinedLabel(_ctx, tkL);
			return;
		}
		lbBLOCK = GammaBuilderLabel(_ctx,  s - 4 + delta);
		L_JUMP:;
		ASM_JMP(_ctx, lbBLOCK);
	}
}

static void CONTINUE_asm(CTX, kStmtExpr *stmt)
{
	ASM_JUMPLABEL(_ctx, stmt, 1);
}

static void BREAK_asm(CTX, kStmtExpr *stmt)
{
	ASM_JUMPLABEL(_ctx, stmt, 2);
}

static void WHILE_asm(CTX, kStmtExpr *stmt)
{
	kBasicBlock* lbCONTINUE = new_BasicBlockLABEL(_ctx);
	kBasicBlock* lbBREAK = new_BasicBlockLABEL(_ctx);
	BUILD_pushLABEL(_ctx, stmt, lbCONTINUE, lbBREAK);
	ASM_LABEL(_ctx, lbCONTINUE);
	ASM_SAFEPOINT(_ctx, DP(stmt)->espidx);
	if(!Tn_isTRUE(stmt, 0)) {
		EXPR_asmJMPIF(_ctx, stmt, 0, 0/*FALSE*/, lbBREAK, DP(stmt)->espidx);
		//ASM_CHECK_INFINITE_LOOP(_ctx, stmt);
	}
	Tn_asmBLOCK(_ctx, stmt, 1);
	ASM_JMP(_ctx, lbCONTINUE);
	ASM_LABEL(_ctx, lbBREAK);
	BUILD_popLABEL(_ctx);
}

static void DO_asm(CTX, kStmtExpr *stmt)
{
	kBasicBlock* lbCONTINUE = new_BasicBlockLABEL(_ctx);
	kBasicBlock* lbBREAK = new_BasicBlockLABEL(_ctx);
	BUILD_pushLABEL(_ctx, stmt, lbCONTINUE, lbBREAK);
	ASM_LABEL(_ctx, lbCONTINUE);
	ASM_SAFEPOINT(_ctx, DP(stmt)->espidx);
	Tn_asmBLOCK(_ctx, stmt, 0);
	EXPR_asmJMPIF(_ctx, stmt, 1, 0/*FALSE*/, lbBREAK, DP(stmt)->espidx);
	ASM_JMP(_ctx, lbCONTINUE);
	ASM_LABEL(_ctx, lbBREAK);
	BUILD_popLABEL(_ctx);
}

static void FOR_asm(CTX, kStmtExpr *stmt)
{
	kBasicBlock* lbCONTINUE = new_BasicBlockLABEL(_ctx);
	kBasicBlock* lbBREAK = new_BasicBlockLABEL(_ctx);
	kBasicBlock* lbREDO = new_BasicBlockLABEL(_ctx);
	BUILD_pushLABEL(_ctx, stmt, lbCONTINUE, lbBREAK);
	/* i = 1 part */
	Tn_asmBLOCK(_ctx, stmt, 0);
	ASM_JMP(_ctx, lbREDO);
	/* i++ part */
	ASM_LABEL(_ctx, lbCONTINUE); /* CONTINUE */
	ASM_SAFEPOINT(_ctx, DP(stmt)->espidx);
	Tn_asmBLOCK(_ctx, stmt, 2);
	/* i < 10 part */
	ASM_LABEL(_ctx, lbREDO);
	if(!Tn_isTRUE(stmt, 1)) {
		EXPR_asmJMPIF(_ctx, stmt, 1, 0/*FALSE*/, lbBREAK, DP(stmt)->espidx);
	}
	Tn_asmBLOCK(_ctx, stmt, 3);
	ASM_JMP(_ctx, lbCONTINUE);
	ASM_LABEL(_ctx, lbBREAK);
	BUILD_popLABEL(_ctx);
}

/* ------------------------------------------------------------------------ */

static void FOREACH_asm(CTX, kStmtExpr *stmt)
{
	kBasicBlock* lbC = new_BasicBlockLABEL(_ctx);
	kBasicBlock* lbB = new_BasicBlockLABEL(_ctx);
	BUILD_pushLABEL(_ctx, stmt, lbC, lbB);
	{
		kTerm *tkN = tkNN(stmt, 0);
		int varidx = Term_index(tkN);
		int itridx = Term_index(tkNN(stmt, 2));
		Tn_asm(_ctx, stmt, 1, itridx);
		ASM_LABEL(_ctx, lbC);
		ASM_SAFEPOINT(_ctx, DP(stmt)->espidx);
		ASMbranch(NEXT, lbB, RTNIDX_(_ctx, varidx, (tkN)->type), SFP_(itridx), RIX_(varidx - itridx), SFP_(_ESPIDX));
	}
	Tn_asmBLOCK(_ctx, stmt, 3);
	ASM_JMP(_ctx, lbC);
	/* end */
	ASM_LABEL(_ctx, lbB);
	BUILD_popLABEL(_ctx);
}

/* ------------------------------------------------------------------------ */
/* [TRY] */

#define BUILD_inTry(_ctx)  IS_StmtExpr(DP(_ctx->gma)->finallyStmt)

static void BUILD_setFINALLY(CTX, kStmtExpr *stmt)
{
	if(IS_NOTNULL(stmt)) {
		if(IS_NOTNULL(DP(_ctx->gma)->finallyStmt)) {
			ErrorMisplaced(_ctx);
			KNH_HINT(_ctx, "try"); // not nested try
			return;
		}
		KSETv(DP(_ctx->gma)->finallyStmt, stmt);
	}
	else { /* stmt == null */
		KSETv(DP(_ctx->gma)->finallyStmt, stmt);
	}
}

static void ASM_FINALLY(CTX)
{
	if(IS_NOTNULL(DP(_ctx->gma)->finallyStmt)) {
		BLOCK_asm(_ctx, DP(_ctx->gma)->finallyStmt);
	}
}

static void TRY_asm(CTX, kStmtExpr *stmt)
{
	kBasicBlock*  lbCATCH   = new_BasicBlockLABEL(_ctx);
	kBasicBlock*  lbFINALLY = new_BasicBlockLABEL(_ctx);
	kTerm *tkIT = Tn_it(stmt, 3/*HDR*/);
	kStmtExpr *stmtCATCH = stmtNN(stmt, 1);
	BUILD_setFINALLY(_ctx, stmtNN(stmt, 2/*finally*/));
	/* try { */
	ASMbranch(TRY, lbCATCH, OC_((tkIT)->index));
	Tn_asmBLOCK(_ctx, stmt, 0/*try*/);
	ASM_JMP(_ctx, lbFINALLY);
	BUILD_setFINALLY(_ctx, (kStmtExpr*)K_NULL); // InTry
	/* catch */
	ASM_LABEL(_ctx, lbCATCH);
	DBG_P("stmtCATCH=%s", CLASS__(O_cid(stmtCATCH)));
	while(stmtCATCH != NULL) {
		DBG_ASSERT(IS_StmtExpr(stmtCATCH));
		if(SP(stmtCATCH)->stt == STT_CATCH) {
			kString *emsg = tkNN(stmtCATCH, 0)->text;
			kTerm *tkN = tkNN(stmtCATCH, 1);
			DBG_ASSERT(IS_String(emsg));
			DBG_ASSERT(TT_(tkN) == TT_FVAR || TT_(tkN) == TT_LVAR);
			if(!knh_isDefinedEvent(_ctx, S_tobytes(emsg))) {
				WARN_Undefined(_ctx, "fault", CLASS_Exception, tkNN(stmtCATCH, 0));
			}
			lbCATCH = new_BasicBlockLABEL(_ctx);
			ASMbranch(CATCH, lbCATCH, OC_((tkN)->index), knh_geteid(_ctx, S_tobytes(emsg)));
			Tn_asmBLOCK(_ctx, stmtCATCH, 2);
			ASM_JMP(_ctx, lbFINALLY);  /* GOTO FINALLY */
			ASM_LABEL(_ctx, lbCATCH); /* _CATCH_NEXT_ */
		}
		stmtCATCH = DP(stmtCATCH)->nextNULL;
	}
	ASM_LABEL(_ctx, lbFINALLY); /* FINALLY */
	Tn_asmBLOCK(_ctx, stmt, 2/*finally*/);
	ASM(THROW, SFP_(((tkIT)->index)-1));
}

static void ASSURE_asm(CTX, kStmtExpr *stmt)
{
	int index = Term_index(tkNN(stmt, 2)); // it
	Tn_asm(_ctx, stmt, 0, index);
	ASM(CHKIN, OC_(index), ClassTBL(CLASS_Assurance)->cdef->checkin);
	Tn_asmBLOCK(_ctx, stmt, 1);
	ASM(CHKOUT, OC_(index), ClassTBL(CLASS_Assurance)->cdef->checkout);
}

static void THROW_asm(CTX, kStmtExpr *stmt)
{
	int start = 0, espidx = DP(stmt)->espidx;
	if(BUILD_inTry(_ctx)) {
		start = espidx;
	}
	Tn_asm(_ctx, stmt, 0, espidx);
	ASM(TR, OC_(espidx), SFP_(espidx), RIX_(espidx-espidx), ClassTBL(CLASS_Exception), _ERR);
	ASM(THROW, SFP_(start));
}

static void RETURN_asm(CTX, kStmtExpr *stmt)
{
	ASM_FINALLY(_ctx);
	size_t size = DP(stmt)->size;
	if(size == 1) {
		Tn_asm(_ctx, stmt, 0, DP(stmt)->espidx);
		ASM_PMOV(_ctx, TY_isUnbox(Tn_type(stmt, 0)), K_RTNIDX, DP(stmt)->espidx);
	}
	if(IS_StmtExpr(DP(stmt)->stmtPOST)) {
		DP(DP(stmt)->stmtPOST)->espidx = DP(stmt)->espidx + 1;
		EXPR_asm(_ctx, DP(stmt)->stmtPOST, DP(stmt)->espidx + 1);
	}
	if(!Stmt_isImplicit(stmt)) {
		ASM_RET(_ctx, stmt);
	}
}


static void ASSERT_asm(CTX, kStmtExpr *stmt)
{
	int espidx = DP(stmt)->espidx;
	kBasicBlock* lbskip = new_BasicBlockLABEL(_ctx);
	/* if */
	EXPR_asmJMPIF(_ctx, stmt, 0, 1, lbskip, DP(stmt)->espidx);
	/*then*/
	ASM(ASSERT, SFP_(espidx), stmt->uline);
	ASM_LABEL(_ctx, lbskip);
}
#endif

/* ------------------------------------------------------------------------ */

static void BLOCK_asm(CTX, kBlock *bk);

#define kStmt_expr(STMT, KW, DEF)  Stmt_expr(_ctx, STMT, KW, DEF)
static kExpr* Stmt_expr(CTX, kStmt *stmt, keyword_t kw, kExpr *def)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, kw);
	if(expr != NULL && IS_Expr(expr)) {
		return expr;
	}
	return def;
}

#define kStmt_block(STMT, KW, DEF)  Stmt_block(_ctx, STMT, KW, DEF)
static kBlock* Stmt_block(CTX, kStmt *stmt, keyword_t kw, kBlock *def)
{
	kBlock *bk = (kBlock*)kObject_getObjectNULL(stmt, kw);
	if(bk != NULL && IS_Block(bk)) {
		return bk;
	}
	return def;
}

static void ErrStmt_asm(CTX, kStmt *stmt, int espidx)
{
	kString *msg = (kString*)kObject_getObjectNULL(stmt, 0);
	DBG_ASSERT(IS_String(msg));
	ASM(ERROR, SFP_(espidx), msg);
}

static void ExprStmt_asm(CTX, kStmt *stmt, int espidx)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, 1);
	DBG_P("expr=%p", expr);
	if(IS_Expr(expr)) {
		EXPR_asm(_ctx, espidx, expr, espidx);
	}
}

static void BlockStmt_asm(CTX, kStmt *stmt, int espidx)
{
	BLOCK_asm(_ctx, kStmt_block(stmt, KW_BLOCK, K_NULLBLOCK));
}

static void IfStmt_asm(CTX, kStmt *stmt, int espidx)
{
	kBasicBlock*  lbELSE = new_BasicBlockLABEL(_ctx);
	kBasicBlock*  lbEND  = new_BasicBlockLABEL(_ctx);
	/* if */
	lbELSE = EXPR_asmJMPIF(_ctx, espidx, kStmt_expr(stmt, 1, NULL), 0/*FALSE*/, lbELSE, espidx);
	/* then */
	BLOCK_asm(_ctx, kStmt_block(stmt, KW_THEN, K_NULLBLOCK));
	ASM_JMP(_ctx, lbEND);
	/* else */
	ASM_LABEL(_ctx, lbELSE);
	BLOCK_asm(_ctx, kStmt_block(stmt, KW_ELSE, K_NULLBLOCK));
	/* endif */
	ASM_LABEL(_ctx, lbEND);
}

static void ReturnStmt_asm(CTX, kStmt *stmt, int espidx)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, 1);
	if(IS_Expr(expr) && expr->ty != TY_void) {
		EXPR_asm(_ctx, K_RTNIDX, expr, espidx);
	}
	ASM_JMP(_ctx, GammaBuilderLabel(2));  // RET
}

static void UndefinedStmt_asm(CTX, kStmt *stmt, int espidx)
{
	DBG_P("undefined asm syntax='%s'", stmt->syn->token);
}

static void BLOCK_asm(CTX, kBlock *bk)
{
	int i, espidx = bk->esp->index;
	DBG_ASSERT(bk->esp->build == TEXPR_LOCAL);
	for(i = 0; i < kArray_size(bk->blockS); i++) {
		kStmt *stmt = bk->blockS->stmts[i];
		if(stmt->syn == NULL) continue;
		kcodemod->uline = stmt->uline;
		switch(stmt->build) {
		case TSTMT_ERR:    ErrStmt_asm(_ctx, stmt, espidx);   return;
		case TSTMT_EXPR:   ExprStmt_asm(_ctx, stmt, espidx);  break;
		case TSTMT_BLOCK:  BlockStmt_asm(_ctx, stmt, espidx); break;
		case TSTMT_RETURN: ReturnStmt_asm(_ctx, stmt, espidx); return;
		case TSTMT_IF:     IfStmt_asm(_ctx, stmt, espidx);     return;
		default: UndefinedStmt_asm(_ctx, stmt, espidx); break;
		}
	}
}

/* ------------------------------------------------------------------------ */

static void _THCODE(CTX, kopl_t *pc, void **codeaddr)
{
#ifdef K_USING_THCODE_
	while(1) {
		pc->codeaddr = codeaddr[pc->opcode];
		if(pc->opcode == OPCODE_RET) break;
		pc++;
	}
#endif
}

void MODCODE_genCode(CTX, kMethod *mtd, kBlock *bk)
{
	DBG_P("START CODE GENERATION..");
	if(kcodemod == NULL) {
		kcodeshare->h.setup(_ctx, NULL);
	}
	DBG_ASSERT(kArray_size(kcodemod->insts) == 0);
	DBG_ASSERT(kArray_size(kcodemod->lstacks) == 0);
	kBasicBlock* lbINIT  = new_BasicBlockLABEL(_ctx);
	kBasicBlock* lbBEGIN = new_BasicBlockLABEL(_ctx);
	kBasicBlock* lbEND   = new_BasicBlockLABEL(_ctx);
	kcodemod->bbNC = lbINIT;
	BUILD_pushLABEL(_ctx, bk, lbBEGIN, lbEND);
	ASM(THCODE, _THCODE);
	ASM_LABEL(_ctx, lbBEGIN);
	BLOCK_asm(_ctx, bk);
	ASM_LABEL(_ctx, lbEND);
	ASM(RET);
	BUILD_popLABEL(_ctx);
	DBG_ASSERT(kArray_size(kcodemod->lstacks) == 0);
	BUILD_compile(_ctx, mtd, lbINIT, lbEND);
}

/* ------------------------------------------------------------------------ */
/* [datatype] */

#define STRUCTNAME(C) \
	.structname = #C,\
	.cid = CLASS_newid,\
	.cflag = 0,\
	.cstruct_size = sizeof(k##C)\

static void BasicBlock_init(CTX, kRawPtr *o, void *conf)
{
	kBasicBlock *bb = (kBasicBlock*)o;
	bb->bottom = 0;
	bb->capacity = 0;
	bb->code = NULL;
	bb->id = 0;
	bb->incoming = 0;
	bb->listNC  = NULL;
	bb->nextNC  = NULL;
	bb->jumpNC  = NULL;
	bb->opbuf = NULL;
	bb->opjmp = NULL;
}

static void BasicBlock_free(CTX, kRawPtr *o)
{
	kBasicBlock *bb = (kBasicBlock*)o;
	if(bb->capacity > 0) {
		KNH_FREE(bb->opbuf, bb->capacity * sizeof(kopl_t));
	}
}

static const KSTRUCT_DEF BasicBlockDef = {
	STRUCTNAME(BasicBlock),
	.init = BasicBlock_init,
	.free = BasicBlock_free,
};

static void KonohaCode_init(CTX, kRawPtr *o, void *conf)
{
	kKonohaCode *b = (kKonohaCode*)o;
	b->codesize = 0;
	b->code = NULL;
	b->uri = 0;
	KINITv(b->source, TS_EMPTY);
}

static void KonohaCode_reftrace(CTX, kRawPtr *o)
{
	kKonohaCode *b = (kKonohaCode*)o;
	BEGIN_REFTRACE(1);
	KREFTRACEv(b->source);
	END_REFTRACE();
}

static void KonohaCode_free(CTX, kRawPtr *o)
{
	kKonohaCode *b = (kKonohaCode*)o;
	KNH_FREE(b->code, b->codesize);
}

static const KSTRUCT_DEF KonohaCodeDef = {
	STRUCTNAME(KonohaCode),
	.init = KonohaCode_init,
	.reftrace = KonohaCode_reftrace,
	.free = KonohaCode_free,
};

static KMETHOD Fmethod_abstract(CTX, ksfp_t *sfp _RIX)
{
//	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
//	ktype_t rtype = ktype_tocid(_ctx, knh_Param_rtype(DP(mtd)->mp), O_cid(sfp[0].o));
//	//KNH_SYSLOG(_ctx, LOG_NOTICE, "AbstractMethod", "%C.%M", mtd->cid, mtd->mn);
//	if(rtype != TY_void) {
//		if(TY_isUnbox(rtype)) {
//			RETURNi_(KINT0);
//		}
//		else {
//			RETURN_(KNH_NULVAL(CLASS_t(rtype)));
//		}
//	}
}

//static kbool_t Method_isAbstract(kMethod *mtd)
//{
//	return (mtd->fcall_1 == Fmethod_abstract);
//}

static void Method_setFunc(CTX, kMethod *mtd, knh_Fmethod func)
{
	func = (func == NULL) ? Fmethod_abstract : func;
	mtd->fcall_1 = func;
	(mtd)->pc_start = CODE_NCALL;
}

/* ------------------------------------------------------------------------ */
/* [kcodemod] */

static void kcodemod_reftrace(CTX, struct kmod_t *baseh)
{
	kcodemod_t *base = (kcodemod_t*)baseh;
	BEGIN_REFTRACE(3);
	KREFTRACEv(base->lstacks);
	KREFTRACEv(base->insts);
	KREFTRACEv(base->constPools);
	END_REFTRACE();
}
static void kcodemod_free(CTX, struct kmod_t *baseh)
{
	kcodemod_t *base = (kcodemod_t*)baseh;
	KNH_FREE(base, sizeof(kcodemod_t));
}

static void kcodeshare_setup(CTX, struct kmodshare_t *def)
{
	assert(_ctx->mod[MOD_CODE] == NULL);
	kcodemod_t *base = (kcodemod_t*)KNH_ZMALLOC(sizeof(kcodemod_t));
	base->h.reftrace = kcodemod_reftrace;
	base->h.free     = kcodemod_free;
	KINITv(base->insts, new_(Array, K_PAGESIZE/sizeof(void*)));
	KINITv(base->lstacks, new_(Array, 64));
	KINITv(base->constPools, new_(Array, 64));
	_ctx->mod[MOD_CODE] = (kmod_t*)base;
}

static void kcodeshare_reftrace(CTX, struct kmodshare_t *baseh)
{
	kcodeshare_t *base = (kcodeshare_t*)baseh;
	BEGIN_REFTRACE(1);
	KREFTRACEn(base->codeNull);
	END_REFTRACE();
}

static void kcodeshare_free(CTX, struct kmodshare_t *baseh)
{
//	kcodeshare_t *base = (kcodeshare_t*)baseh;
	KNH_FREE(baseh, sizeof(kcodeshare_t));
}

void MODCODE_init(CTX, kcontext_t *ctx)
{
	kcodeshare_t *base = (kcodeshare_t*)KNH_ZMALLOC(sizeof(kcodeshare_t));
	opcode_check();
	base->h.name     = "minivm";
	base->h.setup    = kcodeshare_setup;
	base->h.reftrace = kcodeshare_reftrace;
	base->h.free     = kcodeshare_free;

	ksetModule(MOD_CODE, &base->h, 0);
	base->cBasicBlock = kaddClassDef(&BasicBlockDef);
	base->cKonohaCode = kaddClassDef(&KonohaCodeDef);
	kcodeshare_setup(_ctx, &base->h);
	{
		INIT_GCSTACK();
		kBasicBlock* ia = new_(BasicBlock, 0);
		kBasicBlock* ib = new_(BasicBlock, 0);
		PUSH_GCSTACK(ia);
		PUSH_GCSTACK(ib);
		kBasicBlock_add(ia, THCODE, _THCODE);
		kBasicBlock_add(ia, NCALL); // FUNCCALL
		kBasicBlock_add(ia, ENTER);
		kBasicBlock_add(ia, EXIT);
		kBasicBlock_add(ib, RET);   // NEED TERMINATION
		kKonohaCode *kcode = new_KonohaCode(_ctx, ia, ib);
		KINITv(kcodeshare->codeNull, kcode);
		kopl_t *pc = VirtualMachine_run(_ctx, _ctx->esp, kcode->code);
		CODE_ENTER = pc;
		CODE_ENTER = pc+1;
		kArray_clear(kcodemod->insts, 0);
		RESET_GCSTACK();
	}
	klib2_t *l = ctx->lib2;
	l->KMethod_setFunc = Method_setFunc;
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
