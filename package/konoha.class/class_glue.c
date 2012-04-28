#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

// --------------------------------------------------------------------------

#include"class_glue.h"

KDEFINE_PACKAGE* class_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("class", "1.0"),
		.initPackage = class_initPackage,
		.setupPackage = class_setupPackage,
		.initKonohaSpace = class_initKonohaSpace,
		.setupKonohaSpace = class_setupKonohaSpace,
	};
	return &d;
}
