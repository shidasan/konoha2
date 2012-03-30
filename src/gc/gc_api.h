#ifndef GC_API_H_
#define GC_API_H_

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

#endif /* end of include guard */
