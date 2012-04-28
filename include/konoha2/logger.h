/*
 * logger.h
 *
 *  Created on: Apr 16, 2012
 *      Author: kimio
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define LOGPOL_RECORD       (1<<0) /* logger works only with this flag */
 /* Critical */
#define LOGPOL_CRIT         (1<<1)
/*DEFAULT, error*/
#define LOGPOL_ERR          (0)
#define LOGPOL_WARN         (1<<2) /* Warning */
#define LOGPOL_INFO         (1<<3) /* Information */
#define LOGPOL_DEBUG        (1<<4) /* Debug information */

#define LOGPOL_PREACTION    (1<<5)
#define LOGPOL_CHANGE       (1<<6)
#define LOGPOL_AUDIT        (1<<7) /* security auditing */
 /* ??? */
#define LOGPOL_SYSTEMFAULT     (1<<8)
/* scripter's fault */
#define LOGPOL_SCRIPTFAULT     (1<<9)
#define LOGPOL_DATAFAULT       (1<<10) /* invalid data */
#define LOGPOL_UKNOWNFAULT     (1<<11) /* faults that aren't belong above */

#define LOGPOL_INIT         (1<<12) /* DONT USE THIS */
#define LOGPOL_CFUNC        (1<<13) /* DONT USE THIS */

typedef struct klogconf_t {
	int policy;
	void *ptr; // for precompiled formattings
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

#define KEYVALUE_u(K,V)\
	LOG_u_, K, (unsigned int)V

#define KEYVALUE_s(K,V)\
	LOG_s_, K, (char*)V

#define ktrace(POLICY, ...)    do {\
		static klogconf_t _logconf = {LOGPOL_RECORD|LOGPOL_INIT|LOGPOL_CFUNC|POLICY, NULL, {__FUNCTION__}};\
		if(TFLAG_is(int, _logconf.policy, LOGPOL_RECORD)) {\
			(KPI)->Ktrace(_ctx, &_logconf, ## __VA_ARGS__, LOG_END);\
		}\
	}while(0)\


#endif /* LOGGER_H_ */
