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
#include<konoha2/float.h>

// operator only
#include"assignment_glue.h"
#include"while_glue.h"

// class and operator
#include"class_glue.h"
#include"global_glue.h"

// class and operator
//#include"while_glue.h"


// class and operator
#include"../konoha.float/float_glue.h"

// method and operator
#include"array_glue.h"


// --------------------------------------------------------------------------

static int loading_float = 0;

static	kbool_t konoha_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	assignment_initPackage(_ctx, ks, argc, args, pline);
	while_initPackage(_ctx, ks, argc, args, pline);
	class_initPackage(_ctx, ks, argc, args, pline);
	global_initPackage(_ctx, ks, argc, args, pline);

	if(_ctx->modshare[MOD_float] == NULL) {
		loading_float = 1;
		float_initPackage(_ctx, ks, argc, args, pline);
	}

	array_initPackage(_ctx, ks, argc, args, pline);
	return true;
}

static kbool_t konoha_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	assignment_setupPackage(_ctx, ks, pline);
	while_setupPackage(_ctx, ks, pline);
	class_setupPackage(_ctx, ks, pline);
	global_setupPackage(_ctx, ks, pline);

	if(loading_float) {
		float_setupPackage(_ctx, ks, pline);
	}
	array_setupPackage(_ctx, ks, pline);
	return true;
}

static kbool_t konoha_initKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	assignment_initKonohaSpace(_ctx, ks, pline);
	while_initKonohaSpace(_ctx, ks, pline);
	class_initKonohaSpace(_ctx, ks, pline);
	DBG_P("hoge");
	global_initKonohaSpace(_ctx, ks, pline);
	DBG_P("hage2");

	if(loading_float) {
		float_initKonohaSpace(_ctx, ks, pline);
	}
	array_initKonohaSpace(_ctx, ks, pline);
	return true;
}

static kbool_t konoha_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	assignment_setupKonohaSpace(_ctx, ks, pline);
	while_setupKonohaSpace(_ctx, ks, pline);
	class_setupKonohaSpace(_ctx, ks, pline);
	global_setupKonohaSpace(_ctx, ks, pline);

	if(loading_float) {
		float_setupKonohaSpace(_ctx, ks, pline);
	}
	array_setupKonohaSpace(_ctx, ks, pline);
	return true;
}

KDEFINE_PACKAGE* konoha_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("konoha", "1.0"),
		.initPackage = konoha_initPackage,
		.setupPackage = konoha_setupPackage,
		.initKonohaSpace = konoha_initKonohaSpace,
		.setupKonohaSpace = konoha_setupKonohaSpace,
	};
	return &d;
}

