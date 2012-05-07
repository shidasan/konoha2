K.importPackage("python");
System.p(System.importPyModule("numpy").arange(10));
System.p(System.importPyModule("numpy").arange(10).reshape(2,5));
System.p(System.importPyModule("numpy").average(System.importPyModule("numpy").arange(10)));
System.p(System.importPyModule("numpy").sum(System.importPyModule("numpy").arange(10)));
System.p(System.importPyModule("numpy").var(System.importPyModule("numpy").arange(10)));
System.p(System.importPyModule("numpy").std(System.importPyModule("numpy").arange(10)));
