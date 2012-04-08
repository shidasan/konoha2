#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

// Expr Expr.tyCheckStub(Gamma gma, int req_tyid);
//static KMETHOD ExprTyCheck_stub(CTX, ksfp_t *sfp _RIX)
//{
//	VAR_ExprTyCheck(expr, gma, req_ty);
//	DBG_P("stub: size=%d", kArray_size(expr->consNUL));
//	RETURN_(K_NULLEXPR);
//}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t stub_initPackage(CTX, struct kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
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

static kbool_t stub_setupPackage(CTX, struct kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#define TOKEN(T)  .name = T, .namelen = (sizeof(T)-1)

static kbool_t stub_initKonohaSpace(CTX,  struct kKonohaSpace *ks, kline_t pline)
{
//	USING_SUGAR;
//	ksyntaxdef_t SYNTAX[] = {
//		{ TOKEN("float"), .type = TY_Float, },
//		{ TOKEN("double"), .type = TY_Float, },
//		{ TOKEN("$FLOAT"), .kw = KW_TK(TK_FLOAT), .ExprTyCheck = TokenTyCheck_FLOAT, },
//		{ .name = NULL, },
//	};
//	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t stub_setupKonohaSpace(CTX, struct kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KPACKDEF* stub_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("stub", "1.0"),
		.initPackage = stub_initPackage,
		.setupPackage = stub_setupPackage,
		.initKonohaSpace = stub_initKonohaSpace,
		.setupKonohaSpace = stub_setupKonohaSpace,
	};
	return &d;
}
