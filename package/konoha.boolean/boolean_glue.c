/*
 * boolean_glue.c
 *
 *  Created on: 2012/04/17
 *      Author: shinpei
 */


#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

#include"boolean_glue.h"


KDEFINE_PACKAGE* boolean_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("boolean", "1.0"),
		.initPackage = boolean_initPackage,
		.setupPackage = boolean_setupPackage,
		.initKonohaSpace = boolean_initKonohaSpace,
		.setupKonohaSpace = boolean_setupKonohaSpace,
	};
	return &d;
}
