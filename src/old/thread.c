/****************************************************************************
 * KONOHA2 COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
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


#include"commons.h"

#ifdef K_USING_PTHREAD
#include<pthread.h>
#endif

#ifdef K_USING_BTRON
#include<btron/proctask.h>
#include<btron/taskcomm.h>
#endif

#ifdef K_USED_TO_BE_ON_LKM
#include <linux/mutex.h>
#endif

#ifdef K_USING_WINTHREAD_
#include <process.h>
#endif

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [TLS] */

kthread_t kthread_self(void)
{
#if defined(K_USING_PTHREAD)
	return (kthread_t)pthread_self();
#elif defined(K_USING_BTRON)
	return b_get_tid();
#elif defined(K_USING_WINTHREAD_)
	return GetCurrentThread();
#else
	return 0;
#endif
}

/* ------------------------------------------------------------------------ */

#ifdef K_USING_BTRON
typedef struct {
	void* (*func)(void*);
	void* arg;
} kthread_target_btron;

static void kthread_btronEntryPoint(kthread_target_btron* arg)
{
	kthread_target_btron target = *arg;
	free(arg);

	// FIXME: return value is ignored
	target.func(target.arg);

	// BTRON threads must terminate with b_ext_tsk;
	//       that's why we need this stub function
	b_ext_tsk();
}
#endif /* K_USING_BTRON */


/* ------------------------------------------------------------------------ */

int kthread_create(CTX, kthread_t *thread, void *attr, knh_Fthread fgo, void * arg)
{
#if defined(K_USING_PTHREAD)
	return pthread_create((pthread_t*)thread, (pthread_attr_t*)attr, fgo, arg);
#elif defined(K_USING_BTRON)
	// FIXME: attr is ignored
	W err;
	kthread_target_btron* target =
		(kthread_target_btron*)malloc(sizeof(kthread_target_btron));
	if (target == NULL) {
		return -1;
	}
	target->func = fgo;
	target->arg = arg;
	err = b_cre_tsk((FP)kthread_btronEntryPoint, -1, (W)target);
	if (err < 0) {
		free(target);
		return -1;
	}
	*thread = err;
	return 0;
#elif defined(K_USING_WINTHREAD_)
	return _beginthread((void (__cdecl *)(void *))fgo, 0, arg);
#else
	return -1;
#endif
}

/* ------------------------------------------------------------------------ */

int kthread_detach(CTX, kthread_t th)
{
#if defined(K_USING_PTHREAD)
	return pthread_detach((pthread_t)th);
#elif defined(K_USING_WINTHREAD_)
	TODO();
	return 0;
#else
	return 0;
#endif
}

/* ------------------------------------------------------------------------ */

int kthread_join(CTX, kthread_t thread, void **ret)
{
#if defined(K_USING_PTHREAD)
	return pthread_join((pthread_t)thread, ret);
#elif defined(K_USING_WINTHREAD_)
	return WaitForSingleObject((HANDLE)thread, INFINITE);
#else
	return -1;
#endif
}

/* ------------------------------------------------------------------------ */
/* @data */

typedef struct kthreadcc_t {
	CTX;
	ksfp_t *sfp;
} kthreadcc_t ;

//static void *threading(void *p)
//{
//	kthreadcc_t ta = *((kthreadcc_t*)p);
//	CTX = new_ThreadContext(ta.ctx);
//
//	knh_beginContext(_ctx);
//	ksfp_t *lsfp = ctx->stack;
//
//	KSETv(lsfp[0].o, new_ExceptionHandler(_ctx));
//	KNH_TRY(_ctx, L_CATCH, lsfp, 0);
//	{
//		kMethod *mtd = ta.sfp[0].mtd;
//		DBG_ASSERT(IS_Method(mtd));
//		KSETv(lsfp[1].o, mtd);
//		KSETv(lsfp[2].o, ta.sfp[1].o);
//		lsfp[2].ndata = knh_Object_data(ta.sfp[1].o);
//		{
//			int i, args = knh_Method_psize(mtd);
//			for(i = 0; i < args; i++) {
//				KSETv(lsfp[3+i].o, ta.sfp[2+i].o);
//				lsfp[3+i].ndata = knh_Object_data(ta.sfp[2+i].o);
//			}
//			KNH_SCALL(_ctx, lsfp, 1, mtd, args);
//		}
//		goto L_FINALLY;
//	}
//	/* catch */
//L_CATCH:;
//		KNH_PRINT_STACKTRACE(_ctx, lsfp, 0);
//
//L_FINALLY:
//		knh_Context_clearstack(_ctx);
//		KONOHA_END(_ctx);
//		knh_ThreadContext_dispose(_ctx);
//		return NULL;
//}

/* ------------------------------------------------------------------------ */
// sfp |   0   |   1   |   2   |   3  |
//     |  self |  mtd  | arg1  | ...  |

//void knh_stack_threadRun(CTX, ksfp_t *sfp _RIX)
//{
//	kthread_t th;
//	kthreadcc_t ta = {ctx, sfp + 1};
//	kthread_create(_ctx, &th, NULL, threading, (void*)&ta);
//	//kthread_detach(_ctx, th);
//}

/* ------------------------------------------------------------------------ */
/* [mutex] */

//create mutex 	pthread_mutex_init 	CreateMutex
//remove mutex lock 	pthread_mutex_lock 	WaitForSingleObject
//release mutex lock 	pthread_mutex_unlock 	ReleaseMutex
//destroy mutex 	pthread_mutex_destroy 	CloseHandle

int knh_mutex_init(kmutex_t *m)
{
#if defined(K_USING_PTHREAD)
	return pthread_mutex_init((pthread_mutex_t*)m, NULL);
#elif defined(K_USING_BTRON)
	W sem = b_cre_sem(1, SEM_EXCL|DELEXIT);
	if (sem < 0) {
		return -1;
	}
	*m = sem;
	return 0;
#elif defined(K_USED_TO_BE_ON_LKM)
	mutex_init((struct mutex *)m);
	return 0;
#elif defined(K_USING_WINTHREAD_)
	InitializeCriticalSection(m);
	// TODO STATUS_NO_MEMORY
	return 0;
#else
	return 0;
#endif
}

/* ------------------------------------------------------------------------ */

int knh_mutex_lock(kmutex_t *m)
{
	//DBG_P("locking %p", m);
#if defined(K_USING_PTHREAD)
	int ret = pthread_mutex_lock((pthread_mutex_t*)m);
	return ret;
#elif defined(K_USING_BTRON)
	W err = b_wai_sem(*m, T_FOREVER);
	if (err < 0) {
		return -1;
	}
	return 0;
#elif defined(K_USED_TO_BE_ON_LKM)
	mutex_lock((struct mutex *) m);
	return 0;
#elif defined(K_USING_WINTHREAD_)
	EnterCriticalSection(m);
	return 0;
#else
	return 0;
#endif
}

/* ------------------------------------------------------------------------ */

int kmutex_trylock(kmutex_t *m)
{
	//DBG_P("trylock %p", m);
#if defined(K_USING_PTHREAD)
	return pthread_mutex_trylock((pthread_mutex_t*)m);
#elif defined(K_USED_TO_BE_ON_LKM)
	return mutex_trylock((struct mutex *) m);
#elif defined(K_USING_WINTHREAD_)
	return TryEnterCriticalSection(m);
#else
	return 0;
#endif
}

/* ------------------------------------------------------------------------ */

int knh_mutex_unlock(kmutex_t *m)
{
	//DBG_P("unlocking %p", m);
#if defined(K_USING_PTHREAD)
	int ret = pthread_mutex_unlock((pthread_mutex_t*)m);
	return ret;
#elif defined(K_USING_BTRON)
	W err = b_sig_sem(*m);
	if (err < 0) {
		return -1;
	}
	return 0;
#elif defined(K_USED_TO_BE_ON_LKM)
	mutex_unlock((struct mutex *) m);
	return 0;
#elif defined(K_USING_WINTHREAD_)
	LeaveCriticalSection(m);
	return 0;
#else
	return 0;
#endif
}

/* ------------------------------------------------------------------------ */

int knh_mutex_destroy(kmutex_t *m)
{
	//DBG_P("destroying %p", m);
#if defined(K_USING_PTHREAD)
	return pthread_mutex_destroy((pthread_mutex_t*)m);
#elif defined(K_USING_BTRON)
	W err = b_del_sem(*m);
	if (err < 0) {
		return -1;
	}
	return 0;
#elif defined(K_USED_TO_BE_ON_LKM)
	return 0;
#elif defined(K_USING_WINTHREAD_)
	DeleteCriticalSection(m);
	return 0;
#else
	return 0;
#endif
}


kmutex_t *knh_mutex_malloc(CTX)
{
	kmutex_t *m = (kmutex_t*)malloc(sizeof(kmutex_t));
	knh_bzero(m, sizeof(kmutex_t));
	if(knh_mutex_init(m) != 0) {
		KNH_NTRACE2(_ctx, "mutex_init", K_PERROR, KNH_LDATA(LOG_p("mutex", m)));
	}
	return m;
}

void knh_mutex_free(CTX, kmutex_t *m)
{
	if(knh_mutex_destroy(m) != 0) {
		KNH_NTRACE2(_ctx, "mutex_destroy", K_PERROR, KNH_LDATA(LOG_p("mutex", m)));
	}
	free(m);
}

#if defined(K_USING_PTHREAD)

knh_cond_t *kthread_cond_init(CTX)
{
	pthread_cond_t *c = (pthread_cond_t *)KNH_ZMALLOC(sizeof(pthread_cond_t));
	pthread_cond_init(c, NULL);
	return (knh_cond_t *)c;
}

int kthread_cond_wait(knh_cond_t *cond, kmutex_t *m)
{
	return pthread_cond_wait((pthread_cond_t *)cond, (pthread_mutex_t *)m);
}

int kthread_cond_signal(knh_cond_t *cond)
{
	return pthread_cond_signal((pthread_cond_t *)cond);
}

int kthread_cond_broadcast(knh_cond_t *cond)
{
	return pthread_cond_broadcast((pthread_cond_t *)cond);
}

#elif defined(K_USING_WINTHREAD_)

// reference URL: http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

knh_cond_t *kthread_cond_init(CTX)
{
	knh_cond_t *c = (knh_cond_t *)KNH_ZMALLOC(sizeof(knh_cond_t));
	// Create an auto-reset event.
	c->events_[E_SIGNAL] = CreateEvent(NULL,  // no secfileidty
			FALSE, // auto-reset event
			FALSE, // non-signaled initially
			NULL); // unnamed
	// Create a manual-reset event.
	c->events_[E_BROADCAST] = CreateEvent(NULL,  // no secfileidty
			TRUE,  // manual-reset
			FALSE, // non-signaled initially
			NULL); // unnamed
	return c;
}

int kthread_cond_wait(knh_cond_t *cond, kmutex_t *m)
{
	// Release the <external_mutex> here and wait for either event
	// to become signaled, due to <pthread_cond_signal> being
	// called or <pthread_cond_broadcast> being called.
	LeaveCriticalSection(m);
	WaitForMultipleObjects(2, // Wait on both <events_>
			cond->events_,
			FALSE, // Wait for either event to be signaled
			INFINITE); // Wait "forever"
	// Reacquire the mutex before returning.
	EnterCriticalSection(m);
	return 0;
}

int kthread_cond_signal(knh_cond_t *cond)
{
	// Try to release one waiting thread.
	return PulseEvent(cond->events_[E_SIGNAL]);
}

int kthread_cond_broadcast(knh_cond_t *cond)
{
	// Try to release all waiting threads.
	return PulseEvent(cond->events_[E_BROADCAST]);
}

#endif /* defined(K_USING_WINTHREAD_) */

/* ------------------------------------------------------------------------ */
/* [TLS] */

#ifndef CC_T_TLS
#if defined(K_USING_PTHREAD)
static void destr(void *data)
{
	DBG_P("destruction data=%p", data);
}
#endif

/* ------------------------------------------------------------------------ */

int kthread_key_create(kthread_key_t *key)
{
#if defined(K_USING_PTHREAD)
	return pthread_key_create((pthread_key_t*)key, destr);
#elif defined(K_USING_WINTHREAD_)
	DWORD v = TlsAlloc();
	*key = (kthread_key_t)v;
	return (int)v;
#else
	return -1;
#endif
}

/* ------------------------------------------------------------------------ */

int thread_setspecific(kthread_key_t key, const void *data)
{
#if defined(K_USING_PTHREAD)
	return pthread_setspecific(key, data);
#elif defined(K_USING_WINTHREAD_)
	return TlsSetValue((DWORD)key, (LPVOID) data);
#else
	return -1;
#endif
}

/* ------------------------------------------------------------------------ */

void* kthread_getspecific(kthread_key_t key)
{
#if defined(K_USING_PTHREAD)
	return pthread_getspecific(key);
#elif defined(K_USING_WINTHREAD_)
	return (void*)TlsGetValue((DWORD)key);
#else
	return NULL;
#endif
}

/* ------------------------------------------------------------------------ */

int kthread_key_delete(kthread_key_t key)
{
#if defined(K_USING_PTHREAD)
	return pthread_key_delete(key);
#elif defined(K_USING_WINTHREAD_)
	return TlsFree((DWORD) key);
#else
	return -1;
#endif
}
#endif


/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
