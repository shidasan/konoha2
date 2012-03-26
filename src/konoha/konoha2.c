/****************************************************************************
 * KONOHA2.0 COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
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

#include <konoha2/konoha2.h>
#include "../gc/gc_api.h"
#include <dlfcn.h>

#ifdef K_USING_LOGPOOL
#include <logpool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


void MODCODE_init(CTX, kcontext_t *ctx);
void MODEVAL_init(CTX, kcontext_t *ctx);

/* ------------------------------------------------------------------------ */

#define IS_ROOTCTX(o)  (_ctx == (CTX_t)o)

static void klogger_init(CTX, kcontext_t *ctx)
{
#ifdef K_USING_LOGPOOL
	logpool_syslog_param pa = {8, 1024};
	ctx->logger = (struct klogger_t *) ltrace_open((ltrace_t*)_ctx->parent->logger, &pa);
	if(_ctx == NULL) {
		const char *ptrace = getenv("DEOS_TRACE");
		if(ptrace == NULL) {
			ptrace = "$(setenv DEOS_TRACE )";
		}
		ltrace_record(_ctx->logger, LOG_NOTICE, "konoha:newtrace",
				LOG_s("parent", ptrace), LOG_u("ppid", getppid()));
	}
	else {
		ltrace_record(_ctx->logger, LOG_NOTICE, "konoha:newtrace",
				LOG_s("parent", _ctx->trace));
	}
#else
	ctx->logger = NULL;
#endif
}

static void klogger_free(CTX, kcontext_t *ctx)
{
#ifdef K_USING_LOGPOOL
	ltrace_close((ltrace_t*)_ctx->logger);
#endif
}

// -------------------------------------------------------------------------
// ** util **


// -------------------------------------------------------------------------
// ** global **

void konoha_ginit(int argc, const char **argv)
{

}

static void konoha_init(void)
{
	static int isInit = 0;
	if(isInit == 0) {
		isInit = 1;
	}
}

//void knh_beginContext(CTX, void **bottom)
//{
//	ctx->stack->cstack_bottom = bottom;
//}
//
//void knh_endContext(CTX)
//{
//	ctx->stack->cstack_bottom = NULL;
//}

/* ------------------------------------------------------------------------ */
/* stack */

static void kstack_init(CTX, kcontext_t *ctx, size_t n)
{
	size_t i;
	kstack_t *base = (kstack_t*)KNH_ZMALLOC(sizeof(kstack_t));
	base->stacksize = n;
	base->stack = (ksfp_t*)KNH_ZMALLOC(sizeof(ksfp_t)*n);
	base->stack_uplimit = base->stack + (n - 64);
	for(i = 0; i < n; i++) {
		KINITv(base->stack[i].o, K_NULL);
	}
	KINITv(base->gcstack, new_(Array, K_PAGESIZE/sizeof(void*)));
	KARRAY_INIT(base->cwb, K_PAGESIZE * 4, char);
	KARRAY_INIT(base->ref, K_PAGESIZE, REF_t);
	base->tail = base->ref.refhead;
	ctx->esp = base->stack;
	ctx->stack = base;
}

static void kstack_reftrace(CTX, kcontext_t *ctx)
{
	ksfp_t *sp = ctx->stack->stack;
	BEGIN_REFTRACE((_ctx->esp - sp)+1);
	while(sp < ctx->esp) {
		KREFTRACEv(sp[0].o);
		sp++;
	}
	KREFTRACEv(ctx->stack->gcstack);
	END_REFTRACE();
}

static void kstack_free(CTX, kcontext_t *ctx)
{
	KARRAY_FREE(_ctx->stack->cwb, char);
	KARRAY_FREE(_ctx->stack->ref, REF_t);
	KNH_FREE(_ctx->stack->stack, sizeof(ksfp_t) * ctx->stack->stacksize);
	KNH_FREE(_ctx->stack, sizeof(kstack_t));
}

REF_t* kstack_tail(CTX, size_t size)
{
	kstack_t *stack = _ctx->stack;
	size_t ref_size = stack->tail - stack->ref.refhead;
	if(stack->ref.max < size + ref_size) {
		KARRAY_EXPAND(stack->ref, size + ref_size, REF_t);
		stack->tail = stack->ref.refhead + ref_size;
	}
	REF_t *tail = stack->tail;
	stack->tail = NULL;
	return tail;
}

static kbool_t kshare_setModule(CTX, int x, kmodshare_t *d, kline_t pline)
{
	if(_ctx->modshare[x] == NULL) {
		_ctx->modshare[x] = d;
		return 1;
	}
	else {
		kreportf(ERR_, pline, "already registered: %s", _ctx->modshare[x]->name);
		return 0;
	}
}

/* ------------------------------------------------------------------------ */
/* [kcontext] */

// module
// share local
//       logger
// mem   mem
// share stack
// modshare[128] mod[128]
// keval

void MODEVAL_defMethods(CTX);

static kcontext_t* new_context(const kcontext_t *_ctx)
{
	kcontext_t *newctx;
	static volatile size_t ctxid_counter = 0;
	ctxid_counter++;
	if(_ctx == NULL) {  // NULL means first one
		klib2_t *klib2 = (klib2_t*)malloc(sizeof(klib2_t) + sizeof(kcontext_t));
		bzero(klib2, sizeof(klib2_t) + sizeof(kcontext_t));
		klib2_init(klib2);
		klib2->KsetModule = kshare_setModule;
		newctx = (kcontext_t*)(klib2 + 1);
		newctx->lib2 = klib2;
		_ctx = (CTX_t)newctx;

		klogger_init(_ctx, newctx);
		kmemshare_init(_ctx, newctx);
		kshare_init(_ctx, newctx);
		newctx->modshare = (kmodshare_t**)KNH_ZMALLOC(sizeof(kmodshare_t*) * K_PKGMATRIX);
	}
	else {   // others take ctx as its parent
		newctx = (kcontext_t*)KNH_ZMALLOC(sizeof(kcontext_t));
		newctx->lib2 = _ctx->lib2;
		newctx->memshare = _ctx->memshare;
		newctx->share = _ctx->share;
		newctx->modshare = _ctx->modshare;
		klogger_init(_ctx, newctx);
	}
	//kmemlocal_init(_ctx, newctx);
	kstack_init(_ctx, newctx, K_PAGESIZE * 16);
	newctx->mod = (kmod_t**)KNH_ZMALLOC(sizeof(kmod_t*) * K_PKGMATRIX);
//	for(i = 0; i < K_PKGMATRIX; i++) {
//		if(newctx->modshare[i] != NULL && newctx->modshare[i]->new_local != NULL) {
//			newctx->mod[i] = newctx->modshare[i]->new_local((CTX_t)newctx, newctx->modshare[i]);
//		}
//	}
	if(IS_ROOTCTX(newctx)) {
		MODCODE_init(_ctx, newctx);
		MODEVAL_init(_ctx, newctx);
		kshare_init_methods(_ctx);
		MODEVAL_defMethods(_ctx);
	}
	return newctx;
}

static void kcontext_reftrace(CTX, kcontext_t *ctx)
{
	size_t i;
	if(IS_ROOTCTX(_ctx)) {
		kshare_reftrace(_ctx, ctx);
		for(i = 0; i < K_PKGMATRIX; i++) {
			kmodshare_t *p = ctx->modshare[i];
			if(p != NULL && p->reftrace != NULL) {
				p->reftrace(_ctx, p);
			}
		}
	}
	kstack_reftrace(_ctx, ctx);
	for(i = 0; i < K_PKGMATRIX; i++) {
		kmod_t *p = ctx->mod[i];
		if(p != NULL && p->reftrace != NULL) {
			p->reftrace(_ctx, p);
		}
	}
}

void kSystem_reftraceAll(CTX)
{
	kcontext_reftrace(_ctx, (kcontext_t*)_ctx);
}

static void kcontext_free(CTX, kcontext_t *ctx)
{
	size_t i;
	for(i = 0; i < K_PKGMATRIX; i++) {
		kmod_t *p = ctx->mod[i];
		if(p != NULL && p->reftrace != NULL) {
			p->free(_ctx, p);
		}
	}
	KNH_FREE(_ctx->mod, sizeof(kmod_t*) * K_PKGMATRIX);
	kstack_free(_ctx, ctx);
	if(IS_ROOTCTX(_ctx)){  // share
		klib2_t *klib2 = (klib2_t*)ctx - 1;
		for(i = 0; i < K_PKGMATRIX; i++) {
			kmodshare_t *p = ctx->modshare[i];
			if(p != NULL && p->free != NULL) {
				p->free(_ctx, p);
			}
		}
		KNH_FREE(_ctx->modshare, sizeof(kmodshare_t*) * K_PKGMATRIX);
		kmemshare_gc_destroy(_ctx, ctx);
		kshare_free(_ctx, ctx);
		kmemshare_free(_ctx, ctx);
		kmemlocal_free(_ctx, ctx);
		klogger_free(_ctx, ctx);
		free(klib2/*, sizeof(klib2_t) + sizeof(kcontext_t)*/);
	}
	else {
		klogger_free(_ctx, ctx);
		KNH_FREE(ctx, sizeof(kcontext_t));
	}
}

konoha_t konoha_open(void)
{
	konoha_init();
	return (konoha_t)new_context(NULL);
}

void konoha_close(konoha_t konoha)
{
	kcontext_free((CTX_t)konoha, (kcontext_t*)konoha);
}

// -------------------------------------------------------------------------
// keval

kstatus_t MODEVAL_eval(CTX, const char *script, size_t len, kline_t uline);
kstatus_t MODEVAL_loadscript(CTX, const char *path, size_t len, kline_t pline);

/* ------------------------------------------------------------------------ */

static char *(*kreadline)(const char *);
static int  (*kadd_history)(const char *);

static char* readline(const char* prompt)
{
	static int checkCTL = 0;
	int ch, pos = 0;
	static char linebuf[1024]; // THREAD-UNSAFE
	fputs(prompt, stdout);
	while((ch = fgetc(stdin)) != EOF) {
		if(ch == '\r') continue;
		if(ch == 27) {
			/* ^[[A */;
			fgetc(stdin); fgetc(stdin);
			if(checkCTL == 0) {
				fprintf(stdout, " - use readline, it provides better shell experience.\n");
				checkCTL = 1;
			}
			continue;
		}
		if(ch == '\n' || pos == sizeof(linebuf) - 1) {
			linebuf[pos] = 0;
			break;
		}
		linebuf[pos] = ch;
		pos++;
	}
	if(ch == EOF) return NULL;
	char *p = (char*)malloc(pos+1);
	memcpy(p, linebuf, pos+1);
	return p;
}

static int add_history(const char* line)
{
	return 0;
}

static int checkstmt(const char *t, size_t len)
{
	size_t i = 0;
	int ch, quote = 0, nest = 0;
	int hasDOC = 0;
	L_NORMAL:
	for(; i < len; i++) {
		ch = t[i];
		if(ch == '{' || ch == '[' || ch == '(') nest++;
		if(ch == '}' || ch == ']' || ch == ')') nest--;
		if(ch == '\'' || ch == '"' || ch == '`') {
			if(t[i+1] == ch && t[i+2] == ch) {
				quote = ch; i+=2;
				goto L_TQUOTE;
			}
		}
		if(ch == '\n') hasDOC = 0;
		if(ch == '#') {
			hasDOC = 1;
		}
	}
	return (hasDOC == 1) ? 1 : nest;
	L_TQUOTE:
	DBG_ASSERT(i > 0);
	for(; i < len; i++) {
		ch = t[i];
		if(t[i-1] != '\\' && ch == quote) {
			if(t[i+1] == ch && t[i+2] == ch) {
				i+=2;
				goto L_NORMAL;
			}
		}
	}
	return 1;
}

static kstatus_t readstmt(CTX, kwb_t *wb, kushort_t *uline)
{
	int line = 1;
	kstatus_t status = K_CONTINUE;
//	CWB_clear(cwb, 0);
//	fputs(TERM_BBOLD(_ctx), stdout);
	while(1) {
		int check;
		char *ln = kreadline(line == 1 ? ">>> " : "    ");
		if(ln == NULL) {
			kwb_free(wb);
			status = K_BREAK;
			break;
		}
		if(line > 1) kwb_putc(wb, '\n');
		kwb_write(wb, ln, strlen(ln));
		free(ln);
		if((check = checkstmt(kwb_top(wb, 0), kwb_size(wb))) > 0) {
			uline[1]++;
			line++;
			continue;
		}
		if(check < 0) {
			fputs("(Cancelled)...\n", stdout);
			kwb_free(wb);
		}
		break;
	}
	if(kwb_size(wb) > 0) {
		kadd_history(kwb_top(wb, 1));
	}
//	fputs(TERM_EBOLD(_ctx), stdout);
	fflush(stdout);
	return status;
}

static void shell(CTX)
{
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	kline_t uline = KURI("(shell)") | 1;
	void *handler = dlopen("libreadline" K_OSDLLEXT, RTLD_LAZY);
	void *f = (handler != NULL) ? dlsym(handler, "readline") : NULL;
	kreadline = (f != NULL) ? (char* (*)(const char*))f : readline;
	f = (handler != NULL) ? dlsym(handler, "add_history") : NULL;
	kadd_history = (f != NULL) ? (int (*)(const char*))f : add_history;
	fprintf(stdout, "Konoha 2.0-alpha (Miyajima) (%d,%s)\n", __DATE__); // FIXME
	fprintf(stdout, "[gcc %s]\n", __VERSION__);
	while(1) {
		kstatus_t status = readstmt(_ctx, &wb, (kushort_t*)&uline);
		if(status == K_CONTINUE && kwb_size(&wb) > 0) {
			status = MODEVAL_eval(_ctx, kwb_top(&wb, 1), kwb_size(&wb), uline);
			if(status != K_FAILED) {

			}
		}
		kwb_free(&wb);
		if(status == K_BREAK) {
			break;
		}
	}
	kwb_free(&wb);
	fprintf(stdout, "\n");
	return;
}

int konoha_main(konoha_t konoha, int argc, const char **argv)
{
	int ret = 0;
	//int i, ret = 0, n = knh_parseopt(_ctx, argc, argv);
	//argc = argc - n;
	//argv = argv + n;
	if(argc == 1) {
		shell((CTX_t)konoha);
	}
	else {
		if(MODEVAL_loadscript((CTX_t)konoha, argv[1], strlen(argv[1]), 0) == K_CONTINUE/* && !knh_isCompileOnly(_ctx)*/) {
			//ret = knh_runMain(_ctx, argc, argv);
			//if(isInteractiveMode) {
			//	konoha_shell(_ctx, NULL);
			//}
		}
	}
	return ret;
}

#ifdef __cplusplus
}
#endif
