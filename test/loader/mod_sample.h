#include "../src/minivm/konoha2.h"

#ifndef MOD_SAMPLE_H_
#define MOD_SAMPLE_H_

typedef struct ksampleshare_t {
	struct kmodshare_t h;
	const kclass_t *cSample;
} ksampleshare_t;

typedef struct {
	kmodlocal_t h;
	struct kArray *array;
} ksamplemod_t;

#define MOD_SAMPLE 2

typedef struct Sample {
	int x;
} kSample;

#endif /* end of include guard */
