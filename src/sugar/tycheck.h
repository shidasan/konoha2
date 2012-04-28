/****************************************************************************
 * KONOHA2 COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
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
	newone->lvarlst = ctxsugar->lvarlst;
	newone->lvarlst_top = kArray_size(ctxsugar->lvarlst);
	return oldone;
}

static gmabuf_t *Gamma_pop(CTX, kGamma *gma, gmabuf_t *oldone, gmabuf_t *checksum)
{
	gmabuf_t *newone = gma->genv;
	assert(checksum == newone);
	gma->genv = oldone;
	kArray_clear(newone->lvarlst, newone->lvarlst_top);
	if(newone->l.allocsize > 0) {
		KFREE(newone->l.vars, newone->l.allocsize);
	}
	if(newone->f.allocsize > 0) {
		KFREE(newone->f.vars, newone->f.allocsize);
	}
	return newone;
}

#define GAMMA_PUSH(G,B) \
	gmabuf_t *oldbuf_ = Gamma_push(_ctx, G, B);

#define GAMMA_POP(G,B) \
	Gamma_pop(_ctx, G, oldbuf_, B);

// --------------------------------------------------------------------------

#define kExpr_typed(E, B, TY)   Expr_typed(E, TEXPR_##B, TY)
static inline kExpr *Expr_typed(kExpr *expr, int build, ktype_t ty)
{
	((struct _kExpr*)expr)->build = build;
	((struct _kExpr*)expr)->ty = ty;
	return expr;
}

static kline_t Expr_uline(CTX, kExpr *expr, int pe, int* lposNUL)
{
	kToken *tk = expr->tk;
	kArray *a = expr->cons;
	DBG_ASSERT(IS_Expr(expr));
	if(tk->uline > 0) {
		return tk->uline;
	}
	if(a != NULL && IS_Array(a)) {
		size_t i;
		for(i=0; i < kArray_size(a); i++) {
			tk = a->toks[i];
			if(IS_Token(tk) && tk->uline > 0) {
				return tk->uline;
			}
			if(IS_Expr(tk)) {
				kline_t uline = Expr_uline(_ctx, a->exprs[i], pe, lposNUL);
				if(uline > 0) return uline;
			}
		}
	}
	if(IS_Expr(a)) {
		return Expr_uline(_ctx, expr->single, pe, lposNUL);
	}
	if (pe < INFO_) {
		DBG_ABORT("@@@@@@@@ Cannot Find Uline @@@@@@@@@@@");
	}
	return 0;
}

static kExpr *Expr_p(CTX, kExpr *expr, int pe, const char *fmt, ...)
{
	if(expr != K_NULLEXPR) {
		int lpos = -1;
		kline_t uline = Expr_uline(_ctx, expr, pe, &lpos);
		va_list ap;
		va_start(ap, fmt);
		vperrorf(_ctx, pe, uline, lpos, fmt, ap);
		va_end(ap);
	}
	return K_NULLEXPR;
}

static KMETHOD UndefinedExprTyCheck(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	if(Expr_isTerm(expr)) {
		kToken *tk = expr->tk;
		expr = kExpr_p(expr, ERR_, "undefined token type checker: '%s'", kToken_s(tk));
	}
	else {
		expr = kExpr_p(expr, ERR_, "undefined operator type checker: %s",  syn->token);
	}
	RETURN_(expr);
}

static kExpr *ExprTyCheck(CTX, kExpr *expr, kGamma *gma, int reqty)
{
	ksyntax_t *syn = expr->syn;
	kMethod *mtd = (syn == NULL || syn->ExprTyCheck == NULL) ? kmodsugar->UndefinedExprTyCheck : syn->ExprTyCheck;
	INIT_GCSTACK();
	BEGIN_LOCAL(lsfp, 3);
	KSETv(lsfp[K_CALLDELTA+0].o, (kObject*)expr);
	lsfp[K_CALLDELTA+0].ndata = (uintptr_t)syn;
	KSETv(lsfp[K_CALLDELTA+1].o, (kObject*)gma);
	lsfp[K_CALLDELTA+2].ivalue = reqty;
	KCALL(lsfp, 0, mtd, 3, K_NULLEXPR);
	END_LOCAL();
	RESET_GCSTACK();
	DBG_ASSERT(IS_Expr(lsfp[0].o));
	return (kExpr*)lsfp[0].o;
}


static void Expr_putConstValue(CTX, kExpr *expr, ksfp_t *sfp)
{
	if(expr->build == TEXPR_CONST) {
		KSETv(sfp[0].o, expr->data);
		sfp[0].ndata = O_unbox(expr->data);
	}else if(expr->build == TEXPR_NCONST) {
		sfp[0].ndata = expr->ndata;
	}else if(expr->build == TEXPR_NEW) {
		KSETv(sfp[0].o, new_kObject(CT_(expr->ty), expr->ndata /*FIXME*/));
	}else {
		assert(expr->build == TEXPR_NULL);
		KSETv(sfp[0].o, knull(CT_(expr->ty)));
		sfp[0].ndata = 0;
	}
}

static kExpr* ExprCall_toConstValue(CTX, kExpr *expr, kArray *cons, ktype_t rtype)
{
	size_t i, size = kArray_size(cons), psize = size - 2;
	kMethod *mtd = cons->methods[0];
	BEGIN_LOCAL(lsfp, K_CALLDELTA + psize);
	kExpr_p(expr, DEBUG_, "constant folding of %s", T_ty(expr->ty));
	for(i = 1; i < size; i++) {
		Expr_putConstValue(_ctx, cons->exprs[i], lsfp + K_CALLDELTA + i - 1);
	}
	KCALL(lsfp, 0, mtd, psize, knull(CT_(expr->ty)));
	END_LOCAL();
	if(TY_isUnbox(rtype) || rtype == TY_void) {
		return kExpr_setNConstValue(expr, rtype, lsfp[0].ndata);
	}
	return kExpr_setConstValue(expr, rtype, lsfp[0].o);
}

static kbool_t CT_isa(CTX, ktype_t cid1, ktype_t cid2)
{
	DBG_ASSERT(cid1 != cid2); // should be checked
	if(cid2 == CLASS_Object) return true;
	kclass_t *ct = CT_(cid1);
	while(ct->supcid != CLASS_Object) {
		ct = CT_(ct->supcid);
		if(ct->cid == cid2) return true;
	}
	return false;
}

static kExpr *new_BoxingExpr(CTX, kExpr *expr, ktype_t reqty)
{
	if(expr->build == TEXPR_NCONST) {
		W(kExpr, expr);
		Wexpr->build = TEXPR_CONST;
		KINITv(Wexpr->data, new_kObject(CT_(Wexpr->ty), Wexpr->ndata));
		Wexpr->ty = reqty;
		WASSERT(expr);
		return expr;
	}
	else {
		struct _kExpr *texpr = new_W(Expr, NULL);
		PUSH_GCSTACK(texpr);
		KINITv(texpr->single, expr);
		texpr->build = TEXPR_BOX;
		texpr->ty = reqty;
		return texpr;
	}
}

static kExpr *Expr_tyCheckCallParams(CTX, kExpr *expr, kMethod *mtd, kGamma *gma, ktype_t reqty);

static kExpr *Expr_tyCheck(CTX, kExpr *expr, kGamma *gma, ktype_t reqty, int pol)
{
	kExpr *texpr = expr;
	if(expr->ty == TY_var && expr != K_NULLEXPR) {
		if(!IS_Expr(expr)) {
			expr = new_ConstValue(O_cid(expr), expr);
			PUSH_GCSTACK(expr);
		}
		texpr = ExprTyCheck(_ctx, expr, gma, reqty);
	}
	if(texpr != K_NULLEXPR) {
		//DBG_P("type=%s, reqty=%s", T_ty(expr->ty), T_ty(reqty));
		if(texpr->ty == TY_void) {
			return FLAG_is(pol, TPOL_ALLOWVOID) ? texpr: kExpr_p(expr, ERR_, "void is not acceptable");
		}
		if(reqty == TY_var || texpr->ty == reqty || FLAG_is(pol, TPOL_NOCHECK)) {
			return texpr;
		}
		if(CT_isa(_ctx, texpr->ty, reqty)) {
			DBG_P("SUBTYPE type=%s, reqty=%s", T_ty(expr->ty), T_ty(reqty));
			if(TY_isUnbox(texpr->ty) && !TY_isUnbox(reqty)) {
				DBG_P("BOXING type=%s, reqty=%s", T_ty(expr->ty), T_ty(reqty));
				return new_BoxingExpr(_ctx, expr, reqty);
			}
			return texpr;
		}
		kMethod *mtd = kKonohaSpace_getCastMethodNULL(gma->genv->ks, texpr->ty, reqty);
		DBG_P("finding cast %s => %s: %p", T_ty(texpr->ty), T_ty(reqty), mtd);
		if(mtd != NULL && (kMethod_isCoercion(mtd) || FLAG_is(pol, TPOL_COERCION))) {
			kExpr *exprCAST = new_ConsExpr(_ctx, SYN_CALL, 2, mtd, texpr);
			return Expr_tyCheckCallParams(_ctx, exprCAST, mtd, gma, reqty);
		}
		return Expr_p(_ctx, texpr, ERR_, "%s is requested, not %s", T_ty(reqty), T_ty(texpr->ty));
	}
	return texpr;
}

static kExpr* Expr_tyCheckAt(CTX, kExpr *exprP, size_t pos, kGamma *gma, ktype_t reqty, int pol)
{
	if(!Expr_isTerm(exprP) && pos < kArray_size(exprP->cons)) {
		kExpr *expr = exprP->cons->exprs[pos];
		expr = Expr_tyCheck(_ctx, expr, gma, reqty, pol);
		KSETv(exprP->cons->exprs[pos], expr);
		return expr;
	}
	return K_NULLEXPR;
}

static kbool_t Stmt_tyCheckExpr(CTX, kStmt *stmt, keyword_t nameid, kGamma *gma, ktype_t reqty, int pol)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, nameid);
	if(expr != NULL && IS_Expr(expr)) {
		kExpr *texpr = Expr_tyCheck(_ctx, expr, gma, reqty, pol);
//		DBG_P("reqty=%s, texpr->ty=%s isnull=%d", T_cid(reqty), T_cid(texpr->ty), (texpr == K_NULLEXPR));
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

static KMETHOD ExprTyCheck_TEXT(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tk = expr->tk;
	RETURN_(kExpr_setConstValue(expr, TY_String, tk->text));
}

static KMETHOD ExprTyCheck_NULL(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	DBG_P("typing null as %s", T_ty(reqty));
	if(reqty == TY_var) reqty = CLASS_Object;
	RETURN_(kExpr_setVariable(expr, NULL, reqty, 0, gma));
}

static KMETHOD ExprTyCheck_TYPE(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	DBG_ASSERT(TK_isType(expr->tk));
	RETURN_(kExpr_setVariable(expr, NULL, expr->tk->ty, 0, gma));
}

static KMETHOD ExprTyCheck_TRUE(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	RETURN_(kExpr_setNConstValue(expr, TY_Boolean, (uintptr_t)1));
}

static KMETHOD ExprTyCheck_FALSE(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	RETURN_(kExpr_setNConstValue(expr, TY_Boolean, (uintptr_t)0));
}

static KMETHOD ExprTyCheck_INT(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tk = expr->tk;
	long long n = strtoll(S_text(tk->text), NULL, 0);
	RETURN_(kExpr_setNConstValue(expr, TY_Int, (uintptr_t)n));
}

static KMETHOD ExprTyCheck_FLOAT(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	RETURN_(kToken_p(expr->tk, ERR_, "float is unsupported: %s", kToken_s(expr->tk)));
}

static kExpr* Expr_tyCheckVariable(CTX, struct _kExpr *expr, kGamma *gma)
{
	DBG_ASSERT(expr->ty == TY_var);
	kToken *tk = expr->tk;
	ksymbol_t fn = ksymbol(S_text(tk->text), S_size(tk->text), FN_NONAME, SYMPOL_NAME);
	int i;
	gmabuf_t *genv = gma->genv;
	for(i = genv->l.varsize - 1; i >= 0; i--) {
		//DBG_P("searching index=%d, ty=%s fn=%s", i, T_ty(genv->l.vars[i].ty), T_fn(genv->l.vars[i].fn));
		if(genv->l.vars[i].fn == fn) {
			expr->build = TEXPR_LOCAL_;
			expr->ty = genv->l.vars[i].ty;
			expr->index = i;
			kArray_add(genv->lvarlst, expr);
			return expr;
		}
	}
	for(i = genv->f.varsize - 1; i >= 0; i--) {
		if(genv->f.vars[i].fn == fn) {
			expr->build = TEXPR_LOCAL;
			expr->ty = genv->f.vars[i].ty;
			expr->index = i;
			return expr;
		}
	}
//	if(_ctx->lib2->KS_getGetterMethodNULL != NULL) {
//		kMethod *mtd = kKonohaSpace_getGetterMethodNULL(genv->ks, genv->this_cid, fn);
//		if(mtd != NULL) {
//
//		}
//	}
	return kToken_p(tk, ERR_, "undefined name: %s", kToken_s(tk));
}

static KMETHOD ExprTyCheck_SYMBOL(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	RETURN_(Expr_tyCheckVariable(_ctx, (struct _kExpr*)expr, gma));
}

static kObject *KonohaSpace_getSymbolValueNULL(CTX, kKonohaSpace *ks, const char *key, size_t klen)
{
	if(key[0] == 'K' && (key[1] == 0 || strcmp("Konoha", key) == 0)) {
		return (kObject*)ks;
	}
	return NULL;
}

static KMETHOD ExprTyCheck_USYMBOL(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	DBG_P("USYMBOL...");
	kToken *tk = expr->tk;
	kuname_t ukey = kuname(S_text(tk->text), S_size(tk->text), 0, FN_NONAME);
	if(ukey != FN_NONAME) {
		kvs_t *kv = KonohaSpace_getConstNULL(_ctx, gma->genv->ks, ukey);
		if(kv != NULL) {
			if(FN_isBOXED(kv->key)) {
				kExpr_setConstValue(expr, kv->ty, kv->oval);
			}
			else {
				kExpr_setNConstValue(expr, kv->ty, kv->uval);
			}
			RETURN_(expr);
		}
	}
	kObject *v = KonohaSpace_getSymbolValueNULL(_ctx, gma->genv->ks, S_text(tk->text), S_size(tk->text));
	kExpr *texpr = (v == NULL) ?
			kToken_p(tk, ERR_, "undefined name: %s", kToken_s(tk)) : kExpr_setConstValue(expr, O_cid(v), v);
	RETURN_(texpr);
}

static ktype_t ktype_var(CTX, ktype_t ty, kcid_t cid)
{
	return ty;
}

static int param_policy(ksymbol_t fn)
{
	int pol = 0;
	if(FN_isCOERCION(fn)) {
		pol = pol | TPOL_COERCION;
	}
	return pol;
}

static kExpr* Expr_typedWithMethod(CTX, kExpr *expr, kMethod *mtd, ktype_t reqty)
{
	KSETv(expr->cons->methods[0], mtd);
	kExpr_typed(expr, CALL, kMethod_isSmartReturn(mtd) ? reqty : mtd->pa->rtype);
	return expr;
}

static kExpr *Expr_tyCheckCallParams(CTX, kExpr *expr, kMethod *mtd, kGamma *gma, ktype_t reqty)
{
	kArray *cons = expr->cons;
	size_t i, size = kArray_size(cons);
	kExpr *expr1 = cons->exprs[1];
	ktype_t cid = expr1->ty;
	if(!(IS_Method(mtd) && cid != TY_var)) {
		return K_NULLEXPR;
	}
	if(!TY_isUnbox(mtd->cid) && TY_isUnbox(cid)) {
		expr1 = new_BoxingExpr(_ctx, cons->exprs[1], cid);
		KSETv(cons->exprs[1], expr1);
	}
	kParam *pa = mtd->pa;
	ktype_t rtype = ktype_var(_ctx, pa->rtype, cid);
	int isConst = (Expr_isCONST(expr1)) ? 1 : 0;
	//	if(rtype == TY_var && gma->genv->mtd == mtd) {
	//		return ERROR_Unsupported(_ctx, "type inference of recursive calls", TY_unknown, NULL);
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
		if(!Expr_isCONST(expr)) isConst = 0;
	}
	expr = Expr_typedWithMethod(_ctx, expr, mtd, reqty);
	if(isConst && kMethod_isConst(mtd)) {
		return ExprCall_toConstValue(_ctx, expr, cons, rtype);
	}
	return expr;
}

static inline void kToken_setmn(kToken *tk, kmethodn_t mn)
{
	((struct _kToken*)tk)->tt = TK_MN;
	((struct _kToken*)tk)->mn = mn;
}

static kExpr* Expr_tyCheckDynamicCallParams(CTX, kExpr *expr, kMethod *mtd, kGamma *gma, kString *name, kmethodn_t mn, ktype_t reqty)
{
	int i;
	ktype_t ptype = (mtd->pa->psize == 0) ? TY_Object : mtd->pa->p[0].ty;
	for(i = 2; i < kArray_size(expr->cons); i++) {
		kExpr *texpr = kExpr_tyCheckAt(expr, i, gma, ptype, 0);
		if(texpr == K_NULLEXPR) return texpr;
	}
	Expr_add(_ctx, expr, new_ConstValue(TY_String, name));
	return Expr_typedWithMethod(_ctx, expr, mtd, reqty);
}

static kExpr *CallParams(CTX, kExpr *expr, kcid_t this_cid, kGamma *gma, ktype_t reqty)
{
	kMethod *mtd = NULL;
	kKonohaSpace *ks = gma->genv->ks;
	kToken *tkMN = expr->cons->toks[0];
	if(!IS_Token(tkMN)) {
		kExpr *expr0 = (kExpr*)tkMN;
		if(IS_Expr(expr0) && Expr_isTerm(expr0)) {
			tkMN = (struct _kToken *)expr0->tk;
		}
		else {
			DBG_ABORT("THIS NOT HAPPEN");
			return K_NULLEXPR;
		}
	}
	if(tkMN->tt == TK_SYMBOL || tkMN->tt == TK_USYMBOL) {
		kToken_setmn(tkMN, ksymbol(S_text(tkMN->text), S_size(tkMN->text), FN_NEWID, SYMPOL_METHOD));
	}
	//DBG_P("finding %s.%s", T_cid(this_cid), S_text(tkMN->text));
	if(tkMN->kw != 0 && tkMN->kw == expr->syn->kw) {
		if(kArray_size(expr->cons) == 3) {
			mtd = kKonohaSpace_getMethodNULL(ks, this_cid, expr->syn->op2);
			if(mtd == NULL) {
				kToken_p(tkMN, ERR_, "undefined binary operator: %s of %s", S_text(tkMN->text), T_cid(this_cid));
			}
			goto L_RETURN;
		}
		if(kArray_size(expr->cons) == 2) {
			mtd = kKonohaSpace_getMethodNULL(ks, this_cid, expr->syn->op1);
			if(mtd == NULL) {
				kToken_p(tkMN, ERR_, "undefined unary operator: %s of %s", S_text(tkMN->text), T_cid(this_cid));
			}
			goto L_RETURN;
		}
	}
	if(tkMN->tt == TK_MN) {
		mtd = kKonohaSpace_getMethodNULL(ks, this_cid, tkMN->mn);
		if(mtd == NULL) {
			if(tkMN->text != TS_EMPTY) {
				mtd = kKonohaSpace_getMethodNULL(ks, this_cid, 0);
				if(mtd != NULL) {
					return Expr_tyCheckDynamicCallParams(_ctx, expr, mtd, gma, tkMN->text, tkMN->mn, reqty);
				}
			}
			kToken_p(tkMN, ERR_, "undefined method: %s.%s", T_cid(this_cid), kToken_s(tkMN));
		}
	}
	L_RETURN:;
	if(mtd != NULL) {
		return Expr_tyCheckCallParams(_ctx, expr, mtd, gma, reqty);
	}
	return K_NULLEXPR;
}

static KMETHOD ExprTyCheck_call(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kExpr *texpr = kExpr_tyCheckAt(expr, 1, gma, TY_var, 0);
	if(texpr != K_NULLEXPR) {
		kcid_t this_cid = texpr->ty;
		//DBG_P("this_cid=%s", T_cid(this_cid));
		RETURN_(CallParams(_ctx, expr, this_cid, gma, reqty));
	}
}

static kmethodn_t Token_mn(CTX, kToken *tk, const char *name)
{
	if(tk->tt == TK_SYMBOL || tk->tt == TK_USYMBOL) {
		kToken_setmn(tk, ksymbol(S_text(tk->text), S_size(tk->text), FN_NEWID, SYMPOL_METHOD));
	}
	if(tk->tt != TK_MN) {
		kToken_p(tk, ERR_, "%s is not a %s name", kToken_s(tk), name);
		return MN_NONAME;
	}
	return tk->mn;
}

static KMETHOD ExprTyCheck_FuncStyleCall(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kArray *cons = expr->cons;
	DBG_ASSERT(IS_Expr(cons->list[0]));
	DBG_ASSERT(cons->list[1] == K_NULL);
	kcid_t this_cid = TY_unknown;
	kMethod *mtd = NULL;
	if(Expr_isTerm(cons->list[0])) {
		kToken *tk = cons->exprs[0]->tk;
		if(Token_mn(_ctx, tk, "function") != MN_NONAME) {
			if(gma->genv->this_cid !=0) {   /* this.f() */
				mtd = kKonohaSpace_getMethodNULL(gma->genv->ks, gma->genv->this_cid, tk->mn);
				if(mtd != NULL) {
					if(!kMethod_isStatic(mtd)) {
						KSETv(cons->exprs[1], new_Variable(LOCAL, gma->genv->this_cid, 0, gma));
						this_cid = gma->genv->this_cid;
					}
				}
			}
			if(mtd == NULL) {
				mtd = kKonohaSpace_getStaticMethodNULL(gma->genv->ks, tk->mn);
				if(mtd == NULL) {
					RETURN_(kToken_p(tk, ERR_, "undefined function (method): %s", kToken_s(tk)));
				}
			}
		}
	}
	if(mtd != NULL) {
		if(this_cid == TY_unknown) {
			KSETv(cons->exprs[1], new_Variable(NULL, mtd->cid, 0, gma));
		}
		RETURN_(Expr_tyCheckCallParams(_ctx, expr, mtd, gma, reqty));
	}
	else {
		RETURN_(kExpr_p(expr, ERR_, "must be a function name"));
	}
}

static kExpr *ExprTyCheck(CTX, kExpr *expr, kGamma *gma, int reqty);

//static KMETHOD ExprTyCheck_getter(CTX, ksfp_t *sfp _RIX)
//{
//	VAR_ExprTyCheck(expr, syn, gma, reqty);
//	kExpr *texpr = kExpr_tyCheckAt(expr, 0, gma, TY_var, 0);
//	if(texpr != K_NULLEXPR) {
//		kToken *tkF = expr->cons->toks[1];
//		kmethodn_t mn = Token_mn(_ctx, tkF, "field");
//		kMethod *mtd = kKonohaSpace_getMethodNULL(gma->genv->ks, texpr->ty, MN_toGETTER(mn));
//		if(mtd != NULL) {
//			KSETv(expr->cons->methods[0], mtd);
//			KSETv(kExpr_at(expr, 1), texpr); // GC_UNSAFE (swaping)
//			RETURN_(Expr_tyCheckCallParams(_ctx, expr, gma, reqty));
//		}
//		RETURN_(kToken_p(tkF, ERR_, "undefined field accessor: %s", kToken_s(tkF)));
//	}
//	RETURN_(K_NULLEXPR);
//}

static KMETHOD ExprTyCheck_and(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	if(kExpr_tyCheckAt(expr, 1, gma, TY_Boolean, 0) != K_NULLEXPR) {
		if(kExpr_tyCheckAt(expr, 2, gma, TY_Boolean, 0) != K_NULLEXPR) {
			RETURN_(kExpr_typed(expr, AND, TY_Boolean));
		}
	}
	RETURN_(K_NULLEXPR);
}

static KMETHOD ExprTyCheck_or(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	if(kExpr_tyCheckAt(expr, 1, gma, TY_Boolean, 0) != K_NULLEXPR) {
		if(kExpr_tyCheckAt(expr, 2, gma, TY_Boolean, 0) != K_NULLEXPR) {
			RETURN_(kExpr_typed(expr, OR, TY_Boolean));
		}
	}
	RETURN_(K_NULLEXPR);
}

static KMETHOD StmtTyCheck_EXPR(CTX, ksfp_t *sfp _RIX)  // $expr
{
	VAR_StmtTyCheck(stmt, syn, gma);
	kbool_t r = Stmt_tyCheckExpr(_ctx, stmt, 1, gma, TY_var, TPOL_ALLOWVOID);
	kStmt_typed(stmt, EXPR);
	RETURNb_(r);
}

static int addGammaStack(CTX, gstack_t *s, ktype_t ty, ksymbol_t fn)
{
	int index = s->varsize;
	if(!(s->varsize < s->capacity)) {
		s->capacity *= 2;
		size_t asize = sizeof(gammastack_t) * s->capacity;
		gammastack_t *v = (gammastack_t*)KMALLOC(asize);
		memcpy(v, s->vars, asize/2);
		if(s->allocsize > 0) {
			KFREE(s->vars, s->allocsize);
		}
		s->vars = v;
		s->allocsize = asize;
	}
	DBG_P("index=%d, ty=%s fn=%s", index, T_ty(ty), T_fn(fn));
	s->vars[index].ty = ty;
	s->vars[index].fn = fn;
	s->varsize += 1;
	return index;
}

static kbool_t Expr_declLocalVariable(CTX, kExpr *expr, kGamma *gma, ktype_t ty, ksymbol_t fn)
{
	DBG_ASSERT(Expr_isTerm(expr));
	int index = addGammaStack(_ctx, &gma->genv->l, ty, fn);
	kExpr_setVariable(expr, LOCAL_, ty, index, gma);
	return 1;
}

static KMETHOD UndefinedStmtTyCheck(CTX, ksfp_t *sfp _RIX)  // $expr
{
	VAR_StmtTyCheck(stmt, syn, gma);
	const char *msg = kGamma_isTOPLEVEL(gma) ? "at the top level" : "inside method";
	SUGAR_P(ERR_, stmt->uline, -1, "'%s' is not available %s", syn->token, msg);
	RETURNb_(0);
}

static kbool_t Stmt_TyCheck(CTX, ksyntax_t *syn, kStmt *stmt, kGamma *gma)
{
	kMethod *mtd = kGamma_isTOPLEVEL(gma) ? syn->TopStmtTyCheck : syn->StmtTyCheck;
	if(mtd == NULL) mtd = kmodsugar->UndefinedStmtTyCheck;
	BEGIN_LOCAL(lsfp, 5);
	KSETv(lsfp[K_CALLDELTA+0].o, (kObject*)stmt);
	lsfp[K_CALLDELTA+0].ndata = (uintptr_t)syn;  // quick trace
	KSETv(lsfp[K_CALLDELTA+1].o, (kObject*)gma);
	KCALL(lsfp, 0, mtd, 1, knull(CT_Boolean));
	END_LOCAL();
	DBG_P("syn='%s', result=%d", T_kw(syn->kw), lsfp[0].bvalue);
	return lsfp[0].bvalue;
}

static kbool_t Block_tyCheckAll(CTX, kBlock *bk, kGamma *gma)
{
	int i, result = 1, lvarsize = gma->genv->l.varsize;
	for(i = 0; i < kArray_size(bk->blockS); i++) {
		kStmt *stmt = (kStmt*)bk->blockS->list[i];
		ksyntax_t *syn = stmt->syn;
		dumpStmt(_ctx, stmt);
		if(syn == NULL) continue; /* This means 'done' */
		if(syn->kw == KW_ERR) {
			kGamma_setERROR(gma, 1);
			result = 0;
			break;
		}
		int estart = kerrno;
		if(!Stmt_TyCheck(_ctx, syn, stmt, gma)) {
			kStmt_toERR(stmt, estart);
			kGamma_setERROR(gma, 1);
			result = 0;
			break;
		}
	}
	if(bk != K_NULLBLOCK) {
		kExpr_setVariable(bk->esp, LOCAL_, TY_void, gma->genv->l.varsize, gma);
	}
	if(lvarsize < gma->genv->l.varsize) {
		gma->genv->l.varsize = lvarsize;
	}
	return result;
}

static kbool_t Block_checkLastExpr(CTX, kBlock *bk, kGamma *gma, kline_t uline)
{
	if(kArray_size(bk->blockS) > 0) {
		kStmt *stmt = bk->blockS->stmts[kArray_size(bk->blockS)-1];
		if(stmt->syn->kw == KW_EXPR) return 1;
		uline = stmt->uline;
	}
	SUGAR_P(ERR_, uline, -1, "block has no value");
	return 0;
}

static KMETHOD ExprTyCheck_block(CTX, ksfp_t *sfp _RIX)
{
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kBlock *bk = expr->block;
	DBG_ASSERT(IS_Block(bk));
	kExpr *texpr = K_NULLEXPR;
	if(Block_checkLastExpr(_ctx, bk, gma, expr->tk->uline)) {
		int lvarsize = gma->genv->l.varsize;
		kExpr *lvar = new_(Expr, NULL);
		Expr_declLocalVariable(_ctx, lvar, gma, reqty, 0/*FN_*/);
		if(Block_tyCheckAll(_ctx, bk, gma)) {
			kStmt *stmt = bk->blockS->stmts[kArray_size(bk->blockS)-1];
			kExpr *rexpr = kStmt_expr(stmt, KW_EXPR, NULL);  DBG_ASSERT(rexpr != NULL);
			ktype_t ty = rexpr->ty;
			kExpr *letexpr = new_TypedConsExpr(_ctx, TEXPR_LET, TY_void, 3, K_NULL, lvar, rexpr);
			kObject_setObject(stmt, KW_EXPR, letexpr);
			texpr = kExpr_setVariable(expr, BLOCK_, ty, lvarsize, gma);
		}
		if(lvarsize < gma->genv->l.varsize) {
			gma->genv->l.varsize = lvarsize;
		}
	}
	RETURN_(texpr);
}

static void Stmt_toBlockStmt(CTX, kStmt *stmt, kBlock *bk)
{
	kObject_setObject(stmt, KW_block, bk);
	kStmt_typed(stmt, BLOCK);
}

static KMETHOD StmtTyCheck_if(CTX, ksfp_t *sfp _RIX)
{
	kbool_t r = 1;
	VAR_StmtTyCheck(stmt, syn, gma);
	if((r = Stmt_tyCheckExpr(_ctx, stmt, KW_EXPR, gma, TY_Boolean, 0))) {
		kExpr *exprCond = kStmt_expr(stmt, KW_EXPR, NULL);
		kBlock *bkThen = kStmt_block(stmt, KW_block, K_NULLBLOCK);
		kBlock *bkElse = kStmt_block(stmt, KW_else, K_NULLBLOCK);
		if(exprCond->build == TEXPR_NCONST) {
			if(exprCond->ndata) {
				r = Block_tyCheckAll(_ctx, bkThen, gma);
				Stmt_toBlockStmt(_ctx, stmt, bkThen);
			}
			else {
				r = Block_tyCheckAll(_ctx, bkElse, gma);
				Stmt_toBlockStmt(_ctx, stmt, bkElse);
			}
		}
		else {
			r = Block_tyCheckAll(_ctx, bkThen, gma);
			r = r & Block_tyCheckAll(_ctx, bkElse, gma);
			kStmt_typed(stmt, IF);
		}
	}
	RETURNb_(r);
}

static KMETHOD StmtTyCheck_return(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtTyCheck(stmt, syn, gma);
	kbool_t r = 1;
	ktype_t rtype = gma->genv->mtd->pa->rtype;
	kStmt_typed(stmt, RETURN);
	if(rtype != TY_void) {
		//DBG_P("return line=%d rtype %s, %p", (kshort_t)stmt->uline, T_ty(rtype), kStmt_expr(stmt, KW_EXPR, NULL));
		r = Stmt_tyCheckExpr(_ctx, stmt, KW_EXPR, gma, rtype, 0);
	}
	RETURNb_(r);
}

/* ------------------------------------------------------------------------ */

static void Stmt_toExprCall(CTX, kStmt *stmt, kMethod *mtd, int n, ...)
{
	kExpr *expr = new_ConsExpr(_ctx, SYN_CALL, 0);
	int i;
	va_list ap;
	va_start(ap, n);
	kArray_add(expr->cons, mtd);
	for(i = 0; i < n; i++) {
		kObject *v =  (kObject*)va_arg(ap, kObject*);
		assert(v != NULL);
		kArray_add(expr->cons, v);
	}
	va_end(ap);
	kObject_setObject(stmt, 1, expr);
	kStmt_setsyn(stmt, SYN_(kStmt_ks(stmt), KW_EXPR));
	kStmt_typed(stmt, EXPR);
}

///* ------------------------------------------------------------------------ */

static kbool_t Token_checkVariableSymbol(CTX, kToken *tk, ksymbol_t *symbol)
{
	if(tk->tt == TK_SYMBOL) {
		*symbol = ksymbol(S_text(tk->text), S_size(tk->text), FN_NEWID, SYMPOL_NAME);
		return 1;
	}
	return 0;
}

static kbool_t Expr_declType(CTX, kExpr *expr, kGamma *gma, ktype_t ty, kStmt **REFstmt)
{
	DBG_ASSERT(IS_Expr(expr));
	if(Expr_isTerm(expr)) {
		if(REFstmt != NULL) {
			kExpr_p(expr, ERR_, "initial value is required");
			return false;
		}
		else {
			ksymbol_t fn = FN_NONAME;
			return (Token_checkVariableSymbol(_ctx, expr->tk, &fn)
					&& Expr_declLocalVariable(_ctx, expr, gma, ty, fn));
		}
	}
	else if(expr->syn->kw == KW_LET) {
		if(!Expr_declType(_ctx, kExpr_at(expr, 1), gma, ty, NULL)) return false;
		ty = kExpr_at(expr, 1)->ty;
		DBG_P("declare %s index=%d", T_ty(ty), kExpr_at(expr, 1)->index);
		if(kExpr_tyCheckAt(expr, 2, gma, ty, 0) != K_NULLEXPR) {
			kStmt *newstmt = new_(Stmt, Expr_uline(_ctx, expr, INFO_, NULL));
			Block_insertAfter(_ctx, REFstmt[0]->parentNULL, REFstmt[0], newstmt);
			kStmt_setsyn(newstmt, SYN_(kStmt_ks(newstmt), KW_EXPR));
			kExpr_typed(expr, LET, TY_void);
			kObject_setObject(newstmt, KW_EXPR, expr);
			REFstmt[0] = newstmt;
			return true;
		}
		return false;
	} else if(expr->syn->kw == KW_COMMA) {
		size_t i;
		for(i = 1; i < kArray_size(expr->cons); i++) {
			if(!Expr_declType(_ctx, kExpr_at(expr, i), gma, ty, REFstmt)) return false;
		}
		return true;
	}
	kExpr_p(expr, ERR_, "syntax error: not variable");
	return false;
}

static KMETHOD StmtTyCheck_declType(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtTyCheck(stmt, syn, gma);
	kToken *tk  = kStmt_token(stmt, KW_TYPE, NULL);
	kExpr  *expr = kStmt_expr(stmt, KW_EXPR, NULL);
	if(tk == NULL || !TK_isType(tk) || expr == NULL) {
		ERR_SyntaxError(stmt->uline);
		RETURNb_(0);
	}
	kStmt_done(stmt);
	RETURNb_(Expr_declType(_ctx, expr, gma, TK_type(tk), &stmt));
}

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

static kcid_t Stmt_getcid(CTX, kStmt *stmt, kKonohaSpace *ns, keyword_t kw, kcid_t defcid)
{
	kToken *tk = (kToken*)kObject_getObjectNULL(stmt, kw);
	if(tk == NULL || !IS_Token(tk)) {
		return defcid;
	}
	else {
		assert(TK_isType(tk));
		return TK_type(tk);
	}
}

static kcid_t Stmt_getmn(CTX, kStmt *stmt, kKonohaSpace *ns, keyword_t kw, kmethodn_t defmn)
{
	kToken *tk = (kToken*)kObject_getObjectNULL(stmt, kw);
	if(tk == NULL || !IS_Token(tk) || !IS_String(tk->text)) {
		return defmn;
	}
	else {
		DBG_ASSERT(IS_String(tk->text));
		return ksymbol(S_text(tk->text), S_size(tk->text), FN_NEWID, SYMPOL_METHOD);
	}
}

static kParam *Stmt_newMethodParamNULL(CTX, kStmt *stmt, kGamma* gma)
{
	kParam *pa = (kParam*)kObject_getObjectNULL(stmt, KW_params);
	if(pa == NULL || !IS_Param(pa)) {
		ksyntax_t *syn = SYN_(kStmt_ks(stmt), KW_params);
		if(!Stmt_TyCheck(_ctx, syn, stmt, gma)) {
			return NULL;
		}
	}
	pa = (kParam*)kObject_getObjectNULL(stmt, KW_params);
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

static void Stmt_setMethodFunc(CTX, kStmt *stmt, kKonohaSpace *ks, kMethod *mtd)
{
	kToken *tcode = kStmt_token(stmt, KW_block, NULL);
	if(tcode != NULL && tcode->tt == TK_CODE) {
		KSETv(((struct _kMethod*)mtd)->tcode, tcode);  //FIXME
		KSETv(((struct _kMethod*)mtd)->lazyns, ks);
		kMethod_setFunc(mtd, Fmethod_lazyCompilation);
		kArray_add(ctxsugar->definedMethods, mtd);
	}
}

static KMETHOD StmtTyCheck_declMethod(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtTyCheck(stmt, syn, gma);
	kbool_t r = 0;
	kKonohaSpace *ks = gma->genv->ks;
	uintptr_t flag =  Stmt_flag(_ctx, stmt, MethodDeclFlag, 0);
	kcid_t cid =  Stmt_getcid(_ctx, stmt, ks, KW_cname, ks->function_cid);
	kmethodn_t mn = Stmt_getmn(_ctx, stmt, ks, KW_name, MN_("new"));
	kParam *pa = Stmt_newMethodParamNULL(_ctx, stmt, gma);
	if(TY_isSingleton(cid)) flag |= kMethod_Static;
	if(pa != NULL) {
		INIT_GCSTACK();
		kMethod *mtd = new_kMethod(flag, cid, mn, pa, NULL);
		PUSH_GCSTACK(mtd);
		if(kKonohaSpace_defineMethod(ks, mtd, stmt->uline)) {
			r = 1;
			Stmt_setMethodFunc(_ctx, stmt, ks, mtd);
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
	if(kArray_size(expr->cons) != 3) goto L_ERROR;
	kArray *cons = expr->cons;
	if(!IS_Expr(cons->exprs[1])) goto L_ERROR;
	kToken *tkOP = cons->toks[0];
	kToken *tkN = cons->exprs[1]->tk;
	kToken *tkT = cons->toks[2];
	if(!IS_Token(tkOP) || tkOP->kw != KW_COLON) goto L_ERROR;
	if(tkN == NULL || !IS_Token(tkN) || tkN->tt != TK_SYMBOL) goto L_ERROR;
	if(!IS_Token(tkT) || !TK_isType(tkT)) goto L_ERROR;
	ksymbol_t fn = ksymbol(S_text(tkN->text), S_size(tkN->text), FN_NEWID, SYMPOL_NAME);
//	for(i = 0; i < n; i++) {
//		if(p[i].fn == fn) {
//			kToken_p(tkN, ERR_, "duplicated parameter: %s", kToken_s(tkN));
//			return 0;
//		}
//	}
	p[n].fn = fn;
	p[n].ty = TK_type(tkT);
	return 1;

	L_ERROR:;
	kExpr_p(expr, ERR_, "syntax error at the parameter %d", (int)i+1);
	return 0;
}

static KMETHOD StmtTyCheck_declParams(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtTyCheck(stmt, syn, gma);
	kKonohaSpace *ks = gma->genv->ks;
	ktype_t rtype =  Stmt_getcid(_ctx, stmt, ks, KW_type, TY_var);
	kParam *pa = NULL;
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, KW_params);
	if(expr == NULL) {
		pa = (rtype == TY_void) ? K_NULLPARAM : new_kParam(rtype, 0, NULL);
	}
	else if(IS_Param(expr)) {
		pa = (kParam*)expr;
	}
	else if(IS_Expr(expr)) {
		kArray *params = expr->cons;
		size_t i, psize = kArray_size(params);
		kparam_t p[psize];
		for(i = 0; i < psize; i++) {
			if(!Expr_setParam(_ctx, params->exprs[i], i, p)) {
				RETURNb_(0);
			}
		}
		pa = new_kParam(rtype, psize, p);
	}
	kObject_setObject(stmt, KW_params, pa);
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
	kArray *tls = ctxsugar->tokens;
	size_t pos = kArray_size(tls);
	ktokenize(_ctx, script, uline, tls);
	kBlock *bk = new_Block(_ctx, tls, pos, kArray_size(tls), kmodsugar->rootks);
	kArray_clear(tls, pos);
	return bk;
}

static void Gamma_initParam(CTX, gmabuf_t *genv, kParam *pa)
{
	int i, psize = (pa->psize + 1 < genv->f.capacity) ? pa->psize : genv->f.capacity - 1;
	for(i = 0; i < psize; i++) {
		genv->f.vars[i+1].fn = pa->p[i].fn;
		genv->f.vars[i+1].ty = pa->p[i].ty;
	}
	if(!kMethod_isStatic(genv->mtd)) {
		genv->f.vars[0].fn = FN_this;
		genv->f.vars[0].ty = genv->this_cid;
	}
	genv->f.varsize = psize+1;
}

static void Gamma_shiftBlockIndex(CTX, gmabuf_t *genv)
{
	kArray *a = genv->lvarlst;
	size_t i, size = kArray_size(a);
	int shift = genv->f.varsize;
	for(i = genv->lvarlst_top; i < size; i++) {
		struct _kExpr *expr = a->Wexprs[i];
		DBG_ASSERT(expr->build < TEXPR_UNTYPED);
		expr->index += shift;
		expr->build += TEXPR_shift;
	}
}

static kbool_t Method_compile(CTX, kMethod *mtd, kString *text, kline_t uline, kKonohaSpace *ks)
{
	INIT_GCSTACK();
	kGamma *gma = ctxsugar->gma;
	kBlock *bk = Method_newBlock(_ctx, mtd, text, uline);
	gammastack_t fvars[32] = {}, lvars[32] = {};
	gmabuf_t newgma = {
		.mtd = mtd,
		.ks = ks,
		.this_cid = (mtd)->cid,
		.f.vars = fvars, .f.capacity = 32, .f.varsize = 0, .f.allocsize = 0,
		.l.vars = lvars, .l.capacity = 32, .l.varsize = 0, .l.allocsize = 0,
	};
	GAMMA_PUSH(gma, &newgma);
	Gamma_initParam(_ctx, &newgma, mtd->pa);
	Block_tyCheckAll(_ctx, bk, gma);
	Gamma_shiftBlockIndex(_ctx, &newgma);
	kMethod_genCode(mtd, bk);
	GAMMA_POP(gma, &newgma);
	RESET_GCSTACK();
	return 1;
}

/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
// eval

static void Gamma_initIt(CTX, gmabuf_t *genv, kParam *pa)
{
	kstack_t *base = _ctx->stack;
	genv->f.varsize = 0;
	if(base->evalty != TY_void) {
		genv->f.vars[1].fn = FN_("it");
		genv->f.vars[1].ty = base->evalty;
		genv->f.varsize = 1;
	}
}

static kstatus_t Method_runEval(CTX, kMethod *mtd, ktype_t rtype)
{
	BEGIN_LOCAL(lsfp, K_CALLDELTA);
	kstack_t *base = _ctx->stack;
	kstatus_t result = K_CONTINUE;
	DBG_P("TY=%s, running EVAL..", T_cid(rtype));
	if(base->evalty != TY_void) {
		KSETv(lsfp[K_CALLDELTA+1].o, base->stack[base->evalidx].o);
		lsfp[K_CALLDELTA+1].ivalue = base->stack[base->evalidx].ivalue;
	}
	KCALL(lsfp, 0, mtd, 0, knull(CT_(rtype)));
	base->evalty = rtype;
	base->evalidx = (lsfp - _ctx->stack->stack);
	END_LOCAL();
	return result;
}

static ktype_t Stmt_checkReturnType(CTX, kStmt *stmt)
{
	if(stmt->syn == SYN_EXPR) {
		kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, 1);
		DBG_ASSERT(expr != NULL);
		if(expr->ty != TY_void) {
			kStmt_setsyn(stmt, SYN_(kStmt_ks(stmt), KW_return));
			kStmt_typed(stmt, RETURN);
			return expr->ty;
		}
	}
	return TY_void;
}

static ktype_t Gamma_evalMethod(CTX, kGamma *gma, kBlock *bk, kMethod *mtd)
{
	kStmt *stmt = bk->blockS->stmts[0];
	if(stmt->syn == NULL) {
		_ctx->stack->evalty = TY_void;
		return K_CONTINUE;
	}
	if(stmt->syn == SYN_ERR) return K_FAILED;
	ktype_t rtype = Stmt_checkReturnType(_ctx, stmt);
	kMethod_genCode(mtd, bk);
	return Method_runEval(_ctx, mtd, rtype);
}

static kstatus_t SingleBlock_eval(CTX, kBlock *bk, kMethod *mtd, kKonohaSpace *ks)
{
	kstatus_t result;
	kGamma *gma = ctxsugar->gma;
	gammastack_t fvars[32] = {}, lvars[32] = {};
	gmabuf_t newgma = {
		.flag = kGamma_TOPLEVEL,
		.mtd = mtd,
		.ks = ks,
		.this_cid     = TY_System,
		.f.vars = fvars, .f.capacity = 32, .f.varsize = 0, .f.allocsize = 0,
		.l.vars = lvars, .l.capacity = 32, .l.varsize = 0, .l.allocsize = 0,
	};
	GAMMA_PUSH(gma, &newgma);
	Gamma_initIt(_ctx, &newgma, mtd->pa);
	Block_tyCheckAll(_ctx, bk, gma);
	if(kGamma_isERROR(gma)) {
		result = K_BREAK;
		_ctx->stack->evalty = TY_void;
	}
	else {
		Gamma_shiftBlockIndex(_ctx, &newgma);
		result = Gamma_evalMethod(_ctx, gma, bk, mtd);
	}
	GAMMA_POP(gma, &newgma);
	return result;
}

static kstatus_t Block_eval(CTX, kBlock *bk)
{
	INIT_GCSTACK();
	BEGIN_LOCAL(lsfp, 0);
	kBlock *bk1 = ctxsugar->singleBlock;
	kMethod *mtd = new_kMethod(kMethod_Static, 0, 0, K_DEFPARAM, NULL);
	PUSH_GCSTACK(mtd);
	kstack_t *base = _ctx->stack;
	kstatus_t result = K_CONTINUE;
	kjmpbuf_t lbuf = {};
	int i, jmpresult;
	if(base->evaljmpbuf == NULL) {
		base->evaljmpbuf = (kjmpbuf_t*)KCALLOC(sizeof(kjmpbuf_t), 1);
	}
	memcpy(&lbuf, base->evaljmpbuf, sizeof(kjmpbuf_t));
	if((jmpresult = ksetjmp(*base->evaljmpbuf)) == 0) {
		for(i = 0; i < kArray_size(bk->blockS); i++) {
			KSETv(bk1->blockS->list[0], bk->blockS->list[i]);
			KSETv(((struct _kBlock*)bk1)->ks, bk->ks);  // FIXME
			result = SingleBlock_eval(_ctx, bk1, mtd, bk->ks);
			if(result == K_FAILED) break;
		}
	}
	else {
		DBG_P("Catch eval exception jmpresult=%d", jmpresult);
		base->evalty = TY_void;
		result = K_FAILED;
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
