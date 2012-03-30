using konoha.llvm.*;

void main(String[] args) {
	print Module;
	m = new Module("hello");
	Type f0Ty = Type.getInt64Ty();
	Type f1Ty = Type.getInt32Ty();
	Type f2Ty = Type.getInt32PtrTy();
	StructType sType = StructType.get([f0Ty, f1Ty, f2Ty], false);
	m.addTypeName("s1", sType);
	m.dump();
}
