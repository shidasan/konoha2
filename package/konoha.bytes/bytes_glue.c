//#include<konoha2/konoha2.h>
//#include<konoha2/sugar.h>
//#include<konoha2/float.h>
//
//#include"bytes_glue.h"
//
//// --------------------------------------------------------------------------
//
//static	kbool_t bytes_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
//{
//	return share_initbytes(_ctx, ks, pline);
//}
//
//static kbool_t bytes_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
//{
//	return true;
//}
//
//static kbool_t bytes_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
//{
//	return local_initbytes(_ctx, ks, pline);
//}
//
//static kbool_t bytes_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
//{
//	return true;
//}
//
//KPACKDEF* bytes_init(void)
//{
//	static KPACKDEF d = {
//		KPACKNAME("bytes", "1.0"),
//		.initPackage = bytes_initPackage,
//		.setupPackage = bytes_setupPackage,
//		.initKonohaSpace = bytes_initKonohaSpace,
//		.setupKonohaSpace = bytes_setupKonohaSpace,
//	};
//	return &d;
//}
