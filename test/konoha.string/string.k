//written by takeo onoda

void global_setup() {
	K.importPackage("konoha.string");
}

global_setup();

void test_String_equals() {
	System.p("hoge" == "h");
	System.p("hoge" == "hoge");
	System.p("hoge" == "hogeee");
	System.p("hoge" == null);
	System.p("aあiいuう" == "a");
	System.p("aあiいuう" == "aあiいuう");
	System.p("aあiいuう" == "aあiいuうe");
	System.p("aあiいuう" == null);
	System.p("" == "");
	System.p("" == null);
}

void test_String_startsWith() {
	System.p("foobar".startsWith("foo"));
	System.p("foobar".startsWith("foobar"));
	System.p("foobar".startsWith("foobarhoge"));
	System.p("aあiいuう".startsWith("aあiい"));
	System.p("aあiいuう".startsWith("aあiいuう"));
	System.p("aあiいuう".startsWith("aあiいuうe"));
	System.p("".startsWith(""));
	System.p("".startsWith(null));
}

void test_String_endsWith() {
	System.p("foobar".endsWith("bar"));
	System.p("foobar".endsWith("foobar"));
	System.p("foobar".endsWith("hogefoobar"));
	System.p("aあiいuう".endsWith("uう"));
	System.p("aあiいuう".endsWith("aあiいuう"));
	System.p("aあiいuう".endsWith("hogeaあiいuう"));
	System.p("".endsWith(""));
	System.p("".endsWith(null));
}

void test_String_indexOf() {
	System.p("foobar".indexOf("f"));
	System.p("foobar".indexOf("o"));
	System.p("foobar".indexOf("A"));
	System.p("foobar".indexOf("bar"));
	System.p("foobar".indexOf("barr"));
	System.p("foobar".indexOf(null));
	System.p("aあiいuう".indexOf("a"));
	System.p("aあiいuう".indexOf("あ"));
	System.p("aあiいuう".indexOf("I"));
	System.p("aあiいuう".indexOf("いuう"));
	System.p("aあiいuう".indexOf("いuうe"));
	System.p("aあiいuう".indexOf(null));
}

void test_String_lastIndexOf() {
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

void test_String_getSize() {
	System.p("hoge".getSize());
	System.p("aあiいuう".getSize());
	System.p("".getSize());
}

void test_String_get() {
	System.p("hoge".get(0));
	System.p("hoge".get(3));
	//	System.p("hoge".get(4));
	System.p("aあiいuう".get(0));
	System.p("aあiいuう".get(5));
	//	System.p("aあiいuう".get(6));
	//	System.p("".get(1));
}

void test_String_substring() {
	System.p("foobar".substring(0, 3));
	System.p("foobar".substring(3, 3));
	System.p("foobar".substring(3, 6));
	System.p("foobar".substring(1, 0));
	//	System.p("foobar".substring(10, 3));
	//	System.p("foobar".substring(-2, 4));
	System.p("foobar".substring(2, -2));
	System.p("aあiいuう".substring(0, 3));
	System.p("aあiいuう".substring(3, 3));
	System.p("aあiいuう".substring(3, 6));
	System.p("aあiいuう".substring(1, 0));
	//	System.p("aあiいuう".substring(10, 3));
	//	System.p("aあiいuう".substring(-2, 4));
	System.p("aあiいuう".substring(2, -2));
}

void test_String_toUpper() {
	System.p("fOoBaR123".toUpper());
	System.p("aあiいuう".toUpper());
}

void test_String_toLower() {
	System.p("fOoBaR123".toLower());
	System.p("aあiいuう".toLower());
}

void test_String_trim() {
	System.p("  \t\n\t foo \t\n bar \t\n\t".trim());
	System.p("  \t\n\t aあi \t\n いuう \t\n\t".trim());
	System.p(" \t\n\t ".trim());
}

test_String_equals();
test_String_startsWith();
test_String_endsWith();
test_String_indexOf();
test_String_lastIndexOf();
test_String_getSize();
test_String_get();
test_String_substring();
test_String_toUpper();
test_String_toLower();
test_String_trim();
