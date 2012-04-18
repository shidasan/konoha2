/*
 * float_glue.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kimio
 */

#ifndef FLOAT_GLUE_H_
#define FLOAT_GLUE_H_

#include<float.h>

// Int
static void Float_init(CTX, kObject *o, void *conf)
{
	struct _kNumber *n = (struct _kNumber*)o;  // kFloat has the same structure
	n->ndata = (uintptr_t)conf;  // conf is unboxed data
}

static void Float_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kwb_printf(wb, "%f", sfp[pos].fvalue);
}

static void kmodfloat_setup(CTX, struct kmodshare_t *def, int newctx)
{
}

static void kmodfloat_reftrace(CTX, struct kmodshare_t *baseh)
{
}

static void kmodfloat_free(CTX, struct kmodshare_t *baseh)
{
	KNH_FREE(baseh, sizeof(kmodfloat_t));
}

static KMETHOD ExprTyCheck_FLOAT(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tk = expr->tk;
	sfp[4].fvalue = strtod(S_text(tk->text), NULL);
	RETURN_(kExpr_setNConstValue(expr, TY_Float, sfp[4].ndata));
}

// --------------------------------------------------------------------------

/* float + float */
static KMETHOD Float_opADD(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sfp[0].fvalue + sfp[1].fvalue);
}

/* float - float */
static KMETHOD Float_opSUB(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sfp[0].fvalue - sfp[1].fvalue);
}

/* float * float */
static KMETHOD Float_opMUL(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sfp[0].fvalue * sfp[1].fvalue);
}

/* float / float */
static KMETHOD Float_opDIV(CTX, ksfp_t *sfp _RIX)
{
	kfloat_t n = sfp[1].fvalue;
	if(unlikely(n == 0.0)) {
		kreportf(CRIT_, sfp[K_RTNIDX].uline, "Script!!: zero divided");
	}
	RETURNf_(sfp[0].fvalue / n);
}

/* float == float */
static KMETHOD Float_opEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue == sfp[1].fvalue);
}

/* float != float */
static KMETHOD Float_opNEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue != sfp[1].fvalue);
}

/* float < float */
static KMETHOD Float_opLT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue < sfp[1].fvalue);
}

/* float <= float */
static KMETHOD Float_opLTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue <= sfp[1].fvalue);
}

/* float > float */
static KMETHOD Float_opGT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue > sfp[1].fvalue);
}

/* float >= float */
static KMETHOD Float_opGTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue >= sfp[1].fvalue);
}

/* float to int */
static KMETHOD Float_toInt(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_((kint_t)sfp[0].fvalue);
}

/* float >= float */
static KMETHOD Int_toFloat(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_((kfloat_t)sfp[0].ivalue);
}

/* float to String */
static KMETHOD Float_toString(CTX, ksfp_t *sfp _RIX)
{
	char buf[40];
	snprintf(buf, sizeof(buf), "%f", sfp[0].fvalue);
	RETURN_(new_kString(buf, strlen(buf), SPOL_ASCII));
}

/* String to float */
static KMETHOD String_toFloat(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_((kfloat_t)strtod(S_text(sfp[0].s), NULL));
}

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Im       kMethod_Immutable
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t share_initfloat(CTX, kKonohaSpace *ks, kline_t pline)
{
	kmodfloat_t *base = (kmodfloat_t*)KNH_ZMALLOC(sizeof(kmodfloat_t));
	base->h.name     = "float";
	base->h.setup    = kmodfloat_setup;
	base->h.reftrace = kmodfloat_reftrace;
	base->h.free     = kmodfloat_free;
	ksetModule(MOD_float, &base->h, pline);

	KDEFINE_CLASS FloatDef = {
		STRUCTNAME(Float),
		.packid  = ks->packid,
		.packdom = 0,
		.cflag = CFLAG_Int,
		.init = Float_init,
		.p     = Float_p,
	};
	base->cFloat = kaddClassDef(NULL, &FloatDef, pline);
	int FN_x = FN_("x");
	intptr_t MethodData[] = {
		_Public|_Const|_Im, _F(Float_opADD), TY_Float, TY_Float, MN_("opADD"), 1, TY_Float, FN_x,
		_Public|_Const|_Im, _F(Float_opSUB), TY_Float, TY_Float, MN_("opSUB"), 1, TY_Float, FN_x,
		_Public|_Const|_Im, _F(Float_opMUL), TY_Float, TY_Float, MN_("opMUL"), 1, TY_Float, FN_x,
		_Public|_Im, _F(Float_opDIV), TY_Float, TY_Float, MN_("opDIV"), 1, TY_Float, FN_x,
		_Public|_Const|_Im, _F(Float_opEQ),  TY_Boolean, TY_Float, MN_("opEQ"), 1, TY_Float, FN_x,
		_Public|_Const|_Im, _F(Float_opNEQ), TY_Boolean, TY_Float, MN_("opNEQ"), 1, TY_Float, FN_x,
		_Public|_Const|_Im, _F(Float_opLT),  TY_Boolean, TY_Float, MN_("opLT"), 1, TY_Float, FN_x,
		_Public|_Const|_Im, _F(Float_opLTE), TY_Boolean, TY_Float, MN_("opLTE"), 1, TY_Float, FN_x,
		_Public|_Const|_Im, _F(Float_opGT),  TY_Boolean, TY_Float, MN_("opGT"), 1, TY_Float, FN_x,
		_Public|_Const|_Im, _F(Float_opGTE), TY_Boolean, TY_Float, MN_("opGTE"), 1, TY_Float, FN_x,
		_Public|_Const|_Im|_Coercion, _F(Float_toInt), TY_Int, TY_Float, MN_to(TY_Int), 0,
		_Public|_Const|_Im|_Coercion, _F(Int_toFloat), TY_Float, TY_Int, MN_to(TY_Float), 0,
		_Public|_Const|_Im, _F(Float_toString), TY_String, TY_Float, MN_to(TY_String), 0,
		_Public|_Const|_Im, _F(String_toFloat), TY_Float, TY_String, MN_to(TY_Float), 0,
		DEND,
	};
	kloadMethodData(ks, MethodData);
	KDEFINE_FLOAT_CONST FloatData[] = {
		{"FLOAT_EPSILON", TY_Float, DBL_EPSILON},
		{}
	};
	kloadConstData(ks, FloatData, pline);
	return true;
}

static kbool_t local_initfloat(CTX, kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("float"), .type = TY_Float, },
		{ TOKEN("double"), .type = TY_Float, },
		{ TOKEN("$FLOAT"), .kw = KW_TK(TK_FLOAT), .ExprTyCheck = ExprTyCheck_FLOAT, },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

#endif /* FLOAT_GLUE_H_ */
