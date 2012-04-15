/****************************************************************************
 * KONOHA2 COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 * You may choose one of the following two licenses when you use konoha.
 * If you want to use the latter license, please contact us.
 *
 * (1) GNU General Public License 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure License 1.0
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

//#ifdef K_USING_SSA
//
//#include"commons.h"
//
//#define VERTEX(n)       ((kBasicBlock*)kArray_n(vertex, n))
//#define ANCESTOR(bb)    ((kBasicBlock*)Array_BB(ancestor, bb))
//#define SEMI(bb)        IArray_n(semi, DFN(bb))
//#define BEST(bb)        ((kBasicBlock*)Array_BB(best, bb))
//#define IDOM(bb)        ((kBasicBlock*)Array_BB(idom, bb))
//#define SAMEDOM(bb)     ((kBasicBlock*)Array_BB(samedom, bb))
//#define BUCKET(bb)      ((kArray*)Array_BB(bucket, bb))
//#define PRED(bb)        ((kArray*)Array_BB(pred, bb))
//#define DF(bb)          ((kArray*)Array_BB(df, bb))
//#define DEFSITES(n)     ((kArray*)kArray_n(defsites, n))
//#define APHI(n)         ((kArray*)kArray_n(Aphi, n))
//#define STACK(n)        ((kArray*)kArray_n(stack, IArray_n(origidx, n)))
//#define AORIG(n)        ((kArray*)kArray_n(Aorig, n))
//#define PCC(n)          ((kArray*)kArray_n(pcc, n))
//#define UNM(n)          ((kArray*)kArray_n(unm, n))
//#define LIVEIN(n)       ((kArray*)kArray_n(liveIn, n))
//#define LIVEOUT(n)      ((kArray*)kArray_n(liveOut, n))
//#define DFN(bb)         (DP(bb)->id)
//#define Array_BB(a, bb) kArray_n(a, DFN(bb))
//#define TOP(a)          IArray_n(a, kArray_size(a) - 1)
//#define PARENT(bb)      knh_BasicBlock_getParent(vertex, bb)
//#define PUSH(a, n)      IArray_add(_ctx, a, n)
//#define PRINT_AR(fp, msg, o) \
//		fprintf(fp, msg, (o != K_NULL) ? (int)DFN((kBasicBlock*)o) : (-1))
//#define IArray_n(a, i)  ((int)(a)->ilist[i])
//#define ISOBJ(n)        ((n) % 2 == 0)
//#define hasassign(opcode) kopcode_usedef(opcode, 0)
//#define tochar(opcode)  (opcode == OPCODE_PHI) ? "PHI" : T_opcode(opcode)
//#define LINK(bb1, bb2)\
//	KSETv(Array_BB(ancestor, bb2), bb1);\
//	KSETv(Array_BB(best, bb2), bb2)
//#define SWAP(a, b, size)\
//	do {\
//		register size_t __size = (size);\
//		register char *__a = (char*)(a);\
//		register char *__b = (char*)(b);\
//		do {\
//			char __tmp = *__a;\
//			*__a++ = *__b;\
//			*__b++ = __tmp;\
//		} while (--__size > 0);\
//	} while (0)
//#define ISCALL(opcode)  (opcode == OPCODE_SCALL || opcode == OPCODE_VCALL || \
//		opcode == OPCODE_VCALL_ || opcode == OPCODE_CALL)
//
//#if defined(K_USING_THCODE_)
//#define TADDR   NULL, 0/*counter*/
//#else
//#define TADDR   0/*counter*/
//#endif/*K_USING_THCODE_*/
//#define ASMLINE  0
//
//#define OPCODE_PHI             (OPCODE_NOP)
//typedef struct klr_PHI_t {
//	KCODE_HEAD;
//	ksfpidx_t a;
//	ksfpidx_t b;
//	ksfpidx_t c;
//} klr_PHI_t;
//
//struct phidata {
//	kArray *pred;
//	kArray *liveIn;
//	kArray *liveOut;
//	int max;
//};
//
//void BasicBlock_add(CTX, kBasicBlock *bb, int line, kopl_t *op);
//kBasicBlock* new_BasicBlockLABEL(CTX);
//
//#ifdef __cplusplus 
//extern "C" {
//#endif
//
///* ------------------------------------------------------------------------ */
///* [API] */
//
///**
// * Add an int value to int array.
// * @param ctx Context
// * @param a   Target Array<int>
// * @param v   Int value
// */
//static void IArray_add(CTX, kArray *a, kint_t v)
//{
//	BEGIN_LOCAL(_ctx, lsfp, 1);
//	lsfp[0].ivalue = v;
//	a->api->add(_ctx, a, lsfp);
//	END_LOCAL(_ctx, lsfp);
//}
//
///**
// * Pop the stack.
// * @param ctx   Context
// * @param a     Targe Array<int>
// * @return      A top number of the stack.
// */
//static int IArray_pop(CTX, kArray *a)
//{
//	DBG_ASSERT(kArray_size(a) > 1);
//	int v = IArray_n(a, kArray_size(a) - 1);
//	kArray_clear(a, kArray_size(a) - 1);
//	return v;
//}
//
///**
// * Check an array contains an int object.
// * @param ctx Cotnext
// * @param a   A list of int object
// * @param n   Target int object
// * @return    1 if a contains n
// *            or 0 if a does not contain n.
// */
//static int IArray_isContain(kArray *a, int n)
//{
//	size_t i = 0;
//	for (i = 0; i < kArray_size(a); i++) {
//		if(IArray_n(a, i) == n)
//			return 1;
//	}
//	return 0;
//}
//
///**
// * Copy an array of blocks to another array of blocks.
// * @param ctx Cotnext
// * @param af  Copy from this array.
// * @param bt  Copy to this array.
// */
//static void kArray_copy(CTX, kArray *at, kArray *af)
//{
//	size_t i;
//	for (i = 0; i < kArray_size(af); i++) {
//		kArray_add(at, kArray_n(af, i));
//	}
//}
//
///**
// * Insert an object to an array.
// * @param ctx Context
// * @param a   Target Array<void>
// * @param o   Object
// */
//static void kArray_insert(CTX, kArray *a, void *o)
//{
//	size_t i;
//	for (i = 0; i < kArray_size(a); i++) {
//		if (kArray_n(a, i) == (kObject*)o)
//			return;
//	}
//	kArray_add(a, o);
//}
//
///**
// * Check an array contains a block.
// * @param ctx Cotnext
// * @param a   A list of block
// * @param n   Target block
// * @return    1 if a contains n
// *            or 0 if a does not contain n.
// */
//static int Array_isContain(kArray *a, void *n)
//{
//	size_t i;
//	kBasicBlock *p;
//	for (i = 0; i < kArray_size(a); i++) {
//		p = (void *)kArray_n(a, i);
//		if (p == n)
//			return 1;
//	}
//	return 0;
//}
//
///* ------------------------------------------------------------------------ */
///* [debug] */
//
//#ifdef K_USING_DEBUG
//
///**
// * Print BasicBlock.
// * @param ctx Context
// * @param bb  BasicBlock
// */
//static void knh_BasicBlock_print(CTX, kBasicBlock *bb)
//{
//	int i;
//	kopl_t *buf = DP(bb)->opbuf;
//	int size = DP(bb)->size;
//	fprintf(stderr, "+------BB[%02d]------+\n", DP(bb)->id);
//	fprintf(stderr, "|    incoming=%02d   |\n", DP(bb)->incoming);
//	//fprintf(stderr, "| size=%02d, capa=%02d |\n",
//	//DP(bb)->size, DP(bb)->capacity);
//	for (i = 0; i < size; i++) {
//		kopl_t *op = buf + i;
//		if (op->opcode == OPCODE_PHI) {
//			fprintf(stderr, "|  PHI(%02d,%02d,%02d)   |\n", (int)op->data[0], (int)op->data[1], (int)op->data[2]);
//		} else {
////			fprintf(stderr, "|   opcode=%-6s  |\n",
////					T_opcode(op->opcode));
//			knh_opcode_dump(_ctx, op, KNH_STDERR, op);
//		}
//	}
//	fprintf(stderr, "| next=%02d",
//			(bb->nextNC) ? (int)DFN(bb->nextNC) : (-1));
//	fprintf(stderr, ", jump=%02d |\n",
//			(bb->jumpNC) ? (int)DFN(bb->jumpNC) : (-1));
//}
//
///**
// * Print immediately dominator(idom) and others.
// * @param ctx      Context
// * @param vertex   Linear list of BasicBlocks(~= listNC)
// * @param samedom  It is used if a block has same dominator
// *                 as other block.
// * @param ancestor Ancestor block in Control Flow Graph
// * @param semi     Semidominator of the block
// * @param idom     Immediately dominator of the block
// */
//static void printIdom(CTX, kArray *vertex, kArray *samedom, kArray *ancestor, kArray *semi, kArray *idom)
//{
//	size_t i;
//	for (i = 0; i < kArray_size(vertex); i++) {
//		fprintf(stderr, "[%02lu] samedom=", i);
//		PRINT_AR(stderr, "%02d", kArray_n(samedom, i));
//		PRINT_AR(stderr, ", ancestor=%02d", kArray_n(ancestor, i));
//		fprintf(stderr, ", semi=%02d", IArray_n(semi, i));
//		PRINT_AR(stderr, ", idom=%02d\n", kArray_n(idom, i));
//	}
//}
//
///**
// * Print Dominance Frontier(DF).
// * @param ctx    Context
// * @param vertex Linear list of BasicBlocks(~= listNC)
// * @param df     Dominance Frontier
// */
//static void printDF(kArray *vertex, kArray* df)
//{
//	size_t i, j;
//	kBasicBlock *n;
//	for (i = 0; i < kArray_size(vertex); i++) {
//		fprintf(stderr, "[%02lu] DF={", i);
//		for (j = 0; j < kArray_size((kArray*)kArray_n(df, i)); j++) {
//			n = (kBasicBlock*)kArray_n((kArray*)kArray_n(df, i), j);
//			if (j == 0) {
//				fprintf(stderr, "%02d", DFN(n));
//			} else {
//				fprintf(stderr, ", %02d", DFN(n));
//			}
//		}
//		fprintf(stderr, "}\n");
//	}
//}
//
//static void knh_BasicBlock_printLiveness(kArray *liveIn, kArray *liveOut)
//{
//	size_t i, j;
//	for (i = 0; i < kArray_size(liveIn); i++) {
//		fprintf(stderr, "[%02d] liveIn={", (int)i);
//		for (j = 0; j < kArray_size(LIVEIN(i)); j++) {
//			if (j == 0) {
//				fprintf(stderr, "%02d", IArray_n(LIVEIN(i), 0));
//			} else {
//				fprintf(stderr, ", %02d", IArray_n(LIVEIN(i), j));
//			}
//		}
//		fprintf(stderr, "} liveOut={");
//		for (j = 0; j < kArray_size(LIVEOUT(i)); j++) {
//			if (j == 0) {
//				fprintf(stderr, "%02d", IArray_n(LIVEOUT(i), 0));
//			} else {
//				fprintf(stderr, ", %02d", IArray_n(LIVEOUT(i), j));
//			}
//		}
//		fprintf(stderr, "}\n");
//	}
//}
//
//static void knh_BasicBlock_printTree(CTX, kArray *vertex)
//{
//	size_t i;
//	for (i = 0; i < kArray_size(vertex); i++) {
//		knh_BasicBlock_print(_ctx, VERTEX(i));
//	}
//}
//
//static void printStack(kArray *stack)
//{
//	int i, j;
//	for (i = 0; i < (int)kArray_size(stack); i++) {
//		fprintf(stderr, "stack[%02d] {", i);
//		for (j = 0; j < (int)kArray_size((kArray*)kArray_n(stack, i)); j++) {
//			if (j == 0) {
//				fprintf(stderr, "%02d", IArray_n((kArray*)kArray_n(stack, i), j));
//			} else {
//				fprintf(stderr, ", %02d", IArray_n((kArray*)kArray_n(stack, i), j));
//			}
//		}
//		fprintf(stderr, "}\n");
//	}
//}
//
//static void printIArray(kArray *a)
//{
//	int i;
//	fprintf(stderr, "origidx {");
//	for (i = 0; i < (int)kArray_size(a); i++) {
//		if (i % 10 == 0) {
//			fprintf(stderr, "\n");
//		}
//		if (i == 0) {
//			fprintf(stderr, "%02d", IArray_n(a, i));
//		} else {
//			fprintf(stderr, ", %02d", IArray_n(a, i));
//		}
//	}
//	fprintf(stderr, "}\n");
//}
//
//#endif /* K_USING_DEBUG */
//
///* ------------------------------------------------------------------------ */
///* [convert to SSA form] */
//
//static void knh_BasicBlock_addPostbody(CTX/*, kArray *pred*/, kBasicBlock *bb, kBasicBlock *bbN)
//{
//	kBasicBlock bbtmp;
//	kBasicBlock *bbNEW = new_BasicBlockLABEL(_ctx);
//	// swap Block
//	bbtmp = *bbN;
//	*bbN = *bbNEW;
//	*bbNEW = bbtmp;
//	// add pred
//	//kArray_add(pred, new_Array(_ctx, CLASS_BasicBlock, 0));
//	if (bb->nextNC == bbN) {
//		bbN->jumpNC = bbNEW;
//		bb->nextNC = bbNEW;
//		DP(bb->nextNC)->incoming -= 1;
//	} else {
//		bbN->nextNC = bbNEW;
//		bb->jumpNC = bbNEW;
//		DP(bb->jumpNC)->incoming -= 1;
//	}
//}
//
///**
// * Do depth-first search(DFS) and number each block with
// * depth-first number(dfnum).
// * @param ctx    Context
// * @param p      Parent block
// * @param n      Target block
// * @param vertex Linear list of BasicBlocks(~= listNC)
// * @param pred   Predecessor of the block
// */
//static void knh_BasicBlock_DFS(CTX, kBasicBlock *p, kBasicBlock *n, kArray *vertex, kArray *pred)
//{
//	kBasicBlock *bbN, *bbJ;
//	BasicBlock_setVisited(n, 1);
//	DP(n)->id = kArray_size(vertex);
////#ifdef K_USING_DEBUG
////	knh_BasicBlock_print(_ctx, n);
////#endif
//	kArray_add(vertex, n);
//	bbN = n->nextNC;
//	bbJ = n->jumpNC;
//	if (p != NULL) {
//		kArray_add(PRED(n), p);
//		DP(n)->incoming = 1;
//	}
//	if (bbN != NULL) {
////		while (DP(bbN)->incoming > 3) {
////			DBG_P("add Postbody to [%02d]", DFN(n));
////			knh_BasicBlock_addPostbody(_ctx, n, n->nextNC);
////			bbN = n->nextNC;
////		}
//		if (!BasicBlock_isVisited(bbN)) {
//			knh_BasicBlock_DFS(_ctx, n, bbN, vertex, pred);
//		} else if (DP(bbN)->incoming < 2) {
//			kArray_add(PRED(bbN), n);
//			DP(bbN)->incoming += 1;
//		} else {
//			DBG_ASSERT(0);
//		}
//	}
//	if (bbJ != NULL) {
////		while (DP(bbJ)->incoming > 3) {
////			DBG_P("add Postbody to [%02d]", DFN(n));
////			knh_BasicBlock_addPostbody(_ctx, n, n->jumpNC);
////			bbJ = n->jumpNC;
////		}
//		if (!BasicBlock_isVisited(bbJ)) {
//			knh_BasicBlock_DFS(_ctx, n, bbJ, vertex, pred);
//		} else if (DP(bbJ)->incoming < 2) {
//			kArray_add(PRED(bbJ), n);
//			DP(bbJ)->incoming += 1;
//		} else {
//			DBG_ASSERT(0);
//		}
//	}
//}
//
///**
// * Get ancestor block with lowest Semidominator.
// * @param ctx      Context
// * @param vertex   Linear list of BasicBlocks(~= listNC)
// * @param v        Target block
// * @param ancestor Ancestor block in Control Flow Graph
// * @param best     A node with lowest dfnum from ancestor to v
// * @param semi     Semidominator of the block
// * @return         The ancestor block with lowest semidominator
// */
//static kBasicBlock* getAncestorWLS(CTX, kArray *vertex, kBasicBlock *v, kArray* ancestor, kArray* best, kArray* semi)
//{
//	kBasicBlock *a = ANCESTOR(v);
//	kBasicBlock *b = NULL;
//	if (ANCESTOR(a) != (kBasicBlock*)K_NULL) {
//		b = getAncestorWLS(_ctx, vertex, a, ancestor, best, semi);
//		KSETv(Array_BB(ancestor, v), ANCESTOR(a));
//		if (SEMI(b) < SEMI(BEST(v)))
//			KSETv(Array_BB(best, v), b);
//	}
//	return BEST(v);
//}
//
///**
// * Get parent BasicBlock.
// * @param vertex Linear list of BasicBlocks(~= listNC)
// * @param n      Target block
// * @return       Parent block if it exists
// *               or NULL if it does not exists.
// */
//static kBasicBlock* knh_BasicBlock_getParent(kArray *vertex, kBasicBlock *n)
//{
//	size_t i;
//	for (i = 0; i < kArray_size(vertex); i++) {
//		if (VERTEX(i)->nextNC == n || VERTEX(i)->jumpNC == n)
//			return VERTEX(i);
//	}
//	return NULL;
//}
//
///**
// * Set immediately dominator(IDOM) to each BasicBlock.
// * @param ctx    Context
// * @param vertex Linear list of BasicBlocks(~= listNC)
// * @param idom   Immediately dominator of the block
// * @param pred   Predecessor of the block
// */
//static void knh_setIdom(CTX, kArray *vertex, kArray* idom, kArray* pred)
//{
//	size_t i, j;
//	size_t size = kArray_size(vertex);
//
//	BEGIN_LOCAL(_ctx, lsfp, 5);
//	LOCAL_NEW(_ctx, lsfp, 0, kArray*, bucket  , new_Array(_ctx, CLASS_Array, 0));
//	LOCAL_NEW(_ctx, lsfp, 1, kArray*, semi    , new_Array(_ctx, CLASS_Int, 0));
//	LOCAL_NEW(_ctx, lsfp, 2, kArray*, ancestor, new_Array(_ctx, CLASS_BasicBlock, 0));
//	LOCAL_NEW(_ctx, lsfp, 3, kArray*, samedom , new_Array(_ctx, CLASS_BasicBlock, 0));
//	LOCAL_NEW(_ctx, lsfp, 4, kArray*, best    , new_Array(_ctx, CLASS_BasicBlock, 0));
//	kBasicBlock *n, *p, *s, *_s, *v, *y;
//
//	for (i = 0; i < size; i++) {
//		kArray_add(bucket, new_Array(_ctx, CLASS_BasicBlock, 0));
//		IArray_add(_ctx, semi, -1);
//		kArray_add(ancestor, K_NULL);
//		kArray_add(samedom, K_NULL);
//		kArray_add(best, K_NULL);
//	}
//
//	for (i = size - 1; i > 0; i--) {
//		n = VERTEX(i);
//		p = PARENT(n);
//		s = p;
//		for (j = 0; j < kArray_size(PRED(n)); j++) {
//			v = (kBasicBlock*)kArray_n(PRED(n), j);
//			//DBG_P("pred[%d]=%d", j, DFN(PRED(n, j)));
//			if (DFN(v) <= DFN(n)) {
//				_s = v;
//			} else {
//				_s = VERTEX(SEMI(getAncestorWLS(_ctx, vertex, v, ancestor, best, semi)));
//			}
//			if (DFN(_s) < DFN(s))
//				s = _s;
//		}
//		semi->ilist[DFN(n)] = DFN(s);
//		kArray_insert(_ctx, BUCKET(s), n);
//		//DBG_P("bucketsize[%02d] = %02d", DFN(s), kArray_size(BUCKET(s)));
//		LINK(p, n);
//		for (j = 0; j < kArray_size(BUCKET(p)); j++) {
//			v = (kBasicBlock*)kArray_n(BUCKET(p), j);
//			//DBG_P("bucket[%d]=%d", j, DFN(BUCKET(p, j)));
//			y = getAncestorWLS(_ctx, vertex, v, ancestor, best, semi);
//			if (SEMI(y) == SEMI(v)) {
//				KSETv(Array_BB(idom, v), p);
//			} else {
//				KSETv(Array_BB(samedom, v), y);
//			}
//		}
//		kArray_clear(BUCKET(p), 0);
//		//DBG_P("bucketsize[%02d] = %02d", DFN(p), kArray_size(BUCKET(p)));
//	}
//	for (i = 1; i < size; i++) {
//		n = VERTEX(i);
//		if (SAMEDOM(n) != (kBasicBlock*)K_NULL)
//			KSETv(Array_BB(idom, n), IDOM(SAMEDOM(n)));
//	}
//#ifdef K_USING_DEBUG
//	printIdom(_ctx, vertex, samedom, ancestor, semi, idom);
//#endif
//	END_LOCAL(_ctx, lsfp);
//}
//
///**
// * Check a block is dominated by another block.
// * @param ctx  Context
// * @param n    Target block
// * @param w    A block that might be dominate the target block
// * @param idom Immediately dominator of the block
// * @return     1 if n is dominated by w
// *             or 0 if n is not dominated by w
// */
//static int isDominated(CTX, kBasicBlock *n, kBasicBlock *w, kArray* idom)
//{
//	kBasicBlock *tmp;
//	for (tmp = IDOM(w); tmp != (kBasicBlock*)K_NULL; tmp = IDOM(tmp)) {
//		if (n == tmp)
//			return 1;
//	}
//	return 0;
//}
//
///**
// * Compute Dominance Frontier(DF).
// * @param ctx    Context
// * @param vertex Linear list of BasicBlocks(~= listNC)
// * @param n      Target block
// * @param df     Dominance Frontier of the block
// * @param idom   Immediately dominator of the block
// */
//static void computeDF(CTX, kArray *vertex, kBasicBlock *n, kArray *df, kArray *idom)
//{
//	size_t i, j;
//	size_t size = kArray_size(vertex);
//	kBasicBlock *w, *c;
//	BEGIN_LOCAL(_ctx, lsfp, 1);
//	LOCAL_NEW(_ctx, lsfp, 0, kArray*, S, new_Array(_ctx, CLASS_BasicBlock, 0));
//	//DBG_P("S = %p", S);
//	kBasicBlock *y;
//	y = n->nextNC;
//	if (y != NULL) {
//		if (IDOM(y) != n)
//			kArray_insert(_ctx, S, y);
//	}
//	y = n->jumpNC;
//	if (y != NULL) {
//		if (IDOM(y) != n)
//			kArray_insert(_ctx, S, y);
//	}
//	for (i = 0; i < size; i++) {
//		if (IDOM(VERTEX(i)) == n) {
//			c = VERTEX(i);
//			computeDF(_ctx, vertex, c, df, idom);
//			for (j = 0; j < kArray_size(DF(c)); j++) {
//				w = (kBasicBlock*)kArray_n(DF(c), j);
//				if (!isDominated(_ctx, n, w, idom))
//					kArray_insert(_ctx, S, w);
//			}
//		}
//	}
//	kArray_copy(_ctx, DF(n), S);
//	END_LOCAL(_ctx, lsfp);
//}
//
///**
// * Set a list of variable that defined each block n.
// * @param ctx   Context
// * @param Aorig A list of variable that defined each block
// * @param n     Target block
// */
//static void knh_setOrig(CTX, kArray *Aorig, kBasicBlock *n)
//{
//	int i, size;
//	kopl_t *op;
//	size = DP(n)->size;
//	for (i = 0; i < size; i++) {
//		op = DP(n)->opbuf + i;
//		if (hasassign(op->opcode)) {
//			if (op->opcode == OPCODE_iINC) {
//				op->opcode = OPCODE_iADDn;
//				op->data[1] = op->data[0];
//				op->data[2] = 1;
//			} else if (op->opcode == OPCODE_iDEC) {
//				op->opcode = OPCODE_iSUBn;
//				op->data[1] = op->data[0];
//				op->data[2] = 1;
//			}
//			if ((int)op->data[0] >= 0) {
//				DBG_P("Aorig[%02d] add r%d %s", DFN(n), op->data[0], tochar(op->opcode));
//				IArray_add(_ctx, Aorig, op->data[0]);
//			}
//		}
//	}
//}
//
///**
// * Insert phi function.
// * @param ctx  Context
// * @param n    Target block
// * @param idx  An index of register
// * @param argc A number of Predecessor
// */
//static void insertPhi(CTX, kBasicBlock *n, int idx)
//{
//	size_t size = DP(n)->size;
//	klr_PHI_t phi = {TADDR, OPCODE_PHI, ASMLINE, idx, idx, idx};
//	DBG_P("[%02d] insert phifunc of r%d", DFN(n), idx);
//	// at this point insert NOP instead of phifunc
//	if (size > 0) {
//		kopl_t *buf;
//		size_t bsize = size * sizeof(kopl_t);
//		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
//		BasicBlock_add(_ctx, n, 0, (kopl_t*)(&phi));
//		buf = DP(n)->opbuf;
//		knh_Bytes_write(_ctx, cwb->ba, new_bytes2((char *)&phi, sizeof(kopl_t)));
//		knh_Bytes_write(_ctx, cwb->ba, new_bytes2((char *)buf, bsize));
//		knh_memcpy(buf, CWB_totext(_ctx, cwb), CWB_size(cwb));
//		CWB_close(cwb);
//	}
//	else {
//		BasicBlock_add(_ctx, n, 0, (kopl_t*)(&phi));
//	}
//}
//
///**
// * Put phi function to BasicBlock.
// * @param ctx    Context
// * @param vertex Linear list of BasicBlocks(~= listNC)
// * @param df     Dominance Frontier of the block
// * @param max    Max index of register
// */
//static void putPhifuncion(CTX, kArray *vertex, kArray* df, int *max)
//{
//	int a;
//	size_t i, j, size = kArray_size(vertex);
//	kBasicBlock *n, *y;
//	BEGIN_LOCAL(_ctx, lsfp, 4);
//	LOCAL_NEW(_ctx, lsfp, 0, kArray*, Aorig, new_Array(_ctx, CLASS_Array, 0));
//	LOCAL_NEW(_ctx, lsfp, 1, kArray*, Aphi, new_Array(_ctx, CLASS_Array, 0));
//	LOCAL_NEW(_ctx, lsfp, 2, kArray*, defsites, new_Array(_ctx, CLASS_Array, 0));
//	LOCAL_NEW(_ctx, lsfp, 3, kArray*, W, new_Array(_ctx, CLASS_BasicBlock, 0));
//	for (i = 0; i < size; i++) {
//		n = VERTEX(i);
//		kArray_add(Aorig, new_Array(_ctx, CLASS_Int, 0));
//		if (DP(n)->size > 0) {
//			knh_setOrig(_ctx, AORIG(i), n);
//			for (j = 0; j < kArray_size(AORIG(i)); j++) {
//				a = IArray_n(AORIG(i), j);
//				while ((int)kArray_size(defsites) <= a) {
//					DBG_P("add%d", a);
//					kArray_add(defsites, new_Array(_ctx, CLASS_BasicBlock, 0));
//				}
//				kArray_insert(_ctx, DEFSITES(a), n);
//			}
//		}
//	}
//	*max = (int)kArray_size(defsites);
//	DBG_P("max=%d", *max);
//	for (a = 0; a < (int)kArray_size(defsites); a++) {
//		kArray_add(Aphi, new_Array(_ctx, CLASS_BasicBlock, 0));
//		if (kArray_size(DEFSITES(a)) == 0)
//			continue;
//		kArray_copy(_ctx, W, DEFSITES(a));
//		while (kArray_size(W) > 0) {
//			n = (kBasicBlock*)kArray_n(W, kArray_size(W) - 1);
//			kArray_clear(W, kArray_size(W) - 1);
//			for (i = 0; i < kArray_size(DF(n)); i++) {
//				y = (kBasicBlock*)kArray_n(DF(n), i);
//				if (!Array_isContain(APHI(a), y)) {
//					DBG_ASSERT(DP(y)->incoming == 2);
//					insertPhi(_ctx, y, a);
//					kArray_insert(_ctx, APHI(a), y);
//					knh_setOrig(_ctx, AORIG(DFN(y)), y);
//					if (!IArray_isContain(AORIG(DFN(y)), a))
//						kArray_insert(_ctx, W, y);
//				}
//			}
//		}
//	}
//	END_LOCAL(_ctx, lsfp);
//}
//
///**
// * Get Predecessor number
// * @param pred Predecessor of the block
// * @param p    Parent block
// * @param n    Target block
// * @return     An index of p in pred.
// */
//static int getPredNum(kArray *pred, kBasicBlock *p, kBasicBlock *n)
//{
//	size_t i;
//	for (i = 0; i < kArray_size(PRED(n)); i++) {
//		if (kArray_n(PRED(n), i) == (kObject*)p)
//			return i;
//	}
//	return -1;
//}
//
//static void extendLiveness(CTX, kArray *liveIn, kArray *liveOut, int idx, kBasicBlock *n)
//{
//	int i;
//	for (i = DFN(n); i > 0; i--) {
//		if (IArray_isContain(LIVEIN(i), idx)) {
//			//DBG_P("extend r%d from [%02d] to in[%02d]", idx, DFN(n), i);
//			return;
//		} else {
//			IArray_add(_ctx, LIVEIN(i), idx);
//		}
//		if (IArray_isContain(LIVEOUT(i - 1), idx)) {
//			//DBG_P("extend r%d from [%02d] to out[%02d]", idx, DFN(n), i - 1);
//			return;
//		} else {
//			IArray_add(_ctx, LIVEOUT(i - 1), idx);
//		}
//	}
//	IArray_add(_ctx, LIVEIN(0), idx);
//	//DBG_P("extend r%d from [%02d] to begin[%02d]", idx, DFN(n), 0);
//}
//
///**
// * Rename variable to make SSA form.
// * @param ctx    Cotnext
// * @param vertex Linear list of BasicBlocks(~= listNC)
// * @param n      Target block
// * @param stack  A stack that records rename in each variable
// * @param idx    An index of register
// * @param pred   Predecessor of the block
// * @param idom   Immediately dominator of the block
// */
//static void knh_BasicBlock_renameVar(CTX, kArray *vertex, kBasicBlock *n, kArray *stack, kArray *origidx, kArray *pred, kArray *idom, kArray *liveIn, kArray *liveOut, int *idx)
//{
//	size_t i, j, shift, count;
//	kopl_t *s, *op;
//	kBasicBlock *x ,*y;
//	for (i = 0; i < DP(n)->size; i++) {
//		s = DP(n)->opbuf + i;
//		if (s->opcode != OPCODE_PHI && !ISCALL(s->opcode) && s->opcode != OPCODE_FASTCALL0) {
//			if (s->opcode == OPCODE_XIMOV) {
//				DBG_P("do nothing %s %d", tochar(s->opcode), s->data[0]);
//			} else if (s->opcode == OPCODE_iMOVx) {
//				DBG_P("do nothing %s %d", tochar(s->opcode), s->data[1]);
//			} else if (s->opcode == OPCODE_P) {
//				if (s->data[4] != 0) {
//					DBG_P("replace USE %s %d to %d", tochar(s->opcode), s->data[4], (TOP(STACK(s->data[4] * 2 + 1)) - 1) / 2);
//					s->data[4] = (TOP(STACK(s->data[4] * 2 + 1)) - 1) / 2;
//					//extendLiveness(_ctx, liveIn, liveOut, s->data[4] * 2 + 1, n);
//				} else {
//					DBG_P("Don't replace USE %s %d", tochar(s->opcode), s->data[4]);
//				}
//				// todo
//			}
//			size_t osize = kopcode_size(s->opcode);
//			for (j = 1; j < osize; j++) {
//				if (kopcode_usedef(s->opcode, j)) {
//					DBG_P("replace USE %s %d to %d", tochar(s->opcode), s->data[j], TOP(STACK(s->data[j])));
//					s->data[j] = TOP(STACK(s->data[j]));
//					//extendLiveness(_ctx, liveIn, liveOut, s->data[j], n);
//				}
//			}
//		}
//		if (hasassign(s->opcode) || s->opcode == OPCODE_PHI) {
//			if (s->data[0] < 0) {
//				DBG_P("Don't replace DEF %s %d", tochar(s->opcode), s->data[0]);
//			} else if (!ISCALL(s->opcode)) {
//				if (ISOBJ(s->data[0] + *idx)) {
//					*idx += 2;
//					IArray_add(_ctx, origidx, -1);
//				} else {
//					*idx += 1;
//				}
//				IArray_add(_ctx, origidx, s->data[0]);
//				DBG_P("replace DEF %s %d to %d", tochar(s->opcode), s->data[0], *idx);
//				PUSH(STACK(s->data[0]), *idx);
//				s->data[0] = *idx;
//				if (s->opcode == OPCODE_PHI) {
//					//DBG_P("define r%d in [%02d]", (int)s->data[0], DFN(n));
//					IArray_add(_ctx, LIVEIN(DFN(n)), s->data[0]);
//				}
//			} else {
//				for (count = 0; count < i; count++) {
//					op = s - count - 1;
//					if (!hasassign(op->opcode) || kArray_n(origidx, op->data[0]) < s->data[1] || kArray_n(origidx, op->data[0]) >= s->data[2])
//						break;
//				}
//				if (count > 0) {
//					DBG_P("%s argc=%02d", tochar(s->opcode), count);
//					op = s - count;
//					if (ISOBJ(s->data[0] + op->data[0])) {
//						DBG_P("replace DEF %s %d to %d", tochar(s->opcode), s->data[0], op->data[0]);
//						shift = op->data[0] - s->data[0];
//					} else if (IArray_n(origidx, op->data[0] - 1) == -1) {
//						DBG_P("replace DEF %s %d to %d", tochar(s->opcode), s->data[0], op->data[0] - 1);
//						shift = op->data[0] - s->data[0] - 1;
//						//origidx->ilist[op->data[0]] = -1;
//					} else {
//						DBG_P("replace DEF %s %d to %d", tochar(s->opcode), s->data[0], op->data[0] + 1);
//						shift = op->data[0] - s->data[0] + 1;
//						//origidx->ilist[op->data[0]] = -1;
//					}
//					PUSH(STACK(s->data[0]), shift + s->data[0]);
//					DBG_P("shift=%02d", shift);
//					s->data[0] += shift;
//					*idx = s->data[0];
//					for (j = 1; j <= count; j++) {
//						op = s - j;
//						DBG_P("shift DEF %s %d to %d", tochar(op->opcode), op->data[0], shift + IArray_n(origidx, op->data[0]));
//						IArray_pop(_ctx, STACK(op->data[0]));
//						op->data[0] = shift + IArray_n(origidx, op->data[0]);
//					}
//					//DBG_P("origidx=%d, s->data=%d", kArray_size(origidx), s->data[0]);
//					while (kArray_size(origidx) >= s->data[0] && kArray_size(origidx) > kArray_size(stack)) {
//						IArray_pop(_ctx, origidx);
//					}
//					while (kArray_size(origidx) < s->data[0]) {
//						IArray_add(_ctx, origidx, -1);
//					}
//					IArray_add(_ctx, origidx, s->data[0] - shift);
//				} else {
//					if (ISOBJ(s->data[0] + *idx)) {
//						*idx += 2;
//						IArray_add(_ctx, origidx, -1);
//					} else {
//						*idx += 1;
//					}
//					IArray_add(_ctx, origidx, s->data[0]);
//					DBG_P("replace DEF %s %d to %d", tochar(s->opcode), s->data[0], *idx);
//					PUSH(STACK(s->data[0]), *idx);
//					shift = *idx - s->data[0];
//					s->data[0] = *idx;
//				}
//				size_t osize = kopcode_size(s->opcode);
//				for (j = 1; j < osize; j++) {
//					if (kopcode_usedef(s->opcode, j)) {
//						DBG_P("shift USE %s %d to %d", tochar(s->opcode), s->data[j], s->data[j] + shift);
//						s->data[j] += shift;
//					}
//				}
//			}
////			if (s->opcode == OPCODE_TR) {
////				DBG_P("replace USE %s %d to %d", tochar(s->opcode), s->data[1], s->data[0]);
////				s->data[1] = s->data[0];
////			}
//			 if (s->opcode == OPCODE_FASTCALL0) {
//				DBG_P("replace USE %s %d to %d", tochar(s->opcode), s->data[1], s->data[0] - 1);
//				s->data[1] = s->data[0] - 1;
//				DBG_P("replace RIX %s %d to %d", tochar(s->opcode), s->data[2], 0);
//				s->data[2] = 0;
//			}
//		}
//	}
//#ifdef K_USING_DEBUG
//	DBG_P("start %02d", DFN(n));
//	printIArray(origidx);
//	printStack(stack);
//#endif
//	y = n->nextNC;
//	if (y != NULL) {
//		j = getPredNum(pred, n, y) + 1;
//		for (i = 0; i < DP(y)->size; i++) {
//			s = DP(y)->opbuf + i;
//			if (s->opcode == OPCODE_PHI) {
//				DBG_P("replace USE %s %d to %d", tochar(s->opcode), s->data[j], TOP(STACK(s->data[j])));
//				s->data[j] = TOP(STACK(s->data[j]));
//
////				extendLiveness(_ctx, vertex, liveIn, liveOut, s->data[j], y);
//				DBG_P("phi source r%d is live at end of [%02d]", s->data[j], DFN(n));
//				IArray_add(_ctx, LIVEOUT(DFN(n)), (int)s->data[j]);
//			} else {
//				break;
//			}
//		}
//	}
//	y = n->jumpNC;
//	if (y != NULL) {
//		j = getPredNum(pred, n, y) + 1;
//		for (i = 0; i < DP(y)->size; i++) {
//			s = DP(y)->opbuf + i;
//			if (s->opcode == OPCODE_PHI) {
//				DBG_P("replace USE %s %d to %d", tochar(s->opcode), s->data[j], TOP(STACK(s->data[j])));
//				s->data[j] = TOP(STACK(s->data[j]));
//			} else {
//				break;
//			}
//		}
//	}
//	for (i = 0; i < kArray_size(vertex); i++) {
//		x = (kBasicBlock*)VERTEX(i);
//		if (IDOM(x) == n) {
//			knh_BasicBlock_renameVar(_ctx, vertex, x, stack, origidx, pred, idom, liveIn, liveOut, idx);
//		}
//	}
//	for (i = 0; i < DP(n)->size; i++) {
//		s = DP(n)->opbuf + i;
//		if (hasassign(s->opcode) || s->opcode == OPCODE_PHI) {
//			if ((int)s->data[0] > 0 && (int)origidx->ilist[s->data[0]] != -1 && (int)s->data[0] <= *idx) {
//				DBG_P("pop %02d", s->data[0]);
//				IArray_pop(_ctx, STACK(s->data[0]));
//				origidx->ilist[s->data[0]] = -1;
//			} else {
//				DBG_P("don't pop %d", s->data[0]);
//			}
//		}
//	}
//#ifdef K_USING_DEBUG
//	DBG_P("end %02d", DFN(n));
//	printIArray(origidx);
//	printStack(stack);
//#endif
//}
//
//static void BasicBlock_setUnvisited(CTX, kArray *vertex) {
//	size_t i;
//	for (i = 0; i < kArray_size(vertex); i++) {
//#ifdef K_USING_DEBUG
//		knh_BasicBlock_print(_ctx, VERTEX(i));
//#endif
//		BasicBlock_setVisited(VERTEX(i), 0);
//	}
//}
//
///**
// * Convert to SSA form.
// * @param ctx    Context
// * @param bb     BasicBlock
// * @param df     Dominance Frontier of the block
// * @param pred   Predecessor of the block
// * @param idom   Immediately dominator of the block
// * @param vertex Linear list of BasicBlocks(~= listNC)
// * @param stack  A stack that records rename in each variable
// * @param max    Max index of register
// */
//static void knh_BasicBlock_convert(CTX, kBasicBlock *bb, kArray *df, kArray *pred, kArray *idom, kArray *vertex, kArray *liveIn, kArray *liveOut, int *max)
//{
//	int i, idx = 0;
//	knh_BasicBlock_DFS(_ctx, NULL, bb, vertex, pred);
//#ifdef K_USING_DEBUG
//	knh_BasicBlock_printTree(_ctx, vertex);
//#endif
//	knh_setIdom(_ctx, vertex, idom, pred);
//	computeDF(_ctx, vertex, bb, df, idom);
//#ifdef K_USING_DEBUG
//	printDF(vertex, df);
//#endif
//	putPhifuncion(_ctx, vertex, df, &idx);
//	*max = idx;
//	BEGIN_LOCAL(_ctx, lsfp, 2);
//	LOCAL_NEW(_ctx, lsfp, 0, kArray*, stack, new_Array(_ctx, CLASS_Array, 0));
//	LOCAL_NEW(_ctx, lsfp, 1, kArray*, origidx, new_Array(_ctx, CLASS_Int, 0));
//	for (i = 0; i < *max; i++) {
//		kArray_add(stack, new_Array(_ctx, CLASS_Int, 0));
//		IArray_add(_ctx, (kArray*)kArray_n(stack, i), i);
//		IArray_add(_ctx, origidx, i);
//	}
//	if ((int)kArray_size(stack) > 0) {
//		idx -= 1;
//		knh_BasicBlock_renameVar(_ctx, vertex, bb, stack, origidx, pred, idom, liveIn, liveOut, &idx);
//	} else {
//		DBG_P("Don't rename!");
//	}
//	*max = kArray_size(origidx);
//
//#ifdef K_USING_DEBUG
//	knh_BasicBlock_printLiveness(liveIn, liveOut);
//	BasicBlock_setUnvisited(_ctx, vertex);
//#endif
//	END_LOCAL(_ctx, lsfp);
//}
//
///* ------------------------------------------------------------------------ */
///* [convert out of SSA form] */
//
//static int isInterfering(int isSource, int a, int b, kArray *liveIn, kArray *liveOut)
//{
//	if (isSource) {
//	}
//	return 0;
//}
//
//static int isSameArray(kArray *a, kArray *b)
//{
//	size_t i = 0;
//	size_t j = 0;
//	if (kArray_size(a) == kArray_size(b)) {
//		while (i < kArray_size(a)) {
//			while (j < kArray_size(b)) {
//				if (kArray_n(a, i) == kArray_n(b, j)) {
//					i++;
//					b = 0;
//				} else {
//					j++;
//				}
//			}
//			return 0;
//		}
//		return 1;
//	}
//	return 0;
//}
//
//
///**
// * Eliminate phi resource interferences based on
// * data-flow and interference graph updates.
// */
//static void eliminatePRI(CTX, kBasicBlock *bb, kArray *liveIn, kArray *liveOut, kArray *pcc, kArray *unm, int max)
//{
//	int c, i0, j0, i1, j1, xi, xj, yi, yj;
//	kopl_t *op;
//	BEGIN_LOCAL(_ctx, lsfp, 1);
//	// candidateResourceSet
//	LOCAL_NEW(_ctx, lsfp, 0, kArray *, crs, new_Array(_ctx, CLASS_Array, 0));
//#ifdef K_USING_DEBUG
//	knh_BasicBlock_print(_ctx, bb);
//#endif
////	for (c = 0; c < DP(bb)->size; c++) {
////		op = DP(bb)->opbuf + c;
////		if (op->opcode == OPCODE_PHI) {
////			for (i0 = 0; i0 < 2; i0++) {
////				for (j0 = i0 + 1; j0 < 3; j0++) {
////					DBG_P("bb[%02d]", DFN(bb));
////					xi = op->data[i0];
////					DBG_P("x%d=%d", i0, xi);
////					xj = op->data[j0];
////					DBG_P("y%d=%d", j0, yi);
////					for (i1 = 0; i1 < kArray_size(PCC(xi)); i1++) {
////						for (j1 = 0; j1 < kArray_size(PCC(xj)); j1++) {
////							if (!isSameArray(PCC(xi), PCC(xj))) {
////								yi = IArray_n(PCC(xi), i1);
////								yj = IArray_n(PCC(xj), j1);
////								if (isInterfering(i0, yi, yj, liveIn, liveOut)) {
////
////								}
////							}
////						}
////					}
////				}
////			}
////		}
////	}
//	if (bb->nextNC != NULL && BasicBlock_isVisited(bb->nextNC))
//		eliminatePRI(_ctx, bb->nextNC, liveIn, liveOut, pcc, unm, max);
//	if (bb->jumpNC != NULL && BasicBlock_isVisited(bb->jumpNC))
//		eliminatePRI(_ctx, bb->jumpNC, liveIn, liveOut, pcc, unm, max);
//	END_LOCAL(_ctx, lsfp);
//}
//
///**
// * Convert out of SSA form.
// * @param ctx Context
// * @param bb  BasicBlock
// */
//static void phi_elimination_simple(CTX, kArray *vertex, void *data)
//{
//	struct phidata *pdata = (struct phidata*)data;
//	int i, j, k;
//	kopl_t *op;
//	kBasicBlock *target, *bb;
//	kArray *pred    = pdata->pred;
//	kArray *liveIn  = pdata->liveIn;
//	kArray *liveOut = pdata->liveOut;
//	int max = pdata->max;
////#ifdef K_USING_DEBUG
////	knh_BasicBlock_print(_ctx, bb);
////#endif
//	BEGIN_LOCAL(_ctx, lsfp, 2);
//	// phiCongruenceClass
//	LOCAL_NEW(_ctx, lsfp, 0, kArray *, pcc, new_Array(_ctx, CLASS_Array, 0));
//	// unresolvedNaighborMap
//	LOCAL_NEW(_ctx, lsfp, 1, kArray *, unm, new_Array(_ctx, CLASS_Array, 0));
//	for (i = 0; i < max; i++) {
//		kArray_add(pcc, new_Array(_ctx, CLASS_Int, 0));
//		IArray_add(_ctx, PCC(i), i);
//		kArray_add(unm, new_Array(_ctx, CLASS_Int, 0));
//	}
////	eliminatePRI(_ctx, bb, liveIn, liveOut, pcc, unm, max);
//	for (i = 0; i < (int)kArray_size(vertex); i++) {
//		bb = VERTEX(i);
//		for (j = 0; j < (int)DP(bb)->size; j++) {
//			op = DP(bb)->opbuf + j;
//			if (op->opcode == OPCODE_PHI) {
//				for (k = 0; k <= 1; k++) {
//					target = (kBasicBlock*)kArray_n((kArray*)PRED(bb), k);
//					if (ISOBJ(op->data[0])) {
//						klr_OMOV_t omov = {TADDR, OPCODE_OMOV, ASMLINE, op->data[0], op->data[k + 1]};
//						DBG_P("[%02d] add OMOV r%d r%d", DFN(target), op->data[0], op->data[k + 1]);
//						BasicBlock_add(_ctx, target, 0, (kopl_t*)(&omov));
//					} else {
//						klr_NMOV_t nmov = {TADDR, OPCODE_NMOV, ASMLINE, op->data[0], op->data[k + 1]};
//						DBG_P("[%02d] add NMOV r%d r%d", DFN(target), op->data[0], op->data[k + 1]);
//						BasicBlock_add(_ctx, target, 0, (kopl_t*)(&nmov));
//					}
//					if (target->nextNC != NULL && target->jumpNC != NULL) {
//						kopl_t *opLAST = DP(target)->opbuf + DP(target)->size - 1;
//						SWAP(opLAST, opLAST - 1, sizeof(kopl_t));
//					}
//				}
//				op->opcode = OPCODE_NOP;
//			}
//		}
//	}
//	END_LOCAL(_ctx, lsfp);
//}
//
//
//
////#define K_USING_SSA_OPT
//#include "opt/nop.c"
//#include "opt/cfold.c"
//#include "opt/peephole.c"
//
//typedef void (*fbbopt)(CTX, kBasicBlock *bb, kArray *data, int mask);
///* ------------------------------------------------------------------------ */
///**
// * Optimize the BasicBlocks.
// * @param ctx Context
// * @param bbs BasicBlocks
// */
//static void ssa_optimize(CTX, kBasicBlock *bbs)
//{
//#ifdef K_USING_SSA_OPT
//	struct opt {
//		const char *name;
//		fbbopt opt;
//	} opts[] = {
//#ifdef USE_OPT_SIMPLE_CONSTANT_FOLDING
//		DEF_SIMPLE_CONSTANT_FOLDING,
//#endif
//#ifdef USE_OPT_NULL
//		DEF_NULL,
//#endif
//	};
//	int flag = BasicBlock_isVisited(bbs);
//	size_t i;
//	size_t size = (int)(sizeof(x) / sizeof((x)[0]));
//	BEGIN_LOCAL(_ctx, lsfp, 1);
//	LOCAL_NEW(_ctx, lsfp, 0, kArray *, data, new_Array(_ctx, CLASS_Int, 0));
//	for (i = 0; i < size; i++) {
//		DBG_P("opt:%s", opts[i].name);
//		flag = !flag;
//		opts[i].opt(_ctx, bbs, data, flag);
//	}
//	if (size % 2 == 1) 
//		opt_nop(_ctx, bbs, data, !flag);
//	END_LOCAL(_ctx, lsfp);
//#endif
//}
//
///* ------------------------------------------------------------------------ */
///* [SSA optimize] */
//
///**
// * Optimize the KLRcode.
// * @param ctx Context
// * @param bb  BasicBlock
// */
//void knh_BasicBlock_optimize(CTX, kBasicBlock *bb)
//{
//#if 1
//	int max;
//	size_t i, size = kArray_size(bb->listNC);
//	BEGIN_LOCAL(_ctx, lsfp, 6);
//	LOCAL_NEW(_ctx, lsfp, 0, kArray*, df, new_Array(_ctx, CLASS_Array, 0));
//	LOCAL_NEW(_ctx, lsfp, 1, kArray*, pred, new_Array(_ctx, CLASS_Array, 0));
//	LOCAL_NEW(_ctx, lsfp, 2, kArray*, idom, new_Array(_ctx, CLASS_BasicBlock, 0));
//	LOCAL_NEW(_ctx, lsfp, 3, kArray*, vertex, new_Array(_ctx, CLASS_BasicBlock, 0));
//	LOCAL_NEW(_ctx, lsfp, 4, kArray*, liveIn, new_Array(_ctx, CLASS_Array, 0));
//	LOCAL_NEW(_ctx, lsfp, 5, kArray*, liveOut, new_Array(_ctx, CLASS_Array, 0));
//
//	//DBG_P("DF = %p", df);
//	for (i = 0; i < size; i++) {
//		kArray_add(df, new_Array(_ctx, CLASS_BasicBlock, 0));
//		kArray_add(pred, new_Array(_ctx, CLASS_BasicBlock, 0));
//		kArray_add(idom, K_NULL);
//		kArray_add(liveIn, new_Array(_ctx, CLASS_Int, 0));
//		kArray_add(liveOut, new_Array(_ctx, CLASS_Int, 0));
//	}
//	knh_BasicBlock_convert(_ctx, bb, df, pred, idom, vertex, liveIn, liveOut, &max);
//	//ssa_optimize(_ctx, bb);
//	{
//		struct phidata pdata = {pred, liveIn, liveOut, max};
//		DBG_P("max=%d", max);
//		phi_elimination_simple(_ctx, vertex, &pdata);
//#ifdef USE_OPT_PEEPHOLE
//		opt_peephole(_ctx, vertex, 0);
//#endif
//#ifdef USE_OPT_REMOVE_NOPCODE
//		opt_remove_nopcode(_ctx, vertex, NULL);
//#endif
//	}
//	//BasicBlock_setUnvisited(_ctx, vertex);
//	END_LOCAL(_ctx, lsfp);
//#endif
//}
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif /* K_USING_SSA */
