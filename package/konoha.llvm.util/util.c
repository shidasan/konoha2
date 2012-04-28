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

#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
// --------------------------------------------------------------------------
static kArray *create_array(CTX, ksfp_t *sfp, int n)
{
	int i;
	kArray *a = new_(Array, 0);
	for (i = 1; i <= n; ++i) {
		kArray_add(a, sfp[i].o);
	}
	return a;
}

// --------------------------------------------------------------------------
//## static Array Array.new1(Object o1, Object o2, Object o3);
static KMETHOD Array_new1(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(create_array(_ctx, sfp, 1));
}

// --------------------------------------------------------------------------
//## static Array Array.new2(Object o1, Object o2);
static KMETHOD Array_new2(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(create_array(_ctx, sfp, 2));
}

// --------------------------------------------------------------------------
//## static Array Array.new3(Object o1, Object o2, Object o3);
static KMETHOD Array_new3(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(create_array(_ctx, sfp, 3));
}
// --------------------------------------------------------------------------
//## Array Array.getO();
static KMETHOD Array_getO(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	size_t n = check_index(_ctx, sfp[1].ivalue, kArray_size(a), sfp[K_RTNIDX].uline);
	RETURN_(a->list[n]);
}
// --------------------------------------------------------------------------
//## void Array.setO(int n, Object o);
static KMETHOD Array_setO(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	size_t n = check_index(_ctx, sfp[1].ivalue, kArray_size(a), sfp[K_RTNIDX].uline);
	KSETv(a->list[n], sfp[2].o);
	RETURNvoid_();
}

#define _Public   kMethod_Public
#define _Static   kMethod_Static
#define _Im       kMethod_Immutable
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t util_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	int FN_x = FN_("x");
	int FN_y = FN_("y");
	int FN_z = FN_("z");
#define TY_O TY_Object
	intptr_t MethodData[] = {
		_Public|_Static, _F(Array_new1), TY_Array, TY_Array, MN_("new1"), 1, TY_O, FN_x,
		_Public|_Static, _F(Array_new2), TY_Array, TY_Array, MN_("new2"), 2, TY_O, FN_x, TY_O, FN_y,
		_Public|_Static, _F(Array_new3), TY_Array, TY_Array, MN_("new3"), 3, TY_O, FN_x, TY_O, FN_y, TY_O, FN_z,
		_Public, _F(Array_getO), TY_Object, TY_Array, MN_("get"), 1, TY_Int, FN_x,
		_Public, _F(Array_setO), TY_void,   TY_Array, MN_("set"), 2, TY_Int, FN_x, TY_O, FN_y,
		//_Public|_Const|_Im|_Coercion, _F(Float_toInt), TY_Int, TY_Float, MN_to(TY_Int), 0,
		//_Public|_Const|_Im, _F(String_toFloat), TY_Float, TY_String, MN_to(TY_Float), 0,
		DEND,
	};
#undef TY_O
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t util_setup_nop(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* util_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("util", "1.0"),
		.initPackage = util_initPackage,
		.setupPackage = util_setup_nop,
		.initKonohaSpace = util_setup_nop,
		.setupKonohaSpace = util_setup_nop,
	};
	return &d;
}
