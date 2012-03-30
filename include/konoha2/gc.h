#ifndef MODGC_H_
#define MODGC_H_

//#define kgcmod        ((kgcmod_t*)_ctx->mod[MOD_GC])
//#define kgcshare      ((kgcshare_t*)_ctx->modshare[MOD_GC])
//
//typedef struct {
//	kmodshare_t h;
//} kgcshare_t;
//
//typedef struct {
//	kmod_t h;
//} kgcmod_t;

extern void MODGCSHARE_init(CTX, kcontext_t *ctx);
extern void MODGCSHARE_free(CTX, kcontext_t *ctx);
extern void MODGCSHARE_gc_destroy(CTX, kcontext_t *ctx);

extern void MODGC_init(CTX, kcontext_t *ctx);
extern void MODGC_free(CTX, kcontext_t *ctx);
extern kObject *MODGC_omalloc(CTX, size_t size);

/* root reftrace */
extern void kSystem_reftraceAll(CTX);

extern void MODGC_gc_invoke(CTX, int needsCStackTrace);
extern void MODGC_check_malloced_size(void);

#endif /* MODGC_H_ */
