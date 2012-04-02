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

#define PACKSUGAR    .packid = 1, .packdom = 1

/* --------------- */
/* KonohaSpace */

static void KonohaSpace_init(CTX, kRawPtr *o, void *conf)
{
	kKonohaSpace *ks = (kKonohaSpace*)o;
	bzero(&ks->parentNULL, sizeof(kKonohaSpace) - sizeof(kObjectHeader));
	ks->parentNULL = conf;
	ks->static_cid = CLASS_unknown;
}

static void syntax_reftrace(CTX, kmape_t *p)
{
	ksyntax_t *syn = (ksyntax_t*)p->uvalue;
	BEGIN_REFTRACE(5);
	KREFTRACEn(syn->syntaxRule);
	KREFTRACEn(syn->StmtAdd);
	KREFTRACEn(syn->TopStmtTyCheck);
	KREFTRACEn(syn->StmtTyCheck);
	KREFTRACEn(syn->ExprTyCheck);
	END_REFTRACE();
}

//static void symtbl_reftrace(CTX, kmape_t *p)
//{
//	BEGIN_REFTRACE(2);
//	KREFTRACEn(p->skey);
//	KREFTRACEn(p->ovalue);
//	END_REFTRACE();
//}

static void KonohaSpace_reftrace(CTX, kRawPtr *o)
{
	kKonohaSpace *ks = (kKonohaSpace*)o;
	if(ks->syntaxMapNN != NULL) {
		kmap_reftrace(ks->syntaxMapNN, syntax_reftrace);
	}
//	if(ks->symtblMapSO != NULL) {
//		kmap_reftrace(ks->symtblMapSO, symtbl_reftrace);
//	}
	BEGIN_REFTRACE(ks->cl.size + 3);
	if(ks->cl.size > 0) {
		size_t i;
		for(i = 0; i < ks->cl.size; i++) {
			if(FN_isBOXED(ks->cl.keyvals[i].key)) {
				KREFTRACEv(ks->cl.keyvals[i].value);
			}
		}
	}
	KREFTRACEn(ks->parentNULL);
	KREFTRACEn(ks->script);
	KREFTRACEn(ks->methodsNULL);
	END_REFTRACE();
}

static void syntax_free(CTX, void *p)
{
	KNH_FREE(p, sizeof(ksyntax_t));
}

static void KonohaSpace_free(CTX, kRawPtr *o)
{
	kKonohaSpace *ks = (kKonohaSpace*)o;
	if(ks->syntaxMapNN != NULL) {
		kmap_free(ks->syntaxMapNN, syntax_free);
	}
	if(ks->cl.size > 0) {
		KARRAY_FREE(ks->cl, keyvals_t);
	}
}

static KCLASSDEF KonohaSpaceDef = {
	STRUCTNAME(KonohaSpace), PACKSUGAR,
	.init = KonohaSpace_init,
	.reftrace = KonohaSpace_reftrace,
	.free = KonohaSpace_free,
};

static keyvals_t* KonohaSpace_getConstNULL(CTX, kKonohaSpace *ks, ksymbol_t ukey)
{
	size_t min = 0, max = ks->cl.size;
	while(min < max) {
		size_t p = (max + min) / 2;
		ksymbol_t key = FN_UNBOX(ks->cl.keyvals[p].key);
		if(key == ukey) return ks->cl.keyvals + p;
		if(key < ukey) {
			min = p + 1;
		}
		else {
			max = p;
		}
	}
	return NULL;
}

static int comprKeyVal(const void *a, const void *b)
{
	int akey = FN_UNBOX(((keyvals_t*)a)->key);
	int bkey = FN_UNBOX(((keyvals_t*)b)->key);
	return akey - bkey;
}

static void KonohaSpace_mergeConstData(CTX, kKonohaSpace *ks, keyvals_t *kvs, size_t nitems, kline_t pline)
{
	if(nitems > 0) {
		size_t s = ks->cl.size;
		if(s == 0) {
			KARRAY_INIT(ks->cl, nitems, keyvals_t);
		}
		else {
			KARRAY_RESIZE(ks->cl, s + nitems, keyvals_t);
		}
		memcpy(ks->cl.keyvals + s, kvs, nitems * sizeof(keyvals_t));
		ks->cl.size = s + nitems;
		qsort(ks->cl.keyvals, ks->cl.size, sizeof(keyvals_t), comprKeyVal);
		int i, pkey = FN_UNBOX(ks->cl.keyvals[0].key);
		for(i=1; i < ks->cl.size; i++) {
			int key = FN_UNBOX(ks->cl.keyvals[i].key);
			if(pkey == key) {

			}
			pkey = key;
		}
	}
}

static void KonohaSpace_loadConstData(CTX, kKonohaSpace *ks, const char **d, kline_t pline)
{
	INIT_GCSTACK();
	keyvals_t kv;
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	while(d[0] != NULL) {
		DBG_P("key='%s'", d[0]);
		kv.key = kusymbol(d[0], 255) | FN_BOXED;
		kv.ty  = (ktype_t)(uintptr_t)d[1];
		if(kv.ty == TY_TEXT) {
			kv.ty = TY_String;
			kv.svalue = new_kString(d[2], strlen(d[2]), 0);
			PUSH_GCSTACK(kv.value);
		}
		else if(TY_isUnbox(kv.ty)) {
			kv.key = FN_UNBOX(kv.key);
			kv.uvalue = (uintptr_t)d[2];
		}
		else {
			kv.value = (kObject*)d[2];
		}
		kwb_write(&wb, (const char*)(&kv), sizeof(keyvals_t));
		d += 3;
	}
	KonohaSpace_mergeConstData(_ctx, ks, (keyvals_t*)kwb_top(&wb, 0), kwb_size(&wb) / sizeof(keyvals_t), pline);
	kwb_free(&wb);
	RESET_GCSTACK();
}

static ksyntax_t* KonohaSpace_syntax(CTX, kKonohaSpace *ks0, keyword_t keyid, int isnew)
{
	kKonohaSpace *ks = ks0;
	uintptr_t hcode = keyid;
	ksyntax_t *parent = NULL;
	while(ks != NULL) {
		if(ks->syntaxMapNN != NULL) {
			kmape_t *e = kmap_get(ks->syntaxMapNN, hcode);
			while(e != NULL) {
				if(e->hcode == hcode) {
					parent = (ksyntax_t*)e->uvalue;
					if(isnew && ks0 != ks) goto L_NEW;
					return parent;
				}
				e = e->next;
			}
		}
		ks = ks->parentNULL;
	}
	L_NEW:;
	if(isnew == 1) {
		if(ks0->syntaxMapNN == NULL) {
			ks0->syntaxMapNN = kmap_init(0);
		}
		kmape_t *e = kmap_newentry(ks0->syntaxMapNN, hcode);
		kmap_add(ks0->syntaxMapNN, e);
		ksyntax_t *syn = (ksyntax_t*)KNH_ZMALLOC(sizeof(ksyntax_t));
		e->uvalue = (uintptr_t)syn;
		if(parent != NULL) {
			memcpy(syn, parent, sizeof(ksyntax_t));
		}
		else {
			syn->keyid = keyid;
			syn->ty  = CLASS_unknown;
			syn->op1 = 0; /*MN_NONAME;*/
			syn->op2 = 0; /*MN_NONAME;*/
		}
		//syn->parent = parent;
		return syn;
	}
	return NULL;
}

static ksymbol_t keyword(CTX, const char *name, size_t len, ksymbol_t def);
static void parseSyntaxRule(CTX, const char *rule, kline_t pline, kArray *a);

static void KonohaSpace_defineSyntax(CTX, kKonohaSpace *ks, ksyntaxdef_t *syndef)
{
	knh_Fmethod pStmtAdd = NULL, pStmtTyCheck = NULL, pExprTyCheck = NULL;
	kMethod *mStmtAdd = NULL, *mStmtTyCheck = NULL, *mExprTyCheck = NULL;
	ksyntax_t *syn_expr = KonohaSpace_syntax(_ctx, ks, 1, 0);
	while(syndef->name != NULL) {
		keyword_t keyid = (syndef->keyid != 0) ? syndef->keyid : keyword(_ctx, syndef->name, syndef->namelen, FN_NEWID);
		ksyntax_t* syn = KonohaSpace_syntax(_ctx, ks, keyid, 1);
		if(keyid == 1 && syn_expr == NULL) {
			syn_expr = syn;
		}
		syn->token = syndef->name;
		if(syndef->type != 0) {
			syn->ty = syndef->type;
		}
		if(syndef->rule != NULL) {
			KINITv(syn->syntaxRule, new_(Array, 0));
			parseSyntaxRule(_ctx, syndef->rule, 0, syn->syntaxRule);
		}
		if(syndef->op1 != NULL) {
			if (syndef->op1[0] == '*') {
				syn->op1 = MN_NONAME;  // TODO
			} else {
				syn->op1 = ksymbol(syndef->op1, 127, FN_NEWID, SYMPOL_METHOD);
			}
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
			if(syndef->StmtAdd != pStmtAdd) {
				pStmtAdd = syndef->StmtAdd;
				mStmtAdd = new_kMethod(0, 0, 0, NULL, pStmtAdd);
			}
			KINITv(syn->StmtAdd, mStmtAdd);
		}
		if(syndef->TopStmtTyCheck != NULL) {
			if(syndef->TopStmtTyCheck != pStmtTyCheck) {
				pStmtTyCheck = syndef->TopStmtTyCheck;
				mStmtTyCheck = new_kMethod(0, 0, 0, NULL, pStmtTyCheck);
			}
			KINITv(syn->TopStmtTyCheck, mStmtTyCheck);
		}
		if(syndef->StmtTyCheck != NULL) {
			if(syndef->StmtTyCheck != pStmtTyCheck) {
				pStmtTyCheck = syndef->StmtTyCheck;
				mStmtTyCheck = new_kMethod(0, 0, 0, NULL, pStmtTyCheck);
			}
			KINITv(syn->StmtTyCheck, mStmtTyCheck);
		}
		if(syndef->ExprTyCheck != NULL) {
			if(syndef->ExprTyCheck != pExprTyCheck) {
				pExprTyCheck = syndef->ExprTyCheck;
				mExprTyCheck = new_kMethod(0, 0, 0, NULL, pExprTyCheck);
			}
			KINITv(syn->ExprTyCheck, mExprTyCheck);
			if(syn->syntaxRule == NULL) {
				KINITv(syn->syntaxRule, syn_expr->syntaxRule);
			}
			if(syn->StmtTyCheck == NULL) {
				KINITv(syn->StmtTyCheck, syn_expr->StmtTyCheck);
			}
		}
		DBG_ASSERT(syn == KonohaSpace_syntax(_ctx, ks, keyid, 0));
		syndef++;
	}
	// update
	kevalshare_t *base = kevalshare;
	base->syn_err  = KonohaSpace_syntax(_ctx, base->rootks, 0, 0);
	base->syn_expr = KonohaSpace_syntax(_ctx, base->rootks, 1, 0);
	base->syn_call = KonohaSpace_syntax(_ctx, base->rootks, 1, 0);
	base->syn_invoke = KonohaSpace_syntax(_ctx, base->rootks, KW_("$name"), 0);
	base->syn_params = KonohaSpace_syntax(_ctx, base->rootks, KW_("$params"), 0);
	base->syn_return = KonohaSpace_syntax(_ctx, base->rootks, KW_("return"), 0);
//	base->syn_break  = KonohaSpace_syntax(_ctx, base->rootks, KW_("break"), 0);
	base->syn_typedecl = KonohaSpace_syntax(_ctx, base->rootks, KW_(":"), 0);
	base->syn_comma    = KonohaSpace_syntax(_ctx, base->rootks, KW_(","), 0);
	base->syn_let      = KonohaSpace_syntax(_ctx, base->rootks, KW_("="), 0);
}

// KonohaSpace

static kObject *KonohaSpace_getSymbolValueNULL(CTX, kKonohaSpace *ks, const char *key, size_t klen)
{
	if(key[0] == 'K' && (key[1] == 0 || strcmp("Konoha", key) == 0)) {
		return (kObject*)ks;
	}
	return NULL;
}

//static void casehash_add(CTX, kmap_t *kmp, kString *skey, uintptr_t uvalue)
//{
//	uintptr_t hcode = casehash(S_text(skey), S_size(skey));
//	kmape_t *e = kmap_newentry(kmp, hcode);
//	KINITv(e->skey, skey);
//	e->uvalue = uvalue;
//	kmap_add(kmp, e);
//}

static uintptr_t casehash_getuint(CTX, kmap_t *kmp, const char *name, size_t len, uintptr_t def)
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

static kcid_t KonohaSpace_getcid(CTX, kKonohaSpace *ks, const char *name, size_t len, kcid_t def)
{
	kcid_t cid;
	//while(nsN != NULL) {
	//	nsN = nsN->parentNULL;
	//}
	cid = (kcid_t)casehash_getuint(_ctx, _ctx->share->classnameMapNN, name, len, def);
	return (cid != CLASS_unknown && CT_(cid)->packdom == 0) ? cid : def;
}

/* KonohaSpace/Class/Method */
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

#define kKonohaSpace_getMethodNULL(ns, cid, mn)     KonohaSpace_getMethodNULL(_ctx, ns, cid, mn)
#define kKonohaSpace_getStaticMethodNULL(ns, mn)   KonohaSpace_getStaticMethodNULL(_ctx, ns, mn)
static kMethod* KonohaSpace_getMethodNULL(CTX, kKonohaSpace *ks, kcid_t cid, kmethodn_t mn)
{
	while(ks != NULL) {
		if(ks->methodsNULL != NULL) {
			size_t i;
			kArray *methods = ks->methodsNULL;
			for(i = 0; i < kArray_size(methods); i++) {
				kMethod *mtd = methods->methods[i];
				if(mtd->cid == cid && mtd->mn == mn) {
					return mtd;
				}
			}
		}
		ks = ks->parentNULL;
	}
	return CT_findMethodNULL(_ctx, CT_(cid), mn);
}

static kMethod* KonohaSpace_getStaticMethodNULL(CTX, kKonohaSpace *ks, kmethodn_t mn)
{
	while(ks != NULL) {
		if(ks->static_cid != CLASS_unknown) {
			kMethod *mtd = kKonohaSpace_getMethodNULL(ks, ks->static_cid, mn);
			if(mtd != NULL && kMethod_isStatic(mtd)) {
				return mtd;
			}
		}
		ks = ks->parentNULL;
	}
	return NULL;
}

#define kKonohaSpace_getCastMethodNULL(ns, cid, tcid)     KonohaSpace_getCastMethodNULL(_ctx, ns, cid, tcid)
static kMethod* KonohaSpace_getCastMethodNULL(CTX, kKonohaSpace *ks, kcid_t cid, kcid_t tcid)
{
	kMethod *mtd = KonohaSpace_getMethodNULL(_ctx, ks, cid, MN_to(tcid));
	if(mtd != NULL) {
		mtd = KonohaSpace_getMethodNULL(_ctx, ks, cid, MN_as(tcid));
	}
	return mtd;
}

#define kKonohaSpace_addMethod(NS,MTD,UL)  KonohaSpace_addMethod(_ctx, NS, MTD, UL)

static kbool_t KonohaSpace_addMethod(CTX, kKonohaSpace *ks, kMethod *mtd, kline_t pline)
{
	if(pline != 0) {
		kMethod *mtdOLD = KonohaSpace_getMethodNULL(_ctx, ks, mtd->cid, mtd->mn);
		if(mtdOLD != NULL) {
			char mbuf[128];
			kreportf(ERR_, pline, "method %s.%s is already defined", T_cid(mtd->cid), T_mn(mbuf, mtd->mn));
			return 0;
		}
	}
	if(kMethod_isPublic(mtd)) {
		const kclass_t *ct = CT_(mtd->cid);
		kArray_add(ct->methods, mtd);
	}
	else {
		if(ks->methodsNULL == NULL) {
			KINITv(ks->methodsNULL, new_(Array, 8));
		}
		kArray_add(ks->methodsNULL, mtd);
	}
	return 1;
}

static void KonohaSpace_loadMethodData(CTX, kKonohaSpace *ks, intptr_t *data)
{
	intptr_t *d = data;
	kParam *prev = NULL;
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
		if(prev != NULL) {
			if (prev->rtype == rtype && prev->psize == psize) {
				for(i = 0; i < psize; i++) {
					if(p[i].ty != prev->p[i].ty || p[i].fn != prev->p[i].fn) {
						prev = NULL;
						break;
					}
				}
			}
			else prev = NULL;
		}
		kParam *pa = (prev == NULL) ? new_kParam(rtype, psize, p) : prev;
		kMethod *mtd = new_kMethod(flag, cid, mn, pa, f);
		if(ks == NULL || kMethod_isPublic(mtd)) {
			kArray_add(CT_(cid)->methods, mtd);
		} else {
			if (ks->methodsNULL == NULL) {
				KINITv(ks->methodsNULL, new_(Array, 8));
			}
			kArray_add(ks->methodsNULL, mtd);
		}
		prev = pa;
	}
}

#define kKonohaSpace_loadGlueFunc(NS, F, OPT, UL)  KonohaSpace_loadGlueFunc(_ctx, NS, F, OPT, UL)

static knh_Fmethod KonohaSpace_loadGlueFunc(CTX, kKonohaSpace *ks, const char *funcname, int DOPTION, kline_t pline)
{
	void *f = NULL;
	if(ks->gluehdr != NULL) {
		char namebuf[128];
		snprintf(namebuf, sizeof(namebuf), "D%s", funcname);
		if(DOPTION) {
			f = dlsym(ks->gluehdr, (const char*)namebuf);
		}
		if(f == NULL) {
			f = dlsym(ks->gluehdr, (const char*)namebuf+1);
		}
		kreportf(WARN_, pline, "glue method function is not found: %s", namebuf + 1);
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

static KCLASSDEF TokenDef = {
	STRUCTNAME(Token), PACKSUGAR,
	.init = Token_init,
	.reftrace = Token_reftrace,
};

#define kToken_s(tk) kToken_s_(_ctx, tk)
static const char *kToken_s_(CTX, kToken *tk)
{
	switch((int)tk->tt) {
	case AST_PARENTHESIS: return "(... )";
	case AST_BRACE: return "{... }";
	case AST_BRANCET: return "[... ]";
	default:  return S_text(tk->text);
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
	if(konoha_debug) {
		if(tk->tt == TK_MN) {
			char mbuf[128];
			DUMP_P("%s %d+%d: %s\n", T_tt(tk->tt), (short)tk->uline, tk->lpos, T_mn(mbuf, tk->mn));
		}
		else if((int)tk->tt <= TK_TYPE) {
			DUMP_P("%s %d+%d: '%s'\n", T_tt(tk->tt), (short)tk->uline, tk->lpos, S_text(tk->text));
		}
		else {
			DUMP_P("%s\n", T_tt(tk->tt));
		}
	}
}

static void dumpIndent(int nest)
{
	if(konoha_debug) {
		int i;
		for(i = 0; i < nest; i++) {
			DUMP_P("  ");
		}
	}
}

static void dumpTokenArray(CTX, int nest, kArray *a, int s, int e)
{
	if(konoha_debug) {
		if(nest == 0) DUMP_P("\n");
		while(s < e) {
			kToken *tk = a->tts[s];
			dumpIndent(nest);
			if(AST_PARENTHESIS <= (int)tk->tt && (int)tk->tt <= AST_TYPE) {
				DUMP_P("%c\n", tk->topch);
				dumpTokenArray(_ctx, nest+1, tk->sub, 0, kArray_size(tk->sub));
				dumpIndent(nest);
				DUMP_P("%c\n", tk->closech);
			}
			else {
				DUMP_P("TK(%d) ", s);
				dumpToken(_ctx, tk);
			}
			s++;
		}
		if(nest == 0) DUMP_P("====\n");
	}
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

static KCLASSDEF ExprDef = {
	STRUCTNAME(Expr), PACKSUGAR,
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
	if(konoha_debug) {
		if(nest == 0) DUMP_P("\n");
		dumpIndent(nest);
		if(Expr_isTerm(expr)) {
			DUMP_P("[%d] T: %s %s", n, T_tt(expr->tkNUL->tt), kToken_s(expr->tkNUL));
			if(expr->ty != TY_var) {

			}
			DUMP_P("\n");
		}
		else {
			int i;
			DUMP_P("[%d] C: build=%d, size=%ld", n, expr->build, kArray_size(expr->consNUL));
			if(expr->ty != TY_var) {

			}
			DUMP_P("\n");
			for(i=0; i < kArray_size(expr->consNUL); i++) {
				kObject *o = expr->consNUL->list[i];
				if(O_ct(o) == CT_Expr) {
					dumpExpr(_ctx, i, nest+1, (kExpr*)o);
				}
				else {
					dumpIndent(nest+1);
					if(O_ct(o) == CT_Token) {
						kToken *tk = (kToken*)o;
						DUMP_P("[%d] O: %s ", i, S_text(o->h.ct->name));
						dumpToken(_ctx, tk);
					}
					else if(o == K_NULL) {
						DUMP_P("[%d] O: null\n", i);
					}
					else {
						DUMP_P("[%d] O: %s\n", i, S_text(o->h.ct->name));
					}
				}
			}
		}
	}
}

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
		DBG_P("index=%d, expr %p", kArray_size(gma->genv->lvarlst), expr);
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

static KCLASSDEF StmtDef = {
	STRUCTNAME(Stmt), PACKSUGAR,
	.init = Stmt_init,
	.reftrace = Stmt_reftrace,
};

static void _dumpToken(CTX, void *arg, kprodata_t *d)
{
	if((d->key & FN_BOXED) == FN_BOXED) {
		keyword_t key = ~FN_BOXED & d->key;
		DUMP_P("key='%s': ", T_kw(key));
		if(IS_Token(d->oval)) {
			dumpToken(_ctx, (kToken*)d->oval);
		} else if (IS_Expr(d->oval)) {
			dumpExpr(_ctx, 0, 0, (kExpr *) d->oval);
		}
	}
}

static void dumpStmtKeyValue(CTX, kStmt *stmt)
{
	if(konoha_debug) {
		kpromap_each(_ctx, stmt->h.proto, NULL, _dumpToken);
	}
}

static void dumpStmt(CTX, kStmt *stmt)
{
	if(konoha_debug) {
		if(stmt->syn == NULL) {
			DUMP_P("STMT (DONE)\n");
		}
		else {
			DUMP_P("STMT %s {\n", stmt->syn->token);
			dumpStmtKeyValue(_ctx, stmt);
			DUMP_P("\n}\n");
		}
		fflush(stdout);
	}
}

#define kStmt_ks(STMT)   Stmt_ks(_ctx, STMT)
static kKonohaSpace *Stmt_ks(CTX, kStmt *stmt)
{
	return stmt->parentNULL != NULL ? stmt->parentNULL->ks : kevalshare->rootks;
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

static kToken* Stmt_token(CTX, kStmt *stmt, keyword_t kw, kToken *def)
{
	kToken *tk = (kToken*)kObject_getObjectNULL(stmt, kw);
	if(tk != NULL && IS_Token(tk)) {
		return tk;
	}
	return def;
}

static kExpr* Stmt_expr(CTX, kStmt *stmt, keyword_t kw, kExpr *def)
{
	kExpr *expr = (kExpr*)kObject_getObjectNULL(stmt, kw);
	if(expr != NULL && IS_Expr(expr)) {
		return expr;
	}
	return def;
}

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

static kbool_t Token_toBRACE(CTX, kToken *tk);
static kBlock *new_Block(CTX, kArray *tls, int s, int e, kKonohaSpace* ks);
static kBlock* Stmt_block(CTX, kStmt *stmt, keyword_t kw, kBlock *def)
{
	kBlock *bk = (kBlock*)kObject_getObjectNULL(stmt, kw);
	if(bk != NULL) {
		if(IS_Token(bk)) {
			kToken *tk = (kToken*)bk;
			if (tk->tt == TK_CODE) {
				Token_toBRACE(_ctx, tk);
			}
			if (tk->tt == AST_BRACE) {
				bk = new_Block(_ctx, tk->sub, 0, kArray_size(tk->sub), kStmt_ks(stmt));
				kObject_setObject(stmt, kw, bk);
			}
		}
		if(IS_Block(bk)) return bk;
	}
	return def;
}

/* --------------- */
/* Block */

static void Block_init(CTX, kRawPtr *o, void *conf)
{
	kBlock *bk = (kBlock*)o;
	kKonohaSpace *ks = (conf != NULL) ? (kKonohaSpace*)conf : kevalshare->rootks;
	bk->parentNULL = NULL;
	KINITv(bk->ks, ks);
	KINITv(bk->blockS, new_(Array, 0));
	KINITv(bk->esp, new_(Expr, 0));
}

static void Block_reftrace(CTX, kRawPtr *o)
{
	kBlock *bk = (kBlock*)o;
	BEGIN_REFTRACE(4);
	KREFTRACEv(bk->ks);
	KREFTRACEv(bk->blockS);
	KREFTRACEv(bk->esp);
	KREFTRACEn(bk->parentNULL);
	END_REFTRACE();
}

static KCLASSDEF BlockDef = {
	STRUCTNAME(Block), PACKSUGAR,
	.init = Block_init,
	.reftrace = Block_reftrace,
};

static void Block_insertAfter(CTX, kBlock *bk, kStmt *target, kStmt *stmt)
{
	DBG_ASSERT(stmt->parentNULL == NULL);
	KSETv(stmt->parentNULL, bk);
	size_t i;
	for(i = 0; i < kArray_size(bk->blockS); i++) {
		if(bk->blockS->stmts[i] == target) {
			kArray_insert(bk->blockS, i+1, stmt);
			return;
		}
	}
	DBG_ABORT("target was not found!!");
}

/* --------------- */
/* Block */

static void Gamma_init(CTX, kRawPtr *o, void *conf)
{
	kGamma *gma = (kGamma*)o;
	gma->genv = NULL;
}

static KCLASSDEF GammaDef = {
	STRUCTNAME(Gamma), PACKSUGAR,
	.init = Gamma_init,
};
