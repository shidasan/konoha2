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
static uintptr_t *get_addr(void *addr, intptr_t offset, intptr_t datasize)
{
	return (uintptr_t*)(((char*)addr)+offset);
}
#define PTR_SIZE (sizeof(void*)*8)
//## int Pointer.get(int addr, int offset, int sizeof);
static KMETHOD Pointer_get(CTX, ksfp_t *sfp _RIX)
{
	intptr_t size   = sfp[3].ivalue;
	uintptr_t *p = get_addr((void*)sfp[1].ivalue, sfp[2].ivalue, size);
	assert(size <= PTR_SIZE);
	RETURNi_(*p & (~0UL >> (PTR_SIZE - size)));
}

//## void Pointer.set(int addr, int offset, int sizeof, int data);
static KMETHOD Pointer_set(CTX, ksfp_t *sfp _RIX)
{
	intptr_t size   = sfp[3].ivalue;
	uintptr_t data  = sfp[4].ivalue;
	uintptr_t *p = get_addr((void*)sfp[1].ivalue, sfp[2].ivalue, sfp[3].ivalue);
	uintptr_t mask = (~0UL >> (PTR_SIZE - size));
	*p = (data & mask) | (*p & ~mask);
	assert(size <= PTR_SIZE);
	RETURNvoid_();
}

//## int Object.getAddr();
static KMETHOD Object_getAddr(CTX, ksfp_t *sfp _RIX)
{
	uintptr_t p = (uintptr_t)sfp[0].o;
	RETURNi_(p);
}

//## var Pointer.toObject(int addr);
static KMETHOD Pointer_toObject(CTX, ksfp_t *sfp _RIX)
{
	uintptr_t addr = (uintptr_t) sfp[1].ivalue;
	kObject *o = (kObject*)addr;
	RETURN_(o);
}

////FIXME TODO stupid down cast
//static KMETHOD Object_toExpr(CTX, ksfp_t *sfp _RIX)
//{
//	(void)_ctx;
//	RETURN_(sfp[0].o);
//}


static kbool_t native_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{

	static KDEFINE_CLASS PointerDef = {
		.structname = "Pointer"/*structname*/,
		.cid = CLASS_newid/*cid*/,
	};
	kclass_t *cPointer = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &PointerDef, pline);
#define TY_Pointer cPointer->cid
#define _Public   kMethod_Public
#define _Static   kMethod_Static
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)
#define TY_O  TY_Object
	int FN_x = FN_("x");
	int FN_y = FN_("y");
	int FN_z = FN_("z");
	int FN_w = FN_("w");
	intptr_t MethodData[] = {
		_Public|_Static, _F(Pointer_get), TY_Int,  TY_Pointer, MN_("get"),3, TY_Int, FN_x, TY_Int, FN_y, TY_Int, FN_z, 
		_Public|_Static, _F(Pointer_set), TY_void, TY_Pointer, MN_("set"),4, TY_Int, FN_x, TY_Int, FN_y, TY_Int, FN_z,TY_Int, FN_w,
		_Public|_Coercion, _F(Object_getAddr), TY_Int, TY_O, MN_("getAddr"), 0,
		//_Public|_Coercion, _F(Object_toStmt), TY_Stmt, TY_Object, MN_to(TY_Stmt), 0,

#define TO(T) _Public|_Static, _F(Pointer_toObject), TY_##T, TY_Object, MN_("convertTo" # T), 1, TY_Int, FN_x
		TO(Array),
		TO(Object),
		TO(Method),
		TO(String),
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t native_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t native_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t native_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* native_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("jit", "1.0"),
		.initPackage = native_initPackage,
		.setupPackage = native_setupPackage,
		.initKonohaSpace = native_initKonohaSpace,
		.setupKonohaSpace = native_setupKonohaSpace,
	};
	return &d;
}
