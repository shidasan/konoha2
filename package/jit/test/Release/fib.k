K.importPackage("jit");

int fibo(int n) {
    if(n < 3) {
        return 1;
    }
    return fibo(n-1)+fibo(n-2);
}
System.p(fibo(1));
System.p(fibo(2));
System.p(fibo(3));
System.p(fibo(4));
System.p(fibo(5));
System.p(fibo(6));
System.p(fibo(7));
System.p(fibo(8));
System.p(fibo(9));
System.p(fibo(10));

int fibo2(int n) {
    if (n < 3) return 1;
    else return fibo2(n-1) + fibo2(n-2);
}

System.p(fibo2(10));

