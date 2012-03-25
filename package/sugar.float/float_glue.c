#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

typedef struct kFloat kFloat;
struct kFloat {
	kObjectHeader h;
	kfloat_t fvalue;
};

// Int
static void Float_init(CTX, kRawPtr *o, void *conf)
{
	kInt *n = (kInt*)o;
	n->n.data = (uintptr_t)conf;  // conf is unboxed data
}

static KCLASS_DEF FloatDef = {
	STRUCTNAME(Float),
	.cflag = CFLAG_Int,
	.init = Float_init,
};

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
	RETURNf_(sfp[0].fvalue / sfp[1].fvalue);
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

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t float_initPackage(CTX, struct kLingo *lgo, int argc, const char**args, kline_t pline)
{
	const kclass_t *cFloat = kaddClassDef(&FloatDef);
	kcid_t TY_Float = cFloat->cid;
	int FN_x = FN_("x");
	intptr_t methoddata[] = {
		_Public|_Const, _F(Float_opADD), TY_Float, TY_Float, MN_("opADD"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opSUB), TY_Float, TY_Float, MN_("opSUB"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opMUL), TY_Float, TY_Float, MN_("opMUL"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opDIV), TY_Float, TY_Float, MN_("opDIV"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opEQ), TY_Float, TY_Float, MN_("opEQ"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opNEQ), TY_Float, TY_Float, MN_("opNEQ"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opLT), TY_Float, TY_Float, MN_("opLT"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opLTE), TY_Float, TY_Float, MN_("opLTE"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opGT), TY_Float, TY_Float, MN_("opGT"), 1, TY_Float, FN_x,
		_Public|_Const, _F(Float_opGTE), TY_Float, TY_Float, MN_("opGTE"), 1, TY_Float, FN_x,
		_Public|_Const|_Coercion, _F(Float_toInt), TY_Int, TY_Float, MN_to(TY_Int), 0,
		_Public|_Const|_Coercion, _F(Int_toFloat), TY_Float, TY_Int, MN_to(TY_Float), 0,
		_Public|_Const, _F(Float_toString), TY_String, TY_Float, MN_to(TY_String), 0,
		_Public|_Const, _F(String_toFloat), TY_Float, TY_String, MN_to(TY_Float), 0,
		DEND,
	};
	kaddMethodDef(NULL, methoddata);
	return true;
}

static kbool_t float_setupPackage(CTX, struct kLingo *lgo, kline_t pline)
{

	return true;
}

static kbool_t float_initLingo(CTX,  struct kLingo *lgo, kline_t pline)
{
	return true;
}

static kbool_t float_setupLingo(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* float_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("float", "1.0"),
		.initPackage = float_initPackage,
		.setupPackage = float_setupPackage,
		.initLingo = float_initLingo,
		.setupPackage = float_setupLingo,
	};
	return &d;
}
