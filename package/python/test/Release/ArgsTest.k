K.importPackage("konoha.string");
K.importPackage("python");

System.p(System.importPyModule("ktest").args1(1));
System.p(System.importPyModule("ktest").args2(1, 2));
System.p(System.importPyModule("ktest").args3(1, 2, "hoge"));
