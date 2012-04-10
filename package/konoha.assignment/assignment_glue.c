#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

#include"assignment_glue.h"

static	kbool_t assignment_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	return true;
}

static kbool_t assignment_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t assignment_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return local_initassignment(_ctx, ks, pline);
}

static kbool_t assignment_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KPACKDEF* assignment_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("assignment", "1.0"),
		.initPackage = assignment_initPackage,
		.setupPackage = assignment_setupPackage,
		.initKonohaSpace = assignment_initKonohaSpace,
		.setupKonohaSpace = assignment_setupKonohaSpace,
	};
	return &d;
}
