/*
 * float.h
 *
 *  Created on: Mar 28, 2012
 *      Author: kimio
 */

#ifndef MODFLOAT_H_
#define MODFLOAT_H_

#define kfloatmod        ((kfloatmod_t*)_ctx->mod[MOD_FLOAT])
#define kfloatshare      ((kfloatshare_t*)_ctx->modshare[MOD_FLOAT])
#define CT_Float         kfloatshare->cFloat
#define TY_Float         kfloatshare->cFloat->cid

#define IS_Float(O)      ((O)->h.ct == CT_Float)

typedef struct {
	kmodshare_t h;
	const kclass_t *cFloat;
} kfloatshare_t;

typedef struct {
	kmod_t h;
} kfloatmod_t;

#endif /* MODFLOAT_H_ */
