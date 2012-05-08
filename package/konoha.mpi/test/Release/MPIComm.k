// written by wakamatsu
K.importPackage("konoha.float");
K.importPackage("konoha.bytes");
K.importPackage("konoha.mpi");

void test() {
	MPIComm w = MPIComm.getWorld();
	p("hello world. rank = " + w.getRank() + " / " + w.getSize());
}

test();

