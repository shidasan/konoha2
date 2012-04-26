/*
 * bytes_glue.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kimio, yoan
 */

#ifndef BYTES_GLUE_H_
#define BYTES_GLUE_H_

#include<konoha2/bytes.h>

// Int
static void Bytes_init(CTX, kObject *o, void *conf)
{
	//struct _kNumber *n = (struct _kNumber*)o;  // kFloat has the same structure
	//n->ndata = (uintptr_t)conf;  // conf is unboxed data
}

static void Bytes_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	//kwb_printf(wb, "%f", sfp[pos].fvalue);
}

static void kmodbytes_setup(CTX, struct kmodshare_t *def, int newctx)
{
}

static void kmodbytes_reftrace(CTX, struct kmodshare_t *baseh)
{
}

static void kmodbytes_free(CTX, struct kmodshare_t *baseh)
{
	KNH_FREE(baseh, sizeof(kmodbytes_t));
}

static KMETHOD ExprTyCheck_BYTES(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tk = expr->tk;
	RETURN_(kExpr_setConstValue(expr, TY_Bytes, tk->text));
}

static kbool_t local_initbytes(CTX, kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("Bytes"),  .type = TY_Bytes, },
		{ TOKEN("$BYTES"), .kw = KW_TK(TK_BYTES), .ExprTyCheck = ExprTyCheck_BYTES, },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t share_initbytes(CTX, kKonohaSpace *ks, kline_t pline);

#endif /* BYTES_GLUE_H_ */
