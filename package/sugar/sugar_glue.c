/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

#define KW_s(s) SUGAR keyword(_ctx, S_text(s), S_size(s), FN_NONAME)

//## void Stmt.setBuild(int buildid);
static KMETHOD Stmt_setBuild(CTX, ksfp_t *sfp _RIX)
{
	//sfp[0].stmt->build = sfp[1].ivalue;
}

//## Block Stmt.getBlock(String key, Block def);
static KMETHOD Stmt_getBlock(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	RETURN_(kStmt_block(sfp[0].stmt, KW_s(sfp[1].s), sfp[2].bk));
}

//## boolean Stmt.tyCheckExpr(String key, Gamma gma, int typeid, int pol);
static KMETHOD Stmt_tyCheckExpr(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	RETURNb_(SUGAR Stmt_tyCheckExpr(_ctx, sfp[0].stmt, KW_s(sfp[1].s), sfp[2].gma, (ktype_t)sfp[3].ivalue, (int)sfp[4].ivalue));
}

//## boolean Blook.tyCheckAll(Gamma gma);
static KMETHOD Block_tyCheckAll(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	RETURNb_(SUGAR Block_tyCheckAll(_ctx, sfp[0].bk, sfp[1].gma));
}

static inline kbool_t isStmtTyCheck(CTX, kParam *pa)
{
	return (pa->psize == 1 && pa->rtype == TY_Boolean && pa->p[0].ty == kmodsugar->cGamma->cid);
}

//## void KonohaSpace.defineSyntaxRule(String keyword, String rule);
static KMETHOD KonohaSpace_defineSyntaxRule(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	struct _ksyntax *syn = NEWSYN_(sfp[0].ks, KW_s(sfp[1].s));
	if(syn->syntaxRule != NULL) {
		kreportf(INFO_, sfp[K_RTNIDX].uline, "overriding SyntaxRule: %s", S_text(sfp[1].s));
		kArray_clear(syn->syntaxRule, 0);
	}
	else {
		KINITv(syn->syntaxRule, new_(Array, 8));
	}
	SUGAR parseSyntaxRule(_ctx, S_text(sfp[2].s), sfp[K_RTNIDX].uline, syn->syntaxRule);
}

//## void KonohaSpace.defineStmtTyCheck(String keyword, String methodname);
static KMETHOD KonohaSpace_defineStmtTyCheck(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kmethodn_t mn = ksymbol(S_text(sfp[2].s), S_size(sfp[2].s), MN_NONAME, SYMPOL_METHOD);
	kMethod *mtd = SUGAR KonohaSpace_getMethodNULL(_ctx, sfp[0].ks, SUGAR cStmt->cid, mn);
	if(mtd == NULL) {
		kreportf(ERR_, sfp[K_RTNIDX].uline, "undefined method: Stmt.%s", S_text(sfp[2].s));
	}
	if(!isStmtTyCheck(_ctx, mtd->pa)) {
		kreportf(ERR_, sfp[K_RTNIDX].uline, "mismatched method: Stmt.%s", S_text(sfp[2].s));
	}
	struct _ksyntax *syn = NEWSYN_(sfp[0].ks, KW_s(sfp[1].s));
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

static	kbool_t sugar_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	int FN_buildid = FN_("buildid"), FN_key = FN_("key"), FN_defval = FN_("defval");
	int FN_typeid = FN_("typeid"), FN_gma = FN_("gma"), FN_pol = FN_("pol");
	int FN_methodname = FN_("methodName");
	intptr_t MethodData[] = {
		_Public, _F(Stmt_setBuild), TY_void, TY_Stmt, MN_("setBuild"), 1, TY_Int, FN_buildid,
		_Public, _F(Stmt_getBlock), TY_Block, TY_Stmt, MN_("getBlock"), 2, TY_String, FN_key, TY_Block, FN_defval,
		_Public, _F(Stmt_tyCheckExpr), TY_Boolean, TY_Stmt, MN_("tyCheckExpr"), 4, TY_String, FN_key, TY_Gamma, FN_gma, TY_Int, FN_typeid, TY_Int, FN_pol,
		_Public, _F(Block_tyCheckAll), TY_Boolean, TY_Block, MN_("tyCheckAll"), 1, TY_Gamma, FN_gma,
		_Public, _F(KonohaSpace_defineSyntaxRule), TY_void, TY_KonohaSpace, MN_("defineSyntaxRule"),   2, TY_String, FN_key, TY_String, FN_("rule"),
		_Public, _F(KonohaSpace_defineStmtTyCheck), TY_void, TY_KonohaSpace, MN_("defineStmtTyCheck"), 2, TY_String, FN_key, TY_String, FN_methodname,
		DEND,
	};
	kKonohaSpace_loadMethodData(NULL, MethodData);
	return true;
}

static kbool_t sugar_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t sugar_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("sugar"), .rule ="\"sugar\" $toks", /*TopStmtTyCheck_(if),*/ },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t sugar_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* sugar_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("sugar", "1.0"),
		.initPackage = sugar_initPackage,
		.setupPackage = sugar_setupPackage,
		.initKonohaSpace = sugar_initKonohaSpace,
		.setupKonohaSpace = sugar_setupKonohaSpace,
	};
	return &d;
}
