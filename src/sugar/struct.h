/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 * You may choose one of the following two licelanges when you use konoha.
 * If you want to use the latter licelange, please contact us.
 *
 * (1) GNU General Public Licelange 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure Licelange 1.
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

#define STRUCTNAME(C) \
	.structname = #C,\
	.cid = CLASS_newid,\
	.cflag = 0,\
	.cstruct_size = sizeof(k##C)\


/* --------------- */
/* Lingo */

static void Lingo_init(CTX, kRawPtr *o, void *conf)
{
	kLingo *lgo = (kLingo*)o;
	bzero(&lgo->parentNULL, sizeof(kLingo) - sizeof(kObjectHeader));
	lgo->parentNULL = conf;
	lgo->static_cid = CLASS_unknown;
}

static void syntax_reftrace(CTX, kmape_t *p)
{
	ksyntax_t *kw = (ksyntax_t*)p->uvalue;
	BEGIN_REFTRACE(4);
	KREFTRACEn(kw->syntaxRule);
	KREFTRACEn(kw->StmtAdd);
	KREFTRACEn(kw->StmtTyCheck);
	KREFTRACEn(kw->ExprTyCheck);
	END_REFTRACE();
}

static void symtbl_reftrace(CTX, kmape_t *p)
{
	BEGIN_REFTRACE(2);
	KREFTRACEn(p->skey);
	KREFTRACEn(p->ovalue);
	END_REFTRACE();
}

static void Lingo_reftrace(CTX, kRawPtr *o)
{
	kLingo *lgo = (kLingo*)o;
	if(lgo->syntaxMapNN != NULL) {
		kmap_reftrace(lgo->syntaxMapNN, syntax_reftrace);
	}
	if(lgo->symtblMapSO != NULL) {
		kmap_reftrace(lgo->symtblMapSO, symtbl_reftrace);
	}
	BEGIN_REFTRACE(3);
	KREFTRACEn(lgo->parentNULL);
	KREFTRACEn(lgo->script);
	KREFTRACEn(lgo->methodsNULL);
	END_REFTRACE();
}

static void syntax_free(CTX, void *p)
{
	KNH_FREE(p, sizeof(ksyntax_t));
}

static void Lingo_free(CTX, kRawPtr *o)
{
	kLingo *lgo = (kLingo*)o;
	if(lgo->syntaxMapNN != NULL) {
		kmap_free(lgo->syntaxMapNN, syntax_free);
	}
}

static const KSTRUCT_DEF LingoDef = {
	STRUCTNAME(Lingo),
	.init = Lingo_init,
	.reftrace = Lingo_reftrace,
	.free = Lingo_free,
};

static ksyntax_t* kLingo_syntax(CTX, kLingo *lgo0, keyword_t keyid, int isnew)
{
	kLingo *lgo = lgo0;
	uintptr_t hcode = keyid;
	while(lgo != NULL) {
		if(lgo->syntaxMapNN != NULL) {
			kmape_t *e = kmap_get(lgo->syntaxMapNN, hcode);
			while(e != NULL) {
				if(e->hcode == hcode) {
					return (ksyntax_t*)e->uvalue;
				}
				e = e->next;
			}
		}
		lgo = lgo->parentNULL;
	}
	if(isnew == 1) {
		if(lgo0->syntaxMapNN == NULL) {
			lgo0->syntaxMapNN = kmap_init(0);
		}
		kmape_t *e = kmap_newentry(lgo0->syntaxMapNN, hcode);
		kmap_add(lgo0->syntaxMapNN, e);
		ksyntax_t *syn = (ksyntax_t*)KNH_ZMALLOC(sizeof(ksyntax_t));
		e->uvalue = (uintptr_t)syn;
		return syn;
	}
	return NULL;
}

static ksymbol_t keyword(CTX, const char *name, size_t len, ksymbol_t def);
static void parseSyntaxRule(CTX, const char *rule, kline_t pline, kArray *a);

static void Lingo_defineSyntax(CTX, kLingo *lgo, ksyntaxdef_t *syndef)
{
	while(syndef->name != NULL) {
		keyword_t keyid = (syndef->keyid != 0) ? syndef->keyid : keyword(_ctx, syndef->name, syndef->namelen, FN_NEWID);
		ksyntax_t* syn = kLingo_syntax(_ctx, lgo, keyid, 1);
		syn->keyid = keyid;
		syn->token = syndef->name;
		syn->ty = (syndef->type != 0) ? (syndef->type) : CLASS_unknown;
		if(syndef->rule != NULL) {
			KINITv(syn->syntaxRule, new_(Array, 0));
			parseSyntaxRule(_ctx, syndef->rule, 0, syn->syntaxRule);
		}
		if(syndef->op1 != NULL) {
			syn->op1 = MN_NONAME;  // TODO
		}
		if(syndef->op2 != NULL) {
			if (syndef->op2[0] == '*') {
				syn->op2 = MN_NONAME;  // TODO
			} else {
				syn->op2 = ksymbol(syndef->op2, 127, FN_NEWID, SYMPOL_METHOD);
			}
			syn->priority = syndef->priority_op2;
			syn->right = syndef->right;
		}
		if(syndef->StmtAdd != NULL) {
			KINITv(syn->StmtAdd, new_kMethod(0, 0, 0, NULL, syndef->StmtAdd));
		}
		if(syndef->StmtTyCheck != NULL) {
			KINITv(syn->StmtTyCheck, new_kMethod(0, 0, 0, NULL, syndef->StmtTyCheck));
		}
		if(syndef->ExprTyCheck != NULL) {
			KINITv(syn->ExprTyCheck, new_kMethod(0, 0, 0, NULL, syndef->ExprTyCheck));
			ksyntax_t *e = kLingo_syntax(_ctx, lgo, 1, 0);
			if(syn->syntaxRule == NULL) {
				KINITv(syn->syntaxRule, e->syntaxRule);
			}
			if(syn->StmtTyCheck == NULL) {
				KINITv(syn->StmtTyCheck, e->StmtTyCheck);
			}
		}
//		if(syndef->CodeGen != NULL) {
//			KINITv(syn->CodeGen, new_kMethod(0, 0, 0, NULL, syndef->CodeGen));
//		}
		//DBG_P("DEFINE SYNTAX: keyid=%d '%s'", keyid, syn->token);
		DBG_ASSERT(syn == kLingo_syntax(_ctx, lgo, keyid, 0));
		syndef++;
	}
	// update
	kevalshare_t *base = kevalshare;
	base->syn_err  = kLingo_syntax(_ctx, base->rootlgo, 0, 0);
	base->syn_expr = kLingo_syntax(_ctx, base->rootlgo, 1, 0);
	base->syn_call = kLingo_syntax(_ctx, base->rootlgo, 1, 0);
	base->syn_invoke = kLingo_syntax(_ctx, base->rootlgo, KW_("$name"), 0);
	base->syn_params = kLingo_syntax(_ctx, base->rootlgo, KW_("$params"), 0);
	base->syn_return = kLingo_syntax(_ctx, base->rootlgo, KW_("return"), 0);
	base->syn_break = kLingo_syntax(_ctx, base->rootlgo, KW_("break"), 0);
	base->syn_typedecl = kLingo_syntax(_ctx, base->rootlgo, KW_(":"), 0);
}

// Lingo

static kObject *Lingo_getSymbolValueNULL(CTX, kLingo *lgo, const char *key, size_t klen)
{
	if(strcmp("Lingo", key) == 0) {
		return (kObject*)lgo;
	}
	return NULL;
}

static uintptr_t casehash(const char *name, size_t len)
{
	uintptr_t i, hcode = 0;
	for(i = 0; i < len; i++) {
		hcode = tolower(name[i]) + (31 * hcode);
	}
	return hcode;
}

//static void casehash_add(CTX, kmap_t *kmp, kString *skey, uintptr_t uvalue)
//{
//	uintptr_t hcode = casehash(S_text(skey), S_size(skey));
//	kmape_t *e = kmap_newentry(kmp, hcode);
//	KINITv(e->skey, skey);
//	e->uvalue = uvalue;
//	kmap_add(kmp, e);
//}

static uintptr_t casehash_get(CTX, kmap_t *kmp, const char *name, size_t len, uintptr_t def)
{
	uintptr_t hcode = casehash(name, len);
	kmape_t *e = kmap_get(kmp, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && S_size(e->skey) == len && strncasecmp(S_text(e->skey), name, len) == 0) {
			return e->uvalue;
		}
	}
	return def;
}

static kcid_t Lingo_getcid(CTX, kLingo *lgo, const char *name, size_t len, kcid_t def)
{
	kcid_t cid;
	//while(nsN != NULL) {
	//	nsN = nsN->parentNULL;
	//}
	cid = (kcid_t)casehash_get(_ctx, _ctx->share->classnameMapNN, name, len, def);
	return cid;
}

/* Lingo/Class/Method */
static kMethod* CT_findMethodNULL(CTX, const kclass_t *ct, kmethodn_t mn)
{
	const kclass_t *p, *t0 = ct;
	do {
		size_t i;
		kArray *a = t0->methods;
		for(i = 0; i < kArray_size(a); i++) {
			kMethod *mtd = a->methods[i];
			if((mtd)->mn == mn) {
				return mtd;
			}
		}
		p = t0;
		t0 = CT_(t0->supcid);
	}
	while(p != t0);
	return NULL;
}

#define kLingo_getMethodNULL(ns, cid, mn)     Lingo_getMethodNULL(_ctx, ns, cid, mn)
#define kLingo_getStaticMethodNULL(ns, mn)   Lingo_getStaticMethodNULL(_ctx, ns, mn)

kMethod* Lingo_getMethodNULL(CTX, kLingo *lgo, kcid_t cid, kmethodn_t mn)
{
	while(lgo != NULL) {
		if(lgo->methodsNULL != NULL) {
			size_t i;
			kArray *methods = lgo->methodsNULL;
			for(i = 0; i < kArray_size(methods); i++) {
				kMethod *mtd = methods->methods[i];
				if(mtd->cid == cid && mtd->mn == mn) {
					return mtd;
				}
			}
		}
		lgo = lgo->parentNULL;
	}
	return CT_findMethodNULL(_ctx, CT_(cid), mn);
}

static kMethod* Lingo_getStaticMethodNULL(CTX, kLingo *lgo, kmethodn_t mn)
{
	while(lgo != NULL) {
		if(lgo->static_cid != CLASS_unknown) {
			kMethod *mtd = kLingo_getMethodNULL(lgo, lgo->static_cid, mn);
			if(mtd != NULL && kMethod_isStatic(mtd)) {
				return mtd;
			}
		}
		lgo = lgo->parentNULL;
	}
	return NULL;
}

#define kLingo_addMethod(NS,MTD,UL)  Lingo_addMethod(_ctx, NS, MTD, UL)

static kbool_t Lingo_addMethod(CTX, kLingo *lgo, kMethod *mtd, kline_t pline)
{
	kMethod *mtdOLD = Lingo_getMethodNULL(_ctx, lgo, mtd->cid, mtd->mn);
	if(mtdOLD != NULL) {
		char mbuf[128];
		kerror(_ctx, ERR_, pline, -1, "method %s.%s is already defined", T_cid(mtd->cid), T_mn(mbuf, mtd->mn));
		return 0;
	}
	if(kMethod_isPublic(mtd)) {
		const kclass_t *ct = CT_(mtd->cid);
		kArray_add(ct->methods, mtd);
	}
	else {
		if(lgo->methodsNULL == NULL) {
			KINITv(lgo->methodsNULL, new_(Array, 8));
		}
		kArray_add(lgo->methodsNULL, mtd);
	}
	return 1;
}

static void Lingo_loadMethodData(CTX, kLingo *lgo, intptr_t *data)
{
	intptr_t *d = data;
	while(d[0] != -1) {
		uintptr_t flag = (uintptr_t)d[0];
		knh_Fmethod f = (knh_Fmethod)d[1];
		ktype_t rtype = (ktype_t)d[2];
		kcid_t cid  = (kcid_t)d[3];
		kmethodn_t mn = (kmethodn_t)d[4];
		size_t i, psize = (size_t)d[5];
		kparam_t p[psize+1];
		d = d + 6;
		for(i = 0; i < psize; i++) {
			p[i].ty = (ktype_t)d[0];
			p[i].fn = (ksymbol_t)d[1];
			d += 2;
		}
		kParam *pa = new_kParam(rtype, psize, p);
		kMethod *mtd = new_kMethod(flag, cid, mn, pa, f);
		if(lgo == NULL || kMethod_isPublic(mtd)) {
			kArray_add(CT_(cid)->methods, mtd);
		} else {
			if (lgo->methodsNULL == NULL) {
				KINITv(lgo->methodsNULL, new_(Array, 8));
			}
			kArray_add(lgo->methodsNULL, mtd);
		}
	}
}

#define kLingo_loadGlueFunc(NS, F, OPT, UL)  Lingo_loadGlueFunc(_ctx, NS, F, OPT, UL)

static knh_Fmethod Lingo_loadGlueFunc(CTX, kLingo *lgo, const char *funcname, int DOPTION, kline_t uline)
{
	void *f = NULL;
	if(lgo->gluehdr != NULL) {
		char namebuf[128];
		snprintf(namebuf, sizeof(namebuf), "D%s", funcname);
		if(DOPTION) {
			f = dlsym(lgo->gluehdr, (const char*)namebuf);
		}
		if(f == NULL) {
			f = dlsym(lgo->gluehdr, (const char*)namebuf+1);
		}
		kerror(_ctx, WARN_, uline, -1, "glue method function is not found: %s", namebuf + 1);
	}
	return f;
}

/* --------------- */
/* Token */

static void Token_init(CTX, kRawPtr *o, void *conf)
{
	kToken *tk = (kToken*)o;
	tk->uline     =   0;
	tk->tt        =   (ktoken_t)conf;
	tk->topch     =   0;
	tk->lpos      =   -1;
	KINITv(tk->text, TS_EMPTY);
}

static void Token_reftrace(CTX, kRawPtr *o)
{
	kToken *tk = (kToken*)o;
	BEGIN_REFTRACE(1);
	KREFTRACEv(tk->text);
	END_REFTRACE();
}

static const KSTRUCT_DEF TokenDef = {
	STRUCTNAME(Token),
	.init = Token_init,
	.reftrace = Token_reftrace,
};

static const char *kToken_s(kToken *tk)
{
	switch((int)tk->tt) {
	case AST_PARENTHESIS: return "(... )";
	case AST_BRACE: return "{... }";
	case AST_BRANCET: return "[... ]";
	default: 	return S_text(tk->text);
	}
}

static const char *T_tt(ktoken_t t)
{
	static const char* symTKDATA[] = {
		"TK_NONE",
		"TK_CODE",
		"TK_INDENT",
		"TK_WHITESPACE",
		"TK_OPERATOR",
		"TK_SYMBOL",
		"TK_USYMBOL",
		"TK_KEYWORD",
		"TK_TEXT",
		"TK_STEXT",
		"TK_BTEXT",
		"TK_INT",
		"TK_FLOAT",
		"TK_URN",
		"TK_REGEX",
		"TK_METANAME",
		"TK_MN",
		"TK_TYPE",

		"AST_()",
		"AST_{}",
		"AST_[]",
		"AST_OPTIONAL[]",
		"AST_TYPE<>",
		"AST_EXPR",
		"AST_STMT",
		"AST_BLOCK",
	};
	if(t <= AST_BLOCK) {
		return symTKDATA[t];
	}
	return "TK_UNKNOWN";
}

static void dumpToken(CTX, kToken *tk)
{
	if(tk->tt == TK_MN) {
		char buf[256];
		fprintf(stdout, "%s %d+%d: %s\n", T_tt(tk->tt), (short)tk->uline, tk->lpos, Tsymbol(_ctx, buf, sizeof(buf), tk->mn));
	}
	else if((int)tk->tt <= TK_TYPE) {
		fprintf(stdout, "%s %d+%d: '%s'\n", T_tt(tk->tt), (short)tk->uline, tk->lpos, S_text(tk->text));
	}
	else {
		fprintf(stdout, "%s\n", T_tt(tk->tt));
	}
}

static void dumpIndent(int nest)
{
	int i;
	for(i = 0; i < nest; i++) {
		fputc(' ', stdout); fputc(' ', stdout);
	}
}

static void dumpTokenArray(CTX, int nest, kArray *a, int s, int e)
{
	if(nest == 0) fprintf(stdout, "\n");
	while(s < e) {
		kToken *tk = a->tts[s];
		dumpIndent(nest);
		if(AST_PARENTHESIS <= (int)tk->tt && (int)tk->tt <= AST_TYPE) {
			fprintf(stdout, "%c\n", tk->topch);
			dumpTokenArray(_ctx, nest+1, tk->sub, 0, kArray_size(tk->sub));
			dumpIndent(nest);
			fprintf(stdout, "%c\n", tk->closech);
		}
		else {
			fprintf(stdout, "TK(%d) ", s);
			dumpToken(_ctx, tk);
		}
		s++;
	}
	if(nest == 0) fprintf(stdout, "====\n");
}

/* --------------- */
/* Expr */

static void Expr_init(CTX, kRawPtr *o, void *conf)
{
	kExpr *expr      =   (kExpr*)o;
	expr->build      =   TEXPR_UNTYPED;
	expr->ty         =   TY_var;
	expr->index      =   0;
	expr->xindex     =   0;
	expr->dataNUL    = NULL;
	expr->tkNUL      = NULL;
}

static void Expr_reftrace(CTX, kRawPtr *o)
{
	kExpr *expr = (kExpr*)o;
	BEGIN_REFTRACE(2);
	KREFTRACEn(expr->tkNUL);
	KREFTRACEn(expr->dataNUL);
	END_REFTRACE();
}

static const KSTRUCT_DEF ExprDef = {
	STRUCTNAME(Expr),
	.init = Expr_init,
	.reftrace = Expr_reftrace,
};

static kExpr* new_TermExpr(CTX, kToken *tk)
{
	kExpr *expr = new_(Expr, NULL);
	PUSH_GCSTACK(expr);
	Expr_setTerm(expr, 1);
	KINITv(expr->tkNUL, tk);
	return expr;
}

static kExpr* new_ConsExpr(CTX, ksyntax_t *syn, int n, ...)
{
	int i;
	va_list ap;
	va_start(ap, n);
	kExpr *expr = new_(Expr, NULL);
	DBG_ASSERT(syn != NULL);
	expr->syn = syn;
	PUSH_GCSTACK(expr);
	KINITv(expr->consNUL, new_(Array, 8));
	for(i = 0; i < n; i++) {
		kObject *v =  (kObject*)va_arg(ap, kObject*);
		if(v == NULL) return NULL;
		kArray_add(expr->consNUL, v);
	}
	va_end(ap);
	return expr;
}

static kExpr* Expr_add(CTX, kExpr *expr, void *e)
{
	if(expr != NULL && e != NULL) {
		kArray_add(expr->consNUL, e);
		return expr;
	}
	return NULL;
}

void dumpExpr(CTX, int n, int nest, kExpr *expr)
{
	if(nest == 0) fprintf(stdout, "\n");
	dumpIndent(nest);
	if(Expr_isTerm(expr)) {
		fprintf(stdout, "[%d] T: %s %s", n, T_tt(expr->tkNUL->tt), kToken_s(expr->tkNUL));
		if(expr->ty != TY_var) {

		}
		fprintf(stdout, "\n");
	}
	else {
		int i;
		fprintf(stdout, "[%d] C: build=%d, size=%ld", n, expr->build, kArray_size(expr->consNUL));
		if(expr->ty != TY_var) {

		}
		fprintf(stdout, "\n");
		for(i=0; i < kArray_size(expr->consNUL); i++) {
			kObject *o = expr->consNUL->list[i];
			if(O_ct(o) == CT_Expr) {
				dumpExpr(_ctx, i, nest+1, (kExpr*)o);
			}
			else {
				dumpIndent(nest+1);
				if(O_ct(o) == CT_Token) {
					kToken *tk = (kToken*)o;
					fprintf(stdout, "[%d] O: %s ", i, S_text(o->h.ct->name));
					dumpToken(_ctx, tk);
				}
				else if(o == K_NULL) {
					fprintf(stdout, "[%d] O: null\n", i);
				}
				else {
					fprintf(stdout, "[%d] O: %s\n", i, S_text(o->h.ct->name));
				}
			}
		}
	}
}

static kToken *Expr_firstToken(CTX, kExpr *expr)
{
	size_t i;
	kArray *cons = expr->consNUL;
	if(expr->tkNUL != NULL && expr->tkNUL->uline != 0) {
		return expr->tkNUL;
	}
	if(cons != NULL) {
		for(i = 0; i < kArray_size(cons); i++) {
			if(IS_Token(cons->list[i])) return cons->tts[i];
			if(IS_Expr(cons->list[i])) {
				kToken *tk = Expr_firstToken(_ctx, (kExpr*)cons->list[i]);
				if(tk != K_NULLTOKEN) return tk;
			}
		}
	}
	return K_NULLTOKEN;
}

#define new_ConstValue(T, O)  Expr_setConstValue(_ctx, NULL, T, UPCAST(O))
#define kExpr_setConstValue(EXPR, T, O)  Expr_setConstValue(_ctx, EXPR, T, UPCAST(O))

static kExpr* Expr_setConstValue(CTX, kExpr *expr, ktype_t ty, kObject *o)
{
	if(expr == NULL) {
		expr = new_(Expr, 0);
	}
	DBG_ASSERT(expr->dataNUL == NULL);
	expr->ty = ty;
	if(TY_isUnbox(ty)) {
		expr->build = TEXPR_NCONST;
		expr->ndata = N_toint(o);
	}
	else {
		expr->build = TEXPR_CONST;
		KINITv(expr->dataNUL, o);
	}
	return expr;
}

#define new_NConstValue(T, D)  Expr_setNConstValue(_ctx, NULL, T, D)
#define kExpr_setNConstValue(EXPR, T, D)  Expr_setNConstValue(_ctx, EXPR, T, D)

static kExpr* Expr_setNConstValue(CTX, kExpr *expr, ktype_t ty, uintptr_t ndata)
{
	if(expr == NULL) {
		expr = new_(Expr, 0);
	}
	DBG_ASSERT(expr->dataNUL == NULL);
	expr->build = TEXPR_NCONST;
	expr->ndata = ndata;
	expr->ty = ty;
	return expr;
}

#define new_Variable(B, T, I, I2, G)         Expr_setVariable(_ctx, NULL, B, T, I, I2, G)
#define kExpr_setVariable(E, B, T, I, I2, G) Expr_setVariable(_ctx, E, B, T, I, I2, G)
static kExpr *Expr_setVariable(CTX, kExpr *expr, kexpr_t build, ktype_t ty, int index, int xindex, kGamma *gma)
{
	if(expr == NULL) {
		expr = new_(Expr, 0);
	}
	expr->build = build;
	expr->ty = ty;
	expr->index = index;
	expr->xindex = xindex;
	if(build == TEXPR_BLOCKLOCAL_) {
		kArray_add(gma->genv->lvarlst, expr);
	}
	return expr;
}

/* --------------- */
/* Stmt */

static void Stmt_init(CTX, kRawPtr *o, void *conf)
{
	kStmt *stmt = (kStmt*)o;
	stmt->uline      =   (kline_t)conf;
	stmt->syn = NULL;
	stmt->parentNULL = NULL;
}

static void Stmt_reftrace(CTX, kRawPtr *o)
{
	kStmt *stmt = (kStmt*)o;
	BEGIN_REFTRACE(1);
	KREFTRACEn(stmt->parentNULL);
	END_REFTRACE();
}

static const KSTRUCT_DEF StmtDef = {
	STRUCTNAME(Stmt),
	.init = Stmt_init,
	.reftrace = Stmt_reftrace,
};

static void _dumpToken(CTX, void *arg, kprodata_t *d)
{
	if((d->key & OBJECT_MASK) == OBJECT_MASK) {
		keyword_t key = ~OBJECT_MASK & d->key;
		fprintf(stdout, "key='%s': ", T_kw(key));
		if(IS_Token(d->oval)) {
			dumpToken(_ctx, (kToken*)d->oval);
		} else if (IS_Expr(d->oval)) {
			dumpExpr(_ctx, 0, 0, (kExpr *) d->oval);
		}
	}
}

static void dumpStmtKeyValue(CTX, kStmt *stmt)
{
	kpromap_each(_ctx, stmt->h.proto, NULL, _dumpToken);
}

static void dumpStmt(CTX, kStmt *stmt)
{
	if(stmt->syn == NULL) {
		fprintf(stdout, "STMT (DONE)\n");
	}
	else {
		fprintf(stdout, "STMT %s {\n", stmt->syn->token);
		dumpStmtKeyValue(_ctx, stmt);
		fprintf(stdout, "\n}\n");
	}
	fflush(stdout);
}

#define kStmt_done(STMT)  Stmt_done(_ctx, STMT)
static void Stmt_done(CTX, kStmt *stmt)
{
	DBG_P("DONE: STMT='%s'", stmt->syn->token);
	stmt->syn = NULL;
}

#define kStmt_toERR(STMT, ENO)  Stmt_toERR(_ctx, STMT, ENO)
static void Stmt_toERR(CTX, kStmt *stmt, int eno)
{
	stmt->syn = SYN_ERR;
	stmt->build = TSTMT_ERR;
	kObject_setObject(stmt, 0, kstrerror(eno));
}

#define AKEY(T)   T, (sizeof(T)-1)

typedef struct flagop_t {
	const char *key;
	size_t keysize;
	uintptr_t flag;
} flagop_t ;

static uintptr_t Stmt_flag(CTX, kStmt *stmt, flagop_t *fop, uintptr_t flag)
{
	while(fop->key != NULL) {
		keyword_t kw = keyword(_ctx, fop->key, fop->keysize, FN_NONAME);
		if(kw != FN_NONAME) {
			kObject *op = kObject_getObjectNULL(stmt, kw);
			if(op != NULL) {
				DBG_P("found %s", fop->key);
				flag |= fop->flag;
			}
		}
		fop++;
	}
	return flag;
}

#define kStmt_is(STMT, KW) Stmt_is(_ctx, STMT, KW)

static kbool_t Stmt_is(CTX, kStmt *stmt, keyword_t kw)
{
	return (kObject_getObjectNULL(stmt, kw) != NULL);
}

//#define kStmt_expr(STMT, KW, DEF)  Stmt_expr(_ctx, STMT, KW, DEF)
//static kExpr* Stmt_expr(CTX, kStmt *stmt, keyword_t kw, kExpr *def)
//{
//	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, kw);
//	if(expr != NULL && IS_Expr(expr)) {
//		return expr;
//	}
//	return def;
//}

#define kStmt_text(STMT, KW, DEF) Stmt_text(_ctx, STMT, KW, DEF)
static const char* Stmt_text(CTX, kStmt *stmt, keyword_t kw, const char *def)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, kw);
	if(expr != NULL) {
		if(IS_Expr(expr) && Expr_isTerm(expr)) {
			return S_text(expr->tkNUL->text);
		}
		else if(IS_Token(expr)) {
			kToken *tk = (kToken*)expr;
			if(IS_String(tk->text)) return S_text(tk->text);
		}
	}
	return def;
}

//#define kStmt_block(STMT, KW, DEF)  Stmt_block(_ctx, STMT, KW, DEF)
//static kBlock* Stmt_block(CTX, kStmt *stmt, keyword_t kw, kBlock *def)
//{
//	kBlock *bk = (kBlock*)kObject_getObjectNULL(stmt, kw);
//	if(bk != NULL && IS_Block(bk)) {
//		return bk;
//	}
//	return def;
//}

/* --------------- */
/* Block */

static void Block_init(CTX, kRawPtr *o, void *conf)
{
	kBlock *bk = (kBlock*)o;
	kLingo *lgo = (conf != NULL) ? (kLingo*)conf : kevalshare->rootlgo;
	bk->parentNULL = NULL;
	KINITv(bk->lgo, lgo);
	KINITv(bk->blockS, new_(Array, 0));
	KINITv(bk->esp, new_(Expr, 0));
}

static void Block_reftrace(CTX, kRawPtr *o)
{
	kBlock *bk = (kBlock*)o;
	BEGIN_REFTRACE(4);
	KREFTRACEv(bk->lgo);
	KREFTRACEv(bk->blockS);
	KREFTRACEv(bk->esp);
	KREFTRACEn(bk->parentNULL);
	END_REFTRACE();
}

static const KSTRUCT_DEF BlockDef = {
	STRUCTNAME(Block),
	.init = Block_init,
	.reftrace = Block_reftrace,
};


/* --------------- */
/* Block */

static void Gamma_init(CTX, kRawPtr *o, void *conf)
{
	kGamma *gma = (kGamma*)o;
	gma->genv = NULL;
}

static const KSTRUCT_DEF GammaDef = {
	STRUCTNAME(Gamma),
	.init = Gamma_init,
};
