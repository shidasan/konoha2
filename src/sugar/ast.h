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

/* ************************************************************************ */

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
// Block

static int selectStmtLine(CTX, kKonohaSpace *ks, int *indent, kArray *tls, int s, int e, kArray *tlsdst);
static void Block_addStmtLine(CTX, kBlock *bk, kArray *tls, int s, int e);

static kBlock *new_Block(CTX, kArray *tls, int s, int e, kKonohaSpace* ks)
{
	kBlock *bk = new_(Block, ks);
	PUSH_GCSTACK(bk);
	int i = s, indent = 0, atop = kArray_size(tls);
	while(i < e) {
		i = selectStmtLine(_ctx, ks, &indent, tls, i, e, tls);
		int asize = kArray_size(tls);
		if(asize > atop) {
			Block_addStmtLine(_ctx, bk, tls, atop, asize);
			kArray_clear(tls, atop);
		}
	}
	return bk;
}

static void checkKeyword(CTX, kKonohaSpace *ns, kToken *tk)
{
	if(tk->tt < TK_OPERATOR) {
		tk->kw = tk->tt;
	}
	if(tk->tt == TK_SYMBOL || tk->tt == TK_USYMBOL || tk->tt == TK_OPERATOR) {
		keyword_t kw = keyword(_ctx, S_text(tk->text), S_size(tk->text), FN_NONAME);
		if(kw != FN_NONAME) {
			ksyntax_t *syn = SYN_(ns, kw);
			if(syn != NULL) {
				tk->kw = kw;
			}
		}
		else if(tk->tt == TK_OPERATOR) {
			SUGAR_P(ERR_, tk->uline, tk->lpos, "operator '%s' is undefined", kToken_s(tk));
		}
	}
}

static kbool_t Token_toBRACE(CTX, kToken *tk)
{
	if(tk->tt == TK_CODE) {
		INIT_GCSTACK();
		kArray *a = new_(Array, 0);
		PUSH_GCSTACK(a);
		ktokenize(_ctx, S_text(tk->text), tk->uline, _TOPLEVEL, a);
		tk->tt = AST_BRACE; tk->topch = '{'; tk->lpos = '}';
		KSETv(tk->sub, a);
		RESET_GCSTACK();
		return 1;
	}
	return 0;
}

static int makeTree(CTX, kKonohaSpace *ns, kArray *tls, ktoken_t tt, int s, int e, int closech, kArray *tlsdst)
{
	int i;
	kToken *tk = tls->tts[s];
	kToken *tkp = new_(Token, 0);
	kArray_add(tlsdst, tkp);
	tkp->tt = tt; tkp->kw = tt; tkp->uline = tk->uline; tkp->topch = tk->topch; tkp->lpos = closech;
	KSETv(tkp->sub, new_(Array, 0));
	for(i = s + 1; i < e; i++) {
		tk = tls->tts[i];
		//DBG_P("@IDE i=%d, tk->topch='%c'", i, tk->topch);
		if(tk->topch == '(') {
			i = makeTree(_ctx, ns, tls, AST_PARENTHESIS, i, e, ')', tkp->sub); continue; }
		if(tk->topch == '[') {
			i = makeTree(_ctx, ns, tls, AST_BRANCET, i, e, ']', tkp->sub); continue; }
		if(tk->topch == '{') {
			i = makeTree(_ctx, ns, tls, AST_BRACE, i, e, '}', tkp->sub); continue; }
		if(tk->topch == closech) {
			return i;
		}
		if(tk->tt == TK_INDENT && closech != '}') continue;  // remove INDENT;
		checkKeyword(_ctx, ns, tk);
		kArray_add(tkp->sub, tk);
	}
	/* syntax error */ {
		SUGAR_P(ERR_, tk->uline, tk->lpos, "%c is expected", closech);
	}
	return e;
}

static int selectStmtLine(CTX, kKonohaSpace *ks, int *indent, kArray *tls, int s, int e, kArray *tlsdst)
{
	int i = s;
	for(; i < e; i++) {
		kToken *tk = tls->tts[i];
		if (i < e - 1) {
			kToken *tk1 = tls->tts[i+1];
			if(tk->topch == '@' && (tk1->tt == TK_SYMBOL || tk1->tt == TK_USYMBOL)) {
				tk1->tt = TK_METANAME;
				kArray_add(tlsdst, tk1);
				if(i + 2 < e && tls->tts[i+2]->topch == '(') {
					i = makeTree(_ctx, ks, tls, AST_PARENTHESIS, i+2, e, ')', tlsdst);
				}
				else {
					i++;
				}
				continue;
			} else {
			}
		}
		if(tk->tt != TK_INDENT) break;
		if(*indent == 0) *indent = tk->lpos;
	}
	for(; i < e ; i++) {
		kToken *tk = tls->tts[i];
		if(tk->topch == '{') {
			i = makeTree(_ctx, ks, tls, AST_BRACE, i, e, '}', tlsdst); continue;
		}
		if(tk->topch == '[') {
			i = makeTree(_ctx, ks, tls, AST_BRANCET, i, e, ']', tlsdst); continue;
		}
		if(tk->topch == '(') {
			i = makeTree(_ctx, ks, tls, AST_PARENTHESIS, i, e, ')', tlsdst); continue;
		}
		if(tk->topch == ';') {
			i++; break;
		}
		if(tk->tt == TK_INDENT) {
			if(tk->lpos <= *indent) break;
			continue;
		}
		checkKeyword(_ctx, ks, tk);
		kArray_add(tls, tk);
	}
	return i;
}

static kExpr* Stmt_newExpr2(CTX, kStmt *stmt, kArray *tls, int s, int e);

static int Stmt_addAnnotation(CTX, kStmt *stmt, kArray *tls, int s, int e)
{
	int i;
	for(i = s; i < e; i++) {
		kToken *tk = tls->tts[i];
		if(tk->tt != TK_METANAME) break;
		if(i+1 < e) {
			char buf[64];
			snprintf(buf, sizeof(buf), "@%s", S_text(tk->text));
			keyword_t kw = keyword(_ctx, (const char*)buf, S_size(tk->text)+1, FN_NEWID);
			kToken *tk1 = tls->tts[i+1];
			kObject *value = UPCAST(K_TRUE);
			if(tk1->tt == AST_PARENTHESIS) {
				value = (kObject*)Stmt_newExpr2(_ctx, stmt, tk1->sub, 0, kArray_size(tk1->sub));
				i++;
			}
			if(value != NULL) {
				kObject_setObject(stmt, kw, value);
			}
		}
	}
	return i;
}

static void WARN_Ignored(CTX, kArray *tls, int s, int e)
{
	if(s < e) {
		int i = s;
		kwb_t wb;
		kwb_init(&(_ctx->stack->cwb), &wb);
		kwb_printf(&wb, "%s", kToken_s(tls->tts[i])); i++;
		while(i < e) {
			kwb_printf(&wb, " %s", kToken_s(tls->tts[i])); i++;
		}
		SUGAR_P(WARN_, tls->tts[s]->uline, tls->tts[s]->lpos, "ignored tokens: %s", kwb_top(&wb, 1));
		kwb_free(&wb);
	}
}

static int StmtAdd(CTX, ksyntax_t *syn, kStmt *stmt, ksymbol_t name, kArray *tls, int s, int e)
{
	INIT_GCSTACK();
	BEGIN_LOCAL(lsfp, 8);
	KSETv(lsfp[K_CALLDELTA+0].o, (kObject*)stmt);
	lsfp[K_CALLDELTA+0].ndata = (uintptr_t)syn;
	lsfp[K_CALLDELTA+1].ivalue = name;
	KSETv(lsfp[K_CALLDELTA+2].a, tls);
	lsfp[K_CALLDELTA+3].ivalue = s;
	lsfp[K_CALLDELTA+4].ivalue = e;
	KCALL(lsfp, 0, syn->StmtAdd, 4, knull(CT_Int));
	END_LOCAL();
	RESET_GCSTACK();
	return (int)lsfp[0].ivalue;
}

static int matchSyntaxRule(CTX, kStmt *stmt, kArray *rules, kline_t /*parent*/uline, kArray *tls, int s, int e, int optional)
{
	int ri, ti = s;
	for(ri = 0; ri < kArray_size(rules) && ti < e; ri++) {
		kToken *rule = rules->tts[ri];
		kToken *tk = tls->tts[ti];
		uline = tk->uline;
		DBG_P("matching rule=%d,%s,%s token=%d,%s,%s", ri, T_tt(rule->tt), T_kw(rule->kw), ti-s, T_tt(tk->tt), kToken_s(tk));
		if(rule->tt == TK_CODE) {
			if(rule->kw != tk->kw) {
				if(optional) return s;
				kToken_p(tk, ERR_, "%s needs '%s'", stmt->syn->token, T_kw(rule->kw));
				return -1;
			}
			ti++;
			continue;
		}
		else if(rule->tt == TK_METANAME) {
			ksyntax_t *syn = SYN_(kStmt_ks(stmt), rule->kw);
			int next = StmtAdd(_ctx, syn, stmt, rule->nameid, tls, ti, e);
			if(syn == NULL || syn->StmtAdd == NULL) {
				kToken_p(tk, ERR_, "unknown sugar parser: %s", T_kw(rule->kw));
				return -1;
			}
//			DBG_P("matched '%s' nameid='%s', next=%d=>%d", Pkeyword(rule->kw), Pkeyword(rule->nameid), ti, next);
			if(next == -1) {
				if(optional) return s;
				kToken_p(tk, ERR_, "%s must be syntax %s", kToken_s(tk), T_kw(rule->kw));
				return -1;
			}
			optional = 0;
			ti = next;
			continue;
		}
		else if(rule->tt == AST_OPTIONAL) {
			int next = matchSyntaxRule(_ctx, stmt, rule->sub, uline, tls, ti, e, 1);
			if(next == -1) return -1;
			ti = next;
			continue;
		}
		else if(rule->tt == AST_PARENTHESIS || rule->tt == AST_BRACE || rule->tt == AST_BRANCET) {
			if(tk->tt == rule->tt && rule->topch == tk->topch) {
				int next = matchSyntaxRule(_ctx, stmt, rule->sub, uline, tk->sub, 0, kArray_size(tk->sub), 0);
				if(next == -1) return -1;
				ti++;
			}
		}
	}
	if(!optional) {
		for(; ri < kArray_size(rules); ri++) {
			kToken *rule = rules->tts[ri];
			if(rule->tt != AST_OPTIONAL) {
				SUGAR_P(ERR_, uline, -1, "%s needs %s syntax", stmt->syn->token, T_kw(rule->kw));
				return -1;
			}
		}
		WARN_Ignored(_ctx, tls, ti, e);
	}
	return e;
}

// In future, typeof operator is introduced
#define TK_isType(TK)    ((TK)->kw == KW_TYPE)
#define TK_type(TK)       (TK)->ty

static int Stmt_isType(CTX, kStmt *stmt, kArray *tls, int s, int e, int *next)
{
	kKonohaSpace *ks = kStmt_ks(stmt);
	kToken *tk = tls->tts[s];
	while(tk->tt == TK_NONE) {
		s++;
		tk = tls->tts[s];
	}
	if(tk->kw == KW_TYPE) {
		*next = s + 1;
		return 1;
	}
	else if(tk->tt == TK_USYMBOL) {
		kcid_t ty = kKonohaSpace_getcid(ks, S_text(tk->text), S_size(tk->text), TY_unknown);
		if(ty != TY_unknown) {
			tk->kw = KW_TYPE;
			tk->ty = ty;
			*next = s + 1;
			return 1;
		}
	}
	else {
		ksyntax_t *syn = SYN_(ks, tk->kw);
		if(syn->ty != TY_unknown) {
			tk->kw = KW_TYPE;
			tk->ty = syn->ty;
			*next = s + 1;
			return 1;
		}
	}
	return 0;
}

static inline kToken* TokenArray_lookAhead(CTX, kArray *tls, int s, int e)
{
	return (s < e) ? tls->tts[s] : K_NULLTOKEN;
}

static keyword_t Stmt_stmttype(CTX, kStmt *stmt, kArray *tls, int s, int e)
{
	int n = s;
	if(Stmt_isType(_ctx, stmt, tls, s, e, &n)) {
		DBG_P("Found a type at the top of stmt");
		kToken *tk = TokenArray_lookAhead(_ctx, tls, n, e);
		if(tk->tt == TK_SYMBOL || tk->tt == TK_USYMBOL) {
			tk = TokenArray_lookAhead(_ctx, tls, n+1, e);
			if(tk->tt == AST_PARENTHESIS || tk->kw == KW_DOT) {
				return KW_void; // because void is used in defintion
			}
			return KW_COLON;  // : is a typing operator
		}
		return KW_EXPR;
	}
	return (tls->tts[s])->kw;
}

static kbool_t Stmt_parseSyntaxRule(CTX, kStmt *stmt, kArray *tls, int s, int e)
{
	dumpTokenArray(_ctx, 0, tls, s, e);
	keyword_t kw = Stmt_stmttype(_ctx, stmt, tls, s, e);
	ksyntax_t *syn = SYN_(kStmt_ks(stmt), kw);
	if(syn != NULL && SYN_isExpr(syn) && syn->syntaxRule == NULL) {
		syn = SYN_(kStmt_ks(stmt), KW_EXPR);
	}
	int ret = 0;
	if(syn != NULL && syn->syntaxRule != NULL) {
//		DBG_P("trying matching %s", syn->token);
		stmt->syn = syn;
		e = matchSyntaxRule(_ctx, stmt, syn->syntaxRule, stmt->uline, tls, s, e, 0);
//		DBG_P("%s matched e=%d", syn->token, e);
		if(e != -1) {
			ret = 1;
			goto L_return;
		}
	}
	else {
		if(syn != NULL && syn->ExprTyCheck != NULL) {
			SUGAR_P(ERR_, stmt->uline, 0, "not available at the head of expression: %s", T_kw(kw));
		}
		else {
			SUGAR_P(ERR_, stmt->uline, 0, "unknown syntax rule for '%s'", T_kw(kw));
		}
	}
	L_return:;
	return ret;
}

static void Block_addStmtLine(CTX, kBlock *bk, kArray *tls, int s, int e)
{
	kStmt *stmt = new_(Stmt, tls->tts[s]->uline);
	kArray_add(bk->blockS, stmt);
	KINITv(stmt->parentNULL, bk);
	int estart = kerrno;
	s = Stmt_addAnnotation(_ctx, stmt, tls, s, e);
	if(!Stmt_parseSyntaxRule(_ctx, stmt, tls, s, e)) {
		kStmt_toERR(stmt, estart);
	}
	assert(stmt->syn != NULL);
}

/* ------------------------------------------------------------------------ */

static KMETHOD UndefinedStmtParseExpr(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtParseExpr(stmt, syn, tls, s, c, e);
	kToken *tk = tls->tts[c];
	SUGAR_P(ERR_, tk->uline, tk->lpos, "undefined expression parser for '%s'", kToken_s(tk));
	RETURN_(K_NULLEXPR); // unnecessary
}


static kExpr *ParseExpr(CTX, ksyntax_t *syn, kStmt *stmt, kArray *tls, int s, int c, int e)
{
	kMethod *mtd = (syn == NULL || syn->StmtParseExpr == NULL) ? kevalshare->UndefinedStmtParseExpr : syn->StmtParseExpr;
	BEGIN_LOCAL(lsfp, 10);
	KSETv(lsfp[K_CALLDELTA+0].o, (kObject*)stmt);
	lsfp[K_CALLDELTA+0].ndata = (uintptr_t)syn;  // quick access
	KSETv(lsfp[K_CALLDELTA+1].o, tls);
	lsfp[K_CALLDELTA+2].ivalue = s;
	lsfp[K_CALLDELTA+3].ivalue = c;
	lsfp[K_CALLDELTA+4].ivalue = e;
	KCALL(lsfp, 0, mtd, 4, K_NULLEXPR);
	END_LOCAL();
	DBG_ASSERT(IS_Expr(lsfp[0].o));
	return lsfp[0].expr;
}

/* ------------------------------------------------------------------------ */

static kbool_t Stmt_isUniaryOp(CTX, kStmt *stmt, kToken *tk)
{
	ksyntax_t *syn = SYN_(kStmt_ks(stmt), tk->kw);
	return (syn != NULL && syn->op1 != 0);
}

static int Stmt_skipUninaryOp(CTX, kStmt *stmt, kArray *tls, int s, int e)
{
	int i;
	for(i = s; i < e; i++) {
		kToken *tk = tls->tts[i];
		if(!Stmt_isUniaryOp(_ctx, stmt, tk)) {
			break;
		}
	}
	return i;
}

static int Stmt_findBinaryOp(CTX, kStmt *stmt, kArray *tls, int s, int e, ksyntax_t **synR)
{
	int idx = -1, i, prif = 0;
	for(i = Stmt_skipUninaryOp(_ctx, stmt, tls, s, e); i < e; i++) {
		kToken *tk = tls->tts[i];
		ksyntax_t *syn = SYN_(kStmt_ks(stmt), tk->kw);
		if(syn != NULL && syn->op2 != 0) {
			//DBG_P("operator: %s priotiry=%d", Pkeyword(syn->kw), syn->priority);
			if(prif < syn->priority || (prif == syn->priority && syn->right == 1)) {
				prif = syn->priority;
				idx = i;
				*synR = syn;
			}
			i = Stmt_skipUninaryOp(_ctx, stmt, tls, i+1, e) - 1;
		}
	}
	return idx;
}

static kExpr *Stmt_addExprParams(CTX, kStmt *stmt, kExpr *expr, kArray *tls, int s, int e)
{
	int i, start = s;
	DBG_ASSERT(kArray_size(expr->consNUL)  == 2);
	for(i = s; i < e; i++) {
		kToken *tk = tls->tts[i];
		if(tk->kw == KW_COMMA) {
			if(start < i) {
				expr = Expr_add(_ctx, expr, Stmt_newExpr2(_ctx, stmt, tls, start, i));
				start = i + 1;
			}
			else {
				// NULL;
				// Expr_add(_ctx, expr, )
			}
		}
	}
	if(start < i) {
		expr = Expr_add(_ctx, expr, Stmt_newExpr2(_ctx, stmt, tls, start, i));
	}
	kArray_clear(tls, 0);
	return expr;
}

static kExpr* Stmt_newExpr2(CTX, kStmt *stmt, kArray *tls, int s, int e)
{
	if(s < e) {
		ksyntax_t *syn = NULL;
		int idx = Stmt_findBinaryOp(_ctx, stmt, tls, s, e, &syn);
		if(idx != -1) {
			//DBG_P("** Found BinaryOp: s=%d, idx=%d, e=%d, %s**", s, idx, e, S_text(tls->tts[idx]->text));
			return ParseExpr(_ctx, syn, stmt, tls, s, idx, e);
		}
		int c = s;
		if(Stmt_isType(_ctx, stmt, tls, s, e, &c)) {
			kToken *tkT = tls->tts[c-1];
			if(c < e) {   // typing T v
				kToken *tk = new_(Token, TK_OPERATOR);
				tk->kw  = KW_COLON;
				KSETv(tk->text, Skeyword(tk->kw));  // tls
				syn = SYN_(kStmt_ks(stmt), KW_COLON);
				return new_ConsExpr(_ctx, syn, 3, tk, Stmt_newExpr2(_ctx, stmt, tls, c, e), tkT);
			}
			c = c - 1;
		}
		syn = SYN_(kStmt_ks(stmt), (tls->tts[c])->kw);
		return ParseExpr(_ctx, syn, stmt, tls, c, c, e);
	}
	else {
		SUGAR_P(ERR_, stmt->uline, 0, "empty");
		return K_NULLEXPR;
	}
}


#define kExpr_join(EXPR, STMT, TLS, S, C, E)    Expr_join(_ctx, EXPR, STMT, TLS, S, C, E)

static kExpr *Expr_join(CTX, kExpr *expr, kStmt *stmt, kArray *tls, int s, int c, int e)
{
	if(c < e) {
		WARN_Ignored(_ctx, tls, c, e);
	}
	return expr;
}

static KMETHOD StmtParseExpr_Term(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtParseExpr(stmt, syn, tls, s, c, e);
	assert(s == c);
	kToken *tk = tls->tts[c];
	kExpr *expr = new_(Expr, NULL);
	PUSH_GCSTACK(expr);
	Expr_setTerm(expr, 1);
	KINITv(expr->tkNUL, tk);
	expr->syn = SYN_(kStmt_ks(stmt), tk->kw);
	RETURN_(kExpr_join(expr, stmt, tls, s+1, c+1, e));
}

#define Expr_isBinaryOp(E,KW)  (!Expr_isTerm(E)&&IS_Token(E->consNUL->tts[0])&&E->consNUL->tts[0]->kw == KW)

static KMETHOD StmtParseExpr_PARENTHESIS(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtParseExpr(stmt, syn, tls, s, c, e);
	kToken *tk = tls->tts[c];
	if(s == c) {
		kExpr *expr = Stmt_newExpr2(_ctx, stmt, tk->sub, 0, kArray_size(tk->sub));
		RETURN_(kExpr_join(expr, stmt, tls, s+1, c+1, e));
	}
	else {
		kExpr *lexpr = Stmt_newExpr2(_ctx, stmt, tls, s, c);
		if(Expr_isBinaryOp(lexpr, KW_DOT)) {
			KSETv(lexpr->consNUL->exprs[0], lexpr->consNUL->exprs[2]);
			kArray_clear(lexpr->consNUL, 2);
			lexpr->syn = SYN_(kStmt_ks(stmt), KW_params); // CALL
		}
		else {
			syn = SYN_(kStmt_ks(stmt), KW_PARENTHESIS);
			lexpr  = new_ConsExpr(_ctx, syn, 2, lexpr, K_NULL);
		}
		lexpr = Stmt_addExprParams(_ctx, stmt, lexpr, tk->sub, 0, kArray_size(tk->sub));
		RETURN_(kExpr_join(lexpr, stmt, tls, s+1, c+1, e));
	}
}

static KMETHOD StmtParseExpr_COMMA(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtParseExpr(stmt, syn, tls, s, c, e);
	kExpr *expr = new_ConsExpr(_ctx, syn, 1, tls->tts[c]);
	expr = Stmt_addExprParams(_ctx, stmt, expr, tls, s, e);
	RETURN_(expr);
}

static KMETHOD StmtParseExpr_Op(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtParseExpr(stmt, syn, tls, s, c, e);
	kToken *tk = tls->tts[c];
	kExpr *expr, *rexpr = Stmt_newExpr2(_ctx, stmt, tls, c+1, e);
	if(s == c) { // uniary operator
		expr = new_ConsExpr(_ctx, syn, 2, tk, rexpr);
	}
	else {   // binary operator
		kExpr *lexpr = Stmt_newExpr2(_ctx, stmt, tls, s, c);
		expr = new_ConsExpr(_ctx, syn, 3, tk, lexpr, rexpr);
	}
	RETURN_(expr);
}

//static KMETHOD StmtParseExpr_DOT(CTX, ksfp_t *sfp _RIX)
//{
//	VAR_StmtParseExpr(stmt, syn, tls, s, c, e);
//	DBG_ASSERT(s < c); // binary
//	kExpr *lexpr = Stmt_newExpr2(_ctx, stmt, tls, s, c);
//	kExpr *rexpr = Stmt_newExpr2(_ctx, stmt, tls, c+1, e);
//	kToken *tk = tls->tts[c];
//	rexpr = new_ConsExpr(_ctx, syn, 3, tk, lexpr, rexpr);
//	RETURN_(rexpr);
//}

/* ------------------------------------------------------------------------ */

static KMETHOD StmtAdd_expr(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtAdd(stmt, syn, name, tls, s, e);
	INIT_GCSTACK();
	kExpr *expr = Stmt_newExpr2(_ctx, stmt, tls, s, e);
	if(expr != K_NULLEXPR) {
		dumpExpr(_ctx, 0, 0, expr);
		kObject_setObject(stmt, name, expr);
	}
	else {
		e = -1;
	}
	RESET_GCSTACK();
	RETURNi_(e);
}

static KMETHOD StmtAdd_type(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtAdd(stmt, syn, name, tls, s, e);
	int r = -1;
	if(Stmt_isType(_ctx, stmt, tls, s, e, &r)) {
		kToken *tkT = tls->tts[r-1];
		kObject_setObject(stmt, name, tkT);
	}
	else {
		r = -1;
	}
	RETURNi_(r);
}

static KMETHOD StmtAdd_cname(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtAdd(stmt, syn, name, tls, s, e);
	int r = -1;
	kToken *tk = tls->tts[s];
	if(tk->tt == TK_USYMBOL) {
		kObject_setObject(stmt, name, tk);
		r = s + 1;
	}
	RETURNi_(r);
}

static KMETHOD StmtAdd_name(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtAdd(stmt, syn, name, tls, s, e);
	int r = -1;
	kToken *tk = tls->tts[s];
	if(tk->tt == TK_SYMBOL) {
		kObject_setObject(stmt, name, tk);
		r = s + 1;
	}
	RETURNi_(r);
}

static KMETHOD StmtAdd_params(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtAdd(stmt, syn, name, tls, s, e);
	int r = -1;
	kToken *tk = tls->tts[s];
	if(tk->tt == AST_PARENTHESIS) {
		INIT_GCSTACK();
		kExpr *expr = new_ConsExpr(_ctx, SYN_(kStmt_ks(stmt), KW_params), 0);
		expr = Stmt_addExprParams(_ctx, stmt, expr, tk->sub, 0, kArray_size(tk->sub));
		if(expr != NULL) {
			//dumpExpr(_ctx, 0, 0, expr);
			kObject_setObject(stmt, name, expr);
			r = s + 1;
		}
		RESET_GCSTACK();
	}
	RETURNi_(r);
}

static KMETHOD StmtAdd_block(CTX, ksfp_t *sfp _RIX)
{
	INIT_GCSTACK();
	VAR_StmtAdd(stmt, syn, name, tls, s, e);
	kToken *tk = tls->tts[s];
	DBG_P("adding block %s as %s", T_tt(tk->tt), T_kw(name));
	if(tk->tt == TK_CODE) {
		kObject_setObject(stmt, name, tk);
		s++;
	}
	else if(tk->tt == AST_BRACE) {
		kBlock *bk = new_Block(_ctx, tk->sub, 0, kArray_size(tk->sub), kStmt_ks(stmt));
		kObject_setObject(stmt, name, bk);
		s++;
	}
	else {
		DBG_P("block1, s=%d, e=%d", s, e);
		kBlock *bk = new_Block(_ctx, tls, s, e, kStmt_ks(stmt));
		kObject_setObject(stmt, name, bk);
		e = s;
		// s = -1; // ERROR
	}
	RESET_GCSTACK();
	RETURNi_(s);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
