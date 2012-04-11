/*
 * socket_glue.h
 *
 *  Created on: 2012/04/11
 *      Author: shinpei
 */

#ifndef SOCKET_GLUE_H_
#define SOCKET_GLUE_H_

#define WORD2INT(val) (sizeof(val)==8) ? (val&0x7FFFFFFF)|((val>>32)&0x80000000) : val

#define KNH_TODO(msg) do {\
	fprintf(stderr, "TODO(%s) : %s at %s:%d",\
			msg, __FUNCTION__, __FILE__, __LINE__);\
	abort();\
} while (0)
#define KNH_NTRACE2(...) KNH_TODO("ntrace")

#endif /* SOCKET_GLUE_H_ */
