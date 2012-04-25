#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <string>
#include <stdint.h>
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Constants.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/IRBuilder.h"
#if LLVM_VERSION <= 209
#include "llvm/Target/TargetSelect.h"
#include "llvm/Target/TargetRegistry.h"
#else
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#endif
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"

#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include <konoha2/float.h>
#include "jit.h"

using namespace llvm;
/* ************************************************************************ */

namespace konoha {

typedef struct vstack vstack_t;

typedef struct {
	kmodshare_t h;
	Module *global_module;
	vstack_t *stack;
	kmap_t *jitcache;
	//std::vector<Value*> *stack;
	ExecutionEngine *global_ee;
	IRBuilder<> *builder;
	Function *func;
	kline_t uline;
	Value *vctx;
	Value *vsfp;
	Value *returnValue;
} kmodjit_t;

typedef struct {
	kmodlocal_t h;
} kjitmod_t;


struct vstack {
	size_t capacity;
	size_t size;
	Value **vals;
};

static vstack_t *ValueStack_new(CTX, size_t n)
{
	vstack_t *stack = (vstack_t*) KNH_ZMALLOC(sizeof(vstack_t));
	stack->capacity = n;
	stack->size  = 0;
	stack->vals = (Value **)(KNH_MALLOC(sizeof(Value*) * n));
	return stack;
}

static void ValueStack_resize(CTX, vstack_t *stack, size_t n)
{
	if (n >= stack->capacity) {
		Value **vals = stack->vals;
		size_t capacity = stack->capacity;
		stack->capacity = capacity * 2;
		fprintf(stderr, "%p, n=%lu, cap=%lu\n", (void*) stack->vals, n, capacity);
		stack->vals = (Value **)(KNH_MALLOC(sizeof(Value*) * stack->capacity));
		memcpy(stack->vals, vals, sizeof(Value*) * capacity);
		bzero(stack->vals+capacity, sizeof(Value*) * capacity);
		KNH_FREE(vals, sizeof(Value*) * capacity);
	}
}

static size_t ValueStack_size(vstack_t *stack)
{
	return stack->size;
}

//static void ValueStack_dump(CTX)
//{
//	vstack_t *stack = kmodjit->stack;
//	for (size_t i = 0; i < stack->size; i++) {
//		fprintf(stderr, "%d: %p\n", (int)i, (void*)stack->vals[i]);
//	}
//}

static Value *ValueStack_at(CTX, int n)
{
	vstack_t *stack = kmodjit->stack;
	Value *v = stack->vals[n];
	if (size_t(n) > stack->size) {
		abort();
	}
	fprintf(stderr, "%s, idx=%d, size() %lu, %p\n",
			__func__, n, ValueStack_size(kmodjit->stack), (void*) v);
	assert(v);
	return v;
}

static void ValueStack_set(CTX, vstack_t *stack, size_t n, Value *v)
{
	ValueStack_resize(_ctx, stack, n);
	fprintf(stderr, "%s idx %lu, Value %p siz=%lu\n",
			__func__, size_t(n), (void*)v, ValueStack_size(kmodjit->stack));
	stack->vals[n] = v;
	if (stack->size < n) {
		stack->size = n;
	}
}

static void ValueStack_clear(vstack_t *stack)
{
	size_t capacity = stack->capacity;
	bzero(stack->vals, sizeof(Value*) * capacity);
}

static void ValueStack_free(CTX, vstack_t *stack)
{
	size_t capacity = stack->capacity;
	KNH_FREE(stack->vals, capacity * sizeof(Value*));
}

uintptr_t jitcache_hash(kMethod *mtd)
{
	kcid_t cid = mtd->cid;
	kmethodn_t mn = mtd->mn;
	return (cid << sizeof(short)*8) | mn;
}

Function *jitcache_get(CTX, kMethod *mtd)
{
	uintptr_t hcode = jitcache_hash(mtd);
	kmap_t *map = kmodjit->jitcache;
	kmape_t *e = kmap_get(map, hcode);
	if (e) {
		return (Function*) e->uvalue;
	} else {
		return NULL;
	}
}

void jitcache_set(CTX, kMethod *mtd, Function *f)
{
	uintptr_t hcode = jitcache_hash(mtd);
	kmap_t *map = kmodjit->jitcache;
	// TODO support rewriting old function
	//kmape_t *e = kmap_get(map, hcode);
	//if (!e) {
	//	e = kmap_newentry(map, hcode);
	//}
	kmape_t *newe = kmap_newentry(map, hcode);
	newe->uvalue = (uintptr_t) f;
	kmap_add(map, newe);
}

static IRBuilder<> *get_builder(CTX)
{
	return kmodjit->builder;
}

static Function *get_function(CTX)
{
	return kmodjit->func;
}

static Value *getctx(CTX)
{
	return kmodjit->vctx;
}

static Value *getsfp(CTX)
{
	return kmodjit->vsfp;
}

enum ctype {
	TY_VOID,
	TY_LONG,
	TY_INT,
	TY_SHORT,
	TY_FLOAT,
	TY_PTR,
	TY_CHAR,
	TY_BOOL,
	TY_STRING,
	TY_CLASS,
	TY_METHOD,
	TY_OBJECT,
	TY_HOBJECT,
	TY_FUNCTION,
	TY_FPTR,
	TY_CONTEXT,
	TY_STACK,
	TY_NOP = -1
};

struct konoha_object_data {
	enum ctype type;
	const char *field_name;
};

static void vstack_setValue(CTX, int idx, Value *value)
{
	if (idx < 0) {
		/* this is for return expr */
		kmodjit->returnValue = value;
	} else {
		ValueStack_set(_ctx, kmodjit->stack, idx, value);
	}
}

static Type *LLVMTYPE_HOBJECT = NULL;
static Type *LLVMTYPE_OBJECT  = NULL;
static Type *LLVMTYPE_FUNCTION  = NULL;
static Type *LLVMTYPE_FUNCTIONP = NULL;
static Type *LLVMTYPE_METHOD   = NULL;
static Type *LLVMTYPE_CONTEXT  = NULL;
static Type *LLVMTYPE_STACK  = NULL;
static Type *ctype2Type(enum ctype type)
{
	if (type == TY_LONG) {
		//TODO
		return Type::getInt64Ty(getGlobalContext());
	}
	if (type == TY_INT) {
		return Type::getInt64Ty(getGlobalContext());
	}
	if (type == TY_FLOAT) {
		//TODO
		return Type::getDoubleTy(getGlobalContext());
	}
	if (type == TY_SHORT) {
		//TODO
		return Type::getInt16Ty(getGlobalContext());
	}
	if (type == TY_PTR) {
		return Type::getInt8PtrTy(getGlobalContext());
	}
	if (type == TY_CHAR) {
		return Type::getInt8Ty(getGlobalContext());
	}
	if (type == TY_BOOL) {
		return Type::getInt1Ty(getGlobalContext());
	}
	if (type == TY_STRING) {
		return Type::getInt8PtrTy(getGlobalContext());
	}
	if (type == TY_CLASS) {
		//TODO
		return Type::getInt8PtrTy(getGlobalContext());
	}
	if (type == TY_OBJECT) {
		assert(LLVMTYPE_OBJECT != NULL);
		return LLVMTYPE_OBJECT;
	}
	if (type == TY_HOBJECT) {
		assert(LLVMTYPE_HOBJECT != NULL);
		return LLVMTYPE_HOBJECT;
	}
	if (type == TY_METHOD) {
		assert(LLVMTYPE_METHOD != NULL);
		return LLVMTYPE_METHOD;
	}
	if (type == TY_FUNCTION) {
		assert(LLVMTYPE_FUNCTION != NULL);
		return LLVMTYPE_FUNCTION;
	}
	if (type == TY_FPTR) {
		assert(LLVMTYPE_FUNCTIONP != NULL);
		return LLVMTYPE_FUNCTIONP;
	}
	if (type == TY_CONTEXT) {
		assert(LLVMTYPE_CONTEXT != NULL);
		return LLVMTYPE_CONTEXT;
	}
	if (type == TY_STACK) {
		assert(LLVMTYPE_STACK != NULL);
		return LLVMTYPE_STACK;
	}
	if (type == TY_VOID) {
		return Type::getVoidTy(getGlobalContext());
	}
	fprintf(stderr, "%s:%d: [WARN] type=%d", __func__, __LINE__, (int)type);
	return Type::getVoidTy(getGlobalContext());
}

static Value *createConstValue(enum ctype type, void *v)
{
	if (type == TY_INT) {
		return ConstantInt::get(ctype2Type(type), *(uint64_t*)v);
	}
	if (type == TY_BOOL) {
		uint64_t val = *(uint64_t*)v;
		return ConstantInt::get(ctype2Type(type), val);
	}
	if (type == TY_FLOAT) {
		return ConstantFP::get(ctype2Type(type), (*(double*)v));
	}
	if (type == TY_OBJECT || type == TY_METHOD) {
		assert(LLVMTYPE_OBJECT != NULL);
		Constant *val = ConstantInt::get(ctype2Type(TY_LONG), (uintptr_t)v);
		return ConstantExpr::getIntToPtr(val, ctype2Type(type));
	}
	if (type == TY_FUNCTION) {
		assert(LLVMTYPE_FUNCTION != NULL);
		Constant *val = ConstantInt::get(ctype2Type(TY_LONG), (uintptr_t)v);
		return ConstantExpr::getIntToPtr(val, PointerType::get(ctype2Type(type), 0));
	}
	if (type == TY_FPTR) {
		assert(LLVMTYPE_FUNCTIONP != NULL);
		Constant *val = ConstantInt::get(ctype2Type(TY_LONG), (uintptr_t)v);
		return ConstantExpr::getIntToPtr(val, PointerType::get(ctype2Type(type), 0));
	}
	fprintf(stderr, "%s:%d: [WARN] type=%d", __func__, __LINE__, (int)type);
	asm volatile("int3");
	return NULL;
}


static enum ctype cid2ctype(CTX, ktype_t type)
{
	if (type == TY_Int)     { return TY_INT; }
	if (type == TY_Boolean) { return TY_BOOL; }
	if (IS_defineFloat() && type == TY_Float) {
		return TY_FLOAT;
	}
	if (type == TY_void) { return TY_VOID; }
	return TY_OBJECT;
}

static Type *cid2Type(CTX, ktype_t type)
{
	return ctype2Type(cid2ctype(_ctx, type));
}

static kcid_t type2cid(CTX, Type *type)
{
	if (type == ctype2Type(TY_INT))     { return TY_Int; }
	if (type == ctype2Type(TY_BOOL)) { return TY_Boolean; }
	if (IS_defineFloat() && type == ctype2Type(TY_FLOAT)) {
		return TY_Float;
	}
	if (type == ctype2Type(TY_VOID)) { return TY_void; }
	return TY_Object;
}

static bool isUnBoxTy(CTX, kcid_t cid)
{
	return TY_isUnbox(cid);
}

static bool isBoxTy(CTX, kcid_t cid)
{
	return !isUnBoxTy(_ctx, cid);
}

extern "C" kObject *boxing_llvm(CTX, kcid_t cid, uintptr_t data)
{
	return new_kObject(CT_(cid), data);
}

Value *boxing(CTX, Value *v)
{
	Module *m = kmodjit->global_module;
	std::vector <Type *> argsTy;
	argsTy.push_back(ctype2Type(TY_CONTEXT));
	argsTy.push_back(ctype2Type(TY_INT));
	argsTy.push_back(ctype2Type(TY_INT));
	FunctionType* fnTy = FunctionType::get(ctype2Type(TY_OBJECT), argsTy, false);
	Function *box = cast<Function>(m->getOrInsertFunction("boxing_llvm", fnTy));
	Function::arg_iterator args = get_function(_ctx)->arg_begin();

	kcid_t cid = type2cid(_ctx, v->getType());
	std::vector<Value*> params;
	params.push_back(args);/*ctx*/
	params.push_back(createConstValue(cid2ctype(_ctx, cid), (void*)&cid));
	params.push_back(v);

	IRBuilder<> *builder = get_builder(_ctx);
	Value *ret = builder->CreateCall(box, params);
	return ret;
}

/*
 * XXX
 * Remove warning: ISO C++ forbids casting between
 *                 pointer-to-function and pointer-to-object
 **/
template<typename Target, typename Source>
static inline Target bit_cast(Source v) {
	union { Source v; Target p; } val;
	val.v = v;
	return val.p;
}
#define LLVMBool(v)   (ConstantInt::get(ctype2Type(TY_BOOL),   (v)))
#define LLVMInt(v)    (ConstantInt::get(ctype2Type(TY_INT),    (v)))
#define LLVMFloat(v)  (ConstantInt::get(ctype2Type(TY_FLOAT),  (v)))

/* ------------------------------------------------------------------------ */
/* kcode */

#define BBSIZE(BB)  ((BB)->op.bytesize / sizeof(kopl_t))
#define BBOP(BB)     (BB)->op.opl
#define BBOPn(BB,N)  (BB)->op.opl[N]

static Value *EXPR_asm(CTX, int a, kExpr *expr, int espidx);

#define GammaBuilderLabel(n)   (kBasicBlock*)(kmodjit->lstacks->list[n])

#define ASM(T, ...)
#define ASMop(T, OP, ...) 
#define kBasicBlock_add(bb, T, ...)

//#define NC_(sfpidx)    (((sfpidx) * 2) + 1)
//#define OC_(sfpidx)    ((sfpidx) * 2)
//#define SFP_(sfpidx)   ((sfpidx) * 2)
//#define RBP_(rbpidx)   ((sfpidx) / 2)
#define NC_(sfpidx)    (sfpidx)
#define OC_(sfpidx)    (sfpidx)
#define SFP_(sfpidx)   (sfpidx)
#define RBP_(rbpidx)   (sfpidx)

#define RIX_(rix)      rix

/* ------------------------------------------------------------------------ */
/* new_KonohaCode */

#define ASM_LABEL(_ctx, label)
#define ASM_JMP(_ctx, label)

/* ------------------------------------------------------------------------ */
/* CALL */

#define ESP_(sfpidx, args)   SFP_(sfpidx + args + K_CALLDELTA + 1)

/* ------------------------------------------------------------------------ */

static kObject* BUILD_addConstPool(CTX, kObject *o)
{
	(void)_ctx;
	//kArray_add(kmodjit->constPools, o);
	return o;
}

template<typename T>
static T tri(bool cond, T a, T b)
{
	if (cond) {
		return a;
	} else {
		return b;
	}
}


static Value *CALL_asm(CTX, int a, kExpr *expr, int espidx);
static void AND_asm(CTX, int a, kExpr *expr, int espidx);
static void OR_asm(CTX, int a, kExpr *expr, int espidx);
static Value *LETEXPR_asm(CTX, int a, kExpr *expr, int espidx);

static Value* NMOV_asm(CTX, int a, ktype_t ty, int b)
{
	(void)a;
	Value *v = NULL;
	if(TY_isUnbox(ty)) {
		v = ValueStack_at(_ctx, NC_(b));
		vstack_setValue(_ctx, NC_(a), v);
		//ASM(NMOV, NC_(a), NC_(b), CT_(ty));
	}
	else {
		//ASM(NMOV, OC_(a), OC_(b), CT_(ty));
	}
	return v;
}

static Value *ASM_NSET(CTX, int isUnbox, void *n, kclass_t *ty, int a)
{
	fprintf(stderr, "asm_nset %ld is_Undox %d\n", (uintptr_t)n, isUnbox);
	Value *v = createConstValue(cid2ctype(_ctx, ty->cid), n);
	int idx = tri(isUnbox, NC_(a), OC_(a));
	vstack_setValue(_ctx, idx, v);
	return v;
}

static Value *EXPR_asm(CTX, int a, kExpr *expr, int espidx)
{
	Value *res = NULL;
	if (expr->build == TEXPR_CONST) {
		kObject *v = expr->data;
		int isUnbox = TY_isUnbox(expr->ty);
		if(isUnbox) {
			//ASM(NSET, NC_(a), (uintptr_t)N_toint(v), CT_(expr->ty));
			return ASM_NSET(_ctx, isUnbox, (void*)&N_toint(v), CT_(expr->ty), a);
		}
		else {
			v = BUILD_addConstPool(_ctx, v);
			return ASM_NSET(_ctx, isUnbox, (void*)&v, CT_(expr->ty), a);
			//ASM(NSET, OC_(a), (uintptr_t)v, CT_(expr->ty));
		}
	}
	else if(expr->build == TEXPR_NCONST) {
		return ASM_NSET(_ctx, 1, (void*)&expr->ndata, CT_(expr->ty), a);
		//ASM(NSET, NC_(a), expr->ndata, CT_(expr->ty));
	}
	else if(expr->build == TEXPR_LOCAL) {
		return NMOV_asm(_ctx, a, expr->ty, expr->index);
	}
	else if(expr->build == TEXPR_FIELD) {
		abort();
		//kshort_t index = (kshort_t)expr->index;
		//kshort_t xindex = (kshort_t)(expr->index >> (sizeof(kshort_t)*8));
		//if(TY_isUnbox(expr->ty)) {
		//	ASM(NMOVx, NC_(a), OC_(index), xindex, CT_(expr->ty));
		//}
		//else {
		//	ASM(NMOVx, OC_(a), OC_(index), xindex, CT_(expr->ty));
		//}
	}
	else if(expr->build == TEXPR_NULL) {
		int isUnbox = TY_isUnbox(expr->ty);
		if(isUnbox) {
			return ASM_NSET(_ctx, 0, 0, CT_(expr->ty), a);
		}
		else {
			return ASM_NSET(_ctx, 0, (void*)knull(CT_(expr->ty)), CT_(expr->ty), a);
		}
	}
	else if(expr->build == TEXPR_NEW) {
		ASM(NEW, OC_(a), expr->index, CT_(expr->ty));
	}
	else if(expr->build == TEXPR_BOX) {
		abort();
		DBG_ASSERT(IS_Expr(expr->single));
		EXPR_asm(_ctx, a, expr->single, espidx);
		ASM(BOX, OC_(a), NC_(a), CT_(expr->single->ty));
	}
	else if(expr->build == TEXPR_UNBOX) {
		ASM(UNBOX, NC_(a), OC_(a), CT_(expr->ty));
	}
	else if(expr->build == TEXPR_CALL) {
		Value *v = CALL_asm(_ctx, a, expr, espidx);
		if(a != espidx) {
			return NMOV_asm(_ctx, a, expr->ty, espidx);
		}
		return v;
	}
	else if(expr->build == TEXPR_AND) {
		AND_asm(_ctx, a, expr, espidx);
	}
	else if(expr->build == TEXPR_OR) {
		OR_asm(_ctx, a, expr, espidx);
	}
	else if(expr->build == TEXPR_LET) {
		return LETEXPR_asm(_ctx, a, expr, espidx);
	} else {
		DBG_P("unknown expr=%d", expr->build);
		abort();
	}
	assert(res != NULL);
	return res;
}

static Value *loadsfp(CTX, IRBuilder<> *builder, Value *vsfp, ktype_t reqtype, int sfpidx);
static void storesfp(CTX, IRBuilder<> *builder, Value *vsfp, Value *v, ktype_t reqtype, int sfpidx)
{
	Value *vsfp0 = loadsfp(_ctx, builder, vsfp, reqtype, sfpidx);
	builder->CreateStore(v, vsfp0, false);
}

static void emit_param(CTX, kMethod *mtd, kExpr *expr, int thisidx, int i)
{
	if (i < (int)kArray_size(expr->cons)) {
		kExpr *exprN = kExpr_at(expr, i);
		DBG_ASSERT(IS_Expr(exprN));
		Value *v = EXPR_asm(_ctx, thisidx + i - 1, exprN, thisidx + i - 1);
		storesfp(_ctx, get_builder(_ctx), getsfp(_ctx), v, exprN->ty, thisidx+i-1);
		emit_param(_ctx, mtd, expr, thisidx, i+1);
	}
}

static void asm_shift_esp(CTX, size_t idx)
{
	IRBuilder<> *builder = get_builder(_ctx);
	Value *vesp = builder->CreateStructGEP(getctx(_ctx), 1, "gep"); /* esp */
	Value *vsfp = builder->CreateConstInBoundsGEP1_32(getsfp(_ctx), idx);
	builder->CreateStore(vsfp, vesp, false);
}

static Value *loadsfp(CTX, IRBuilder<> *builder, Value *vsfp, ktype_t reqtype, int sfpidx);
static Value *loadMtdFromSfp(CTX, IRBuilder<> *builder, Value *vsfp, int mtdidx)
{
	Value *v = loadsfp(_ctx, builder, vsfp, TY_Int, mtdidx);
	return builder->CreateBitCast(v, PointerType::get(ctype2Type(TY_METHOD), 0), "cast");
}

static void prepare_call_stack(CTX, int thisidx, Value *vsfp, kObject *def, kMethod *mtd, kline_t uline)
{
	int rtnidx = thisidx + K_RTNIDX;
	int mtdidx = thisidx + K_MTDIDX;
	IRBuilder<> *builder = get_builder(_ctx);
	if (O_cid(def) != TY_void) {
		Value *vsfp0 = loadsfp(_ctx, builder, vsfp, O_cid(def), rtnidx);
		if (TY_isUnbox(O_cid(def))) {
			uintptr_t ndata = O_unbox(def);
			Value *vdef  = createConstValue(cid2ctype(_ctx, O_cid(def)), (void*)&ndata);
			builder->CreateStore(vdef, vsfp0, false);
		} else {
			Value *vdef  = createConstValue(TY_OBJECT, bit_cast<void*>(def));
			builder->CreateStore(vdef, vsfp0, false);
		}
	}
	Value *vsfp1 = loadMtdFromSfp(_ctx, builder, vsfp, mtdidx);
	Value *vmtd  = createConstValue(TY_METHOD, bit_cast<void*>(mtd));
	builder->CreateStore(vmtd, vsfp1, false);

	Value *vsfp2 = loadsfp(_ctx, builder, vsfp, TY_Int, rtnidx);
	Value *vline = createConstValue(TY_INT, bit_cast<void*>(&uline));
	builder->CreateStore(vline, vsfp2, false);

}

static void param_setCtxSfp(CTX, std::vector<Value*> &params, int sfpidx)
{
	IRBuilder<> *builder = get_builder(_ctx);
	Function::arg_iterator args = get_function(_ctx)->arg_begin();
	Value *arg_ctx = args++;
	Value *arg_sfp = args;
	params.push_back(arg_ctx);
	if (sfpidx > 0) {
		arg_sfp = builder->CreateGEP(arg_sfp, LLVMInt(sfpidx), "sfpshift");
	}
	params.push_back(arg_sfp);
}

#if 0
// use trap(), createTrap() for debuging
extern "C" void trap(void *p0, void *p1) {
	kMethod *mtd = (kMethod *) p0;
	asm volatile("int3");
}

void createTrap(CTX, IRBuilder<> *builder, Value *v0, Value *v1 = NULL)
{
	Module *m = kmodjit->global_module;
	Type *retTy = Type::getVoidTy(getGlobalContext());
	std::vector<Type *> argsTy;
	Type *voidPtrTy = Type::getInt8PtrTy(getGlobalContext());
	argsTy.push_back(voidPtrTy);

	if (v1) {
		argsTy.push_back(voidPtrTy);
	}
	std::vector<Value *> args;
	FunctionType *fnTy = FunctionType::get(retTy, argsTy, false);

	Function *f = cast<Function>(m->getOrInsertFunction("trap", fnTy));

	Value *p0 = builder->CreateBitCast(v0, voidPtrTy, "cast");
	args.push_back(p0);
	if (v1) {
		if (v1->getType() == Type::getInt16Ty(getGlobalContext())||
				v1->getType() == Type::getInt64Ty(getGlobalContext())
				) {
			v1 = builder->CreateIntToPtr(v1, voidPtrTy);
		}
		Value *p1 = builder->CreateBitCast(v1, voidPtrTy, "cast");
		args.push_back(p1);
	}
	builder->CreateCall(f, args);
}
#endif

static kmethodn_t MN_opNEG;
static kmethodn_t MN_opADD;
static kmethodn_t MN_opSUB;
static kmethodn_t MN_opMUL;
static kmethodn_t MN_opDIV;
static kmethodn_t MN_opMOD;
static kmethodn_t MN_opEQ;
static kmethodn_t MN_opNE;
static kmethodn_t MN_opLT;
static kmethodn_t MN_opLTE;
static kmethodn_t MN_opGT;
static kmethodn_t MN_opGTE;
static kmethodn_t MN_opLAND;
static kmethodn_t MN_opLOR;
static kmethodn_t MN_opLXOR;
static kmethodn_t MN_opLSFT;
static kmethodn_t MN_opRSFT;

static bool check_asm_int_op(kmethodn_t mn, int argc)
{
	if (argc == 0) {
		if(mn == MN_opNEG) {
			return true;
		}
	}
	if (argc == 1) {
		if(mn == MN_opADD ) { return true; }
		if(mn == MN_opSUB ) { return true; }
		if(mn == MN_opMUL ) { return true; }
		if(mn == MN_opDIV ) { return true; }
		if(mn == MN_opMOD ) { return true; }
		if(mn == MN_opEQ  ) { return true; }
		if(mn == MN_opNE  ) { return true; }
		if(mn == MN_opLT  ) { return true; }
		if(mn == MN_opLTE ) { return true; }
		if(mn == MN_opGT  ) { return true; }
		if(mn == MN_opGTE ) { return true; }
		if(mn == MN_opLAND) { return true; }
		if(mn == MN_opLOR ) { return true; }
		if(mn == MN_opLXOR) { return true; }
		if(mn == MN_opLSFT) { return true; }
		if(mn == MN_opRSFT) { return true; }
	}
	return false;
}

static Value *asm_int_op(CTX, kmethodn_t mn, kExpr *exprN, int local)
{
	IRBuilder<> *builder = get_builder(_ctx);
	if(mn == MN_opNEG) {
		Value *va = EXPR_asm(_ctx, local+0, kExpr_at(exprN, 1), local+0);
		return builder->CreateNeg(va, "neg");
	} else {
		Value *va = EXPR_asm(_ctx, local+0, kExpr_at(exprN, 1), local+0);
		Value *vb = EXPR_asm(_ctx, local+1, kExpr_at(exprN, 2), local+1);
		if(mn == MN_opADD ) { return builder->CreateAdd(va, vb, "add");}
		if(mn == MN_opSUB ) { return builder->CreateSub(va, vb, "sub");}
		if(mn == MN_opMUL ) { return builder->CreateMul(va, vb, "mul");}
		if(mn == MN_opDIV ) { return builder->CreateSDiv(va, vb, "div");}
		if(mn == MN_opMOD ) { return builder->CreateSRem(va, vb, "mod");}
		if(mn == MN_opEQ  ) { return builder->CreateICmpEQ(va, vb, "eq");}
		if(mn == MN_opNE  ) { return builder->CreateICmpNE(va, vb, "ne");}
		if(mn == MN_opLT  ) { return builder->CreateICmpSLT(va, vb, "lt");}
		if(mn == MN_opLTE ) { return builder->CreateICmpSLE(va, vb, "le");}
		if(mn == MN_opGT  ) { return builder->CreateICmpSGT(va, vb, "gt");}
		if(mn == MN_opGTE ) { return builder->CreateICmpSGE(va, vb, "ge");}
		if(mn == MN_opLAND) { return builder->CreateAnd( va, vb, "and");}
		if(mn == MN_opLOR ) { return builder->CreateOr(va, vb, "or");}
		if(mn == MN_opLXOR) { return builder->CreateXor(va, vb, "xor");}
		if(mn == MN_opLSFT) { return builder->CreateShl(va, vb, "lshr");}
		if(mn == MN_opRSFT) { return builder->CreateAShr(va, vb, "rshr");}
	}
	return NULL;
}

static void push_back_args(CTX, std::vector<Value*> &args, kExpr *expr, int local, int i, int n)
{
	if (i < n) {
		Value *v = EXPR_asm(_ctx, local+i, kExpr_at(expr, i), local+i);
		args.push_back(v);
		push_back_args(_ctx, args, expr, local, i+1, n);
	}
}

Value *asm_functoin_call(CTX, kline_t uline, kExpr *expr, int thisidx, int s, Function *f)
{
	std::vector<Value*> args;
	IRBuilder<> *builder = get_builder(_ctx);
	args.push_back(getctx(_ctx));
	args.push_back(getsfp(_ctx));
	push_back_args(_ctx, args, expr, thisidx+s-1, s, kArray_size(expr->cons));
	return builder->CreateCall(f, args);
}

static void ASM_SCALL(CTX, kline_t uline, int sfpidx, int espidx, kMethod *mtd, kObject *defval)
{
	int thisidx = RBP_(sfpidx);
	IRBuilder<> *builder = get_builder(_ctx);
	Value *vmtd  = createConstValue(TY_METHOD, bit_cast<void*>(mtd));
	Value *funcp = builder->CreateStructGEP(vmtd, 1, "fcall1");
	Value *func  = builder->CreateLoad(funcp);
	prepare_call_stack(_ctx, thisidx, getsfp(_ctx), defval, mtd, uline);
	std::vector<Value*> params;
	param_setCtxSfp(_ctx, params, thisidx);
	params.push_back(LLVMInt(K_RTNIDX));
	asm_shift_esp(_ctx, espidx);
	builder->CreateCall(func, params);
}

static Value *CALL_asm(CTX, int a, kExpr *expr, int espidx)
{
	(void)a;
	kMethod *mtd = expr->cons->methods[0];
	DBG_ASSERT(IS_Method(mtd));
	int s = tri(kMethod_isStatic(mtd), 2, 1);
	int thisidx = espidx + K_CALLDELTA;
	int argc = kArray_size(expr->cons) - 2;
	if(kMethod_isVirtual(mtd)) {
		ASM(NSET, NC_(thisidx-1), (intptr_t)mtd, CT_Method);
		ASM(CALL, kmodjit->uline, SFP_(thisidx), ESP_(espidx, argc), knull(CT_(expr->ty)));
	}
	else {
		if (mtd->cid == TY_Int) {
			if (check_asm_int_op(mtd->mn, argc)) {
				Value *v = asm_int_op(_ctx, mtd->mn, expr, thisidx + s - 1);
				vstack_setValue(_ctx, NC_(espidx), v);
				return v;
			}
		}
		if (IS_defineFloat() && mtd->cid == TY_Float) {
		}
		Function *f = jitcache_get(_ctx, mtd);
		if(f == NULL) {
			emit_param(_ctx, mtd, expr, thisidx, s);
			ASM_SCALL(_ctx, kmodjit->uline, SFP_(thisidx), ESP_(espidx, argc), mtd, knull(CT_(expr->ty)));
		}
		else {
			Value *v = asm_functoin_call(_ctx, kmodjit->uline, expr, thisidx, s, f);
			if (v) {
				vstack_setValue(_ctx, NC_(espidx), v);
			}
			return v;
			//TODO Call llvm-nized function directly
			//ASM(VCALL, kmodjit->uline, SFP_(thisidx), ESP_(espidx, argc), mtd, knull(CT_(expr->ty)));
		}
	}
	if (mtd->pa->rtype != TY_void) {
		IRBuilder<> *builder = get_builder(_ctx);
		Value *v = loadsfp(_ctx, builder, getsfp(_ctx), mtd->pa->rtype, SFP_(thisidx) + K_RTNIDX);
		v = builder->CreateLoad(v);
		vstack_setValue(_ctx, tri(TY_isUnbox(mtd->pa->rtype), NC_(espidx), OC_(espidx)), v);
		return v;
	}
	return NULL;
}

static void bool_createPHI(CTX, std::vector<BasicBlock*> *blocks, PHINode *phi, Value *vfirst, Value *vlast) {
	BasicBlock *bb = blocks->front();
	blocks->erase(blocks->begin());
	if (blocks->empty()) {
		phi->addIncoming(vlast, bb);
	} else {
		phi->addIncoming(vfirst, bb);
		bool_createPHI(_ctx, blocks, phi, vfirst, vlast);
	}
}

static void OR_asm_inner(CTX, kExpr *expr, std::vector<BasicBlock*> *blocks, BasicBlock *merge, int i, int size, int espidx)
{
	IRBuilder<> *builder = kmodjit->builder;
	builder->SetInsertPoint(*blocks->end());
	BasicBlock *next = BasicBlock::Create(getGlobalContext(), "bbOrExprCond", kmodjit->func);
	blocks->push_back(next);
	Value *cond = EXPR_asm(_ctx, espidx+1, kExpr_at(expr, i), espidx+1);
	builder->CreateCondBr(cond, merge, next);
	if (i == size) {
		builder->SetInsertPoint(next);
		builder->CreateBr(merge);
	} else {
		OR_asm_inner(_ctx, expr, blocks, merge, i+1, size, espidx);
	}
}

static void OR_asm(CTX, int a, kExpr *expr, int espidx)
{
	(void)a;
	int size = kArray_size(expr->cons);
	IRBuilder<> *builder = kmodjit->builder;
	BasicBlock *bbMerge = BasicBlock::Create(getGlobalContext(), "bbOrExprMerge", kmodjit->func);
	BasicBlock *bbCond = BasicBlock::Create(getGlobalContext(), "bbOrExprCond", kmodjit->func);
	std::vector<BasicBlock*> blocks;
	blocks.push_back(bbCond);
	OR_asm_inner(_ctx, expr, &blocks, bbMerge, 1, size, espidx);
	builder->SetInsertPoint(bbMerge);
	PHINode *phi = builder->CreatePHI(ctype2Type(TY_BOOL), 0, "orExprPhi");
	bool_createPHI(_ctx, &blocks, phi, LLVMBool(1), LLVMBool(0));
}

static void AND_asm_inner(CTX, kExpr *expr, std::vector<BasicBlock*> *blocks, BasicBlock *merge, int i, int size, int espidx)
{
	IRBuilder<> *builder = kmodjit->builder;
	builder->SetInsertPoint(*blocks->end());
	BasicBlock *next = BasicBlock::Create(getGlobalContext(), "bbAndExprCond", kmodjit->func);
	blocks->push_back(next);
	Value *cond = EXPR_asm(_ctx, espidx+1, kExpr_at(expr, i), espidx+1);
	builder->CreateCondBr(cond, next, merge);
	if (i == size) {
		builder->SetInsertPoint(next);
		builder->CreateBr(merge);
	} else {
		AND_asm_inner(_ctx, expr, blocks, merge, i+1, size, espidx);
	}
}

static void AND_asm(CTX, int a, kExpr *expr, int espidx)
{
	(void)a;
	int size = kArray_size(expr->cons);
	IRBuilder<> *builder = kmodjit->builder;
	BasicBlock *bbMerge = BasicBlock::Create(getGlobalContext(), "bbAndExprMerge", kmodjit->func);
	BasicBlock *bbCond = BasicBlock::Create(getGlobalContext(), "bbAndExprCond", kmodjit->func);
	std::vector<BasicBlock*> blocks;
	blocks.push_back(bbCond);
	AND_asm_inner(_ctx, expr, &blocks, bbMerge, 1, size, espidx);
	builder->SetInsertPoint(bbMerge);
	PHINode *phi = builder->CreatePHI(ctype2Type(TY_BOOL), 0, "andExprPhi");
	bool_createPHI(_ctx, &blocks, phi, LLVMBool(0), LLVMBool(1));
}

static Value * LETEXPR_asm(CTX, int a, kExpr *expr, int espidx)
{
	kExpr *exprL = kExpr_at(expr, 1);
	kExpr *exprR = kExpr_at(expr, 2);
	if(exprL->build == TEXPR_LOCAL) {
		Value *v = EXPR_asm(_ctx, exprL->index, exprR, espidx);
		if(a != espidx) {
			return NMOV_asm(_ctx, a, exprL->ty, espidx);
		}
		return v;
	}
	else{
		assert(exprL->build == TEXPR_FIELD);
		Value *v = EXPR_asm(_ctx, espidx, exprR, espidx);
		(void)v;
		abort();
		//kshort_t index = (kshort_t)exprL->index;
		//kshort_t xindex = (kshort_t)(exprL->index >> (sizeof(kshort_t)*8));
		if(TY_isUnbox(exprR->ty)) {
			/* TODO field access*/
			ASM(XNMOV, OC_(index), xindex, NC_(espidx));
			return NULL;
		}
		else {
			/* TODO field access*/
			ASM(XNMOV, OC_(index), xindex, OC_(espidx));
			return NULL;
		}
		if(a != espidx) {
			return NMOV_asm(_ctx, a, exprL->ty, espidx);
		}
	}
	assert(0 && "non-reachable");
	return NULL;
}

/* ------------------------------------------------------------------------ */
/* [LABEL]  */

/* ------------------------------------------------------------------------ */

//static void ASM_SAFEPOINT(CTX, int espidx)
//{
//	(void)espidx;
//	(void)_ctx;
//	//kBasicBlock *bb = kmodjit->curbbNC;
//	//size_t i;
//	//for(i = 0; i < BBSIZE(bb); i++) {
//	//	kopl_t *op = BBOP(bb) + i;
//	//	if(op->opcode == OPCODE_SAFEPOINT) return;
//	//}
//	//ASM(SAFEPOINT, SFP_(espidx));
//}

static void BLOCK_asm(CTX, kBlock *bk);

static void ErrStmt_asm(CTX, kStmt *stmt, int espidx)
{
	kString *msg = (kString*)kObject_getObjectNULL(stmt, 0);
	(void)espidx;
	(void)_ctx;
	DBG_ASSERT(IS_String(msg));
	ASM(ERROR, SFP_(espidx), msg);
}

static void ExprStmt_asm(CTX, kStmt *stmt, int espidx)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, 1);
	if(IS_Expr(expr)) {
		Value *v = EXPR_asm(_ctx, espidx, expr, espidx);
		(void)v;
	}
}

static void BlockStmt_asm(CTX, kStmt *stmt, int espidx)
{
	USING_SUGAR;
	BLOCK_asm(_ctx, kStmt_block(stmt, KW_block, K_NULLBLOCK));
	(void)espidx;
}

static void IfStmt_asm(CTX, kStmt *stmt, int espidx)
{
	USING_SUGAR;
	(void)espidx;(void)_ctx;(void)stmt;
	IRBuilder<> *builder = kmodjit->builder;
	BasicBlock *bbThen = BasicBlock::Create(getGlobalContext(), "bbThen", kmodjit->func);
	BasicBlock *bbElse = BasicBlock::Create(getGlobalContext(), "bbElse", kmodjit->func);
	BasicBlock *bbMerge = BasicBlock::Create(getGlobalContext(), "bbMerge", kmodjit->func);

	/* cond codegen */
	Value *cond = EXPR_asm(_ctx, espidx, kStmt_expr(stmt, 1, NULL), espidx);
	builder->CreateCondBr(cond, bbThen, bbElse);

	/* bbThen codegen */
	builder->SetInsertPoint(bbThen);
	BLOCK_asm(_ctx, kStmt_block(stmt, KW_block, K_NULLBLOCK));
	//bbThen = builder->GetInsertBlock();
	if (builder->GetInsertBlock()->getTerminator() == NULL) {
		builder->CreateBr(bbMerge);
	}

	/* bbElse codegen */
	builder->SetInsertPoint(bbElse);
	BLOCK_asm(_ctx, kStmt_block(stmt, KW_else, K_NULLBLOCK));
	//bbElse = builder->GetInsertBlock();
	if (builder->GetInsertBlock()->getTerminator() == NULL) {
		builder->CreateBr(bbMerge);
	}

	/* TODO PHI Node */

	/* bbMerge codegen */
	builder->SetInsertPoint(bbMerge);

	return;
}

static void ReturnStmt_asm(CTX, kStmt *stmt, int espidx)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, 1);
	IRBuilder<> *builder = get_builder(_ctx);
	if(IS_Expr(expr) && expr->ty != TY_void) {
		Value *retval = EXPR_asm(_ctx, K_RTNIDX, expr, espidx);
		Function *f = get_function(_ctx);
		Type *retTy = f->getReturnType();
		if (isBoxTy(_ctx, type2cid(_ctx, retTy)) && isUnBoxTy(_ctx, type2cid(_ctx, retval->getType()))) {
			Value *boxed = boxing(_ctx, retval);
			builder->CreateRet(boxed);
		} else {
			builder->CreateRet(retval);
		}
	} else if (expr->ty == TY_void) {
		builder->CreateRetVoid();
	}
}

static void LoopStmt_asm(CTX, kStmt *stmt, int espidx)
{
	(void)_ctx;(void)stmt;(void)espidx;
	//USING_SUGAR;
	//kBasicBlock* lbCONTINUE = new_BasicBlockLABEL(_ctx);
	//kBasicBlock* lbBREAK = new_BasicBlockLABEL(_ctx);
	//BUILD_pushLABEL(_ctx, stmt, lbCONTINUE, lbBREAK);
	//ASM_LABEL(_ctx, lbCONTINUE);
	//ASM_SAFEPOINT(_ctx, espidx);
	//EXPR_asmJMPIF(_ctx, espidx, kStmt_expr(stmt, 1, NULL), 0/*FALSE*/, lbBREAK, espidx);
	//BLOCK_asm(_ctx, kStmt_block(stmt, KW_block, K_NULLBLOCK));
	//ASM_JMP(_ctx, lbCONTINUE);
	//ASM_LABEL(_ctx, lbBREAK);
	//BUILD_popLABEL(_ctx);
}

static void UndefinedStmt_asm(CTX, kStmt *stmt, int espidx)
{
	(void)_ctx;(void)espidx;
	DBG_P("undefined asm syntax='%s'", stmt->syn->token);
}

static void BLOCK_asm2(CTX, kBlock *bk, int i, int espidx)
{
	if (i < (int)kArray_size(bk->blockS)) {
		kStmt *stmt = bk->blockS->stmts[i];
		kmodjit->uline = stmt->uline;
		if(stmt->syn != NULL) {;
			if (stmt->build == TSTMT_ERR) {
				ErrStmt_asm(_ctx, stmt, espidx);
				return;
			}
			if (stmt->build == TSTMT_EXPR) {
				ExprStmt_asm(_ctx, stmt, espidx);
			}
			if (stmt->build == TSTMT_BLOCK) {
				BlockStmt_asm(_ctx, stmt, espidx);
			}
			if (stmt->build == TSTMT_RETURN) {
				ReturnStmt_asm(_ctx, stmt, espidx);
				return;
			}
			if (stmt->build == TSTMT_IF) {
				IfStmt_asm(_ctx, stmt, espidx);
			}
			if (stmt->build == TSTMT_LOOP) {
				LoopStmt_asm(_ctx, stmt, espidx);
			} else {
				UndefinedStmt_asm(_ctx, stmt, espidx);
			}
		}
		BLOCK_asm2(_ctx, bk, i+1, espidx);
	}
}
static void BLOCK_asm(CTX, kBlock *bk)
{
	int espidx = bk->esp->index;
	BLOCK_asm2(_ctx, bk, 0, espidx);
}

/* ------------------------------------------------------------------------ */

static void createType(std::vector<Type *> &fieldsTy, konoha_object_data *data)
{
	konoha_object_data *p = data;
	if (p->field_name != NULL) {
		fieldsTy.push_back(ctype2Type(p->type));
		createType(fieldsTy, p+1);
	}
}
static Type *createStructType(Module *m, const char *name, konoha_object_data *data, bool needPtr = false)
{
	std::vector<Type *> fieldsTy;
	createType(fieldsTy, data);
	StructType *structTy = StructType::create(m->getContext(), fieldsTy, name, false);
	if (needPtr) {
		return structTy;
	}
	PointerType *ptrTy = PointerType::get(structTy, 0);
	return ptrTy;
}

static FunctionType *createFunctionType(Type *retTy, std::vector<Type *> &argsTy)
{
	FunctionType *fnTy = FunctionType::get(retTy, argsTy, false);
	return fnTy;
}

static void construct_args(CTX, std::vector<Type *> &argsTy, kParam *pa, int n, int psize)
{
	if (n < psize) {
		argsTy.push_back(cid2Type(_ctx, pa->p[n].ty));
		construct_args(_ctx, argsTy, pa, n+1, psize);
	}
}

static std::string method2string(CTX, kMethod *mtd)
{
	kcid_t cid = mtd->cid;
	kmethodn_t mn = mtd->mn;
	char mbuf[128];
	std::string cname(T_cid(cid));
	std::string fname(T_mn(mbuf, mn));
	return cname + "_" + fname;
}

static Value *loadsfp(CTX, IRBuilder<> *builder, Value *vsfp, ktype_t reqtype, int sfpidx)
{
	unsigned int idx = tri(TY_isUnbox(reqtype), 1, 0);
	Value *v = builder->CreateConstGEP2_32(vsfp, sfpidx, idx, "gep");
	if (reqtype != TY_Boolean && !(IS_defineFloat() && reqtype == TY_Float)) {
		return v;
	}
	return builder->CreateBitCast(v, PointerType::get(cid2Type(_ctx, reqtype), 0), "cast");
}

static void push_back_params(CTX, std::vector<Value*> &params, IRBuilder<> *builder, Value *vsfp, kParam *pa, int i)
{
	if ((unsigned)i < pa->psize) {
		Value *v = loadsfp(_ctx, builder, vsfp, pa->p[i].ty, i+1);
		params.push_back(builder->CreateLoad(v, "load"));
		push_back_params(_ctx, params, builder, vsfp, pa, i+1);
	}
}

static Function *build_wrapper_func(CTX, kMethod *mtd, Module *m, Function *f)
{
	kParam *pa = mtd->pa;
	ktype_t retTy = pa->rtype;
	std::string name = method2string(_ctx, mtd) + "_wrapper";
	FunctionType *fnTy = cast<FunctionType>(LLVMTYPE_FUNCTION);
	Function *wrap = cast<Function>(m->getOrInsertFunction(name, fnTy));
	BasicBlock *bb = BasicBlock::Create(getGlobalContext(), "EntryBlock", wrap);
	IRBuilder<> *builder = new IRBuilder<>(bb);
	Function::arg_iterator args = wrap->arg_begin();
	Value *arg_ctx = args++;
	Value *arg_sfp = args++;
	arg_ctx->setName("ctx");
	arg_sfp->setName("sfp");
	args->setName("rix");

	builder->SetInsertPoint(bb);
	/* load konoha args from sfp and build call params*/
	std::vector<Value*> params;
	params.push_back(arg_ctx);
	params.push_back(arg_sfp);
	push_back_params(_ctx, params, builder, arg_sfp, pa, 0);

	/* call function and  set return values */
	Value *callinst = builder->CreateCall(f, params);
	if (retTy != TY_void) {
		Value *v = loadsfp(_ctx, builder, arg_sfp, retTy, K_RTNIDX);
		builder->CreateStore(callinst, v, false/*isVolatile*/);
	}
	builder->CreateRetVoid();
	delete builder;
	return wrap;
}

static void function_set_params(CTX, Module *m, std::vector<Value *> &args, kParam *pa, int i, int n)
{
	if (i < n) {
		int reg = i+1;
		int idx = tri(TY_isUnbox(pa->p[i].ty), NC_(reg), OC_(reg));
		Value *v = args[i];
		fprintf(stderr, "PARAM %d:%p\n", idx, (void*)v);
		vstack_setValue(_ctx, idx, v);
		function_set_params(_ctx, m, args, pa, i+1, n);
	}
}

static void push_back_params(CTX, std::vector<Value*> &params, Function::arg_iterator itr, int i, int n)
{
	if (i < n) {
		Value *v = itr++;
		params.push_back(v);
		push_back_params(_ctx, params, itr, i+1, n);
	}
}

Function *create_function(CTX, Module *m, kMethod *mtd)
{
	int psize = mtd->pa->psize;
	std::vector<Type *> argsTy;
	argsTy.push_back(ctype2Type(TY_CONTEXT));
	argsTy.push_back(ctype2Type(TY_STACK));
	construct_args(_ctx, argsTy, mtd->pa, 0, psize);
	FunctionType *fnTy = createFunctionType(cid2Type(_ctx, mtd->pa->rtype), argsTy);
	std::string fname = method2string(_ctx, mtd);
	Function *f = Function::Create(fnTy, GlobalValue::ExternalLinkage, fname, m);

	Function::arg_iterator args = f->arg_begin();
	kmodjit->vctx = args++;
	kmodjit->vsfp = args++;

	std::vector<Value *> a;
	push_back_params(_ctx, a, args, 0, psize);
	function_set_params(_ctx, m, a, mtd->pa, 0, psize);

	return f;
}

static void jit_genCode(CTX, kMethod *mtd, kBlock *bk)
{
	DBG_P("START CODE GENERATION..", "");
	Module *m = kmodjit->global_module;
	Function *f = create_function(_ctx, m, mtd);
	jitcache_set(_ctx, mtd, f);
	kmodjit->func = f;
	assert(kmodjit->builder == NULL);
	BasicBlock *bb = BasicBlock::Create(getGlobalContext(), "BB", f);
	IRBuilder<> *builder = new IRBuilder<>(bb);
	kmodjit->builder = builder;
	BLOCK_asm(_ctx, bk);
	if (builder->GetInsertBlock()->getTerminator() == NULL) {
		ktype_t rtype = mtd->pa->rtype;
		if (rtype == TY_void) {
			builder->CreateRetVoid();
		} else {
			builder->CreateRet(createConstValue(cid2ctype(_ctx, rtype), (void*)knull(CT_(rtype))));
		}
	}
	Function *wrap = build_wrapper_func(_ctx, mtd, m, f);
	kmodjit->global_module->dump();

	FunctionPassManager pm(m);
	pm.add(createVerifierPass());
	pm.doInitialization();
	pm.run(*f);
	pm.run(*wrap);

	knh_Fmethod fptr = bit_cast<knh_Fmethod>(kmodjit->global_ee->getPointerToFunction(wrap));
	assert(fptr != NULL);
	kMethod_setFunc(mtd, fptr);
	delete kmodjit->builder;
	ValueStack_clear(kmodjit->stack);
	kmodjit->builder = NULL;
}

/* ------------------------------------------------------------------------ */
static const konoha_object_data _ObjectHeader[] = {
	{TY_LONG,  "magicflag"},
	{TY_CLASS, "ct"},
	{TY_LONG,  "hash"},
	{TY_PTR,   "kvproto"},
	{TY_NOP, NULL}
};

static const konoha_object_data _Object[] = {
	{TY_HOBJECT,  "h"},
	{TY_PTR,   "field0"},
	{TY_PTR,   "field1"},
	{TY_PTR,   "field2"},
	{TY_PTR,   "field3"},
	{TY_NOP, NULL}
};

static const konoha_object_data _Stack[] = {
	{TY_OBJECT,"o"},
	{TY_LONG,  "ndata"},
	{TY_NOP, NULL}
};

static const konoha_object_data _Context[] = {
	{TY_INT,   "safepoint"},
	{TY_STACK, "esp"},
	{TY_PTR,   "memshare"},
	{TY_PTR,   "memlocal"},
	{TY_PTR,   "share"},
	{TY_PTR,   "local"},
	{TY_PTR,   "logger"},
	{TY_PTR,   "modshare"},
	{TY_PTR,   "modlocal"},
	{TY_NOP, NULL}
};

static const konoha_object_data _Method[] = {
	{TY_HOBJECT,  "h"},
	{TY_FPTR,    "fcall_1"},
	{TY_PTR,     "pc_start"},
	{TY_LONG,    "flag"},
	{TY_SHORT,   "cid"},
	{TY_SHORT,   "mn"},
	{TY_SHORT,   "delta"},
	{TY_SHORT,   "packid"},
	{TY_OBJECT,  "pa"},
	{TY_OBJECT,  "tcode"},
	{TY_OBJECT,  "objdata"},
	{TY_OBJECT,  "proceedNUL"},
	{TY_NOP, NULL}
};

static void kmodjit_setup(CTX, struct kmodshare_t *def, int newctx)
{
	(void)_ctx;(void)def;(void)newctx;
}

static void kmodjit_reftrace(CTX, struct kmodshare_t *baseh)
{
	(void)_ctx;(void)baseh;
}

static void kmodjit_free(CTX, struct kmodshare_t *baseh)
{
	kmodjit_t *modshare = (kmodjit_t*) baseh;
	ValueStack_free(_ctx, modshare->stack);
	kmap_free(modshare->jitcache, NULL);
	//delete modshare->global_module;
	//delete modshare->global_ee;
	KNH_FREE(baseh, sizeof(kmodjit_t));
}

static kbool_t kmodjit_init(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	(void)ks;(void)argc;(void)args;(void)pline;
	InitializeNativeTarget();
	kmodjit_t *base = (kmodjit_t*)KNH_ZMALLOC(sizeof(*base));
	base->h.name     = "llvmjit";
	base->h.setup    = kmodjit_setup;
	base->h.reftrace = kmodjit_reftrace;
	base->h.free     = kmodjit_free;

	InitializeNativeTarget();
	Module *m = new Module("test", getGlobalContext());
	ExecutionEngine *ee = EngineBuilder(m).setEngineKind(EngineKind::JIT).create();
	LLVMTYPE_HOBJECT = createStructType(m, "ObjectHeader", const_cast<konoha_object_data*>(_ObjectHeader), true);
	LLVMTYPE_OBJECT  = createStructType(m, "Object", const_cast<konoha_object_data*>(_Object), false);
	LLVMTYPE_STACK   = createStructType(m, "sfp", const_cast<konoha_object_data*>(_Stack), false);
	LLVMTYPE_CONTEXT = createStructType(m, "context", const_cast<konoha_object_data*>(_Context), false);

	std::vector<Type *> argsTy;
	argsTy.push_back(LLVMTYPE_CONTEXT);
	argsTy.push_back(LLVMTYPE_STACK);
#if 1/* We use K_RIX */
	argsTy.push_back(ctype2Type(TY_LONG));
#endif
	LLVMTYPE_FUNCTION  = createFunctionType(ctype2Type(TY_VOID), argsTy);
	LLVMTYPE_FUNCTIONP = PointerType::get(LLVMTYPE_FUNCTION, 0);

	LLVMTYPE_METHOD  = createStructType(m, "Method", const_cast<konoha_object_data*>(_Method), false);
	base->global_module = m;
	base->global_ee = ee;
	base->stack = ValueStack_new(_ctx, 4);
	base->jitcache = kmap_init(0);
	Konoha_setModule(MOD_jit, &base->h, 0);
	{
		struct _klib2 *l = (struct _klib2*)_ctx->lib2;
		l->KMethod_genCode = jit_genCode;
	}
	MN_opNEG  = MN_("opNEG");
	MN_opADD  = MN_("opADD");
	MN_opSUB  = MN_("opSUB");
	MN_opMUL  = MN_("opMNL");
	MN_opDIV  = MN_("opDIV");
	MN_opMOD  = MN_("opMOD");
	MN_opEQ   = MN_("opEQ");
	MN_opNE   = MN_("opNEQ");
	MN_opLT   = MN_("opLT");
	MN_opLTE  = MN_("opLTE");
	MN_opGT   = MN_("opGT");
	MN_opGTE  = MN_("opGTE");
	MN_opLAND = MN_("opLAND");
	MN_opLOR  = MN_("opLOR");
	MN_opLXOR = MN_("opLXOR");
	MN_opLSFT = MN_("opLSFT");
	MN_opRSFT = MN_("opRSFT");
	return true;
}

} /* namespace konoha */

#ifdef __cplusplus
extern "C" {
#endif

kbool_t llvm_jit_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	return konoha::kmodjit_init(_ctx, ks, argc, args, pline);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
