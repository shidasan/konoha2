/*
 * testkonoha.h
 *
 *  Created on: Apr 12, 2012
 *      Author: kimio
 */

#ifndef TESTKONOHA_H_
#define TESTKONOHA_H_

#define MAX 1000

static uintptr_t keys[] = {
	1, 3, 2, 3, 8, 1, 2, 4, 9, 12, 2, 19, 3, 2, 7, 9, 10,
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	40, 80, 90, 100, 1000, 30, 32, 33, 45, 12, 13, 14, 15
};

static int checkkvs(size_t key, uintptr_t val)
{
	long i;
	for(i= (sizeof(keys)/sizeof(uintptr_t)) - 1; i >= 0; i--) {
		if(keys[i] == key) {
			return (val == i);
		}
	}
	return false;
}

static int test_kvproto(CTX)
{
	size_t i;
	kObject *o = K_NULL;
	assert(sizeof(kvs_t) == sizeof(void*) * 2);
	for(i=0; i < sizeof(keys)/sizeof(uintptr_t); i++) {
		ksymbol_t key = (ksymbol_t)keys[i];
		kObject_setUnboxedValue(o, key, TY_Int, i);
	}
	for(i=0; i < sizeof(keys)/sizeof(uintptr_t); i++) {
		ksymbol_t key = (ksymbol_t)keys[i];
		uintptr_t val = kObject_getUnboxedValue(o, key, -1);
		//DBG_P("key=%d, val=%d, %d, results=%d", key, val, i, checkkvs(key, val));
		assert(checkkvs(key, val));
	}
	for(i=0; i < sizeof(keys)/sizeof(uintptr_t); i++) {
		ksymbol_t key = (ksymbol_t)keys[i];
		kObject_removeKey(o, key);
	}
	for(i=0; i < sizeof(keys)/sizeof(uintptr_t); i++) {
		ksymbol_t key = (ksymbol_t)keys[i];
		intptr_t val = (intptr_t)kObject_getUnboxedValue(o, key, -1);
		assert(val == -1); // removed
	}
	return 0;
}

static int test_kwb(CTX)
{
	const char *t= "0123456789012345678901234567890123456789";
	size_t i = 0;
	karray_t *buf = new_karray(_ctx, 0, 64);
	kwb_t wb;
	kwb_init(buf, &wb);
	kwb_printf(&wb, "%s%s%s%s", t, t, t, t);
	t = kwb_top(&wb, 1);
	while(*t != 0) {
//		DBG_P("i=%d, ch='%c', '%c'", i, *t, '0' + (i % 10));
		assert(*t == '0' + (i % 10));
		i++; t++;
	}
	assert(i == kwb_bytesize(&wb));
	kwb_free(&wb);
	KARRAY_FREE(buf);
	KNH_FREE(buf, sizeof(karray_t));
	return 0;
}

#include "konoha2/logger.h"

static int test_logger(CTX)
{
	int v2 = 3;
	int v3 = -3;
	int i;
	for (i = 0; i < 10; i++) {
		ktrace(LOGPOL_ERR,
				LOG_s "key1", "hello world",
				LOG_u "key2", v2,
				LOG_u "key3", v3
				);
	}
	return 0;
}
#define FN(T)  #T, T

DEFINE_TESTFUNC KonohaTestSet[] = {
	{FN(test_kvproto)},
	{FN(test_kwb)},
	{FN(test_logger)},
	{NULL, NULL},
};

#endif /* TESTKONOHA_H_ */
