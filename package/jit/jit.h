#ifndef JIT_H_
#define JIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MOD_jit  40/*TODO*/
#define kjitmod ((kjitmod_t*)_ctx->mod[MOD_jit])
#define kmodjit ((kmodjit_t*)_ctx->modshare[MOD_jit])
#define MtdGenCode (kmodjit)->GenCode

kbool_t llvm_jit_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline);

#ifdef __cplusplus
}
#endif
#endif /* end of include guard */
