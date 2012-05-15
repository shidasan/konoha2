#include<math.h>

#define Int_to(T, a)               ((T)a.ivalue)
#define Float_to(T, a)             ((T)a.fvalue)

static KMETHOD Math_abs(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(abs(Int_to(int, sfp[1])));
}

static KMETHOD Math_fabs(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(fabs(Float_to(double, sfp[1])));
}

static KMETHOD Math_pow(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(pow(Float_to(double, sfp[1]),Float_to(double, sfp[2])));
}

static KMETHOD Math_ldexp(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(ldexp(Float_to(double, sfp[1]), Int_to(int, sfp[2])));
}

static KMETHOD Math_modf(CTX, ksfp_t *sfp _RIX)
{
	double iprt = Float_to(double, sfp[2]);
	RETURNf_(modf(Float_to(double, sfp[1]), &iprt));
}

static KMETHOD Math_frexp(CTX, ksfp_t *sfp _RIX)
{
	int exp = Int_to(int, sfp[2]);
	RETURNf_(frexp(Float_to(double, sfp[1]), &exp));
}

static KMETHOD Math_fmod(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(fmod(Float_to(double, sfp[1]),Float_to(double, sfp[2])));
}

static KMETHOD Math_ceil(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(ceil(Float_to(double, sfp[1])));
}

#ifdef K_USING_WIN32_
static KMETHOD Math_round(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(round(Float_to(double, sfp[1])));
}

static KMETHOD Math_nearByInt(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(nearbyint(Float_to(double, sfp[1])));
}
#endif

static KMETHOD Math_floor(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(floor(Float_to(double, sfp[1])));
}

static KMETHOD Math_sqrt(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sqrt(Float_to(double, sfp[1])));
}

static KMETHOD Math_exp(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(exp(Float_to(double, sfp[1])));
}

static KMETHOD Math_log10(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(log10(Float_to(double, sfp[1])));
}

static KMETHOD Math_log(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(log(Float_to(double, sfp[1])));
}

static KMETHOD Math_sin(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sin(Float_to(double, sfp[1])));
}

static KMETHOD Math_cos(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(cos(Float_to(double, sfp[1])));
}

static KMETHOD Math_tan(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(tan(Float_to(double, sfp[1])));
}

static KMETHOD Math_asin(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(asin(Float_to(double, sfp[1])));
}

static KMETHOD Math_acos(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(acos(Float_to(double, sfp[1])));
}

static KMETHOD Math_atan(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(atan(Float_to(double, sfp[1])));
}

static KMETHOD Math_atan2(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(atan2(Float_to(double, sfp[1]),Float_to(double, sfp[2])));
}

static KMETHOD Math_sinh(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sinh(Float_to(double, sfp[1])));
}

static KMETHOD Math_cosh(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(cosh(Float_to(double, sfp[1])));
}

static KMETHOD Math_tanh(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(tanh(Float_to(double, sfp[1])));
}

#if defined(K_USING_WIN32_)
static KMETHOD Math_asinh(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(asinh(Float_to(double, sfp[1])));
}

static KMETHOD Math_acosh(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(acosh(Float_to(double, sfp[1])));
}

static KMETHOD Math_atanh(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(atanh(Float_to(double, sfp[1])));
}
#endif

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)
#define _KVf(T) "MATH_" #T, TY_Float, M_##T
#define TY_Math  (cMath->cid)

static	kbool_t math_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	KREQUIRE_PACKAGE("konoha.float", pline);
	static KDEFINE_CLASS MathDef = {
			.structname = "Math"/*structname*/,
			.cid = CLASS_newid/*cid*/,
	};
	kclass_t *cMath = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &MathDef, pline);
	int FN_x = FN_("x");
	int FN_y = FN_("y");
	intptr_t MethodData[] = {
			_Public|_Const, _F(Math_abs), TY_Int, TY_Math, MN_("abs"), 1, TY_Int, FN_x,
			_Public|_Const, _F(Math_fabs), TY_Float, TY_Math, MN_("fabs"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_pow), TY_Float, TY_Math, MN_("pow"), 2, TY_Float, FN_x, TY_Float, FN_y,
			_Public|_Const, _F(Math_ldexp), TY_Float, TY_Math, MN_("ldexp"), 2, TY_Float, FN_x, TY_Int, FN_y,
			_Public|_Const, _F(Math_modf), TY_Float, TY_Math, MN_("modf"), 2, TY_Float, FN_x, TY_Float, FN_y,
			_Public|_Const, _F(Math_frexp), TY_Float, TY_Math, MN_("frexp"), 2, TY_Float, FN_x, TY_Int, FN_y,
			_Public|_Const, _F(Math_fmod), TY_Float, TY_Math, MN_("fmod"), 2, TY_Float, FN_x, TY_Float, FN_y,
			_Public|_Const, _F(Math_ceil), TY_Float, TY_Math, MN_("ceil"), 1, TY_Float, FN_x,
#ifdef K_USING_WIN32_
			_Public, _F(Math_round), TY_Float, TY_Math, MN_("round"), 1, TY_Float, FN_x,
			_Public, _F(Math_nearByInt), TY_Float, TY_Math, MN_("nearByInt"), 1, TY_Float, FN_x,
#endif
			_Public|_Const, _F(Math_floor), TY_Float, TY_Math, MN_("floor"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_sqrt), TY_Float, TY_Math, MN_("sqrt"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_exp), TY_Float, TY_Math, MN_("exp"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_log10), TY_Float, TY_Math, MN_("log10"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_log), TY_Float, TY_Math, MN_("log"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_sin), TY_Float, TY_Math, MN_("sin"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_cos), TY_Float, TY_Math, MN_("cos"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_tan), TY_Float, TY_Math, MN_("tan"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_asin), TY_Float, TY_Math, MN_("asin"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_acos), TY_Float, TY_Math, MN_("acos"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_atan), TY_Float, TY_Math, MN_("atan"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_atan2), TY_Float, TY_Math, MN_("atan2"), 2, TY_Float, FN_x, TY_Float, FN_y,
			_Public|_Const, _F(Math_sinh), TY_Float, TY_Math, MN_("sinh"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_cosh), TY_Float, TY_Math, MN_("cosh"), 1, TY_Float, FN_x,
			_Public|_Const, _F(Math_tanh), TY_Float, TY_Math, MN_("tanh"), 1, TY_Float, FN_x,
#if defined(K_USING_WIN32_)
			_Public, _F(Math_asinh), TY_Float, TY_Math, MN_("asinh"), 1, TY_Float, FN_x,
			_Public, _F(Math_acosh), TY_Float, TY_Math, MN_("acosh"), 1, TY_Float, FN_x,
			_Public, _F(Math_atanh), TY_Float, TY_Math, MN_("atanh"), 1, TY_Float, FN_x,
#endif
			DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);

	KDEFINE_FLOAT_CONST FloatData[] = {
			{_KVf(E)},
			{_KVf(LOG2E)},
			{_KVf(LOG10E)},
			{_KVf(LN2)},
			{_KVf(LN10)},
			{_KVf(PI)},
			{_KVf(SQRT2)},
			{}
	};
	kKonohaSpace_loadConstData(ks, FloatData, 0);
	return true;
}

static kbool_t math_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t math_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	KEXPORT_PACKAGE("konoha.float", ks, pline);
	return true;
}

static kbool_t math_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}
