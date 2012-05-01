// written by kimio kuramitsu
int f(int n) {
	int a = 1;
	return a+n;
}
System.p(f(0));
System.p(f(1));

assert(f(0) == 1);
assert(f(1) == 2);

void g() {
	int a = 1;
	int b = 2;
	System.p(a);
	System.p(b);
	assert(a == 1);
	assert(b == 2);
}

g();

