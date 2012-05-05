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

//## boolean Token.isTypeName();
static KMETHOD Token_isTypeName(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(TK_isType(sfp[0].tk));
}

//## boolean Token.isParenthesis();
static KMETHOD Token_isParenthesis(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].tk->tt == AST_PARENTHESIS);
}

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

// --------------------------------------------------------------------------

typedef kbool_t (*FcheckParam)(CTX, kParam *);

static kbool_t checkMethod(CTX, kMethod *mtd, FcheckParam f, kclass_t *ct, kString *name, kline_t pline)
{
	if(mtd == NULL) {
		kreportf(CRIT_, pline, "undefined method: %s.%s", T_CT(ct), S_text(name));
		return false;
	}
	if(mtd->cid != ct->cid || !f(_ctx, mtd->pa)) {
		kreportf(CRIT_, pline, "mismatched method: %s.%s", T_CT(ct), S_text(name));
		return false;
	}
	return true;
}

static void setSyntaxMethod(CTX, kMethod **synptr, kMethod *mtd, kString *key, kString *name, kline_t pline)
{
	if(synptr[0] != NULL && synptr[0] != mtd) {
		kreportf(INFO_, pline, "overloaded method: %s %s", S_text(key), S_text(name));
		Method_setProceedMethod(_ctx, mtd, synptr[0]);
		KSETv(synptr[0], mtd);
	}
	else{
		KINITv(synptr[0], mtd);
	}
}

static kbool_t isParseStmt(CTX, kParam *pa)
{
	USING_SUGAR;
	return (pa->psize == 3 && pa->rtype == TY_Int
			&& pa->p[0].ty == TY_TokenArray && pa->p[1].ty == TY_Int && pa->p[2].ty == TY_Int );
}

//## void KonohaSpace.addParseStmt(String keyword, String name);
static KMETHOD KonohaSpace_addParseStmt(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kString *key = sfp[1].s;
	kString *name = sfp[2].s;
	kmethodn_t mn = ksymbol(S_text(name), S_size(name), MN_NONAME, SYMPOL_METHOD);
	kMethod *mtd = kKonohaSpace_getMethodNULL(sfp[0].ks, TY_Stmt, mn);
	if(checkMethod(_ctx, mtd, isParseStmt, CT_Stmt, name, sfp[K_RTNIDX].uline)) {
		struct _ksyntax *syn = NEWSYN_(sfp[0].ks, KW_s(key));
		setSyntaxMethod(_ctx, &syn->ParseStmtNULL, mtd, key, name, sfp[K_RTNIDX].uline);
	}
}

static kbool_t isParseExpr(CTX, kParam *pa)
{
	USING_SUGAR;
	return (pa->psize == 4 && pa->rtype == TY_Expr
			&& pa->p[0].ty == TY_TokenArray && pa->p[1].ty == TY_Int
			&& pa->p[2].ty == TY_Int && pa->p[3].ty == TY_Int );
}

//## void KonohaSpace.addParseExpr(String keyword, String name);
static KMETHOD KonohaSpace_addParseExpr(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kString *key = sfp[1].s;
	kString *name = sfp[2].s;
	kmethodn_t mn = ksymbol(S_text(name), S_size(name), MN_NONAME, SYMPOL_METHOD);
	kMethod *mtd = kKonohaSpace_getMethodNULL(sfp[0].ks, TY_Stmt, mn);
	if(checkMethod(_ctx, mtd, isParseExpr, CT_Stmt, name, sfp[K_RTNIDX].uline)) {
		struct _ksyntax *syn = NEWSYN_(sfp[0].ks, KW_s(key));
		setSyntaxMethod(_ctx, &syn->ParseExpr, mtd, key, name, sfp[K_RTNIDX].uline);
	}
}

static kbool_t isStmtTyCheck(CTX, kParam *pa)
{
	USING_SUGAR;
	return (pa->psize == 1 && pa->rtype == TY_Boolean && pa->p[0].ty == TY_Gamma);
}

//## void KonohaSpace.addStmtTyCheck(String keyword, String name);
static KMETHOD KonohaSpace_addStmtTyCheck(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kString *key = sfp[1].s;
	kString *name = sfp[2].s;
	kmethodn_t mn = ksymbol(S_text(name), S_size(name), MN_NONAME, SYMPOL_METHOD);
	kMethod *mtd = kKonohaSpace_getMethodNULL(sfp[0].ks, TY_Stmt, mn);
	if(checkMethod(_ctx, mtd, isStmtTyCheck, CT_Stmt, name, sfp[K_RTNIDX].uline)) {
		struct _ksyntax *syn = NEWSYN_(sfp[0].ks, KW_s(key));
		setSyntaxMethod(_ctx, &syn->StmtTyCheck, mtd, key, name, sfp[K_RTNIDX].uline);
	}
}

//## void KonohaSpace.addTopStmtTyCheck(String keyword, String name);
static KMETHOD KonohaSpace_addTopStmtTyCheck(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kString *key = sfp[1].s;
	kString *name = sfp[2].s;
	kmethodn_t mn = ksymbol(S_text(name), S_size(name), MN_NONAME, SYMPOL_METHOD);
	kMethod *mtd = kKonohaSpace_getMethodNULL(sfp[0].ks, TY_Stmt, mn);
	if(checkMethod(_ctx, mtd, isStmtTyCheck, CT_Stmt, name, sfp[K_RTNIDX].uline)) {
		struct _ksyntax *syn = NEWSYN_(sfp[0].ks, KW_s(key));
		setSyntaxMethod(_ctx, &syn->TopStmtTyCheck, mtd, key, name, sfp[K_RTNIDX].uline);
	}
}

static kbool_t isExprTyCheck(CTX, kParam *pa)
{
	USING_SUGAR;
	return (pa->psize == 2 && pa->rtype == TY_Boolean && pa->p[0].ty == TY_Gamma && pa->p[1].ty == TY_Int);
}

//## void KonohaSpace.addExprTyCheck(String keyword, String name);
static KMETHOD KonohaSpace_addExprTyCheck(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kString *key = sfp[1].s;
	kString *name = sfp[2].s;
	kmethodn_t mn = ksymbol(S_text(name), S_size(name), MN_NONAME, SYMPOL_METHOD);
	kMethod *mtd = kKonohaSpace_getMethodNULL(sfp[0].ks, TY_Expr, mn);
	if(checkMethod(_ctx, mtd, isExprTyCheck, CT_Expr, name, sfp[K_RTNIDX].uline)) {
		struct _ksyntax *syn = NEWSYN_(sfp[0].ks, KW_s(key));
		setSyntaxMethod(_ctx, &syn->ExprTyCheck, mtd, key, name, sfp[K_RTNIDX].uline);
	}
}

// --------------------------------------------------------------------------
// AST Method

//static ksyntax_t* get_syntax(CTX, kKonohaSpace *ks, kString *key)
//{
//	USING_SUGAR;
//	keyword_t kw = KW_s(key);
//	if(kw == FN_NONAME) {
//		kreportf(CRIT_, "undefined keyword: %s", S_text(key));
//	}
//	ksyntax_t *syn = SYN_(ks, kw);
//	if(syn == NULL) {
//		kreportf(CRIT_, "undefined syntax: %s", S_text(key));
//	}
//	return syn;
//}

//## Expr Token.printSyntaxError();
static KMETHOD Token_printSyntaxError(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kToken *tk  = sfp[0].tk;
	if(IS_String(tk->text)) {
		SUGAR p(_ctx, ERR_, tk->uline, tk->lpos, "syntax error: %s", S_text(tk->text));
	}
	else {
		SUGAR p(_ctx, ERR_, tk->uline, tk->lpos, "syntax error");
	}
	RETURN_(K_NULLEXPR);
}

//## Expr Stmt.newBlock(Token[] tls, int s, int e);
static KMETHOD Stmt_newBlock(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kStmt *stmt  = sfp[0].stmt;
	kArray *tls  = sfp[1].a;
	int s = sfp[2].ivalue, e = sfp[3].ivalue;
	RETURN_(SUGAR new_Block(_ctx, kStmt_ks(stmt), stmt, tls, s, e, ';'));
}

//## Expr Stmt.newExpr(Token[] tls, int s, int e);
static KMETHOD Stmt_newExpr(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kStmt *stmt  = sfp[0].stmt;
	kArray *tls  = sfp[1].a;
	int s = sfp[2].ivalue, e = sfp[3].ivalue;
	RETURN_(SUGAR Stmt_newExpr2(_ctx, stmt, tls, s, e));
}

//## Expr Stmt.newMethodCallExpr(Token key, Token self);
static KMETHOD Stmt_newMethodCallExpr(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kStmt *stmt  = sfp[0].stmt;
	kToken *tk   = sfp[1].tk;
	assert(tk->kw != 0);
	struct _kExpr *expr = new_W(Expr, SYN_(kStmt_ks(stmt), tk->kw));
	KSETv(expr->tk, tk);
	KSETv(expr->cons, new_(Array, 8));
	RETURN_(expr);
}

//## Expr Stmt.addExprParam(Token tk);
static KMETHOD Stmt_addExprParam(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kStmt *stmt  = sfp[0].stmt;
	kExpr *expr  = sfp[1].expr;
	kToken *tk     = sfp[2].tk;
	if(tk->tt != AST_PARENTHESIS || tk->tt != AST_BRANCET) {
		SUGAR p(_ctx, WARN_, tk->uline, tk->lpos, "not parameter token");
		kObject_setNullObject(expr, 1);
	}
	if(IS_NOTNULL(expr)) {
		assert(IS_Array(tk->sub));
		expr = SUGAR Stmt_addExprParams(_ctx, stmt, expr, tk->sub, 0, kArray_size(tk->sub), 1/*allowEmpty*/);
	}
	RETURN_(expr);
}

//## Expr Expr.addExpr(Expr expr, Expr o);
static KMETHOD Expr_addExpr(CTX, ksfp_t *sfp _RIX)
{
	kExpr *expr  = sfp[0].expr;
	kExpr *o     = sfp[1].expr;
	if(IS_NULL(o) && IS_Array(expr->cons)) {
		kObject_setNullObject(expr, 1);
	}
	if(IS_NOTNULL(expr)) {
		kArray_add(expr->cons, o);
	}
	RETURN_(expr);
}

// --------------------------------------------------------------------------

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
	int FN_name = FN_("name");
	int FN_tls = FN_("tokens"), FN_s = FN_("s"), FN_e = FN_("e");
	intptr_t MethodData[] = {
		_Public, _F(Token_isTypeName), TY_Boolean, TY_Token, MN_("isTypeName"), 0,
		_Public, _F(Token_isParenthesis), TY_Boolean, TY_Token, MN_("isParenthesis"), 0,

		_Public, _F(Stmt_setBuild), TY_void, TY_Stmt, MN_("setBuild"), 1, TY_Int, FN_buildid,
		_Public, _F(Stmt_getBlock), TY_Block, TY_Stmt, MN_("getBlock"), 2, TY_String, FN_key, TY_Block, FN_defval,
		_Public, _F(Stmt_tyCheckExpr), TY_Boolean, TY_Stmt, MN_("tyCheckExpr"), 4, TY_String, FN_key, TY_Gamma, FN_gma, TY_Int, FN_typeid, TY_Int, FN_pol,
		_Public, _F(Block_tyCheckAll), TY_Boolean, TY_Block, MN_("tyCheckAll"), 1, TY_Gamma, FN_gma,

		_Public, _F(KonohaSpace_addParseStmt), TY_void, TY_KonohaSpace, MN_("addStmtParser"), 2, TY_String, FN_key, TY_String, FN_name,
		_Public, _F(KonohaSpace_addParseExpr), TY_void, TY_KonohaSpace, MN_("addExprParser"), 2, TY_String, FN_key, TY_String, FN_name,
		_Public, _F(KonohaSpace_addTopStmtTyCheck), TY_void, TY_KonohaSpace, MN_("addTopStmtTyChecker"), 2, TY_String, FN_key, TY_String, FN_name,
		_Public, _F(KonohaSpace_addStmtTyCheck), TY_void, TY_KonohaSpace, MN_("addStmtTyChecker"), 2, TY_String, FN_key, TY_String, FN_name,
		_Public, _F(KonohaSpace_addExprTyCheck), TY_void, TY_KonohaSpace, MN_("addExprTyChecker"), 2, TY_String, FN_key, TY_String, FN_name,

		_Public, _F(Stmt_newExpr), TY_Expr, TY_Stmt, MN_("newExpr"), 1, TY_String, FN_key,
//		_Public, _F(Stmt_parsedExpr), TY_Expr, TY_Stmt, MN_("parseExpr"), 3, TY_TokenArray, FN_tls, TY_Int, FN_s, TY_Int, FN_e,
		DEND,
	};
	kKonohaSpace_loadMethodData(NULL, MethodData);
	return true;
}

static kbool_t sugar_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t isSubKeyword(CTX, kArray *tls, int s, int e)
{
	if(s+1 < e && tls->toks[s+1]->tt == TK_TEXT) {
		const char *t = S_text(tls->toks[s+1]->text);
		if(isalpha(t[0]) || t[0] < 0 /* multibytes char */) {
			return 1;
		}
	}
	return 0;
}

static struct _ksyntax *toks_syntax(CTX, kKonohaSpace *ks, kArray *tls)
{
	USING_SUGAR;
	int s = 0, e = kArray_size(tls);
	if(s < e) {
		if(tls->toks[s]->tt == TK_TEXT) {
			keyword_t kw;
			if(isSubKeyword(_ctx, tls, s, e)) {
				char buf[256];
				snprintf(buf, sizeof(buf), "%s %s", S_text(tls->toks[s]->text), S_text(tls->toks[s+1]->text));
				kw = SUGAR keyword(_ctx, (const char*)buf, strlen(buf), FN_NEWID);
			}
			else {
				kw = SUGAR keyword(_ctx, S_text(tls->toks[s]->text), S_size(tls->toks[s]->text), FN_NEWID);
			}
			return (struct _ksyntax*)NEWSYN_(ks, kw);
		}
	}
	return NULL;
}

static KMETHOD StmtTyCheck_sugar(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	kbool_t r = 0;
	VAR_StmtTyCheck(stmt, syn, gma);
	kTokenArray *tls = (kTokenArray*)kObject_getObject(stmt, KW_Toks, NULL);
	if(tls != NULL) {
		struct _ksyntax *syn = toks_syntax(_ctx, gma->genv->ks, tls);
		if(syn != NULL) {
			if(syn->syntaxRuleNULL != NULL) {
				SUGAR p(_ctx, WARN_, stmt->uline, -1, "overriding syntax rule: %s", T_kw(syn->kw));
				kArray_clear(syn->syntaxRuleNULL, 0);
			}
			else {
				KINITv(syn->syntaxRuleNULL, new_(Array, 8));
			}
			if(SUGAR makeSyntaxRule(_ctx, tls, 0, kArray_size(tls), syn->syntaxRuleNULL)) {
				r = 1;
			}
			else {
				kArray_clear(syn->syntaxRuleNULL, 0);
			}
		}
		kStmt_done(stmt);
	}
	if(r == 0) {
		SUGAR p(_ctx, CRIT_, stmt->uline, -1, "invalid sugar syntax");
	}
	RETURNb_(r);
}

static kbool_t sugar_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("sugar"), .rule ="\"sugar\" $toks", TopStmtTyCheck_(sugar), },
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
