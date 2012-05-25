/* written by shinpei */
K.import("konoha.assignment");
K.import("konoha.while");
K.import("konoha.bytes");
K.import("posix.file");

void f () {
	File f = System.fopen("helloworld.txt", "r");
	int bufsize = 64;
	Bytes buf = Bytes.new(bufsize);
	int ret = bufsize;
	String str = "";
	while(ret == bufsize) {
		buf.setAll(0); // zero clear
		ret = f.read(buf, 0, buf.getSize());
		str = str + buf.toString();
	}
	p(str);
}
f();

