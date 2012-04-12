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

#include "commons.h"

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
////	KNH_FREE(base, sizeof(klocalgc_t));
//}

static void kmodgc_setup(CTX, struct kmodshare_t *def, int newctx)
{
//	if(_ctx->mod[MOD_gc] == NULL) {
//		klocalgc_t *base = (klocalgc_t*)KNH_ZMALLOC(sizeof(klocalgc_t));
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
	KNH_FREE(baseh, sizeof(kmodshare_t));
}

typedef struct kmodgc_t {
	kmodshare_t h;
} kmodgc_t;

void MODGC_init2(CTX, kcontext_t *ctx)
{
	kmodgc_t *base = (kmodgc_t*)KNH_ZMALLOC(sizeof(kmodgc_t));
	base->h.name     = "bmgc";
	base->h.setup    = kmodgc_setup;
	base->h.reftrace = kmodgc_reftrace;
	base->h.free     = kmodgc_free;
//
//	klib2_t *l = ctx->lib2;
//	l->KKonohaSpace_getcid   = KonohaSpace_getcid;
//	l->KloadMethodData = KonohaSpace_loadMethodData;
//	l->KloadConstData  = KonohaSpace_loadConstData;
//	l->Kraise = Kraise;
}

//---------------------------------------------------------------------------


///* ------------------------------------------------------------------------ */
///* [logger] */
//
//typedef struct  {
//	kmodlocal_t h;
//	karray_t logbuf;
//
//} ctxlogger_t;
//
//typedef struct  {
//	kmodshare_t h;
//} kmodlogger_t;
//
//#define ctxlogger    ((ctxlogger_t*)_ctx->modlocal[MOD_logger])
//#define kmodlogger  ((kmodlogger_t*)_ctx->modshare[MOD_logger])
//
//static char *write_byte_toebuf(const char *text, size_t len, char *p, char *ebuf)
//{
//	if(ebuf - p > len) {
//		memcpy(p, text, len);
//		return p+len;
//	}
//	return p;
//}
//
//static char *write_text_tobuf(const char *s, char *p, char *ebuf)
//{
//	if(p < ebuf) { p[0] = '"'; p++; }
//	while(*s != 0 && p < ebuf) {
//		if(*s == '"') {
//			p[0] = '\"'; p++;
//			if(p < ebuf) {p[0] = s[0]; p++;}
//		}
//		else if(*s == '\n') {
//			p[0] = '\\'; p++;
//			if(p < ebuf) {p[0] = 'n'; p++;}
//		}
//		else {
//			p[0] = s[0]; p++;
//		}
//		s++;
//	}
//	if(p < ebuf) { p[0] = '"'; p++; }
//	return p;
//}
//
//static void reverse(char *const start, char *const end, const int len)
//{
//	int i, l = len / 2;
//	register char *s = start;
//	register char *e = end - 1;
//	for (i = 0; i < l; i++) {
//		char tmp = *s;
//		tmp  = *s;
//		*s++ = *e;
//		*e-- = tmp;
//	}
//}
//
//static char *write_uint_toebuf(uintptr_t uvalue, char *const p, const char *const end)
//{
//	int i = 0;
//	while (p + i < end) {
//		int tmp = uvalue % 10;
//		uvalue /= 10;
//		p[i] = '0' + tmp;
//		++i;
//		if (uvalue == 0)
//			break;
//	}
//	reverse(p, p + i, i);
//	return p + i;
//}
//
//#define EBUFSIZ 1024
//
//static void Ktrace(CTX, int pe, void *cstack, ...)
//{
//	va_list ap;
//	va_start(ap, cstack);
//	char buf[EBUFSIZ], *p = buf, *ebuf =  p + (EBUFSIZ - 4);
//	p[0] = '{'; p++;
//	{
//		int c = 0, logtype;
//		while((logtype = va_arg(ap, int)) != 0) {
//			const char *key = va_arg(ap, const char*);
//			if(c > 0 && p + 2 < ebuf) { p[0] = ','; p[1] = ' '; p+=2; }
//			p = write_text_tobuf(key, p, ebuf);
//			if(p + 2 < ebuf) { p[0] = ':'; p[1] = ' '; p+=2; }
//			switch(logtype) {
//			case LOG_u: {
//				p = write_uint_toebuf(va_arg(ap, uintptr_t), p, ebuf);
//				break;
//			}
//			case LOG_s: {
//				const char *text = va_arg(ap, const char*);
//				p = write_text_toebuf(text, p, ebuf);
//				break;
//			}
//			default:
//				if(p + 4 < ebuf) { p[0] = 'n'; p[1] = 'u'; p[2] = 'l'; p[3] = 'l'; p+=4; }
//			}
//			c++;
//		}
//	}
//	p[0] = '}'; p++;
//	p[0] = '0';
//	va_end(ap);
//	fprintf(stderr, "%s\n", buf);
//}
//
//static void ctxlogger_reftrace(CTX, struct kmodlocal_t *baseh)
//{
//	ctxlogger_t *base = (ctxlogger_t*)baseh;
//}
//static void ctxlogger_free(CTX, struct kmodlocal_t *baseh)
//{
//	ctxlogger_t *base = (ctxlogger_t*)baseh;
//	KNH_FREE(base, sizeof(ctxlogger_t));
//}
//
//static void kmodlogger_setup(CTX, struct kmodshare_t *def, int newctx)
//{
//	if(_ctx->modlocal[MOD_logger] == NULL) {
//		ctxlogger_t *base = (ctxlogger_t*)KNH_ZMALLOC(sizeof(klocallogger_t));
//		base->h.reftrace = klocallogger_reftrace;
//		base->h.free     = klocallogger_free;
//		_ctx->modlocal[MOD_logger] = (kmodlocal_t*)base;
//	}
//}
//
//static void kmodlogger_reftrace(CTX, struct kmodshare_t *baseh)
//{
//}
//
//static void kmodlogger_free(CTX, struct kmodshare_t *baseh)
//{
//	KNH_FREE(baseh, sizeof(kmodshare_t));
//}
//
//
//
//void MODLOGGER_init(CTX, kcontext_t *ctx)
//{
//	kmodlogger_t *base = (kmodlogger_t*)KNH_ZMALLOC(sizeof(kmodlogger_t));
//	base->h.name     = "syslog";
//	base->h.setup    = kmodlogger_setup;
//	base->h.reftrace = kmodlogger_reftrace;
//	base->h.free     = kmodlogger_free;
//	ksetModule(MOD_logger, (kmodshare_t*)base, 0);
//
////
////	klib2_t *l = ctx->lib2;
////	l->KKonohaSpace_getcid   = KonohaSpace_getcid;
////	l->KloadMethodData = KonohaSpace_loadMethodData;
////	l->KloadConstData  = KonohaSpace_loadConstData;
////	l->Kraise = Kraise;
//}

