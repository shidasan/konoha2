/*
 * konoha2_local.h
 *
 *  Created on: Apr 12, 2012
 *      Author: kimio
 */

#ifndef KONOHA2_LOCAL_H_
#define KONOHA2_LOCAL_H_

// These functions are local functions in konoha2 binary.
// Don't call from packages directly   (kimio)

struct _kObject** KONOHA_reftail(CTX, size_t size);
void KONOHA_reftraceObject(CTX, kObject *o);  // called from MODGC
void KONOHA_freeObjectField(CTX, struct _kObject *o);       // callled from MODGC

void MODCODE_init(CTX, kcontext_t *ctx);
//void MODCODE_genCode(CTX, kMethod *mtd, const struct _kBlock *bk);

void MODSUGAR_init(CTX, kcontext_t *ctx);
kstatus_t MODSUGAR_loadscript(CTX, const char *path, size_t len, kline_t pline);
kstatus_t MODSUGAR_eval(CTX, const char *script, kline_t uline);

void MODLOGGER_init(CTX, kcontext_t *ctx);
void MODSUGAR_defMethods(CTX);




#endif /* KONOHA2_LOCAL_H_ */
