#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

// --------------------------------------------------------------------------

static kbool_t Expr_isGetter(CTX, kExpr *expr)
{
	DBG_P("setter is unsupported .. ");
	return false;
}

static kExpr *Expr_toSetter(CTX, kExpr *expr, kExpr *vexpr)
{
	return K_NULLEXPR;
}

// --------------------------------------------------------------------------

// Expr Expr.tyCheckStub(Gamma gma, int req_tyid);
static KMETHOD ExprTyCheck_assignment(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, req_ty);
	DBG_P("typing: assignment .. ");
	kExpr *lexpr = kExpr_tyCheckAt(expr, 1, gma, TY_var, TPOL_ALLOWVOID);
	if(lexpr != K_NULLEXPR) {
		kExpr *rexpr = kExpr_tyCheckAt(expr, 2, gma, lexpr->ty, 0);
		if(lexpr->build == TEXPR_LOCAL || lexpr->build == TEXPR_BLOCKLOCAL_ || lexpr->build == TEXPR_FIELD) {
			if(rexpr != K_NULLEXPR) {
				expr->build = TEXPR_LET;
				rexpr->ty = lexpr->ty;
			}
			RETURN_(expr);
		}
		if(Expr_isGetter(_ctx, lexpr)) {
			RETURN_(Expr_toSetter(_ctx, lexpr, rexpr));
		}
	}
	RETURN_(K_NULLEXPR);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t assignment_initPackage(CTX, struct kKonohaSpace *ks, int argc, const char**args, kline_t pline)
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

static kbool_t assignment_setupPackage(CTX, struct kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#define TOKEN(T)  .name = T/*, .namelen = (sizeof(T)-1)*/

static kbool_t assignment_initKonohaSpace(CTX,  struct kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	ksyntaxdef_t SYNTAX[] = {
		{ TOKEN("="), .op2 = "*", .priority_op2 = 4096, .ExprTyCheck = ExprTyCheck_assignment},
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t assignment_setupKonohaSpace(CTX, struct kKonohaSpace *ks, kline_t pline)
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
