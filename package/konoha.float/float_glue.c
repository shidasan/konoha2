#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>
#include<konoha2/float.h>

#include"float_glue.h"

// --------------------------------------------------------------------------

static	kbool_t float_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	return share_initfloat(_ctx, ks, pline);
}

static kbool_t float_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t float_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return local_initfloat(_ctx, ks, pline);
}

static kbool_t float_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KPACKDEF* float_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("float", "1.0"),
		.initPackage = float_initPackage,
		.setupPackage = float_setupPackage,
		.initKonohaSpace = float_initKonohaSpace,
		.setupKonohaSpace = float_setupKonohaSpace,
	};
	return &d;
}
