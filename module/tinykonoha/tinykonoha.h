#ifndef SAMPLE_H
#define SAMPLE_H

#include <t_services.h>

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

extern void TaskMain(VP_INT exinf);
extern void TaskDisp(VP_INT exinf);
extern void cyc0(VP_INT exinf);

#endif

#ifdef __cplusplus
}
#endif

#endif
