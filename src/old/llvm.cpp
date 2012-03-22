/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2011 Masahiro Ide <imasahiro9 at gmail.com>
 *           (c) 2011 Shunsuke Shida
 *           (c) 2011 Yuuki Wakamatsu
 *           (c) 2006-2011, Kimio Kuramitsu <kimio at ynu.ac.jp>
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

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Constants.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Function.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/system_error.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Target/TargetData.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Transforms/Scalar.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>

#undef HAVE_SYS_TYPES_H
#undef HAVE_SYS_STAT_H
#undef HAVE_UNISTD_H
#undef HAVE_SYS_TIME_H
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_VERSION
#include "commons.h"
#define ASM_PREFIX llvmasm::
#define LLVM_TODO(str) {\
	fprintf(stderr, "(TODO: %s %d):", __func__, __LINE__);\
	fprintf(stderr, "%s\n", str);\
	asm volatile("int3");\
}
#define LLVM_WARN(str) {\
	fprintf(stderr, "(WARN: %s %d):", __func__, __LINE__);\
	fprintf(stderr, "%s\n", str);\
}

#define _UNUSED_ __attribute__((unused))

/* FIXME remove IS_Tnumbox */
#define IS_Tnumbox(t)     (t == CLASS_Object || t == CLASS_Number)

/* ************************************************************************ */

namespace llvmasm {
using namespace llvm;

#define ASM(X, ...)
static void Tn_asm(CTX ctx, knh_Stmt_t *stmt, size_t n, knh_type_t reqt, int local);
static int _BLOCK_asm(CTX ctx, knh_Stmt_t *stmtH, knh_type_t reqt, int sfpidx);

struct label_stack {
	BasicBlock *bbCon;
	BasicBlock *bbBreak;
	knh_Array_t *phiCon;
	knh_Array_t *phiBreak;
};

static ExecutionEngine *ee_global;
static Module *mod_global;
static inline ExecutionEngine *LLVM_EE(CTX ctx) {
	return ee_global;
}
class AsmContext {
public:
	llvm::Module      *m_;
	CTX ctx_;
	knh_Method_t *mtd_;
	llvm::Function    *F_;
	llvm::IRBuilder<> *B_;
	std::vector<struct label_stack*> *lstacks;
	AsmContext(Module *m, CTX ctx, knh_Method_t *mtd);
	void Finish(CTX ctx, knh_Method_t *mtd);
	~AsmContext();
};

static inline Module *LLVM_MODULE(CTX ctx)
{
	AsmContext *lctx = (AsmContext*) DP(ctx->gma)->asm_data;
	Module *m = lctx->m_;
	return m;
}
static inline Function *LLVM_FUNCTION(CTX ctx)
{
	AsmContext *lctx = (AsmContext*) DP(ctx->gma)->asm_data;
	return lctx->F_;
}
static inline IRBuilder<> *LLVM_BUILDER(CTX ctx)
{
	AsmContext *lctx = (AsmContext*) DP(ctx->gma)->asm_data;
	return lctx->B_;
}
static inline std::vector<label_stack*> *LLVM_BBSTACK(CTX ctx)
{
	AsmContext *lctx = (AsmContext*) DP(ctx->gma)->asm_data;
	return lctx->lstacks;
}

static void PUSH_LABEL(CTX ctx, struct label_stack *l)
{
	std::vector<label_stack *> *bbstack = LLVM_BBSTACK(ctx);
	bbstack->push_back(l);
}

static void POP_LABEL(CTX ctx)
{
	std::vector<label_stack *> *bbstack = LLVM_BBSTACK(ctx);
	bbstack->back();
	bbstack->pop_back();
}

#define NC_(sfpidx)    (((sfpidx) * 2) + 1)
#define OC_(sfpidx)    ((sfpidx) * 2)
#define TC_(type, i) ((IS_Tunbox(type))?(NC_(i)):(OC_(i)))

#define SFP_(sfpidx)   ((sfpidx) * 2)

#define RIX_(rix)      rix

/* ------------------------------------------------------------------------ */
/* [Gamma] */

static Value *getctx(CTX ctx)
{
		Function::arg_iterator args = LLVM_FUNCTION(ctx)->arg_begin();
		return args;
}
static Value *getsfp(CTX ctx)
{
		Function::arg_iterator args = LLVM_FUNCTION(ctx)->arg_begin();
		args++;/* ctx */
		return args;
}
static Value *VNAME_(Value *v, const char *name)
{
	v->setName(name);
	return v;
}

static Value *loadsfp(CTX ctx, IRBuilder<> *builder, Value *v, knh_type_t type, int idx0);

#define LLVM_CONTEXT() (llvm::getGlobalContext())
#define LLVMTYPE_Void  (Type::getVoidTy(LLVM_CONTEXT()))
#define LLVMTYPE_Int   (Type::getInt64Ty(LLVM_CONTEXT()))
#define LLVMTYPE_Bool  (Type::getInt64Ty(LLVM_CONTEXT()))
#define LLVMTYPE_Float (Type::getDoubleTy(LLVM_CONTEXT()))
static const Type *LLVMTYPE_ObjectField = NULL;
static const Type *LLVMTYPE_hObject = NULL;
static const Type *LLVMTYPE_Object = NULL;
static const Type *LLVMTYPE_Array = NULL;
static const Type *LLVMTYPE_OutputStream = NULL;
static const Type *LLVMTYPE_InputStream = NULL;
static const Type *LLVMTYPE_Method = NULL;
static const Type *LLVMTYPE_context = NULL;
static const Type *LLVMTYPE_fcall = NULL;
static const Type *LLVMTYPE_checkin = NULL;
static const Type *LLVMTYPE_checkout = NULL;
static const Type *LLVMTYPE_sfp   = NULL;
static const Type *LLVMTYPE_itr   = NULL;
static const Type *LLVMTYPE_Iterator = NULL;
static const Type *LLVMTYPE_SystemEX = NULL;
static const Type *LLVMTYPE_System   = NULL;

static void ValueStack_set(CTX ctx, int index, Value *v);
static Value *ValueStack_get(CTX ctx, int index)
{
	knh_Array_t *lstacks = DP(ctx->gma)->lstacks;
	knh_sfp_t lsfp = {};
	index = index + (-1 * K_RTNIDX);
	lsfp.a = lstacks;
	lstacks->api->fastget(ctx, &lsfp, index, 0);
	return (Value*) lsfp.ndata;
}

static Value *ValueStack_load(CTX ctx, int index, knh_class_t cid)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *v = loadsfp(ctx, builder, getsfp(ctx), cid, index);
	v = builder->CreateLoad(v);
	return v;
}

static Value *ValueStack_load_set(CTX ctx, int index, knh_class_t cid)
{
	Value *v = ValueStack_load(ctx, index, cid);
	ValueStack_set(ctx, index, v);
	return v;
}

static Value *ValueStack_get_or_load(CTX ctx, int index, knh_class_t cid)
{
	Value *v = ValueStack_get(ctx, index);
	if (!v) {
		v = ValueStack_load(ctx, index, cid);
	}
	return v;
}

#define knh_Array_capacity(a) ((a)->dim->capacity)
static void ValueStack_set(CTX ctx, int index, Value *v)
{
	knh_Array_t *lstacks = DP(ctx->gma)->lstacks;
	knh_sfp_t lsfp = {};
	index = index + (-1 * K_RTNIDX);
	if (v->getType() == Type::getInt1Ty(LLVM_CONTEXT())) {
		v = LLVM_BUILDER(ctx)->CreateZExt(v, LLVMTYPE_Int);
	}
	if ((int)knh_Array_capacity(lstacks) < index) {
		knh_Array_grow(ctx, lstacks, index, index);
	}
	lsfp.ndata = (knh_ndata_t) v;
	lstacks->api->set(ctx, lstacks, index, &lsfp);
}

static knh_Array_t *ValueStack_copy(CTX ctx, knh_Array_t *a)
{
	const knh_ClassTBL_t *ct = O_cTBL(a);
	knh_Array_t *newlstacks = (knh_Array_t*) new_hObject_(ctx, ct);
	ct->cdef->initcopy(ctx, RAWPTR(newlstacks), RAWPTR(a));
	return newlstacks;
}


#define TT_isSFPIDX(tk)   (TT_(tk) == TT_LVAR || TT_(tk) == TT_FVAR)
#define Token_index(tk)   Token_index_(ctx, tk)
static int Token_index_(CTX ctx, knh_Token_t *tk)
{
	return (int)(tk)->index/* + ((TT_(tk) == TT_LVAR) ? DP(ctx->gma)->ebpidx : 0)*/;
}

//static void ASM_UNBOX(CTX ctx, knh_type_t atype, int a)
//{
//	LLVM_TODO("ASM_UNBOX");
//}

static void ASM_MOVL(CTX ctx, knh_type_t reqt, int sfpidx, knh_type_t ltype, int local)
{
	Value *v = ValueStack_get(ctx, local);
	if (v) ValueStack_set(ctx, sfpidx, v);
	//if(sfpidx < local/*DP(stmt)->espidx*/) {
	//	Value *v = ValueStack_get(ctx, local);
	//	ValueStack_set(ctx, sfpidx, v);
	//}
}
static int Tn_put(CTX ctx, knh_Stmt_t *stmt, size_t n, knh_type_t reqt, int sfpidx)
{
	knh_Token_t *tk = tkNN(stmt, n);
	if(TT_(tk) == TT_FVAR || TT_(tk) == TT_LVAR) return Token_index(tk);
	Tn_asm(ctx, stmt, n, reqt, sfpidx);
	return sfpidx;
}

static knh_Token_t* Tn_putTK(CTX ctx, knh_Stmt_t *stmt, size_t n, knh_type_t reqt, int sfpidx)
{
	knh_Token_t *tk = tkNN(stmt, n);
	if(IS_Stmt(tk)) {
		Tn_asm(ctx, stmt, n, reqt, sfpidx);
		DBG_ASSERT(IS_Token(tkNN(stmt, n)));
		return tkNN(stmt, n);
	}
	return tk;
}

static const Type *convert_type(knh_class_t cid)
{
	switch (cid) {
		case TYPE_void:    return LLVMTYPE_Void;
		case TYPE_Boolean: return LLVMTYPE_Bool;
		case TYPE_Int:     return LLVMTYPE_Int;
		case TYPE_Float:   return LLVMTYPE_Float;
	}
	return LLVMTYPE_Object;
}

static const Type *getLongTy(Module *m)
{
#if defined(SIZEOF_VOIDP) && (SIZEOF_VOIDP == 4)
	return Type::getInt32Ty(m->getContext());
#else
	return Type::getInt64Ty(m->getContext());
#endif
}
static const Type *getShortTy(Module *m)
{
#if defined(SIZEOF_VOIDP) && (SIZEOF_VOIDP == 4)
	return Type::getInt16Ty(m->getContext());
#else
	return Type::getInt32Ty(m->getContext());
#endif
}

#define LLVMBool(v)   (ConstantInt::get(LLVMTYPE_Bool, (v)))
#define LLVMInt(v)    (ConstantInt::get(LLVMTYPE_Int, (v)))
#define LLVMFloat(v)  (ConstantFP::get(LLVMTYPE_Float, (v)))
#define LLVMValue(T, v) LLVMValue_(ctx, T, (knh_intptr_t)v)
static Value *LLVMValue_(CTX ctx, const Type *type, knh_intptr_t ptr)
{
	Module *m = LLVM_MODULE(ctx);
	Value *v = ConstantInt::get(getLongTy(m), ptr);
	return LLVM_BUILDER(ctx)->CreateIntToPtr(v, type);
}

static void asm_nulval(CTX ctx, int a, knh_class_t cid)
{
	knh_Object_t *o = KNH_NULVAL(cid);
	Value *v = LLVMValue(LLVMTYPE_Object, o);
	ValueStack_set(ctx, a, v);
}

static void param_setCtxSfp(CTX ctx, std::vector<Value*> &params, int sfpidx);

static int _BOX_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	knh_class_t cid = Tn_cid(stmt, 0);
	Tn_asm(ctx, stmt, 0, cid, sfpidx);
	DBG_ASSERT(IS_Tunbox(cid));
	
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Module *m = LLVM_MODULE(ctx);

	std::vector<const Type*>args_list;
	args_list.push_back(LLVMTYPE_context);
	args_list.push_back(LLVMTYPE_sfp);
	args_list.push_back(LLVMTYPE_Int); /* sfpidx */
	args_list.push_back(LLVMTYPE_Int); /* knh_ClassTBL_t */
	FunctionType* fnTy = FunctionType::get(LLVMTYPE_Void, args_list, false);
	Function *func = cast<Function>(m->getOrInsertFunction("BOX_llvm", fnTy));

	std::vector<Value*> params;
	param_setCtxSfp(ctx, params, sfpidx);
	params.push_back(LLVMInt(0));
	params.push_back(LLVMInt((knh_int_t)ClassTBL(cid)));

	Value *val = ValueStack_get(ctx, sfpidx);
	Value *sftsfp = builder->CreateConstInBoundsGEP1_32(getsfp(ctx), sfpidx);
	sftsfp = builder->CreateStructGEP(sftsfp, 1);
	sftsfp = builder->CreateBitCast(sftsfp, PointerType::get(val->getType(), 0));
	builder->CreateStore(val, sftsfp);

	builder->CreateCall(func, params.begin(), params.end());
	ValueStack_load_set(ctx, sfpidx, reqt);
	return 0;
}

static void ASM_SMOVx(CTX ctx, knh_type_t atype, int a, knh_type_t btype, knh_sfx_t bx)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *v = ValueStack_get_or_load(ctx, bx.i, CLASS_Object);
	const Type *ty = convert_type(btype);
	v = builder->CreateBitCast(v, LLVMTYPE_ObjectField, "cast");
	v = builder->CreateStructGEP(v, 1, "gep");
	v = builder->CreateLoad(v, "load");
	v = builder->CreateBitCast(v, PointerType::get(ty, 0), "cast");
	v = builder->CreateConstInBoundsGEP1_32(v, bx.n, "get_");
	v = builder->CreateLoad(v);
	ValueStack_set(ctx, a, v);
}

static Value *Fset_CTX(CTX ctx, Value *arg_ctx)
{
	LLVM_TODO("SYSVAL ctx");
	return NULL;
}
static Value *GetCTX_IO(CTX ctx, Value *arg_ctx, int idx)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *v = builder->CreateStructGEP(arg_ctx, idx, "gep");
	v = builder->CreateLoad(v, "l1");
	return v;
}
static Value *Fset_CTXIN(CTX ctx, Value *arg_ctx)
{
	return GetCTX_IO(ctx, arg_ctx, 29);
}
static Value *Fset_CTXOUT(CTX ctx, Value *arg_ctx)
{
	return GetCTX_IO(ctx, arg_ctx, 30);
}
static Value *Fset_CTXERR(CTX ctx, Value *arg_ctx)
{
	return GetCTX_IO(ctx, arg_ctx, 31);
}
static Value *Fset_SYS(CTX ctx, Value *arg_ctx)
{
	return LLVM_BUILDER(ctx)->CreateStructGEP(arg_ctx, 3, "gep");
}
static Value *GetSTD_IO(CTX ctx, Value *arg_ctx, int idx)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *v = Fset_SYS(ctx, arg_ctx);
	v = builder->CreateLoad(v, "l1");
	v = builder->CreateStructGEP(v, 1, "gep_");
	return builder->CreateStructGEP(v, idx, "gep");
}
static Value *Fset_STDIN(CTX ctx, Value *arg_ctx)
{
	return GetSTD_IO(ctx, arg_ctx, 3);
}
static Value *Fset_STDOUT(CTX ctx, Value *arg_ctx)
{
	return GetSTD_IO(ctx, arg_ctx, 4);
}
static Value *Fset_STDERR(CTX ctx, Value *arg_ctx)
{
	return GetSTD_IO(ctx, arg_ctx, 5);
}
static Value *Fset_SCRIPT(CTX ctx, Value *arg_ctx)
{
	return LLVM_BUILDER(ctx)->CreateStructGEP(arg_ctx, 4, "gep");
}
typedef Value* (*Fset)(CTX, Value*);
static Fset SYSVAL_LOAD_INSTS[] = {
	Fset_CTX,
	Fset_CTXIN,
	Fset_CTXOUT,
	Fset_CTXERR,
	Fset_STDIN,
	Fset_STDOUT,
	Fset_STDERR,
	Fset_SYS,
	Fset_SCRIPT,
};

static knh_int_t O_int(knh_Object_t *o)
{
	return ((knh_Int_t*)(o))->n.ivalue;
}

static knh_float_t O_float(knh_Object_t *o)
{
	return ((knh_Int_t*)(o))->n.fvalue;
}

static void Method_addConstData(CTX ctx, knh_Method_t *mtd, knh_Object_t *o)
{
	knh_Array_t *a = (knh_Array_t*) DP(mtd)->kcode;
	knh_Array_add(ctx, a, o);
}
static void ASM_TR(CTX ctx, int thisidx, int sfpidx, knh_class_t cid, const char *fname);
static void ASM_SMOV(CTX ctx, knh_type_t atype, int a/*flocal*/, knh_Token_t *tkb)
{
	knh_type_t btype = SP(tkb)->type;
	switch(TT_(tkb)) {
		case TT_ASIS:
			break;
		case TT_NULL/*DEFVAL*/: {
			knh_class_t cid = (tkb)->cid;
			knh_Object_t *o = KNH_NULVAL(cid);
			if(o != ClassTBL(cid)->defnull) {
				/* TODO */
				ASM(TR, OC_(a), SFP_(a), RIX_(a-a), ClassTBL(cid), _NULVAL);
				break;
			}
			KNH_SETv(ctx, (tkb)->data, o);
			goto L_CONST;
		}
		case TT_CID:
			KNH_SETv(ctx, (tkb)->data, new_Type(ctx, (tkb)->cid));
		case TT_CONST: L_CONST:; {
			Object *o = (tkb)->data;
			Value *v = NULL;
			if (IS_String((tkb)->data)) {
				Method_addConstData(ctx, DP(ctx->gma)->mtd, o);
			}
			if(IS_Tunbox(atype)) {
				if (IS_Tint(atype))        v = LLVMInt(O_int(o));
				else if (IS_Tfloat(atype)) v = LLVMFloat(O_float(o));
				else if (IS_Tbool(atype))  v = LLVMBool(O_data(o));
			}
			else  {
				v = LLVMValue(LLVMTYPE_Object, o);
			}
			ValueStack_set(ctx, a, v);
			break;
		}
		case TT_FVAR:
		case TT_LVAR: {
			int b = Token_index(tkb);
			if(IS_Tunbox(btype)) {
				Value *v = ValueStack_get_or_load(ctx, b, btype);
				ValueStack_set(ctx, a, v);
			}
			else {
				Value *v = ValueStack_get_or_load(ctx, b, btype);
				ValueStack_set(ctx, a, v);
				//ASM(OMOV, OC_(a), OC_(b));
				//if(IS_Tnumbox(btype)) {
				//	if(IS_Tnumbox(atype)) {
				//		ASM(NMOV, NC_(a), NC_(b));
				//	}
				//	else {
				//		ASM(TR, OC_(a), SFP_(b), RIX_(a-b), ClassTBL(atype), _OBOX);
				//	}
				//}
			}
			break;
		}
		case TT_FIELD: {
			knh_sfx_t bx = {0, (tkb)->index};
			if(IS_Token(tkb->token) && TT_isSFPIDX(tkb->token)) {
				bx.i = Token_index(tkb->token);
			}
			ASM_SMOVx(ctx, atype, a, btype, bx);
			break;
		}
		//case TT_SCRFIELD: {
		//	int b = (int)(tkb)->index;
		//	//knh_sfx_t bx = {OC_(DP(ctx->gma)->scridx), (size_t)b};
		//	//ASM_SMOVx(ctx, atype, a, btype, bx);
		//	IRBuilder<> *builder = LLVM_BUILDER(ctx);
		//	const Type *ty = convert_type(btype);
		//	Value *v = ConstantInt::get(Type::getInt64Ty(LLVM_CONTEXT()), (knh_uint_t)(ctx->script));
		//	v = builder->CreateBitCast(v, LLVMTYPE_ObjectField, "cast");
		//	v = builder->CreateStructGEP(v, 1, "gep");
		//	v = builder->CreateLoad(v, "load");
		//	v = builder->CreateBitCast(v, PointerType::get(ty, 0), "cast");
		//	v = builder->CreateConstInBoundsGEP1_32(v, b, "get_");
		//	v = builder->CreateLoad(v);
		//	ValueStack_set(ctx, a, v);
		//	break;
		//}
		case TT_SYSVAL: {
			size_t sysid = (tkb)->index;
			KNH_ASSERT(sysid < K_SYSVAL_MAX);
			Value *v = SYSVAL_LOAD_INSTS[sysid](ctx, getctx(ctx));
			ValueStack_set(ctx, a, v);
			break;
		}
		case TT_PROPN: {
			IRBuilder<> *builder = LLVM_BUILDER(ctx);
			Value *v = LLVMValue(LLVMTYPE_Object, (tkb)->data);
			ValueStack_set(ctx, a, v);
			Value *gep = loadsfp(ctx, builder, getsfp(ctx), CLASS_String, a);
			builder->CreateStore(v, gep);
			if(IS_Tunbox(btype)) {
				ASM_TR(ctx, a, a, btype, "knh_NPROP");
			}
			else {
				ASM_TR(ctx, a, a, btype, "knh_PROP");
			}
			break;
		}
		default: {
			DBG_ABORT("unknown TT=%s", TT__(TT_(tkb)));
		}
	}/*switch*/
}

static void ASM_XMOVx(CTX ctx, knh_type_t atype, knh_sfx_t ax, knh_type_t btype, knh_sfx_t bx)
{
	if(IS_Tunbox(atype)) {
		ASM(XNMOVx, ax, bx);
	}
	else {
		ASM(XMOVx, ax, bx);
	}
}

static void ASM_XMOV_local(CTX ctx, IRBuilder<> *builder, knh_class_t ty, knh_sfx_t ax, Value *val)
{
	const Type *ptype = PointerType::get(convert_type(ty), 0);
	Value *v = ValueStack_get_or_load(ctx, ax.i, CLASS_Object);
	v = builder->CreateBitCast(v, LLVMTYPE_ObjectField, "cast");
	v = builder->CreateStructGEP(v, 1, "oxp");
	v = builder->CreateLoad(v, "load");
	v = builder->CreateBitCast(v, ptype, "v");
	v = builder->CreateConstInBoundsGEP1_32(v, ax.n, "p");
	builder->CreateStore(val, v, false);
}

static void ASM_XMOV(CTX ctx, knh_type_t atype, int a, size_t an, knh_Token_t *tkb, int espidx)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	knh_sfx_t ax = {a, an};
	knh_type_t btype = SP(tkb)->type;
	int use_espidx = -1;
	DBG_ASSERT(espidx != -1);
	switch(TT_(tkb)) {
		case TT_NULL/*DEFVAL*/: {
			knh_class_t cid = (tkb)->cid;
			knh_Object_t *v = KNH_NULVAL(cid);
			if(v != ClassTBL(cid)->defnull) {
				use_espidx = espidx;
				ASM(TR, OC_(espidx), SFP_(espidx), RIX_(espidx-espidx), ClassTBL(cid), _NULVAL);
				break;
			}
			KNH_SETv(ctx, (tkb)->data, v);
			goto L_CONST;
		}
		case TT_CID:
			KNH_SETv(ctx, (tkb)->data, new_Type(ctx, (tkb)->cid));
		case TT_CONST: L_CONST:; {
			Object *o = (tkb)->data;
			Value *vdata;
			if (IS_Tfloat(atype))     vdata = LLVMFloat(O_float(o));
			else if (IS_Tint(atype))  vdata = LLVMInt(O_int(o));
			else if (IS_Tbool(atype)) vdata = LLVMBool(O_data(o));
			else                      vdata = LLVMValue(LLVMTYPE_Object, o);

			ASM_XMOV_local(ctx, builder, atype, ax, vdata);
			break;
		}
		case TT_FVAR:
		case TT_LVAR: {
			int b = Token_index(tkb);
			Value *v = ValueStack_get(ctx, b);
			ASM_XMOV_local(ctx, builder, atype, ax, v);
			break;
		}
		case TT_FIELD: {
			size_t b = (tkb)->index;
			knh_sfx_t bx = {0, b};
			if(IS_Token(tkb->token) && TT_isSFPIDX(tkb->token)) {
				bx.i = Token_index(tkb->token);
			}
			ASM_XMOVx(ctx, atype, ax, btype, bx);
			break;
		}
		//case TT_SCRFIELD: {
		//	size_t b = (tkb)->index;
		//	knh_sfx_t bx = {OC_(DP(ctx->gma)->scridx), b};
		//	ASM_XMOVx(ctx, atype, ax, btype, bx);
		//	break;
		//}
		case TT_SYSVAL: {
			size_t sysid = (tkb)->index;
			Value *v = SYSVAL_LOAD_INSTS[sysid](ctx, getctx(ctx));
			ValueStack_set(ctx, espidx, v);
			use_espidx = espidx;
			break;
		}
		case TT_PROPN: {
			use_espidx = espidx;
			LLVM_WARN("TODO Debug");
			IRBuilder<> *builder = LLVM_BUILDER(ctx);
			Value *v = LLVMValue(LLVMTYPE_Object, (tkb)->data);
			ValueStack_set(ctx, espidx, v);
			Value *gep = loadsfp(ctx, builder, getsfp(ctx), CLASS_String, espidx);
			builder->CreateStore(v, gep);
			if(IS_Tunbox(atype)) {
				ASM_TR(ctx, espidx, espidx, atype, "knh_NPROP");
			}
			else {
				ASM_TR(ctx, espidx, espidx, atype, "knh_PROP");
			}

			break;
		}
		default: {
			DBG_ABORT("unknown TT=%d", TT_(tkb));
		}
	}/*switch*/
	if(use_espidx != -1) {
		if(IS_Tunbox(atype)) {
			ASM(XNMOV, ax, NC_(espidx));
		}
		else {
			ASM(XMOV, ax, OC_(espidx));
		}
	}
}

static void ASM_MOV(CTX ctx, knh_Token_t *tka, knh_Token_t *tkb, int espidx)
{
	DBG_ASSERT(Token_isTyped(tka));
	DBG_ASSERT(IS_Token(tkb));
	DBG_ASSERT(Token_isTyped(tkb));
	knh_type_t atype = SP(tka)->type;
	if(TT_(tka) == TT_LVAR || TT_(tka) == TT_FVAR) {
		ASM_SMOV(ctx, atype, Token_index(tka), tkb);
	}
	else {
		DBG_ASSERT(TT_(tka) == TT_FIELD);
		int an = (int)(tka)->index;
		ASM_XMOV(ctx, atype, 0, an, tkb, espidx);
	}
}

/* ------------------------------------------------------------------------ */
/* [EXPR] */
static knh_int_t Tn_int(knh_Stmt_t *stmt, size_t n)
{
	knh_Token_t *tk = tkNN(stmt, n);
	DBG_ASSERT(TT_(tk) == TT_CONST);
	return ((tk)->num)->n.ivalue;
}

static knh_float_t Tn_float(knh_Stmt_t *stmt, size_t n)
{
	knh_Token_t *tk = tkNN(stmt, n);
	DBG_ASSERT(TT_(tk) == TT_CONST);
	return ((tk)->num)->n.fvalue;
}

static int _CALL_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx);
static knh_opcode_t OPCODE_imn(knh_methodn_t mn, int diff)
{
	switch(mn) {
	case MN_opNEG: return OPCODE_iNEG;
	case MN_opADD: return OPCODE_iADD + diff;
	case MN_opSUB: return OPCODE_iSUB + diff;
	case MN_opMUL: return OPCODE_iMUL + diff;
	case MN_opDIV: return OPCODE_iDIV+ diff;
	case MN_opMOD: return OPCODE_iMOD+ diff;
	case MN_opEQ:  return OPCODE_iEQ+ diff;
	case MN_opNOTEQ: return OPCODE_iNEQ+ diff;
	case MN_opLT:  return OPCODE_iLT+ diff;
	case MN_opLTE: return OPCODE_iLTE+ diff;
	case MN_opGT:  return OPCODE_iGT+ diff;
	case MN_opGTE: return OPCODE_iGTE+ diff;
#ifdef OPCODE_iAND
	case MN_opLAND :  return OPCODE_iAND  + diff;
	case MN_opLOR  :  return OPCODE_iOR   + diff;
	case MN_opLXOR :  return OPCODE_iXOR  + diff;
	case MN_opLSFT:  return OPCODE_iLSFT  + diff;
	case MN_opRSFT:  return OPCODE_iRSFT  + diff;
#endif
	}
	return OPCODE_NOP;
}

static knh_opcode_t OPCODE_fmn(knh_methodn_t mn, int diff)
{
	switch(mn) {
	case MN_opNEG: return OPCODE_fNEG;
	case MN_opADD: return OPCODE_fADD + diff;
	case MN_opSUB: return OPCODE_fSUB + diff;
	case MN_opMUL: return OPCODE_fMUL + diff;
	case MN_opDIV: return OPCODE_fDIV + diff;
	case MN_opEQ:  return OPCODE_fEQ + diff;
	case MN_opNOTEQ: return OPCODE_fNEQ + diff;
	case MN_opLT:  return OPCODE_fLT + diff;
	case MN_opLTE: return OPCODE_fLTE + diff;
	case MN_opGT:  return OPCODE_fGT + diff;
	case MN_opGTE: return OPCODE_fGTE + diff;
	}
	return OPCODE_NOP;
}

static knh_bool_t OPR_hasCONST(CTX ctx, knh_Stmt_t *stmt, knh_methodn_t *mn, int swap)
{
	int isCONST = (TT_(tmNN(stmt, 2)) == TT_CONST);
	if(swap == 1 && TT_(tmNN(stmt, 1)) == TT_CONST) {
		knh_methodn_t newmn = *mn;
		knh_Stmt_swap(ctx, stmt, 1, 2);
		if(*mn == MN_opLT) newmn = MN_opGT;  /* 1 < n ==> n > 1 */
		else if(*mn == MN_opLTE) newmn = MN_opGTE; /* 1 <= n => n >= 1 */
		else if(*mn == MN_opGT)  newmn = MN_opLT;
		else if(*mn == MN_opGTE) newmn = MN_opLTE;
		//DBG_P("swap %s ==> %s", MN__(*mn), MN__(newmn));
		*mn = newmn;
		isCONST = 1;
	}
	return isCONST;
}

#define VSET(ctx, local, create) ValueStack_set(ctx, local, LLVM_BUILDER(ctx)->create)
static int ASMiop(CTX ctx, knh_methodn_t mn, Value *va, Value *vb, int local)
{
	switch(mn) {
	case MN_opNEG  : VSET(ctx, local, CreateNeg(va, "neg"));break;
	case MN_opADD  : VSET(ctx, local, CreateAdd(va, vb, "add"));break;
	case MN_opSUB  : VSET(ctx, local, CreateSub(va, vb, "sub"));break;
	case MN_opMUL  : VSET(ctx, local, CreateMul(va, vb, "mul"));break;
	case MN_opDIV  : VSET(ctx, local, CreateSDiv(va, vb, "div"));break;
	case MN_opMOD  : VSET(ctx, local, CreateSRem(va, vb, "mod"));break;
	case MN_opEQ   : VSET(ctx, local, CreateICmpEQ(va, vb, "eq"));break;
	case MN_opNOTEQ: VSET(ctx, local, CreateICmpNE(va, vb, "ne"));break;
	case MN_opLT   : VSET(ctx, local, CreateICmpSLT(va, vb, "lt"));break;
	case MN_opLTE  : VSET(ctx, local, CreateICmpSLE(va, vb, "le"));break;
	case MN_opGT   : VSET(ctx, local, CreateICmpSGT(va, vb, "gt"));break;
	case MN_opGTE  : VSET(ctx, local, CreateICmpSGE(va, vb, "ge"));break;
#ifdef OPCODE_iAND
	case MN_opLAND : VSET(ctx, local, CreateAnd(va, vb, "and"));break;
	case MN_opLOR  : VSET(ctx, local, CreateOr(va, vb, "or"));break;
	case MN_opLXOR : VSET(ctx, local, CreateXor(va, vb, "xor"));break;
	case MN_opLSFT : VSET(ctx, local, CreateShl(va, vb, "lshr"));break;
	case MN_opRSFT : VSET(ctx, local, CreateAShr(va, vb, "rshr"));break;
#endif
	}
	return 1;
}
static int ASMfop(CTX ctx, knh_methodn_t mn, Value *va, Value *vb, int local)
{
	switch(mn) {
	case MN_opNEG  : VSET(ctx, local, CreateFNeg   (va, "neg"));break;
	case MN_opADD  : VSET(ctx, local, CreateFAdd   (va, vb, "add"));break;
	case MN_opSUB  : VSET(ctx, local, CreateFSub   (va, vb, "sub"));break;
	case MN_opMUL  : VSET(ctx, local, CreateFMul   (va, vb, "mul"));break;
	case MN_opDIV  : VSET(ctx, local, CreateFDiv   (va, vb, "div"));break;
	case MN_opEQ   : VSET(ctx, local, CreateFCmpOEQ(va, vb, "eq"));break;
	case MN_opNOTEQ: VSET(ctx, local, CreateFCmpONE(va, vb, "ne"));break;
	case MN_opLT   : VSET(ctx, local, CreateFCmpOLT(va, vb, "lt"));break;
	case MN_opLTE  : VSET(ctx, local, CreateFCmpOLE(va, vb, "le"));break;
	case MN_opGT   : VSET(ctx, local, CreateFCmpOGT(va, vb, "gt"));break;
	case MN_opGTE  : VSET(ctx, local, CreateFCmpOGE(va, vb, "ge"));break;
	}
	return 1;
}
static int _OPR_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx;
	knh_Method_t *mtd = (tkNN(stmt, 0))->mtd;
	if (IS_NULL(mtd)) return 0;
	knh_methodn_t mn = (mtd)->mn;
	knh_class_t cid = CLASS_t(SP(tkNN(stmt, 1))->type);
	knh_opcode_t opcode;
	if(cid == CLASS_Boolean && mn == MN_opNOT) {
		int a = Tn_put(ctx, stmt, 1, TYPE_Boolean, local + 1);
		Value *va = ValueStack_get(ctx, a);
		va = LLVM_BUILDER(ctx)->CreateNot(va, "not");
		ValueStack_set(ctx, sfpidx, va);
		return 1;
	}
	if (cid == CLASS_Int && ((opcode = OPCODE_imn(mn, 0)) != OPCODE_NOP)) {
		int swap = 1;
		if (mn == MN_opNEG) {
			int a = Tn_put(ctx, stmt, 1, TYPE_Int, local + 1);
			Value *va = ValueStack_get(ctx, a);
			ASMiop(ctx, MN_opNEG, va, NULL, sfpidx);
			return 1;
		}
		if (mn == MN_opSUB || mn == MN_opDIV || mn == MN_opMOD ||
				mn == MN_opLSFT || mn == MN_opRSFT) swap =0;
		if (OPR_hasCONST(ctx, stmt, &mn, swap)) {
			int a = Tn_put(ctx, stmt, 1, TYPE_Int, local + 1);
			knh_int_t b = Tn_int(stmt, 2);
			if (b == 0 && mn == MN_opDIV) {
				b = 1;
				WARN_DividedByZero(ctx);
			}
			Value *va = ValueStack_get(ctx, a);
			Value *vb = LLVMInt(b);
			ASMiop(ctx, mn, va, vb, sfpidx);
		}
		else {
			int a = Tn_put(ctx, stmt, 1, TYPE_Int, local + 1);
			int b = Tn_put(ctx, stmt, 2, TYPE_Int, local + 2);
			Value *va = ValueStack_get(ctx, a);
			Value *vb = ValueStack_get(ctx, b);
			ASMiop(ctx, mn, va, vb, sfpidx);
		}
		return 1;
	}
	if (cid == CLASS_Float && ((opcode = OPCODE_fmn(mn, 0)) != OPCODE_NOP)) {
		int swap = 1;
		if (mn == MN_opNEG) {
			int a = Tn_put(ctx, stmt, 1, TYPE_Float, local + 1);
			Value *va = ValueStack_get(ctx, a);
			ASMfop(ctx, MN_opNEG, va, NULL, sfpidx);
			return 1;
		}
		if (mn == MN_opSUB || mn == MN_opDIV || mn == MN_opMOD) swap =0;
		if (OPR_hasCONST(ctx, stmt, &mn, swap)) {
			int a = Tn_put(ctx, stmt, 1, TYPE_Float, local + 1);
			knh_float_t b = Tn_float(stmt, 2);
			if (b == K_FLOAT_ZERO && mn == MN_opDIV) {
				b = K_FLOAT_ONE;
				WARN_DividedByZero(ctx);
			}
			Value *va = ValueStack_get(ctx, a);
			Value *vb = LLVMFloat(b);
			ASMfop(ctx, mn, va, vb, sfpidx);
		}
		else {
			int a = Tn_put(ctx, stmt, 1, TYPE_Float, local + 1);
			int b = Tn_put(ctx, stmt, 2, TYPE_Float, local + 2);
			Value *va = ValueStack_get(ctx, a);
			Value *vb = ValueStack_get(ctx, b);
			ASMfop(ctx, mn, va, vb, sfpidx);
		}
		return 1;
	}
	_CALL_asm(ctx, stmt, reqt, sfpidx);
	return 0;
}

/* ------------------------------------------------------------------------ */
/* CALL */

static knh_type_t Tn_ptype(CTX ctx, knh_Stmt_t *stmt, size_t n, knh_class_t cid, knh_Method_t *mtd)
{
	if(!IS_Method(mtd)) {
		return TYPE_dyn;  // boxing
	}
	if(n == 1) { // base
		if(IS_Tunbox(cid) && cid != (mtd)->cid) {
			/* Object, Number, dynamic */
			cid = (mtd)->cid;
		}
		DBG_(
		if(cid != (mtd)->cid) {
			DBG_P("** WATCH OUT ** mtd_cid=%s, (mtd)->cid=%s", CLASS__(cid), CLASS__((mtd)->cid));
		});
		return cid;
	}
	else {
		asm volatile("int3");
		knh_type_t ptype = knh_Method_ptype(ctx, mtd, n - 2, cid);
		return knh_type_tocid(ctx, ptype, cid);
	}
}
static int CALLPARAMs_asm(CTX ctx, knh_Stmt_t *stmt, size_t s, int local, knh_class_t cid, knh_Method_t *mtd)
{
	Value *arg_sfp = getsfp(ctx);
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	if(s == 1 && Method_isStatic(mtd))
		// ignoring static caller, like Script
		s = 2;
	if(DP(stmt)->size + DP(stmt)->espidx > 32) {
		//int n = DP(stmt)->size + DP(stmt)->espidx;
		//Value *sfpptr = loadsfp(ctx, builder, arg_sfp, CLASS_Object, n);
		//if (sfpptr > ctx->stactop) {
		//  rbp = builder->CreateCall(knh_stack_initexpand);
		//}
		LLVM_TODO("check stack");
	}
	for (size_t i = s; i < DP(stmt)->size; i++) {
		knh_type_t reqt = Tn_ptype(ctx, stmt, i, cid, mtd);
		int a = local + i + (K_CALLDELTA-1);
		Tn_asm(ctx, stmt, i, reqt, a);
		Value *v = ValueStack_get_or_load(ctx, a, cid);
		const Type *ty = v->getType();
		if (reqt == TYPE_dyn && ty == LLVMTYPE_Int) {
			reqt = TYPE_Int;
		} else if (reqt == TYPE_dyn && ty == LLVMTYPE_Bool) {
			reqt = TYPE_Boolean;
		}

		Value *ptr = loadsfp(ctx, builder, arg_sfp, reqt, a);
		builder->CreateStore(v, ptr, false/*isVolatile*/);
	}
	return 1;
}

static void asm_shift_esp(CTX ctx, size_t idx)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *vsfp, *vesp;
	vesp = builder->CreateStructGEP(getctx(ctx), 7, "gep"); /* esp */
	vsfp = builder->CreateConstInBoundsGEP1_32(getsfp(ctx), idx);
	builder->CreateStore(vsfp, vesp, false);
}

static int ASM_SCALL(CTX ctx, int sfpidx, int thisidx, knh_Method_t* mtd)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *func = LLVMValue(LLVMTYPE_fcall, mtd->fcall_1);
	std::vector<Value*> params;
	param_setCtxSfp(ctx, params, thisidx);
	params.push_back(LLVMInt(0/*K_RTNIDX*/));
	asm_shift_esp(ctx, 1+/*argc*/1+thisidx);
	builder->CreateCall(func, params.begin(), params.end());
	return 0;
}

static void _CALL(CTX ctx, knh_type_t reqt, int sfpidx, knh_type_t rtype, knh_Method_t *mtd, int isStatic, size_t argc)
{
	int thisidx = sfpidx + K_CALLDELTA;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	DBG_ASSERT(IS_Method(mtd));
	KNH_ASSERT(sfpidx >= DP(stmt)->espidx);
	if(Method_isFinal(mtd) || isStatic) {
		knh_class_t retTy;
		Value *vfunc, *ret_v;
		std::vector<Value*> params;

		param_setCtxSfp(ctx, params, thisidx);
		params.push_back(LLVMInt(K_RTNIDX));
		asm_shift_esp(ctx, argc+1+thisidx);
		if(Method_isKonohaCode(mtd) || DP(ctx->gma)->mtd == mtd) {
			vfunc = LLVMValue(LLVMTYPE_Method, mtd);
			vfunc = builder->CreateStructGEP(vfunc, 4, "mtdgep");
			vfunc = builder->CreateLoad(vfunc, "fcall");
		}
		else {
			knh_Fmethod func = mtd->fcall_1;
			vfunc = LLVMValue(LLVMTYPE_fcall, func);
		}
		builder->CreateCall(vfunc, params.begin(), params.end());

		retTy = knh_ParamArray_rtype(DP(mtd)->mp);
		if(retTy != TYPE_void) {
			Value *ptr = loadsfp(ctx, builder, getsfp(ctx), retTy, thisidx+K_RTNIDX);
			ret_v = builder->CreateLoad(ptr, "ret_v");
			ValueStack_set(ctx, sfpidx, ret_v);
		}
	}
	else {
		ASM_SCALL(ctx,thisidx,thisidx,mtd);
		knh_class_t retTy;
		retTy = knh_ParamArray_rtype(DP(mtd)->mp);
		if(retTy != TYPE_void) {
			Value *ptr = loadsfp(ctx, builder, getsfp(ctx), retTy, thisidx+K_RTNIDX);
			Value *ret_v = builder->CreateLoad(ptr, "ret_v");
			ValueStack_set(ctx, thisidx+K_RTNIDX, ret_v);
		}
	}
}

static BasicBlock *BB_CREATE(CTX ctx, const char *bbName)
{
	Module *m = LLVM_MODULE(ctx);
	LLVMContext &llvmctx = m->getContext();
	Function *f = LLVM_FUNCTION(ctx);
	return BasicBlock::Create(llvmctx, bbName, f);
}

static void check_array_boundary(CTX ctx, Value *a, Value *n)
{
	Module *m = LLVM_MODULE(ctx);
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bb1 = BB_CREATE(ctx, "OutOfIDX");
	BasicBlock *bb2 = BB_CREATE(ctx, "bb");
	Value *size = builder->CreateStructGEP(a, 2, "size");
	size = builder->CreateLoad(size);

	Value *cond = builder->CreateICmpSLE(size, n, "le");
	builder->CreateCondBr(cond, bb1, bb2);
	builder->SetInsertPoint(bb1);

	/* throw out of index */
	std::vector<Value*> params;

	std::vector<const Type*> argsTy;
	argsTy.push_back(LLVMTYPE_context);
	argsTy.push_back(LLVMTYPE_sfp);
	argsTy.push_back(getLongTy(m));
	argsTy.push_back(getLongTy(m));

	param_setCtxSfp(ctx, params, 0);
	params.push_back(n);
	params.push_back(size);
	FunctionType *fnTy = FunctionType::get(LLVMTYPE_Void, argsTy, false);
	Function *f = cast<Function>(m->getOrInsertFunction("THROW_OutOfRange", fnTy));
	builder->CreateCall(f, params.begin(), params.end());
	builder->CreateBr(bb2);
	builder->SetInsertPoint(bb2);
}

static Value *ASM_ARRAY_N(CTX ctx, int a, knh_class_t cid, int sfpidx, Value *n)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *vobj = ValueStack_get(ctx, a);
	const Type *type = convert_type(C_p1(cid));
	const Type *ptype = PointerType::get(type, 0);

	Value *array = builder->CreateBitCast(vobj, LLVMTYPE_Array, "array");
	check_array_boundary(ctx, array, n);
	Value *lptr = builder->CreateStructGEP(array, 1, "listptr");
	Value *list = builder->CreateLoad(lptr, "list");
	list = builder->CreateBitCast(list, ptype, "cast");
	Value *vptr = builder->CreateInBoundsGEP(list, n, "vptr");
	return vptr;
}
static void ASM_ARRAY_GET(CTX ctx, int a, knh_class_t cid, int sfpidx, Value *n)
{
	Value *vptr = ASM_ARRAY_N(ctx, a, cid, sfpidx, n);
	Value *v = LLVM_BUILDER(ctx)->CreateLoad(vptr, "v");
	ValueStack_set(ctx, sfpidx, v);
}
static void ASM_ARRAY_SET(CTX ctx, int a, knh_class_t cid, int sfpidx, Value *n, Value *v)
{
	Value *vptr = ASM_ARRAY_N(ctx, a, cid, sfpidx, n);
	LLVM_BUILDER(ctx)->CreateStore(v, vptr, false);
}

static void ASM_Object_checkNULL(CTX ctx, int sfpidx, int a, int isNULL)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *idx[] = {
				ConstantInt::get(Type::getInt64Ty(LLVM_CONTEXT()), 0),
				ConstantInt::get(Type::getInt32Ty(LLVM_CONTEXT()), 0),
				ConstantInt::get(Type::getInt32Ty(LLVM_CONTEXT()), 0)
	};
	SmallVector<Value*, 3> idxs(idx, idx+3);
	Value *vflg = LLVMInt(1);
	Value *v = ValueStack_get(ctx, a);
	v = builder->CreateInBoundsGEP(v, idxs.begin(), idxs.end(), "flg");
	v = builder->CreateLoad(v);
	v = builder->CreateLShr(v, LLVMInt(1));
	v = builder->CreateAnd(v, vflg);
	if (!isNULL) {
		v = builder->CreateXor(v, LLVMInt(1));
	}
	v = builder->CreateTrunc(v, LLVMTYPE_Bool);
	ValueStack_set(ctx, sfpidx, v);
}
static void sfp_store(CTX ctx, int sfpidx, knh_class_t cid, Value *v);

static bool _FASTCALL(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx, knh_type_t rtype, knh_Method_t *mtd, knh_class_t cid)
{
	int local = sfpidx;
	int thisidx = sfpidx + K_CALLDELTA;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);

	std::string fname(knh_getmnname(ctx, SP(mtd)->mn));
	std::stringstream ss;
	ss << CLASS__(SP(mtd)->cid) << "_" << fname;
	Function *func = LLVM_MODULE(ctx)->getFunction(ss.str());
	if (func == NULL || Method_isKonohaCode(mtd)) {
		return false;
	}

	asm_shift_esp(ctx, 1 + thisidx + 2);
	Value *sfpsft = builder->CreateConstInBoundsGEP1_32(getsfp(ctx), thisidx, "sfpsft");

	std::vector<Value*> params;
	params.push_back(getctx(ctx));
	params.push_back(sfpsft);

	size_t i = Method_isStatic(mtd) ? 2 : 1;
	for ( ; i < DP(stmt)->size; i++) {
		knh_type_t reqt = Tn_ptype(ctx, stmt, i, cid, mtd);
		int a = local + i + (K_CALLDELTA-1);
		Tn_asm(ctx, stmt, i, reqt, a);
		Value *v = ValueStack_get_or_load(ctx, a, cid);
		const Type *ty = v->getType();
		if (reqt == TYPE_dyn && ty == LLVMTYPE_Int) {
			reqt = TYPE_Int;
		} else if (reqt == TYPE_dyn && ty == LLVMTYPE_Bool) {
			reqt = TYPE_Boolean;
		}
		if (i == 1) {
			sfp_store(ctx, a, reqt, v);
		} else {
			params.push_back(v);
			if (reqt != TYPE_Int && reqt != TYPE_Float && reqt != TYPE_Boolean) {
				sfp_store(ctx, a, reqt, v);
			}
		}
	}

	CallInst *ret_v = builder->CreateCall(func, params.begin(), params.end());
	knh_type_t retTy = knh_ParamArray_rtype(DP(mtd)->mp);
	if (retTy != TYPE_void) {
		if (retTy != TYPE_Int && retTy != TYPE_Float && retTy != TYPE_Boolean) {
			sfp_store(ctx, sfpidx, CLASS_Object, ret_v);
		}
		ValueStack_set(ctx, sfpidx, ret_v);
	}
	return true;
}

static int _CALL_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx;
	knh_Token_t *tkMTD = tkNN(stmt, 0);
	knh_Method_t *mtd = (tkMTD)->mtd;
	knh_class_t cid = Tn_cid(stmt, 1);
	if(!IS_Method(mtd)) {
		LLVM_TODO("LOADMTD");
		return 0;
	}
	knh_class_t mtd_cid = (mtd)->cid;
	knh_methodn_t mtd_mn = (mtd)->mn;
	if(mtd_cid == CLASS_Array) {
		if (mtd_mn == MN_get) {
			int a = Tn_put(ctx, stmt, 1, cid, local + 1);
			if(Tn_isCONST(stmt, 2)) {
				knh_intptr_t n = (knh_intptr_t)Tn_int(stmt, 2);
				if(n < 0) {
					goto L_USECALL;
				}
				ASM_ARRAY_GET(ctx, a, cid, sfpidx, LLVMInt(n));
			} else {
				int n = Tn_put(ctx, stmt, 2, TYPE_Int, local + 2);
				ASM_ARRAY_GET(ctx, a, cid, sfpidx, ValueStack_get(ctx, n));
			}
			return 0;
		}
		else if (mtd_mn == MN_set) {
			int a = Tn_put(ctx, stmt, 1, cid, local + 1);
			knh_type_t ptype = knh_Method_ptype(ctx, mtd, cid, 1);
			int v = Tn_put(ctx, stmt, 3, ptype, local + 3);
			Value *value = ValueStack_get(ctx, v);
			if(Tn_isCONST(stmt, 2)) {
				knh_intptr_t n = (knh_intptr_t)Tn_int(stmt, 2);
				if(n < 0) {
					goto L_USECALL;
				}
				ASM_ARRAY_SET(ctx, a, cid, sfpidx, LLVMInt(n), value);
			}
			else {
				int n = Tn_put(ctx, stmt, 2, TYPE_Int, local + 2);
				ASM_ARRAY_SET(ctx, a, cid, sfpidx, ValueStack_get(ctx, n), value);
			}
			return 0;
		}
	}
#if defined(OPCODE_BGETIDX)
	if(mtd_cid == CLASS_Bytes) {
		LLVM_TODO("CALL Bytes get/set");
	}
#endif
#ifdef OPCODE_bNUL
	if(mtd_cid == CLASS_Object) {
		if (mtd_mn == MN_isNull) {
			int a = Tn_put(ctx, stmt, 1, cid, local + 1);
			ASM_Object_checkNULL(ctx, sfpidx, a, 1/*isNULL*/);
			return 0;
		}
		else if (mtd_mn == MN_isNotNull) {
			int a = Tn_put(ctx, stmt, 1, cid, local + 1);
			ASM_Object_checkNULL(ctx, sfpidx, a, 0/*isNotNULL*/);
			return 0;
		}
	}
#endif
	{
		knh_index_t deltaidx = knh_Method_indexOfGetterField(mtd);
		if(deltaidx != -1) {
			int b = Tn_put(ctx, stmt, 1, mtd_cid, local + 1);
			knh_type_t type = knh_ParamArray_rtype(DP(mtd)->mp);
			knh_sfx_t bx = {b, deltaidx};
			ASM_SMOVx(ctx, reqt, sfpidx, type, bx);
			return 0;
		}
		deltaidx = knh_Method_indexOfSetterField(mtd);
		if(deltaidx != -1) {
			int b = Tn_put(ctx, stmt, 1, mtd_cid, local + 1);
			knh_type_t reqt2 = knh_Method_ptype(ctx, mtd, cid, 0);
			knh_Token_t *tkV = Tn_putTK(ctx, stmt, 2, reqt, local + 2);
			ASM_XMOV(ctx, reqt2, b, deltaidx, tkV, local);
			if(reqt != TYPE_void) {
				ASM_SMOV(ctx, reqt, sfpidx, tkV);
			}
			return 0;
		}
	}
	L_USECALL:;
	{
		int isStatic = Method_isStatic(mtd);
		knh_type_t rtype = knh_type_tocid(ctx, knh_ParamArray_rtype(DP(mtd)->mp), cid);
		if(Method_isFinal(mtd) || isStatic) {
			if(_FASTCALL(ctx, stmt, reqt, local, SP(stmt)->type, mtd, cid)){
				ASM_MOVL(ctx, reqt, sfpidx, SP(stmt)->type, local);
				return 0;
			}
		}
		if(CALLPARAMs_asm(ctx, stmt, 1, local, cid, mtd)) {
			_CALL(ctx, reqt, local, rtype, mtd, isStatic, DP(stmt)->size - 2);
			ASM_MOVL(ctx, reqt, sfpidx, SP(stmt)->type, local);
		}
	}

	return 0;
}

static int _FUNCCALL_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	knh_Method_t *mtd = (tkNN(stmt, 0))->mtd;
	knh_class_t cid = Tn_cid(stmt, 1);
	knh_ParamArray_t *pa = ClassTBL(cid)->cparam;
	for (size_t i = 0; i < pa->psize; i++) {
		kparam_t *p = knh_ParamArray_get(pa, i);
		knh_type_t reqt = knh_type_tocid(ctx, p->type, DP(ctx->gma)->this_cid);
		int n = local + i + (K_CALLDELTA + 1);
		Tn_asm(ctx, stmt, i+2, reqt, n);
		Value *gep = loadsfp(ctx, builder, getsfp(ctx), reqt, n);
		builder->CreateStore(ValueStack_get(ctx, n), gep);
	}
	int n = local + K_CALLDELTA;
	Tn_asm(ctx, stmt, 1, cid, n);
	Value *gep = loadsfp(ctx, builder, getsfp(ctx), cid, n);
	builder->CreateStore(ValueStack_get(ctx, n), gep);

	if(Stmt_isDYNCALL(stmt)) {
		//int a = local + K_CALLDELTA;
		//ASM(TR, OC_(a), SFP_(a), RIX_(a-a), ClassTBL(cid), _TCHECK);
		LLVM_TODO("Support Dynamic Call");
	}

	int thisidx = local + K_CALLDELTA;
	ASM_SCALL(ctx, thisidx, thisidx, mtd);
	//TODO reqt is correct type?
	//knh_type_t rtype = knh_type_tocid(ctx, knh_ParamArray_rtype(DP(mtd)->mp), cid);
	//ValueStack_set(ctx, sfpidx, ValueStack_load(ctx, local, rtype));
	ValueStack_set(ctx, sfpidx, ValueStack_load(ctx, thisidx, reqt));
	return 0;
}

static int _EXPR_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx);

static int _CALL1_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	if(IS_Stmt(DP(stmt)->stmtPOST)) {  /* a++ */
		Tn_asm(ctx, stmt, 0, reqt, sfpidx);
		_EXPR_asm(ctx, DP(stmt)->stmtPOST, reqt, sfpidx+1);
		ASM_MOVL(ctx, reqt, sfpidx, SP(stmt)->type, sfpidx);
	}
	else {
		Tn_asm(ctx, stmt, 0, reqt, sfpidx);
	}
	return 0;
}

static void ASM_TR(CTX ctx, int thisidx, int sfpidx, knh_class_t cid, const char *fname)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	std::vector<const Type*> argsTy;
	argsTy.push_back(LLVMTYPE_context);
	argsTy.push_back(LLVMTYPE_sfp);
	argsTy.push_back(LLVMTYPE_Int);
	argsTy.push_back(LLVMTYPE_Int);
	FunctionType *fnTy = FunctionType::get(LLVMTYPE_Void, argsTy, false);
	std::vector<Value*> params;
	param_setCtxSfp(ctx, params, thisidx);
	params.push_back(LLVMInt(RIX_(sfpidx-thisidx)));
	params.push_back(LLVMInt((knh_int_t)ClassTBL(cid)));
	Function *func = cast<Function>(LLVM_MODULE(ctx)->getOrInsertFunction(fname, fnTy));
	builder->CreateCall(func, params.begin(), params.end());
	ValueStack_load_set(ctx, sfpidx, cid);
}

static int _NEW_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx, thisidx = local + K_CALLDELTA;
	knh_Method_t *mtd = (tkNN(stmt, 0))->mtd;
	knh_class_t cid = (tkNN(stmt, 1))->cid;
	if(DP(stmt)->size == 2 && (mtd)->cid == CLASS_Object && (mtd)->mn == MN_new) {
		ASM_TR(ctx, thisidx, sfpidx, cid, "TR_NEW");
	}
	else {
		ASM_TR(ctx, thisidx, thisidx, cid, "TR_NEW");
		CALLPARAMs_asm(ctx, stmt, 2, local, cid, mtd);
		_CALL(ctx, reqt, local, cid, mtd, 1, DP(stmt)->size - 2);
		ASM_MOVL(ctx, cid, sfpidx, SP(stmt)->type, local);
	}
	return 0;
}

static void sfp_store(CTX ctx, int sfpidx, knh_class_t cid, Value *v);

static int _TCAST_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	knh_type_t srct = Tn_type(stmt, 1);
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	if(srct != reqt) {
		int local = sfpidx;
		knh_Token_t *tkC = tkNN(stmt, 0);
		knh_TypeMap_t *trl = (tkC)->mpr;
		Tn_asm(ctx, stmt, 1, srct, local);
		if(IS_TypeMap(trl)) {
			knh_class_t scid = SP(trl)->scid, tcid = SP(trl)->tcid;
			if(1/*TypeMap_isFinal(trl)*/) {
				if(scid == CLASS_Int && tcid == CLASS_Float) {
					Value *v = ValueStack_get(ctx, local);
					v = builder->CreateSIToFP(v, LLVMTYPE_Float, "fcast");
					ValueStack_set(ctx, local, v);
				}
				else if(scid == CLASS_Float && tcid == CLASS_Int) {
					Value *v = ValueStack_get(ctx, local);
					v = builder->CreateFPToSI(v, LLVMTYPE_Int, "icast");
					ValueStack_set(ctx, local, v);
				}
				else {
					Value *v, *val = ValueStack_get(ctx, local);
					Value *func = LLVMValue(LLVMTYPE_fcall, trl->ftypemap_1);
					sfp_store(ctx, local, scid, val);
					std::vector<Value*> params;
					param_setCtxSfp(ctx, params, local);
					params.push_back(LLVMInt(0));
					builder->CreateCall(func, params.begin(), params.end());
					v = ValueStack_load_set(ctx, local, (stmt)->type);

					//ASM(SCAST, RTNIDX_(ctx, local, stmt->type), SFP_(local), RIX_(local-local), trl);
				}
			}
			else {
				LLVM_TODO("!TypeMap_isFinal(trl)");
			}
		}
		else {
			LLVM_TODO("ACAST");
		}
		ASM_MOVL(ctx, reqt, sfpidx, SP(stmt)->type, local);
	}
	return 0;
}

#define VBOOL(v) (VBOOL_(ctx, v))
static Value *VBOOL_(CTX ctx, Value *v)
{
	if (v->getType() == Type::getInt1Ty(LLVM_CONTEXT())) {
		return v;
	}
	DBG_ASSERT(v->getType() == LLVMTYPE_Bool);
	return LLVM_BUILDER(ctx)->CreateTrunc(v, Type::getInt1Ty(LLVM_CONTEXT()), "cond");
}

static int _ALT_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	DBG_ABORT("TODO: ALT");
	return 0;
}

static int _OR_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx,  size = DP(stmt)->size;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbTrue = BB_CREATE(ctx, "true");
	BasicBlock *bbNext = BB_CREATE(ctx, "next");
	std::vector<BasicBlock *> blocks;

	for (int i = 0; i < size; i++) {
		int n = Tn_put(ctx, stmt, i, TYPE_Boolean, local + 1);
		Value *cond = ValueStack_get(ctx, n);
		builder->CreateCondBr(VBOOL(cond), bbTrue, bbNext);

		blocks.push_back(builder->GetInsertBlock());
		builder->SetInsertPoint(bbNext);
		if(i + 1 != size)
			bbNext = BB_CREATE(ctx, "next");
	}
	builder->CreateBr(bbTrue);
	builder->SetInsertPoint(bbTrue);

	PHINode *phi = builder->CreatePHI(LLVMTYPE_Bool, "or_result");
	phi->addIncoming(LLVMBool(0), bbNext);

	std::vector<BasicBlock*>::iterator itr;
	for (itr = blocks.begin(); itr != blocks.end(); itr++) {
		phi->addIncoming(LLVMBool(1), *itr);
	}
	ValueStack_set(ctx, sfpidx, phi);

	return 0;
}

static int _AND_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx, size = DP(stmt)->size;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbFalse = BB_CREATE(ctx, "false");
	BasicBlock *bbNext  = BB_CREATE(ctx, "next");
	std::vector<BasicBlock *> blocks;
	for (int i = 0; i < size; i++) {
		int n = Tn_put(ctx, stmt, i, TYPE_Boolean, local + 1);
		Value *cond = ValueStack_get(ctx, n);
		builder->CreateCondBr(VBOOL(cond), bbNext, bbFalse);
		blocks.push_back(builder->GetInsertBlock());
		builder->SetInsertPoint(bbNext);
		if(i + 1 != size)
			bbNext = BB_CREATE(ctx, "next");
	}
	builder->CreateBr(bbFalse);
	builder->SetInsertPoint(bbFalse);

	PHINode *phi = builder->CreatePHI(LLVMTYPE_Bool, "and_result");
	phi->addIncoming(LLVMBool(1), bbNext);
	std::vector<BasicBlock*>::iterator itr;
	for (itr = blocks.begin(); itr != blocks.end(); itr++) {
		phi->addIncoming(LLVMBool(0), *itr);
	}
	ValueStack_set(ctx, sfpidx, phi);
	return 0;
}

static int Tn_CondAsm(CTX ctx, knh_Stmt_t *stmt, size_t n, int isTRUE, int flocal);

static int _TRI_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbThen  = BB_CREATE(ctx, "then");
	BasicBlock *bbElse  = BB_CREATE(ctx, "else");
	BasicBlock *bbMerge = BB_CREATE(ctx, "merge");

	int a = Tn_CondAsm(ctx, stmt, 0, 0, sfpidx);
	Value *cond = ValueStack_get(ctx, a);
	builder->CreateCondBr(VBOOL(cond), bbThen, bbElse);

	builder->SetInsertPoint(bbThen);
	int b = Tn_put(ctx, stmt, 1, reqt, local);
	Value* vb = ValueStack_get(ctx, b);
	builder->CreateBr(bbMerge);

	builder->SetInsertPoint(bbElse);
	int c = Tn_put(ctx, stmt, 2, reqt, local);
	Value* vc = ValueStack_get(ctx, c);
	builder->CreateBr(bbMerge);

	builder->SetInsertPoint(bbMerge);
	PHINode *phi = builder->CreatePHI(convert_type(reqt), "tri_result");
	phi->addIncoming(vb, bbThen);
	phi->addIncoming(vc, bbElse);
	ValueStack_set(ctx, sfpidx, phi);
	return 0;
}

/* TODO */
/* checking espidx using 'for stmt' */
static bool FLAG_FOR_LET = 0;

static int _LET_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx;
	knh_Token_t *tkL = tkNN(stmt, 1);
	if(TT_(tkL) == TT_LVAR || TT_(tkL) == TT_FVAR) {
		int index = Token_index(tkL);
		int a = Tn_put(ctx, stmt, 2, SP(tkL)->type, local+1);
		Value *v = ValueStack_get(ctx, a);
		ValueStack_set(ctx, index, v);
		/* TODO for stmt */
		if (index == sfpidx && DP(stmt)->espidx == 0)
			FLAG_FOR_LET = true;
	}
	else if(IS_Token(tkNN(stmt, 2))) {
		ASM_MOV(ctx, tkL, tkNN(stmt, 2), DP(stmt)->espidx);
	}
	else {
		knh_term_t tt = TT_(tkL); /* NOTE: tkL is reused inside stmt */
		knh_short_t index = (tkL)->index;
		Tn_asm(ctx, stmt, 2, SP(tkL)->type, local);
		TT_(tkL) = tt;
		(tkL)->index = index;
		ASM_MOV(ctx, tkL, tkNN(stmt, 2), DP(stmt)->espidx);
		if(local == sfpidx)
			// we do not need to let variable.
			return 0;
	}
	if(stmt->type != TYPE_void && reqt != TYPE_void) {
		ASM_SMOV(ctx, reqt, Token_index(tkL), tkL);
	}

	return 0;
}

static KMETHOD Fmethod_empty(CTX ctx, knh_sfp_t *sfp _RIX) {}

static knh_Method_t* Gamma_getFmt(CTX ctx, knh_class_t cid, knh_methodn_t mn0)
{
	knh_methodn_t mn = mn0;
	knh_NameSpace_t *ns = K_GMANS;
	knh_Method_t *mtd = knh_NameSpace_getFmtNULL(ctx, ns, cid, mn);
	if(mtd == NULL) {
		WarningUndefinedFmt(ctx, cid, mn0);
		mtd = new_Method(ctx, 0, cid, mn0, Fmethod_empty);
		KNH_SETv(ctx, DP(mtd)->mp, KNH_TNULL(ParamArray));
		knh_NameSpace_addFmt(ctx, ns, mtd);
	}
	return mtd;
}

static int _FMTCALL_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	LLVM_TODO("FMTCALL");
	return 0;
}

static int ASM_SEND(CTX ctx, int sfpidx, int thisidx,const char* s);
static int _W1_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int isCWB = 0;
	int local = DP(stmt)->espidx;
	size_t thisidx = local + K_CALLDELTA;
	knh_Method_t *mtdf  = tkNN(stmt, 0)->mtd;
	DBG_ASSERT(DP(stmt)->size == 3);
	if(TT_(tmNN(stmt, 1)) == TT_ASIS) {
		isCWB = 1;
		ASM_SEND(ctx, sfpidx, thisidx, "CWB");
		KNH_SETv(ctx, tmNN(stmt, 1), knh_Token_toTYPED(ctx, tkNN(stmt, 1), TT_FVAR, TYPE_OutputStream, thisidx));
	}
	else {
		DBG_ASSERT(TT_(tkNN(stmt, 1)) == TT_FVAR);
		DBG_ASSERT(Tn_type(stmt, 1) == TYPE_OutputStream);
		thisidx = tkNN(stmt, 1)->index;
	}
	if(IS_Method(mtdf)) {
		knh_type_t reqt = Tn_type(stmt, 2);
		Tn_asm(ctx, stmt, 2, reqt, thisidx + 1);
		ASM(SCALL, -1, SFP_(thisidx), ESP_((thisidx-K_CALLDELTA), 1), mtdf);
	}
	if(isCWB) {
		ASM_SEND(ctx, sfpidx,  thisidx, "TOSTR");
	}
	return 0;
}

static int ASM_SEND(CTX ctx, int sfpidx, int thisidx, const char *s)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	std::vector<const Type*> argsTy;
	std::string fname(s);
	argsTy.push_back(LLVMTYPE_context);
	argsTy.push_back(LLVMTYPE_sfp);
	argsTy.push_back(LLVMTYPE_Int);
	FunctionType *fnTy = FunctionType::get(LLVMTYPE_Void, argsTy, false);
	std::vector<Value*> params;
	param_setCtxSfp(ctx, params, thisidx);
	params.push_back(LLVMInt(sfpidx-thisidx));
	//asm_shift_esp(ctx, thisidx);

	Function *func = cast<Function>(LLVM_MODULE(ctx)->getOrInsertFunction(fname+"_llvm", fnTy));
	
	builder->CreateCall(func, params.begin(), params.end());

	return 0;
}


static int _SEND_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	int local = sfpidx;
	size_t thisidx = local + K_CALLDELTA;
	int isCWB = 0;
	if(TT_(tmNN(stmt, 1)) == TT_ASIS) {
		isCWB = 1;

		ASM_SEND(ctx, thisidx,  thisidx, "CWB");

		KNH_SETv(ctx, tmNN(stmt, 1), knh_Token_toTYPED(ctx, tkNN(stmt, 1), TT_FVAR, TYPE_OutputStream, thisidx));
	}
	else {
		int j = Tn_put(ctx, stmt, 1, TYPE_OutputStream, thisidx+1);
		Value *v = ValueStack_get(ctx, j);
		v = LLVM_BUILDER(ctx)->CreateBitCast(v, LLVMTYPE_Object, "cast");
		sfp_store(ctx, thisidx, TYPE_OutputStream, v);
	}
	for (size_t i = 2; i < DP(stmt)->size; i++) {
		if(STT_(stmtNN(stmt, i)) == STT_W1) {
			knh_Stmt_t *stmtIN = stmtNN(stmt, i);
			DBG_ASSERT(TT_(tkNN(stmtIN, 1)) == TT_ASIS);
			KNH_SETv(ctx, tkNN(stmtIN, 1), tkNN(stmt, 1));
			_W1_asm(ctx, stmtIN, TYPE_void, local + 1);
		}
		else {
			knh_Method_t *mtd = NULL;
			knh_class_t cid = Tn_cid(stmt, i);
			int j = Tn_put(ctx, stmt, i, cid/* not TYPE_Object*/, thisidx+1);
			Value *v = ValueStack_get_or_load(ctx, j, cid);
			sfp_store(ctx, thisidx+1, cid, v);
			if(cid == CLASS_String) {
				mtd = knh_NameSpace_getMethodNULL(ctx, K_GMANS, TYPE_OutputStream, MN_send);
				DBG_ASSERT(mtd != NULL);
			}
			else {
				mtd = Gamma_getFmt(ctx, cid, MN__s);
			}
			ASM_SCALL(ctx, thisidx, thisidx, mtd);
		}
	}
	if(isCWB) {
		ASM_SEND(ctx, sfpidx, thisidx, "TOSTR");
		Value *v = ValueStack_load(ctx, sfpidx, CLASS_String);
		ValueStack_set(ctx,sfpidx, v);
	}
	return 0;
}


/* ------------------------------------------------------------------------ */

#define CASE_ASM(XX, ...) case STT_##XX : { \
		_##XX##_asm(ctx, stmt, ## __VA_ARGS__); \
		break;\
	}\

static int _EXPR_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	switch(STT_(stmt)) {
	CASE_ASM(LET, reqt, sfpidx);
	CASE_ASM(FUNCCALL, reqt, sfpidx);
	CASE_ASM(CALL, reqt, sfpidx);
	CASE_ASM(CALL1, reqt, sfpidx);
	CASE_ASM(OPR, reqt, sfpidx);
	CASE_ASM(NEW, reqt, sfpidx);
	CASE_ASM(TCAST, reqt, sfpidx);
	CASE_ASM(BOX, reqt, sfpidx);
	CASE_ASM(AND, reqt, sfpidx);
	CASE_ASM(OR, reqt, sfpidx);
	CASE_ASM(ALT, reqt, sfpidx);
	CASE_ASM(TRI, reqt, sfpidx);
	CASE_ASM(SEND, reqt, sfpidx);
	CASE_ASM(W1, reqt, sfpidx);
	CASE_ASM(FMTCALL, reqt, sfpidx);
	default:
		DBG_ABORT("unknown stt=%s", TT__(STT_(stmt)));
	}
	return 0;
}

static void Tn_asm(CTX ctx, knh_Stmt_t *stmt, size_t n, knh_type_t reqt, int local)
{
	if(IS_Token(tkNN(stmt, n))) {
		ASM_SMOV(ctx, reqt, local, tkNN(stmt, n));
	}
	else {
		_EXPR_asm(ctx, stmtNN(stmt, n), reqt, local);
	}
	if(IS_Token(tkNN(stmt, n))) {
		knh_Token_t *tk = tkNN(stmt, n);
		if(TT_(tk) != TT_FVAR) {
			knh_Token_toTYPED(ctx, tk, TT_FVAR, reqt, local);
		}
	}
	else {
		knh_Token_t *tk = new_TokenTYPED(ctx, TT_FVAR, reqt, local);
		KNH_SETv(ctx, tkNN(stmt, n), tk);
	}
}

/* ------------------------------------------------------------------------ */
/* [IF, WHILE, DO, FOR, FOREACH]  */
static bool BB_hasReturn(BasicBlock *bb, BasicBlock *bbTo)
{
	BasicBlock::iterator itr;
	for (itr = bb->begin(); itr != bb->end(); itr++) {
		Instruction &inst = *itr;
		if (ReturnInst::classof(&inst)) {
			return true;
		}
		if (BranchInst::classof(&inst)) {
			BranchInst *bi = (BranchInst *)&inst;
			int n = bi->getNumSuccessors();
			for (int i = 0; i < n; i++) {
				if (bi->getSuccessor(i) == bbTo) {
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
static bool BB_hasReturnOrBreak(BasicBlock *bb)
{
	BasicBlock::iterator itr;
	for (itr = bb->begin(); itr != bb->end(); itr++) {
		Instruction &inst = *itr;
		if(ReturnInst::classof(&inst) || BranchInst::classof(&inst)) {
			return true;
		}
	}
	return false;
}

static void ASM_BBLAST(CTX ctx, void *ptr, void (*func)(CTX, void*))
{
	BasicBlock *bb = LLVM_BUILDER(ctx)->GetInsertBlock();
	if (!BB_hasReturnOrBreak(bb)) {
		func(ctx, ptr);
	}
}

static int Tn_CondAsm(CTX ctx, knh_Stmt_t *stmt, size_t n, int isTRUE, int flocal)
{
	knh_Token_t *tk = tkNN(stmt, n);
	if(TT_(tk) == TT_CONST) {
		/* TODO isTRUE2 */
		//int isTRUE2 = IS_TRUE((tk)->data);
		ASM_SMOV(ctx, TYPE_Boolean, flocal, tk);
		return flocal;
	}
	if(TT_(tk) == TT_LVAR || TT_(tk) == TT_FVAR) {
		int index = Token_index(tk);
		if(isTRUE) {
			ASM(bNOT, NC_(index), NC_(index));
		}
		return index;
	}
	else {
		Tn_asm(ctx, stmt, n, TYPE_Boolean, flocal);
		if(isTRUE) {
			ASM(bNOT, NC_(flocal), NC_(flocal));
		}
		return flocal;
	}
}

static inline void Tn_asmBLOCK(CTX ctx, knh_Stmt_t *stmt, size_t n, knh_type_t reqt)
{
	DBG_ASSERT(IS_Stmt(stmtNN(stmt, n)));
	_BLOCK_asm(ctx, stmtNN(stmt, n), reqt, 0);
}

static void __asm_br(CTX ctx, void *ptr)
{
	BasicBlock *bbMerge = (BasicBlock*) ptr;
	LLVM_BUILDER(ctx)->CreateBr(bbMerge);
}
static void __asm_ret(CTX ctx, void *ptr)
{
	knh_ParamArray_t *pa = DP(DP(ctx->gma)->mtd)->mp;
	knh_class_t rtype = knh_ParamArray_rtype(pa);
	Value *v = ValueStack_get_or_load(ctx, K_RTNIDX, rtype);
	const Type *retTy = LLVM_FUNCTION(ctx)->getReturnType();
	if (retTy != LLVMTYPE_Void) {
		LLVM_BUILDER(ctx)->CreateRet(v);
	} else {
		LLVM_BUILDER(ctx)->CreateRetVoid();
	}
}

static void phi_nop(CTX ctx, knh_Array_t *a, int i, Value *v0, Value *v1, Value *v2, BasicBlock *bb1, BasicBlock *bb2)
{
	(void)a;(void)v1;(void)v2;(void)i;
	(void)bb1;(void)bb2;
}
static void phi_then(CTX ctx, knh_Array_t *a, int i, Value *v0, Value *v1, Value *v2, BasicBlock *bb1, BasicBlock *bb2)
{
	a->nlist[i] = (knh_ndata_t)v1;
}
static void phi_else(CTX ctx, knh_Array_t *a, int i, Value *v0, Value *v1, Value *v2, BasicBlock *bb1, BasicBlock *bb2)
{
	a->nlist[i] = (knh_ndata_t)v2;
}

static void phi_phi(CTX ctx, knh_Array_t *a, int i, Value *v0, Value *v1, Value *v2, BasicBlock *bb1, BasicBlock *bb2)
{
	PHINode *phi = LLVM_BUILDER(ctx)->CreatePHI(v1->getType(), "phi");
	phi->addIncoming(v1, bb1);
	phi->addIncoming(v2, bb2);
	a->nlist[i] = (knh_ndata_t)phi;
}

typedef void (*fphi_t)(CTX ctx, knh_Array_t *a, int i, Value *, Value *, Value *, BasicBlock *, BasicBlock *);

static int PHI_asm(CTX ctx, int local, knh_Array_t *prev, knh_Array_t *thenArray, knh_Array_t *elseArray, BasicBlock *bbThen, BasicBlock *bbElse, BasicBlock *bbMerge)
{
	int size = local + (-1 * K_RTNIDX);
	fphi_t fphi = NULL;
	if (BB_hasReturn(bbThen, bbMerge) && BB_hasReturn(bbElse, bbMerge)) {
		fphi = phi_nop;
	} else if (BB_hasReturn(bbThen, bbMerge)) {
		fphi = phi_else;
	} else if (BB_hasReturn(bbElse, bbMerge)) {
		fphi = phi_then;
	} else {
		fphi = phi_phi;
	}
	for (int i = 0; i < size; i++) {
		Value *vp = (Value *)prev->nlist[i];
		if(vp == NULL) continue;
		Value *v1 = (Value *)thenArray->nlist[i];
		Value *v2 = (Value *)elseArray->nlist[i];
		if(vp != v1 || vp != v2) {
			fphi(ctx, prev, i, vp, v1, v2, bbThen, bbElse);
		}
	}
	return 1;
}

static int _IF_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx _UNUSED_)
{
	Value *cond = NULL;//expr
	int local = DP(stmt)->espidx;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbThen  = BB_CREATE(ctx, "then");
	BasicBlock *bbElse  = BB_CREATE(ctx, "else");
	BasicBlock *bbMerge = BB_CREATE(ctx, "ifcont");

	//Tn_JMPIF(ctx, stmt, 0, 0/*FALSE*/, ElseBB, local);
	int a = Tn_CondAsm(ctx, stmt, 0, 0, local);
	cond = ValueStack_get(ctx, a);
	builder->CreateCondBr(VBOOL(cond), bbThen, bbElse);
	knh_Array_t *prev = DP(ctx->gma)->lstacks;
	knh_Array_t *st1 = ValueStack_copy(ctx, prev);
	knh_Array_t *st2 = ValueStack_copy(ctx, prev);
	BEGIN_LOCAL(ctx, lsfp, 3);
	KNH_SETv(ctx, lsfp[0].o, prev);
	KNH_SETv(ctx, lsfp[1].o, st1);
	KNH_SETv(ctx, lsfp[2].o, st2);

	builder->SetInsertPoint(bbThen);
	KNH_SETv(ctx, DP(ctx->gma)->lstacks, st1);
	Tn_asmBLOCK(ctx, stmt, 1, reqt);
	ASM_BBLAST(ctx, (void*)bbMerge, __asm_br);
	bbThen = builder->GetInsertBlock();

	builder->SetInsertPoint(bbElse);
	KNH_SETv(ctx, DP(ctx->gma)->lstacks, st2);
	Tn_asmBLOCK(ctx, stmt, 2, reqt);
	ASM_BBLAST(ctx, (void*)bbMerge, __asm_br);
	bbElse = builder->GetInsertBlock();

	builder->SetInsertPoint(bbMerge);
	PHI_asm(ctx, local, prev, st1, st2, bbThen, bbElse, bbMerge);
	KNH_SETv(ctx, DP(ctx->gma)->lstacks, prev);

	END_LOCAL(ctx, lsfp);

	return 0;
}

static int _SWITCH_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx)
{
	LLVM_TODO("SWITCH");
	return 0;
}

#if 0
static void ASM_JUMPLABEL(CTX ctx, knh_Stmt_t *stmt, int delta)
{
	LLVM_TODO("JUMPLABEL");
}
#endif

static int addPhiArray(CTX ctx, knh_Array_t *phi, knh_Array_t *block, BasicBlock *bb, int esp);

static int _CONTINUE_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	std::vector<label_stack *> *bbstack = LLVM_BBSTACK(ctx);
	label_stack *l = bbstack->back();
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bb = l->bbCon;
	knh_Array_t *phi = l->phiCon;
	addPhiArray(ctx, phi, DP(ctx->gma)->lstacks, builder->GetInsertBlock(), DP(stmt)->espidx);
	builder->CreateBr(bb);
	return 0;
}

static int _BREAK_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	std::vector<label_stack *> *bbstack = LLVM_BBSTACK(ctx);
	label_stack *l = bbstack->back();
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bb = l->bbBreak;
	knh_Array_t *phi = l->phiBreak;
	addPhiArray(ctx, phi, DP(ctx->gma)->lstacks, builder->GetInsertBlock(), DP(stmt)->espidx);
	builder->CreateBr(bb);
	return 0;
}

static knh_Array_t *createPhiArray(CTX ctx, BasicBlock *bb, knh_Array_t *a, int esp)
{
	knh_Array_t *newlstacks = ValueStack_copy(ctx, a);
	int size = esp + K_CALLDELTA;
	for (int i = K_CALLDELTA; i < size; i++) {
		Value *v = (Value *)knh_Array_n(a, i);
		if (v != NULL) {
			PHINode *phi = PHINode::Create(v->getType(), "phi", bb);
			knh_Array_n(newlstacks, i) = (knh_Object_t *) phi;
		}
	}
	return newlstacks;
}

static int addPhiArray(CTX ctx, knh_Array_t *phi, knh_Array_t *block, BasicBlock *bb, int esp) {
	int size = esp + K_CALLDELTA;
	for (int i = K_CALLDELTA; i < size; i++) {
		Value *p = (Value *)knh_Array_n(phi, i);
		Value *v = (Value *)knh_Array_n(block, i);
		if (p != NULL && v != NULL) {
			if (PHINode::classof(p) && p->getType() == v->getType()) {
				PHINode *phi = static_cast<PHINode*>(p);
				phi->addIncoming(v, bb);
			}
		}
	}
	return 0;
}

static bool hasNoEntryPhi(CTX ctx, knh_Array_t *phi, int esp)
{
	int size = esp + K_CALLDELTA;
	for (int i = K_CALLDELTA; i < size; i++) {
		Value *v = (Value *)knh_Array_n(phi, i);
		if (v != NULL && PHINode::classof(v)) {
			PHINode *phi = static_cast<PHINode*>(v);
			if (phi->getNumIncomingValues() == 0) {
				return true;
			}
		}
	}
	return false;
}

static int _WHILE_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	int local = DP(stmt)->espidx;
	Value *cond;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbContinue = BB_CREATE(ctx, "continue");
	BasicBlock *bbBreak    = BB_CREATE(ctx, "break");
	BasicBlock *bbBlock    = BB_CREATE(ctx, "block");
	BasicBlock *bbPrev     = builder->GetInsertBlock();

	// create phi array
	knh_Array_t *prev = DP(ctx->gma)->lstacks;
	knh_Array_t *conPhi = createPhiArray(ctx, bbContinue, prev, local);
	knh_Array_t *brPhi = createPhiArray(ctx, bbBreak, prev, local);
	DP(ctx->gma)->lstacks = ValueStack_copy(ctx, conPhi);

	builder->CreateBr(bbContinue);
	addPhiArray(ctx, conPhi, prev, bbPrev, local);

	// continue part
	builder->SetInsertPoint(bbContinue);
	if (!Tn_isTRUE(stmt, 0)) {
		int n = Tn_CondAsm(ctx, stmt, 0, 0, local+1);
		cond = ValueStack_get(ctx, n);
	} else {
		cond = LLVMBool(1);
	}
	builder->CreateCondBr(VBOOL(cond), bbBlock, bbBreak);
	addPhiArray(ctx, brPhi, DP(ctx->gma)->lstacks, builder->GetInsertBlock(), local);

	// block part
	builder->SetInsertPoint(bbBlock);
	struct label_stack l = {bbContinue, bbBreak, conPhi, brPhi};
	PUSH_LABEL(ctx, &l);
	Tn_asmBLOCK(ctx, stmt, 1, TYPE_void);
	POP_LABEL(ctx);
	BasicBlock *bbTemp = builder->GetInsertBlock();
	if (!BB_hasReturnOrBreak(bbTemp)) {
		builder->CreateBr(bbContinue);
		addPhiArray(ctx, conPhi, DP(ctx->gma)->lstacks, bbTemp, local);
	}
	
	// break part
	builder->SetInsertPoint(bbBreak);
	DP(ctx->gma)->lstacks = brPhi;
	return 0;
}

static int _DO_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	int local = DP(stmt)->espidx;
	Value *cond;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbContinue = BB_CREATE(ctx, "continue");
	BasicBlock *bbBreak    = BB_CREATE(ctx, "break");
	BasicBlock *bbBlock    = BB_CREATE(ctx, "block");
	BasicBlock *bbPrev     = builder->GetInsertBlock();

	// create phi array
	knh_Array_t *prev = DP(ctx->gma)->lstacks;
	knh_Array_t *blockPhi = createPhiArray(ctx, bbBlock, prev, local);
	knh_Array_t *conPhi = createPhiArray(ctx, bbContinue, prev, local);
	knh_Array_t *brPhi = createPhiArray(ctx, bbBreak, prev, local);
	DP(ctx->gma)->lstacks = ValueStack_copy(ctx, blockPhi);

	builder->CreateBr(bbBlock);
	addPhiArray(ctx, blockPhi, prev, bbPrev, local);

	// block part
	builder->SetInsertPoint(bbBlock);
	struct label_stack l = {bbContinue, bbBreak, conPhi, brPhi};
	PUSH_LABEL(ctx, &l);
	Tn_asmBLOCK(ctx, stmt, 0, TYPE_void);
	POP_LABEL(ctx);
	BasicBlock *bbTemp = builder->GetInsertBlock();
	if (!BB_hasReturnOrBreak(bbTemp)) {
		builder->CreateBr(bbContinue);
		addPhiArray(ctx, conPhi, DP(ctx->gma)->lstacks, bbTemp, local);
	}	

	// continue part
	if(hasNoEntryPhi(ctx, conPhi, local)) {
		LLVM_FUNCTION(ctx)->getBasicBlockList().remove(bbContinue);
	} else {
		DP(ctx->gma)->lstacks = conPhi;
		builder->SetInsertPoint(bbContinue);
		if (!Tn_isTRUE(stmt, 1)) {
			int n = Tn_CondAsm(ctx, stmt, 1, 0, local+1);
			cond = ValueStack_get(ctx, n);
		} else {
			cond = LLVMBool(1);
		}
		builder->CreateCondBr(VBOOL(cond), bbBlock, bbBreak);
		addPhiArray(ctx, blockPhi, DP(ctx->gma)->lstacks, builder->GetInsertBlock(), local);
		addPhiArray(ctx, brPhi, DP(ctx->gma)->lstacks, builder->GetInsertBlock(), local);
	}

	// break part
	builder->SetInsertPoint(bbBreak);
	DP(ctx->gma)->lstacks = brPhi;
	return 0;
}

static int _FOR_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	int local = DP(stmt)->espidx;
	Value *cond;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbContinue = BB_CREATE(ctx, "continue");
	BasicBlock *bbCond     = BB_CREATE(ctx, "cond");
	BasicBlock *bbBreak    = BB_CREATE(ctx, "break");
	BasicBlock *bbBlock    = BB_CREATE(ctx, "block");
	BasicBlock *bbPrev     = builder->GetInsertBlock();

	// init part
	FLAG_FOR_LET = false;
	Tn_asm(ctx, stmt, 0, TYPE_void, local);
	if(FLAG_FOR_LET)
		local += 1;
	builder->CreateBr(bbCond);

	// create phi array
	knh_Array_t *prev = DP(ctx->gma)->lstacks;
	knh_Array_t *condPhi = createPhiArray(ctx, bbCond, prev, local);
	knh_Array_t *contPhi = createPhiArray(ctx, bbContinue, prev, local);
	knh_Array_t *brPhi   = createPhiArray(ctx, bbBreak, prev, local);
	DP(ctx->gma)->lstacks = ValueStack_copy(ctx, condPhi);

	addPhiArray(ctx, condPhi, prev, bbPrev, local);
	
	// condition part
	builder->SetInsertPoint(bbCond);
	if (!Tn_isTRUE(stmt, 1)) {
		int n = Tn_CondAsm(ctx, stmt, 1, 0, local+1);
		cond = ValueStack_get(ctx, n);
	} else {
		cond = LLVMBool(1);
	}
	builder->CreateCondBr(VBOOL(cond), bbBlock, bbBreak);
	addPhiArray(ctx, brPhi, DP(ctx->gma)->lstacks, builder->GetInsertBlock(), local);

	// block part
	builder->SetInsertPoint(bbBlock);
	struct label_stack l = {bbContinue, bbBreak, contPhi, brPhi};
	PUSH_LABEL(ctx, &l);
	Tn_asmBLOCK(ctx, stmt, 3, TYPE_void);
	POP_LABEL(ctx);
	BasicBlock *bbTemp = builder->GetInsertBlock();
	if (!BB_hasReturnOrBreak(bbTemp)) {
		builder->CreateBr(bbContinue);
		addPhiArray(ctx, contPhi, DP(ctx->gma)->lstacks, bbTemp, local);
	}
	
	// continue part
	if (hasNoEntryPhi(ctx, contPhi, local)) {
		LLVM_FUNCTION(ctx)->getBasicBlockList().remove(bbContinue);
	} else {
		builder->SetInsertPoint(bbContinue);
		DP(ctx->gma)->lstacks = contPhi;
		Tn_asmBLOCK(ctx, stmt, 2, TYPE_void);
		builder->CreateBr(bbCond);
		addPhiArray(ctx, condPhi, DP(ctx->gma)->lstacks, builder->GetInsertBlock(), local);
	}

	// break part
	builder->SetInsertPoint(bbBreak);
	DP(ctx->gma)->lstacks = brPhi;

	return 0;
}

static void param_setCtxSfp(CTX ctx, std::vector<Value*> &params, int sfpidx)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Function::arg_iterator args = LLVM_FUNCTION(ctx)->arg_begin();
	Value *arg_ctx = args++;
	Value *arg_sfp = args;
	params.push_back(arg_ctx);
	if (sfpidx) {
		arg_sfp = builder->CreateGEP(arg_sfp, LLVMInt(sfpidx), "sfpsft");
	}
	params.push_back(arg_sfp);
}

static void sfp_store(CTX ctx, int sfpidx, knh_class_t cid, Value *v)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	Value *vsfp = getsfp(ctx);
	vsfp = loadsfp(ctx, builder, vsfp, cid, sfpidx);
	builder->CreateStore(v, vsfp, false/*isVolatile*/);
}

static int _FOREACH_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	Value *cond, *v, *vitr;
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbContinue = BB_CREATE(ctx, "continue");
	BasicBlock *bbBreak    = BB_CREATE(ctx, "break");
	BasicBlock *bbBlock    = BB_CREATE(ctx, "block");

	knh_Token_t *tkN = tkNN(stmt, 0);
	int varidx = Token_index(tkN);
	int itridx = Token_index(tkNN(stmt, 3));
	Tn_asm(ctx, stmt, 1, TYPE_Iterator, itridx);

	vitr = ValueStack_get(ctx, itridx);
	v = vitr;
	v = builder->CreateBitCast(v, LLVMTYPE_Iterator, "cast");
	v = builder->CreateStructGEP(v, 2, "gep");
	v = builder->CreateLoad(v);

	builder->CreateBr(bbContinue);
	builder->SetInsertPoint(bbContinue);

	sfp_store(ctx, itridx, CLASS_Iterator, vitr);
	std::vector<Value*> params;
	param_setCtxSfp(ctx, params, itridx);
	params.push_back(LLVMInt(varidx-itridx));

	cond = builder->CreateCall(v, params.begin(), params.end());

	v = ValueStack_load_set(ctx, varidx, (tkN)->type);
	builder->CreateCondBr(VBOOL(cond), bbBlock, bbBreak);

	builder->SetInsertPoint(bbBlock);
	Tn_asmBLOCK(ctx, stmt, 2, TYPE_void);
	builder->CreateBr(bbContinue);
	builder->SetInsertPoint(bbBreak);
	POP_LABEL(ctx);
	return 0;

}

/* ------------------------------------------------------------------------ */
/* [TRY] */

#define Gamma_inTry(ctx)  IS_Stmt(DP(ctx->gma)->finallyStmt)

#if 0
static void Gamma_setFINALLY(CTX ctx, knh_Stmt_t *stmt)
{
	if(IS_NOTNULL(stmt)) {
		if(IS_NOTNULL(DP(ctx->gma)->finallyStmt)) {
			ErrorMisplaced(ctx);
			KNH_HINT(ctx, "try"); // not nested try
			return;
		}
		KNH_SETv(ctx, DP(ctx->gma)->finallyStmt, stmt);
	}
	else { /* stmt == null */
		KNH_SETv(ctx, DP(ctx->gma)->finallyStmt, stmt);
	}
}
#endif

static void ASM_FINALLY(CTX ctx)
{
	if(IS_NOTNULL(DP(ctx->gma)->finallyStmt)) {
		_BLOCK_asm(ctx, DP(ctx->gma)->finallyStmt, TYPE_void, 0);
	}
}

static int _TRY_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	LLVM_TODO("TRY");
	return 0;
}

static void ASM_CHECKIN(CTX ctx, int thisidx, int sfpidx, knh_class_t cid)
{
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	std::vector<Value*> params;
  int *func = (int*)ClassTBL(CLASS_Assurance)->cdef->checkin;
	Value *vfunc = LLVMValue(LLVMTYPE_checkin, func);


	param_setCtxSfp(ctx, params, thisidx);
	Value *v = ValueStack_get_or_load(ctx, sfpidx, cid);
	params.push_back(v);


	builder->CreateCall(vfunc, params.begin(), params.end());
	return;
}

static void ASM_CHECKOUT(CTX ctx, int thisidx, int sfpidx, knh_class_t cid, int index)
{

	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	std::vector<Value*> params;
  int *func = (int*)ClassTBL(CLASS_Assurance)->cdef->checkout;
	Value *vfunc = LLVMValue(LLVMTYPE_checkout, func);

	Function::arg_iterator args = LLVM_FUNCTION(ctx)->arg_begin();
	Value *arg_ctx = args;
	params.push_back(arg_ctx);
	Value *v = ValueStack_get_or_load(ctx, sfpidx, cid);
	params.push_back(v);
	params.push_back(LLVMInt(cid));

	builder->CreateCall(vfunc, params.begin(), params.end());
	return;

}

static int _ASSURE_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	int local = DP(stmt)->espidx, thisidx = local + K_CALLDELTA;
	int index = Token_index(tkNN(stmt, 2)); // it

	knh_class_t cid = (tkNN(stmt, 2))->cid;
	Tn_asm(ctx, stmt, 0, CLASS_Assurance, index);

	ASM_CHECKIN(ctx, thisidx, local, cid);
	Tn_asmBLOCK(ctx, stmt, 1, TYPE_void);
	ASM_CHECKOUT(ctx, thisidx, local, cid, index);
	return 0;
}

static int _ASSERT_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	Module *m = LLVM_MODULE(ctx);
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	BasicBlock *bbThen  = BB_CREATE(ctx, "then");
	BasicBlock *bbElse  = BB_CREATE(ctx, "else");

	int a = Tn_CondAsm(ctx, stmt, 0, 0, DP(stmt)->espidx);
	Value *cond = ValueStack_get(ctx, a);
	builder->CreateCondBr(VBOOL(cond), bbThen, bbElse);

	builder->SetInsertPoint(bbElse);

	std::vector<const Type*>args_list;
	args_list.push_back(LLVMTYPE_context);
	args_list.push_back(LLVMTYPE_sfp);
	args_list.push_back(LLVMTYPE_Int); /* start */
	args_list.push_back(LLVMTYPE_Int); /* uline */
	FunctionType* fnTy = FunctionType::get(LLVMTYPE_Void, args_list, false);
	Function *func = cast<Function>(m->getOrInsertFunction("knh_assert", fnTy));

	std::vector<Value*> params;
	param_setCtxSfp(ctx, params, 0);
	params.push_back(LLVMInt(DP(stmt)->espidx));
	params.push_back(LLVMInt(stmt->uline));
	builder->CreateCall(func, params.begin(), params.end());

	builder->CreateBr(bbThen);
	builder->SetInsertPoint(bbThen);
	return 0;
}

static int _THROW_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	LLVM_TODO("THROW");
	return 0;
}

static void ASM_LastRET(CTX ctx, knh_Stmt_t *stmt)
{
	//while(DP(stmt)->nextNULL != NULL) {
	//	stmt = DP(stmt)->nextNULL;
	//	if(STT_(stmt) == STT_CHKOUT) {
	//		CHKOUT_asm(ctx, stmt);
	//	}
	//}
	BasicBlock *bb = LLVM_BUILDER(ctx)->GetInsertBlock();
	ASM_BBLAST(ctx, (void*)bb, __asm_ret);
}


static int _RETURN_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx _UNUSED_)
{
	ASM_FINALLY(ctx);
	size_t size = DP(stmt)->size;
	knh_type_t rtype = TYPE_void;
	if(size == 1) {
		rtype = Tn_type(stmt, 0);
		Tn_asm(ctx, stmt, 0, rtype, DP(stmt)->espidx);
	}
	if(IS_Stmt(DP(stmt)->stmtPOST)) {
		_EXPR_asm(ctx, DP(stmt)->stmtPOST, Tn_type(stmt, 0), DP(DP(stmt)->stmtPOST)->espidx);
	}
	if(!Stmt_isImplicit(stmt)) {
		Value *v = NULL;
		if (rtype != TYPE_void) {
			int retidx = (SP(DP(ctx->gma)->mtd)->mn == MN_new)?0:DP(stmt)->espidx;
			v = ValueStack_get_or_load(ctx, retidx, reqt);
		}
		LLVM_BUILDER(ctx)->CreateRet(v);
	}
	return 0;
}

static int _YIELD_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	LLVM_TODO("yield");
	return 0;
}


static int _ERR_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	LLVM_TODO("ERR");
	return 0;
}

/* copied from asm.c */
#define K_FLAG_PF_STDERR      1
#define K_FLAG_PF_EOL         (1<<1)
#define K_FLAG_PF_TIME        (1<<2)
#define K_FLAG_PF_FUNC        (1<<3)
#define K_FLAG_PF_LINE        (1<<4)
#define K_FLAG_PF_NAME        (1<<5)
#define K_FLAG_PF_BOL         (1<<6)
#define K_FLAG_PF_BREAK       (1<<7)


static knh_flag_t PRINT_flag(CTX ctx, knh_Stmt_t *o)
{
	knh_flag_t flag = 0;
	if(IS_Map(DP(o)->metaDictCaseMap)) {
		Object *v = knh_DictMap_getNULL(ctx,  DP(o)->metaDictCaseMap, STEXT("Time"));
		if(v != NULL) {
			flag |= K_FLAG_PF_TIME;
		}
	}
	return flag;
}

static Function *PRINT_func(CTX ctx, const char *name, knh_class_t cid)
{
	Module *m = LLVM_MODULE(ctx);
	std::vector<const Type*>args_list;
	args_list.push_back(LLVMTYPE_context);
	args_list.push_back(LLVMTYPE_sfp);
	args_list.push_back(LLVMTYPE_Int); /* flag */
	args_list.push_back(LLVMTYPE_Int); /* uline */
	args_list.push_back(LLVMTYPE_Object); /* msg */
	args_list.push_back(convert_type(cid));
	FunctionType* fnTy = FunctionType::get(LLVMTYPE_Void, args_list, false);
	Function *func = cast<Function>(m->getOrInsertFunction(name, fnTy));
	return func;
}

struct print_data {
	knh_class_t cid;
	const char *name;
};
static struct print_data PRINT_DATA[] = {
	{CLASS_Boolean, "knh_PRINTb"},
	{CLASS_Int,     "knh_PRINTi"},
	{CLASS_Float,   "knh_PRINTf"},
	{CLASS_Object,  "knh_PRINT"},
};

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
static void init_print_func(llvm::Module *m)
{
	std::vector<const Type*>args;
	for (size_t i = 0; i < ARRAY_SIZE(PRINT_DATA); i++) {
		knh_class_t cid = PRINT_DATA[i].cid;
		const char *name = PRINT_DATA[i].name;
		args.push_back(LLVMTYPE_context);
		args.push_back(LLVMTYPE_sfp);
		args.push_back(LLVMTYPE_Int); /* flag */
		args.push_back(LLVMTYPE_Int); /* uline */
		args.push_back(LLVMTYPE_Object); /* msg */
		args.push_back(convert_type(cid));

		FunctionType* fnTy = FunctionType::get(LLVMTYPE_Void, args, false);
		Function* func = Function::Create(fnTy, GlobalValue::ExternalLinkage, name, m);
		func->setCallingConv(CallingConv::C);
		args.clear();
	}
}

static void ASM_P(CTX ctx, const char *name, knh_flag_t flag, knh_uline_t line, knh_String_t *msg, knh_class_t cid, Value *v)
{
	Function *f = PRINT_func(ctx, name, cid);
	IRBuilder<> *builder = LLVM_BUILDER(ctx);
	std::vector<Value*> params;
	param_setCtxSfp(ctx, params, 0);
	params.push_back(LLVMInt(flag));
	params.push_back(LLVMInt(line));
	params.push_back(LLVMValue(LLVMTYPE_Object, msg)); /* msg */
	params.push_back(v);
	builder->CreateCall(f, params.begin(), params.end());
}

static int _PRINT_asm(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt _UNUSED_, int sfpidx _UNUSED_)
{
	knh_flag_t flag = PRINT_flag(ctx, stmt) | K_FLAG_PF_BOL | K_FLAG_PF_LINE;
	long i, espidx = DP(stmt)->espidx;
	for (i = 0; i < DP(stmt)->size; i++) {
		knh_Token_t *tkn = tkNN(stmt, i);
		if(TT_(tkn) != TT_CONST || !IS_String((tkn)->data)) {
			knh_class_t cid = Tn_cid(stmt, i);
			Tn_asm(ctx, stmt, i, cid, espidx + i);
		}
	}
	for(i = 0; i < DP(stmt)->size; i++) {
		knh_flag_t mask = 0;
		knh_String_t *msg = (knh_String_t*)KNH_NULL;
		L_REDO:;
		knh_Token_t *tkn = tkNN(stmt, i);
		if(i == (long)DP(stmt)->size - 1) {
			mask |= K_FLAG_PF_EOL;
		}
		if(TT_(tkn) == TT_CONST && IS_String((tkn)->data)) {
			if(Token_isPNAME(tkn)) { /* name= */
				msg = (tkn)->text;
				mask |= K_FLAG_PF_NAME; i++;
				goto L_REDO;
			}
			DBG_ASSERT(stmt->uline == ctx->gma->uline);
			knh_class_t cid = CLASS_String;
			int n = Tn_put(ctx, stmt, i, cid, espidx+i);
			Value *v = ValueStack_get(ctx, n);
			const char *fname = "knh_PRINT";
			ASM_P(ctx, fname, flag | mask, tkNN(stmt, i)->uline, msg, cid, v);
		}
		else {
			knh_class_t cid = Tn_cid(stmt, i);
			Value *v = ValueStack_get(ctx, espidx+i);
			const char *fname;
			if(IS_Tint(cid))        fname = "knh_PRINTi";
			else if(IS_Tfloat(cid)) fname = "knh_PRINf";
			else if(IS_Tbool(cid))  fname = "knh_PRINTb";
			else                    fname = "knh_PRINT";
			ASM_P(ctx, fname, flag | mask, tkNN(stmt, i)->uline, msg, cid, v);
			flag=0;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

static int _BLOCK_asm(CTX ctx, knh_Stmt_t *stmtH, knh_type_t reqt, int sfpidx _UNUSED_)
{
	knh_Stmt_t *stmt = stmtH;
	while(stmt != NULL) {
		knh_type_t etype = (DP(stmt)->nextNULL == NULL) ? reqt : TYPE_void;
		ctx->gma->uline = stmt->uline;
		switch(STT_(stmt)) {
		case STT_REGISTER:
		{
			size_t i;
			for(i = 0; i < DP(stmt)->size; i++) {
				knh_Stmt_t *stmtLET = stmtNN(stmt, i);
				if(STT_(stmtLET) == STT_LET) {
					DP(stmtLET)->espidx = DP(stmt)->espidx;
					_LET_asm(ctx, stmtLET, TYPE_Tvoid, DP(stmtH)->espidx/*TODO*/);
				}
			}
			break;
		}
		case STT_BLOCK :
		{
			_BLOCK_asm(ctx, stmtNN(stmt, 0), etype, sfpidx);
			break;
		}
		CASE_ASM(IF      , etype    , 0);
		CASE_ASM(SWITCH  , etype    , 0);
		CASE_ASM(WHILE   , TYPE_void, 0);
		CASE_ASM(DO      , TYPE_void, 0);
		CASE_ASM(FOR     , TYPE_void, 0);
		CASE_ASM(FOREACH , TYPE_void, 0);
		CASE_ASM(BREAK   , TYPE_void, 0);
		CASE_ASM(CONTINUE, TYPE_void, 0);
		CASE_ASM(TRY     , TYPE_void, 0);
		CASE_ASM(THROW   , TYPE_void, 0);
		CASE_ASM(RETURN  , TYPE_void, 0);
		CASE_ASM(YIELD   , TYPE_void, 0);
		CASE_ASM(PRINT   , TYPE_void, 0);
		CASE_ASM(ASSURE  , TYPE_void, 0);
		CASE_ASM(ASSERT  , TYPE_void, 0);
		CASE_ASM(ERR     , TYPE_void, 0);
		case STT_DECL: case STT_DONE: break;
		default:
			_EXPR_asm(ctx, stmt, TYPE_Tvoid, DP(stmt)->espidx);
		}
		stmt = DP(stmt)->nextNULL;
	}
	return 0;
}

enum asmcode_type {
	ASMCODE_TYPE_VOID,
	ASMCODE_TYPE_SHORT,
	ASMCODE_TYPE_LONG,
	ASMCODE_TYPE_INT64,
	ASMCODE_TYPE_VOIDPTR,
	ASMCODE_TYPE_VEC16,
	ASMCODE_TYPE_CONTEXT,
	ASMCODE_TYPE_SFPPTR,
	ASMCODE_TYPE_SYSPTR,
	ASMCODE_TYPE_SYSB,
	ASMCODE_TYPE_OBJECTPTR,
	ASMCODE_TYPE_OBJECTPTRPTR,
	ASMCODE_TYPE_HOBJECT,
	ASMCODE_TYPE_ARRAYPTR,
	ASMCODE_TYPE_OUSPTR,
	ASMCODE_TYPE_INSPTR,
	ASMCODE_TYPE_FUNCTION,
	ASMCODE_TYPE_MAX
};
struct codeasm_field {
	const char *name;
	enum asmcode_type type;
};
struct cstruct {
	const char *name;
	size_t size;
	const struct codeasm_field *fields;
};

#include "./llvm_struct.h"
static const struct codeasm_field F_KNH_FMETHOD[] = {
	{"ctx", ASMCODE_TYPE_CONTEXT},
	{"sfp", ASMCODE_TYPE_SFPPTR},
	{"rix", ASMCODE_TYPE_LONG},
};
#define F_KNH_FITR F_KNH_FMETHOD
static const struct codeasm_field F_KNH_FCHECKIN[] = {
	{"ctx", ASMCODE_TYPE_CONTEXT},
	{"sfp", ASMCODE_TYPE_SFPPTR},
	{"obj", ASMCODE_TYPE_OBJECTPTR},
};
static const struct codeasm_field F_KNH_FCHECKOUT[] = {
	{"ctx", ASMCODE_TYPE_CONTEXT},
	{"obj", ASMCODE_TYPE_OBJECTPTR},
	{"idx", ASMCODE_TYPE_LONG},
};

static const Type *ToType(Module *m, enum asmcode_type type)
{
	const Type *voidPtr = PointerType::get(getLongTy(m), 0);
	switch (type) {
		case ASMCODE_TYPE_VOID:      return Type::getVoidTy(m->getContext());
		case ASMCODE_TYPE_SHORT:     return getShortTy(m);
		case ASMCODE_TYPE_LONG:      return getLongTy(m);
		case ASMCODE_TYPE_INT64:     return LLVMTYPE_Int;
		case ASMCODE_TYPE_VOIDPTR:   return voidPtr;
		case ASMCODE_TYPE_VEC16:     return voidPtr;/* TODO */
		case ASMCODE_TYPE_CONTEXT:   return LLVMTYPE_context;
		case ASMCODE_TYPE_SFPPTR:    return LLVMTYPE_sfp;
		case ASMCODE_TYPE_SYSPTR:    return LLVMTYPE_System;
		case ASMCODE_TYPE_SYSB:      return LLVMTYPE_SystemEX;
		case ASMCODE_TYPE_OBJECTPTR: 
			if (LLVMTYPE_Object)
				return LLVMTYPE_Object;
			else
				return voidPtr;
		case ASMCODE_TYPE_OBJECTPTRPTR: return PointerType::get(LLVMTYPE_Object, 0);
		case ASMCODE_TYPE_HOBJECT:      return LLVMTYPE_hObject;
		case ASMCODE_TYPE_ARRAYPTR:     return LLVMTYPE_Array;
		case ASMCODE_TYPE_OUSPTR:       return LLVMTYPE_OutputStream;
		case ASMCODE_TYPE_INSPTR:       return LLVMTYPE_InputStream;
		case ASMCODE_TYPE_FUNCTION:     return LLVMTYPE_fcall;
		default: return getLongTy(m);
	}
}

enum gen_type { LLVM_ObjectRef, LLVM_Struct, LLVM_Function };

static const Type *GenType(Module *m, const char *name, const struct cstruct &cs, enum gen_type type = LLVM_ObjectRef)
{
	std::vector<const Type*>fields;
	StructType* structTy  = NULL;
	const Type *ptrTy;

	for (size_t i = 0; i < cs.size; ++i) {
		fields.push_back(ToType(m, cs.fields[i].type));
	}
	if (type == LLVM_Function) {
		const Type *voidTy = Type::getVoidTy(m->getContext());
		FunctionType *Ty = FunctionType::get(voidTy, fields, false);
		m->addTypeName(name, Ty);
		return PointerType::get(Ty, 0);
	}
	else if (type == LLVM_Struct) {
		structTy = StructType::get(m->getContext(), fields, false);
		m->addTypeName(name, structTy);
		return structTy;
	}
	else {
		structTy = StructType::get(m->getContext(), fields, false);
		ptrTy = PointerType::get(structTy, 0);
		m->addTypeName(name, ptrTy);
		return ptrTy;
	}
}

static void ConstructObjectStruct(Module *m)
{
	/* hObject */
	LLVMTYPE_hObject = GenType(m, "hObject", STRUCT_KNH_HOBJECT_T, LLVM_Struct);
	LLVMTYPE_Object = GenType(m, "Object", STRUCT_KNH_OBJECT_T);
	LLVMTYPE_ObjectField = GenType(m, "ObjectField", STRUCT_KNH_OBJECTFIELD_T);
	LLVMTYPE_Array = GenType(m, "Array", STRUCT_KNH_ARRAY_T);
	LLVMTYPE_OutputStream = GenType(m, "OutputStream", STRUCT_KNH_OUTPUTSTREAM_T);
	LLVMTYPE_InputStream  = GenType(m, "InputStream",  STRUCT_KNH_INPUTSTREAM_T);
	LLVMTYPE_SystemEX = GenType(m, "SystemEX", STRUCT_KNH_SYSTEMEX_T);
	LLVMTYPE_System = GenType(m, "System", STRUCT_KNH_SYSTEM_T);

	/* sfp */
	/* TODO */
	std::vector<const Type*>fields;
	fields.push_back(LLVMTYPE_Object);
	//fields.push_back(LLVMTYPE_Int);
	fields.push_back(LLVMTYPE_Float);
	const Type *sfpTy  = StructType::get(m->getContext(), fields, /*isPacked=*/false);
	const Type *sfpPtr = PointerType::get(sfpTy, 0);
	m->addTypeName("struct.knh_sfp_t", sfpTy);
	m->addTypeName("knh_sfp_t_ptr", sfpPtr);
	LLVMTYPE_sfp = sfpPtr;
	fields.clear();

	/* ctx */
	const Type *ctxTy = GenType(m, "struct context", STRUCT_KNH_CONTEXT_T, LLVM_Struct);
	const Type *ctxPtr = PointerType::get(ctxTy, 0);
	m->addTypeName("ctx", ctxPtr);
	LLVMTYPE_context = ctxPtr;

	// Function Type Definitions
	struct cstruct fcs = {};
	fcs.name = "fcall";fcs.size = 3;fcs.fields = F_KNH_FMETHOD;
	LLVMTYPE_fcall = GenType(m, "fcall", fcs, LLVM_Function);
	/* checkin */
	fcs.name = "fcheckin";fcs.size = 3;fcs.fields = F_KNH_FCHECKIN;
	LLVMTYPE_checkin = GenType(m, "fcheckin", fcs, LLVM_Function);
	/* checkout */
	fcs.name = "fcheckout";fcs.size = 3;fcs.fields = F_KNH_FCHECKOUT;
	LLVMTYPE_checkout = GenType(m, "fcheckout", fcs, LLVM_Function);
	/* fitr */
	fcs.name = "fitr";fcs.size = 3;fcs.fields = F_KNH_FITR;
	LLVMTYPE_itr = GenType(m, "fitr", fcs, LLVM_Function);

	LLVMTYPE_Method = GenType(m, "Method", STRUCT_KNH_METHOD_T);
	LLVMTYPE_Iterator = GenType(m, "Iterator", STRUCT_KNH_ITERATOR_T);
}

static std::string build_function_name(CTX ctx, knh_Method_t *mtd, std::string suffix)
{
	static int count = 0;
	std::string fname(knh_getmnname(ctx, SP(mtd)->mn));
	std::stringstream ss;
	ss << CLASS__(SP(mtd)->cid);

	if (fname == "")
		ss << "_" << (count++/2) << suffix;
	else
		ss <<  "_" << fname << suffix;
	return ss.str();
}

static Function *build_function(CTX ctx, Module *m, knh_Method_t *mtd)
{
	size_t i;
	knh_ParamArray_t *pa = DP(mtd)->mp;
	knh_class_t retTy = knh_ParamArray_rtype(pa);;
	std::vector<const Type*> argsTy;
	std::string name = build_function_name(ctx, mtd, "");

	argsTy.push_back(LLVMTYPE_context);
	argsTy.push_back(LLVMTYPE_sfp);
	for (i = 0; i < pa->psize; i++) {
		knh_type_t type = knh_Method_ptype(ctx, mtd, i, mtd->cid);
		argsTy.push_back(convert_type(type));
	}
	FunctionType *fnTy = FunctionType::get(convert_type(retTy), argsTy, false);
	Function *func = cast<Function>(m->getOrInsertFunction(name, fnTy));
	Function::arg_iterator args = func->arg_begin();
	VNAME_(args++, "ctx");
	VNAME_(args, "sfp");
	return func;
}

static Value *loadsfp(CTX ctx, IRBuilder<> *builder, Value *v, knh_type_t type, int idx0)
{
	unsigned int idx = TC_(type, 0);
	v = builder->CreateConstGEP2_32(v, (unsigned)idx0, (unsigned)idx, "gep");
	if (IS_Tint(type) || IS_Tbool(type)) {
		v = builder->CreateBitCast(v, PointerType::get(convert_type(type), 0), "cast");
	}
	return v;
}

static Function *build_wrapper_func(CTX ctx, Module *m, knh_Method_t *mtd, Function *orig_func)
{
	knh_ParamArray_t *pa = DP(mtd)->mp;
	knh_class_t retTy = knh_ParamArray_rtype(pa);
	std::string name = build_function_name(ctx, mtd, "_wrapper");
	const FunctionType *fnTy = cast<FunctionType>(m->getTypeByName("fcall"));
	Function *f = cast<Function>(m->getOrInsertFunction(name, fnTy));
	BasicBlock *bb = BasicBlock::Create(LLVM_CONTEXT(), "EntryBlock", f);
	IRBuilder<> *builder = new IRBuilder<>(bb);

	Function::arg_iterator args = f->arg_begin();
	Value *arg_ctx = VNAME_(args++, "ctx");
	Value *arg_sfp = VNAME_(args++, "sfp");
	VNAME_(args++, "rix");

	builder->SetInsertPoint(bb);

	/* load konoha args from sfp and build call params*/
	std::vector<Value*> params;
	params.push_back(arg_ctx);
	params.push_back(arg_sfp);
	for (size_t i = 0; i < pa->psize; i++) {
		kparam_t *p = knh_ParamArray_get(DP(mtd)->mp, i);
		Value *v = arg_sfp;
		v = loadsfp(ctx, builder, v, p->type, i+1);
		v = builder->CreateLoad(v, "load");
		params.push_back(v);
	}

	/* call function and  set return values */
	{
		Value *callinst = builder->CreateCall(orig_func, params.begin(), params.end());
		if (retTy != TYPE_void) {
			Value *v = arg_sfp;
			v = loadsfp(ctx, builder, v, retTy, K_RTNIDX);
			builder->CreateStore(callinst, v, false/*isVolatile*/);
		}
		builder->CreateRetVoid();
	}
	delete builder;
	return f;
}

AsmContext::AsmContext (Module *m, CTX ctx, knh_Method_t *mtd) :
	m_(m), ctx_(ctx), mtd_(mtd),
	F_(build_function(ctx_, m_, mtd_)),
	B_(new IRBuilder<>(BasicBlock::Create(LLVM_CONTEXT(), "EntryBlock", F_))),
	lstacks(new std::vector<label_stack *>())
{
	Function::arg_iterator args = F_->arg_begin();
	args++;/*ctx*/
	args++;/*sfp*/
	for (size_t i = 0; i < DP(mtd)->mp->psize; i++) {
		kparam_t *p = knh_ParamArray_get(DP(mtd)->mp, i);
		Value *arg = VNAME_(args++, FN__(p->fn));
		ValueStack_set(ctx, i+1, arg);
	}
	knh_Gamma_t *gma = ctx_->gma;
	DP(gma)->asm_data = (void*) this;
	KNH_SETv(ctx, DP(mtd)->kcode, new_Array(ctx, CLASS_String, 0));
}

AsmContext::~AsmContext () {
	Finish(ctx_, mtd_);
	knh_Gamma_t *gma = ctx_->gma;
	DP(gma)->asm_data = NULL;
	delete B_;
	delete lstacks;
}

void AsmContext::Finish(CTX ctx, knh_Method_t *mtd) {
	knh_Fmethod f = NULL;
	Module *m = LLVM_MODULE(ctx);
	Function *func = LLVM_FUNCTION(ctx);
	/* asm for script function is done. */

	/* build wrapper function and compile to native code. */
	ExecutionEngine *ee = LLVM_EE(ctx);
	Function *func1 = build_wrapper_func(ctx, m, mtd, func);

#ifdef K_USING_DEBUG
	(*m).dump();
#endif

	/* optimization */
	FunctionPassManager pm(m);
	pm.add(new TargetData(*(ee->getTargetData())));
	pm.add(createVerifierPass());
	pm.add(createInstructionCombiningPass()); // Cleanup for scalarrepl.
	pm.add(createLICMPass());                 // Hoist loop invariants
	pm.add(createIndVarSimplifyPass());       // Canonicalize indvars
	pm.add(createLoopDeletionPass());         // Delete dead loops

	//Simplify code
	for(int repeat=0; repeat < 3; repeat++) {
		pm.add(createGVNPass());                  // Remove redundancies
		pm.add(createSCCPPass());                 // Constant prop with SCCP
		pm.add(createCFGSimplificationPass());    // Merge & remove BBs
		pm.add(createInstructionCombiningPass());
		pm.add(createConstantPropagationPass());
		pm.add(createAggressiveDCEPass());        // Delete dead instructions
		pm.add(createCFGSimplificationPass());    // Merge & remove BBs
		pm.add(createDeadStoreEliminationPass()); // Delete dead stores
		pm.add(createDemoteRegisterToMemoryPass());
	}

	pm.doInitialization();
	pm.run(*func);
	pm.run(*func1);

	f = (knh_Fmethod) ee->getPointerToFunction(func1);
	knh_Method_setFunc(ctx, mtd, f);
}

//static void parseFile(const char *fname)
//{
//	std::string parse_error;
//	OwningPtr<MemoryBuffer> buffer;
//	error_code error = MemoryBuffer::getFile(fname, buffer);
//	std::auto_ptr<Module> m(ParseBitcodeFile(buffer.get(), LLVM_CONTEXT(), &parse_error));
//}

#define _ALLOW_asm _EXPR_asm
#define _DENY_asm _EXPR_asm
#define _CASE_asm _EXPR_asm
#define _REGISTER_asm _EXPR_asm
#define _DECL_asm _EXPR_asm
#define _ACALL_asm _EXPR_asm
#define _ASM_(prefix, f) prefix _ ## f
#define MAX_CODEASM (31)
typedef int (*fcodeasm)(CTX ctx, knh_Stmt_t *stmt, knh_type_t reqt, int sfpidx);
typedef struct CodeAsm {
	fcodeasm asm_[MAX_CODEASM];
} CodeWriter_t;
struct CodeAsm CODEASM_ = {{
	_ASM_(ASM_PREFIX, ALLOW_asm),
	_ASM_(ASM_PREFIX, DENY_asm),
	_ASM_(ASM_PREFIX, RETURN_asm),
	_ASM_(ASM_PREFIX, YIELD_asm),
	_ASM_(ASM_PREFIX, IF_asm),
	_ASM_(ASM_PREFIX, SWITCH_asm),
	_ASM_(ASM_PREFIX, CASE_asm),
	_ASM_(ASM_PREFIX, WHILE_asm),
	_ASM_(ASM_PREFIX, DO_asm),
	_ASM_(ASM_PREFIX, FOR_asm),
	_ASM_(ASM_PREFIX, THROW_asm),
	_ASM_(ASM_PREFIX, PRINT_asm),
	_ASM_(ASM_PREFIX, ASSURE_asm),
	_ASM_(ASM_PREFIX, ASSERT_asm),
	_ASM_(ASM_PREFIX, REGISTER_asm),
	_ASM_(ASM_PREFIX, DECL_asm),
	_ASM_(ASM_PREFIX, LET_asm),
	_ASM_(ASM_PREFIX, TCAST_asm),
	_ASM_(ASM_PREFIX, OPR_asm),
	_ASM_(ASM_PREFIX, CALL_asm),
	_ASM_(ASM_PREFIX, NEW_asm),
	_ASM_(ASM_PREFIX, FUNCCALL_asm),
	_ASM_(ASM_PREFIX, ACALL_asm),
	_ASM_(ASM_PREFIX, ALT_asm),
	_ASM_(ASM_PREFIX, TRI_asm),
	_ASM_(ASM_PREFIX, AND_asm),
	_ASM_(ASM_PREFIX, OR_asm),
	_ASM_(ASM_PREFIX, W1_asm),
	_ASM_(ASM_PREFIX, SEND_asm),
	_ASM_(ASM_PREFIX, FMTCALL_asm),
	_ASM_(ASM_PREFIX, CALL1_asm),
}};
} /* namespace llvmasm */

#ifdef __cplusplus
extern "C" {
#endif

void knh_llvm_init(int argc, int n, const char **argv)
{
	using namespace llvm;
	using namespace llvmasm;
	InitializeNativeTarget();
	//TODO multithread mode
	//llvm_start_multithreaded();
	mod_global = new Module("test", LLVM_CONTEXT());
	ee_global = EngineBuilder(mod_global).setEngineKind(EngineKind::JIT).create();
	ConstructObjectStruct(mod_global);
	init_print_func(mod_global);
}

void knh_llvm_exit(void)
{
	delete llvmasm::ee_global;
	//delete llvmasm::mod_global;
	llvm::llvm_shutdown();
}

void knh_LLVMMethod_asm(CTX ctx, knh_Method_t *mtd, knh_Stmt_t *stmtB)
{
	knh_Array_t *lstack_org, *lstack;
	knh_Array_t *insts_org, *insts;
	int i;

#define STACK_N 64
	insts     = new_Array(ctx, CLASS_Int, 8);
	lstack    = new_Array(ctx, CLASS_Int, STACK_N);
	insts_org  = DP(ctx->gma)->insts;
	lstack_org = DP(ctx->gma)->lstacks;
	BEGIN_LOCAL(ctx, lsfp, 4);
	KNH_SETv(ctx, lsfp[0].o, insts);
	KNH_SETv(ctx, lsfp[1].o, insts_org);
	KNH_SETv(ctx, lsfp[2].o, lstack);
	KNH_SETv(ctx, lsfp[3].o, lstack_org);
	KNH_SETv(ctx, DP(ctx->gma)->insts, insts);
	KNH_SETv(ctx, DP(ctx->gma)->lstacks, lstack);

	{
		BEGIN_LOCAL(ctx, lsfp2, 1);
		for (i = 0; i < STACK_N; i++) {
			lsfp2[0].ivalue = 0;
			lstack->api->add(ctx, lstack, lsfp2);
		}
		END_LOCAL(ctx, lsfp2);
	}

	{
		llvmasm::AsmContext asmctx(llvmasm::mod_global, ctx, mtd);
		DBG_ASSERT(knh_Array_size(DP(ctx->gma)->insts) == 0);
		SP(ctx->gma)->uline = SP(stmtB)->uline;
		if(Method_isStatic(mtd) && Gamma_hasFIELD(ctx->gma)) {
			llvmasm::asm_nulval(ctx, 0, DP(ctx->gma)->this_cid);
		}
		llvmasm::_BLOCK_asm(ctx, stmtB, knh_ParamArray_rtype(DP(mtd)->mp), 0);
		llvmasm::ASM_LastRET(ctx, stmtB);
	}

	KNH_SETv(ctx, DP(ctx->gma)->insts, insts_org);
	KNH_SETv(ctx, DP(ctx->gma)->lstacks, lstack_org);
	END_LOCAL(ctx, lsfp);
}

void CWB_llvm(CTX ctx, knh_sfp_t *sfp, knh_sfpidx_t c)
{
	knh_cwb_t cwbbuf, *cwb = knh_cwb_open(ctx, &cwbbuf);
	KNH_SETv(ctx, sfp[c].o, cwb->w);
	sfp[c].ivalue = cwb->pos;
}

void TOSTR_llvm(CTX ctx, knh_sfp_t *sfp, knh_sfpidx_t c)
{
	DBG_ASSERT(IS_OutputStream(sfp[0].w));
	knh_cwb_t cwbbuf = {ctx->bufa, ctx->bufw, (size_t)(sfp[0].ivalue)};
	knh_String_t *s = knh_cwb_newString(ctx, &cwbbuf, 0);
	KNH_SETv(ctx, sfp[c].o, s);
}

void BOX_llvm(CTX ctx, knh_sfp_t *sfp, knh_sfpidx_t c, const knh_ClassTBL_t *ct)
{
	Object *v = new_Boxing(ctx, sfp, ct);
	KNH_SETv(ctx, sfp[c].o, v);
}


#ifdef __cplusplus
}
#endif
