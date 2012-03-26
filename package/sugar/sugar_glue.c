#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>


static KMETHOD StmtTyCheck_while(CTX, ksfp_t *sfp _RIX)
{
	kbool_t r = 1;
	VAR_StmtTyCheck(stmt, gma);
	if((r = Stmt_tycheckExpr(_ctx, stmt, KW_EXPR, gma, TY_Boolean, 0))) {
		kBlock *bk = Stmt_getBlockNULL(_ctx, stmt, KW_BLOCK);
		Block_tycheck(_ctx, bk, gma);
		stmt->build = TSTMT_IF;
	}
	RETURNb_(r);
}


// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t loop_initPackage(CTX, struct kLingo *lgo, int argc, const char**args, kline_t pline)
{

	return true;
}

static kbool_t loop_setupPackage(CTX, struct kLingo *lgo, kline_t pline)
{

	return true;
}

static kbool_t loop_initLingo(CTX,  struct kLingo *lgo, kline_t pline)
{
	ksyntaxdef_t SYNTAX[] = {
		{ TOKEN("while"), .rule ="'while' '(' $expr ')' $block", .StmtTyCheck = StmtTyCheck_while, },
		{ .name = NULL, },
	};
	kLingo_defineSyntax(lgo, SYNTAX);
	return true;
}

static kbool_t loop_setupLingo(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* loop_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("float", "1.0"),
		.initPackage = loop_initPackage,
		.setupPackage = loop_setupPackage,
		.initLingo = loop_initLingo,
		.setupPackage = loop_setupLingo,
	};
	return &d;
}
