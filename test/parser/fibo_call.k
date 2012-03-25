@Static int fibo(int n) {
    if (n.opLT(3)) { return 1; }
    return fibo(n.opSUB(1)).opADD(fibo(n.opSUB(2)));
}

fibo(1);
