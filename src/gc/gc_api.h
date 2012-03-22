#ifndef GC_API_H_
#define GC_API_H_

extern void kmemshare_init(CTX, kcontext_t *ctx);
extern void kmemshare_free(CTX, kcontext_t *ctx);
extern void kmemshare_gc_destroy(CTX, kcontext_t *ctx);

extern void kmemlocal_init(CTX, kcontext_t *ctx);
extern void kmemlocal_free(CTX, kcontext_t *ctx);
extern kObject *bmgc_omalloc(CTX, size_t size);

/* root reftrace */
extern void kSystem_reftraceAll(CTX);

extern void knh_System_gc(CTX, int needsCStackTrace);
extern void klib2_GCAPI_init(klib2_t *l);
extern void klib2_check_malloced_size(void);

#endif /* end of include guard */
