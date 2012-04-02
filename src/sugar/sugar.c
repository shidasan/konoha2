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
 * (2) Konoha Non-Disclosure License 1.
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

#define USING_SUGAR_AS_BUILTIN 1
#include<konoha2/sugar.h>

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

// MOD_CODE
void MODCODE_genCode(CTX, kMethod *mtd, kBlock *bk);

//
static const char *Pkeyword_(CTX, keyword_t keyid);
static kString *Skeyword_(CTX, keyword_t keyid);
static void Kraise(CTX, int isContinue);

#include "perror.h"
#include "struct.h"
#include "token.h"
#include "ast.h"
#include "tycheck.h"

#define TOKEN(T)  .name = T, .namelen = (sizeof(T)-1)
#define _EXPR     .rule ="$expr"

static void defineDefaultSyntax(CTX, kKonohaSpace *ks)
{
	ksyntaxdef_t SYNTAX[] = {
		{ TOKEN("$ERR"), .StmtTyCheck = StmtTyCheck_err },
		{ TOKEN("$expr"),  .rule ="$expr", .StmtAdd = StmtAdd_expr, .TopStmtTyCheck = StmtTyCheck_expr, .StmtTyCheck = StmtTyCheck_expr, .ExprTyCheck = ExprTyCheck_call, },
		{ TOKEN("$block"), .StmtAdd = StmtAdd_block, },
		{ TOKEN("$type"),  .StmtAdd = StmtAdd_type, },
		{ TOKEN("$cname"), .StmtAdd = StmtAdd_cname, },
		{ TOKEN("$name"),  .StmtAdd = StmtAdd_name, .ExprTyCheck = ExprTyCheck_invoke, },
		{ TOKEN("$params"),  .StmtAdd = StmtAdd_params, .TopStmtTyCheck = StmtTyCheck_declParams, },
		{ TOKEN("."), .op2 = "*", .priority_op2 = 16, .right = 1, .ExprTyCheck = ExprTyCheck_getter },
		{ TOKEN("/"), .op2 = "opDIV", .priority_op2 = 32,  .right = 1, .ExprTyCheck = ExprTyCheck_call  },
		{ TOKEN("%"), .op2 = "opMOD", .priority_op2 = 32,  .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN("*"), .op2 = "opMUL", .priority_op2 = 32,  .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN("+"), .op1 = "opPLUS", .op2 = "opADD", .priority_op2 = 64, .right = 1, .ExprTyCheck = ExprTyCheck_call},
		{ TOKEN("-"), .op1 = "opMINUS", .op2 = "opSUB", .priority_op2 = 64, .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN("<"), .op2 = "opLT", .priority_op2 = 256, .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN("<="), .op2 = "opLTE", .priority_op2 = 256, .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN(">"), .op2 = "opGT", .priority_op2 = 256, .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN(">="), .op2 = "opGTE", .priority_op2 = 256, .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN("=="), .op2 = "opEQ", .priority_op2 = 512, .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN("!="), .op2 = "opNEQ", .priority_op2 = 512, .right = 1, .ExprTyCheck = ExprTyCheck_call },
		{ TOKEN("&&"), .op2 = "*", .priority_op2 = 1024, .right = 1, },
		{ TOKEN("||"), .op2 = "*", .priority_op2 = 2048, .right = 1, },
		{ TOKEN("!"),  .op1 = "opNOT", },
		{ TOKEN(":"), .priority_op2 = 3072, },
		{ TOKEN("="), .op2 = "*", .priority_op2 = 4096, },
		{ TOKEN(","), .op2 = "*", .priority_op2 = 8192, },
		{ TOKEN("void"), .type = TY_void, .rule ="$type [$cname '.'] $name $params [$block]", .TopStmtTyCheck = StmtTyCheck_declMethod},
		{ TOKEN("var"),  .type = TY_var, .rule ="$type $expr", .StmtTyCheck = StmtTyCheck_declType},
		{ TOKEN("boolean"), .type = TY_Boolean, },
		{ TOKEN("int"),     .type = TY_Int, },
		//	{ TOKEN("dynamic"), .type = TY_dynamic, },
		{ TOKEN("null"), .ExprTyCheck = TokenTyCheck_NULL,},
		{ TOKEN("true"),  .ExprTyCheck = TokenTyCheck_TRUE,},
		{ TOKEN("false"), .ExprTyCheck = TokenTyCheck_FALSE,},
		{ TOKEN("if"), .rule ="'if' '(' $expr ')' then: $block ['else' else: $block]", .TopStmtTyCheck = StmtTyCheck_if, .StmtTyCheck = StmtTyCheck_if, },
		{ TOKEN("else"), .rule = "'else' else: $block" },
//		{ TOKEN("break"), .rule ="'break' [$expr]", .StmtTyCheck = StmtTyCheck_nop },
		{ TOKEN("return"), .rule ="'return' [$expr]", .StmtTyCheck = StmtTyCheck_return, },
		{ TOKEN("$SYMBOL"), .keyid = KW_TK(TK_SYMBOL), .ExprTyCheck = TokenTyCheck_SYMBOL, },
		{ TOKEN("$USYMBOL"), .keyid = KW_TK(TK_USYMBOL), .ExprTyCheck = TokenTyCheck_USYMBOL, },
		{ TOKEN("$TEXT"), .keyid = KW_TK(TK_TEXT), .ExprTyCheck = TokenTyCheck_TEXT, },
		{ TOKEN("$INT"), .keyid = KW_TK(TK_INT), .ExprTyCheck = TokenTyCheck_INT, },
		{ TOKEN("$TYPE"), .keyid = KW_TK(TK_TYPE), .ExprTyCheck = TokenTyCheck_TYPE, },
		{ TOKEN("$FLOAT"), .keyid = KW_TK(TK_FLOAT), .ExprTyCheck = TokenTyCheck_FLOAT, },
		{ .name = NULL, },
	};
	KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
}

/* ------------------------------------------------------------------------ */
/* kevalmod_t global functions */

static kstatus_t KonohaSpace_eval(CTX, kKonohaSpace *ks, const char *script, kline_t uline)
{
	kstatus_t result;
	kevalshare->h.setup(_ctx, (kmodshare_t*)kevalshare);
	{
		INIT_GCSTACK();
		kArray *tls = kevalmod->tokens;
		size_t pos = kArray_size(tls);
		ktokenize(_ctx, script, uline, _TOPLEVEL, tls);
		kBlock *bk = new_Block(_ctx, tls, pos, kArray_size(tls), ks);
		kArray_clear(tls, pos);
		result = Block_eval(_ctx, bk);
		RESET_GCSTACK();
	}
	return result;
}

kstatus_t MODEVAL_eval(CTX, const char *script, size_t len, kline_t uline)
{
	DBG_ASSERT(script[len] == 0);
	if(konoha_debug) {
		DUMP_P("\n>>>----\n'%s'\n------\n", script);
	}
	kevalshare->h.setup(_ctx, (kmodshare_t*)kevalshare);
	return KonohaSpace_eval(_ctx, kevalshare->rootks, script, uline);
}

/* ------------------------------------------------------------------------ */
/* [kevalmod] */

static void kevalmod_reftrace(CTX, struct kmod_t *baseh)
{
	kevalmod_t *base = (kevalmod_t*)baseh;
	BEGIN_REFTRACE(7);
	KREFTRACEv(base->tokens);
	KREFTRACEv(base->errors);
	KREFTRACEv(base->gma);
	KREFTRACEv(base->lvarlst);
	KREFTRACEv(base->singleBlock);
	KREFTRACEv(base->definedMethods);
	END_REFTRACE();
}
static void kevalmod_free(CTX, struct kmod_t *baseh)
{
	kevalmod_t *base = (kevalmod_t*)baseh;
	KARRAY_FREE(base->cwb, char);
	KNH_FREE(base->evaljmpbuf, sizeof(kjmpbuf_t));
	KNH_FREE(base, sizeof(kevalmod_t));
}

static void kevalshare_setup(CTX, struct kmodshare_t *def)
{
	if(_ctx->mod[MOD_EVAL] == NULL) {
		kevalmod_t *base = (kevalmod_t*)KNH_ZMALLOC(sizeof(kevalmod_t));
		base->h.reftrace = kevalmod_reftrace;
		base->h.free     = kevalmod_free;
		KINITv(base->tokens, new_(Array, K_PAGESIZE/sizeof(void*)));
		KINITv(base->errors, new_(Array, 8));
		KINITv(base->lvarlst, new_(Array, K_PAGESIZE/sizeof(void*)));
		KINITv(base->definedMethods, new_(Array, 8));

		KINITv(base->gma, new_(Gamma, NULL));
		KINITv(base->singleBlock, new_(Block, NULL));
		kArray_add(base->singleBlock->blockS, K_NULL);
		KARRAY_INIT(base->cwb, K_PAGESIZE, char);
		base->iseval = 0;
		base->evalty = TY_void;
		base->evalidx = 0;
		base->evaljmpbuf = (kjmpbuf_t*)KNH_ZMALLOC(sizeof(kjmpbuf_t));
		_ctx->mod[MOD_EVAL] = (kmod_t*)base;
	}
}

static void pack_reftrace(CTX, kmape_t *p)
{
	kpackage_t *pack = (kpackage_t*)p->uvalue;
	BEGIN_REFTRACE(2);
	KREFTRACEn(pack->name);
	KREFTRACEn(pack->ks);
	END_REFTRACE();
}

static void pack_free(CTX, void *p)
{
	KNH_FREE(p, sizeof(kpackage_t));
}

static void kevalshare_reftrace(CTX, struct kmodshare_t *baseh)
{
	kevalshare_t *base = (kevalshare_t*)baseh;
	kmap_reftrace(base->packageMapNO, pack_reftrace);
	BEGIN_REFTRACE(3);
	KREFTRACEv(base->keywordList);
	KREFTRACEv(base->packageList);
	KREFTRACEv(base->aBuffer);
	END_REFTRACE();
}

static void kevalshare_free(CTX, struct kmodshare_t *baseh)
{
	kevalshare_t *base = (kevalshare_t*)baseh;
	kmap_free(base->keywordMapNN, NULL);
	kmap_free(base->packageMapNO, pack_free);
	KNH_FREE(baseh, sizeof(kevalshare_t));
}

void MODEVAL_init(CTX, kcontext_t *ctx)
{
	kevalshare_t *base = (kevalshare_t*)KNH_ZMALLOC(sizeof(kevalshare_t));
	base->h.name     = "sugar";
	base->h.setup    = kevalshare_setup;
	base->h.reftrace = kevalshare_reftrace;
	base->h.free     = kevalshare_free;

	klib2_t *l = ctx->lib2;
	l->KKonohaSpace_getcid   = KonohaSpace_getcid;
	l->KloadMethodData = KonohaSpace_loadMethodData;
	l->Kraise = Kraise;

	KINITv(base->keywordList, new_(Array, 32));
	base->keywordMapNN = kmap_init(0);
	KINITv(base->packageList, new_(Array, 8));
	base->packageMapNO = kmap_init(0);

	ksetModule(MOD_EVAL, (kmodshare_t*)base, 0);
	base->cKonohaSpace = kaddClassDef(NULL, &KonohaSpaceDef, 0);
	base->cToken = kaddClassDef(NULL, &TokenDef, 0);
	base->cExpr  = kaddClassDef(NULL, &ExprDef, 0);
	base->cStmt  = kaddClassDef(NULL, &StmtDef, 0);
	base->cBlock = kaddClassDef(NULL, &BlockDef, 0);
	base->cGamma = kaddClassDef(NULL, &GammaDef, 0);

	KINITv(base->rootks, new_(KonohaSpace, NULL));
	KINITv(base->nullToken, new_(Token, NULL));
	kObject_setNullObject(base->nullToken, 1);
	KINITv(base->nullExpr, new_(Expr, NULL));
	kObject_setNullObject(base->nullExpr,  1);
	KINITv(base->nullBlock, new_(Block, NULL));
	kObject_setNullObject(base->nullBlock,  1);

	KINITv(base->aBuffer, new_(Array, 0));

	defineDefaultSyntax(_ctx, base->rootks);

	base->kw_dot = KW_(".");
	base->kw_comma = KW_(",");
	base->kw_colon = KW_(":");
	base->kw_declmethod = KW_("void");
	base->kw_decltype = KW_("var");
	base->kw_type   = KW_("$type");
	base->kw_params = KW_("$params");
	base->kw_then = KW_("then");
	base->kw_else = KW_("else");

	// export
	base->keyword             = keyword;
	base->Stmt_token          = Stmt_token;
	base->Stmt_block          = Stmt_block;
	base->Stmt_expr           = Stmt_expr;
	base->Stmt_text           = Stmt_text;

	base->Expr_setConstValue  = Expr_setConstValue;
	base->Expr_setNConstValue  = Expr_setNConstValue;
	base->Expr_setVariable    = Expr_setVariable;
	base->Expr_tyCheckAt      = Expr_tyCheckAt;
	base->Stmt_tyCheckExpr    = Stmt_tyCheckExpr;
	base->Block_tyCheckAll    = Block_tyCheckAll;
	base->Stmt_toExprCall     = Stmt_toExprCall;
	base->parseSyntaxRule     = parseSyntaxRule;
	base->KonohaSpace_syntax        = KonohaSpace_syntax;
	base->KonohaSpace_defineSyntax  = KonohaSpace_defineSyntax;
	base->KonohaSpace_getMethodNULL = KonohaSpace_getMethodNULL;
}

static const char *Pkeyword_(CTX, keyword_t keyid)
{
	kArray *a = kevalshare->keywordList;
	if(keyid < kArray_size(a)) {
		return S_text(a->strings[keyid]);
	}
	return "unknown keyword";
}

static kString *Skeyword_(CTX, keyword_t keyid)
{
	kArray *a = kevalshare->keywordList;
	if(keyid < kArray_size(a)) {
		return a->strings[keyid];
	}
	return TS_EMPTY;
}

static ksymbol_t keyword_get(CTX, const char *name, size_t len, uintptr_t hcode, ksymbol_t def)
{
	kmap_t  *symmap = kevalshare->keywordMapNN;
	kmape_t *e = kmap_get(symmap, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && len == S_size(e->skey) && strncmp(S_text(e->skey), name, len) == 0) {
//			DBG_P("GET keyword='%s', hcode=%ld, symbol=%d", name, hcode, (ksymbol_t)e->uvalue);
			return (ksymbol_t)e->uvalue;
		}
		e = e->next;
	}
	if(def == FN_NEWID) {
		kString *skey = new_kString(name, len, SPOL_ASCII|SPOL_POOL);
		uintptr_t sym = kArray_size(kevalshare->keywordList);
		kArray_add(kevalshare->keywordList, skey);
		e = kmap_newentry(symmap, hcode);
		KINITv(e->skey, skey);
		e->uvalue = sym;
		kmap_add(symmap, e);
//		DBG_P("NEW keyword='%s', hcode=%ld, symbol=%d, len=%ld", name, hcode, (ksymbol_t)e->uvalue, len);
		return (ksymbol_t)e->uvalue;
	}
	return def;
}

static uintptr_t keyword_hash(const char *name, size_t len)
{
	uintptr_t hcode = 0;
	size_t i;
	for(i = 0; i < len; i++) {
		int ch = name[i];
		if(ch == 0) {
			len = i; break;
		}
		hcode = ch + (31 * hcode);
	}
	return hcode;
}

static ksymbol_t keyword(CTX, const char *name, size_t len, ksymbol_t def)
{
	uintptr_t hcode = keyword_hash(name, len);
	return keyword_get(_ctx, name, len, hcode, def);
}

// -------------------------------------------------------------------------

static kline_t readquote(CTX, FILE *fp, kline_t line, kwb_t *wb, int quote)
{
	int ch, prev = quote;
	while((ch = fgetc(fp)) != EOF) {
		if(ch == '\r') continue;
		if(ch == '\n') line++;
		kwb_putc(wb, ch);  // SLOW
		if(ch == quote && prev != '\\') {
			return line;
		}
		prev = ch;
	}
	return line;
}

static kline_t readcomment(CTX, FILE *fp, kline_t line, kwb_t *wb)
{
	int ch, prev = 0, level = 1;
	while((ch = fgetc(fp)) != EOF) {
		if(ch == '\r') continue;
		if(ch == '\n') line++;
		kwb_putc(wb, ch);  // SLOW
		if(prev == '*' && ch == '/') level--;
		if(prev == '/' && ch == '*') level++;
		if(level == 0) return line;
		prev = ch;
	}
	return line;
}

static kline_t readchunk(CTX, FILE *fp, kline_t line, kwb_t *wb)
{
	int ch;
	int prev = 0, isBLOCK = 0;
	while((ch = fgetc(fp)) != EOF) {
		if(ch == '\r') continue;
		if(ch == '\n') line++;
		kwb_putc(wb, ch);  // SLOW
		if(prev == '/' && ch == '*') {
			line = readcomment(_ctx, fp, line, wb);
			continue;
		}
		if(ch == '\'' || ch == '"' || ch == '`') {
			line = readquote(_ctx, fp, line, wb, ch);
			continue;
		}
		if(isBLOCK != 1 && prev == '\n' && ch == '\n') {
			break;
		}
		if(prev == '{') {
			isBLOCK = 1;
		}
		if(prev == '\n' && ch == '}') {
			isBLOCK = 0;
		}
		prev = ch;
	}
	return line;
}

static int isemptychunk(const char *t, size_t len)
{
	size_t i;
	for(i = 0; i < len; i++) {
		if(!isspace(t[i])) return 1;
	}
	return 0;
}

static kstatus_t KonohaSpace_loadstream(CTX, kKonohaSpace *ns, FILE *fp, kline_t uline)
{
	kstatus_t status;
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	while(!feof(fp)) {
		kline_t chunkheadline = uline;
		uline = readchunk(_ctx, fp, uline, &wb);
		const char *script = kwb_top(&wb, 1);
		size_t len = kwb_size(&wb);
		if(isemptychunk(script, len)) {
			status = MODEVAL_eval(_ctx, script, len, chunkheadline);
		}
		if(status != K_CONTINUE) break;
		kwb_free(&wb);
	}
	kwb_free(&wb);
	return status;
}

static kline_t uline_init(CTX, const char *path, size_t len, int line, int isreal)
{
	kline_t uline = line;
	if(isreal) {
		char buf[256];
		char *ptr = realpath(path, buf);
		uline |= kuri((const char*)buf, strlen(ptr));
		if(ptr != buf && ptr != NULL) {
			free(ptr);
		}
	}
	else {
		uline |= kuri(path, len);
	}
	return uline;
}

static kstatus_t KonohaSpace_loadscript(CTX, kKonohaSpace *ks, const char *path, size_t len, kline_t pline)
{
	kstatus_t status = K_BREAK;
	if(path[0] == '-' && path[1] == 0) {
		kline_t uline = KURI("<stdin>") | 1;
		status = KonohaSpace_loadstream(_ctx, ks, stdin, uline);
	}
	else {
		FILE *fp = fopen(path, "r");
		if(fp != NULL) {
			kline_t uline = uline_init(_ctx, path, len, 1, 1);
			status = KonohaSpace_loadstream(_ctx, ks, fp, uline);
			fclose(fp);
		}
		else {
			kreportf(ERR_, pline, "script not found: %s", path);
		}
	}
	return status;
}

kstatus_t MODEVAL_loadscript(CTX, const char *path, size_t len, kline_t pline)
{
	if (kevalmod == NULL) {
		kevalshare->h.setup(_ctx, (kmodshare_t*)kevalshare);
	}
	INIT_GCSTACK();
	kKonohaSpace *ns = new_(KonohaSpace, kevalshare->rootks);
	PUSH_GCSTACK(ns);
	kstatus_t result = KonohaSpace_loadscript(_ctx, ns, path, len, pline);
	RESET_GCSTACK();
	return result;
}

// ---------------------------------------------------------------------------
// package

static const char* packname(const char *str)
{
	char *p = strrchr(str, '.');
	return (p == NULL) ? str : (const char*)p+1;
}

static KPACKDEF PKGDEFNULL = {
	.konoha_checksum = 0,
	.name = "*stub",
	.version = "0.0",
	.note = "this is stub",
	.initPackage = NULL,
	.setupPackage = NULL,
	.initKonohaSpace = NULL,
	.setupKonohaSpace = NULL,
	.konoha_revision = 0,
};

static KPACKDEF *KonohaSpace_openGlueHandler(CTX, kKonohaSpace *ks, char *pathbuf, size_t bufsiz, const char *pname, kline_t pline)
{
	char *p = strrchr(pathbuf, '.');
	snprintf(p, bufsiz - (p  - pathbuf), "%s", K_OSDLLEXT);
	ks->gluehdr = dlopen(pathbuf, CTX_isCompileOnly() ? RTLD_NOW : RTLD_LAZY);
	if(ks->gluehdr != NULL) {
		char funcbuf[80];
		snprintf(funcbuf, sizeof(funcbuf), "%s_init", packname(pname));
		Fpackageinit f = (Fpackageinit)dlsym(ks->gluehdr, funcbuf);
		if(f != NULL) {
			KPACKDEF *packdef = f();
			return (packdef != NULL) ? packdef : &PKGDEFNULL;
		}
		else {
			kreportf(WARN_, pline, "package loader: %s has no %s function", pathbuf, funcbuf);
		}
	}
	else {
		kreportf(DEBUG_, pline, "package loader: %s has no glue library: %s", pname, pathbuf);
	}
	return &PKGDEFNULL;
}

static const char* packagepath(CTX, char *buf, size_t bufsiz, kString *pkgname)
{
	char *path = getenv("KONOHA_PACKAGEPATH"), *local = "";
	const char *fname = S_text(pkgname);
	if(path == NULL) {
		path = getenv("HOME");
		local = "/.konoha2/package";
	}
	snprintf(buf, bufsiz, "%s%s/%s/%s_glue.k", path, local, fname, packname(fname));
	return (const char*)buf;
}

static kline_t scripturi(CTX, char *pathbuf, size_t bufsiz, const char *pname)
{
	char *p = strrchr(pathbuf, '/');
	snprintf(p, bufsiz - (p  - pathbuf), "/%s_exports.k", packname(pname));
	FILE *fp = fopen(pathbuf, "r");
	if(fp != NULL) {
		fclose(fp);
		return kuri(pathbuf, strlen(pathbuf)) | 1;
	}
	return 0;
}

static kpackage_t *loadPackageNULL(CTX, kString *pkgname, kline_t pline)
{
	char fbuf[256];
	const char *path = packagepath(_ctx, fbuf, sizeof(fbuf), pkgname);
	FILE *fp = fopen(path, "r");
	kpackage_t *pack = NULL;
	if(fp != NULL) {
		INIT_GCSTACK();
		kKonohaSpace *ks = new_(KonohaSpace, kevalshare->rootks);
		kline_t uline = uline_init(_ctx, path, strlen(path), 1, 1);
		KPACKDEF *packdef = KonohaSpace_openGlueHandler(_ctx, ks, fbuf, sizeof(fbuf), S_text(pkgname), pline);
		if(packdef->initPackage != NULL) {
			packdef->initPackage(_ctx, ks, 0, NULL, pline);
		}
		if(KonohaSpace_loadstream(_ctx, ks, fp, uline) == K_CONTINUE) {
			if(packdef->initPackage != NULL) {
				packdef->setupPackage(_ctx, ks, pline);
			}
			pack = (kpackage_t*)KNH_ZMALLOC(sizeof(kpackage_t));
			KINITv(pack->name, pkgname);
			KINITv(pack->ks, ks);
			pack->packdef = packdef;
			pack->export_script = scripturi(_ctx, fbuf, sizeof(fbuf), S_text(pkgname));
			return pack;
		}
		fclose(fp);
		RESET_GCSTACK();
	}
	else {
		kreportf(ERR_, pline, "package not found: %s path=%s", S_text(pkgname), path);
		kraise(0);
	}
	return NULL;
}

static kpackage_t *getPackageNULL(CTX, kString *pkgname, kline_t pline)
{
	kmap_t *kmp = kevalshare->packageMapNO;
	size_t len = S_size(pkgname);
	uintptr_t hcode = strhash(S_text(pkgname), len);
	kmape_t *e = kmap_get(kmp, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && len == S_size(e->skey) && strncmp(S_text(e->skey), S_text(pkgname), len) == 0) {
			return (kpackage_t*)e->pvalue;
		}
		e = e->next;
	}
	kpackage_t *pack = loadPackageNULL(_ctx, pkgname, pline);
	if(pack != NULL) {
		pack->pid = kArray_size(kevalshare->packageList);
		kArray_add(kevalshare->packageList, pkgname);
		e = kmap_newentry(kmp, hcode);
		KINITv(e->skey, pkgname);
		e->pvalue = pack;
		kmap_add(kmp, e);
		return (kpackage_t*)e->pvalue;
	}
	return pack;
}

static kbool_t KonohaSpace_importPackage(CTX, kKonohaSpace *ks, kString *pkgname, kline_t pline)
{
	kbool_t res = 0;
	kpackage_t *pack = getPackageNULL(_ctx, pkgname, pline);
	if(pack != NULL) {
		if(pack->packdef->initKonohaSpace != NULL) {
			res = pack->packdef->initKonohaSpace(_ctx, ks, pline);
		}
		if(res && pack->export_script != 0) {
			kString *fname = S_uri(pack->export_script);
			kline_t uline = pack->export_script | (kline_t)1;
			FILE *fp = fopen(S_text(fname), "r");
			if(fp != NULL) {
				res = (KonohaSpace_loadstream(_ctx, ks, fp, uline) == K_CONTINUE);
				fclose(fp);
			}
			else {
				kreportf(WARN_, pline, "script not found: %s", S_text(fname));
				res = 0;
			}
		}
		if(res && pack->packdef->setupKonohaSpace != NULL) {
			res = pack->packdef->setupKonohaSpace(_ctx, ks, pline);
		}
	}
	return res;
}

// boolean KonohaSpace.importPackage(String pkgname);
static KMETHOD KonohaSpace_importPackage_(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(KonohaSpace_importPackage(_ctx, sfp[0].ks, sfp[1].s, sfp[K_RTNIDX].uline));
}

// boolean KonohaSpace.loadScript(String path);
static KMETHOD KonohaSpace_loadScript_(CTX, ksfp_t *sfp _RIX)
{
	kline_t pline = sfp[K_RTNIDX].uline;
	FILE *fp = fopen(S_text(sfp[1].s), "r");
	if(fp != NULL) {
		kline_t uline = uline_init(_ctx, S_text(sfp[1].s), S_size(sfp[1].s), 1, 1);
		kstatus_t status = KonohaSpace_loadstream(_ctx, sfp[0].ks, fp, uline);
		fclose(fp);
		RETURNb_(status == K_CONTINUE);
	}
	else {
		kreportf(ERR_, pline, "script not found: %s", S_text(sfp[1].s));
		RETURNb_(0);
	}
}

#define _Public kMethod_Public
#define _Static kMethod_Static
#define _F(F)   (intptr_t)(F)
#define TY_KonohaSpace  (CT_KonohaSpace)->cid

KMETHOD KonohaSpace_man(CTX, ksfp_t *sfp _RIX);

void MODEVAL_defMethods(CTX)
{
	int FN_pkgname = FN_("pkgname");
	intptr_t methoddata[] = {
		_Public, _F(KonohaSpace_importPackage_), TY_Boolean, TY_KonohaSpace, MN_("importPackage"), 1, TY_String, FN_pkgname,
		_Public, _F(KonohaSpace_loadScript_), TY_Boolean, TY_KonohaSpace, MN_("loadScript"), 1, TY_String, FN_("path"),
//		_Public, _F(KonohaSpace_man), TY_void, TY_KonohaSpace, MN_("man"), 1, TY_Object, FN_("x") | FN_COERCION,
		DEND,
	};
	kloadMethodData(NULL, methoddata);
}

#ifdef __cplusplus
}
#endif
