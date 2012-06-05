/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#define Int_to(T, a)               ((T)a.ivalue)
#define Float_to(T, a)             ((T)a.fvalue)


static KMETHOD System_balanceInit(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	balance_init();
#endif
}
static KMETHOD System_ecrobotInitNxtstate(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	syscall(serial_ctl_por(CONSOLE_PORTID, (IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));
	ecrobot_init_nxtstate();
#endif
}
static KMETHOD System_ecrobotInitSensors(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	ecrobot_init_sensors();
#endif
}
static KMETHOD System_ecrobotSetLightSensorActive(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	ecrobot_set_light_sensor_active(NXT_PORT_S3);
#endif
}
static KMETHOD System_ecrobotGetGyroSensor(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	RETURNi_(ecrobot_get_gyro_sensor(NXT_PORT_S1));
#else
	RETURNi_(0);
#endif
}
static KMETHOD System_ecrobotGetLightSensor(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	RETURNi_(ecrobot_get_light_sensor(NXT_PORT_S3));
#else
	RETURNi_(0);
#endif
}
static KMETHOD System_nxtMotorSetCount(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	int i = Int_to(int, sfp[1]);
	nxt_motor_set_count((i == 1) ? NXT_PORT_C : NXT_PORT_B, Int_to(int, sfp[2]));
#endif
}
static KMETHOD System_nxtMotorGetCount(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	int i = Int_to(int, sfp[1]);
	RETURNi_(nxt_motor_get_count((i == 1) ? NXT_PORT_C : NXT_PORT_B));
#else
	RETURNi_(0);
#endif
}
static KMETHOD System_staCyc(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	sta_cyc(CYC0);
#endif
}
static KMETHOD System_waiSem(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	wai_sem(EVT_SEM);
#endif
}
static KMETHOD System_balanceControl(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	signed char pwm_L, pwm_R;
	balance_control(
			Float_to(float, sfp[1]),
			Float_to(float, sfp[2]),
			Float_to(float, sfp[3]),
			Float_to(float, sfp[4]),
			Float_to(float, sfp[5]),
			Float_to(float, sfp[6]),
			ecrobot_get_battery_voltage(),
			&pwm_L,
			&pwm_R);
	ecrobot_set_motor_speed(NXT_PORT_C, pwm_L);
	ecrobot_set_motor_speed(NXT_PORT_B, pwm_R);
#endif
}

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _F(F)   (intptr_t)(F)

#ifndef K_USING_TINYVM
static	kbool_t nxt_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	KREQUIRE_PACKAGE("konoha.float", pline);
	//static KDEFINE_CLASS MathDef = {
	//		.structname = "Math"/*structname*/,
	//		.cid = CLASS_newid/*cid*/,
	//};
	//kclass_t *cMath = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &MathDef, pline);
	int FN_x = FN_("x");
	int FN_y = FN_("y");
	int FN_a = FN_("a");
	int FN_b = FN_("b");
	int FN_c = FN_("c");
	int FN_d = FN_("d");
	int FN_e = FN_("e");
	intptr_t MethodData[] = {
			_Public|_Const, _F(System_balanceInit), TY_void, TY_System, MN_("balanceInit"), 0,
			_Public|_Const, _F(System_ecrobotInitNxtstate), TY_void, TY_System, MN_("ecrobotInitNxtstate"), 0,
			_Public|_Const, _F(System_ecrobotInitSensors), TY_void, TY_System, MN_("ecrobotInitSensors"), 0,
			_Public|_Const, _F(System_ecrobotSetLightSensorActive), TY_void, TY_System, MN_("ecrobotSetLightSensorActive"), 0,
			_Public|_Const, _F(System_ecrobotGetGyroSensor), TY_Int, TY_System, MN_("ecrobotGetGyroSensor"), 0,
			_Public|_Const, _F(System_ecrobotGetLightSensor), TY_Int, TY_System, MN_("ecrobotGetLightSensor"), 0,
			_Public|_Const, _F(System_nxtMotorSetCount), TY_void, TY_System, MN_("nxtMotorSetCount"), 2, TY_Int, FN_x, TY_Int, FN_y,
			_Public|_Const, _F(System_nxtMotorGetCount), TY_Int, TY_System, MN_("nxtMotorGetCount"), 1, TY_Int, FN_x, 
			_Public|_Const, _F(System_staCyc), TY_void, TY_System, MN_("staCyc"), 0,
			_Public|_Const, _F(System_waiSem), TY_void, TY_System, MN_("waiSem"), 0,
			_Public|_Const, _F(System_balanceControl), TY_void, TY_System, MN_("balanceControl"), 6, TY_Float, FN_x, TY_Float, FN_y, TY_Float, FN_a, TY_Float, FN_b, TY_Float, FN_c, TY_Float, FN_d, 



//			_Public|_Const, _F(Math_abs), TY_Int, TY_Math, MN_("abs"), 1, TY_Int, FN_x,
//			_Public|_Const, _F(Math_fabs), TY_Float, TY_Math, MN_("fabs"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_pow), TY_Float, TY_Math, MN_("pow"), 2, TY_Float, FN_x, TY_Float, FN_y,
//			_Public|_Const, _F(Math_ldexp), TY_Float, TY_Math, MN_("ldexp"), 2, TY_Float, FN_x, TY_Int, FN_y,
//			_Public|_Const, _F(Math_modf), TY_Float, TY_Math, MN_("modf"), 2, TY_Float, FN_x, TY_Float, FN_y,
//			_Public|_Const, _F(Math_frexp), TY_Float, TY_Math, MN_("frexp"), 2, TY_Float, FN_x, TY_Int, FN_y,
//			_Public|_Const, _F(Math_fmod), TY_Float, TY_Math, MN_("fmod"), 2, TY_Float, FN_x, TY_Float, FN_y,
//			_Public|_Const, _F(Math_ceil), TY_Float, TY_Math, MN_("ceil"), 1, TY_Float, FN_x,
//#ifdef K_USING_WIN32_
//			_Public, _F(Math_round), TY_Float, TY_Math, MN_("round"), 1, TY_Float, FN_x,
//			_Public, _F(Math_nearByInt), TY_Float, TY_Math, MN_("nearByInt"), 1, TY_Float, FN_x,
//#endif
//			_Public|_Const, _F(Math_floor), TY_Float, TY_Math, MN_("floor"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_sqrt), TY_Float, TY_Math, MN_("sqrt"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_exp), TY_Float, TY_Math, MN_("exp"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_log10), TY_Float, TY_Math, MN_("log10"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_log), TY_Float, TY_Math, MN_("log"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_sin), TY_Float, TY_Math, MN_("sin"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_cos), TY_Float, TY_Math, MN_("cos"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_tan), TY_Float, TY_Math, MN_("tan"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_asin), TY_Float, TY_Math, MN_("asin"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_acos), TY_Float, TY_Math, MN_("acos"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_atan), TY_Float, TY_Math, MN_("atan"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_atan2), TY_Float, TY_Math, MN_("atan2"), 2, TY_Float, FN_x, TY_Float, FN_y,
//			_Public|_Const, _F(Math_sinh), TY_Float, TY_Math, MN_("sinh"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_cosh), TY_Float, TY_Math, MN_("cosh"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_tanh), TY_Float, TY_Math, MN_("tanh"), 1, TY_Float, FN_x,
//#if defined(K_USING_WIN32_)
//			_Public, _F(Math_asinh), TY_Float, TY_Math, MN_("asinh"), 1, TY_Float, FN_x,
//			_Public, _F(Math_acosh), TY_Float, TY_Math, MN_("acosh"), 1, TY_Float, FN_x,
//			_Public, _F(Math_atanh), TY_Float, TY_Math, MN_("atanh"), 1, TY_Float, FN_x,
//#endif
			DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);

	//KDEFINE_FLOAT_CONST FloatData[] = {
	//		{_KVf(E)},
	//		{_KVf(LOG2E)},
	//		{_KVf(LOG10E)},
	//		{_KVf(LN2)},
	//		{_KVf(LN10)},
	//		{_KVf(PI)},
	//		{_KVf(SQRT2)},
	//		{}
	//};
	//kKonohaSpace_loadConstData(ks, FloatData, 0);
	return true;
}

//Static KMETHOD Math_abs(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNi_(abs(Int_to(int, sfp[1])));
//}
//
//Static KMETHOD Math_fabs(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(fabs(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_pow(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(pow(Float_to(double, sfp[1]),Float_to(double, sfp[2])));
//}
//
//Static KMETHOD Math_ldexp(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(ldexp(Float_to(double, sfp[1]), Int_to(int, sfp[2])));
//}
//
//Static KMETHOD Math_modf(CTX, ksfp_t *sfp _RIX)
//{
//	double iprt = Float_to(double, sfp[2]);
//	RETURNf_(modf(Float_to(double, sfp[1]), &iprt));
//}
//
//Static KMETHOD Math_frexp(CTX, ksfp_t *sfp _RIX)
//{
//	int exp = Int_to(int, sfp[2]);
//	RETURNf_(frexp(Float_to(double, sfp[1]), &exp));
//}
//
//Static KMETHOD Math_fmod(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(fmod(Float_to(double, sfp[1]),Float_to(double, sfp[2])));
//}
//
//Static KMETHOD Math_ceil(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(ceil(Float_to(double, sfp[1])));
//}
//
//#ifdef K_USING_WIN32_
//Static KMETHOD Math_round(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(round(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_nearByInt(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(nearbyint(Float_to(double, sfp[1])));
//}
//#endif
//
//Static KMETHOD Math_floor(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(floor(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_sqrt(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(sqrt(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_exp(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(exp(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_log10(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(log10(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_log(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(log(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_sin(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(sin(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_cos(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(cos(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_tan(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(tan(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_asin(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(asin(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_acos(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(acos(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_atan(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(atan(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_atan2(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(atan2(Float_to(double, sfp[1]),Float_to(double, sfp[2])));
//}
//
//Static KMETHOD Math_sinh(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(sinh(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_cosh(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(cosh(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_tanh(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(tanh(Float_to(double, sfp[1])));
//}
//
//#if defined(K_USING_WIN32_)
//Static KMETHOD Math_asinh(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(asinh(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_acosh(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(acosh(Float_to(double, sfp[1])));
//}
//
//Static KMETHOD Math_atanh(CTX, ksfp_t *sfp _RIX)
//{
//	RETURNf_(atanh(Float_to(double, sfp[1])));
//}
//#endif
//
//// --------------------------------------------------------------------------
//
//#define _Public   kMethod_Public
//#define _Const    kMethod_Const
//#define _Coercion kMethod_Coercion
//#define _F(F)   (intptr_t)(F)
//#define _KVf(T) "MATH_" #T, TY_Float, M_##T
//#define TY_Math  (cMath->cid)
//
//Static	kbool_t math_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
//{
//	KREQUIRE_PACKAGE("konoha.float", pline);
//	static KDEFINE_CLASS MathDef = {
//			.structname = "Math"/*structname*/,
//			.cid = CLASS_newid/*cid*/,
//	};
//	kclass_t *cMath = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &MathDef, pline);
//	int FN_x = FN_("x");
//	int FN_y = FN_("y");
//	intptr_t MethodData[] = {
//			_Public|_Const, _F(Math_abs), TY_Int, TY_Math, MN_("abs"), 1, TY_Int, FN_x,
//			_Public|_Const, _F(Math_fabs), TY_Float, TY_Math, MN_("fabs"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_pow), TY_Float, TY_Math, MN_("pow"), 2, TY_Float, FN_x, TY_Float, FN_y,
//			_Public|_Const, _F(Math_ldexp), TY_Float, TY_Math, MN_("ldexp"), 2, TY_Float, FN_x, TY_Int, FN_y,
//			_Public|_Const, _F(Math_modf), TY_Float, TY_Math, MN_("modf"), 2, TY_Float, FN_x, TY_Float, FN_y,
//			_Public|_Const, _F(Math_frexp), TY_Float, TY_Math, MN_("frexp"), 2, TY_Float, FN_x, TY_Int, FN_y,
//			_Public|_Const, _F(Math_fmod), TY_Float, TY_Math, MN_("fmod"), 2, TY_Float, FN_x, TY_Float, FN_y,
//			_Public|_Const, _F(Math_ceil), TY_Float, TY_Math, MN_("ceil"), 1, TY_Float, FN_x,
//#ifdef K_USING_WIN32_
//			_Public, _F(Math_round), TY_Float, TY_Math, MN_("round"), 1, TY_Float, FN_x,
//			_Public, _F(Math_nearByInt), TY_Float, TY_Math, MN_("nearByInt"), 1, TY_Float, FN_x,
//#endif
//			_Public|_Const, _F(Math_floor), TY_Float, TY_Math, MN_("floor"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_sqrt), TY_Float, TY_Math, MN_("sqrt"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_exp), TY_Float, TY_Math, MN_("exp"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_log10), TY_Float, TY_Math, MN_("log10"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_log), TY_Float, TY_Math, MN_("log"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_sin), TY_Float, TY_Math, MN_("sin"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_cos), TY_Float, TY_Math, MN_("cos"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_tan), TY_Float, TY_Math, MN_("tan"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_asin), TY_Float, TY_Math, MN_("asin"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_acos), TY_Float, TY_Math, MN_("acos"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_atan), TY_Float, TY_Math, MN_("atan"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_atan2), TY_Float, TY_Math, MN_("atan2"), 2, TY_Float, FN_x, TY_Float, FN_y,
//			_Public|_Const, _F(Math_sinh), TY_Float, TY_Math, MN_("sinh"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_cosh), TY_Float, TY_Math, MN_("cosh"), 1, TY_Float, FN_x,
//			_Public|_Const, _F(Math_tanh), TY_Float, TY_Math, MN_("tanh"), 1, TY_Float, FN_x,
//#if defined(K_USING_WIN32_)
//			_Public, _F(Math_asinh), TY_Float, TY_Math, MN_("asinh"), 1, TY_Float, FN_x,
//			_Public, _F(Math_acosh), TY_Float, TY_Math, MN_("acosh"), 1, TY_Float, FN_x,
//			_Public, _F(Math_atanh), TY_Float, TY_Math, MN_("atanh"), 1, TY_Float, FN_x,
//#endif
//			DEND,
//	};
//	kKonohaSpace_loadMethodData(ks, MethodData);
//
//	KDEFINE_FLOAT_CONST FloatData[] = {
//			{_KVf(E)},
//			{_KVf(LOG2E)},
//			{_KVf(LOG10E)},
//			{_KVf(LN2)},
//			{_KVf(LN10)},
//			{_KVf(PI)},
//			{_KVf(SQRT2)},
//			{}
//	};
//	kKonohaSpace_loadConstData(ks, FloatData, 0);
//	return true;
//}

static kbool_t nxt_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t nxt_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	KEXPORT_PACKAGE("konoha.float", ks, pline);
	return true;
}

static kbool_t nxt_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}
#endif
