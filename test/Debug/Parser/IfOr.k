// written by Masahiro Ide

int f(int a) {
    if(a == 1 || a == 2) {
        return 2;
    }
    return 0;
}

assert(f(0) == 0);
assert(f(1) == 2);
assert(f(2) == 2);
