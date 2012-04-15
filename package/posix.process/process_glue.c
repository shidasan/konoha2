#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>
#include <unistd.h>
#include <signal.h>

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

#define _KVi(T) #T, TY_Int, T

static	kbool_t process_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	intptr_t MethodData[] = {
		_Public|_Static, _F(System_getPid), TY_Int, TY_System, MN_("getPid"), 0,
		DEND,
	};
	kloadMethodData(ks, MethodData);
	KDEFINE_INT_CONST IntData[] = {
		{_KVi(SIGHUP)},
		{_KVi(SIGINT)},
		{_KVi(SIGABRT)},
		{_KVi(SIGKILL)},
		{}
	};
	kloadConstData(kmodsugar->rootks, IntData, 0);
	return true;
}

static kbool_t process_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t process_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t process_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* process_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("package.posix", "1.0"),
		KPACKLIB("POSIX.1", "1.0"),
		.initPackage = process_initPackage,
		.setupPackage = process_setupPackage,
		.initKonohaSpace = process_initKonohaSpace,
		.setupKonohaSpace = process_setupKonohaSpace,
	};
	return &d;
}
