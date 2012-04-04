Konoha.importPackage("konoha.assert");

int add(int a) {
    return a + 1;
}
System.p(add(10));
assert(add(10) == 11);
