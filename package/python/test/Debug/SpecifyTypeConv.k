K.importPackage("python");
K.importPackage("konoha.string");

//System.p(System.importPyModule("ktest").retString());
System.p(System.importPyModule("ktest").retString().toString().startsWith("hello"));
