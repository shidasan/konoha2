Konoha.importPackage("konoha.assert");
Konoha.importPackage("konoha.string");

String f(String x, String y) {
    return x + y;
}
boolean cmp(String x, String y) {
    return x == y;
}

assert(f("abcd", "1234") == "abcd1234");
