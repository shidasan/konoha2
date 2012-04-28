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

#include "commons.h"

int verbose_gc = 0;

#ifdef K_USING_RCGC
#include "rcgc.h"
#elif defined(K_USING_GENGC)
#include "gbmgc.h"
#else
#include "bmgc.h"
#endif

/* ------------------------------------------------------------------------ */
/* [klocalgc] */

//static void klocalgc_reftrace(CTX, struct kmodlocal_t *baseh)
//{
////	klocalgc_t *base = (klocalgc_t*)baseh;
//}
//static void klocalgc_free(CTX, struct kmodlocal_t *baseh)
//{
////	klocalgc_t *base = (klocalgc_t*)baseh;
////	KFREE(base, sizeof(klocalgc_t));
//}

static void kmodgc_setup(CTX, struct kmodshare_t *def, int newctx)
{
//	if(_ctx->mod[MOD_gc] == NULL) {
//		klocalgc_t *base = (klocalgc_t*)KCALLOC(sizeof(klocalgc_t));
//		base->h.reftrace = klocalgc_reftrace;
//		base->h.free     = klocalgc_free;
//		_ctx->mod[MOD_gc] = (kmodlocal_t*)base;
//	}
}

static void kmodgc_reftrace(CTX, struct kmodshare_t *baseh)
{
}

static void kmodgc_free(CTX, struct kmodshare_t *baseh)
{
	KFREE(baseh, sizeof(kmodshare_t));
}

typedef struct kmodgc_t {
	kmodshare_t h;
} kmodgc_t;

void MODGC_init2(CTX, kcontext_t *ctx)
{
	kmodgc_t *base = (kmodgc_t*)KCALLOC(sizeof(kmodgc_t), 1);
	base->h.name     = "bmgc";
	base->h.setup    = kmodgc_setup;
	base->h.reftrace = kmodgc_reftrace;
	base->h.free     = kmodgc_free;
//
//	klib2_t *l = ctx->lib2;
//	l->KKonohaSpace_getCT   = KonohaSpace_getCT;
//	l->KloadMethodData = KonohaSpace_loadMethodData;
//	l->KloadConstData  = KonohaSpace_loadConstData;
//	l->Kraise = Kraise;
}
