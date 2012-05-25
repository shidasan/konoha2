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

#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>
#include <unistd.h>
#include <signal.h>

//## @Static @Public Int System.getpid();

static KMETHOD System_getPid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getpid());
}

static KMETHOD System_getPpid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getppid());
}
static KMETHOD System_getPgid(CTX, ksfp_t *sfp _RIX)
{
	int pid = sfp[1].ivalue;
	int gid = getpgid(pid);
	RETURNi_(gid);
}


// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Static   kMethod_Static
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

#define _KVi(T) #T, TY_Int, T

static	kbool_t process_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	intptr_t MethodData[] = {
		_Public|_Static, _F(System_getPid), TY_Int, TY_System, MN_("getPid"), 0,
		_Public|_Static, _F(System_getPpid), TY_Int, TY_System, MN_("getPpid"), 0,
		_Public|_Static, _F(System_getPgid), TY_Int, TY_System, MN_("getPgid"), 1, TY_Int, FN_("pid"),
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	KDEFINE_INT_CONST IntData[] = {
		{_KVi(SIGHUP)},
		{_KVi(SIGINT)},
		{_KVi(SIGABRT)},
		{_KVi(SIGKILL)},
		{}
	};
	kKonohaSpace_loadConstData(kmodsugar->rootks, IntData, 0);
	return true;
}

static kbool_t process_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t process_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t process_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* process_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("package.posix", "1.0"),
		KPACKLIB("POSIX.1", "1.0"),
		.initPackage = process_initPackage,
		.setupPackage = process_setupPackage,
		.initKonohaSpace = process_initKonohaSpace,
		.setupKonohaSpace = process_setupKonohaSpace,
	};
	return &d;
}
