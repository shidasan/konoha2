#ifndef SAMPLE_H
#define SAMPLE_H

#ifdef K_USING_TINYVM
#include <t_services.h>
#endif
#define K_USING_THCODE_ 1
#define MAX_CT     16

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MACRO_ONLY


enum {
	MINIT = 0,
	MWAIT,
	MRUNNING,
	MSTOP1,
	MSTOP2,
	MSTOP3,
	MPREWAIT
};

#ifdef K_USING_TINYVM
extern void TaskMain(VP_INT exinf);
extern void TaskDisp(VP_INT exinf);
extern void cyc0(VP_INT exinf);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
