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

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t sugar_initPackage(CTX, struct kLingo *lgo, int argc, const char**args, kline_t pline)
{
	kevalshare_t *export = kevalshare;
	int TY_Stmt = export->cStmt->cid, TY_Block = export->cBlock->cid, TY_Gamma = export->cGamma->cid;
	int FN_buildid = FN_("buildid"), FN_key = FN_("key"), FN_defval = FN_("defval");
	int FN_typeid = FN_("typeid"), FN_gma = FN_("gma"), FN_pol = FN_("pol");
	intptr_t methoddata[] = {
		_Public, _F(Stmt_setBuild), TY_void, TY_Stmt, MN_("setBuild"), 1, TY_Int, FN_buildid,
		_Public, _F(Stmt_getBlock), TY_Block, TY_Stmt, MN_("getBlock"), 2, TY_String, FN_key, TY_Block, FN_defval,
		_Public, _F(Stmt_tyCheckExpr), TY_Boolean, TY_Stmt, MN_("tyCheckExpr"), 4, TY_String, FN_key, TY_Gamma, FN_gma, TY_Int, FN_typeid, TY_Int, FN_pol,
		_Public, _F(Block_tyCheckAll), TY_Boolean, TY_Block, MN_("tyCheckAll"), 1, TY_Gamma, FN_gma,
		DEND,
	};
	kaddMethodDef(NULL, methoddata);
	return true;
}

static kbool_t sugar_setupPackage(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t sugar_initLingo(CTX,  struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t sugar_setupLingo(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* sugar_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("sugar", "1.0"),
		.initPackage = sugar_initPackage,
		.setupPackage = sugar_setupPackage,
		.initLingo = sugar_initLingo,
		.setupPackage = sugar_setupLingo,
	};
	return &d;
}
