#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>
#include <unistd.h>

//## @Static @Public Int System.getpid();
static KMETHOD System_getPid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getpid());
}


// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Static   kMethod_Static
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t process_initPackage(CTX, struct kKonohaSpace *lgo, int argc, const char**args, kline_t pline)
{
	intptr_t methoddata[] = {
		_Public|_Static, _F(System_getPid), TY_Int, TY_System, MN_("getPid"), 0,
		DEND,
	};
	kloadMethodData(lgo, methoddata);
	return true;
}

static kbool_t process_setupPackage(CTX, struct kKonohaSpace *lgo, kline_t pline)
{
	return true;
}

static kbool_t process_initKonohaSpace(CTX,  struct kKonohaSpace *lgo, kline_t pline)
{
	return true;
}

static kbool_t process_setupKonohaSpace(CTX, struct kKonohaSpace *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* process_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("package.posix", "1.0"),
		KPACKLIB("POSIX.1", "1.0"),
		.initPackage = process_initPackage,
		.setupPackage = process_setupPackage,
		.initKonohaSpace = process_initKonohaSpace,
		.setupPackage = process_setupKonohaSpace,
	};
	return &d;
}
