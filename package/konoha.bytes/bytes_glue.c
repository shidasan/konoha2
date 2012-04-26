#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

#include "bytes_glue.h"

KDEFINE_PACKAGE* bytes_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("bytes", "1.0"),
		.initPackage = bytes_initPackage,
		.setupPackage = bytes_setupPackage,
		.initKonohaSpace = bytes_initKonohaSpace,
		.setupKonohaSpace = bytes_setupKonohaSpace,
	};
	return &d;
}


