/*
 * klib.c
 *
 *  Created on: Jan 28, 2012
 *      Author: kimio
 */

#include <konoha2/konoha2.h>
#include <konoha2/klib.h>
#include "konoha2/gc.h"

static void karray_init(CTX, karray_t *m, size_t max, size_t w)
{
	DBG_ASSERT(max != 0);
	m->size = 0;
	m->max  = max;
	m->body = (void*)KNH_ZMALLOC(max * w);
}

static void karray_expand(CTX, karray_t *m, size_t min, size_t w)
{
	DBG_ASSERT(m->size <= m->max);
	size_t oldsize = m->max * w, newsize = m->max * 2;
	if(min > newsize) newsize = min;
	newsize *= w;
	char *newbody = (char*)KNH_MALLOC(newsize);
	memcpy(newbody, m->body, oldsize);
	bzero(newbody + oldsize, newsize - oldsize);
	KNH_FREE(m->body, oldsize);
	m->buf = newbody;
	m->max = newsize / w;
}

static void karray_free(CTX, karray_t *m, size_t w)
{
	KNH_FREE(m->body, m->max * w);
	m->body = NULL;
	m->size = 0;
	m->max  = 0;
}

static void Kwb_init(karray_t *m, kwb_t *wb)
{
	wb->w = m;
	wb->pos = m->size;
}

static void Kwb_write(CTX, kwb_t *wb, const char *data, size_t len)
{
	karray_t *m = wb->w;
	if(!(m->size + len < m->max)) {
		karray_expand(_ctx, m, m->size + len, 1);
	}
	memcpy(m->buf + m->size, data, len);
	m->size += len;
}

static void Kwb_putc(CTX, kwb_t *wb, ...)
{
	char buf[256];
	int ch, len = 0;
	va_list ap;
	va_start(ap , wb);
	while((ch = (int)va_arg(ap, int)) != -1) {
		buf[len] = ch;
		len++;
 	}
	Kwb_write(_ctx, wb, buf, len);
	va_end(ap);
}

static void Kwb_vprintf(CTX, kwb_t *wb, const char *fmt, va_list ap)
{
	size_t s = wb->w->size;
	size_t bufsiz = wb->w->max - s;
	char *buf = wb->w->buf + s;
	va_list ap2;
	va_copy(ap2, ap);
	size_t n = vsnprintf(buf, bufsiz, fmt, ap);
	if(n >= bufsiz) {
		karray_expand(_ctx, wb->w, n + 1, 1);
		n = vsnprintf(wb->w->buf + s, bufsiz, fmt, ap2);
	}
	va_end(ap2);
	wb->w->size += n;
}

static void Kwb_printf(CTX, kwb_t *wb, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	Kwb_vprintf(_ctx, wb, fmt, ap);
	va_end(ap);
}

static const char* Kwb_top(CTX, kwb_t *wb, int ensureZero)
{
	karray_t *m = wb->w;
	if(ensureZero) {
		if(!(m->size + 1 < m->max)) {
			karray_expand(_ctx, m, m->size + 1, 1);
		}
		m->buf[m->size] = 0;
	}
	return (const char*)m->buf + wb->pos;
}

static void Kwb_free(kwb_t *wb)
{
	karray_t *m = wb->w;
	bzero(m->buf + wb->pos, m->size - wb->pos);
	m->size = wb->pos;
}

// -------------------------------------------------------------------------
// kmap_t

#define HMAP_INIT 83

static void kmap_makeFreeList(kmap_t *kmap, size_t s, size_t e)
{
	bzero(kmap->arena + s, (e - s) * sizeof(kmape_t));
	kmap->unused = kmap->arena + s;
	size_t i;
	for(i = s; i < e - 1; i++) {
		kmap->arena[i].hcode = ((uintptr_t)-1);
		kmap->arena[i].uvalue = 0;
		kmap->arena[i].next = kmap->arena + i + 1;
	}
	kmap->arena[e-1].hcode = ((uintptr_t)-1);
	kmap->arena[e-1].uvalue = 0;
	DBG_ASSERT(kmap->arena[e-1].next == NULL);
}

static void kmap_rehash(CTX, kmap_t *kmap)
{
	size_t i, newhmax = kmap->hmax * 2 + 1;
	kmape_t **newhentry = (kmape_t**)KNH_ZMALLOC(newhmax * sizeof(kmape_t*));
	for(i = 0; i < kmap->arenasize / 2; i++) {
		kmape_t *e = kmap->arena + i;
		kuint_t ni = e->hcode % newhmax;
		e->next = newhentry[ni];
		newhentry[ni] = e;
	}
	KNH_FREE(kmap->hentry, kmap->hmax * sizeof(kmape_t*));
	kmap->hentry = newhentry;
	kmap->hmax = newhmax;
}

static void kmap_shiftptr(kmap_t *kmap, intptr_t shift)
{
	size_t i, size = kmap->arenasize / 2;
	for(i = 0; i < size; i++) {
		kmape_t *e = kmap->arena + i;
		if(e->next != NULL) {
			e->next = (kmape_t*)(((char*)e->next) + shift);
			DBG_ASSERT(kmap->arena <= e->next && e->next < kmap->arena + size);
		}
	}
}

static kmape_t *Kmap_newentry(CTX, kmap_t *kmap, kuint_t hcode)
{
	kmape_t *e;
	if(kmap->unused == NULL) {
		size_t oarenasize = kmap->arenasize;
		char *oarena = (char*)kmap->arena;
		kmap->arenasize *= 2;
		kmap->arena = KNH_MALLOC(kmap->arenasize * sizeof(kmape_t));
		kmap_shiftptr(kmap, (char*)kmap->arena - oarena);
		kmap_makeFreeList(kmap, oarenasize, kmap->arenasize);
		KNH_FREE(oarena, oarenasize * sizeof(kmape_t));
		kmap_rehash(_ctx, kmap);
	}
	e = kmap->unused;
	kmap->unused = e->next;
	e->hcode = hcode;
	e->next = NULL;
	kmap->size++;
	return e;
}

static kmap_t *Kmap_init(CTX, size_t init)
{
	kmap_t *kmap = (kmap_t*)KNH_ZMALLOC(sizeof(kmap_t));
	if(init < HMAP_INIT) init = HMAP_INIT;
	kmap->arenasize = (init * 3) / 4;
	kmap->arena = (kmape_t*)KNH_MALLOC(kmap->arenasize * sizeof(kmape_t));
	kmap_makeFreeList(kmap, 0, kmap->arenasize);
	kmap->hentry = (kmape_t**)KNH_ZMALLOC(init * sizeof(kmape_t*));
	kmap->hmax = init;
	kmap->size = 0;
	return (kmap_t*)kmap;
}

static void Kmap_reftrace(CTX, kmap_t *kmap, void (*f)(CTX, kmape_t *))
{
	size_t i;
	for(i = 0; i < kmap->hmax; i++) {
		kmape_t *e = kmap->hentry[i];
		while(e != NULL) {
			f(_ctx, e);
			e = e->next;
		}
	}
}

static void Kmap_free(CTX, kmap_t *kmap, void (*f)(CTX, void *))
{
	if(f != NULL) {
		size_t i;
		for(i = 0; i < kmap->hmax; i++) {
			kmape_t *e = kmap->hentry[i];
			while(e != NULL) {
				f(_ctx, e->pvalue);
				e = e->next;
			}
		}
	}
	KNH_FREE(kmap->arena, sizeof(kmape_t)*(kmap->arenasize));
	KNH_FREE(kmap->hentry, sizeof(kmape_t*)*(kmap->hmax));
	KNH_FREE(kmap, sizeof(kmap_t));
}

static kmape_t *Kmap_getentry(kmap_t* kmap, kuint_t hcode)
{
	kmape_t **hlist = kmap->hentry;
	size_t idx = hcode % kmap->hmax;
	kmape_t *e = hlist[idx];
	while(e != NULL) {
		if(e->hcode == hcode) return e;
		e = e->next;
	}
	return NULL;
}

static void Kmap_add(kmap_t* kmap, kmape_t *ne)
{
	DBG_ASSERT(ne->next == NULL);
	kmape_t **hlist = kmap->hentry;
	size_t idx = ne->hcode % kmap->hmax;
	ne->next = hlist[idx];
	hlist[idx] = ne;
}

static void kmap_unuse(kmap_t *kmap, kmape_t *e)
{
	e->next = kmap->unused;
	kmap->unused = e;
	e->hcode = ((uintptr_t)-1);
	e->uvalue  = 0;
	kmap->size--;
}

static void Kmap_remove(kmap_t* kmap, kmape_t *oe)
{
	kmape_t **hlist = kmap->hentry;
	size_t idx = oe->hcode % kmap->hmax;
	kmape_t *e = hlist[idx];
	while(e != NULL) {
		if(e->next == oe) {
			e->next = oe->next;
			kmap_unuse(kmap, oe);
			return;
		}
		e = e->next;
	}
	hlist[idx] = oe->next;
	kmap_unuse(kmap, oe);
}

static const char* KTsymbol(CTX, char *buf, size_t bufsiz, ksymbol_t sym)
{
	int index = MN_UNMASK(sym);
	if(MN_isTOCID(sym)) {
		snprintf(buf, bufsiz, "to%s", T_cid(index));
	}
	else if(index < kArray_size(_ctx->share->symbolList)) {
		const char *name = S_text(_ctx->share->symbolList->strings[index]);
		if(MN_isISBOOL(sym)) {
			snprintf(buf, bufsiz, "is%s", name);
			buf[2] = toupper(buf[2]);
		}
		else if(MN_isGETTER(sym)) {
			snprintf(buf, bufsiz, "get%s", name);
			buf[3] = toupper(buf[3]);
		}
		else if(MN_isSETTER(sym)) {
			snprintf(buf, bufsiz, "set%s", name);
			buf[3] = toupper(buf[3]);
		}
		else {
			snprintf(buf, bufsiz, "%s", name);
		}
	}
	else {
		snprintf(buf, bufsiz, "unknown symbol=%d !< %ld", index, kArray_size(_ctx->share->symbolList));
	}
	return (const char*)buf;
}

// -------------------------------------------------------------------------

/* debug mode */
int konoha_debug;
#define CTX_isTERM()     CTX_isInteractive()

static const char* T_BEGIN(CTX, int pe)
{
	if(CTX_isTERM()) {
		switch(pe) {
			case CRIT_:
			case ERR_/*ERROR*/: return "\x1b[1m\x1b[31m";
			case WARN_/*WARNING*/: return "\x1b[1m\x1b[31m";
			case INFO_/*INFO*/: return "\x1b[1m";
			case PRINT_/*INFO*/: return "\x1b[1m";
			default:/*DEBUG*/ return "";
		}
	}
	return "";
}

static const char* T_END(CTX, int pe)
{
	return CTX_isTERM() ? "\x1b[0m" : "";
}

static void Kreport(CTX, int level, const char *msg)
{
	fflush(stdout);
	fputs(T_BEGIN(_ctx, level), stdout);
	fputs(" - ", stdout);
	fputs(msg, stdout);
	fputs(T_END(_ctx, level), stdout);
	fputs("\n", stdout);
}

static const char *T_ERR(int level)
{
	switch(level) {
		case CRIT_:
		case ERR_/*ERROR*/: return "(error) ";
		case WARN_/*WARNING*/: return "(warning) ";
		case INFO_/*INFO, NOTICE*/: return "(info) ";
		case PRINT_: return "";
		default/*DEBUG*/: return "(debug) ";
	}
}

static void Kreportf(CTX, int level, kline_t pline, const char *fmt, ...)
{
	if(level == DEBUG_ && !konoha_debug) return;
	va_list ap;
	va_start(ap , fmt);
	fflush(stdout);
	fputs(T_BEGIN(_ctx, level), stdout);
	if(pline != 0) {
		const char *file = S_text(S_uri(pline));
		fprintf(stdout, " - (%s:%d) %s" , filename(file), (kushort_t)pline, T_ERR(level));
	}
	else {
		fprintf(stdout, " - %s" , T_ERR(level));
	}
	vfprintf(stdout, fmt, ap);
	fputs(T_END(_ctx, level), stdout);
	fprintf(stdout, "\n");
	va_end(ap);
}


// -------------------------------------------------------------------------
static void Kdbg_p(const char *file, const char *func, int line, const char *fmt, ...)
{
	if(konoha_debug) {
		va_list ap;
		va_start(ap , fmt);
		fflush(stdout);
		fprintf(stderr, "DEBUG(%s:%d) ", func, line);
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
		va_end(ap);
	}
}

// -------------------------------------------------------------------------

void klib2_init(klib2_t *l)
{
	l->Karray_init   = karray_init;
	l->Karray_expand = karray_expand;
	l->Karray_free   = karray_free;
	l->Kwb_init      = Kwb_init;
	l->Kwb_write     = Kwb_write;
	l->Kwb_putc      = Kwb_putc;
	l->Kwb_vprintf   = Kwb_vprintf;
	l->Kwb_printf    = Kwb_printf;
	l->Kwb_top       = Kwb_top;
	l->Kwb_free      = Kwb_free;
	l->Kmap_init     = Kmap_init;
	l->Kmap_free     = Kmap_free;
	l->Kmap_reftrace = Kmap_reftrace;
	l->Kmap_newentry = Kmap_newentry;
	l->Kmap_get      = Kmap_getentry;
	l->Kmap_add      = Kmap_add;
	l->Kmap_remove   = Kmap_remove;
	l->KTsymbol      = KTsymbol;
	l->Kreport       = Kreport;
	l->Kreportf      = Kreportf;
	l->Kp            = Kdbg_p;
}
