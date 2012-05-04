// written by imasahiro
K.importPackage("konoha.float");
K.importPackage("konoha.llvm");

void f() {
	Module m = Module.new("hello");
	m.dump();
}

f();
