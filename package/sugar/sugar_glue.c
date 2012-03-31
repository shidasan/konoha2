#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

#define KW_s(s) export->keyword(_ctx, S_text(s), S_size(s), FN_NONAME)

//## void Stmt.setBuild(int buildid);
static KMETHOD Stmt_setBuild(CTX, ksfp_t *sfp _RIX)
{
	sfp[0].stmt->build = sfp[1].ivalue;
}

//## Block Stmt.getBlock(String key, Block def);
static KMETHOD Stmt_getBlock(CTX, ksfp_t *sfp _RIX)
{
	kevalshare_t *export = kevalshare;
	RETURN_(export->Stmt_getBlock(_ctx, sfp[0].stmt, KW_s(sfp[1].s), sfp[2].bk));
}

//## boolean Stmt.tyCheckExpr(String key, Gamma gma, int typeid, int pol);
static KMETHOD Stmt_tyCheckExpr(CTX, ksfp_t *sfp _RIX)
{
	kevalshare_t *export = kevalshare;
	RETURNb_(export->Stmt_tyCheckExpr(_ctx, sfp[0].stmt, KW_s(sfp[1].s), sfp[2].gma, (ktype_t)sfp[3].ivalue, (int)sfp[4].ivalue));
}

//## boolean Blook.tyCheckAll(Gamma gma);
static KMETHOD Block_tyCheckAll(CTX, ksfp_t *sfp _RIX)
{
	kevalshare_t *export = kevalshare;
	RETURNb_(export->Block_tyCheckAll(_ctx, sfp[0].bk, sfp[1].gma));
}

static inline kbool_t isStmtTyCheck(CTX, kParam *pa)
{
	return (pa->psize == 1 && pa->rtype == TY_Boolean && pa->p[0].ty == kevalshare->cGamma->cid);
}

//## void KonohaSpace.defineSyntaxRule(String keyword, String rule);
static KMETHOD KonohaSpace_defineSyntaxRule(CTX, ksfp_t *sfp _RIX)
{
	kevalshare_t *export = kevalshare;
	ksyntax_t *syn = export->KonohaSpace_syntax(_ctx, sfp[0].lgo, KW_s(sfp[1].s), 1/*isnew*/);
	if(syn->syntaxRule != NULL) {
		kreportf(INFO_, sfp[K_RTNIDX].uline, "overriding SyntaxRule: %s", S_text(sfp[1].s));
		KSETv(syn->syntaxRule, new_(Array, 0));
	}
	else {
		KINITv(syn->syntaxRule, new_(Array, 0));
	}
	export->parseSyntaxRule(_ctx, S_text(sfp[2].s), sfp[K_RTNIDX].uline, syn->syntaxRule);
}

//## void KonohaSpace.defineStmtTyCheck(String keyword, String methodname);
static KMETHOD KonohaSpace_defineStmtTyCheck(CTX, ksfp_t *sfp _RIX)
{
	kevalshare_t *export = kevalshare;
	kmethodn_t mn = ksymbol(S_text(sfp[2].s), S_size(sfp[2].s), MN_NONAME, SYMPOL_METHOD);
	kMethod *mtd = export->KonohaSpace_getMethodNULL(_ctx, sfp[0].lgo, export->cStmt->cid, mn);
	if(mtd == NULL) {
		kreportf(ERR_, sfp[K_RTNIDX].uline, "undefined method: Stmt.%s", S_text(sfp[2].s));
	}
	if(!isStmtTyCheck(_ctx, mtd->pa)) {
		kreportf(ERR_, sfp[K_RTNIDX].uline, "mismatched method: Stmt.%s", S_text(sfp[2].s));
	}
	ksyntax_t *syn = export->KonohaSpace_syntax(_ctx, sfp[0].lgo, KW_s(sfp[1].s), 1/*isnew*/);
	if(syn->StmtTyCheck != NULL) {
		kreportf(INFO_, sfp[K_RTNIDX].uline, "overriding StmtTyCheck: %s", S_text(sfp[1].s));
	}
	KSETv(syn->StmtTyCheck, mtd);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t sugar_initPackage(CTX, struct kKonohaSpace *lgo, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	int FN_buildid = FN_("buildid"), FN_key = FN_("key"), FN_defval = FN_("defval");
	int FN_typeid = FN_("typeid"), FN_gma = FN_("gma"), FN_pol = FN_("pol");
	int FN_methodname = FN_("methodName");
	intptr_t methoddata[] = {
		_Public, _F(Stmt_setBuild), TY_void, TY_Stmt, MN_("setBuild"), 1, TY_Int, FN_buildid,
		_Public, _F(Stmt_getBlock), TY_Block, TY_Stmt, MN_("getBlock"), 2, TY_String, FN_key, TY_Block, FN_defval,
		_Public, _F(Stmt_tyCheckExpr), TY_Boolean, TY_Stmt, MN_("tyCheckExpr"), 4, TY_String, FN_key, TY_Gamma, FN_gma, TY_Int, FN_typeid, TY_Int, FN_pol,
		_Public, _F(Block_tyCheckAll), TY_Boolean, TY_Block, MN_("tyCheckAll"), 1, TY_Gamma, FN_gma,
		_Public, _F(KonohaSpace_defineSyntaxRule), TY_void, TY_KonohaSpace, MN_("defineSyntaxRule"),   2, TY_String, FN_key, TY_String, FN_("rule"),
		_Public, _F(KonohaSpace_defineStmtTyCheck), TY_void, TY_KonohaSpace, MN_("defineStmtTyCheck"), 2, TY_String, FN_key, TY_String, FN_methodname,
		DEND,
	};
	kloadMethodData(NULL, methoddata);
	return true;
}

static kbool_t sugar_setupPackage(CTX, struct kKonohaSpace *lgo, kline_t pline)
{
	return true;
}

static kbool_t sugar_initKonohaSpace(CTX,  struct kKonohaSpace *lgo, kline_t pline)
{

	return true;
}

static kbool_t sugar_setupKonohaSpace(CTX, struct kKonohaSpace *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* sugar_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("sugar", "1.0"),
		.initPackage = sugar_initPackage,
		.setupPackage = sugar_setupPackage,
		.initKonohaSpace = sugar_initKonohaSpace,
		.setupPackage = sugar_setupKonohaSpace,
	};
	return &d;
}
