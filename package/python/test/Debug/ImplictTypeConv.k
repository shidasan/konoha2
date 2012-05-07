K.importPackage("konoha.string");
K.importPackage("python");

// retString() returns Python String object, but startsWith() is konoha String object.
System.p(System.importPyModule("ktest").retString().startsWith("hello"));
