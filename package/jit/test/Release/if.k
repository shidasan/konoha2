K.importPackage("jit");
int g(int x) {
    if (x >= 10) {
        if (x > 100) {
            return 10;
        } else {
            return 20;
        }
    } else {
        return 30;
    }
}
System.p(g(1000));
System.p(g(10));
System.p(g(1));
assert(g(1000) == 10);
assert(g(10) == 20);
assert(g(1) == 30);
