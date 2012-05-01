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
