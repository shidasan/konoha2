//written by takeo onoda

K.importPackage("konoha.string");

void test() {
	System.p("foobar".lastIndexOf("f"));
	System.p("foobar".lastIndexOf("o"));
	System.p("foobar".lastIndexOf("A"));
	System.p("foobarfoobar".lastIndexOf("bar"));
	System.p("foobarfoobar".lastIndexOf("barr"));
	System.p("foobar".lastIndexOf(null));
	System.p("aあiいuう".indexOf("a"));
	System.p("aあiいuう".indexOf("あ"));
	System.p("aあiいuう".indexOf("I"));
	System.p("aあiいuう".indexOf("いuう"));
	System.p("aあiいuう".indexOf("いuうe"));
	System.p("aあiいuう".indexOf(null));
}

test();
