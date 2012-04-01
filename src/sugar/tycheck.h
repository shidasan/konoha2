/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved..
 *
 * You may choose one of the following two licenses when you use konoha.
 * If you want to use the latter license, please contact us.
 *
 * (1) GNU General Public License 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure License 1.0
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

static gmabuf_t *Gamma_push(CTX, kGamma *gma, gmabuf_t *newone)
{
	gmabuf_t *oldone = gma->genv;
	gma->genv = newone;
	newone->lvarlst = kevalmod->lvarlst;
	newone->lvarlst_top = kArray_size(kevalmod->lvarlst);
	DBG_P("push lvarlst.. size=%d", newone->lvarlst_top);
	return oldone;
}

static gmabuf_t *Gamma_pop(CTX, kGamma *gma, gmabuf_t *oldone, gmabuf_t *checksum)
{
	gmabuf_t *newone = gma->genv;
	assert(checksum == newone);
	gma->genv = oldone;
	DBG_P("pop lvarlst.. size%d=>%d", kArray_size(newone->lvarlst), newone->lvarlst_top);
	kArray_clear(newone->lvarlst, newone->lvarlst_top);
	return newone;
}

#define GAMMA_PUSH(G,B) \
	gmabuf_t *oldbuf_ = Gamma_push(_ctx, G, B);

#define GAMMA_POP(G,B) \
	Gamma_pop(_ctx, G, oldbuf_, B);

// --------------------------------------------------------------------------

static kline_t Expr_uline(CTX, kExpr *expr, int* lpos)
{
	kToken *tk = expr->tkNUL;
	kArray *a = expr->consNUL;
	if(tk != NULL && tk->uline > 0) {
		return tk->uline;
	}
	if(a != NULL && IS_Array(a)) {
		size_t i;
		for(i=0; i < kArray_size(a); i++) {
			tk = a->tts[i];
			if(IS_Token(tk) && tk->uline > 0) {
				return tk->uline;
			}
			if(IS_Expr(tk)) {
				kline_t uline = Expr_uline(_ctx, a->exprs[i], lpos);
				if(uline > 0) return uline;
			}
		}
	}
	DBG_P("@@@@@@@@ Cannot Find Uline @@@@@@@@@@@");
	return 0;
}

static kExpr *Expr_p(CTX, kExpr *expr, int pe, const char *fmt, ...)
{
	int lpos = -1;
	kline_t uline = Expr_uline(_ctx, expr, &lpos);
	va_list ap;
	va_start(ap, fmt);
	vperrorf(_ctx, pe, uline, lpos, fmt, ap);
	va_end(ap);
	return K_NULLEXPR;
}

static kExpr *Expr_typed(CTX, kExpr *expr, kGamma *gma, int req_ty)
{
	ksyntax_t *syn;
	if(Expr_isTerm(expr)) {
		kToken *tk = expr->tkNUL;
		keyword_t keyid = tk->tt == TK_KEYWORD ? tk->keyid : KW_TK(tk->tt);
		syn = KonohaSpace_syntax(_ctx, gma->genv->ks, keyid, 0);
		if(syn == NULL || syn->ExprTyCheck == NULL) {
			return kExpr_p(expr, ERR_, "undefined term type checker: %s %s", T_tt(tk->tt), kToken_s(tk));
		}
	}
	else {
		syn = expr->syn;
		if(syn->ExprTyCheck == NULL) {
			return kExpr_p(expr, ERR_, "undefined expression type checker: %s", syn->token);
		}
	}
	{
		BEGIN_LOCAL(lsfp, 3);
		KSETv(lsfp[K_CALLDELTA+0].o, (kObject*)expr);
		KSETv(lsfp[K_CALLDELTA+1].o, (kObject*)gma);
		lsfp[K_CALLDELTA+2].ivalue = req_ty;
		KCALL(lsfp, 0, syn->ExprTyCheck, 3);
		END_LOCAL();
		DBG_ASSERT(IS_Expr(lsfp[0].o));
		return (kExpr*)lsfp[0].o;
	}
}

static kObject *ConstValue(CTX, kObject *o)
{
	if(IS_Expr(o)) {
		kExpr *expr = (kExpr*)o;
		if(expr->build == TEXPR_CONST) {
			return expr->dataNUL;
		}
		if(expr->build == TEXPR_NEW) {
			return new_kObject(CT_(expr->ty), NULL);
		}
		assert(expr->build == TEXPR_NULL);
	}
	return o;
}

static kExpr* ExprCall_toConstValue(CTX, kExpr *expr, kArray *cons, ktype_t rtype)
{
	size_t i, size = kArray_size(cons), psize = size - 2;
	kMethod *mtd = cons->methods[0];
	BEGIN_LOCAL(lsfp, K_CALLDELTA + psize);
	KSETv(lsfp[K_CALLDELTA+0].o, (kObject*)expr);
	DBG_P("CallExpr is turned into a Const value");
	for(i = 1; i < size; i++) {
		kObject *o = ConstValue(_ctx, cons->list[i]);
		KSETv(lsfp[K_CALLDELTA + i - 1].o, o);
		lsfp[K_CALLDELTA + i - 1].ivalue = N_toint(o);
	}
	KCALL(lsfp, 0, mtd, psize);
	END_LOCAL();
	if(TY_isUnbox(rtype) || rtype == TY_void) {
		return kExpr_setNConstValue(expr, rtype, lsfp[0].ndata);
	}
	return kExpr_setConstValue(expr, rtype, lsfp[0].o);
}

static int param_policy(ksymbol_t fn)
{
	return 0;
}

static kExpr *Expr_tycheck(CTX, kExpr *expr, kGamma *gma, ktype_t req_ty, int pol)
{
	kExpr *texpr = K_NULLEXPR;
	if(expr->ty == TY_var && expr != K_NULLEXPR) {
		if(!IS_Expr(expr)) {
			expr = new_ConstValue(O_cid(expr), expr);
			PUSH_GCSTACK(expr);
		}
		texpr = Expr_typed(_ctx, expr, gma, req_ty);
	}
	if(texpr != K_NULLEXPR) {
		if(texpr->ty == TY_void && !FLAG_is(pol, TPOL_ALLOWVOID)) {
			return kExpr_p(expr, ERR_, "void is not acceptable");
		}
		if(FLAG_is(pol, TPOL_NOCHECK) || texpr->ty == req_ty ) {
			return texpr;
		}
		if(req_ty == TY_var) {
			return texpr;
		}
		kMethod *mtd = kKonohaSpace_getCastMethodNULL(gma->genv->ks, texpr->ty, req_ty);
		if(mtd != NULL && (kMethod_isCoercion(mtd) || FLAG_is(pol, TPOL_COERCION))) {
			kExpr *Cexpr = new_ConsExpr(_ctx, SYN_CALL, 2, mtd, texpr);
			Cexpr->ty = req_ty;
			if(kMethod_isConst(mtd) && (Cexpr->build == TEXPR_CONST || Cexpr->build == TEXPR_NCONST)) {
				return ExprCall_toConstValue(_ctx, Cexpr, Cexpr->consNUL, req_ty);
			}
			return Cexpr;
		}
		return K_NULLEXPR;
	}
	return texpr;
}

static kExpr* Expr_tyCheckAt(CTX, kExpr *exprP, size_t pos, kGamma *gma, ktype_t req_ty, int pol)
{
	if(!Expr_isTerm(exprP) && pos < kArray_size(exprP->consNUL)) {
		kExpr *expr = exprP->consNUL->exprs[pos];
		expr = Expr_tycheck(_ctx, expr, gma, req_ty, pol);
		KSETv(exprP->consNUL->exprs[pos], expr);
		return expr;
	}
	return K_NULLEXPR;
}
static kbool_t Stmt_tyCheckExpr(CTX, kStmt *stmt, keyword_t nameid, kGamma *gma, ktype_t req_ty, int pol)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, nameid);
	if(expr != NULL && IS_Expr(expr)) {
		kExpr *texpr = Expr_tycheck(_ctx, expr, gma, req_ty, pol);
		DBG_P("req_ty=%s, texpr->ty=%s isnull=%d", T_cid(req_ty), T_cid(texpr->ty), (texpr == K_NULLEXPR));
		if(texpr != K_NULLEXPR) {
			if(texpr != expr) {
				kObject_setObject(stmt, nameid, texpr);
			}
			return 1;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

static KMETHOD TokenTyCheck_TEXT(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	kToken *tk = expr->tkNUL;
	RETURN_(kExpr_setConstValue(expr, TY_String, tk->text));
}

static KMETHOD TokenTyCheck_NULL(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	if(req_ty == TY_var) req_ty = CLASS_Object;
	RETURN_(kExpr_setVariable(expr, TEXPR_NULL, req_ty, 0, 0, gma));
}

static KMETHOD TokenTyCheck_TYPE(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	RETURN_(kExpr_setVariable(expr, TEXPR_NULL, expr->tkNUL->ty, 0, 0, gma));
}

static KMETHOD TokenTyCheck_TRUE(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	RETURN_(kExpr_setNConstValue(expr, TY_Boolean, (uintptr_t)1));
}

static KMETHOD TokenTyCheck_FALSE(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	RETURN_(kExpr_setNConstValue(expr, TY_Boolean, (uintptr_t)0));
}

static KMETHOD TokenTyCheck_INT(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	kToken *tk = expr->tkNUL;
	long long n = strtoll(S_text(tk->text), NULL, 0);
	RETURN_(kExpr_setNConstValue(expr, TY_Int, (uintptr_t)n));
}

static KMETHOD TokenTyCheck_FLOAT(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	RETURN_(kToken_p(expr->tkNUL, ERR_, "float is unsupported: %s", kToken_s(expr->tkNUL)));
}

static kExpr* Expr_tyCheckVariable(CTX, kExpr *expr, kGamma *gma)
{
	DBG_ASSERT(expr->ty == TY_var);
	kToken *tk = expr->tkNUL;
	ksymbol_t fn = ksymbol(S_text(tk->text), S_size(tk->text), FN_NONAME, SYMPOL_NAME);
	int i;
	gmabuf_t *genv = gma->genv;
	for(i = genv->lvarsize - 1; i >= 0; i--) {
		if(genv->lvars[i].fn == fn) {
			expr->build = TEXPR_BLOCKLOCAL_;
			expr->ty = genv->lvars[i].ty;
			expr->index = i;
			DBG_P("index=%d, expr %p", kArray_size(genv->lvarlst), expr);
			kArray_add(genv->lvarlst, expr);
			return expr;
		}
	}
	for(i = genv->fvarsize - 1; i >= 0; i--) {
		if(genv->fvars[i].fn == fn) {
			expr->build = TEXPR_LOCAL;
			expr->ty = genv->fvars[i].ty;
			expr->index = i;
			return expr;
		}
	}
	return kToken_p(tk, ERR_, "undefined variable: %s", kToken_s(tk));
}

static KMETHOD TokenTyCheck_SYMBOL(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	RETURN_(Expr_tyCheckVariable(_ctx, expr, gma));
}

static KMETHOD TokenTyCheck_USYMBOL(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	kToken *tk = expr->tkNUL;
	kObject *v = KonohaSpace_getSymbolValueNULL(_ctx, gma->genv->ks, S_text(tk->text), S_size(tk->text));
	kExpr *texpr = (v == NULL) ?
		kToken_p(tk, ERR_, "undefined symbol: %s", kToken_s(tk)) : kExpr_setConstValue(expr, O_cid(v), v);
	RETURN_(texpr);
}


static ktype_t ktype_var(CTX, ktype_t ty, kcid_t cid)
{
	return ty;
}

static kExpr *Cons_tycheckParams(CTX, kExpr *expr, ktype_t cid, kGamma *gma, ktype_t req_ty)
{
	kArray *cons = expr->consNUL;
	size_t i, size = kArray_size(cons);
	kMethod *mtd = cons->methods[0];
	if(IS_Method(mtd)) {
		kParam *pa = mtd->pa;
		ktype_t rtype = ktype_var(_ctx, pa->rtype, cid);
		int isConst = 1;
	//	if(rtype == TY_var && gma->genv->mtd == mtd) {
	//		return ERROR_Unsupported(_ctx, "type inference of recursive calls", CLASS_unknown, NULL);
	//	}
		if(pa->psize + 2 != size) {
			char mbuf[128];
			return kExpr_p(expr, ERR_, "%s.%s takes %d parameter(s), but given %d parameter(s)", T_cid(cid), T_mn(mbuf, mtd->mn), (int)pa->psize, (int)size-2);
		}
		for(i = 0; i < pa->psize; i++) {
			size_t n = i + 2;
			ktype_t ptype = ktype_var(_ctx, pa->p[i].ty, cid);
			int pol = param_policy(pa->p[i].fn);
			kExpr *texpr = kExpr_tyCheckAt(expr, n, gma, ptype, pol);
			if(texpr == K_NULLEXPR) {
				char mbuf[128];
				return kExpr_p(expr, ERR_, "%s.%s accepts %s at the parameter %d", T_cid(cid), T_mn(mbuf, mtd->mn), T_ty(ptype), (int)i+1);
			}
			if(texpr->build != TEXPR_CONST && texpr->build != TEXPR_NEW) isConst = 0;
		}
		if(isConst && kMethod_isConst(mtd)) {
			return ExprCall_toConstValue(_ctx, expr, cons, rtype);
		}
		else {
			expr->build = TEXPR_CALL;
			expr->ty = rtype;
		}
		return expr;
	}
	return kExpr_p(expr, ERR_, "method was not found");
}

static void Cons_setMethod(CTX, kExpr *expr, kcid_t this_cid, kGamma *gma)
{
	kKonohaSpace *ns = gma->genv->ks;
	kToken *tkMN = expr->consNUL->tts[0];
	kMethod *mtd = NULL;
	if(!IS_Token(tkMN)) {
		kExpr *expr0 = (kExpr*)tkMN;
		if(IS_Expr(expr0) && Expr_isTerm(expr0)) {
			tkMN = expr0->tkNUL;
		}
		else {
			DBG_P("THIS NOT HAPPEN");
			abort();
			return;
		}
	}
	if(tkMN->tt == TK_SYMBOL || tkMN->tt == TK_USYMBOL) {
		tkMN->tt = TK_MN;
		tkMN->mn = ksymbol(S_text(tkMN->text), S_size(tkMN->text), FN_NEWID, SYMPOL_METHOD);
	}
	DBG_P("finding %s.%s", T_cid(this_cid), S_text(tkMN->text));
	if(tkMN->tt == TK_KEYWORD) {
		if(kArray_size(expr->consNUL) == 3) {
			mtd = kKonohaSpace_getMethodNULL(ns, this_cid, expr->syn->op2);
			if(mtd == NULL) {
				kToken_p(tkMN, ERR_, "undefined binary operator: %s of %s", S_text(tkMN->text), T_cid(this_cid));
			}
			goto L_RETURN;
		}
		if(kArray_size(expr->consNUL) == 2) {
			mtd = kKonohaSpace_getMethodNULL(ns, this_cid, expr->syn->op1);
			if(mtd == NULL) {
				kToken_p(tkMN, ERR_, "undefined uninary operator: %s of %s", S_text(tkMN->text), T_cid(this_cid));
			}
			goto L_RETURN;
		}
	}
	if(tkMN->tt == TK_MN) {
		mtd = kKonohaSpace_getMethodNULL(ns, this_cid, tkMN->mn);
		if(mtd == NULL) {
			kToken_p(tkMN, ERR_, "undefined method: %s.%s", T_cid(this_cid), kToken_s(tkMN));
		}
	}
	L_RETURN:;
	if(mtd != NULL) {
		KSETv(expr->consNUL->methods[0], mtd);
	}
}

static KMETHOD ExprTyCheck_call(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	DBG_P("call: size=%d", kArray_size(expr->consNUL));
	kExpr *texpr = kExpr_tyCheckAt(expr, 1, gma, TY_var, 0);
	if(texpr == K_NULLEXPR) {
		RETURN_(texpr);
	}
	kcid_t this_cid = texpr->ty;
	DBG_P("this_cid=%s", T_cid(this_cid));
	Cons_setMethod(_ctx, expr, this_cid, gma);
	RETURN_(Cons_tycheckParams(_ctx, expr, this_cid, gma, req_ty));
}

static KMETHOD ExprTyCheck_invoke(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	kArray *cons = expr->consNUL;
	DBG_P("invoke: size=%d", kArray_size(cons));
	DBG_ASSERT(cons->list[1] == K_NULL);
	kcid_t this_cid = CLASS_unknown;
	if(IS_Expr(cons->list[0])) {
		kToken *tk = cons->exprs[0]->tkNUL;
		if(tk->tt == TK_SYMBOL || tk->tt == TK_USYMBOL) {
			tk->tt = TK_MN;
			tk->symbol = ksymbol(S_text(tk->text), S_size(tk->text), FN_NEWID, SYMPOL_METHOD);
		}
		if(tk->tt == TK_MN) {
			kMethod *mtd = NULL;
			if(true/*FIXME gma->genv->this_cid != 0*/) {
				mtd = kKonohaSpace_getMethodNULL(gma->genv->ks, gma->genv->this_cid, tk->mn);
				if(mtd != NULL) {
					if(!kMethod_isStatic(mtd)) {
						KSETv(cons->tts[1], new_Variable(TEXPR_LOCAL, gma->genv->this_cid, 0, 0, gma));
						this_cid = gma->genv->this_cid;
					}
				}
			}
			if(mtd == NULL) {
				mtd = kKonohaSpace_getStaticMethodNULL(gma->genv->ks, tk->mn);
			}
			if(mtd == NULL) {
				RETURN_(kToken_p(tk, ERR_, "undefined function (method): %s", kToken_s(tk)));
			} else {
				KSETv(cons->methods[0], mtd);
			}
		}
	}
	if(IS_Method(cons->methods[0])) {
		if(this_cid == CLASS_unknown) this_cid = cons->methods[0]->cid;
		RETURN_(Cons_tycheckParams(_ctx, expr, this_cid, gma, req_ty));
	}
	else {
		RETURN_(kExpr_p(expr, ERR_, "must be a function name"));
	}
}

static kExpr *Expr_typed(CTX, kExpr *expr, kGamma *gma, int req_ty);

static KMETHOD ExprTyCheck_getter(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, gma, req_ty);
	DBG_P("getter: size=%d", kArray_size(expr->consNUL));
	kToken *tk = expr->consNUL->tts[1];
	RETURN_(kExpr_p(expr, ERR_, "HELP!! Somebody fix this!!"));
}

static KMETHOD StmtTyCheck_err(CTX, ksfp_t *sfp _RIX)  // $expr
{
	RETURNb_(1);
}

static KMETHOD StmtTyCheck_expr(CTX, ksfp_t *sfp _RIX)  // $expr
{
	VAR_StmtTyCheck(stmt, gma);
	kbool_t r = Stmt_tyCheckExpr(_ctx, stmt, 1, gma, TY_var, TPOL_NOCHECK);
	stmt->syn = SYN_EXPR;
	stmt->build = TSTMT_EXPR;
	RETURNb_(r);
}

static kbool_t Stmt_TyCheck(CTX, ksyntax_t *syn, kStmt *stmt, kGamma *gma)
{
	BEGIN_LOCAL(lsfp, 5);
	KSETv(lsfp[K_CALLDELTA+0].o, (kObject*)stmt);
	KSETv(lsfp[K_CALLDELTA+1].o, (kObject*)gma);
	KCALL(lsfp, 0, syn->StmtTyCheck, 1);
	END_LOCAL();
	return lsfp[0].bvalue;
}

static kbool_t Block_tyCheckAll(CTX, kBlock *bk, kGamma *gma)
{
	int i, result = 1, lvarsize = gma->genv->lvarsize;
	for(i = 0; i < kArray_size(bk->blockS); i++) {
		kStmt *stmt = (kStmt*)bk->blockS->list[i];
		ksyntax_t *syn = stmt->syn;
		DBG_P("i=%d, syn=%p", i, syn);
		dumpStmt(_ctx, stmt);
		if(syn == NULL) continue;
		int estart = kerrno;
		if(syn->StmtTyCheck == NULL) {
			SUGAR_P(ERR_, stmt->uline, 0, "undefined statement type checker: %s", syn->token);
			kStmt_toERR(stmt, estart);
		}
		else if(!Stmt_TyCheck(_ctx, syn, stmt, gma)) {
			kStmt_toERR(stmt, estart);
		}
		if(stmt->syn == SYN_ERR) {
			result = 0;
			break;
		}
	}
	if(bk != K_NULLBLOCK) {
		DBG_P("bk=%p esp=%p ty=%s", bk, bk->esp, T_cid(bk->esp->ty));
		kExpr_setVariable(bk->esp, TEXPR_BLOCKLOCAL_, TY_void, gma->genv->lvarsize, 0, gma);
	}
	if(lvarsize < gma->genv->lvarsize) {
		gma->genv->lvarsize = lvarsize;
	}
	return result;
}

static void Stmt_toBlockStmt(CTX, kStmt *stmt, kBlock *bk)
{
	kObject_setObject(stmt, KW_BLOCK, bk);
	stmt->build = TSTMT_BLOCK;
}

static kBlock *Stmt_getBlock(CTX, kStmt *stmt, ksymbol_t nameid, kBlock *bkdef)
{
	kBlock *bk = (kBlock*) kObject_getObjectNULL(stmt, nameid);
	if(bk != NULL) {
		if(IS_Token(bk)) {
			kToken *tk = (kToken*)bk;
			if (tk->tt == TK_CODE) {
				kToken_toBRACE(_ctx, tk);
			}
			if (tk->tt == AST_BRACE) {
				bk = new_Block(_ctx, tk->sub, 0, kArray_size(tk->sub), kStmt_ks(stmt));
				kObject_setObject(stmt, nameid, bk);
			}
		}
		if(IS_Block(bk)) {
			return bk;
		}
	}
	return bkdef;
}

static KMETHOD StmtTyCheck_if(CTX, ksfp_t *sfp _RIX)
{
	kbool_t r = 1;
	VAR_StmtTyCheck(stmt, gma);
	if((r = Stmt_tyCheckExpr(_ctx, stmt, KW_EXPR, gma, TY_Boolean, 0))) {
		kExpr *exprCond = (kExpr*)kObject_getObjectNULL(stmt, KW_EXPR);
		kBlock *bkThen = Stmt_getBlock(_ctx, stmt, KW_THEN, K_NULLBLOCK);
		kBlock *bkElse = Stmt_getBlock(_ctx, stmt, KW_ELSE, K_NULLBLOCK);
		if(exprCond->build == TEXPR_NCONST) {
			if(exprCond->ndata) {
				Block_tyCheckAll(_ctx, bkThen, gma);
				Stmt_toBlockStmt(_ctx, stmt, bkThen);
			}
			else {
				Block_tyCheckAll(_ctx, bkElse, gma);
				Stmt_toBlockStmt(_ctx, stmt, bkElse);
			}
		}
		else {
			Block_tyCheckAll(_ctx, bkThen, gma);
			Block_tyCheckAll(_ctx, bkElse, gma);
			stmt->build = TSTMT_IF;
		}
	}
	RETURNb_(r);
}

static KMETHOD StmtTyCheck_return(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtTyCheck(stmt, gma);
	kbool_t r = 0;
	stmt->build = TSTMT_RETURN;
	assert (gma->genv->mtd != NULL);
	r = Stmt_tyCheckExpr(_ctx, stmt, KW_EXPR, gma, gma->genv->mtd->pa->rtype, 0);
	RETURNb_(r);
}

///* ------------------------------------------------------------------------ */

//static void Stmt_toExprCall(CTX, kStmt *stmt, kMethod *mtd, int n, ...)
//{
//	kExpr *expr = new_ConsExpr(_ctx, SYN_CALL, 0);
//	int i;
//	va_list ap;
//	va_start(ap, n);
//	for(i = 0; i < n; i++) {
//		kObject *v =  (kObject*)va_arg(ap, kObject*);
//		assert(v != NULL);
//		kArray_add(expr->consNUL, v);
//	}
//	va_end(ap);
//	kObject_setObject(stmt, 1, expr);
//	stmt->syn = SYN_EXPR;
//	stmt->build = TSTMT_EXPR;
//}

///* ------------------------------------------------------------------------ */
///* [MethodDecl] */

static flagop_t MethodDeclFlag[] = {
	{AKEY("@Virtual"),    kMethod_Virtual},
	{AKEY("@Public"),     kMethod_Public},
	{AKEY("@Const"),      kMethod_Const},
	{AKEY("@Static"),     kMethod_Static},
	{AKEY("@Restricted"), kMethod_Restricted},
	{NULL},
};

static kcid_t Stmt_getcid(CTX, kStmt *stmt, kKonohaSpace *ns, keyword_t keyid, kcid_t defcid)
{
	kToken *tk = (kToken*)kObject_getObjectNULL(stmt, keyid);
	if(tk == NULL || !IS_Token(tk)) {
		return defcid;
	}
	else {
		assert(tk->tt == TK_TYPE);
		return tk->ty;
	}
}

static kcid_t Stmt_getmn(CTX, kStmt *stmt, kKonohaSpace *ns, keyword_t keyid, kmethodn_t defmn)
{
	kToken *tk = (kToken*)kObject_getObjectNULL(stmt, keyid);
	if(tk == NULL || !IS_Token(tk) || !IS_String(tk->text)) {
		return defmn;
	}
	else {
		DBG_ASSERT(IS_String(tk->text));
		return ksymbol(S_text(tk->text), S_size(tk->text), FN_NEWID, SYMPOL_METHOD);
	}
}

static kParam *Stmt_newMethodParam(CTX, kStmt *stmt, kGamma* gma)
{
	kParam *pa = (kParam*)kObject_getObjectNULL(stmt, KW_PARAMS);
	if(pa == NULL || !IS_Param(pa)) {
		if(!Stmt_TyCheck(_ctx, SYN_PARAMS, stmt, gma)) {
			return NULL;
		}
	}
	pa = (kParam*)kObject_getObjectNULL(stmt, KW_PARAMS);
	DBG_ASSERT(IS_Param(pa));
	return pa;
}

static kbool_t Method_compile(CTX, kMethod *mtd, kString *text, kline_t uline, kKonohaSpace *ks);

static KMETHOD Fmethod_lazyCompilation(CTX, ksfp_t *sfp _RIX)
{
	ksfp_t *esp = _ctx->esp;
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	kString *text = mtd->tcode->text;
	kline_t uline = mtd->tcode->uline;
	kKonohaSpace *ns = mtd->lazyns;
	Method_compile(_ctx, mtd, text, uline, ns);
	((kcontext_t*)_ctx)->esp = esp;
	mtd->fcall_1(_ctx, sfp K_RIXPARAM); // call again;
}

static void Stmt_setMethodFunc(CTX, kStmt *stmt, kKonohaSpace *ns, kMethod *mtd)
{
	if(kStmt_is(stmt, KW_("@Glue"))) {
		const char *funcname = kStmt_text(stmt, KW_("@Glue"), NULL);
		char namebuf[128], mbuf[128];
		if(funcname != NULL) {
			const char *mname = kStmt_text(stmt, KW_("$name"), NULL);
			if(mname == NULL) mname = T_mn(mbuf, mtd->mn);
			snprintf(namebuf, sizeof(namebuf), "%s_%s", T_cid(mtd->cid), mname);
			funcname = (const char*)namebuf;
		}
		knh_Fmethod f = kKonohaSpace_loadGlueFunc(ns, funcname, 1, stmt->uline);
		if(f != NULL) {
			kMethod_setFunc(mtd, f);
		}
	}
	else {
		kToken *tcode = (kToken*)kObject_getObjectNULL(stmt, KW_("$block"));
		if(tcode != NULL && IS_Token(tcode) && tcode->tt == TK_CODE) {
			KSETv(mtd->tcode, tcode);
			KSETv(mtd->lazyns, ns);
			//kevalmod->gma->genv->static_cid = mtd->cid;
			//ns->static_cid = (kevalmod->gma->genv)->static_cid;
			kMethod_setFunc(mtd, Fmethod_lazyCompilation);
			kArray_add(kevalmod->definedMethods, mtd);
		}
	}
}

static KMETHOD StmtTyCheck_declMethod(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtTyCheck(stmt, gma);
	kbool_t r = 0;
	kKonohaSpace *ns = gma->genv->ks;
	uintptr_t flag =  Stmt_flag(_ctx, stmt, MethodDeclFlag, 0);
	kcid_t cid =  Stmt_getcid(_ctx, stmt, ns, KW_("$cname"), ns->function_cid);
	kmethodn_t mn = Stmt_getmn(_ctx, stmt, ns, KW_("$name"), MN_("new"));
	kParam *pa = Stmt_newMethodParam(_ctx, stmt, gma);
	if(TY_isSingleton(cid)) flag |= kMethod_Static;
	if(pa != NULL) {
		INIT_GCSTACK();
		kMethod *mtd = new_kMethod(flag, cid, mn, pa, NULL);
		PUSH_GCSTACK(mtd);
		if(kKonohaSpace_addMethod(ns, mtd, stmt->uline)) {
			r = 1;
			Stmt_setMethodFunc(_ctx, stmt, ns, mtd);
			kStmt_done(stmt);
		}
		RESET_GCSTACK();
	}
	RETURNb_(r);
}

static kbool_t Expr_setParam(CTX, kExpr *expr, int n, kparam_t *p)
{
	int i = 0;
	if(!IS_Expr(expr)) goto L_ERROR;
	if(Expr_isTerm(expr)) goto L_ERROR;
	if(kArray_size(expr->consNUL) != 3) goto L_ERROR;
	kArray *cons = expr->consNUL;
	if(!IS_Expr(cons->exprs[1])) goto L_ERROR;
	kToken *tkOP = cons->tts[0];
	kToken *tkN = cons->exprs[1]->tkNUL;
	kToken *tkT = cons->tts[2];
	if(!IS_Token(tkOP) || tkOP->tt != TK_KEYWORD || tkOP->keyid != KW_COLON) goto L_ERROR;
	if(tkN == NULL || !IS_Token(tkN) || tkN->tt != TK_SYMBOL) goto L_ERROR;
	if(!IS_Token(tkT) || tkT->tt != TK_TYPE) goto L_ERROR;
	ksymbol_t fn = ksymbol(S_text(tkN->text), S_size(tkN->text), FN_NEWID, SYMPOL_NAME);
	for(i = 0; i < n; i++) {
		if(p[i].fn == fn) {
			kToken_p(tkN, ERR_, "duplicated definition: %s", kToken_s(tkN));
			return 0;
		}
	}
	p[n].fn = fn;
	p[n].ty = tkT->ty;
	return 1;

	L_ERROR:;
	kExpr_p(expr, ERR_, "syntax error at the parameter %d", (int)i+1);
	return 0;
}

static KMETHOD StmtTyCheck_declParams(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtTyCheck(stmt, gma);
	kKonohaSpace *ns = gma->genv->ks;
	ktype_t rtype =  Stmt_getcid(_ctx, stmt, ns, KW_("$type"), TY_var);
	kParam *pa = NULL;
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, KW_PARAMS);
	if(expr == NULL) {
		pa = (rtype == TY_void) ? K_NULLPARAM : new_kParam(rtype, 0, NULL);
	}
	else if(IS_Param(expr)) {
		pa = (kParam*)expr;
	}
	else if(IS_Expr(expr)) {
		kArray *params = expr->consNUL;
		size_t i, psize = kArray_size(params);
		kparam_t p[psize];
		for(i = 0; i < psize; i++) {
			if(!Expr_setParam(_ctx, params->exprs[i], i, p)) {
				RETURNb_(0);
			}
		}
		pa = new_kParam(rtype, psize, p);
	}
	kObject_setObject(stmt, KW_PARAMS, pa);
	RETURNb_(1);
}

static kBlock* Method_newBlock(CTX, kMethod *mtd, kString *source, kline_t uline)
{
	const char *script = S_text(source);
	if(IS_NULL(source) || script[0] == 0) {
		DBG_ASSERT(IS_Token(mtd->tcode));
		script = S_text(mtd->tcode->text);
		uline = mtd->tcode->uline;
	}
	kArray *tls = kevalmod->tokens;
	size_t pos = kArray_size(tls);
	ktokenize(_ctx, script, uline, _TOPLEVEL, tls);
	kBlock *bk = new_Block(_ctx, tls, pos, kArray_size(tls), kevalshare->rootks);
	kArray_clear(tls, pos);
	return bk;
}

static void Gamma_initParam(CTX, gmabuf_t *genv, kParam *pa)
{
	int i, psize = (pa->psize + 1 < genv->fcapacity) ? pa->psize : genv->fcapacity - 1;
	for(i = 0; i < psize; i++) {
		genv->fvars[i+1].fn = pa->p[i].fn;
		genv->fvars[i+1].ty = pa->p[i].ty;
	}
	if(!kMethod_isStatic(genv->mtd)) {
		genv->fvars[0].fn = FN_this;
		genv->fvars[0].ty = genv->this_cid;
	}
	genv->fvarsize = psize+1;
}

static void Gamma_shiftBlockIndex(CTX, gmabuf_t *genv)
{
	kArray *a = genv->lvarlst;
	size_t i, size = kArray_size(a);
	int shift = genv->fvarsize;
	for(i = genv->lvarlst_top; i < size; i++) {
		kExpr *expr = a->exprs[i];
		DBG_P("i=%d expr %p", i, expr);
		DBG_ASSERT(expr->build == TEXPR_BLOCKLOCAL_);
		expr->index += shift;
		expr->build = TEXPR_LOCAL;
	}
}

static kbool_t Method_compile(CTX, kMethod *mtd, kString *text, kline_t uline, kKonohaSpace *ks)
{
	INIT_GCSTACK();
	kGamma *gma = kevalmod->gma;
	kBlock *bk = Method_newBlock(_ctx, mtd, text, uline);
	gammastack_t fvars[32] = {}, lvars[32] = {};
	gmabuf_t newgma = {
		.mtd = mtd,
		.ks = ks,
		.this_cid = (mtd)->cid,
		.fvars = fvars, .fcapacity = 32,
		.lvars = lvars, .lcapacity = 32,
	};
	GAMMA_PUSH(gma, &newgma);
	Gamma_initParam(_ctx, &newgma, mtd->pa);
	Block_tyCheckAll(_ctx, bk, gma);
	Gamma_shiftBlockIndex(_ctx, &newgma);
	MODCODE_genCode(_ctx, mtd, bk);
	GAMMA_POP(gma, &newgma);
	RESET_GCSTACK();
	return 1;
}

/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
// eval

static void Gamma_initIt(CTX, gmabuf_t *genv, kParam *pa)
{
	kevalmod_t *base = kevalmod;
	genv->fvarsize = 0;
	if(base->evalty != TY_void) {
		genv->fvars[1].fn = FN_("it");
		genv->fvars[1].ty = base->evalty;
		genv->fvarsize = 1;
	}
}

static kstatus_t Method_runEval(CTX, kMethod *mtd, ktype_t rtype)
{
	BEGIN_LOCAL(lsfp, K_CALLDELTA);
	kevalmod_t *base = kevalmod;
	kstatus_t result = K_CONTINUE;
	DBG_P("TY=%s, running EVAL..", T_cid(rtype));
	if(base->evalty != TY_void) {
		KSETv(lsfp[K_CALLDELTA+1].o, _ctx->stack->stack[base->evalidx].o);
		lsfp[K_CALLDELTA+1].ivalue = _ctx->stack->stack[base->evalidx].ivalue;
	}
	KCALL(lsfp, 0, mtd, 0);
	if(rtype != TY_void) {
		base->evalty = rtype;
		base->evalidx = (lsfp - _ctx->stack->stack);
	}
	END_LOCAL();
	return result;
}

void MODEVAL_dumpEval(CTX, kwb_t *wb)
{
	kevalmod_t *base = kevalmod;
	ktype_t ty = base->evalty;
	if(ty != TY_void) {
		ksfp_t *lsfp = _ctx->stack->stack + base->evalidx;
		CT_(ty)->p(_ctx, lsfp, 0, wb, KP_DUMP);
		fflush(stdout);
		fprintf(stdout, "TY=%s EVAL=%s\n", T_cid(ty), kwb_top(wb,1));
	}
}

static ktype_t Stmt_checkReturnType(CTX, kStmt *stmt)
{
	if(stmt->syn == SYN_EXPR) {
		kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, 1);
		DBG_ASSERT(expr != NULL);
		if(expr->ty != TY_void) {
			stmt->syn = SYN_RETURN;
			stmt->build = TSTMT_RETURN;
			return expr->ty;
		}
	}
	return TY_void;
}

static ktype_t Gamma_evalMethod(CTX, kGamma *gma, kBlock *bk, kMethod *mtd)
{
	kStmt *stmt = bk->blockS->stmts[0];
	if(stmt->syn == NULL) return K_CONTINUE;
	if(stmt->syn == SYN_BREAK) return K_BREAK;
	if(stmt->syn == SYN_ERR) return K_FAILED;
	ktype_t rtype = Stmt_checkReturnType(_ctx, stmt);
	MODCODE_genCode(_ctx, mtd, bk);
	return Method_runEval(_ctx, mtd, rtype);
}

static kstatus_t SingleBlock_eval(CTX, kBlock *bk, kMethod *mtd, kKonohaSpace *ks)
{
	kGamma *gma = kevalmod->gma;
	gammastack_t fvars[32] = {}, lvars[32] = {};
	gmabuf_t newgma = {
		.mtd = mtd,
		.ks = ks,
		.this_cid     = (mtd)->cid,
		.fvars = fvars, .fcapacity = 32,
		.lvars = lvars, .lcapacity = 32,
	};
	GAMMA_PUSH(gma, &newgma);
	Gamma_initIt(_ctx, &newgma, mtd->pa);
	Block_tyCheckAll(_ctx, bk, gma);
	Gamma_shiftBlockIndex(_ctx, &newgma);
	kstatus_t result = Gamma_evalMethod(_ctx, gma, bk, mtd);
	GAMMA_POP(gma, &newgma);
	return result;
}

#define EVALJMP_FAILED        1
#define EVALJMP_SKIP          2

static void Kraise(CTX, int isContinue)
{
	kevalmod_t *base = kevalmod;
	klongjmp(*base->evaljmpbuf, isContinue ? EVALJMP_SKIP : EVALJMP_FAILED);
}

static kstatus_t Block_eval(CTX, kBlock *bk)
{
	INIT_GCSTACK();
	BEGIN_LOCAL(lsfp, 0);
	kBlock *bk1 = kevalmod->singleBlock;
	kMethod *mtd = new_kMethod(kMethod_Static, 0, 0, K_NULLPARAM, NULL);
	PUSH_GCSTACK(mtd);
	kevalmod_t *base = kevalmod;
	kstatus_t result = K_CONTINUE;
	kjmpbuf_t lbuf = {};
	int i, jmpresult;
	memcpy(&lbuf, base->evaljmpbuf, sizeof(kjmpbuf_t));
	if((jmpresult = ksetjmp(*base->evaljmpbuf)) == 0) {
		for(i = 0; i < kArray_size(bk->blockS); i++) {
			KSETv(bk1->blockS->list[0], bk->blockS->list[i]);
			KSETv(bk1->ks, bk->ks);
			result = SingleBlock_eval(_ctx, bk1, mtd, bk->ks);
			//kflag_clear(kevalmod->flags);
			if(result == K_FAILED) break;
		}
	}
	else {
		DBG_P("Catch eval exception jmpresult=%d", jmpresult);
		base->evalty = TY_void;
		result = jmpresult == EVALJMP_SKIP ? K_BREAK : K_FAILED;
	}
	memcpy(base->evaljmpbuf, &lbuf, sizeof(kjmpbuf_t));
	END_LOCAL();
	RESET_GCSTACK();
	return result;
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
