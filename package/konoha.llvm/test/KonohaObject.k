using konoha.llvm.*;
Map<String, Type> ktype = {};

void constructObjectStruct(Module m) {
	Type[] fields = [];
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	hObjectTy = StructType.get(fields, false);
	m.addTypeName("hObject", hObjectTy);

	fields = [];
	fields.add(hObjectTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	objectTy = StructType.get(fields, false);
	objectPtrTy = PointerType.get(objectTy);
	objectPtrPtrTy = PointerType.get(objectPtrTy)
	m.addTypeName("Object", objectTy);
	m.addTypeName("ObjectPtr", objectPtrTy);
	ktype["objectPtrTy"] = objectPtrTy;
	ktype["objectPtrPtrTy"] = objectPtrPtrTy;

	fields = [];
	fields.add(hObjectTy);
	fields.add(Type.getInt64PtrTy());
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	arrayTy = StructType.get(fields, false);
	arrayPtrTy = PointerType.get(arrayTy);
	m.addTypeName("Array", arrayTy);

	fields = [];
	fields.add(hObjectTy);
	fields.add(objectPtrPtrTy);
	fields.add(objectPtrTy);
	objectFieldTy    = StructType.get(fields, false);
	objectFieldPtrTy = PointerType.get(objectFieldTy)
	m.addTypeName("ObjectField", objectFieldTy);

	fields = [];
	fields.add(hObjectTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	outputStreamTy = StructType.get(fields, false);
	outputStreamPtrTy = PointerType.get(outputStreamTy);
	m.addTypeName("OutputStream", outputStreamTy);
	ktype["outputStreamPtrTy"] = outputStreamPtrTy;

	fields = [];
	fields.add(hObjectTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	inputStreamTy = StructType.get(fields, false);
	inputStreamPtrTy = PointerType.get(inputStreamTy);
	m.addTypeName("InputStream", inputStreamTy);
	ktype["inputStreamPtrTy"] = inputStreamPtrTy;

	fields = [];
	fields.add(objectPtrTy);
	fields.add(Type.getFloatTy());
	sfpTy = StructType.get(fields, false);
	sfpPtrTy = PointerType.get(sfpTy);
	m.addTypeName("sfp", sfpTy);
	ktype["sfpPtrTy"] = sfpPtrTy;

	fields = [];
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(inputStreamPtrTy);
	fields.add(outputStreamPtrTy);
	fields.add(outputStreamPtrTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(arrayPtrTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	sysExTy = StructType.get(fields, false);

	fields = [];
	fields.add(hObjectTy);
	fields.add(sysExTy);
	sysTy = StructType.get(fields, false);
	sysPtrTy = PointerType.get(sysTy);
	m.addTypeName("System", sysTy);
	ktype["sysPtrTy"] = sysPtrTy;

	fields = [];
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(sysPtrTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(sfpPtrTy);
	fields.add(sfpPtrTy);
	fields.add(Type.getInt64Ty());
	fields.add(sfpPtrTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(objectPtrTy);
	fields.add(objectPtrTy);
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(objectPtrPtrTy);
	fields.add(Type.getInt64Ty());
	fields.add(objectPtrPtrTy);
	fields.add(Type.getInt64Ty());
	fields.add(objectPtrPtrTy);
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(inputStreamPtrTy);
	fields.add(outputStreamPtrTy);
	fields.add(outputStreamPtrTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(outputStreamPtrTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt32Ty);
	fields.add(Type.getInt32Ty);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	Type[] vec16_fields = [];
	for (i=0; i < 16; i++) {
		vec16_fields.add(Type.getInt1Ty());
	}
	fields.add(StructType.get(vec16_fields, false));
	fields.add(Type.getInt64Ty());
	fields.add(Type.getInt8PtrTy());
	fields.add(objectPtrTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt8PtrTy());

	ctxTy = StructType.get(fields, false);
	ctxPtrTy = PointerType.get(ctxTy);
	m.addTypeName("context", ctxTy);
	m.addTypeName("CTX", ctxPtrTy);
	ktype["ctxTy"] = ctxPtrTy;

	Type[] argsTy = [];
	argsTy.add(ctxPtrTy);
	argsTy.add(sfpPtrTy);
	argsTy.add(Type.getInt64PtrTy());
	FunctionType fnTy = FunctionType.get(Type.getVoidTy(), argsTy, false);
	fmethodTy = PointerType.get(fnTy);
	ktype["fmethodTy"] = fmethodTy;

	fields = [];
	fields.add(hObjectTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(Type.getInt32Ty);
	fields.add(Type.getInt32Ty);
	fields.add(fmethodTy);
	fields.add(Type.getInt8PtrTy());
	methodTy = StructType.get(fields, false);
	m.addTypeName("Method", methodTy);
	ktype["MethodPtrTy"] = PointerType.get(methodTy);

	fields = [];
	fields.add(hObjectTy);
	fields.add(Type.getInt8PtrTy());
	fields.add(fmethodTy);
	iteratorTy = StructType.get(fields, false);
	m.addTypeName("Iterator", iteratorTy);
	ktype["IteratorPtrTy"] = PointerType.get(iteratorTy);
}
Module global_module = new Module("hello");
constructObjectStruct(global_module);
global_module.dump();

