#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include <konoha2/klib.h>

// --------------------------------------------------------------------------

//## method @static void KonohaSpace.assert(boolean cond)
static KMETHOD KonohaSpace_assert(CTX, ksfp_t *sfp _RIX)
{
	kbool_t cond = sfp[1].bvalue;
	kline_t fileid  = sfp[K_RTNIDX].uline;
	if (cond == false) {
		const char *fname = T_file(fileid);
		uintptr_t line = ULINE_line(fileid);
		fprintf(stderr, "Assertion!!: %s at line %lu\n", fname, line);
	}
	RETURNvoid_();
}

// --------------------------------------------------------------------------

static KMETHOD StmtTyCheck_assert(CTX, ksfp_t *sfp _RIX)
{
	//FIXME
//	USING_SUGAR;
//	kbool_t r = 1;
//	VAR_StmtTyCheck(stmt, syn, gma);
//	if((r = SUGAR Stmt_tyCheckExpr(_ctx, stmt, KW_EXPR, gma, TY_Boolean, 0))) {
//		kExpr *expr = kStmt_expr(stmt, KW_EXPR, NULL);
//		kMethod *mtd = kKonohaSpace_getMethodNULL(gma->genv->ks, TY_KonohaSpace, MN_("assert"));
//		assert(expr != NULL);
//		assert(mtd != NULL);
//		kStmt_toExprCall(stmt, mtd, 2, gma->genv->ks, expr);
//		expr = kStmt_expr(stmt, KW_EXPR, NULL);
//		expr->build = TEXPR_CALL;
//	}
//	RETURNb_(r);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Static   kMethod_Static
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t assert_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	int FN_cond = FN_("cond");
	intptr_t MethodData[] = {
		_Static|_Public, _F(KonohaSpace_assert), TY_Int, TY_KonohaSpace, MN_("assert"), 1, TY_Boolean, FN_cond,
		DEND,
	};
	Konoha_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t assert_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#define TOKEN(T)  .name = T/*, .namelen = (sizeof(T)-1)*/

static kbool_t assert_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("assert"), .rule = "'assert' '(' $expr ')'", .TopStmtTyCheck = StmtTyCheck_assert, .StmtTyCheck = StmtTyCheck_assert},
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);

	return true;
}


static kbool_t assert_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* assert_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("assert", "1.0"),
		.initPackage = assert_initPackage,
		.setupPackage = assert_setupPackage,
		.initKonohaSpace = assert_initKonohaSpace,
		.setupKonohaSpace = assert_setupKonohaSpace,
	};
	return &d;
}
