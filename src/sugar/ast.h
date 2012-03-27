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

#define SYM_error 0
#define SYM_expr  1

/* ------------------------------------------------------------------------ */
// Block

static int selectStmtLine(CTX, kLingo *lgo, int *indent, kArray *tls, int s, int e, kArray *tlsdst);
static void Block_addStmtLine(CTX, kBlock *bk, kArray *tls, int s, int e);

kBlock *new_Block(CTX, kArray *tls, int s, int e, kLingo* lgo)
{
	kBlock *bk = new_(Block, lgo);
	PUSH_GCSTACK(bk);
	int i = s, indent = 0, atop = kArray_size(tls);
	while(i < e) {
		i = selectStmtLine(_ctx, lgo, &indent, tls, i, e, tls);
		int asize = kArray_size(tls);
		if(asize > atop) {
			Block_addStmtLine(_ctx, bk, tls, atop, asize);
			kArray_clear(tls, atop);
		}
	}
	return bk;
}

static void checkKeyword(CTX, kLingo *ns, kToken *tk)
{
	if(tk->tt == TK_SYMBOL || tk->tt == TK_OPERATOR) {
		keyword_t keyid = keyword(_ctx, S_text(tk->text), S_size(tk->text), FN_NONAME);
		if(keyid != FN_NONAME) {
			ksyntax_t *syn = kLingo_syntax(_ctx, ns, keyid, 0);
			if(syn != NULL) {
				tk->tt = TK_KEYWORD; tk->keyid = keyid;
			}
		}
	}
}

static kbool_t kToken_toBRACE(CTX, kToken *tk)
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

static int makeTree(CTX, kLingo *ns, kArray *tls, ktoken_t tt, int s, int e, int closech, kArray *tlsdst)
{
	int i;
	kToken *tk = tls->tts[s];
	kToken *tkp = new_(Token, 0);
	kArray_add(tlsdst, tkp);
	tkp->tt = tt; tkp->uline = tk->uline; tkp->topch = tk->topch; tkp->lpos = closech;
	KSETv(tkp->sub, new_(Array, 0));
	for(i = s + 1; i < e; i++) {
		kToken *tk = tls->tts[i];
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
		kerror(_ctx, ERR_, tk->uline, tk->lpos, "%c is expected", closech);
	}
	return e;
}

static int selectStmtLine(CTX, kLingo *lgo, int *indent, kArray *tls, int s, int e, kArray *tlsdst)
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
					i = makeTree(_ctx, lgo, tls, AST_PARENTHESIS, i+2, e, ')', tlsdst);
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
			i = makeTree(_ctx, lgo, tls, AST_BRACE, i, e, '}', tlsdst); continue;
		}
		if(tk->topch == '[') {
			i = makeTree(_ctx, lgo, tls, AST_BRANCET, i, e, ']', tlsdst); continue;
		}
		if(tk->topch == '(') {
			i = makeTree(_ctx, lgo, tls, AST_PARENTHESIS, i, e, ')', tlsdst); continue;
		}
		if(tk->topch == ';') {
			i++; break;
		}
//		if(tk->tt == TK_CODE) {  ???
//			if (kflag_test(kevalmod->flags, FLAG_METHOD_LAZYCOMPILE)) {
//				kToken_toBRACE(_ctx, tk);
//			}
//		}
		if(tk->tt == TK_INDENT) {
			if(tk->lpos <= *indent) break;
			continue;
		}
		checkKeyword(_ctx, lgo, tk);
		kArray_add(tls, tk);
	}
	return i;
}

static kExpr* Stmt_newExpr(CTX, kStmt *stmt, kArray *tls, int s, int e, int *next);

static int Stmt_addAnnotation(CTX, kStmt *stmt, kArray *tls, int s, int e)
{
	int i;
	for(i = s; i < e; i++) {
		kToken *tk = tls->tts[i];
		if(tk->tt != TK_METANAME) break;
		if(i+1 < e) {
			char buf[128];
			snprintf(buf, sizeof(buf), "@%s", S_text(tk->text));
			keyword_t keyid = keyword(_ctx, (const char*)buf, S_size(tk->text)+1, FN_NEWID);
			kToken *tk1 = tls->tts[i+1];
			kObject *value = UPCAST(K_TRUE);
			if(tk1->tt == AST_PARENTHESIS) {
				value = (kObject*)Stmt_newExpr(_ctx, stmt, tk1->sub, 0, kArray_size(tk1->sub), 0);
				i++;
			}
			if(value != NULL) {
				kObject_setObject(stmt, keyid, value);
			}
		}
	}
	return i;
}

static int Stmt_addSugarSyntax(CTX, ksyntax_t *syn, kStmt *stmt, ksymbol_t name, kArray *tls, int s, int e)
{
	BEGIN_LOCAL(lsfp, 8);
	KSETv(lsfp[K_CALLDELTA+0].o, (kObject*)stmt);
	lsfp[K_CALLDELTA+1].ivalue = name;
	KSETv(lsfp[K_CALLDELTA+2].a, tls);
	lsfp[K_CALLDELTA+3].ivalue = s;
	lsfp[K_CALLDELTA+4].ivalue = e;
	KCALL(lsfp, 0, syn->StmtAdd, 4);
	END_LOCAL();
	return (int)lsfp[0].ivalue;
}

static void WARN_Ignored(CTX, kArray *tls, int s, int e)
{
	if(s < e) {
		kToken *tkS = tls->tts[s];
		kToken *tkE = tls->tts[e-1];
		if(tkS == tkE) {
			kerror(_ctx, WARN_, tkS->uline, tkS->lpos, "ignored token %s", kToken_s(tkS));
		}
		else {
			kerror(_ctx, WARN_, tkS->uline, tkS->lpos, "ignored tokens %s .. %s", kToken_s(tkS), kToken_s(tkE));
		}
	}
}

static int matchSyntaxRule(CTX, kStmt *stmt, kArray *rules, kline_t /*parent*/uline, kArray *tls, int s, int e, int optional)
{
	int ri, ti = s;
	for(ri = 0; ri < kArray_size(rules) && ti < e; ri++) {
		kToken *rule = rules->tts[ri];
		kToken *tk = tls->tts[ti];
		uline = tk->uline;
		DBG_P("matching rule=%d,%s,%s token=%d,%s,%s", ri, T_tt(rule->tt), T_kw(rule->keyid), ti-s, T_tt(tk->tt), kToken_s(tk));
		if(rule->tt == TK_KEYWORD) {
			if(tk->tt != TK_KEYWORD || rule->keyid != tk->keyid) {
				if(optional) return s;
				kerror(_ctx, ERR_, tk->uline, tk->lpos, "needs '%s'", T_kw(rule->keyid));
				return -1;
			}
			ti++;
			continue;
		}
		else if(rule->tt == TK_OPERATOR) {
			ksyntax_t *syn = kLingo_syntax(_ctx, kStmt_lgo(stmt), rule->keyid, 0);
			if(syn == NULL || syn->StmtAdd == NULL) {
				kerror(_ctx, ERR_, tk->uline, tk->lpos, "unknown sugar %s", T_kw(rule->keyid));
				return -1;
			}
			int next = Stmt_addSugarSyntax(_ctx, syn, stmt, rule->nameid, tls, ti, e);
//			DBG_P("matched '%s' nameid='%s', next=%d=>%d", Pkeyword(rule->keyid), Pkeyword(rule->nameid), ti, next);
			if(next == -1) {
				if(optional) return s;
				kerror(_ctx, ERR_, tk->uline, tk->lpos, "%s must be %s", kToken_s(tk), T_kw(rule->keyid));
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
				kerror(_ctx, ERR_, uline, -1, "needs %s", T_kw(rule->keyid));
				return -1;
			}
		}
		WARN_Ignored(_ctx, tls, ti, e);
	}
	return e;
}

static int Stmt_isType(CTX, kStmt *stmt, kArray *tls, int s, int e, int *next)
{
	kLingo *lgo = kStmt_lgo(stmt);
	kToken *tk = tls->tts[s];
	while(tk->tt == TK_NONE) {
		s++;
		tk = tls->tts[s];
	}
	if(tk->tt == TK_TYPE) {
		*next = s + 1;
		return 1;
	}
	else if(tk->tt == TK_KEYWORD) {
		ksyntax_t *syn = kLingo_syntax(_ctx, lgo, tk->keyid, 0);
		if(syn->ty != CLASS_unknown) {
			tk->tt = TK_TYPE;
			tk->ty = syn->ty;
			*next = s + 1;
			return 1;
		}
	}
	else if(tk->tt == TK_USYMBOL) {
		kcid_t ty = kLingo_getcid(lgo, S_text(tk->text), S_size(tk->text), CLASS_unknown);
		if(ty != CLASS_unknown) {
			tk->tt = TK_TYPE;
			tk->ty = ty;
			*next = s + 1;
			return 1;
		}
	}
	return 0;
}

static kToken* TokenArray_lookAhead(CTX, kArray *tls, int s, int e)
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
			if(tk->tt == AST_PARENTHESIS || (tk->tt == TK_KEYWORD && tk->keyid == KW_DOT)) {
				DBG_P("Found method decl");
				return KW_DECLMETHOD;
			}
			DBG_P("Found type decl");
			return KW_DECLTYPE;
		}
		DBG_P("not tk->tt=%s", T_tt(tk->tt));
	}
	kToken *tk = tls->tts[s];
	if (tk->tt == TK_KEYWORD) {
		return tk->keyid;
	}
	return 1;  // expression
}

static kbool_t Stmt_makeTree(CTX, kStmt *stmt, kArray *tls, int s, int e)
{
	dumpTokenArray(_ctx, 0, tls, s, e);
	keyword_t keyid = Stmt_stmttype(_ctx, stmt, tls, s, e);
	ksyntax_t *syn = kLingo_syntax(_ctx, kStmt_lgo(stmt), keyid, 0);
	int ret = 0;
//	if (keyid == KW_DECLMETHOD) {
//		kevalmod->flags = kflag_set(kevalmod->flags, FLAG_METHOD_LAZYCOMPILE);
//	}
	if(syn != NULL && syn->syntaxRule != NULL) {
//		DBG_P("trying matching %s", syn->token);
		e = matchSyntaxRule(_ctx, stmt, syn->syntaxRule, stmt->uline, tls, s, e, 0);
//		DBG_P("%s matched e=%d", syn->token, e);
		if(e != -1) {
			stmt->syn = syn;
			ret = 1;
			goto L_return;
		}
	}
	else {
		kerror(_ctx, ERR_, stmt->uline, 0, "unknown statement %s", T_kw(keyid));
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
	if(!Stmt_makeTree(_ctx, stmt, tls, s, e)) {
		kStmt_toERR(stmt, estart);
	}
	assert(stmt->syn != NULL);
}

/* ------------------------------------------------------------------------ */

static kbool_t Stmt_isUninaryOp(CTX, kStmt *stmt, kToken *tk)
{
	if(tk->tt == TK_KEYWORD) {
		ksyntax_t *syn = kLingo_syntax(_ctx, kStmt_lgo(stmt), tk->keyid, 0);
		if(syn != NULL && syn->op1 != 0) return 1;
	}
	return 0;
}

static int Stmt_skipUninaryOp(CTX, kStmt *stmt, kArray *tls, int s, int e)
{
	int i;
	for(i = s; i < e; i++) {
		kToken *tk = tls->tts[i];
		if(!Stmt_isUninaryOp(_ctx, stmt, tk)) {
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
		if(tk->tt == TK_KEYWORD) {
			ksyntax_t *syn = kLingo_syntax(_ctx, kStmt_lgo(stmt), tk->keyid, 0);
			if(syn != NULL && syn->op2 != 0) {
				//DBG_P("operator: %s priotiry=%d", Pkeyword(syn->keyid), syn->priority);
				if(prif < syn->priority || (prif == syn->priority && syn->right == 1)) {
					prif = syn->priority;
					idx = i;
					*synR = syn;
				}
				i = Stmt_skipUninaryOp(_ctx, stmt, tls, i+1, e) - 1;
			}
		}
	}
	return idx;
}

static kExpr *Stmt_newTerm(CTX, kStmt *stmt, kArray *tls, int s, int e, int *next)
{
	int tn;
	if(Stmt_isType(_ctx, stmt, tls, s, e, &tn)) {
		kToken *tkT = tls->tts[tn-1];
		if(tn < e) {
			kToken *tk = new_(Token, TK_KEYWORD);
			tk->keyid  = KW_COLON; KSETv(tk->text, Skeyword(tk->keyid));
			return new_ConsExpr(_ctx, SYN_TYPEDECL, 3, tk, Stmt_newExpr(_ctx, stmt, tls, tn, e, next), tkT);
		}
		*next = e;
		return new_TermExpr(_ctx, tkT);
	}
	else {
		kToken *tk0 = tls->tts[s];
		*next = s + 1;
		switch((int)tk0->tt) {
			case TK_KEYWORD: {

			}
			case TK_SYMBOL:
			case TK_USYMBOL:
			case TK_TEXT:
			case TK_STEXT:
			case TK_INT:
			case TK_FLOAT:
			case TK_URN:   {
				return new_TermExpr(_ctx, tk0);
			}
			case TK_REGEX:
			case TK_MN:
			case TK_TYPE:
			case AST_PARENTHESIS: {
				return Stmt_newExpr(_ctx, stmt, tk0->sub, 0, kArray_size(tk0->sub), NULL);
			}
			case AST_BRACE:
			case AST_BRANCET:
				break;
			case AST_EXPR: {
				return tk0->expr;
			}
		}
		kerror(_ctx, ERR_, tk0->uline, tk0->lpos, "not a term: %s", kToken_s(tk0));
		return NULL;
	}
}

static kExpr *Stmt_addExprParams(CTX, kStmt *stmt, kExpr *expr, kArray *tls, int s, int e)
{
	int i, start = s;
	for(i = s; i < e; i++) {
		kToken *tk = tls->tts[i];
		if(tk->tt == TK_KEYWORD && tk->keyid == KW_COMMA) {
			if(start < i) {
				expr = Expr_add(_ctx, expr, Stmt_newExpr(_ctx, stmt, tls, start, i, NULL));
				start = i + 1;
			}
			else {
				// NULL;
				// Expr_add(_ctx, expr, )
			}
		}
	}
	if(start < i) {
		expr = Expr_add(_ctx, expr, Stmt_newExpr(_ctx, stmt, tls, start, i, NULL));
	}
	kArray_clear(tls, 0);
	return expr;
}

static kExpr* Stmt_newExprLeft(CTX, kStmt *stmt, kArray *tls, int s, int e, int *next)
{
	int i = s;
	kExpr *expr = Stmt_newTerm(_ctx, stmt, tls, s, e, &i);
	while(expr != NULL && i < e) {
		kToken *tk = tls->tts[i];
		if(tk->tt == AST_PARENTHESIS) {  // expr (expr, expr)
			expr = new_ConsExpr(_ctx, SYN_INVOKE, 2, expr, K_NULL);
			expr = Stmt_addExprParams(_ctx, stmt, expr, tk->sub, 0, kArray_size(tk->sub));
			i++;
			continue;
		}
//		if(tk->tt == AST_BRANCET) {  // expr [expr, expr]
//			kToken *tkM = new_(Token, TK_MN);
//			tkM->mn = ksymbol("get", sizeof("get")-1, MN_NEWID, SYMPOL_METHOD);
//			expr = new_ConsExpr(_ctx, NULL, 2, tkM, expr);
//			Expr_setCallOp(expr, 1);
//			expr = Stmt_addExprParams(_ctx, stmt, expr, tk->sub, 0, kArray_size(tk->sub));
//			s++;
//			continue;
//		}
		if(i < e) {
			WARN_Ignored(_ctx, tls, i, e);
			break;
		}
	}
	if(next != NULL) *next=e;
	return expr;
}

static kExpr* Stmt_newExpr(CTX, kStmt *stmt, kArray *tls, int s, int e, int *next)
{
	if(s < e) {
		ksyntax_t *syn = NULL;
		int idx = Stmt_findBinaryOp(_ctx, stmt, tls, s, e, &syn);
		if(idx != -1) {
			DBG_P("** Found BinaryOp: s=%d, idx=%d, e=%d, %s**", s, idx, e, S_text(tls->tts[idx]->text));
			if(syn->keyid == KW_COMMA) {
				kExpr *expr = new_ConsExpr(_ctx, syn, 1, tls->tts[idx]);
				return Stmt_addExprParams(_ctx, stmt, expr, tls, s, e);
			}
			kExpr *lexpr = Stmt_newExpr(_ctx, stmt, tls, s, idx, NULL);
			kExpr *rexpr = Stmt_newExpr(_ctx, stmt, tls, idx+1, e, next);
			if(syn->keyid == KW_DOT && lexpr != NULL && rexpr != NULL && rexpr->syn == SYN_INVOKE) {
				rexpr->syn = SYN_CALL;
				KSETv(rexpr->consNUL->list[1], lexpr);
				return rexpr;
			}
			else {
				kToken *tkOP = tls->tts[idx];
				return new_ConsExpr(_ctx, syn, 3, tkOP, lexpr, rexpr);
			}
		}
//		if(Stmt_isUninaryOp(_ctx, stmt, tls->tts[s])) {
//			DBG_P("** Found UninaryOp: %s **", S_text(tls->tts[s]->text));
//			return Stmt_newUninaryExpr(_ctx, stmt, tls, s, e, next);
//		}
		return Stmt_newExprLeft(_ctx, stmt, tls, s, e, next);
	}
	else {
		kerror(_ctx, ERR_, stmt->uline, 0, "empty");
		if(next != NULL) *next = e;
		return NULL;
	}
}

static KMETHOD StmtAdd_expr(CTX, ksfp_t *sfp _RIX)
{
	VAR_StmtAdd(stmt, name, tls, s, e);
	DBG_P("s=%d, e=%d", s, e);
	INIT_GCSTACK();
	kExpr *expr = Stmt_newExpr(_ctx, stmt, tls, s, e, &e);
	if(expr != NULL) {
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
	VAR_StmtAdd(stmt, name, tls, s, e);
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
	VAR_StmtAdd(stmt, name, tls, s, e);
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
	VAR_StmtAdd(stmt, name, tls, s, e);
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
	VAR_StmtAdd(stmt, name, tls, s, e);
	int r = -1;
	kToken *tk = tls->tts[s];
	if(tk->tt == AST_PARENTHESIS) {
		INIT_GCSTACK();
		kExpr *expr = new_ConsExpr(_ctx, SYN_PARAMS, 0);
		expr = Stmt_addExprParams(_ctx, stmt, expr, tk->sub, 0, kArray_size(tk->sub));
		if(expr != NULL) {
			dumpExpr(_ctx, 0, 0, expr);
			kObject_setObject(stmt, name, expr);
			r = s+1;
		}
		RESET_GCSTACK();
	}
	RETURNi_(r);
}

static KMETHOD StmtAdd_block(CTX, ksfp_t *sfp _RIX)
{
	INIT_GCSTACK();
	VAR_StmtAdd(stmt, name, tls, s, e);
	kToken *tk = tls->tts[s];
	if(tk->tt == TK_CODE) {
		kObject_setObject(stmt, name, tk);
		s++;
	}
	else if(tk->tt == AST_BRACE) {
		kBlock *bk = new_Block(_ctx, tk->sub, 0, kArray_size(tk->sub), kStmt_lgo(stmt));
		kObject_setObject(stmt, name, bk);
		s++;
	}
	else {
		DBG_P("block1, s=%d, e=%d", s, e);
		kBlock *bk = new_Block(_ctx, tls, s, e, kStmt_lgo(stmt));
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
