#include "mod_sample.h"

void ksamplemod_reftrace(CTX, struct kmodlocal_t *mod_)
{
	ksamplemod_t *mod = (ksamplemod_t*)mod_;
	BEGIN_REFTRACE(2);
	KREFTRACEv(mod->array);
	END_REFTRACE();
}

void ksamplemod_free(CTX, struct kmodlocal_t *mod_)
{
	ksamplemod_t *mod = (ksamplemod_t*)mod_;
	KNH_FREE(mod, sizeof(ksamplemod_t));
}

void ksampleshare_setup(CTX, struct kmodshare_t *def)
{
	if(_ctx->mod[MOD_SAMPLE] == NULL) {
		ksamplemod_t *mod = (ksamplemod_t *) KNH_ZMALLOC(sizeof(ksamplemod_t));
		mod->h.reftrace = ksamplemod_reftrace;
		mod->h.free     = ksamplemod_free;
		KINITv(mod->array, new_(Array, 8));
		_ctx->mod[MOD_SAMPLE] = (kmodlocal_t*)mod;
	}
}

void ksampleshare_reftrace(CTX, struct kmodshare_t *mod_)
{
}

void ksampleshare_free(CTX, struct kmodshare_t *mod_)
{
	ksampleshare_t *mod = (ksampleshare_t*)mod_;
	KNH_FREE(mod, sizeof(ksampleshare_t));
}

void ksampleshare_init(CTX, kcontext_t *ctx)
{
	ksampleshare_t *mod = (ksampleshare_t *) KNH_ZMALLOC(sizeof(ksampleshare_t));
	mod->h.name     = "sample";
	mod->h.setup    = ksampleshare_setup;
	mod->h.reftrace = ksampleshare_reftrace;
	mod->h.free     = ksampleshare_free;
	ksetModule(MOD_SAMPLE, (kmodshare_t*)mod, 0);
}

