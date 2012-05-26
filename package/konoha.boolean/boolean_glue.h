/*
 * boolean_glue.h
 *
 *  Created on: 2012/04/17
 *      Author: shinpei
 */

#ifndef BOOLEAN_GLUE_H_
#define BOOLEAN_GLUE_H_

static void Boolean_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{

}

static KMETHOD Boolean_opEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].bvalue == sfp[1].bvalue);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Im       kMethod_Immutable
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t boolean_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	intptr_t MethodData[] = {
		_Public|_Const|_Im, _F(Boolean_opEQ),  TY_Boolean, TY_Boolean, MN_("opEQ"), 1, TY_Boolean, FN_("b"),
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t boolean_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t boolean_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t boolean_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}
#endif /* BOOLEAN_GLUE_H_ */
