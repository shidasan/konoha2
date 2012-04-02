#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include <konoha2/klib.h>

// --------------------------------------------------------------------------

//## method @static void KonohaSpace.assert(boolean cond)
static KMETHOD KonohaSpace_assert(CTX, ksfp_t *sfp _RIX)
{
	kbool_t cond = sfp[1].bvalue;
	kline_t uri  = sfp[0].uline;
	if (cond == false) {
		const char *fname = S_text(Suri(_ctx, uri));
		uintptr_t line = ULINE_line(uri);
		fprintf(stderr, "Assertion!!: %s at line %lu\n", fname, line);
	}
	RETURNvoid_();
}

// --------------------------------------------------------------------------
static KMETHOD StmtTyCheck_assert(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kbool_t r = 1;
	VAR_StmtTyCheck(stmt, gma);
	if((r = SUGAR Stmt_tyCheckExpr(_ctx, stmt, KW_EXPR, gma, TY_Boolean, 0))) {
		kExpr *expr = kStmt_expr(stmt, KW_EXPR, NULL);
		kMethod *mtd = SUGAR KonohaSpace_getMethodNULL(_ctx, gma->genv->ks, TY_KonohaSpace, MN_("assert"));
		assert(expr != NULL);
		assert(mtd != NULL);
		SUGAR Stmt_toExprCall(_ctx, stmt, mtd, 2, K_NULL/*TODO KNH_TNULL(KonohaSpace)*/, expr);
		expr = kStmt_expr(stmt, KW_EXPR, NULL);
		expr->build = TEXPR_CALL;
	}
	RETURNb_(r);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Static   kMethod_Static
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t assert_initPackage(CTX, struct kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	int FN_cond = FN_("cond");
	intptr_t methoddata[] = {
		_Static|_Public, _F(KonohaSpace_assert), TY_Int, TY_KonohaSpace, MN_("assert"), 1, TY_Boolean, FN_cond,
		DEND,
	};
	kloadMethodData(ks, methoddata);
	return true;
}

static kbool_t assert_setupPackage(CTX, struct kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#define TOKEN(T)  .name = T, .namelen = (sizeof(T)-1)

static kbool_t assert_initKonohaSpace(CTX,  struct kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	ksyntaxdef_t SYNTAX[] = {
		{ TOKEN("assert"), .rule = "'assert' '(' $expr ')'", .TopStmtTyCheck = StmtTyCheck_assert, .StmtTyCheck = StmtTyCheck_assert},
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);

	return true;
}


static kbool_t assert_setupKonohaSpace(CTX, struct kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KPACKDEF* assert_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("assert", "1.0"),
		.initPackage = assert_initPackage,
		.setupPackage = assert_setupPackage,
		.initKonohaSpace = assert_initKonohaSpace,
		.setupKonohaSpace = assert_setupKonohaSpace,
	};
	return &d;
}
