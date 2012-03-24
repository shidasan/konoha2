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

#include "perror.h"
#include "struct.h"
#include "token.h"
#include "ast.h"
#include "tycheck.h"

#define TOKEN(T)  .name = T, .namelen = (sizeof(T)-1)
#define _EXPR     .rule ="$expr"

static ksyntaxdef_t SYNTAX[] = {
	{ TOKEN("$ERR"), .StmtTyCheck = StmtTyCheck_err },
	{ TOKEN("$expr"),  .rule ="$expr", .StmtAdd = StmtAdd_expr, .StmtTyCheck = StmtTyCheck_expr, .ExprTyCheck = ExprTyCheck_call, },
	{ TOKEN("$block"), .StmtAdd = StmtAdd_block, },
	{ TOKEN("$type"),  .StmtAdd = StmtAdd_type, },
	{ TOKEN("$cname"), .StmtAdd = StmtAdd_cname, },
	{ TOKEN("$name"),  .StmtAdd = StmtAdd_name, .ExprTyCheck = ExprTyCheck_invoke, },
	{ TOKEN("$params"),  .StmtAdd = StmtAdd_params, .StmtTyCheck = StmtTyCheck_declParams, },
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
	{ TOKEN("void"), .type = TY_void, .rule ="$type [$cname '.'] $name $params [$block]", .StmtTyCheck = StmtTyCheck_declMethod},
	{ TOKEN("var"), .type = TY_var, .rule ="$type $expr", },
	{ TOKEN("int"), .type = TY_Int, },
	//	{ TOKEN("dynamic"), .type = TY_dynamic, },
	{ TOKEN("null"), .ExprTyCheck = TokenTyCheck_NULL,},
	{ TOKEN("true"),  .ExprTyCheck = TokenTyCheck_TRUE,},
	{ TOKEN("false"), .ExprTyCheck = TokenTyCheck_FALSE,},
	{ TOKEN("if"), .rule ="'if' '(' $expr ')' then: $block ['else' else: $block]", .StmtTyCheck = StmtTyCheck_if, },
	{ TOKEN("else"), .rule = "'else' else: $block" },
	{ TOKEN("break"), .rule ="'break' [$expr]", .StmtTyCheck = StmtTyCheck_nop },
	{ TOKEN("return"), .rule ="'return' [$expr]", .StmtTyCheck = StmtTyCheck_return, },
	{ TOKEN("TK_SYMBOL"), .keyid = KW_TK(TK_SYMBOL), .ExprTyCheck = TokenTyCheck_SYMBOL, },
	{ TOKEN("TK_USYMBOL"), .keyid = KW_TK(TK_USYMBOL), .ExprTyCheck = TokenTyCheck_USYMBOL, },
	{ TOKEN("TK_TEXT"), .keyid = KW_TK(TK_TEXT), .ExprTyCheck = TokenTyCheck_TEXT, },
	{ TOKEN("TK_INT"), .keyid = KW_TK(TK_INT), .ExprTyCheck = TokenTyCheck_INT, },
	{ TOKEN("TK_FLOAT"), .keyid = KW_TK(TK_FLOAT), .ExprTyCheck = TokenTyCheck_FLOAT, },
	{ .name = NULL, },
};

/* ------------------------------------------------------------------------ */
/* kevalmod_t global functions */

static kstatus_t Lingo_eval(CTX, kLingo *lgo, const char *script, kline_t uline)
{
	kevalshare->h.setup(_ctx, (kmodshare_t*)kevalshare);
	{
		INIT_GCSTACK();
		kArray *tls = kevalmod->tokens;
		size_t pos = kArray_size(tls);
		if(uline == 0) {

		}
		ktokenize(_ctx, script, uline, _TOPLEVEL, tls);
		kBlock *bk = new_Block(_ctx, tls, pos, kArray_size(tls), lgo);
		kArray_clear(tls, pos);
		Block_eval(_ctx, bk);
		RESET_GCSTACK();
	}
	return K_CONTINUE;
}

kstatus_t MODEVAL_eval(CTX, const char *script, size_t len, kline_t uline)
{
	DBG_ASSERT(script[len] == 0);
	//DBG_(if(knh_isVerboseLang()) {
	fprintf(stderr, "\n>>>----\n'%s'\n------\n", script);
	//});
	kevalshare->h.setup(_ctx, (kmodshare_t*)kevalshare);
	return Lingo_eval(_ctx, kevalshare->rootlgo, script, uline);
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
		KINITv(base->evalval.o, K_NULL);
		base->evalval.ivalue = 0;
		base->evaljmpbuf = (kjmpbuf_t*)KNH_ZMALLOC(sizeof(kjmpbuf_t));
		_ctx->mod[MOD_EVAL] = (kmod_t*)base;
	}
}

static void pack_reftrace(CTX, kmape_t *p)
{
	kpackage_t *pack = (kpackage_t*)p->uvalue;
	BEGIN_REFTRACE(2);
	KREFTRACEn(pack->name);
	KREFTRACEn(pack->lgo);
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
	l->KLingo_getcid   = Lingo_getcid;
	l->KloadMethodData = Lingo_loadMethodData;

	KINITv(base->keywordList, new_(Array, 32));
	base->keywordMapNN = kmap_init(0);
	KINITv(base->packageList, new_(Array, 8));
	base->packageMapNO = kmap_init(0);

	ksetModule(MOD_EVAL, (kmodshare_t*)base, 0);
	base->cLingo = kaddClassDef(&LingoDef);
	base->cToken = kaddClassDef(&TokenDef);
	base->cExpr  = kaddClassDef(&ExprDef);
	base->cStmt  = kaddClassDef(&StmtDef);
	base->cBlock = kaddClassDef(&BlockDef);
	base->cGamma = kaddClassDef(&GammaDef);

	KINITv(base->rootlgo, new_(Lingo, NULL));
	KINITv(base->nullToken, new_(Token, NULL));
	kObject_setNullObject(base->nullToken, 1);
	KINITv(base->nullExpr, new_(Expr, NULL));
	kObject_setNullObject(base->nullExpr,  1);
	KINITv(base->nullBlock, new_(Block, NULL));
	kObject_setNullObject(base->nullBlock,  1);

	KINITv(base->aBuffer, new_(Array, 0));

	Lingo_defineSyntax(_ctx, base->rootlgo, SYNTAX);

	base->kw_dot = KW_(".");
	base->kw_comma = KW_(",");
	base->kw_colon = KW_(":");
	base->kw_declmethod = KW_("void");
	base->kw_decltype = KW_("var");
	base->kw_params = KW_("$params");
	base->kw_then = KW_("then");
	base->kw_else = KW_("else");
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

static kstatus_t Lingo_loadstream(CTX, kLingo *ns, FILE *fp, kline_t uline)
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

static kstatus_t Lingo_loadscript(CTX, kLingo *lgo, const char *path, size_t len, kline_t pline)
{
	kstatus_t status = K_BREAK;
	if(path[0] == '-' && path[1] == 0) {
		kline_t uline = KURI("<stdin>") | 1;
		status = Lingo_loadstream(_ctx, lgo, stdin, uline);
	}
	else {
		FILE *fp = fopen(path, "r");
		if(fp != NULL) {
			kline_t uline = uline_init(_ctx, path, len, 1, 1);
			status = Lingo_loadstream(_ctx, lgo, fp, uline);
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
	kLingo *ns = new_(Lingo, kevalshare->rootlgo);
	PUSH_GCSTACK(ns);
	kstatus_t result = Lingo_loadscript(_ctx, ns, path, len, pline);
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

static kpackage_def PKGDEFNULL = {
	.konoha_checksum = 0,
	.name = "*stub",
	.version = "0.0",
	.note = "this is stub",
	.initPackage = NULL,
	.setupPackage = NULL,
	.initLingo = NULL,
	.setupLingo = NULL,
	.konoha_revision = 0,
};

static kpackage_def *Lingo_openGlueHandler(CTX, kLingo *lgo, char *pathbuf, size_t bufsiz, const char *pname, kline_t pline)
{
	char *p = strrchr(pathbuf, '.');
	snprintf(p, bufsiz - (p  - pathbuf), "%s", K_OSDLLEXT);
	lgo->gluehdr = dlopen(pathbuf, CTX_isCompileOnly() ? RTLD_NOW : RTLD_LAZY);
	if(lgo->gluehdr != NULL) {
		char funcbuf[80];
		snprintf(funcbuf, sizeof(funcbuf), "%s_init", packname(pname));
		Fpackageinit f = (Fpackageinit)dlsym(lgo->gluehdr, funcbuf);
		if(f != NULL) {
			kpackage_def *packdef = f();
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
		kLingo *lgo = new_(Lingo, kevalshare->rootlgo);
		kline_t uline = uline_init(_ctx, path, strlen(path), 1, 1);
		kpackage_def *packdef = Lingo_openGlueHandler(_ctx, lgo, fbuf, sizeof(fbuf), S_text(pkgname), pline);
		if(packdef->initPackage != NULL) {
			packdef->initPackage(_ctx, lgo, pline);
		}
		if(Lingo_loadstream(_ctx, lgo, fp, uline) == K_CONTINUE) {
			if(packdef->initPackage != NULL) {
				packdef->setupPackage(_ctx, lgo, pline);
			}
			pack = (kpackage_t*)KNH_ZMALLOC(sizeof(kpackage_t));
			KINITv(pack->name, pkgname);
			KINITv(pack->lgo, lgo);
			pack->packdef = packdef;
			pack->export_script = scripturi(_ctx, fbuf, sizeof(fbuf), S_text(pkgname));
			return pack;
		}
		fclose(fp);
		RESET_GCSTACK();
	}
	else {
		kreportf(ERR_, pline, "package not found: %s path=%s", S_text(pkgname), path);
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

static void Lingo_importPackage(CTX, kLingo *lgo, kString *pkgname, kline_t pline)
{
	kpackage_t *pack = getPackageNULL(_ctx, pkgname, pline);
	if(pack != NULL) {
		if(pack->packdef->initLingo != NULL) {
			pack->packdef->initLingo(_ctx, lgo, pline);
		}
		if(pack->export_script != 0) {
			kString *fname = S_uri(pack->export_script);
			kline_t uline = pack->export_script | (kline_t)1;
			FILE *fp = fopen(S_text(fname), "r");
			if(fp != NULL) {
				Lingo_loadstream(_ctx, lgo, fp, uline);
				fclose(fp);
			}
			else {
				kreportf(ERR_, pline, "script not found: %s", S_text(fname));
			}
		}
		if(pack->packdef->setupLingo != NULL) {
			pack->packdef->setupLingo(_ctx, lgo, pline);
		}
	}
}

// void Lingo.importPackage(String pkgname);
static KMETHOD Lingo_importPackage_(CTX, ksfp_t *sfp _RIX)
{
	Lingo_importPackage(_ctx, sfp[0].lgo, sfp[1].s, sfp[K_RTNIDX].uline);
}

// boolean Lingo.load(String path);
static KMETHOD Lingo_loadScript_(CTX, ksfp_t *sfp _RIX)
{
	kline_t pline = sfp[K_RTNIDX].uline;
	FILE *fp = fopen(S_text(sfp[1].s), "r");
	if(fp != NULL) {
		kline_t uline = uline_init(_ctx, S_text(sfp[1].s), S_size(sfp[1].s), 1, 1);
		kstatus_t status = Lingo_loadstream(_ctx, sfp[0].lgo, fp, uline);
		fclose(fp);
		RETURNb_(status == K_CONTINUE);
	}
	else {
		kreportf(ERR_, pline, "script not found: %s", S_text(sfp[1].s));
		RETURNb_(0);
	}
}

// void Lingo.p(String msg);
static KMETHOD Lingo_p(CTX, ksfp_t *sfp _RIX)
{
	kline_t uline = sfp[K_RTNIDX].uline;
	fprintf(stdout, "uline=%ld, %s\n", uline, S_text(sfp[1].s));
}

#define _Public kMethod_Public
#define _Static kMethod_Static
#define _F(F)   (intptr_t)(F)
#define TY_Lingo  (CT_Lingo)->cid

void MODEVAL_defMethods(CTX)
{
	int FN_msg = FN_("msg");
	int FN_pkgname = FN_("pkgname");
	intptr_t methoddata[] = {
		_Public, _F(Lingo_p), TY_void, TY_Lingo, MN_("p"), 1, TY_String, FN_msg,
		_Public, _F(Lingo_importPackage_), TY_void, TY_Lingo, MN_("importPackage"), 1, TY_String, FN_pkgname,
		_Public, _F(Lingo_loadScript_), TY_Boolean, TY_Lingo, MN_("loadScript"), 1, TY_String, FN_("path"),
		DEND,
	};
	kaddMethodDef(NULL, methoddata);
}

#ifdef __cplusplus
}
#endif
