/*
 * klibexec.h
 *
 *  Created on: Jan 28, 2012
 *      Author: kimio
 */


static void karray_init(CTX, karray_t *m, size_t max, size_t w)
{
	DBG_ASSERT(max != 0);
	m->size = 0;
	m->max  = max;
	m->body = (void*)KNH_ZMALLOC(max * w);
}

static void karray_resize(CTX, karray_t *m, size_t newitems, size_t w)
{
	size_t oldsize = m->max * w;
	size_t newsize = newitems * w;
	char *newbody = (char*)KNH_MALLOC(newsize);
	if(oldsize < newsize) {
		memcpy(newbody, m->body, oldsize);
		bzero(newbody + oldsize, newsize - oldsize);
	}
	else {
		memcpy(newbody, m->body, newsize);
	}
	KNH_FREE(m->body, oldsize);
	m->buf = newbody;
	m->max = newitems;
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

// key management

static void map_addStringUnboxValue(CTX, kmap_t *kmp, uintptr_t hcode, kString *skey, uintptr_t uvalue)
{
	kmape_t *e = kmap_newentry(kmp, hcode);
	KINITv(e->skey, skey);
	e->uvalue = uvalue;
	kmap_add(kmp, e);
}

static ksymbol_t KMAP_getcode(CTX, kmap_t *kmp, kArray *list, const char *name, size_t len, uintptr_t hcode, ksymbol_t def)
{
	kmape_t *e = kmap_get(kmp, hcode);
	while(e != NULL) {
		if(e->hcode == hcode && len == S_size(e->skey) && strncmp(S_text(e->skey), name, len) == 0) {
			return (ksymbol_t)e->uvalue;
		}
		e = e->next;
	}
	if(def == FN_NEWID) {
		kString *skey = new_kString(name, len, SPOL_ASCII|SPOL_POOL);
		uintptr_t sym = kArray_size(list);
		kArray_add(list, skey);
		map_addStringUnboxValue(_ctx, kmp, hcode, skey, sym);
		return (ksymbol_t)sym;
	}
	return def;
}

static kline_t Kfileid(CTX, const char *name, size_t len, ksymbol_t def)
{
	uintptr_t hcode = strhash(name, len);
	kline_t uline = KMAP_getcode(_ctx, _ctx->share->fileidMapNN, _ctx->share->fileidList, name, len, hcode, def);
	return uline << (sizeof(kshort_t) * 8);
}

static kpack_t Kpack(CTX, const char *name, size_t len, ksymbol_t def)
{
	uintptr_t hcode = strhash(name, len);
	return KMAP_getcode(_ctx, _ctx->share->packMapNN, _ctx->share->packList, name, len, hcode, def);
}

static kpack_t Kuname(CTX, const char *name, size_t len, ksymbol_t def)
{
	uintptr_t hcode = strhash(name, len);
	return KMAP_getcode(_ctx, _ctx->share->unameMapNN, _ctx->share->unameList, name, len, hcode, def);
}

//static const char* uname_norm(char *buf, size_t bufsiz, const char *t, size_t len, uintptr_t *hcodeR)
//{
//	uintptr_t i, hcode = 0;
//	char *p = buf;
//	for(i = 0; i < len; i++) {
//		if(t[i]==0) break;
//		int ch = toupper(t[i]);     // Int_MAX and INT_MAX are same
//		if(ch == '_') continue;   // INT_MAX and INTMAX  are same
//		*p = ch; p++;
//		hcode = ch + (31 * hcode);
//	}
//	*p=0;
//	*hcodeR=hcode;
//	return (const char*)buf;
//}
//
//static ksymbol_t Kuname(CTX, const char *name, size_t len)
//{
//	uintptr_t hcode = 0;
//	char buf[len+1];
//	name = uname_norm(buf, len+1, name, len, &hcode);
//	len = strlen(name);
//	return KMAP_getcode(_ctx, _ctx->share->unameMapNN, _ctx->share->unameList, name, len, hcode, FN_NEWID);
//}

static const char* ksymbol_norm(char *buf, const char *t, size_t *lenR, uintptr_t *hcodeR, ksymbol_t *mask, int pol)
{
	int i, toup = 0, len = (*lenR > 128) ? 128 : *lenR;
	char *w = buf;
	for(i = 0; i < len; i++) {
		int ch = t[i];
		if(ch == 0) break;
		if(ch == '_') {
			if(toup > 0) toup = 2;
			continue;
		}
		ch = (toup == 2) ? toupper(ch) : ch;
		toup = 1;
		*w = ch; w++;
	}
	*w = 0;
	*lenR = w - buf;
	t = buf;
	if(pol == SYMPOL_METHOD) {
		if(buf[1] == 'e' && buf[2] == 't') {
			if(buf[0] == 'g') {
				*lenR -= 3; *mask = MN_GETTER;
				t = buf + 3;
			}
			else if(buf[0] == 's') {
				*lenR -= 3; *mask = MN_SETTER;
				t = buf + 3;
			}
		}
		else if(buf[0] == 'i' && buf[1] == 's') {
			*lenR -= 2; *mask = MN_ISBOOL;
			t = buf + 2;
		}
	}
	w = (char*)t;
	uintptr_t hcode = 0;
	while(*w != 0) {
		int ch = *w;
		hcode = tolower(ch) + (31 * hcode);
		w++;
	}
	*hcodeR = hcode;
	return t;
}

static ksymbol_t Ksymbol(CTX, const char *name, size_t len, ksymbol_t def, int pol)
{
	uintptr_t hcode = 0;
	if(pol == SYMPOL_RAW) {
		size_t i;
		for(i = 0; i < len; i++) {
			int ch = name[i];
			if(ch == 0) {
				len = i; break;
			}
			hcode = ch + (31 * hcode);
		}
		return KMAP_getcode(_ctx, _ctx->share->symbolMapNN, _ctx->share->symbolList, name, len, hcode, def);
	}
	else {
		char buf[256];
		ksymbol_t sym, mask = 0;
		name = ksymbol_norm(buf, name, &len, &hcode, &mask, pol);
		sym = KMAP_getcode(_ctx, _ctx->share->symbolMapNN, _ctx->share->symbolList, name, len, hcode, def);
		if(def == sym) return def;
		return sym | mask;
	}
}

// -------------------

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

// library

#define KPROMAP_DELTA 3

static inline kvsarray_t * kpromap_null(void)  // for proto_get safe null
{
	static kvs_t dnull[3] = {};
	static kvsarray_t pnull = {
		.kvs = dnull, .size = 1, .capacity = 0,
	};
	return &pnull;
}

static kvs_t* kpromap_get(kvsarray_t *p, ksymbol_t key)
{
	kvs_t *d = p->kvs + (((size_t)key) % p->size);
	if(d->key == key) return d; else d++;
	if(d->key == key) return d; else d++;
	if(d->key == key) return d; else d++;
	size_t i;
	for(i = 0; i < KPROMAP_DELTA - 3; i++) {
		if(d->key == key) return d;
		d++;
	}
	return NULL;
}

static inline void kpromap_findset(kvs_t *d, kvs_t *newd)
{
	size_t i;
	for(i = 0; i < KPROMAP_DELTA - 1; i++) {
		if(newd->key == 0) {
			*newd = *d;
			return;
		}
		newd++;
	}
}

static void kpromap_rehash(CTX, kvsarray_t *p)
{
	size_t i, newcapacity = p->capacity * 2, newsize = newcapacity - KPROMAP_DELTA;
	kvs_t *newdatamap = (kvs_t*)KNH_MALLOC(sizeof(kvs_t) * newcapacity);
	for(i = 0; i < p->capacity; i++) {
		kvs_t *d = p->kvs + i;
		if(d->key != 0) {
			kvs_t *newd = newdatamap + ((size_t)d->key) % newsize;
			if(newd->key == 0) {
				*newd = *d;
			}
			else {
				kpromap_findset(d, newd+1);
			}
		}
	}
	KNH_FREE(p->kvs, sizeof(kvs_t) * p->capacity);
	p->kvs = newdatamap;
	p->capacity = newcapacity;
	p->size = newsize;
}

static kvsarray_t *kpromap_new(CTX)
{
	kvsarray_t *p;
	p = (kvsarray_t*)KNH_MALLOC(sizeof(kvsarray_t));
	p->kvs = (kvs_t*)KNH_MALLOC(sizeof(kvs_t) * 8);
	p->capacity = 8;
	p->size = p->capacity - KPROMAP_DELTA;
	return p;
}

void kpromap_reftrace(CTX, kvsarray_t *p)
{
	size_t i;
	kvs_t *d = p->kvs;
	BEGIN_REFTRACE(p->capacity);
	for(i = 0; i < p->capacity; i++) {
		if(FN_isBOXED(d->key)) {
			KREFTRACEv(d->oval);
		}
		d++;
	}
	END_REFTRACE();
}

void kpromap_free(CTX, kvsarray_t *p)
{
	if(p->size > 1) {
		KNH_FREE(p->kvs, p->capacity * sizeof(kvs_t));
		KNH_FREE(p, sizeof(kvsarray_t));
	}
}

static void kpromap_set(CTX, kvsarray_t **pval, ksymbol_t key, ktype_t ty, uintptr_t uval)
{
	kvsarray_t *p = pval[0];
	if(p->size == 1) {
		p = kpromap_new(_ctx);
		pval[0] = p;
	}
	do {
		size_t i;
		kvs_t *d = p->kvs + (((size_t)key) % p->size);
		for(i = 0; i < KPROMAP_DELTA; i++) {
			if(d->key == key || d->key == 0) {
				d->key = key; d->ty = ty; d->uval = uval;
				return;
			}
			d++;
		}
		kpromap_rehash(_ctx, p);
	}
	while(1);
}

void kpromap_each(CTX, kvsarray_t *p, void *arg, void (*f)(CTX, void *, kvs_t *d))
{
	size_t i;
	kvs_t *d = p->kvs;
	for (i = 0; i < p->capacity; ++i, ++d) {
		f(_ctx, arg, d);
	}
}

static kObject* Object_getObjectNULL(CTX, kObject *o, ksymbol_t key, kObject *defval)
{
	kvs_t *d = kpromap_get(o->h.proto, key | FN_BOXED);
	return (d != NULL) ? d->oval : defval;
}

static void Object_setObject(CTX, kObject *o, ksymbol_t key, ktype_t ty, kObject *val)
{
	kpromap_set(_ctx, &o->h.proto, key | FN_BOXED, ty, (uintptr_t)val);
}

static uintptr_t Object_getUnboxedValue(CTX, kObject *o, ksymbol_t key, uintptr_t defval)
{
	kvs_t *d = kpromap_get(o->h.proto, key);
	return (d != NULL) ? d->uval : defval;
}

static void Object_setUnboxedValue(CTX, kObject *o, ksymbol_t key, ktype_t ty, uintptr_t uval)
{
	kpromap_set(_ctx, &o->h.proto, key, ty, uval);
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
		const char *file = T_file(pline);
		fprintf(stdout, " - (%s:%d) %s" , filename(file), (kushort_t)pline, T_ERR(level));
	}
	else {
		fprintf(stdout, " - %s" , T_ERR(level));
	}
	vfprintf(stdout, fmt, ap);
	fputs(T_END(_ctx, level), stdout);
	fprintf(stdout, "\n");
	va_end(ap);
	if(level == CRIT_) {
		kraise(0);
	}
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
	l->Karray_resize = karray_resize;
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
