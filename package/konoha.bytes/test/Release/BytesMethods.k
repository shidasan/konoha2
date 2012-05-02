// written by shinpei nakata

void global_setup() {
	K.importPackage("konoha.bytes");
}

boolean test_decode() {
	Bytes b = "".toBytes();
	String s = b.decodeFrom();
	System.p(s);
}

test_decode();
