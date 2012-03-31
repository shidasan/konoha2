#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>
#include<konoha2/float.h>

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t math_initPackage(CTX, struct kKonohaSpace *lgo, int argc, const char**args, kline_t pline)
{
//	KREQUIRE("konoha.float");
//	int FN_methodname = FN_("x");
//	intptr_t methoddata[] = {
//		_Public, _F(Stmt_setBuild), TY_void, TY_Stmt, MN_("setBuild"), 1, TY_Int, FN_buildid,
//		_Public, _F(Stmt_getBlock), TY_Block, TY_Stmt, MN_("getBlock"), 2, TY_String, FN_key, TY_Block, FN_defval,
//		_Public, _F(Stmt_tyCheckExpr), TY_Boolean, TY_Stmt, MN_("tyCheckExpr"), 4, TY_String, FN_key, TY_Gamma, FN_gma, TY_Int, FN_typeid, TY_Int, FN_pol,
//		_Public, _F(Block_tyCheckAll), TY_Boolean, TY_Block, MN_("tyCheckAll"), 1, TY_Gamma, FN_gma,
//		_Public, _F(KonohaSpace_defineSyntaxRule), TY_void, TY_KonohaSpace, MN_("defineSyntaxRule"),   2, TY_String, FN_key, TY_String, FN_("rule"),
//		_Public, _F(KonohaSpace_defineStmtTyCheck), TY_void, TY_KonohaSpace, MN_("defineStmtTyCheck"), 2, TY_String, FN_key, TY_String, FN_methodname,
//		DEND,
//	};
//	kloadMethodData(NULL, methoddata);
	return true;
}

static kbool_t math_setupPackage(CTX, struct kKonohaSpace *lgo, kline_t pline)
{
	return true;
}

static kbool_t math_initKonohaSpace(CTX,  struct kKonohaSpace *lgo, kline_t pline)
{

	return true;
}

static kbool_t math_setupKonohaSpace(CTX, struct kKonohaSpace *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* math_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("math", "1.0"),
		.initPackage = math_initPackage,
		.setupPackage = math_setupPackage,
		.initKonohaSpace = math_initKonohaSpace,
		.setupPackage = math_setupKonohaSpace,
	};
	return &d;
}
