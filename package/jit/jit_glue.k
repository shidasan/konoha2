K.importPackage("sugar");
K.importPackage("konoha.float");
K.importPackage("konoha.llvm");
K.importPackage("konoha.assignment");
K.importPackage("konoha.string");
K.importPackage("native");

//enum ctype {
//	/*TY_VOID*/1,
//	/*TY_LONG*/2,
//	/*TY_INT*/3,
//	/*TY_SHORT*/4,
//	/*TY_FLOAT*/5,
//	/*TY_PTR*/6,
//	/*TY_CHAR*/7,
//	/*TY_BOOL*/8,
//	/*TY_STRING*/9,
//	/*TY_CLASS*/10,
//	/*TY_METHOD*/11,
//	/*TY_OBJECT*/12,
//	/*TY_HOBJECT*/13,
//	/*TY_FUNCTION*/14,
//	/*TY_FPTR*/15,
//	/*TY_CONTEXT*/16,
//	/*TY_STACK*/17,
//	/*TY_NOP*/-1 = -1
//};

int Expr.getBuild() {
	int x = this.getAddr();
	return Pointer.get(x, 36, 16);
}

Object Expr.getData() {
	int x = this.getAddr();
	int addr = Pointer.get(x, 48, 64);
	return Pointer.convertToObject(addr);
}

int Expr.getIndex() {
	int x = this.getAddr();
	return Pointer.get(x, 56, 64);
}

int Expr.getNdata() {
	int x = this.getAddr();
	return Pointer.get(x, 56, 64);
}

int Expr.getType() {
	int x = this.getAddr();
	return Pointer.get(x, 32, 16);
}

Array Expr.getCons() {
	int x = this.getAddr();
	int ptr = Pointer.get(x, 48, 64);
	return Pointer.convertToArray(ptr);
}
Method Object.toMethod() {
	int ptr = this.getAddr();
	return Pointer.convertToMethod(ptr);
}

int Method.getCid() {
	int x = this.getAddr();
	return Pointer.get(x, 56, 16);
}

int Object.getCid() {
	int x = this.getAddr();
	int ct = Pointer.get(x, 8, 64);
	int cid = Pointer.get(ct, 88, 16);
	return cid;
}

Expr Expr.at(int i) {
	Array a = this.getCons();
	return a.get(i);
}

void Method.setFunc(int faddr) {
	int x = this.getAddr();
	Pointer.set(x, 32, 64, faddr);
}


void vstack_setValue(int idx, Value value) {
	if (idx < 0) {
		System.setRetVal(value);
	} else {
		System.setValue(idx, value);
	}
}

Type ctype2Type(int ctype) {
	if (ctype == /*TY_LONG*/2) {
		//TODO
		return Type.getInt64Ty();
	}
	if (ctype == /*TY_INT*/3) {
		return Type.getInt64Ty();
	}
	if (ctype == /*TY_FLOAT*/5) {
		//TODO
		return Type.getDoubleTy();
	}
	if (ctype == /*TY_SHORT*/4) {
		//TODO
		return Type.getInt16Ty();
	}
	if (ctype == /*TY_PTR*/6) {
		return Type.getInt8PtrTy();
	}
	if (ctype == /*TY_CHAR*/7) {
		return Type.getInt8Ty();
	}
	if (ctype == /*TY_BOOL*/8) {
		return Type.getInt1Ty();
	}
	if (ctype == /*TY_STRING*/9) {
		return Type.getInt8PtrTy();
	}
	if (ctype == /*TY_CLASS*/10) {
		//TODO
		return Type.getInt8PtrTy();
	}
	if (ctype == /*TY_OBJECT*/12) {
		return System.getTyObject();
	}
	if (ctype == /*TY_HOBJECT*/13) {
		return System.getTyHObject();
	}
	if (ctype == /*TY_METHOD*/11) {
		return System.getTyMethod();
	}
	if (ctype == /*TY_FUNCTION*/14) {
		return System.getTyFunction();
	}
	if (ctype == /*TY_FPTR*/15) {
		return System.getTyFunctionP();
	}
	if (ctype == /*TY_CONTEXT*/16) {
		return System.getTyContext();
	}
	if (ctype == /*TY_STACK*/17) {
		return System.getTyStack();
	}
	if (ctype == /*TY_VOID*/1) {
		return Type.getVoidTy();
	}
	System.p("[WARN] ctype not matched");
	return Type.getVoidTy();
}

Value createConstValue(int ctype, Object v, int i, float f) {
	if (ctype == /*TY_INT*/3) {
		return ConstantInt.getValue(ctype2Type(ctype), i);
	}
	if (ctype == /*TY_BOOL*/8) {
		return ConstantInt.getValue(ctype2Type(ctype), i);
	}
	if (ctype == /*TY_FLOAT*/5) {
		return ConstantFP.getValue(ctype2Type(ctype), f);
	}
	if (ctype == /*TY_OBJECT*/12) {
		//assert(LLVMTYPE_OBJECT != null);
		Constant val = ConstantInt.getValue(ctype2Type(/*TY_LONG*/2), v.getAddr());
		return ConstantExpr.getIntToPtr(val, ctype2Type(ctype));
	}
	if (ctype == /*TY_METHOD*/11) {
		//assert(LLVMTYPE_OBJECT != null);
		Constant val = ConstantInt.getValue(ctype2Type(/*TY_LONG*/2), v.getAddr());
		return ConstantExpr.getIntToPtr(val, ctype2Type(ctype));
	}
	if (ctype == /*TY_FUNCTION*/14) {
		//assert(LLVMTYPE_FUNCTION != null);
		Constant val = ConstantInt.getValue(ctype2Type(/*TY_LONG*/2), v.getAddr());
		return ConstantExpr.getIntToPtr(val, PointerType.get(ctype2Type(ctype)));
	}
	if (ctype == /*TY_FPTR*/15) {
		//assert(LLVMTYPE_FUNCTIONP != null);
		Constant val = ConstantInt.getValue(ctype2Type(/*TY_LONG*/2), v.getAddr());
		return ConstantExpr.getIntToPtr(val, PointerType.get(ctype2Type(ctype)));
	}
	System.p("createConstValue: [WARN]");
	return null;
}

int cid2ctype(int type) {
	if (type == /*TY_Int*/4)     { return /*TY_INT*/3; }
	if (type == /*TY_Boolean*/3) { return /*TY_BOOL*/8; }
	if (type == /*TY_Float*/-10/*TODO*/) {
		return /*TY_FLOAT*/5;
	}
	if (type == /*TY_void*/0) { return /*TY_VOID*/1; }
	return /*TY_OBJECT*/12;
}

Type cid2Type(int type) {
	return ctype2Type(cid2ctype(type));
}

int type2cid(Type type) {
	if (type == ctype2Type(/*TY_INT*/3))   { return /*TY_Int*/4; }
	if (type == ctype2Type(/*TY_BOOL*/8))  { return /*TY_Boolean*/3; }
	if (type == ctype2Type(/*TY_FLOAT*/5)) {
		return /*TY_Float*/-10/*TODO*/;
	}
	if (type == ctype2Type(/*TY_VOID*/1)) { return /*TY_void*/0; }
	return /*TY_Object*/2;
}

boolean ty_isUnbox(int cid) {
	if (cid == /*TY_Int*/4) { return true; }
	if (cid == /*TY_Boolean*/3) { return true; }
	if (cid == /*TY_Float*/-10/*TODO*/) { return true; }
	return false;
}

boolean isUnBoxTy(int cid) {
	return ty_isUnbox(cid);
}

boolean isBoxTy(int cid) {
	if (isUnBoxTy(cid)) {
		return false;
	} else {
		return true;
	}
}

Value getvctx() {
	Value v = System.getCTX();
	Type  t = v.getType();
	return v;
}
Value getvsfp() {
	Value v = System.getSFP();
	Type  t = v.getType();
	return v;
}

Value boxing(Value v) {
	Module m = System.getModule();
	IRBuilder builder = System.getBuilder();
	Array argsTy = Array.newN(3);
	argsTy.set(0, ctype2Type(/*TY_CONTEXT*/16));
	argsTy.set(1, ctype2Type(/*TY_INT*/3));
	argsTy.set(2, ctype2Type(/*TY_INT*/3));
	FunctionType fnTy = FunctionType.get(ctype2Type(/*TY_OBJECT*/12), argsTy, false);
	Function box = m.getOrInsertFunction("boxing_jit", fnTy);
	Array args = box.getArguments();

	int cid = type2cid(v.getType());
	Array params = Array.newN(3);
	params.set(0, getvctx());
	params.set(1, createConstValue(cid2ctype(cid), null, cid, 0.0));
	if (v.getType() == ctype2Type(/*TY_INT*/3)) {
		params.set(2, v);
	} else {
		//TODO float type
		Value v0 = builder.createZExt(v, ctype2Type(/*TY_LONG*/2));
		params.set(2, v0);
	}

	Value ret = builder.createCall(box, params);
	return ret;
}

Value llvm_getBool(boolean v) {
	if (v) {
		return ConstantInt.getValue(ctype2Type(/*TY_BOOL*/8), 1);
	} else {
		return ConstantInt.getValue(ctype2Type(/*TY_BOOL*/8), 0);
	}
}

Value llvm_getInt(int v) {
    return (ConstantInt.getValue(ctype2Type(/*TY_INT*/3),    (v)));
}

Value llvm_getFloat(float v) {
    return (ConstantInt.getValue(ctype2Type(/*TY_FLOAT*/5),  (v)));
}


//#define nc(sfpidx)    (((sfpidx) * 2) + 1)
//#define oc(sfpidx)    ((sfpidx) * 2)
//#define sfp(sfpidx)   ((sfpidx) * 2)
//#define rfp(rbpidx)   ((sfpidx) / 2)
int nc(int sfpidx)  { return (sfpidx); }
int oc(int sfpidx)  { return (sfpidx); }
int sfp(int sfpidx) { return (sfpidx); }
int rfp(int rbpidx) { return (rbpidx); }

int esp(int sfpidx, int args) { return  sfp(sfpidx + args + /*K_CALLDELTA*/4 + 1)}

/* ------------------------------------------------------------------------ */
Value create_object(int a, int index, int cid) {
	Module m = System.getModule();
	IRBuilder builder = System.getBuilder();
	Array argsTy = Array.newN(2);
	argsTy.set(0, ctype2Type(/*TY_CONTEXT*/16));
	argsTy.set(1, ctype2Type(/*TY_INT*/3));
	FunctionType fnTy = FunctionType.get(ctype2Type(/*TY_OBJECT*/12), argsTy, false);
	Function fnew = m.getOrInsertFunction("new_object_jit", fnTy);
	Array args = fnew.getArguments();

	Array params = Array.newN(2);
	Value vctx = getvctx();
	Value arg1 = createConstValue(/*TY_INT*/3, null, cid, 0.0);
	arg1.dump();
	params.set(0, vctx);
	params.set(1, arg1);
	Value ret = builder.createCall(fnew, params);
	return ret;
}

int tri(boolean cond, int a, int b) {
	if (cond) {
		return a;
	} else {
		return b;
	}
}

Value nmov_asm(int a, int ty, int b) {
	if(ty_isUnbox(ty)) {
		Value v = System.getValue(nc(b));
		vstack_setValue(nc(a), v);
		//ASM(NMOV, nc(a), nc(b), CT_(ty));
		return v;
	}
	else {
		Value v = System.getValue(oc(b));
		vstack_setValue(oc(a), v);
		return v;
	}
}

Value asm_nset(int data, int cid, int a) {
	Value v = createConstValue(cid2ctype(cid), null, data, 0.0);
	int idx = nc(a);
	vstack_setValue(idx, v);
	return v;
}

Value asm_oset(Object data, int cid, int a) {
	Value v = createConstValue(cid2ctype(cid), data, 0, 0.0);
	int idx = oc(a);
	vstack_setValue(idx, v);
	return v;
}

Value expr_asm(int a, Expr expr, int espidx) {
	//System.p("build="+expr.getBuild()+":"+expr);
	if (expr.getBuild() == /*TEXPR_CONST*/0) {
		Object v = expr.getData();
		boolean isUnbox = ty_isUnbox(expr.getType());
		if(isUnbox) {
			//ASM(NSET, nc(a), (uintptr_t)N_toint(v), CT_(expr.getType()));
			System.p("FIXME TODO");
			return asm_nset(v.getAddr(), expr.getType(), a);
		}
		else {
			int addr = v.getAddr();
			System.addConstPool(v);
			return asm_oset(v, expr.getType(), a);
		}
	}
	else if(expr.getBuild() == /*TEXPR_NEW*/1) {
		int index = expr.getIndex();
		int type  = expr.getType();
		//ASM(NEW, oc(a), expr.index, CT_(expr.getType()));
		return create_object(a, index, type);
	}
	else if(expr.getBuild() == /*TEXPR_NULL*/2) {
		boolean isUnbox = ty_isUnbox(expr.getType());
		if(isUnbox) {
			return asm_nset(0, (expr.getType()), a);
		}
		else {
			return asm_oset(System.knull(expr.getType()), expr.getType(), a);
		}
	}
	else if(expr.getBuild() == /*TEXPR_NCONST*/3) {
		return asm_nset(expr.getNdata(), expr.getType(), a);
		//ASM(NSET, nc(a), expr.ndata, CT_(expr.getType()));
	}
	else if(expr.getBuild() == /*TEXPR_LOCAL*/4) {
		//System.p("type="+expr.getType());
		return nmov_asm(a, expr.getType(), expr.getIndex());
	}
	else if(expr.getBuild() == /*TEXPR_BLOCK*/5) {
		assert(false);
	}
	else if(expr.getBuild() == /*TEXPR_FIELD*/6) {
		assert(false);
		//kshort_t index = (kshort_t)expr.index;
		//kshort_t xindex = (kshort_t)(expr.index >> (sizeof(kshort_t)*8));
		//if(ty_isUnbox(expr.getType())) {
		//	ASM(NMOVx, nc(a), oc(index), xindex, CT_(expr.getType()));
		//}
		//else {
		//	ASM(NMOVx, oc(a), oc(index), xindex, CT_(expr.getType()));
		//}
	}
	else if(expr.getBuild() == /*TEXPR_BOX*/7) {
		Expr single = expr.getSingle();
		Value v = expr_asm(a, single, espidx);
		return boxing(v);
	}
	else if(expr.getBuild() == /*TEXPR_UNBOX*/8) {
		System.p("TODO");
		assert(false);
		//ASM(UNBOX, nc(a), oc(a), CT_(expr.getType()));
	}
	else if(expr.getBuild() == /*TEXPR_CALL*/9) {
		Value v = call_asm(a, expr, espidx);
		if(a != espidx) {
			return nmov_asm(a, expr.getType(), espidx);
		}
		return v;
	}
	else if(expr.getBuild() == /*TEXPR_AND*/10) {
		and_asm(a, expr, espidx);
	}
	else if(expr.getBuild() == /*TEXPR_OR*/11) {
		or_asm(a, expr, espidx);
	}
	else if(expr.getBuild() == /*TEXPR_LET*/12) {
		return letexpr_asm(a, expr, espidx);
	} else {
		System.p("unknown expr="+ expr.getBuild());
		System.assert(false);
	}
	//abort();
}

void storesfp(Value val, int reqtype, int sfpidx) {
	IRBuilder builder = System.getBuilder();
	Value vsfp  = getvsfp();
	Value vsfp0 = loadsfp(builder, vsfp, reqtype, sfpidx);
	builder.createStore(val, vsfp0, false);
}

void emit_param(Method mtd, Expr expr, int thisidx, int i) {
	Array cons = expr.getCons();
	Method orig_mtd = mtd;
	int size = cons.getSize();
	int idx = thisidx + i - 1;
	if (i < cons.getSize()) {
		Expr exprN = expr.at(i);
		Value v = expr_asm(idx, exprN, idx);
		storesfp(v, exprN.getType(), idx);
		emit_param(orig_mtd, expr, thisidx, i+1);
	}
}

void asm_shift_esp(int idx) {
	IRBuilder builder = System.getBuilder();
	Value vesp = builder.createStructGEP(getvctx(), 1); /* esp */
	Value vsfp = builder.createConstInBoundsGEP1_32(getvsfp(), idx);
	builder.createStore(vsfp, vesp, false);
}

Value loadMtdFromSfp(IRBuilder builder, Value vsfp, int mtdidx) {
	Value v = loadsfp(builder, vsfp, /*TY_Int*/4, mtdidx);
	return builder.createBitCast(v, PointerType.get(ctype2Type(/*TY_METHOD*/11)));
}

void prepare_call_stack(int thisidx, Value vsfp, Object def, Method mtd, int uline) {
	int rtnidx = thisidx + /*K_RTNIDX*/-4;
	int mtdidx = thisidx + /*K_MTDIDX*/-1;
	IRBuilder builder = System.getBuilder();
	if ((def).getCid() != /*TY_void*/0) {
		Value vsfp0 = loadsfp(builder, vsfp, (def).getCid(), rtnidx);
		if (ty_isUnbox(def.getCid())) {
			int ndata = def.unbox();
			Value vdef  = createConstValue(cid2ctype(def.getCid()), null, ndata, 0);
			builder.createStore(vdef, vsfp0, false);
		} else {
			Value vdef  = createConstValue(/*TY_OBJECT*/12, (def), 0, 0);
			builder.createStore(vdef, vsfp0, false);
		}
	}
	Value vsfp1 = loadMtdFromSfp(builder, vsfp, mtdidx);
	Value vmtd  = createConstValue(/*TY_METHOD*/11, (mtd), 0, 0);
	builder.createStore(vmtd, vsfp1, false);

	Value vsfp2 = loadsfp(builder, vsfp, /*TY_Int*/4, rtnidx);
	Value vline = createConstValue(/*TY_INT*/3, null, uline, 0);
	builder.createStore(vline, vsfp2, false);

}

void param_setCtxSfp(Array params, int sfpidx) {
	IRBuilder builder = System.getBuilder();
	Function f = System.getFunction();
	Array args = f.getArguments();
	Value vctx = args.get(0);
	Value vsfp = args.get(1);

	params.set(0, vctx);
	if (sfpidx > 0) {
		Array arg0 = Array.new1(llvm_getInt(sfpidx));
		Value vsfp1 = builder.createGEP(vsfp, arg0);
		params.set(1, vsfp1);
	} else {
		params.set(1, vsfp);
	}
}

boolean check_asm_int_op(String mn, int argc) {
	if (argc == 0) {
		if(mn == "opNEG") {
			return true;
		}
	}
	if (argc == 1) {
		if(mn == "opADD" ) { return true; }
		if(mn == "opSUB" ) { return true; }
		if(mn == "opMUL" ) { return true; }
		if(mn == "opDIV" ) { return true; }
		if(mn == "opMOD" ) { return true; }
		if(mn == "opEQ"  ) { return true; }
		if(mn == "opNE"  ) { return true; }
		if(mn == "opLT"  ) { return true; }
		if(mn == "opLTE" ) { return true; }
		if(mn == "opGT"  ) { return true; }
		if(mn == "opGTE" ) { return true; }
		if(mn == "opLAND") { return true; }
		if(mn == "opLOR" ) { return true; }
		if(mn == "opLXOR") { return true; }
		if(mn == "opLSFT") { return true; }
		if(mn == "opRSFT") { return true; }
	}
	return false;
}

Value asm_int_op(String mn, Expr exprN, int local) {
	IRBuilder builder = System.getBuilder();
	if(mn == "opNEG") {
		Value va = expr_asm(local+0, exprN.at(1), local+0);
		return builder.createNeg(va);
	} else {
		Value va = expr_asm(local+0, exprN.at(1), local+0);
		Value vb = expr_asm(local+1, exprN.at(2), local+1);
		if(mn == "opADD" ) { return builder.createAdd(va, vb);}
		if(mn == "opSUB" ) { return builder.createSub(va, vb);}
		if(mn == "opMUL" ) { return builder.createMul(va, vb);}
		if(mn == "opDIV" ) { return builder.createSDiv(va, vb);}
		if(mn == "opMOD" ) { return builder.createSRem(va, vb);}
		if(mn == "opEQ"  ) { return builder.createICmpEQ(va, vb);}
		if(mn == "opNE"  ) { return builder.createICmpNE(va, vb);}
		if(mn == "opLT"  ) { return builder.createICmpSLT(va, vb);}
		if(mn == "opLTE" ) { return builder.createICmpSLE(va, vb);}
		if(mn == "opGT"  ) { return builder.createICmpSGT(va, vb);}
		if(mn == "opGTE" ) { return builder.createICmpSGE(va, vb);}
		if(mn == "opLAND") { return builder.createAnd( va, vb);}
		if(mn == "opLOR" ) { return builder.createOr(va, vb);}
		if(mn == "opLXOR") { return builder.createXor(va, vb);}
		if(mn == "opLSFT") { return builder.createShl(va, vb);}
		if(mn == "opRSFT") { return builder.createAShr(va, vb);}
	}
	return null;
}

void push_back_args(Array args, Expr expr, int local, int s, int i, int n) {
	if (i < n) {
		Value v = expr_asm(local+i, (expr).at(i), local+i);
		args.set(i - s + 2, v);
		push_back_args(args, expr, local, s, i+1, n);
	}
}

Value asm_function_call(int uline, Expr expr, int thisidx, int s, Function f) {
	Array cons = expr.getCons();
	Array args = Array.newN(cons.getSize());
	IRBuilder builder = System.getBuilder();
	args.set(0, getvctx());
	args.set(1, getvsfp());
	push_back_args(args, expr, thisidx+s-1, s, s, cons.getSize());
	return builder.createCall(f, args);
}

void asm_scall(int uline, int sfpidx, int espidx, Method mtd, Object defval) {
	int thisidx = rfp(sfpidx);
	IRBuilder builder = System.getBuilder();
	Value vmtd  = createConstValue(/*TY_METHOD*/11, mtd, 0, 0.0);
	Value funcp = builder.createStructGEP(vmtd, 1);
	Value func  = builder.createLoad(funcp, false);
	prepare_call_stack(thisidx, getvsfp(), defval, mtd, uline);
	Array params = Array.newN(3);
	param_setCtxSfp(params, thisidx);
	params.set(2, llvm_getInt(/*K_RTNIDX*/-4));
	asm_shift_esp(espidx);
	builder.createCall(func, params);
}

Value call_asm(int a, Expr expr, int espidx) {
	Array cons = expr.getCons();
	Method mtd = cons.get(0).toMethod();
	int s = tri(mtd.isStatic(), 2, 1);
	int thisidx = espidx + /*K_CALLDELTA*/4;
	int argc = (cons).getSize() - 2;
	if(mtd.isVirtual()) {
		System.p("TODO");
		assert(false);
		//ASM(NSET, nc(thisidx-1), (intptr_t)mtd, CT_Method);
		//ASM(CALL, kmodjit.uline, sfp(thisidx), esp(espidx, argc), System.knull(expr.getType()));
	}
	else {
		if (mtd.getCid() == /*TY_Int*/4) {
			if (check_asm_int_op(mtd.getFname(), argc)) {
				Value v = asm_int_op(mtd.getFname(), expr, thisidx + s - 1);
				vstack_setValue(nc(espidx), v);
				return v;
			}
		}
		if (mtd.getCid() == /*TY_Float*/-10/*TODO*/) {
		}
		Function f = System.getJITCache(mtd);
		if(f.isNull()) {
			emit_param(mtd, expr, thisidx, s);
			asm_scall(System.getUline(), sfp(thisidx), esp(espidx, argc), mtd, System.knull(expr.getType()));
		}
		else {
			Value v = asm_function_call(System.getUline(), expr, thisidx, s, f);
			if (v.isNull()) {
			} else {
				vstack_setValue(nc(espidx), v);
			}
			return v;
		}
	}
	if (mtd.getReturnType() != /*TY_void*/0) {
		IRBuilder builder = System.getBuilder();
		Value v = loadsfp(builder, getvsfp(), mtd.getReturnType(), sfp(thisidx) + /*K_RTNIDX*/-4);
		Value v1 = builder.createLoad(v, false);
		vstack_setValue(tri(ty_isUnbox(mtd.getReturnType()), nc(espidx), oc(espidx)), v1);
		return v1;
	}
	return null;
}

Object Array.end() {
	Array a = this;
	int size = a.getSize();
	if (size > 0) {
		return a.get(size-1);
	}
	return null;
}
Array Array.erase(int n) {
	System.p("TODO");
	return null;
}


void bool_createPHI(Array blocks, PHINode phi, Value vfirst, Value vlast) {
	LLVMBasicBlock bb = blocks.get(0);
	blocks.erase(0);
	if (blocks.getSize() > 0) {
		phi.addIncoming(vlast, bb);
	} else {
		phi.addIncoming(vfirst, bb);
		bool_createPHI(blocks, phi, vfirst, vlast);
	}
}


void or_asm_inner(Expr expr, Array blocks, LLVMBasicBlock merge, int i, int size, int espidx) {
	IRBuilder builder = System.getBuilder();
	builder.setInsertPoint(blocks.end());
	LLVMBasicBlock next = LLVMBasicBlock.create(System.getFunction(), "bbOrExprCond");
	blocks.set(i, next);
	Value cond = expr_asm(espidx+1, expr.at(i), espidx+1);
	builder.createCondBr(cond, merge, next);
	if (i == size) {
		builder.setInsertPoint(next);
		builder.createBr(merge);
	} else {
		or_asm_inner(expr, blocks, merge, i+1, size, espidx);
	}
}

void or_asm(int a, Expr expr, int espidx) {
	Array cons = expr.getCons();
	int size = (cons).getSize();
	IRBuilder builder = System.getBuilder();
	LLVMBasicBlock bbMerge = LLVMBasicBlock.create(System.getFunction(), "bbOrExprMerge");
	LLVMBasicBlock bbCond = LLVMBasicBlock.create(System.getFunction(), "bbOrExprCond");
	Array blocks = Array.newN(1+size);
	blocks.set(0, bbCond);
	or_asm_inner(expr, blocks, bbMerge, 1, size, espidx);
	builder.setInsertPoint(bbMerge);
	PHINode phi = builder.createPHI(ctype2Type(/*TY_BOOL*/8), 0);
	bool_createPHI(blocks, phi, llvm_getBool(true), llvm_getBool(false));
}

void and_asm_inner(Expr expr, Array blocks, LLVMBasicBlock merge, int i, int size, int espidx) {
	IRBuilder builder = System.getBuilder();
	builder.setInsertPoint(blocks.end());
	LLVMBasicBlock next = LLVMBasicBlock.create(System.getFunction(), "bbAndExprCond");
	blocks.set(i, next);
	Value cond = expr_asm(espidx+1, expr.at(i), espidx+1);
	builder.createCondBr(cond, next, merge);
	if (i == size) {
		builder.setInsertPoint(next);
		builder.createBr(merge);
	} else {
		and_asm_inner(expr, blocks, merge, i+1, size, espidx);
	}
}

void and_asm(int a, Expr expr, int espidx) {
	Array cons = expr.getCons();
	int size = (cons).getSize();
	IRBuilder builder = System.getBuilder();
	LLVMBasicBlock bbMerge = LLVMBasicBlock.create(System.getFunction(), "bbAndExprMerge");
	LLVMBasicBlock bbCond = LLVMBasicBlock.create(System.getFunction(), "bbAndExprCond");

	Array blocks = Array.newN(1+size);
	blocks.set(0, bbCond);
	and_asm_inner(expr, blocks, bbMerge, 1, size, espidx);
	builder.setInsertPoint(bbMerge);
	PHINode phi = builder.createPHI(ctype2Type(/*TY_BOOL*/8), 0);
	bool_createPHI(blocks, phi, llvm_getBool(true), llvm_getBool(false));
}

Value letexpr_asm(int a, Expr expr, int espidx) {
	Expr exprL = expr.at(1);
	Expr exprR = expr.at(2);
	if(exprL.getBuild() == /*TEXPR_LOCAL*/4) {
		Value v = expr_asm(exprL.getIndex(), exprR, espidx);
		if(a != espidx) {
			return nmov_asm(a, exprL.getType(), espidx);
		}
		return v;
	}
	else{
		assert(exprL.getBuild() == /*TEXPR_FIELD*/6);
		Value v = expr_asm(espidx, exprR, espidx);
		System.p("TODO");
		//abort();
		//kshort_t index = (kshort_t)exprL.index;
		//kshort_t xindex = (kshort_t)(exprL.index >> (sizeof(kshort_t)*8));
		if(ty_isUnbox(exprR.getType())) {
			System.p("TODO field access");
			//ASM(XNMOV, oc(index), xindex, nc(espidx));
			return null;
		}
		else {
			System.p("TODO field access");
			//ASM(XNMOV, oc(index), xindex, oc(espidx));
			return null;
		}
		if(a != espidx) {
			return nmov_asm(a, exprL.getType(), espidx);
		}
	}
	assert(false);// "non-reachable"
	return null;
}

void errstmt_asm(Stmt stmt, int espidx) {
	Object o = stmt.getObjectNULL(0);
	String msg = Pointer.convertToString(o.getAddr());
	System.p("TODO");
	assert(false);
	//DBG_ASSERT(IS_String(msg));
	//ASM(ERROR, sfp(espidx), msg);
}

Boolean isExpr(Expr expr) {
	int x   = expr.getAddr();
	int ct  = Pointer.get(x, 8, 64);
	int cid = Pointer.get(ct, 88, 16);
	return cid == 15/*TY_Expr*/;
}
Expr Stmt.getExpr(int id, Expr def) {
	Expr expr = this.getObjectNULL(id);
	if (expr.isNull()) {
		return def;
	}
	return expr;
}

void exprstmt_asm(Stmt stmt, int espidx) {
	Expr expr = stmt.getObjectNULL(1);
	if(isExpr(expr)) {
		expr_asm(espidx, expr, espidx);
	}
}

void blockstmt_asm(Stmt stmt, int espidx) {
	Block bk = stmt.getBlock("$block", null);
	block_asm(bk);
}

void ifstmt_asm(Stmt stmt, int espidx) {
	IRBuilder builder = System.getBuilder();
	LLVMBasicBlock bbThen = LLVMBasicBlock.create(System.getFunction(), "bbThen");
	LLVMBasicBlock bbElse = LLVMBasicBlock.create(System.getFunction(), "bbElse");
	LLVMBasicBlock bbMerge = LLVMBasicBlock.create(System.getFunction(), "bbMerge");

	/* cond codegen */
	Value cond = expr_asm(espidx, stmt.getExpr(1, null), espidx);
	builder.createCondBr(cond, bbThen, bbElse);

	/* bbThen codegen */
	builder.setInsertPoint(bbThen);
	block_asm(stmt.getBlock("$block", null));
	//bbThen = builder.getInsertBlock();
	if (builder.getInsertBlock().getTerminator().isNull()) {
		builder.createBr(bbMerge);
	}

	/* bbElse codegen */
	builder.setInsertPoint(bbElse);
	block_asm(stmt.getBlock("else", null));
	//bbElse = builder.getInsertBlock();
	if (builder.getInsertBlock().getTerminator().isNull()) {
		builder.createBr(bbMerge);
	}

	/* TODO PHI Node */

	/* bbMerge codegen */
	builder.setInsertPoint(bbMerge);
}

void returnstmt_asm(Stmt stmt, int espidx) {
	Expr expr = stmt.getObjectNULL(1);
	IRBuilder builder = System.getBuilder();
	if(isExpr(expr) && expr.getType() != /*TY_void*/0) {
		Value retval = expr_asm(/*K_RTNIDX*/-4, expr, espidx);
		Function f = System.getFunction();
		Type retTy = f.getReturnType();
		int cid1 = type2cid(retTy);
		int cid2 = type2cid(retval.getType());
		if (isBoxTy(type2cid(retTy)) && isUnBoxTy(type2cid(retval.getType()))) {
			Value boxed = boxing(retval);
			builder.createRet(boxed);
		} else {
			builder.createRet(retval);
		}
	} else if (expr.getType() == /*TY_void*/0) {
		builder.createRetVoid();
	}
}

void loopstmt_asm(Stmt stmt, int espidx) {
	System.p("TODO loop stmt");
}

void undefinedstmt_asm(Stmt stmt, int espidx) {
	System.p("undefined asm syntax");
}

int block_asm2(Block bk, int i, int espidx) {
	Array blockS = bk.getBlocks();
	if (i < blockS.getSize()) {
		Stmt stmt = blockS.get(i);
		System.setUline(stmt.getUline());
		if(stmt.hasSyntax()) {
			if (stmt.getBuild() == /*TSTMT_ERR*/1) {
				errstmt_asm(stmt, espidx);
				return 0;
			} else if (stmt.getBuild() == /*TSTMT_EXPR*/2) {
				exprstmt_asm(stmt, espidx);
			} else if (stmt.getBuild() == /*TSTMT_BLOCK*/3) {
				blockstmt_asm(stmt, espidx);
			} else if (stmt.getBuild() == /*TSTMT_RETURN*/4) {
				returnstmt_asm(stmt, espidx);
				return 0;
			} else if (stmt.getBuild() == /*TSTMT_IF*/5) {
				ifstmt_asm(stmt, espidx);
			} else if (stmt.getBuild() == /*TSTMT_LOOP*/6) {
				loopstmt_asm(stmt, espidx);
			} else {
				undefinedstmt_asm(stmt, espidx);
			}
		}
		block_asm2(bk, i+1, espidx);
	}
	return 0;
}
void block_asm(Block bk) {
	int espidx = bk.getEspIndex();
	block_asm2(bk, 0, espidx);
}

Type createStructType(Module m, String name, Array fieldsTy, boolean needPtr) {
	//TODO FIXME StructType should be requested
	Type structTy = StructType.create(fieldsTy, name, false);
	if (needPtr) {
		return structTy;
	}
	//TODO FIXME PointerType should be requested
	Type ptrTy = PointerType.get(structTy);
	return ptrTy;
}

FunctionType createFunctionType(Type retTy, Array argsTy) {
	FunctionType fnTy = FunctionType.get(retTy, argsTy, false);
	return fnTy;
}

void construct_args(Array argsTy, Param pa, int n, int psize) {
	if (n < psize) {
		argsTy.set(n+2/*ctx+sfp*/, cid2Type(pa.getType(n)));
		construct_args(argsTy, pa, n+1, psize);
	}
}

String method2string(Method mtd) {
	String cname = mtd.getCname();
	String fname = mtd.getFname();
	return cname + "_" + fname;
}

Value loadsfp(IRBuilder builder, Value vsfp, int reqtype, int sfpidx) {
	int idx = tri(ty_isUnbox(reqtype), 1, 0);
	Value v = builder.createConstGEP2_32(vsfp, sfpidx, idx);
	if (reqtype != /*TY_Boolean*/3 && !(reqtype == /*TY_Float*/-10/*TODO*/)) {
		return v;
	}
	return builder.createBitCast(v, PointerType.get(cid2Type(reqtype)));
}

void push_back_params(Array params, IRBuilder builder, Value vsfp, Param pa, int i, int psize) {
	if (i < psize) {
		Value v = loadsfp(builder, vsfp, pa.getType(i), i+1);
		params.set(i+2, builder.createLoad(v, false));
		push_back_params(params, builder, vsfp, pa, i+1, psize);
	}
}

Function build_wrapper_func(Method mtd, Module m, Function f) {
	Param pa = mtd.getParam();
	int retTy = mtd.getReturnType();
	String name = method2string(mtd) + "_wrapper";
	FunctionType fnTy = System.getTyFunction();
	Function wrap = m.getOrInsertFunction(name, fnTy);
	LLVMBasicBlock bb = LLVMBasicBlock.create(wrap, "EntryBlock");
	IRBuilder builder = IRBuilder.new(bb);

	Array args = wrap.getArguments();
	Value arg_ctx = args.get(0);
	Value arg_sfp = args.get(1);
	Value arg_rix = args.get(2);

	arg_ctx.setName("ctx");
	arg_sfp.setName("sfp");
	arg_rix.setName("rix");

	builder.setInsertPoint(bb);
	/* load konoha args from sfp and build call params*/
	Array params = Array.newN(2+mtd.getParamSize());
	params.set(0, arg_ctx);
	params.set(1, arg_sfp);
	push_back_params(params, builder, arg_sfp, pa, 0, mtd.getParamSize());

	/* call function and  set return values */
	Value callinst = builder.createCall(f, params);
	if (retTy != /*TY_void*/0) {
		Value v = loadsfp(builder, arg_sfp, retTy, /*K_RTNIDX*/-4);
		builder.createStore(callinst, v, false/*isVolatile*/);
	}
	builder.createRetVoid();
	return wrap;
}

void function_set_params(Module m, Array args, Param pa, int i, int n) {
	if (i < n) {
		int reg = i+1;
		int idx = tri(ty_isUnbox(pa.getType(i)), nc(reg), oc(reg));
		Value v = args.get(i);
		vstack_setValue(idx, v);
		function_set_params(m, args, pa, i+1, n);
	}
}

void push_back_fparams(Array params, Array args, int i, int n) {
	if (i < n) {
		Value v = args.get(i+2);
		params.set(i, v);
		push_back_fparams(params, args, i+2, n);
	}
}

Function create_function(Module m, Method mtd) {
	int psize = mtd.getParamSize();
	int rtype = mtd.getReturnType();
	int argc  = 2 + psize;
	Array argsTy = Array.newN(argc);
	argsTy.set(0, ctype2Type(/*TY_CONTEXT*/16));
	argsTy.set(1, ctype2Type(/*TY_STACK*/17));
	construct_args(argsTy, mtd.getParam(), 0, psize);
	FunctionType fnTy = createFunctionType(cid2Type(rtype), argsTy);
	String fname = method2string(mtd);
	Function f = Function.create(fname, fnTy, m, ExternalLinkage);

	Array args = f.getArguments();
	Value vctx = args.get(0);
	Value vsfp = args.get(1);
	vctx.setName("lctx");
	vsfp.setName("lsfp");

	System.setCTX(vctx);
	System.setSFP(vsfp);

	Array a = Array.newN(psize);
	push_back_fparams(a, args, 0, psize);
	function_set_params(m, a, mtd.getParam(), 0, psize);
	return f;
}

int jit_init() {
	Module m = Module.new("test");
	ExecutionEngine ee = m.createExecutionEngine(2);
	Array objectHeaderTy = Array.newN(4);
	objectHeaderTy.set(0, ctype2Type(/*TY_LONG*/2),  /*magicflag*/);
	objectHeaderTy.set(1, ctype2Type(/*TY_CLASS*/10), /*ct*/);
	objectHeaderTy.set(2, ctype2Type(/*TY_LONG*/2),  /*hash*/);
	objectHeaderTy.set(3, ctype2Type(/*TY_PTR*/6),   /*kvproto*/);
	System.setTyHObject(createStructType(m, "ObjectHeader", objectHeaderTy, true));

	Array objectTy = Array.newN(5);
	objectTy.set(0, ctype2Type(/*TY_HOBJECT*/13),  /*h*/);
	objectTy.set(1, ctype2Type(/*TY_PTR*/6),   /*field0*/);
	objectTy.set(2, ctype2Type(/*TY_PTR*/6),   /*field1*/);
	objectTy.set(3, ctype2Type(/*TY_PTR*/6),   /*field2*/);
	objectTy.set(4, ctype2Type(/*TY_PTR*/6),   /*field3*/);
	System.setTyObject(createStructType(m, "Object", objectTy, false));

	Array sfpTy = Array.newN(2);
	sfpTy.set(0, ctype2Type(/*TY_OBJECT*/12),/*o*/);
	sfpTy.set(1, ctype2Type(/*TY_LONG*/2),  /*ndata*/);
	System.setTyStack(createStructType(m, "sfp", sfpTy, false));

	Array contextTy = Array.newN(10);
	contextTy.set(0, ctype2Type(/*TY_INT*/3),   /*safepoint*/);
	contextTy.set(1, ctype2Type(/*TY_STACK*/17), /*esp*/);
	contextTy.set(2, ctype2Type(/*TY_PTR*/6),   /*lib2*/);
	contextTy.set(3, ctype2Type(/*TY_PTR*/6),   /*memshare*/);
	contextTy.set(4, ctype2Type(/*TY_PTR*/6),   /*memlocal*/);
	contextTy.set(5, ctype2Type(/*TY_PTR*/6),   /*share*/);
	contextTy.set(6, ctype2Type(/*TY_PTR*/6),   /*local*/);
	contextTy.set(7, ctype2Type(/*TY_PTR*/6),   /*logger*/);
	contextTy.set(8, ctype2Type(/*TY_PTR*/6),   /*modshare*/);
	contextTy.set(9, ctype2Type(/*TY_PTR*/6),   /*modlocal*/);
	System.setTyContext(createStructType(m, "context", contextTy, false));

	Array argsTy = Array.new3(System.getTyContext(), System.getTyStack(), ctype2Type(/*TY_LONG*/2));
	System.setTyFunction(createFunctionType(ctype2Type(/*TY_VOID*/1), argsTy));
	System.setTyFunctionP(PointerType.get(System.getTyFunction()));

	// Method
	Array mtdTy = Array.newN(12);
	mtdTy.set( 0, ctype2Type(/*TY_HOBJECT*/13),  /*h*/);
	mtdTy.set( 1, ctype2Type(/*TY_FPTR*/15),    /*fcall_1*/);
	mtdTy.set( 2, ctype2Type(/*TY_PTR*/6),     /*pc_start*/);
	mtdTy.set( 3, ctype2Type(/*TY_LONG*/2),    /*flag*/);
	mtdTy.set( 4, ctype2Type(/*TY_SHORT*/4),   /*cid*/);
	mtdTy.set( 5, ctype2Type(/*TY_SHORT*/4),   /*mn*/);
	mtdTy.set( 6, ctype2Type(/*TY_SHORT*/4),   /*delta*/);
	mtdTy.set( 7, ctype2Type(/*TY_SHORT*/4),   /*packid*/);
	mtdTy.set( 8, ctype2Type(/*TY_OBJECT*/12),  /*pa*/);
	mtdTy.set( 9, ctype2Type(/*TY_OBJECT*/12),  /*tcode*/);
	mtdTy.set(10, ctype2Type(/*TY_OBJECT*/12),  /*objdata*/);
	mtdTy.set(11, ctype2Type(/*TY_OBJECT*/12),  /*proceedNUL*/);
	System.setTyMethod(createStructType(m, "mtd", mtdTy, false));

	System.setModule(m);
	System.setExecutionEngine(ee);
	//base.jitcache = kmap_init(0);
}

void System.genCode(Method mtd, Block bk) {
	//System.p("START CODE GENERATION..");
	if (System.getModule().isNull()) {
		jit_init();
	}
	Module m = System.getModule();
	Function f = create_function(m, mtd);
	System.setJITCache(mtd, f);
	System.setFunction(f);
	//assert(System.getBuilder().isNull());
	LLVMBasicBlock bb = LLVMBasicBlock.create(f, "BB");
	IRBuilder builder = IRBuilder.new(bb);
	System.setBuilder(builder);
	block_asm(bk);
	LLVMBasicBlock iblock = builder.getInsertBlock();
	if (iblock.getTerminator().isNull()) {
		int rtype = mtd.getReturnType();
		if (rtype == /*TY_void*/0) {
			builder.createRetVoid();
		} else {
			Value v = createConstValue(cid2ctype(rtype), System.knull(rtype), 0, 0.0);
			builder.createRet(v);
		}
	}
	Function wrap = build_wrapper_func(mtd, m, f);
	//f.dump();
	//m.dump();

	FunctionPassManager pm = FunctionPassManager.new(m);
	pm.add(LLVM.createVerifierPass());
	pm.doInitialization();
	pm.run(f);
	pm.run(wrap);

	ExecutionEngine ee = System.getExecutionEngine();
	int fptr = ee.getPointerToFunction(wrap);
	//System.p(fptr);
	System.assert(fptr != 0);
	System.clearValue();
	System.setBuilder(null);
	mtd.setFunc(fptr);
}

