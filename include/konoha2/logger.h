/*
 * logger.h
 *
 *  Created on: Apr 16, 2012
 *      Author: kimio
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define LOGPOL_RECORD       (1<<0)
#define LOGPOL_CRIT         (1<<1)
#define LOGPOL_ERR          (0)  /*DEFAULT */
#define LOGPOL_WARN         (1<<2)
#define LOGPOL_INFO         (1<<3)
#define LOGPOL_DEBUG        (1<<4)
#define LOGPOL_PREACTION    (1<<5)
#define LOGPOL_CHANGE       (1<<6)
#define LOGPOL_AUDIT        (1<<7)
#define LOGPOL_INIT         (1<<8)
#define LOGPOL_CFUNC        (1<<9)

typedef struct {
	int policy;
	void *ptr;
	union {
		const char *func;
		const struct _kMethod *mtd;
	};
} klogconf_t ;

#define LOG_END 0
#define LOG_s   1,
#define LOG_s_  1
#define LOG_u   2,
#define LOG_u_  2

#define ktrace(POLICY, ...)    do {\
		static klogconf_t _logconf = {LOGPOL_RECORD|LOGPOL_INIT|LOGPOL_CFUNC|POLICY, NULL, {__FUNCNAME__}};\
		if(TFLAG_is(int, _logconf.policy, LOGPOL_RECORD)) {\
			(KPI)->Ktrace(_ctx, &_logconf, ## __VA_ARGS__, LOG_END);\
		}\
	}while(0)\


#endif /* LOGGER_H_ */
