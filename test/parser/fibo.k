Konoha.importPackage("konoha.assert");

int fibo(int n) {
    if (n < 3) {
        return 1;
    } else {
        return fibo(n-1) + fibo(n-2);
    }
}
assert(fibo(10) == 55);
