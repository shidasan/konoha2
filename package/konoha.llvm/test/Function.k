using konoha.llvm.*;
int f(int x, int y);

void main(String[] args) {
	print Module;
	m = new Module("hello");
	Type arg0Ty = Type.getInt64Ty();
	Type arg1Ty = Type.getInt64Ty();
	Type retTy  = Type.getInt64Ty();
	FunctionType fnTy = FunctionType.get(retTy, [arg0Ty, arg1Ty], false);
	Function func = m.getOrInsertFunction("test", fnTy);
	BasicBlock bb = BasicBlock.create(func);
	IRBuilder builder = new IRBuilder(bb);

	Array<Value> vargs = func.getArguments();
	vargs[0].setName("a0");
	vargs[1].setName("a1");
	v = builder.createAdd(vargs[0], vargs[1]);
	builder.createRet(v);
	m.dump();
	//ee = m.createExecutionEngine();
	//((Method)f).setFunction(ee.getPointerToFunction(m, func));
	//print f(10, 20);
}
