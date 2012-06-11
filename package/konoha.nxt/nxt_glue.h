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
static KMETHOD System_dly(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	int delay = Int_to(int, sfp[1]);
	dly_tsk(delay);
#endif
}
static KMETHOD System_actMainTask(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	act_tsk(TASK0);
#endif
}
static KMETHOD System_ecrobotIsRunning(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	RETURNb_(ecrobotIsRunning());
#else
	RETURNb_(0);
#endif
}
static KMETHOD System_tailControl(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
#define P_GAIN 2.5F
#define PWM_ABS_MAX 60
	int angle = Int_to(int, sfp[1]);
	float pwm = (float)(angle - nxt_motor_get_count(NXT_PORT_A)) * P_GAIN;
	if (pwm > PWM_ABS_MAX) {
		pwm = PWM_ABS_MAX;
	} else if (pwm < -PWM_ABS_MAX) {
		pwm = -PWM_ABS_MAX;
	}
	nxt_motor_set_speed(NXT_PORT_A, (signed char)pwm, 1);
#endif
}
static KMETHOD System_manipulateTail(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	manipulate_tail();
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
static KMETHOD System_nxtMotorSetSpeed(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_USING_TOPPERS
	int i = Int_to(int, sfp[1]);
	nxt_motor_set_speed((i == 1) ? NXT_PORT_C : NXT_PORT_B, Int_to(int, sfp[2]), 1);
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
	//sta_cyc(CYC0);
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
#define GYRO_OFFSET 589
	static signed char pwm_L, pwm_R;
	balance_control(
			Float_to(float, sfp[1]),
			Float_to(float, sfp[2]),
			ecrobot_get_gyro_sensor(NXT_PORT_S1),
			GYRO_OFFSET,
			nxt_motor_get_count(NXT_PORT_C),
			nxt_motor_get_count(NXT_PORT_B),
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
	intptr_t MethodData[] = {
			_Public|_Const, _F(System_balanceInit), TY_void, TY_System, MN_("balanceInit"), 0,
			_Public|_Const, _F(System_dly), TY_void, TY_System, MN_("dly"), 1, TY_Int, FN_x, 
			_Public|_Const, _F(System_actMainTask), TY_void, TY_System, MN_("actMainTask"), 0, 
			_Public|_Const, _F(System_ecrobotIsRunning), TY_Boolean, TY_System, MN_("ecrobotIsRunning"), 0, 
			_Public|_Const, _F(System_tailControl), TY_void, TY_System, MN_("tailControl"), 1, TY_Int, FN_x, 
			_Public|_Const, _F(System_manipulateTail), TY_void, TY_System, MN_("manipulateTail"), 0,
			_Public|_Const, _F(System_ecrobotInitNxtstate), TY_void, TY_System, MN_("ecrobotInitNxtstate"), 0,
			_Public|_Const, _F(System_ecrobotInitSensors), TY_void, TY_System, MN_("ecrobotInitSensors"), 0,
			_Public|_Const, _F(System_ecrobotSetLightSensorActive), TY_void, TY_System, MN_("ecrobotSetLightSensorActive"), 0,
			_Public|_Const, _F(System_ecrobotGetGyroSensor), TY_Int, TY_System, MN_("ecrobotGetGyroSensor"), 0,
			_Public|_Const, _F(System_ecrobotGetLightSensor), TY_Int, TY_System, MN_("ecrobotGetLightSensor"), 0,
			_Public|_Const, _F(System_nxtMotorSetSpeed), TY_void, TY_System, MN_("nxtMotorSetSpeed"), 2, TY_Int, FN_x, TY_Int, FN_y,
			_Public|_Const, _F(System_nxtMotorSetCount), TY_void, TY_System, MN_("nxtMotorSetCount"), 2, TY_Int, FN_x, TY_Int, FN_y,
			_Public|_Const, _F(System_nxtMotorGetCount), TY_Int, TY_System, MN_("nxtMotorGetCount"), 1, TY_Int, FN_x, 
			_Public|_Const, _F(System_staCyc), TY_void, TY_System, MN_("staCyc"), 0,
			_Public|_Const, _F(System_waiSem), TY_void, TY_System, MN_("waiSem"), 0,
			_Public|_Const, _F(System_balanceControl), TY_void, TY_System, MN_("balanceControl"), 2, TY_Float, FN_x, TY_Float, FN_y, 
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
