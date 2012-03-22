/*
 * konoha2_inlinelibs.h
 *
 *  Created on: Mar 15, 2012
 *      Author: kimio
 */

#ifndef KONOHA2_INLINELIBS_H_
#define KONOHA2_INLINELIBS_H_

static inline uintptr_t strhash(const char *name, size_t len)
{
	uintptr_t i, hcode = 0;
	for(i = 0; i < len; i++) {
		hcode = name[i] + (31 * hcode);
	}
	return hcode;
}


#endif /* KONOHA2_INLINELIBS_H_ */
