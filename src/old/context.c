/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
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

/* ************************************************************************ */

#define K_DEFINE_API2
#include"commons.h"

#ifdef K_USING_POSIX_
#include <unistd.h>
#endif

#define K_USING_LOADDATA
#include"../../include/konoha2/konohalang.h"

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------ */
/* [Initialize Object] */

static kInputStream *new_InputStreamStdIn(CTX, kString *enc)
{
	kio_t *io2 = new_io2_stdio(_ctx, 0, 0);
	return new_InputStream(_ctx, io2, new_Path(_ctx, TS_DEVSTDIN));
}

static kOutputStream *new_OutputStreamStdOut(CTX, kString *enc)
{
	kio_t *io2 = new_io2_stdio(_ctx, 1, 4096);
	kOutputStream *w = new_OutputStream(_ctx, io2, new_Path(_ctx, TS_DEVSTDOUT));
	OutputStream_setAutoFlush(w, 1);
	return w;
}

static kOutputStream *new_OutputStreamStdErr(CTX, kString *enc)
{
	kio_t *io2 = new_io2_stdio(_ctx, 2, 0);
	return new_OutputStream(_ctx, io2, new_Path(_ctx, TS_DEVSTDERR));
}

/* ------------------------------------------------------------------------ */
/* [ContextTable] */

static kcontext_t* new_hcontext(CTX0)
{
	kcontext_t *ctx;
	static volatile size_t ctxid_counter = 0;
	if(_ctx0 == NULL) {
		ctx = (kcontext_t*)malloc(sizeof(kcontext_t));
		knh_bzero(_ctx, sizeof(kcontext_t));
	}
	else {
		KNH_ASSERT_CTX0(_ctx0);
		ctx = (kcontext_t*)KNH_MALLOC(_ctx0, sizeof(kcontext_t));
		knh_bzero(_ctx, sizeof(kcontext_t));
	}
	ctx->flag = 0;
	ctx->ctxid = ctxid_counter;
	ctxid_counter++;
//	ctx->freeObjectList = NULL;
//	ctx->freeMemoryList = NULL;
	ctx->parent = ctx;
	ctx->api2 = getapi2();
	{
		uintptr_t i = 0, ch;
		kuint_t t = knh_rand();
		ch = t % 26;
		ctx->trace[i] = 'A' + ch;
		for(i = 1; i < 9; i++) {
			t = t / 36;
			if (t == 0) t = knh_rand();
			ch = t % 36;
			ctx->trace[i] = (ch < 10) ? '0' + ch : 'A' + (ch - 10);
		}
	}
	ctx->seq = 0;
	ctx->ctxobjNC = NULL;
	if(_ctx0 == NULL) {
		const char *ptrace = knh_getenv(K_DEOS_TRACE);
		if(ptrace == NULL) {
			ptrace = "$(setenv " K_DEOS_TRACE ")";
		}
		KNH_NTRACE2(_ctx, "konoha:newtrace", K_NOTICE, KNH_LDATA(
					LOG_s("parent", ptrace)
#if defined(K_USING_POSIX_)
					, LOG_u("ppid", getppid())
#endif /* !defined(K_USING_POSIX_) */
					));
	}
	else {
		KNH_NTRACE2(_ctx, "konoha:newtrace", K_NOTICE, KNH_LDATA(LOG_s("parent", ctx0->trace), LOG_u("seq", ctx0->seq)));
	}
	return ctx;
}

/* ------------------------------------------------------------------------ */
/* [ContextCommon] */

static void CommonContext_init(CTX, kcontext_t *o)
{
	KNH_ASSERT_CTX0(_ctx);
	DBG_ASSERT(o->script != NULL);
	DBG_ASSERT(o->gma != NULL);
	KINITv(o->enc, ctx->share->enc);
	KINITv(o->in,  ctx->share->in);
	KINITv(o->out, ctx->share->out);
	KINITv(o->err, ctx->share->err);
	KINITv(o->e, K_NULL);
	KINITv(o->evaled, K_NULL);
	KINITv(o->errmsgs, new_Array0(_ctx, 0));
	KINITv(o->gcstack, new_Array0(_ctx, 0));
#ifndef K_USING_STRINGPOOL
	KINITv(o->symbolDictMap, new_DictMap0(_ctx, 256, 0/*isCaseMap*/, "Context.symbolDictMap"));
#endif
//	KINITv(o->constPools, new_Array0(_ctx, 0));
	o->ctxlock = knh_mutex_malloc(_ctx);
}

static kObject** CommonContext_reftrace(CTX, kcontext_t *ctxo)
{
	size_t i;
#ifndef K_USING_STRINGPOOL
	KREFTRACEv(_ctxo->symbolDictMap);
#endif
//	KREFTRACEv(_ctxo->constPools);
	KREFTRACEv(_ctxo->e);
	KREFTRACEv(_ctxo->evaled);
	KREFTRACEv(_ctxo->errmsgs);
	KREFTRACEv(_ctxo->gcstack);
	KREFTRACEv((_ctxo->script));
	KREFTRACEv((_ctxo->enc));
	KREFTRACEv((_ctxo->in));
	KREFTRACEv((_ctxo->out));
	KREFTRACEv((_ctxo->err));
	KREFTRACEv(_ctxo->gma);
	// stack
	KREFTRACEv((_ctxo->bufa));
	KREFTRACEv((_ctxo->bufw));
#ifdef K_USING_RCGC
	kref_ensureSize(_ctxo->stacksize);
	for(i = 0; i < ctxo->stacksize; i++) {
		KREFTRACEv(_ctxo->stack[i].o);
	}
#else
	{
		size_t stacksize = (_ctxo->esp - ctxo->stack) + 8;
		kref_ensureSize(stacksize);
		for(i = 0; i < stacksize; i++) {
			KREFTRACEv(_ctxo->stack[i].o);
		}
	}
#endif
	if(_ctxo->sighandlers != NULL) {
		kref_ensureSize(K_SIGNAL_MAX);
		for (i = 0; i < K_SIGNAL_MAX; i++) {
			KREFTRACEn(_ctxo->sighandlers[i]);
		}
	}
	return tail_;
}

static void CommonContext_free(CTX, kcontext_t *ctxo)
{
	KNH_FREE(_ctx, ctxo->stack, sizeof(ksfp_t) * ctxo->stacksize);
	ctxo->stack = NULL;
	ctxo->esp = NULL;
	ctxo->stack_uplimit = NULL;
	ctxo->stacksize = 0;
	KNH_FREE(_ctx, ctxo->mtdcache,  K_MTDCACHE_SIZE * sizeof(knh_mtdcache_t));
	KNH_FREE(_ctx, ctxo->tmrcache, K_TMAPCACHE_SIZE * sizeof(knh_tmrcache_t));
	ctxo->mtdcache  = NULL;
	ctxo->tmrcache = NULL;
	knh_mutex_free(_ctxo, ctxo->ctxlock);
	ctxo->ctxlock = NULL;
	ctxo->bufa = NULL;
	if(_ctx->sighandlers != NULL) {
		KNH_FREE(_ctx, ctx->sighandlers, sizeof(kFunc*) * K_SIGNAL_MAX);
		ctxo->sighandlers = NULL;
	}
}

/* ------------------------------------------------------------------------ */
/* [RootContext] */


/* ------------------------------------------------------------------------ */

void knh_EventTBL_expand(CTX)
{
	size_t s = ctx->share->sizeEventTBL, max = ctx->share->capacityEventTBL * 2;
	knh_EventTBL_t *newt = (knh_EventTBL_t*)KNH_ZMALLOC(SIZEOF_TEXPT(max));
	knh_bzero(newt, SIZEOF_TEXPT(max));
	knh_memcpy(newt, ctx->share->EventTBL, SIZEOF_TEXPT(s));
	((kshare_t*)ctx->share)->EventTBL = newt;
	((kshare_t*)ctx->share)->capacityEventTBL = max;
}

static void initServiceSPI(knh_ServiceSPI_t *spi);
void knh_loadScriptAliasTermData(CTX);

static kcontext_t* new_RootContext(void)
{
	kcontext_t *ctx = (kcontext_t*)new_hcontext(NULL);
	const knh_LoaderAPI_t *kapi = knh_getLoaderAPI();
	kshare_t *share = (kshare_t*)malloc(sizeof(kshare_t) + sizeof(kstatinfo_t) + sizeof(knh_ServiceSPI_t));
	ctx->share = share;
	knh_bzero(share, sizeof(kshare_t) + sizeof(kstatinfo_t) + sizeof(knh_ServiceSPI_t));
	share->syslock = knh_mutex_malloc(_ctx);
	ctx->stat = (kstatinfo_t*)((share+1));
	ctx->spi = (const knh_ServiceSPI_t*)(_ctx->stat + 1);
	initServiceSPI((knh_ServiceSPI_t*)ctx->spi);

	kmemshare_init(_ctx);
	share->ClassTBL = (const kclass_t**)KNH_MALLOC((CTX)ctx, sizeof(kclass_t*)*(K_CLASSTABLE_INIT));
	knh_bzero(share->ClassTBL, sizeof(kclass_t*)*(K_CLASSTABLE_INIT));
	share->sizeClassTBL = 0;
	share->capacityClassTBL  = K_CLASSTABLE_INIT;

	share->EventTBL = (knh_EventTBL_t*)KNH_ZMALLOC(SIZEOF_TEXPT(K_EVENTTBL_INIT));
	knh_bzero((void*)share->EventTBL, SIZEOF_TEXPT(K_EVENTTBL_INIT));
	share->sizeEventTBL = 0;
	share->capacityEventTBL  = K_EVENTTBL_INIT;
	kshare_loadStructData(_ctx, kapi);

	KINITv(share->funcDictSet, new_DictSet0(_ctx, 0, 0, "funcDictSet"));
	KINITv(share->constPtrMap, new_PtrMap(_ctx, 0));
	KINITv(share->inferPtrMap, new_PtrMap(_ctx, 0));
	KINITv(share->xdataPtrMap, new_PtrMap(_ctx, 0));
	KINITv(share->constPools, new_Array0(_ctx, 0));
	knh_loadSystemTypeMapRule(_ctx);
	knh_ClassTBL_setConstPool(_ctx, ClassTBL(CLASS_Int));
	knh_ClassTBL_setConstPool(_ctx, ClassTBL(CLASS_Float));
#ifdef K_USING_STRINGPOOL
	knh_ClassTBL_setConstPool(_ctx, ClassTBL(CLASS_String));
#endif
	{
		kObject *p = (kObject*)new_hObject_(_ctx, ClassTBL(CLASS_Object));
		Object_setNullObject(p, 1);
		KINITv(share->constNull, p);
	}
	{
		kBoolean *o = new_H(Boolean);
		o->n.bvalue = 1;
		KINITv(share->constTrue, o);
		o = new_H(Boolean);
		o->n.bvalue = 0;
		KINITv(share->constFalse, o);
	}
	{
		static const kdim_t dimINIT = {};
		kArray *a = new_H(Array);
		(a)->size = 0;
		(a)->dim = &dimINIT;
		KINITv(share->emptyArray, a);
	}
	share->tString = (kString**)KNH_ZMALLOC(SIZEOF_TSTRING);
	knh_bzero(share->tString, SIZEOF_TSTRING);
	knh_loadScriptSystemString(_ctx);
	KINITv(share->cwdPath, new_CurrentPath(_ctx));

	/* These are not shared, but needed to initialize System*/
	knh_stack_initexpand(_ctx, NULL, K_STACKSIZE);
	KINITv(share->packageDictMap, new_DictMap0(_ctx, 0, 1/*isCaseMap*/, "packageDictMap"));
	KINITv(share->secfileidtyDictMap, new_DictMap0(_ctx, 0, 1/*isCaseMap*/, "secfileidtyDictMap")); // added by Wakamori
	KINITv(share->classNameDictSet, new_DictSet0(_ctx, 128, 1/*isCaseMap*/, "classNameDictSet"));
	KINITv(share->eventDictSet, new_DictSet0(_ctx, 32, 1/*isCaseMap*/, "eventDictSet"));
	KINITv(share->streamDpiDictSet, new_DictSet0(_ctx, 0, 1/*isCaseMap*/, "streamDpiDictSet"));
	KINITv(share->mapDpiDictSet, new_DictSet0(_ctx, 0, 1/*isCaseMap*/, "mapDpiDictSet"));
	KINITv(share->convDpiDictSet, new_DictSet0(_ctx, 0, 1/*isCaseMap*/, "convDpiDictSet"));
	KINITv(share->rconvDpiDictSet, new_DictSet0(_ctx, 0, 1/*isCaseMap*/, "convDpiDictSet"));

	KINITv(share->enc,   new_T(knh_getSystemEncoding()));
	KINITv(share->in,    new_InputStreamStdIn(_ctx, share->enc));
	KINITv(share->out,   new_OutputStreamStdOut(_ctx, share->enc));
	KINITv(share->err,   new_OutputStreamStdErr(_ctx, share->enc));

	KINITv(share->props, new_DictMap0(_ctx, 20, 1/*isCaseMap*/, "System.props"));
	KINITv(share->symbolDictCaseSet, new_DictSet0(_ctx, K_TFIELD_SIZE + 10, 1/*isCaseMap*/, "System.symbolDictSet"));
	KINITv(share->symbolList, new_Array0(_ctx, K_TFIELD_SIZE + 10));
	KINITv(share->urnDictSet, new_DictSet0(_ctx, 0, 0/*isCaseMap*/, "System.urnDictSet"));
	KINITv(share->urns, new_Array0(_ctx, 1));
	KINITv(share->corelang, new_(Lang));
	knh_initSugarData(_ctx);
	KINITv(share->tokenDictSet, new_DictSet0(_ctx, (TT_MAX - STT_MAX), 0/*isCaseMap*/, "System.tokenDictSet"));
//	KINITv(share->URNAliasDictMap, new_DictMap0(_ctx, 0, 0/*isCaseMap*/, "System.URNAliasDictMap"));

	KINITv(share->rootks, new_(KonohaSpace));
	knh_loadScriptSystemData(_ctx, share->rootks, kapi);
	knh_System_initPath(_ctx);                 // require rootks
	KINITv(_ctx->script, new_(Script));     // require rootks
	KINITv(_ctx->gma, new_(GammaBuilder));         // require script
	knh_loadScriptSystemKonohaCode(_ctx);      // require gamma
	loadPolicy(_ctx); // added by Wakamori
	knh_loadScriptSystemMethod(_ctx, kapi);
	CommonContext_init(_ctx, ctx);
	knh_loadScriptTermData(_ctx);
	knh_loadScriptAliasTermData(_ctx);
	share->ctx0 = ctx;
	knh_GammaBuilder_init(_ctx);  // initalize gamma->gf, reported by uh
	knh_initBuiltInPackage(_ctx, knh_getLoaderAPI());

	/* CompilerAPI */
	KINITv(share->konoha_compiler, K_NULL);
	share->compilerAPI = NULL;

	share->contextCounter = 1;
	share->threadCounter = 1;
	share->stopCounter = 0;
	share->gcStopCounter = 0;
	KINITv(share->contextListNULL, new_Array0(_ctx, 4));
	kArray_add(ctx->share->contextListNULL, knh_toContext(_ctx));
#if defined(K_USING_THREAD)
	share->stop_cond = kthread_cond_init(_ctx);
	share->start_cond = kthread_cond_init(_ctx);
	share->close_cond = kthread_cond_init(_ctx);
#endif
	return ctx;
}

kcontext_t *new_ThreadContext(CTX)
{
	KNH_SYSLOCK(_ctx);
	kcontext_t *newCtx = new_hcontext(_ctx);
	newCtx->share = ctx->share;
	newCtx->stat = ctx->stat;
	newCtx->spi = ctx->spi;
	newCtx->script = ctx->script;
	newCtx->parent = WCTX(_ctx);
//	newCtx->freeObjectList = NULL;
//	newCtx->freeObjectTail = NULL;
	KINITv(newCtx->gma, new_(GammaBuilder));
	knh_GammaBuilder_init(newCtx);
	CommonContext_init(_ctx, newCtx);
	knh_stack_initexpand(newCtx, NULL, K_STACKSIZE);

	ctx->wshare->contextCounter++;
	ctx->wshare->threadCounter++;
	if(newCtx->ctxobjNC == NULL) {
		newCtx->ctxobjNC = knh_toContext(newCtx);
	}
	kArray_add(ctx->share->contextListNULL, newCtx->ctxobjNC);
	newCtx->safepoint = ctx->share->ctx0->safepoint;
	KNH_SYSUNLOCK(_ctx);
	return newCtx;
}

static int _lock(kmutex_t *m DBG_TRACE)
{
	TRACE_P("LOCK mutex=%p", m);
	return 0;
}

static int _unlock(kmutex_t *m DBG_TRACE)
{
	TRACE_P("UNLOCK mutex=%p", m);
	return 0;
}

static int thread_lock(kmutex_t *m DBG_TRACE)
{
	TRACE_P("LOCK mutex=%p", m);
	return knh_mutex_lock(m);
}

static int thread_unlock(kmutex_t *m DBG_TRACE)
{
	TRACE_P("UNLOCK mutex=%p", m);
	return knh_mutex_unlock(m);
}

static knh_iconv_t _iconv_open(const char *t, const char *f)
{
	return (knh_iconv_t)(-1);
}
static size_t _iconv(knh_iconv_t i, char **t, size_t *ts, char **f, size_t *fs)
{
	return 0;
}
static int _iconv_close(knh_iconv_t i)
{
	return 0;
}

static void _setsfp(CTX, ksfp_t *sfp, void *v)
{
	kObject *o = (kObject*)v;
	DBG_ASSERT_ISOBJECT(o);
	knh_Object_RCinc(o);
	knh_Object_RCdec(sfp[0].o);
	if(Object_isRC0(sfp[0].o)) {
		knh_Object_RCfree(_ctx, sfp[0].o);
	}
	sfp[0].o = o;
}

extern void (*knh_syslog)(int priority, const char *message, ...);
extern void (*knh_vsyslog)(int priority, const char *message, va_list args);

static void initServiceSPI(knh_ServiceSPI_t *spi)
{
	spi->syncspi = "nothread";
	spi->lockSPI = _lock;
	spi->unlockSPI = _unlock;
	spi->syslogspi = "fprintf(stderr)";
	spi->syslog = knh_syslog;  // unnecessary
	spi->vsyslog = knh_vsyslog;
	spi->iconvspi       = "noiconv";
	spi->iconv_openSPI  = _iconv_open;
	spi->iconvSPI       = _iconv;
	spi->iconv_closeSPI = _iconv_close;
	spi->mallocSPI = knh_fastmalloc;
	spi->freeSPI = knh_fastfree;
	spi->setsfpSPI = _setsfp;
	spi->closeItrSPI = knh_Iterator_close;
	spi->ntraceSPI = knh_ntrace;
	spi->pSPI = dbg_p;
}

/* ------------------------------------------------------------------------ */

void Context_initMultiThread(CTX)
{
	knh_ServiceSPI_t *spi = __CONST_CAST__(knh_ServiceSPI_t*, ctx->spi);
	spi->syncspi   = "thread";
	spi->lockSPI   = thread_lock;
	spi->unlockSPI = thread_unlock;
}
/* ------------------------------------------------------------------------ */

static kObject **share_reftrace(CTX, kshare_t *share)
{
	size_t i;
	KREFTRACEv(  share->constNull);
	KREFTRACEv(  share->constTrue);
	KREFTRACEv(  share->constFalse);
	KREFTRACEv(  share->emptyArray);
	KREFTRACEv(  share->cwdPath);

	KREFTRACEv((share->enc));
	KREFTRACEv((share->in));
	KREFTRACEv((share->out));
	KREFTRACEv((share->err));
	KREFTRACEv((share->props));
	KREFTRACEv((share->symbolDictCaseSet));
	KREFTRACEv((share->symbolList));
	KREFTRACEv((share->urnDictSet));
	KREFTRACEv((share->urns));
	KREFTRACEv((share->tokenDictSet));
	KREFTRACEv((share->corelang));
//	KREFTRACEv((share->URNAliasDictMap));

	KREFTRACEv(  share->rootks);
	KREFTRACEv(  share->funcDictSet);
	KREFTRACEv(share->sysAliasDictMap);
	KREFTRACEv(  share->constPtrMap);
	KREFTRACEv(  share->inferPtrMap);
	KREFTRACEv(  share->xdataPtrMap);
	KREFTRACEv(  share->constPools);
	KREFTRACEv(  share->packageDictMap);
	KREFTRACEv(  share->secfileidtyDictMap); // added by Wakamori
	KREFTRACEv(  share->classNameDictSet);
	KREFTRACEv(  share->eventDictSet);
	KREFTRACEv(  share->streamDpiDictSet);
	KREFTRACEv(  share->mapDpiDictSet);
	KREFTRACEv(  share->convDpiDictSet);
	KREFTRACEv(  share->rconvDpiDictSet);
	KREFTRACEn(  share->contextListNULL);

	kref_ensureSize(K_TSTRING_SIZE);
	for(i = 0; i < K_TSTRING_SIZE; i++) {
		KREFTRACEv(share->tString[i]);
	}
	kref_ensureSize(share->sizeEventTBL);
	for(i = 0; i < share->sizeEventTBL; i++) {
		if(EventTBL(i).name != NULL) {
			KREFTRACEv(EventTBL(i).name);
		}
	}
	/* tclass */
	kref_ensureSize(share->sizeClassTBL * 11);
	for(i = 0; i < share->sizeClassTBL; i++) {
		const kclass_t *ct = ClassTBL(i);
		DBG_ASSERT(ct->lname != NULL);
		KREFTRACEn(  ct->typeNULL);
		KREFTRACEv(    ct->methods);
		KREFTRACEv(    ct->typemaps);
		KREFTRACEn(  ct->cparam);
		KREFTRACEn(  ct->defnull);
		KREFTRACEn(  ct->constDictCaseMapNULL);
		KREFTRACEv(    ct->sname);
		KREFTRACEv(    ct->lname);
		if(ct->bcid == CLASS_Object && ct->cid > ct->bcid) {
			KREFTRACEv(ct->protoNULL);
		}
		KREFTRACEn(ct->constPoolMapNULL);
	}

	/* CompilerAPI */
	KREFTRACEv(share->konoha_compiler);
	return tail_;
}

static void share_free(CTX, kshare_t *share)
{
	size_t i;
	/* CompilerAPI */
	share->compilerAPI = NULL;
	KNH_FREE(_ctx, (void*)share->EventTBL, SIZEOF_TEXPT(_ctx->share->capacityEventTBL));
	share->EventTBL = NULL;
	KNH_FREE(_ctx, share->tString, SIZEOF_TSTRING);
	share->tString = NULL;
	for(i = 0; i < share->sizeClassTBL; i++) {
		kclass_t *ct = varClassTBL(i);
		if(ct->constPoolMapNULL) {
			knh_PtrMap_stat(_ctx, ct->constPoolMapNULL, S_text(ct->sname));
			ct->constPoolMapNULL = NULL;
		}
	}
	kmemshare_free(_ctx);
	/* freeing cdef */
	for(i = 0; i < share->sizeClassTBL; i++) {
		kclass_t *ct = varClassTBL(i);
		const kclass_t *supTBL = ClassTBL(ct->supcid);
		if (ct->cdef != supTBL ->cdef && ct->cdef->asize > 0) {
			DBG_P("freeing ClassDef cid=%d %s", i, ct->cdef->name);
			KNH_FREE(_ctx, (void*)ct->cdef, ct->cdef->asize);
		}
	}
	/* freeing ClassTBL->fields and ClassTBL */
	for(i = 0; i < share->sizeClassTBL; i++) {
		kclass_t *ct = varClassTBL(i);
		if(ct->fcapacity > 0) {
			KNH_FREE(_ctx, ct->fields, sizeof(kfieldinfo_t) * ct->fcapacity);
			ct->fields = NULL;
		}
		KNH_FREE(_ctx, ct, sizeof(kclass_t));
	}
	KNH_FREE(_ctx, (void*)share->ClassTBL, sizeof(kclass_t*)*(share->capacityClassTBL));
	share->ClassTBL = NULL;

	if(_ctx->stat->usedMemorySize != 0) {
		GC_LOG("memory leaking size=%ldbytes", (long)ctx->stat->usedMemorySize);
	}
	kmemlocal_free(_ctx);
	knh_mutex_free(_ctx, share->syslock);
	knh_bzero(share, sizeof(kshare_t) + sizeof(kstatinfo_t) + sizeof(knh_ServiceSPI_t));
	free(share);
}

/* ------------------------------------------------------------------------ */

kContext* knh_toContext(CTX)
{
	if(_ctx->ctxobjNC == NULL) {
		kContext *cx = new_H(Context);
		cx->ctx = (kcontext_t*)ctx;
		((kcontext_t*)ctx)->ctxobjNC = cx;
	}
	return ctx->ctxobjNC;
}

static kcontext_t* knh_getRootContext(CTX)
{
	kcontext_t *ctx0 = (kcontext_t*)ctx;
	while(_ctx0->parent != ctx0) {
		ctx0 = ctx0->parent;
	}
	return ctx0;
}

static kObject **knh_context_reftrace(CTX, kcontext_t *o)
{
	tail_ = CommonContext_reftrace(_ctx, o FTRDATA);
	if(knh_getRootContext(_ctx) == (CTX)o) {
		tail_ = share_reftrace(_ctx, (kshare_t*)o->share FTRDATA);
	}
	kref_setSize();
	return tail_;
}

void knh_Context_free(CTX, kcontext_t* ctxo)
{
	CommonContext_free(_ctx, ctxo);
	if(knh_getRootContext(_ctx) == (CTX)ctxo) {
		size_t i, j;
		for(i = 0; i < ctxo->share->sizeClassTBL; i++) {
			kclass_t *t = varClassTBL(i);
			kArray *a = t->methods;
			for(j = 0; j < kArray_size(a); j++) {
				kMethodoAbstract(_ctx, a->methods[j]);
			}
		}
		share_free(_ctx, (kshare_t*)ctxo->share);
		knh_bzero((void*)ctxo, sizeof(kcontext_t));
		free((void*)ctxo);
	}
	else {
		kmemlocal_free(_ctx);
		knh_bzero((void*)ctxo, sizeof(kcontext_t));
		KNH_FREE(_ctx, (void*)ctxo, sizeof(kcontext_t));
	}
}


/* ------------------------------------------------------------------------ */
/* [konoha api] */

/* ------------------------------------------------------------------------ */
/* [ctxkey] */


KNHAPI2(kcontext_t*) knh_getCurrentContext(void)
{
#if defined(K_USING_THREAD) && !defined(CC_T_TLS)
	kcontext_t* ctx = (kcontext_t*)kthread_getspecific(_ctxkey);
	if(_ctx == NULL) {
		ctx = curctx;
	}
#else
	kcontext_t* ctx = curctx;
#endif
	if(_ctx == NULL) {
		KNH_DIE("NOT IN KONOHA CTX");
	}
	return ctx;
}


kObject **knh_reftraceRoot(CTX)
{
	int i, size = kArray_size(_ctx->share->contextListNULL);
	for(i=0; i<size; i++) {
		kContext *ctx0 = (kContext *)kArray_n(_ctx->share->contextListNULL, i);
		tail_ = knh_context_reftrace(_ctx, (kcontext_t *)ctx0->ctx FTRDATA);
	}
	kref_setSize();
	return tail_;
}

static void check_allThreadExit(CTX)
{
	int i;
	KNH_SYSLOCK(_ctx);
	ctx->wshare->stopCounter++;
	if(_ctx->share->gcStopCounter != 0) {
		kthread_cond_signal(_ctx->share->start_cond);
	}
	if(_ctx->share->threadCounter != 1) {
		kthread_cond_wait(_ctx->share->close_cond, ctx->share->syslock);
	}
	for(i = kArray_size(_ctx->share->contextListNULL) - 1; i >= 0; i--) {
		kContext *c = (kContext *)kArray_n(_ctx->share->contextListNULL, i);
		if(c->ctx != ctx) {
			knh_Context_free(_ctx, WCTX(c->ctx));
			kArray_remove(_ctx, ctx->share->contextListNULL, i);
		}
	}
	KNH_SYSUNLOCK(_ctx);
}


/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
