/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2011-2012 Masahiro Ide <imasahiro9 at gmail.com>
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

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Intrinsics.h>
#include <llvm/Attributes.h>
#include <llvm/PassManager.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/UnifyFunctionExitNodes.h>
#ifdef USE_LLVM_3_1
#include <llvm/Transforms/Vectorize.h>
#endif
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/DomPrinter.h>
#include <llvm/Analysis/RegionPass.h>
#include <llvm/Analysis/RegionPrinter.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/Lint.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/ADT/Statistic.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/ADT/Triple.h>

#include <iostream>

#undef HAVE_SYS_TYPES_H
#undef HAVE_SYS_STAT_H
#undef HAVE_UNISTD_H
#undef HAVE_SYS_TIME_H
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_VERSION
#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>

namespace konoha {
template <class T>
inline T object_cast(kRawPtr *po)
{
	return static_cast<T>(po->rawptr);
}

template <class T>
inline void convert_array(std::vector<T> &vec, kArray *a)
{
	size_t size = a->size;
	for (size_t i=0; i < size; i++) {
		T v = konoha::object_cast<T>(a->ptrs[i]);
		vec.push_back(v);
	}
}
}

using namespace llvm;

#ifdef __cplusplus
extern "C" {
#endif

#define LLVM_TODO(str) do {\
	fprintf(stderr, "(TODO: %s %d):", __func__, __LINE__);\
	fprintf(stderr, "%s\n", str);\
	abort();\
} while (0)

#define LLVM_WARN(str) do {\
	fprintf(stderr, "(WARN: %s %d):", __func__, __LINE__);\
	fprintf(stderr, "%s\n", str);\
} while (0)

#define _UNUSED_ __attribute__((unused))

#define PKG_NULVAL(T) PKG_NULVAL_##T
#define PKG_NULVAL_int    (0)
#define PKG_NULVAL_float  (0.0)
#define PKG_NULVAL_String (KNH_NULVAL(CLASS_String))
#define WRAP(ptr) ((void*)ptr)
#define Int_to(T, a)               ((T)a.ivalue)
#define DEFAPI(T) T

static void Type_init(CTX _UNUSED_, kRawPtr *po, void *conf)
{
	po->rawptr = conf;
}
static void Type_free(CTX _UNUSED_, kRawPtr *po)
{
	po->rawptr = NULL;
}

static inline kRawPtr *new_ReturnCppObject(CTX, ksfp_t *sfp, void *ptr _RIX)
{
	kObject *defobj = sfp[K_RIX].o;
	kObject *ret = new_kObject(O_ct(defobj), ptr);
	return (kRawPtr*) ret;
}

//## @Static Type Type.getVoidTy();
KMETHOD Type_getVoidTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getVoidTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static Type Type.getLabelTy();
KMETHOD Type_getLabelTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getLabelTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static Type Type.getFloatTy();
KMETHOD Type_getFloatTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getFloatTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static Type Type.getDoubleTy();
KMETHOD Type_getDoubleTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getDoubleTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static Type Type.getMetadataTy();
KMETHOD Type_getMetadataTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getMetadataTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static Type Type.getX86FP80Ty();
KMETHOD Type_getX86FP80Ty(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getX86_FP80Ty(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static Type Type.getFP128Ty();
KMETHOD Type_getFP128Ty(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getFP128Ty(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static Type Type.getPPCFP128Ty();
KMETHOD Type_getPPCFP128Ty(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getPPC_FP128Ty(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static Type Type.getX86MMXTy();
KMETHOD Type_getX86MMXTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getX86_MMXTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static IntegerType Type.getInt1Ty();
KMETHOD Type_getInt1Ty(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt1Ty(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static IntegerType Type.getInt8Ty();
KMETHOD Type_getInt8Ty(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt8Ty(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static IntegerType Type.getInt16Ty();
KMETHOD Type_getInt16Ty(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt16Ty(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static IntegerType Type.getInt32Ty();
KMETHOD Type_getInt32Ty(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt32Ty(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static IntegerType Type.getInt64Ty();
KMETHOD Type_getInt64Ty(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt64Ty(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getFloatPtrTy();
KMETHOD Type_getFloatPtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getFloatPtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getDoublePtrTy();
KMETHOD Type_getDoublePtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getDoublePtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getX86FP80PtrTy();
KMETHOD Type_getX86FP80PtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getX86_FP80PtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getFP128PtrTy();
KMETHOD Type_getFP128PtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getFP128PtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getPPCFP128PtrTy();
KMETHOD Type_getPPCFP128PtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getPPC_FP128PtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getX86MMXPtrTy();
KMETHOD Type_getX86MMXPtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getX86_MMXPtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getInt1PtrTy();
KMETHOD Type_getInt1PtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt1PtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getInt8PtrTy();
KMETHOD Type_getInt8PtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt8PtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getInt16PtrTy();
KMETHOD Type_getInt16PtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt16PtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getInt32PtrTy();
KMETHOD Type_getInt32PtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt32PtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType Type.getInt64PtrTy();
KMETHOD Type_getInt64PtrTy(CTX, ksfp_t *sfp _RIX)
{
	const Type *ptr = Type::getInt64PtrTy(getGlobalContext());
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static PointerType PointerType.get(Type type);
KMETHOD PointerType_get(CTX, ksfp_t *sfp _RIX)
{
	Type *type = konoha::object_cast<Type *>(sfp[1].p);
	const Type *ptr  = PointerType::get(type, 0);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## IRBuilder IRBuilder.new(BasicBlock bb);
KMETHOD IRBuilder_new(CTX, ksfp_t *sfp _RIX)
{
	BasicBlock *bb = konoha::object_cast<BasicBlock *>(sfp[1].p);
	IRBuilder<> *self = new IRBuilder<>(bb);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(self) K_RIXPARAM);
	RETURN_(p);
}

//## ReturnInst IRBuilder.CreateRetVoid();
KMETHOD IRBuilder_createRetVoid(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	ReturnInst *ptr = self->CreateRetVoid();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ReturnInst IRBuilder.CreateRet(Value V);
KMETHOD IRBuilder_createRet(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	ReturnInst *ptr = self->CreateRet(V);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

////## ReturnInst IRBuilder.CreateAggregateRet(Value retVals, int N);
//KMETHOD IRBuilder_createAggregateRet(CTX, ksfp_t *sfp _RIX)
//{
//	LLVM_TODO("NO SUPPORT");
//	//IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
//	//Value *const retVals = konoha::object_cast<Value *const>(sfp[1].p);
//	//kint_t N = Int_to(kint_t,sfp[2]);
//	//ReturnInst *ptr = self->CreateAggregateRet(retVals, N);
//	//kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	//RETURN_(p);
//}

//## BranchInst IRBuilder.CreateBr(BasicBlock Dest);
KMETHOD IRBuilder_createBr(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	BasicBlock *Dest = konoha::object_cast<BasicBlock *>(sfp[1].p);
	BranchInst *ptr = self->CreateBr(Dest);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## BranchInst IRBuilder.CreateCondBr(Value Cond, BasicBlock True, BasicBlock False);
KMETHOD IRBuilder_createCondBr(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Cond = konoha::object_cast<Value *>(sfp[1].p);
	BasicBlock *True = konoha::object_cast<BasicBlock *>(sfp[2].p);
	BasicBlock *False = konoha::object_cast<BasicBlock *>(sfp[3].p);
	BranchInst *ptr = self->CreateCondBr(Cond, True, False);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## SwitchInst IRBuilder.CreateSwitch(Value V, BasicBlock Dest);
KMETHOD IRBuilder_createSwitch(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	BasicBlock *Dest = konoha::object_cast<BasicBlock *>(sfp[2].p);
	SwitchInst *ptr = self->CreateSwitch(V, Dest);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## IndirectBrInst IRBuilder.CreateIndirectBr(Value Addr);
KMETHOD IRBuilder_createIndirectBr(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Addr = konoha::object_cast<Value *>(sfp[1].p);
	IndirectBrInst *ptr = self->CreateIndirectBr(Addr);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## InvokeInst IRBuilder.CreateInvoke0(Value Callee, BasicBlock NormalDest, BasicBlock UnwindDest);
KMETHOD IRBuilder_createInvoke0(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	BasicBlock *NormalDest = konoha::object_cast<BasicBlock *>(sfp[2].p);
	BasicBlock *UnwindDest = konoha::object_cast<BasicBlock *>(sfp[3].p);
	InvokeInst *ptr = self->CreateInvoke(Callee, NormalDest, UnwindDest);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## InvokeInst IRBuilder.CreateInvoke1(Value Callee, BasicBlock NormalDest, BasicBlock UnwindDest, Value Arg1);
KMETHOD IRBuilder_createInvoke1(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	BasicBlock *NormalDest = konoha::object_cast<BasicBlock *>(sfp[2].p);
	BasicBlock *UnwindDest = konoha::object_cast<BasicBlock *>(sfp[3].p);
	Value *Arg1 = konoha::object_cast<Value *>(sfp[4].p);
	InvokeInst *ptr = self->CreateInvoke(Callee, NormalDest, UnwindDest, Arg1);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## InvokeInst IRBuilder.CreateInvoke3(Value Callee, BasicBlock NormalDest, BasicBlock UnwindDest, Value Arg1, Value Arg2, Value Arg3);
KMETHOD IRBuilder_createInvoke3(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	BasicBlock *NormalDest = konoha::object_cast<BasicBlock *>(sfp[2].p);
	BasicBlock *UnwindDest = konoha::object_cast<BasicBlock *>(sfp[3].p);
	Value *Arg1 = konoha::object_cast<Value *>(sfp[4].p);
	Value *Arg2 = konoha::object_cast<Value *>(sfp[5].p);
	Value *Arg3 = konoha::object_cast<Value *>(sfp[6].p);
	InvokeInst *ptr = self->CreateInvoke3(Callee, NormalDest, UnwindDest, Arg1, Arg2, Arg3);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

////## InvokeInst IRBuilder.CreateInvoke(Value Callee, BasicBlock NormalDest, BasicBlock UnwindDest, ArrayRef<Value> Args);
//KMETHOD IRBuilder_createInvoke(CTX, ksfp_t *sfp _RIX)
//{
//	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
//	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
//	BasicBlock *NormalDest = konoha::object_cast<BasicBlock *>(sfp[2].p);
//	BasicBlock *UnwindDest = konoha::object_cast<BasicBlock *>(sfp[3].p);
//	kArray *Args = (sfp[4].a);
//	std::vector<Value*> List;
//	konoha::convert_array(List, Args);
//	InvokeInst *ptr = self->CreateInvoke(Callee, NormalDest, UnwindDest, List.begin(), List.end());
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

////## ResumeInst IRBuilder.CreateResume(Value Exn);
//KMETHOD IRBuilder_createResume(CTX, ksfp_t *sfp _RIX)
//{
//	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
//	Value *Exn = konoha::object_cast<Value *>(sfp[1].p);
//	ResumeInst *ptr = self->CreateResume(Exn);
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}
//
//## UnreachableInst IRBuilder.CreateUnreachable();
KMETHOD IRBuilder_createUnreachable(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	UnreachableInst *ptr = self->CreateUnreachable();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateAdd(Value LHS, Value RHS);
KMETHOD IRBuilder_createAdd(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateAdd(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNSWAdd(Value LHS, Value RHS);
KMETHOD IRBuilder_createNSWAdd(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateNSWAdd(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNUWAdd(Value LHS, Value RHS);
KMETHOD IRBuilder_createNUWAdd(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateNUWAdd(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFAdd(Value LHS, Value RHS);
KMETHOD IRBuilder_createFAdd(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFAdd(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateSub(Value LHS, Value RHS);
KMETHOD IRBuilder_createSub(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateSub(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNSWSub(Value LHS, Value RHS);
KMETHOD IRBuilder_createNSWSub(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateNSWSub(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNUWSub(Value LHS, Value RHS);
KMETHOD IRBuilder_createNUWSub(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateNUWSub(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFSub(Value LHS, Value RHS);
KMETHOD IRBuilder_createFSub(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFSub(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateMul(Value LHS, Value RHS);
KMETHOD IRBuilder_createMul(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateMul(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNSWMul(Value LHS, Value RHS);
KMETHOD IRBuilder_createNSWMul(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateNSWMul(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNUWMul(Value LHS, Value RHS);
KMETHOD IRBuilder_createNUWMul(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateNUWMul(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFMul(Value LHS, Value RHS);
KMETHOD IRBuilder_createFMul(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFMul(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateUDiv(Value LHS, Value RHS);
KMETHOD IRBuilder_createUDiv(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateUDiv(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateExactUDiv(Value LHS, Value RHS);
KMETHOD IRBuilder_createExactUDiv(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateExactUDiv(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateSDiv(Value LHS, Value RHS);
KMETHOD IRBuilder_createSDiv(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateSDiv(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateExactSDiv(Value LHS, Value RHS);
KMETHOD IRBuilder_createExactSDiv(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateExactSDiv(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFDiv(Value LHS, Value RHS);
KMETHOD IRBuilder_createFDiv(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFDiv(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateURem(Value LHS, Value RHS);
KMETHOD IRBuilder_createURem(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateURem(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateSRem(Value LHS, Value RHS);
KMETHOD IRBuilder_createSRem(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateSRem(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFRem(Value LHS, Value RHS);
KMETHOD IRBuilder_createFRem(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFRem(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateShl(Value LHS, Value RHS);
KMETHOD IRBuilder_createShl(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateShl(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateLShr(Value LHS, Value RHS);
KMETHOD IRBuilder_createLShr(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateLShr(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateAShr(Value LHS, Value RHS);
KMETHOD IRBuilder_createAShr(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateAShr(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateAnd(Value LHS, Value RHS);
KMETHOD IRBuilder_createAnd(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateAnd(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateOr(Value LHS, Value RHS);
KMETHOD IRBuilder_createOr(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateOr(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateXor(Value LHS, Value RHS);
KMETHOD IRBuilder_createXor(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateXor(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNeg(Value V);
KMETHOD IRBuilder_createNeg(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Value *ptr = self->CreateNeg(V);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNSWNeg(Value V);
KMETHOD IRBuilder_createNSWNeg(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Value *ptr = self->CreateNSWNeg(V);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNUWNeg(Value V);
KMETHOD IRBuilder_createNUWNeg(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Value *ptr = self->CreateNUWNeg(V);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFNeg(Value V);
KMETHOD IRBuilder_createFNeg(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Value *ptr = self->CreateFNeg(V);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateNot(Value V);
KMETHOD IRBuilder_createNot(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Value *ptr = self->CreateNot(V);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## AllocaInst IRBuilder.CreateAlloca(Type Ty, Value ArraySize);
KMETHOD IRBuilder_createAlloca(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Type *Ty = konoha::object_cast<Type *>(sfp[1].p);
	Value *ArraySize = konoha::object_cast<Value *>(sfp[2].p);
	AllocaInst *ptr = self->CreateAlloca(Ty, ArraySize);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## LoadInst IRBuilder.CreateLoad(Value Ptr, boolean isVolatile);
KMETHOD IRBuilder_createLoad(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kbool_t isVolatile = sfp[2].bvalue;
	LoadInst *ptr = self->CreateLoad(Ptr, isVolatile);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//@Native LoadInst LoadInst.new(Value ptr);
//## LoadInst IRBuilder.CreateLoad(Value Ptr, boolean isVolatile);
KMETHOD LoadInst_new(CTX, ksfp_t *sfp _RIX)
{
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	LoadInst *ptr = new LoadInst(Ptr);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## StoreInst IRBuilder.CreateStore(Value Val, Value Ptr, boolean isVolatile);
KMETHOD IRBuilder_createStore(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Val = konoha::object_cast<Value *>(sfp[1].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[2].p);
	kbool_t isVolatile = sfp[3].bvalue;
	StoreInst *ptr = self->CreateStore(Val, Ptr, isVolatile);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

////## FenceInst IRBuilder.CreateFence(AtomicOrdering Ordering, SynchronizationScope SynchScope);
//KMETHOD IRBuilder_createFence(CTX, ksfp_t *sfp _RIX)
//{
//	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
//	AtomicOrdering *Ordering = konoha::object_cast<AtomicOrdering *>(sfp[1].p);
//	SynchronizationScope *SynchScope = konoha::object_cast<SynchronizationScope *>(sfp[2].p);
//	FenceInst *ptr = self->CreateFence(Ordering, SynchScope);
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}
//
////## AtomicCmpXchgInst IRBuilder.CreateAtomicCmpXchg(Value Ptr, Value Cmp, Value New, AtomicOrdering Ordering, SynchronizationScope SynchScope);
//KMETHOD IRBuilder_createAtomicCmpXchg(CTX, ksfp_t *sfp _RIX)
//{
//	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
//	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
//	Value *Cmp = konoha::object_cast<Value *>(sfp[2].p);
//	Value *New = konoha::object_cast<Value *>(sfp[3].p);
//	AtomicOrdering *Ordering = konoha::object_cast<AtomicOrdering *>(sfp[4].p);
//	SynchronizationScope *SynchScope = konoha::object_cast<SynchronizationScope *>(sfp[5].p);
//	AtomicCmpXchgInst *ptr = self->CreateAtomicCmpXchg(Ptr, Cmp, New, Ordering, SynchScope);
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

//## @Native AllocaInst AllocaInst.new(Type ty, Value arraySize);
KMETHOD AllocaInst_new(CTX, ksfp_t *sfp _RIX)
{
	Type *Ty = konoha::object_cast<Type *>(sfp[1].p);
	Value *ArraySize = konoha::object_cast<Value *>(sfp[2].p);
	AllocaInst *ptr = new AllocaInst(Ty, ArraySize);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Native StoreInst StoreInst.new(Value val, Value ptr);
KMETHOD StoreInst_new(CTX, ksfp_t *sfp _RIX)
{
	Value *Val = konoha::object_cast<Value *>(sfp[1].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[2].p);
	StoreInst *ptr = new StoreInst(Val, Ptr);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Native @Static GetElementPtrInst GetElementPtrInst.create(Value ptr, Array<Value> idxList);
KMETHOD GetElementPtrInst_create(CTX, ksfp_t *sfp _RIX)
{
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kArray *IdxList = sfp[2].a;
	std::vector<Value*> List;
	konoha::convert_array(List, IdxList);
	GetElementPtrInst *ptr = GetElementPtrInst::Create(Ptr, List);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Native @Static GetElementPtrInst GetElementPtrInst.CreateInBounds(Value ptr, Array<Value> idxList);
KMETHOD GetElementPtrInst_createInBounds(CTX, ksfp_t *sfp _RIX)
{
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kArray *IdxList = sfp[2].a;
	std::vector<Value*> List;
	konoha::convert_array(List, IdxList);
	GetElementPtrInst *ptr = GetElementPtrInst::CreateInBounds(Ptr, List);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateGEP(Value Ptr, ArrayRef< Value > IdxList);
KMETHOD IRBuilder_createGEP(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kArray *IdxList = sfp[2].a;
	std::vector<Value*> List;
	konoha::convert_array(List, IdxList);
	Value *ptr = self->CreateGEP(Ptr, List);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateInBoundsGEP(Value Ptr, ArrayRef< Value > IdxList);
KMETHOD IRBuilder_createInBoundsGEP(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kArray *IdxList = sfp[2].a;
	std::vector<Value*> List;
	konoha::convert_array(List, IdxList);
	Value *ptr = self->CreateInBoundsGEP(Ptr, List);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateGEP1(Value Ptr, Value Idx);
KMETHOD IRBuilder_createGEP1(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	Value *Idx = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateGEP(Ptr, Idx);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateInBoundsGEP1(Value Ptr, Value Idx);
KMETHOD IRBuilder_createInBoundsGEP1(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	Value *Idx = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateInBoundsGEP(Ptr, Idx);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateConstGEP1_32(Value Ptr, int Idx0);
KMETHOD IRBuilder_createConstGEP132(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx0 = Int_to(kint_t,sfp[2]);
	Value *ptr = self->CreateConstGEP1_32(Ptr, Idx0);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateConstInBoundsGEP1_32(Value Ptr, int Idx0);
KMETHOD IRBuilder_createConstInBoundsGEP132(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx0 = Int_to(kint_t,sfp[2]);
	Value *ptr = self->CreateConstInBoundsGEP1_32(Ptr, Idx0);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateConstGEP2_32(Value Ptr, int Idx0, int Idx1);
KMETHOD IRBuilder_createConstGEP232(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx0 = Int_to(kint_t,sfp[2]);
	kint_t Idx1 = Int_to(kint_t,sfp[3]);
	Value *ptr = self->CreateConstGEP2_32(Ptr, Idx0, Idx1);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateConstInBoundsGEP2_32(Value Ptr, int Idx0, int Idx1);
KMETHOD IRBuilder_createConstInBoundsGEP232(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx0 = Int_to(kint_t,sfp[2]);
	kint_t Idx1 = Int_to(kint_t,sfp[3]);
	Value *ptr = self->CreateConstInBoundsGEP2_32(Ptr, Idx0, Idx1);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateConstGEP1_64(Value Ptr, uint64_t Idx0);
KMETHOD IRBuilder_createConstGEP164(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx0 = sfp[2].ivalue;
	Value *ptr = self->CreateConstGEP1_64(Ptr, Idx0);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateConstInBoundsGEP1_64(Value Ptr, uint64_t Idx0);
KMETHOD IRBuilder_createConstInBoundsGEP164(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx0 = sfp[2].ivalue;
	Value *ptr = self->CreateConstInBoundsGEP1_64(Ptr, Idx0);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateConstGEP2_64(Value Ptr, uint64_t Idx0, uint64_t Idx1);
KMETHOD IRBuilder_createConstGEP264(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx0 = sfp[2].ivalue;
	kint_t Idx1 = sfp[3].ivalue;
	Value *ptr = self->CreateConstGEP2_64(Ptr, Idx0, Idx1);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateConstInBoundsGEP2_64(Value Ptr, uint64_t Idx0, uint64_t Idx1);
KMETHOD IRBuilder_createConstInBoundsGEP264(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx0 = sfp[2].ivalue;
	kint_t Idx1 = sfp[3].ivalue;
	Value *ptr = self->CreateConstInBoundsGEP2_64(Ptr, Idx0, Idx1);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateStructGEP(Value Ptr, int Idx);
KMETHOD IRBuilder_createStructGEP(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Ptr = konoha::object_cast<Value *>(sfp[1].p);
	kint_t Idx = Int_to(kint_t,sfp[2]);
	Value *ptr = self->CreateStructGEP(Ptr, Idx, "gep");
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateGlobalString(StringRef Str);
KMETHOD IRBuilder_createGlobalString(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	kString *Str = sfp[1].s;
	Value *ptr = self->CreateGlobalString(S_text(Str));
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateGlobalStringPtr(StringRef Str);
KMETHOD IRBuilder_createGlobalStringPtr(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	kString *Str = sfp[1].s;
	Value *ptr = self->CreateGlobalStringPtr(S_text(Str));
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateTrunc(Value V, Type DestTy);
KMETHOD IRBuilder_createTrunc(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateTrunc(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateZExt(Value V, Type DestTy);
KMETHOD IRBuilder_createZExt(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateZExt(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateSExt(Value V, Type DestTy);
KMETHOD IRBuilder_createSExt(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateSExt(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFPToUI(Value V, Type DestTy);
KMETHOD IRBuilder_createFPToUI(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateFPToUI(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFPToSI(Value V, Type DestTy);
KMETHOD IRBuilder_createFPToSI(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateFPToSI(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateUIToFP(Value V, Type DestTy);
KMETHOD IRBuilder_createUIToFP(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateUIToFP(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateSIToFP(Value V, Type DestTy);
KMETHOD IRBuilder_createSIToFP(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateSIToFP(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFPTrunc(Value V, Type DestTy);
KMETHOD IRBuilder_createFPTrunc(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateFPTrunc(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFPExt(Value V, Type DestTy);
KMETHOD IRBuilder_createFPExt(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateFPExt(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreatePtrToInt(Value V, Type DestTy);
KMETHOD IRBuilder_createPtrToInt(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreatePtrToInt(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateIntToPtr(Value V, Type DestTy);
KMETHOD IRBuilder_createIntToPtr(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateIntToPtr(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateBitCast(Value V, Type DestTy);
KMETHOD IRBuilder_createBitCast(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateBitCast(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateZExtOrBitCast(Value V, Type DestTy);
KMETHOD IRBuilder_createZExtOrBitCast(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateZExtOrBitCast(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateSExtOrBitCast(Value V, Type DestTy);
KMETHOD IRBuilder_createSExtOrBitCast(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateSExtOrBitCast(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateTruncOrBitCast(Value V, Type DestTy);
KMETHOD IRBuilder_createTruncOrBitCast(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateTruncOrBitCast(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreatePointerCast(Value V, Type DestTy);
KMETHOD IRBuilder_createPointerCast(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreatePointerCast(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateIntCast(Value V, Type DestTy, boolean isSigned);
KMETHOD IRBuilder_createIntCast(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	kbool_t isSigned = sfp[3].bvalue;
	Value *ptr = self->CreateIntCast(V, DestTy, isSigned);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFPCast(Value V, Type DestTy);
KMETHOD IRBuilder_createFPCast(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V = konoha::object_cast<Value *>(sfp[1].p);
	Type *DestTy = konoha::object_cast<Type *>(sfp[2].p);
	Value *ptr = self->CreateFPCast(V, DestTy);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpEQ(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpEQ(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpEQ(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpNE(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpNE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpNE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpUGT(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpUGT(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpUGT(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpUGE(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpUGE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpUGE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpULT(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpULT(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpULT(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpULE(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpULE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpULE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpSGT(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpSGT(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpSGT(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpSGE(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpSGE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpSGE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpSLT(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpSLT(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpSLT(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateICmpSLE(Value LHS, Value RHS);
KMETHOD IRBuilder_createICmpSLE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateICmpSLE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpOEQ(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpOEQ(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpOEQ(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpOGT(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpOGT(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpOGT(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpOGE(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpOGE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpOGE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpOLT(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpOLT(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpOLT(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpOLE(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpOLE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpOLE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpONE(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpONE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpONE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpORD(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpORD(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpORD(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpUNO(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpUNO(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpUNO(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpUEQ(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpUEQ(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpUEQ(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpUGT(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpUGT(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpUGT(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpUGE(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpUGE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpUGE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpULT(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpULT(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpULT(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpULE(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpULE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpULE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateFCmpUNE(Value LHS, Value RHS);
KMETHOD IRBuilder_createFCmpUNE(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateFCmpUNE(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## PHINode IRBuilder.CreatePHI(Type Ty, int numReservedValues);
KMETHOD IRBuilder_createPHI(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Type *Ty = konoha::object_cast<Type *>(sfp[1].p);
	kint_t num = sfp[2].ivalue;
	PHINode *ptr = self->CreatePHI(Ty, num);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## void IRBuilder.addIncoming(Type Ty, BasicBlock bb);
KMETHOD PHINode_addIncoming(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	PHINode *self = konoha::object_cast<PHINode *>(sfp[0].p);
	Value *v = konoha::object_cast<Value *>(sfp[1].p);
	BasicBlock *bb = konoha::object_cast<BasicBlock *>(sfp[2].p);
	self->addIncoming(v, bb);
	RETURNvoid_();
}

//## CallInst IRBuilder.CreateCall1(Value Callee, Value Arg);
KMETHOD IRBuilder_createCall1(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	Value *Arg = konoha::object_cast<Value *>(sfp[2].p);
	CallInst *ptr = self->CreateCall(Callee, Arg);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## CallInst IRBuilder.CreateCall2(Value Callee, Value Arg1, Value Arg2);
KMETHOD IRBuilder_createCall2(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	Value *Arg1 = konoha::object_cast<Value *>(sfp[2].p);
	Value *Arg2 = konoha::object_cast<Value *>(sfp[3].p);
	CallInst *ptr = self->CreateCall2(Callee, Arg1, Arg2);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## CallInst IRBuilder.CreateCall3(Value Callee, Value Arg1, Value Arg2, Value Arg3);
KMETHOD IRBuilder_createCall3(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	Value *Arg1 = konoha::object_cast<Value *>(sfp[2].p);
	Value *Arg2 = konoha::object_cast<Value *>(sfp[3].p);
	Value *Arg3 = konoha::object_cast<Value *>(sfp[4].p);
	CallInst *ptr = self->CreateCall3(Callee, Arg1, Arg2, Arg3);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## CallInst IRBuilder.CreateCall4(Value Callee, Value Arg1, Value Arg2, Value Arg3, Value Arg4);
KMETHOD IRBuilder_createCall4(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	Value *Arg1 = konoha::object_cast<Value *>(sfp[2].p);
	Value *Arg2 = konoha::object_cast<Value *>(sfp[3].p);
	Value *Arg3 = konoha::object_cast<Value *>(sfp[4].p);
	Value *Arg4 = konoha::object_cast<Value *>(sfp[5].p);
	CallInst *ptr = self->CreateCall4(Callee, Arg1, Arg2, Arg3, Arg4);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## CallInst IRBuilder.CreateCall5(Value Callee, Value Arg1, Value Arg2, Value Arg3, Value Arg4, Value Arg5);
KMETHOD IRBuilder_createCall5(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	Value *Arg1 = konoha::object_cast<Value *>(sfp[2].p);
	Value *Arg2 = konoha::object_cast<Value *>(sfp[3].p);
	Value *Arg3 = konoha::object_cast<Value *>(sfp[4].p);
	Value *Arg4 = konoha::object_cast<Value *>(sfp[5].p);
	Value *Arg5 = konoha::object_cast<Value *>(sfp[6].p);
	CallInst *ptr = self->CreateCall5(Callee, Arg1, Arg2, Arg3, Arg4, Arg5);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## CallInst IRBuilder.CreateCall(Value Callee, ArrayRef< Value > Args);
KMETHOD IRBuilder_createCall(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Callee = konoha::object_cast<Value *>(sfp[1].p);
	kArray *Args = sfp[2].a;
	std::vector<Value*> List;
	konoha::convert_array(List, Args);
	CallInst *ptr = self->CreateCall(Callee, List);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateSelect(Value C, Value True, Value False);
KMETHOD IRBuilder_createSelect(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *C = konoha::object_cast<Value *>(sfp[1].p);
	Value *True = konoha::object_cast<Value *>(sfp[2].p);
	Value *False = konoha::object_cast<Value *>(sfp[3].p);
	Value *ptr = self->CreateSelect(C, True, False);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## VAArgInst IRBuilder.CreateVAArg(Value List, Type Ty);
KMETHOD IRBuilder_createVAArg(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *List = konoha::object_cast<Value *>(sfp[1].p);
	Type *Ty = konoha::object_cast<Type *>(sfp[2].p);
	VAArgInst *ptr = self->CreateVAArg(List, Ty);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateExtractElement(Value Vec, Value Idx);
KMETHOD IRBuilder_createExtractElement(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Vec = konoha::object_cast<Value *>(sfp[1].p);
	Value *Idx = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreateExtractElement(Vec, Idx);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateInsertElement(Value Vec, Value NewElt, Value Idx);
KMETHOD IRBuilder_createInsertElement(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Vec = konoha::object_cast<Value *>(sfp[1].p);
	Value *NewElt = konoha::object_cast<Value *>(sfp[2].p);
	Value *Idx = konoha::object_cast<Value *>(sfp[3].p);
	Value *ptr = self->CreateInsertElement(Vec, NewElt, Idx);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateShuffleVector(Value V1, Value V2, Value Mask);
KMETHOD IRBuilder_createShuffleVector(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *V1 = konoha::object_cast<Value *>(sfp[1].p);
	Value *V2 = konoha::object_cast<Value *>(sfp[2].p);
	Value *Mask = konoha::object_cast<Value *>(sfp[3].p);
	Value *ptr = self->CreateShuffleVector(V1, V2, Mask);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

////## Value IRBuilder.CreateExtractValue(Value Agg, Array<int> Idxs);
//KMETHOD IRBuilder_createExtractValue(CTX, ksfp_t *sfp _RIX)
//{
//	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
//	Value *Agg = konoha::object_cast<Value *>(sfp[1].p);
//	kArray *Idxs = sfp[2].a;
//	std::vector<int> List;
//	konoha::convert_array_int(List, Idxs);
//	Value *ptr = self->CreateExtractValue(Agg, List.begin(), List.end());
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

////## Value IRBuilder.CreateInsertValue(Value Agg, Value Val, Array<int> Idxs);
//KMETHOD IRBuilder_createInsertValue(CTX, ksfp_t *sfp _RIX)
//{
//	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
//	Value *Agg = konoha::object_cast<Value *>(sfp[1].p);
//	Value *Val = konoha::object_cast<Value *>(sfp[2].p);
//	kArray *Idxs = sfp[2].a;
//	std::vector<int> List;
//	konoha::convert_array_int(List, Idxs);
//	Value *ptr = self->CreateInsertValue(Agg, Val, List.begin(), List.end());
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

//## Value IRBuilder.CreateIsNull(Value Arg);
KMETHOD IRBuilder_createIsNull(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Arg = konoha::object_cast<Value *>(sfp[1].p);
	Value *ptr = self->CreateIsNull(Arg);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreateIsNotNull(Value Arg);
KMETHOD IRBuilder_createIsNotNull(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *Arg = konoha::object_cast<Value *>(sfp[1].p);
	Value *ptr = self->CreateIsNotNull(Arg);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Value IRBuilder.CreatePtrDiff(Value LHS, Value RHS);
KMETHOD IRBuilder_createPtrDiff(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	Value *LHS = konoha::object_cast<Value *>(sfp[1].p);
	Value *RHS = konoha::object_cast<Value *>(sfp[2].p);
	Value *ptr = self->CreatePtrDiff(LHS, RHS);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## void IRBuilder.SetInsertPoint(BasicBlock BB);
KMETHOD IRBuilder_setInsertPoint(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	BasicBlock * BB = konoha::object_cast<BasicBlock *>(sfp[1].p);
	self->SetInsertPoint(BB);
	RETURNvoid_();
}

//## BasicBlock IRBuilder.GetInsertBlock();
KMETHOD IRBuilder_getInsertBlock(CTX, ksfp_t *sfp _RIX)
{
	IRBuilder<> *self = konoha::object_cast<IRBuilder<> *>(sfp[0].p);
	BasicBlock *BB = self->GetInsertBlock();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(BB) K_RIXPARAM);
	RETURN_(p);
}

//## Function BasicBlock.getParent();
KMETHOD BasicBlock_getParent(CTX, ksfp_t *sfp _RIX)
{
	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
	Function *ptr = self->getParent();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Instruction BasicBlock.getTerminator();
KMETHOD BasicBlock_getTerminator(CTX, ksfp_t *sfp _RIX)
{
	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
	TerminatorInst *ptr = self->getTerminator();
	if (ptr) {
		kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
		RETURN_(p);
	} else {
		RETURN_(K_NULL);
	}
}


////## iterator BasicBlock.begin();
//KMETHOD BasicBlock_begin(CTX, ksfp_t *sfp _RIX)
//{
//	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
//	*ptr = self->Create();
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(K_NULL);
//}
//
////## iterator BasicBlock.end();
//KMETHOD BasicBlock_end(CTX, ksfp_t *sfp _RIX)
//{
//	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
//	*ptr = self->Create();
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(K_NULL);
//}

//## Instruction BasicBlock.getLastInst();
KMETHOD BasicBlock_getLastInst(CTX, ksfp_t *sfp _RIX)
{
	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
	BasicBlock::iterator I = self->end();
	Instruction *ptr;
	if (self->size() > 0)
		--I;
	ptr = I;
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Instruction BasicBlock.insertBefore(Instruction before, Instruction inst);
KMETHOD BasicBlock_insertBefore(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
	Instruction *inst0 = konoha::object_cast<Instruction *>(sfp[1].p);
	Instruction *inst1 = konoha::object_cast<Instruction *>(sfp[2].p);
	self->getInstList().insert(inst0, inst1);
	RETURNvoid_();
}

//## int BasicBlock.size();
KMETHOD BasicBlock_size(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
	int ret = self->size();
	RETURNi_(ret);
}

//## boolean BasicBlock.empty();
KMETHOD BasicBlock_empty(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
	bool isEmpty = self->empty();
	RETURNb_(isEmpty);
}

//## Argument Argument.new(Type ty, int scid);
KMETHOD Argument_new(CTX, ksfp_t *sfp _RIX)
{
	Type *ty = konoha::object_cast<Type *>(sfp[1].p);
	Value *v = new Argument(ty, "", 0);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(v) K_RIXPARAM);
	RETURN_(p);
}

//static void str_replace (std::string& str, const std::string& from, const std::string& to) {
//	std::string::size_type pos = 0;
//	while ((pos = str.find(from, pos)) != std::string::npos) {
//		str.replace( pos, from.size(), to );
//		pos++;
//	}
//}
//
//## Module Module.new(String name);
KMETHOD Module_new(CTX, ksfp_t *sfp _RIX)
{
	kString *name = sfp[1].s;
	LLVMContext &Context = getGlobalContext();
	Module *M = new Module(S_text(name), Context);
#if 0
	Triple T(sys::getDefaultTargetTriple());
	const Target *Target = 0;
	std::string Arch = T.getArchName();
	for (TargetRegistry::iterator it = TargetRegistry::begin(),
			ie = TargetRegistry::end(); it != ie; ++it) {
		std::string tmp(it->getName());
		str_replace(tmp, "-", "_");
		if (Arch == tmp) {
			Target = &*it;
			break;
		}
	}
	assert(Target != 0);
	std::string FeaturesStr;
	TargetOptions Options;
	TargetMachine *TM = Target->createTargetMachine(T.getTriple(), Target->getName(), FeaturesStr, Options);
	M->setTargetTriple(T.getTriple());
	M->setDataLayout(TM->getTargetData()->getStringRepresentation());
#endif
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(M) K_RIXPARAM);
	RETURN_(p);
}

//## void Module.dump();
KMETHOD Module_dump(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	Module *self = konoha::object_cast<Module *>(sfp[0].p);
	(*self).dump();
	RETURNvoid_();
}

//## Type Module.getTypeByName(String name);
KMETHOD Module_getTypeByName(CTX, ksfp_t *sfp _RIX)
{
	Module *self = konoha::object_cast<Module *>(sfp[0].p);
	kString *name = sfp[1].s;
	Type *ptr = self->getTypeByName(S_text(name));
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## void BasicBlock.dump();
KMETHOD BasicBlock_dump(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	BasicBlock *self = konoha::object_cast<BasicBlock *>(sfp[0].p);
	(*self).dump();
	RETURNvoid_();
}

//## Function Module.getOrInsertFunction(String name, FunctionType fnTy);
KMETHOD Module_getOrInsertFunction(CTX, ksfp_t *sfp _RIX)
{
	Module *self = konoha::object_cast<Module *>(sfp[0].p);
	kString *name = sfp[1].s;
	FunctionType *fnTy = konoha::object_cast<FunctionType *>(sfp[2].p);
	Function *ptr = cast<Function>(self->getOrInsertFunction(S_text(name), fnTy));
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static @Native Function Function.create(String name, FunctionType fnTy, Module m, Linkage linkage);
KMETHOD Function_create(CTX, ksfp_t *sfp _RIX)
{
	kString *name = sfp[1].s;
	FunctionType *fnTy = konoha::object_cast<FunctionType *>(sfp[2].p);
	Module *m = konoha::object_cast<Module *>(sfp[3].p);
	kint_t v = sfp[4].ivalue;
	GlobalValue::LinkageTypes linkage = (GlobalValue::LinkageTypes) v;
	Function *ptr = Function::Create(fnTy, linkage, S_text(name), m);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Native void Function.dump();
KMETHOD Function_dump(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	Function *func = konoha::object_cast<Function *>(sfp[0].p);
	func->dump();
	RETURNvoid_();
}

//## @Native void Function.addFnAttr(Int attributes);
KMETHOD Function_addFnAttr(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	Function *F = konoha::object_cast<Function *>(sfp[0].p);
	Attributes N = (Attributes) sfp[1].ivalue;
	F->addFnAttr(N);
	RETURNvoid_();
}

//## ExecutionEngine Module.createExecutionEngine(int optLevel);
KMETHOD Module_createExecutionEngine(CTX, ksfp_t *sfp _RIX)
{
	Module *self = konoha::object_cast<Module *>(sfp[0].p);
	CodeGenOpt::Level OptLevel = (CodeGenOpt::Level) sfp[1].ivalue;
	ExecutionEngine *ptr = EngineBuilder(self).setEngineKind(EngineKind::JIT).setOptLevel(OptLevel).create();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

static int BasicBlock_compareTo(kRawPtr *p1, kRawPtr *p2)
{
	return (p1->rawptr != p2->rawptr);
}

//void defBasicBlock(CTX _UNUSED_, kcid_t cid _UNUSED_, kclassdef_t *cdef)
//{
//	cdef->name = "llvm::BasicBlock";
//	cdef->compareTo = BasicBlock_compareTo;
//}

//## @Static BasicBlock BasicBlock.create(Function parent, String name);
KMETHOD BasicBlock_create(CTX, ksfp_t *sfp _RIX)
{
	Function * parent = konoha::object_cast<Function *>(sfp[1].p);
	kString *name = sfp[2].s;
	const char *bbname = "";
	if (IS_NOTNULL(name)) {
		bbname = S_text(name);
	}
	BasicBlock *ptr = BasicBlock::Create(getGlobalContext(), bbname, parent);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static FunctionType.get(Type retTy, Array<Type> args, boolean b);
KMETHOD FunctionType_get(CTX, ksfp_t *sfp _RIX)
{
	Type *retTy = konoha::object_cast<Type *>(sfp[1].p);
	kArray * args = sfp[2].a;
	kbool_t b = sfp[3].bvalue;
	std::vector<Type*> List;
	konoha::convert_array(List, args);
	FunctionType *ptr = FunctionType::get(retTy, List, b);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Native Value ConstantInt.get(Type type, int v);
KMETHOD ConstantInt_get(CTX, ksfp_t *sfp _RIX)
{
	Type *type  = konoha::object_cast<Type *>(sfp[1].p);
	kint_t v = sfp[2].ivalue;
	Value *ptr = ConstantInt::get(type, v);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Native Value ConstantFP.get(Type type, float v);
KMETHOD ConstantFP_get(CTX, ksfp_t *sfp _RIX)
{
	Type *type  = konoha::object_cast<Type *>(sfp[1].p);
	kfloat_t v = sfp[2].fvalue;
	Value *ptr = ConstantFP::get(type, v);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static @Native Value ConstantPointerNull.get(Type type);
KMETHOD ConstantPointerNull_get(CTX, ksfp_t *sfp _RIX)
{
	PointerType *type  = konoha::object_cast<PointerType *>(sfp[1].p);
	Value *ptr = ConstantPointerNull::get(type);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static @Native Value ConstantStruct.get(Type type, Array<Constant> V);
KMETHOD ConstantStruct_get(CTX, ksfp_t *sfp _RIX)
{
	StructType *type  = konoha::object_cast<StructType *>(sfp[1].p);
	kArray *args = sfp[2].a;
	std::vector<Constant*> List;
	konoha::convert_array(List, args);
	Value *ptr = ConstantStruct::get(type, List);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static StructType.get(Array<Type> args, boolean isPacked);
KMETHOD StructType_get(CTX, ksfp_t *sfp _RIX)
{
	kArray *args = sfp[1].a;
	kbool_t isPacked = sfp[2].bvalue;
	std::vector<Type*> List;
	konoha::convert_array(List, args);
	StructType *ptr = StructType::get(getGlobalContext(), List, isPacked);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Static @Native StructType.create(Array<Type> args, String name, boolean isPacked);
KMETHOD StructType_create(CTX, ksfp_t *sfp _RIX)
{
	kArray *args = sfp[1].a;
	kString *name = sfp[2].s;
	kbool_t isPacked = sfp[3].bvalue;
	StructType *ptr;
	if (IS_NULL(args)) {
		ptr = StructType::create(getGlobalContext(), S_text(name));
	} else if (kArray_size(args) == 0) {
		std::vector<Type*> List;
		ptr = StructType::create(getGlobalContext(), S_text(name));
		ptr->setBody(List, isPacked);
	} else {
		std::vector<Type*> List;
		konoha::convert_array(List, args);
		ptr = StructType::create(List, S_text(name), isPacked);
	}
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Native @Static ArrayType ArrayType.get(Type t, int elemSize);
KMETHOD ArrayType_get(CTX, ksfp_t *sfp _RIX)
{
	Type *Ty = konoha::object_cast<Type *>(sfp[1].p);
	kint_t N = sfp[2].bvalue;
	ArrayType *ptr = ArrayType::get(Ty, N);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## @Native void StructType.setBody(Array<Type> args, boolean isPacked);
KMETHOD StructType_setBody(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	StructType *type  = konoha::object_cast<StructType *>(sfp[0].p);
	kArray *args = sfp[1].a;
	kbool_t isPacked = sfp[2].bvalue;
	std::vector<Type*> List;
	konoha::convert_array(List, args);
	type->setBody(List, isPacked);
	RETURNvoid_();
}

//## @Native boolean StructType.isOpaque();
KMETHOD StructType_isOpaque(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	StructType *type  = konoha::object_cast<StructType *>(sfp[0].p);
	bool ret = type->isOpaque();
	RETURNb_(ret);
}

//## NativeFunction ExecutionEngine.getPointerToFunction(Function func);
KMETHOD ExecutionEngine_getPointerToFunction(CTX, ksfp_t *sfp _RIX)
{
	ExecutionEngine *ee = konoha::object_cast<ExecutionEngine *>(sfp[0].p);
	Function *func = konoha::object_cast<Function *>(sfp[1].p);
	void *ptr = ee->getPointerToFunction(func);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}
//## @Native void ExecutionEngine.addGlobalMapping(GlobalVariable g, int addr);
KMETHOD ExecutionEngine_addGlobalMapping(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	ExecutionEngine *ee = konoha::object_cast<ExecutionEngine *>(sfp[0].p);
	GlobalVariable *g   = konoha::object_cast<GlobalVariable *>(sfp[1].p);
	long addr = sfp[2].ivalue;
	ee->addGlobalMapping(g, (void*)addr);
	RETURNvoid_();
}
//## @Native GlobalVariable GlobalVariable.new(Module m, Type ty, Constant c, Linkage linkage, String name);
KMETHOD GlobalVariable_new(CTX, ksfp_t *sfp _RIX)
{
	Module *m     = konoha::object_cast<Module *>(sfp[1].p);
	Type *ty      = konoha::object_cast<Type *>(sfp[2].p);
	Constant *c   = konoha::object_cast<Constant *>(sfp[3].p);
	GlobalValue::LinkageTypes linkage = (GlobalValue::LinkageTypes) sfp[4].ivalue;
	kString *name = sfp[5].s;
	bool isConstant = (c) ? true : false;
	GlobalVariable *ptr = new GlobalVariable(*m, ty, isConstant, linkage, c, S_text(name));
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

static void PassManagerBuilder_ptr_init(CTX _UNUSED_, kRawPtr *po, void *conf)
{
	po->rawptr = conf;
}

static void PassManagerBuilder_ptr_free(CTX _UNUSED_, kRawPtr *po)
{
	PassManagerBuilder *o = static_cast<PassManagerBuilder *>(po->rawptr);
	delete o;
}

KMETHOD PassManagerBuilder_new(CTX, ksfp_t *sfp _RIX)
{
	PassManagerBuilder *self = new PassManagerBuilder();
	self->OptLevel = 3;
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(self) K_RIXPARAM);
	RETURN_(p);
}
KMETHOD PassManagerBuilder_populateModulePassManager(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	PassManagerBuilder *self = konoha::object_cast<PassManagerBuilder *>(sfp[0].p);
	PassManager *manager = konoha::object_cast<PassManager *>(sfp[1].p);
	self->populateModulePassManager(*manager);
	RETURNvoid_();
}

static void PassManager_ptr_init(CTX _UNUSED_, kRawPtr *po, void *conf)
{
	po->rawptr = conf;
}

static void PassManager_ptr_free(CTX _UNUSED_, kRawPtr *po)
{
	PassManager *o = static_cast<PassManager *>(po->rawptr);
	delete o;
}

//## PassManager PassManager.new()
KMETHOD PassManager_new(CTX, ksfp_t *sfp _RIX)
{
	PassManager *self = new PassManager();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(self) K_RIXPARAM);
	RETURN_(p);
}

//## void PassManager.run(Function func)
KMETHOD PassManager_run(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	PassManager *self = konoha::object_cast<PassManager *>(sfp[0].p);
	Module *m = konoha::object_cast<Module *>(sfp[1].p);
	self->run(*m);
	RETURNvoid_();
}
//## void PassManager.add(Pass p)
KMETHOD PassManager_addPass(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	PassManager *self = konoha::object_cast<PassManager *>(sfp[0].p);
	Pass *pass = konoha::object_cast<Pass *>(sfp[1].p);
	self->add(pass);
	RETURNvoid_();
}
//## void PassManager.add(Pass p)
KMETHOD PassManager_addImmutablePass(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	PassManager *self = konoha::object_cast<PassManager *>(sfp[0].p);
	ImmutablePass *pass = konoha::object_cast<ImmutablePass *>(sfp[1].p);
	self->add(pass);
	RETURNvoid_();
}
//## void PassManager.addFunctionPass(Pass p)
KMETHOD PassManager_addFunctionPass(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	PassManager *self = konoha::object_cast<PassManager *>(sfp[0].p);
	FunctionPass *pass = konoha::object_cast<FunctionPass *>(sfp[1].p);
	self->add(pass);
	RETURNvoid_();
}
//## void PassManager.addModulePass(Pass p)
KMETHOD PassManager_addModulePass(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	PassManager *self = konoha::object_cast<PassManager *>(sfp[0].p);
	ModulePass *pass = konoha::object_cast<ModulePass *>(sfp[1].p);
	self->add(pass);
	RETURNvoid_();
}

static void FunctionPassManager_ptr_init(CTX _UNUSED_, kRawPtr *po, void *conf)
{
	po->rawptr = conf;
}

static void FunctionPassManager_ptr_free(CTX _UNUSED_, kRawPtr *po)
{
	FunctionPassManager *o = static_cast<FunctionPassManager *>(po->rawptr);
	delete o;
}

//## FunctionPassManager FunctionPassManager.new(Module m)
KMETHOD FunctionPassManager_new(CTX, ksfp_t *sfp _RIX)
{
	Module *m = konoha::object_cast<Module *>(sfp[1].p);
	FunctionPassManager *self = new FunctionPassManager(m);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(self) K_RIXPARAM);
	RETURN_(p);
}
//## void FuncitonPassManager.add(Pass p)
KMETHOD FunctionPassManager_add(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	FunctionPassManager *self = konoha::object_cast<FunctionPassManager *>(sfp[0].p);
	Pass *pass = konoha::object_cast<Pass *>(sfp[1].p);
	self->add(pass);
	RETURNvoid_();
}
//## void FunctionPassManager.doInitialization()
KMETHOD FunctionPassManager_doInitialization(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	FunctionPassManager *self = konoha::object_cast<FunctionPassManager *>(sfp[0].p);
	self->doInitialization();
	RETURNvoid_();
}

//## void FunctionPassManager.run(Function func)
KMETHOD FunctionPassManager_run(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	FunctionPassManager *self = konoha::object_cast<FunctionPassManager *>(sfp[0].p);
	Function *func = konoha::object_cast<Function *>(sfp[1].p);
	self->run(*func);
	RETURNvoid_();
}

//## TargetData ExecutionEngine.getTargetData();
KMETHOD ExecutionEngine_getTargetData(CTX, ksfp_t *sfp _RIX)
{
	ExecutionEngine *ee = konoha::object_cast<ExecutionEngine *>(sfp[0].p);
	TargetData *ptr = new TargetData(*(ee->getTargetData()));
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## void Method.setFunction(NativeFunction func);
KMETHOD Method_setFunction(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = (kMethod*) sfp[0].o;
	kRawPtr *po = sfp[1].p;
	union anyptr { void *p; knh_Fmethod f;} ptr = {po->rawptr};
	kMethod_setFunc(mtd, ptr.f);
	RETURNvoid_();
}

//## @Native Array<Value> Function.getArguments();
KMETHOD Function_getArguments(CTX, ksfp_t *sfp _RIX)
{
	ktype_t rtype = sfp[K_MTDIDX].mtdNC->pa->rtype;
	kcid_t cid = CT_(rtype)->p1;
	Function *func = konoha::object_cast<Function *>(sfp[0].p);
	kArray *a = (kArray*) new_kObject(CT_(rtype), 0);
	for (Function::arg_iterator I = func->arg_begin(), E = func->arg_end();
			I != E; ++I) {
		Value *v = I;
		kObject *o = new_kObject(CT_(cid)/*"Value"*/, WRAP(v));
		kArray_add(a, o);
	}
	RETURN_(a);
}
//## void Value.replaceAllUsesWith(Value v);
KMETHOD Value_replaceAllUsesWith(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	Value *self = konoha::object_cast<Value *>(sfp[0].p);
	Value *v = konoha::object_cast<Value *>(sfp[1].p);
	self->replaceAllUsesWith(v);
	RETURNvoid_();
}
//## Value Value.setName(String name);
KMETHOD Value_setName(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	Value *self = konoha::object_cast<Value *>(sfp[0].p);
	kString *name = sfp[1].s;
	self->setName(S_text(name));
	RETURNvoid_();
}
//## void LoadInst.setAlignment(int align);
KMETHOD LoadInst_setAlignment(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	LoadInst *self = konoha::object_cast<LoadInst *>(sfp[0].p);
	int align = sfp[1].ivalue;
	self->setAlignment(align);
	RETURNvoid_();
}
//## void StoreInst.setAlignment(int align);
KMETHOD StoreInst_setAlignment(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	StoreInst *self = konoha::object_cast<StoreInst *>(sfp[0].p);
	int align = sfp[1].ivalue;
	self->setAlignment(align);
	RETURNvoid_();
}
//## Type Value.getType();
KMETHOD Value_getType(CTX, ksfp_t *sfp _RIX)
{
	Value *self = konoha::object_cast<Value *>(sfp[0].p);
	const Type *ptr = self->getType();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## void Value.dump();
KMETHOD Value_dump(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	Value *self = konoha::object_cast<Value *>(sfp[0].p);
	self->dump();
	RETURNvoid_();
}

//## @Native void Type.dump();
KMETHOD Type_dump(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	Type *type = konoha::object_cast<Type *>(sfp[0].p);
	type->dump();
	RETURNvoid_();
}

//## @Static boolean DynamicLibrary.loadLibraryPermanently(String libname);
KMETHOD DynamicLibrary_loadLibraryPermanently(CTX, ksfp_t *sfp _RIX)
{
	const char *libname = S_text(sfp[1].s);
	std::string ErrMsg;
	kbool_t ret = sys::DynamicLibrary::LoadLibraryPermanently(libname, &ErrMsg);
	if (ret == 0) {
		//TODO
		//KNH_NTRACE2(_ctx, "LoadLibraryPermanently", K_FAILED, KNH_LDATA(LOG_s("libname", libname), LOG_msg(ErrMsg.c_str())));
	}
	RETURNb_(ret);
}

//## @Static Int DynamicLibrary.searchForAddressOfSymbol(String fname);
KMETHOD DynamicLibrary_searchForAddressOfSymbol(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	const char *fname = S_text(sfp[1].s);
	kint_t ret = 0;
	void *symAddr = NULL;
	if (!(symAddr = sys::DynamicLibrary::SearchForAddressOfSymbol(fname))) {
		ret = reinterpret_cast<kint_t>(symAddr);
	}
	RETURNi_(ret);
}

//## FunctionPass LLVM.createDomPrinterPass();
KMETHOD LLVM_createDomPrinterPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createDomPrinterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createDomOnlyPrinterPass();
KMETHOD LLVM_createDomOnlyPrinterPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createDomOnlyPrinterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createDomViewerPass();
KMETHOD LLVM_createDomViewerPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createDomViewerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createDomOnlyViewerPass();
KMETHOD LLVM_createDomOnlyViewerPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createDomOnlyViewerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createPostDomPrinterPass();
KMETHOD LLVM_createPostDomPrinterPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createPostDomPrinterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createPostDomOnlyPrinterPass();
KMETHOD LLVM_createPostDomOnlyPrinterPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createPostDomOnlyPrinterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createPostDomViewerPass();
KMETHOD LLVM_createPostDomViewerPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createPostDomViewerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createPostDomOnlyViewerPass();
KMETHOD LLVM_createPostDomOnlyViewerPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createPostDomOnlyViewerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createGlobalsModRefPass();
KMETHOD LLVM_createGlobalsModRefPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createGlobalsModRefPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createAliasDebugger();
KMETHOD LLVM_createAliasDebugger(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createAliasDebugger();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createAliasAnalysisCounterPass();
KMETHOD LLVM_createAliasAnalysisCounterPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createAliasAnalysisCounterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createAAEvalPass();
KMETHOD LLVM_createAAEvalPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createAAEvalPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createLibCallAliasAnalysisPass(LibCallInfo lci);
KMETHOD LLVM_createLibCallAliasAnalysisPass(CTX, ksfp_t *sfp _RIX)
{
	LibCallInfo *lci = konoha::object_cast<LibCallInfo *>(sfp[0].p);
	FunctionPass *ptr = createLibCallAliasAnalysisPass(lci);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createScalarEvolutionAliasAnalysisPass();
KMETHOD LLVM_createScalarEvolutionAliasAnalysisPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createScalarEvolutionAliasAnalysisPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createProfileLoaderPass();
KMETHOD LLVM_createProfileLoaderPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createProfileLoaderPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createProfileEstimatorPass();
KMETHOD LLVM_createProfileEstimatorPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createProfileEstimatorPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createProfileVerifierPass();
KMETHOD LLVM_createProfileVerifierPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createProfileVerifierPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createPathProfileLoaderPass();
KMETHOD LLVM_createPathProfileLoaderPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createPathProfileLoaderPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createPathProfileVerifierPass();
KMETHOD LLVM_createPathProfileVerifierPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createPathProfileVerifierPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createLazyValueInfoPass();
KMETHOD LLVM_createLazyValueInfoPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createLazyValueInfoPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## LoopPass LLVM.createLoopDependenceAnalysisPass();
KMETHOD LLVM_createLoopDependenceAnalysisPass(CTX, ksfp_t *sfp _RIX)
{
	LoopPass *ptr = createLoopDependenceAnalysisPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createInstCountPass();
KMETHOD LLVM_createInstCountPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createInstCountPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createDbgInfoPrinterPass();
KMETHOD LLVM_createDbgInfoPrinterPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createDbgInfoPrinterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createRegionInfoPass();
KMETHOD LLVM_createRegionInfoPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createRegionInfoPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createModuleDebugInfoPrinterPass();
KMETHOD LLVM_createModuleDebugInfoPrinterPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createModuleDebugInfoPrinterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createMemDepPrinter();
KMETHOD LLVM_createMemDepPrinter(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createMemDepPrinter();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createPostDomTree();
KMETHOD LLVM_createPostDomTree(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createPostDomTree();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createRegionViewerPass();
KMETHOD LLVM_createRegionViewerPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createRegionViewerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createRegionOnlyViewerPass();
KMETHOD LLVM_createRegionOnlyViewerPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createRegionOnlyViewerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createRegionPrinterPass();
KMETHOD LLVM_createRegionPrinterPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createRegionPrinterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createRegionOnlyPrinterPass();
KMETHOD LLVM_createRegionOnlyPrinterPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createRegionOnlyPrinterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createLintPass();
KMETHOD LLVM_createLintPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createLintPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

////## ModulePass LLVM.createPrintModulePass(raw_ostream *OS);
//KMETHOD LLVM_createPrintModulePass(CTX, ksfp_t *sfp _RIX)
//{
//	raw_ostream **OS = konoha::object_cast<raw_ostream *>(sfp[0].p);
//	ModulePass *ptr = createPrintModulePass(*OS);
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}
//
////## FunctionPass LLVM.createPrintFunctionPass(String banner, OutputStream os, boolean deleteStream);
//KMETHOD LLVM_createPrintFunctionPass(CTX, ksfp_t *sfp _RIX)
//{
//	String *banner = konoha::object_cast<String *>(sfp[0].p);
//	OutputStream *os = konoha::object_cast<OutputStream *>(sfp[1].p);
//	bool deleteStream = sfp[2].bvalue;
//	FunctionPass *ptr = createPrintFunctionPass(banner,os,deleteStream);
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

////## ModulePass LLVM.createEdgeProfilerPass();
//KMETHOD LLVM_createEdgeProfilerPass(CTX, ksfp_t *sfp _RIX)
//{
//	ModulePass *ptr = createEdgeProfilerPass();
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

////## ModulePass LLVM.createOptimalEdgeProfilerPass();
//KMETHOD LLVM_createOptimalEdgeProfilerPass(CTX, ksfp_t *sfp _RIX)
//{
//	ModulePass *ptr = createOptimalEdgeProfilerPass();
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

////## ModulePass LLVM.createPathProfilerPass();
//KMETHOD LLVM_createPathProfilerPass(CTX, ksfp_t *sfp _RIX)
//{
//	ModulePass *ptr = createPathProfilerPass();
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

////## ModulePass LLVM.createGCOVProfilerPass(boolean emitNotes, boolean emitData, boolean use402Format);
//KMETHOD LLVM_createGCOVProfilerPass(CTX, ksfp_t *sfp _RIX)
//{
//	bool emitNotes = sfp[0].bvalue;
//	bool emitData = sfp[1].bvalue;
//	bool use402Format = sfp[2].bvalue;
//	ModulePass *ptr = createGCOVProfilerPass(emitNotes,emitData,use402Format);
//	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
//	RETURN_(p);
//}

//## ModulePass LLVM.createStripSymbolsPass(bool onlyDebugInfo);
KMETHOD LLVM_createStripSymbolsPass(CTX, ksfp_t *sfp _RIX)
{
	bool onlyDebugInfo = sfp[0].bvalue;
	ModulePass *ptr = createStripSymbolsPass(onlyDebugInfo);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createStripNonDebugSymbolsPass();
KMETHOD LLVM_createStripNonDebugSymbolsPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createStripNonDebugSymbolsPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createStripDeadDebugInfoPass();
KMETHOD LLVM_createStripDeadDebugInfoPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createStripDeadDebugInfoPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createConstantMergePass();
KMETHOD LLVM_createConstantMergePass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createConstantMergePass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createGlobalOptimizerPass();
KMETHOD LLVM_createGlobalOptimizerPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createGlobalOptimizerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createGlobalDCEPass();
KMETHOD LLVM_createGlobalDCEPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createGlobalDCEPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createFunctionInliningPass(int threshold);
KMETHOD LLVM_createFunctionInliningPass(CTX, ksfp_t *sfp _RIX)
{
	int threshold = sfp[0].ivalue;
	Pass *ptr = createFunctionInliningPass(threshold);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createAlwaysInlinerPass();
KMETHOD LLVM_createAlwaysInlinerPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createAlwaysInlinerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createPruneEHPass();
KMETHOD LLVM_createPruneEHPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createPruneEHPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createInternalizePass(bool allButMain);
KMETHOD LLVM_createInternalizePass(CTX, ksfp_t *sfp _RIX)
{
	bool allButMain = sfp[0].bvalue;
	ModulePass *ptr = createInternalizePass(allButMain);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createDeadArgEliminationPass();
KMETHOD LLVM_createDeadArgEliminationPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createDeadArgEliminationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createArgumentPromotionPass(int maxElements);
KMETHOD LLVM_createArgumentPromotionPass(CTX, ksfp_t *sfp _RIX)
{
	int maxElements = sfp[0].ivalue;
	Pass *ptr = createArgumentPromotionPass(maxElements);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createIPConstantPropagationPass();
KMETHOD LLVM_createIPConstantPropagationPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createIPConstantPropagationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createIPSCCPPass();
KMETHOD LLVM_createIPSCCPPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createIPSCCPPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLoopExtractorPass();
KMETHOD LLVM_createLoopExtractorPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLoopExtractorPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createSingleLoopExtractorPass();
KMETHOD LLVM_createSingleLoopExtractorPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createSingleLoopExtractorPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createBlockExtractorPass();
KMETHOD LLVM_createBlockExtractorPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createBlockExtractorPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createStripDeadPrototypesPass();
KMETHOD LLVM_createStripDeadPrototypesPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createStripDeadPrototypesPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createFunctionAttrsPass();
KMETHOD LLVM_createFunctionAttrsPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createFunctionAttrsPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createMergeFunctionsPass();
KMETHOD LLVM_createMergeFunctionsPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createMergeFunctionsPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ModulePass LLVM.createPartialInliningPass();
KMETHOD LLVM_createPartialInliningPass(CTX, ksfp_t *sfp _RIX)
{
	ModulePass *ptr = createPartialInliningPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createConstantPropagationPass();
KMETHOD LLVM_createConstantPropagationPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createConstantPropagationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createSCCPPass();
KMETHOD LLVM_createSCCPPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createSCCPPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createDeadInstEliminationPass();
KMETHOD LLVM_createDeadInstEliminationPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createDeadInstEliminationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createDeadCodeEliminationPass();
KMETHOD LLVM_createDeadCodeEliminationPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createDeadCodeEliminationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createDeadStoreEliminationPass();
KMETHOD LLVM_createDeadStoreEliminationPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createDeadStoreEliminationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createAggressiveDCEPass();
KMETHOD LLVM_createAggressiveDCEPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createAggressiveDCEPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createScalarReplAggregatesPass(int threshold);
KMETHOD LLVM_createScalarReplAggregatesPass(CTX, ksfp_t *sfp _RIX)
{
	int threshold = sfp[0].ivalue;
	FunctionPass *ptr = createScalarReplAggregatesPass(threshold);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createIndVarSimplifyPass();
KMETHOD LLVM_createIndVarSimplifyPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createIndVarSimplifyPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createInstructionCombiningPass();
KMETHOD LLVM_createInstructionCombiningPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createInstructionCombiningPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLICMPass();
KMETHOD LLVM_createLICMPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLICMPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLoopUnswitchPass(bool optimizeForSize);
KMETHOD LLVM_createLoopUnswitchPass(CTX, ksfp_t *sfp _RIX)
{
	bool optimizeForSize = sfp[0].bvalue;
	Pass *ptr = createLoopUnswitchPass(optimizeForSize);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLoopInstSimplifyPass();
KMETHOD LLVM_createLoopInstSimplifyPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLoopInstSimplifyPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLoopUnrollPass(int threshold, int count, int allowPartial);
KMETHOD LLVM_createLoopUnrollPass(CTX, ksfp_t *sfp _RIX)
{
	int threshold = sfp[0].ivalue;
	int count = sfp[1].ivalue;
	int allowPartial = sfp[2].ivalue;
	Pass *ptr = createLoopUnrollPass(threshold,count,allowPartial);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLoopRotatePass();
KMETHOD LLVM_createLoopRotatePass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLoopRotatePass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLoopIdiomPass();
KMETHOD LLVM_createLoopIdiomPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLoopIdiomPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createPromoteMemoryToRegisterPass();
KMETHOD LLVM_createPromoteMemoryToRegisterPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createPromoteMemoryToRegisterPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createDemoteRegisterToMemoryPass();
KMETHOD LLVM_createDemoteRegisterToMemoryPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createDemoteRegisterToMemoryPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createReassociatePass();
KMETHOD LLVM_createReassociatePass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createReassociatePass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createJumpThreadingPass();
KMETHOD LLVM_createJumpThreadingPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createJumpThreadingPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createCFGSimplificationPass();
KMETHOD LLVM_createCFGSimplificationPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createCFGSimplificationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createBreakCriticalEdgesPass();
KMETHOD LLVM_createBreakCriticalEdgesPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createBreakCriticalEdgesPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLoopSimplifyPass();
KMETHOD LLVM_createLoopSimplifyPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLoopSimplifyPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createTailCallEliminationPass();
KMETHOD LLVM_createTailCallEliminationPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createTailCallEliminationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createLowerSwitchPass();
KMETHOD LLVM_createLowerSwitchPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createLowerSwitchPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createBlockPlacementPass();
KMETHOD LLVM_createBlockPlacementPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createBlockPlacementPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLCSSAPass();
KMETHOD LLVM_createLCSSAPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLCSSAPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createEarlyCSEPass();
KMETHOD LLVM_createEarlyCSEPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createEarlyCSEPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createGVNPass(bool noLoads);
KMETHOD LLVM_createGVNPass(CTX, ksfp_t *sfp _RIX)
{
	bool noLoads = sfp[0].bvalue;
	FunctionPass *ptr = createGVNPass(noLoads);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createMemCpyOptPass();
KMETHOD LLVM_createMemCpyOptPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createMemCpyOptPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLoopDeletionPass();
KMETHOD LLVM_createLoopDeletionPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLoopDeletionPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createSimplifyLibCallsPass();
KMETHOD LLVM_createSimplifyLibCallsPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createSimplifyLibCallsPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createInstructionNamerPass();
KMETHOD LLVM_createInstructionNamerPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createInstructionNamerPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createSinkingPass();
KMETHOD LLVM_createSinkingPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createSinkingPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createLowerAtomicPass();
KMETHOD LLVM_createLowerAtomicPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createLowerAtomicPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createCorrelatedValuePropagationPass();
KMETHOD LLVM_createCorrelatedValuePropagationPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createCorrelatedValuePropagationPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createObjCARCExpandPass();
KMETHOD LLVM_createObjCARCExpandPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createObjCARCExpandPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createObjCARCContractPass();
KMETHOD LLVM_createObjCARCContractPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createObjCARCContractPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createObjCARCOptPass();
KMETHOD LLVM_createObjCARCOptPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createObjCARCOptPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createInstructionSimplifierPass();
KMETHOD LLVM_createInstructionSimplifierPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createInstructionSimplifierPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## FunctionPass LLVM.createLowerExpectIntrinsicPass();
KMETHOD LLVM_createLowerExpectIntrinsicPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createLowerExpectIntrinsicPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## Pass LLVM.createUnifyFunctionExitNodesPass();
KMETHOD LLVM_createUnifyFunctionExitNodesPass(CTX, ksfp_t *sfp _RIX)
{
	Pass *ptr = createUnifyFunctionExitNodesPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ImmutablePass LLVM.createTypeBasedAliasAnalysisPass();
KMETHOD LLVM_createTypeBasedAliasAnalysisPass(CTX, ksfp_t *sfp _RIX)
{
	ImmutablePass *ptr = createTypeBasedAliasAnalysisPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ImmutablePass LLVM.createBasicAliasAnalysisPass();
KMETHOD LLVM_createBasicAliasAnalysisPass(CTX, ksfp_t *sfp _RIX)
{
	ImmutablePass *ptr = createBasicAliasAnalysisPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//## ImmutablePass LLVM.createVerifierPass();
KMETHOD LLVM_createVerifierPass(CTX, ksfp_t *sfp _RIX)
{
	FunctionPass *ptr = createVerifierPass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

#ifdef USE_LLVM_3_1
//## BasicBlockPass LLVM.createBBVectorizePass();
KMETHOD LLVM_createBBVectorizePass(CTX, ksfp_t *sfp _RIX)
{
	BasicBlockPass *ptr = createBBVectorizePass();
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}
#endif

//FunctionType Intrinsic::getType(int id, Type[] args);
KMETHOD Intrinsic_getType(CTX, ksfp_t *sfp _RIX)
{
	Intrinsic::ID id = (Intrinsic::ID) sfp[1].ivalue;
	kArray *args = sfp[2].a;
	std::vector<Type*> List;
	konoha::convert_array(List, args);
	FunctionType *ptr = Intrinsic::getType(getGlobalContext(), id, List);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

//Function     Intrinsic::getDeclaration(Module m, int id, Type[] args);
KMETHOD Intrinsic_getDeclaration(CTX, ksfp_t *sfp _RIX)
{
	Module *m = konoha::object_cast<Module *>(sfp[1].p);
	Intrinsic::ID id = (Intrinsic::ID) sfp[2].ivalue;
	kArray *args = sfp[3].a;
	std::vector<Type*> List;
	konoha::convert_array(List, args);
	Function *ptr = Intrinsic::getDeclaration(m, id, List);
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(ptr) K_RIXPARAM);
	RETURN_(p);
}

KMETHOD LLVM_parseBitcodeFile(CTX, ksfp_t *sfp _RIX)
{
	kString *Str = sfp[1].s;
	LLVMContext &Context = getGlobalContext();
	std::string ErrMsg;
	OwningPtr<MemoryBuffer> BufferPtr;
	std::string fname(S_text(Str));
	if (error_code ec = MemoryBuffer::getFile(fname, BufferPtr)) {
		std::cout << "Could not open file" << ec.message() << std::endl;
	}
	MemoryBuffer *Buffer = BufferPtr.take();
	//Module *m = getLazyBitcodeModule(Buffer, Context, &ErrMsg);
	Module *m = ParseBitcodeFile(Buffer, Context, &ErrMsg);
	if (!m) {
		std::cout << "error" << ErrMsg << std::endl;
	}
	kRawPtr *p = new_ReturnCppObject(_ctx, sfp, WRAP(m) K_RIXPARAM);
	RETURN_(p);
}

//TODO Scriptnize
KMETHOD Instruction_setMetadata(CTX _UNUSED_, ksfp_t *sfp _RIX)
{
	Instruction *inst = konoha::object_cast<Instruction *>(sfp[0].p);
	Module *m = konoha::object_cast<Module *>(sfp[1].p);
	kString *Str = sfp[2].s;
	kint_t N = Int_to(kint_t,sfp[3]);
	Value *Info[] = {
		ConstantInt::get(Type::getInt32Ty(getGlobalContext()), N)
	};
	LLVMContext &Context = getGlobalContext();
	MDNode *node = MDNode::get(Context, Info);
	NamedMDNode *NMD = m->getOrInsertNamedMetadata(S_text(Str));
	unsigned KindID = Context.getMDKindID(S_text(Str));
	NMD->addOperand(node);
	inst->setMetadata(KindID, node);
	RETURNvoid_();
}

//static knh_IntData_t IntIntrinsic[] = {
//	{"Pow"  ,    (int) Intrinsic::pow},
//	{"Sqrt" ,    (int) Intrinsic::sqrt},
//	{"Exp"  ,    (int) Intrinsic::exp},
//	{"Log10",    (int) Intrinsic::log10},
//	{"Log"  ,    (int) Intrinsic::log},
//	{"Sin"  ,    (int) Intrinsic::sin},
//	{"Cos"  ,    (int) Intrinsic::cos},
//	{NULL, 0}
//};
//
//static knh_IntData_t IntGlobalVariable[] = {
//	{"ExternalLinkage",                 GlobalValue::ExternalLinkage},
//	{"AvailableExternallyLinkage",      GlobalValue::AvailableExternallyLinkage},
//	{"LinkOnceAnyLinkage",              GlobalValue::LinkOnceODRLinkage},
//	{"WeakAnyLinkage",                  GlobalValue::WeakAnyLinkage},
//	{"WeakODRLinkage",                  GlobalValue::WeakODRLinkage},
//	{"AppendingLinkage",                GlobalValue::AppendingLinkage},
//	{"InternalLinkage",                 GlobalValue::InternalLinkage},
//	{"PrivateLinkage",                  GlobalValue::PrivateLinkage},
//	{"LinkerPrivateLinkage",            GlobalValue::LinkerPrivateLinkage},
//	{"LinkerPrivateWeakLinkage",        GlobalValue::LinkerPrivateWeakLinkage},
//	{"LinkerPrivateWeakDefAutoLinkage", GlobalValue::LinkerPrivateWeakDefAutoLinkage},
//	{"DLLImportLinkage",                GlobalValue::DLLImportLinkage},
//	{"DLLExportLinkage",                GlobalValue::DLLExportLinkage},
//	{"ExternalWeakLinkage",             GlobalValue::ExternalWeakLinkage},
//	{"CommonLinkage",                   GlobalValue::CommonLinkage},
//	{NULL, 0}
//};
//
//void defGlobalValue(CTX _UNUSED_, kcid_t cid _UNUSED_, kclassdef_t *cdef)
//{
//	cdef->name = "GlobalValue";
//}
//
//void constGlobalValue(CTX, kcid_t cid, const knh_LoaderAPI_t *kapi)
//{
//	kapi->loadClassIntConst(_ctx, cid, IntGlobalVariable);
//}
//
//void defIntrinsic(CTX _UNUSED_, kcid_t cid _UNUSED_, kclassdef_t *cdef)
//{
//	cdef->name = "Intrinsic";
//}
//
//void constIntrinsic(CTX, kcid_t cid, const knh_LoaderAPI_t *kapi)
//{
//	kapi->loadClassIntConst(_ctx, cid, IntIntrinsic);
//}
//
//#define C_(S) {#S , S ## _i}
//using namespace llvm::Attribute;
//static const knh_IntData_t IntAttributes[] = {
//	C_(None),
//	C_(ZExt),
//	C_(SExt),
//	C_(NoReturn),
//	C_(InReg),
//	C_(StructRet),
//	C_(NoUnwind),
//	C_(NoAlias),
//	C_(ByVal),
//	C_(Nest),
//	C_(ReadNone),
//	C_(ReadOnly),
//	C_(NoInline),
//	C_(AlwaysInline),
//	C_(OptimizeForSize),
//	C_(StackProtect),
//	C_(StackProtectReq),
//	C_(Alignment),
//	C_(NoCapture),
//	C_(NoRedZone),
//	C_(NoImplicitFloat),
//	C_(Naked),
//	C_(InlineHint),
//	C_(StackAlignment),
//	C_(ReturnsTwice),
//	C_(UWTable),
//	C_(NonLazyBind),
//	{NULL, 0}
//};
//#undef C_
//
//void defAttributes(CTX _UNUSED_, kcid_t cid _UNUSED_, kclassdef_t *cdef)
//{
//	cdef->name = "Attributes";
//}
//
//void constAttributes(CTX _UNUSED_, kcid_t cid _UNUSED_, const knh_LoaderAPI_t *kapi)
//{
//	kapi->loadClassIntConst(_ctx, cid, IntAttributes);
//}

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t llvm_initPackage(CTX, struct kLingo *lgo, int argc, const char **args, kline_t pline)
{

	static const char *TypeDefName[] = {
		"Type",
		"IntegerType",
		"PointerType",
	};
	{
		static KSTRUCT_DEF TypeDef;
		bzero(&TypeDef, sizeof(KSTRUCT_DEF));
		TypeDef.cid  = CLASS_newid;
		TypeDef.init = Type_init;
		TypeDef.free = Type_free;
		for (int i = 0; i < 3; ++i) {
			TypeDef.structname = TypeDefName[i];
			kaddClassDef(lgo->pid, 0, &TypeDef);
		}
	}
	static KSTRUCT_DEF BasicBlockDef = {
		"BasicBlock"/*structname*/,
		CLASS_newid/*cid*/,  0/*cflag*/,
		0/*bcid*/, 0/*supcid*/, 0/*cstruct_size*/,
		NULL/*fields*/, 0/*fsize*/, 0/*fallocsize*/,
		0/*init*/,
		0/*reftrace*/,
		0/*free*/,
		0/*fnull*/,
		0/*p*/, 0/*unbox*/,
		BasicBlock_compareTo/*compareTo*/,
		0/*getkey*/,
		0/*hashCode*/,
		0/*initdef*/
	};
	static KSTRUCT_DEF PassManagerBuilderDef = {
		"BasicBlock"/*structname*/,
		CLASS_newid/*cid*/,  0/*cflag*/,
		0/*bcid*/, 0/*supcid*/, 0/*cstruct_size*/,
		NULL/*fields*/, 0/*fsize*/, 0/*fallocsize*/,
		PassManagerBuilder_ptr_init/*init*/,
		0/*reftrace*/,
		PassManagerBuilder_ptr_free/*free*/,
		0/*fnull*/,
		0/*p*/, 0/*unbox*/,
		0/*compareTo*/,
		0/*getkey*/,
		0/*hashCode*/,
		0/*initdef*/
	};
	static KSTRUCT_DEF PassManagerDef = {
		"PassManager"/*structname*/,
		CLASS_newid/*cid*/,  0/*cflag*/,
		0/*bcid*/, 0/*supcid*/, 0/*cstruct_size*/,
		NULL/*fields*/, 0/*fsize*/, 0/*fallocsize*/,
		PassManager_ptr_init/*init*/,
		0/*reftrace*/,
		PassManager_ptr_free/*free*/,
		0/*fnull*/,
		0/*p*/, 0/*unbox*/,
		0/*compareTo*/,
		0/*getkey*/,
		0/*hashCode*/,
		0/*initdef*/
	};
	static KSTRUCT_DEF FunctionPassManagerDef = {
		"FunctionPassManager"/*structname*/,
		CLASS_newid/*cid*/,  0/*cflag*/,
		0/*bcid*/, 0/*supcid*/, 0/*cstruct_size*/,
		NULL/*fields*/, 0/*fsize*/, 0/*fallocsize*/,
		FunctionPassManager_ptr_init/*init*/,
		0/*reftrace*/,
		FunctionPassManager_ptr_free/*free*/,
		0/*fnull*/,
		0/*p*/, 0/*unbox*/,
		0/*compareTo*/,
		0/*getkey*/,
		0/*hashCode*/,
		0/*initdef*/
	};
	kaddClassDef(lgo->pid, 0, &BasicBlockDef);
	kaddClassDef(lgo->pid, 0, &PassManagerDef);
	kaddClassDef(lgo->pid, 0, &PassManagerBuilderDef);
	kaddClassDef(lgo->pid, 0, &FunctionPassManagerDef);
	return true;
}

static kbool_t llvm_setupPackage(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t llvm_initLingo(CTX,  struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t llvm_setupLingo(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* llvm_init(void)
{
	InitializeNativeTarget();
	static KPACKDEF d = {
		K_CHECKSUM,
		"llvm", "3.0", "", "", "",
		llvm_initPackage,
		llvm_setupPackage,
		llvm_initLingo,
		llvm_setupLingo,
		K_REVISION
	};

	return &d;
}

#ifdef __cplusplus
}
#endif
