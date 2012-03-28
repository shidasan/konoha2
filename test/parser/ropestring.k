
Lingo.importPackage("konoha.string");

String f(String x, String y) {
    return x + y;
}
boolean cmp(String x, String y) {
    return x == y;
}

cmp(f("abcd", "1234"), "abcd1234");
