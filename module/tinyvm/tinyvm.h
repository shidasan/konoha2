#ifndef TYNY_VM_H
#define TYNY_VM_H

#include "../../src/vm/vm.h"
struct vmop {
    unsigned opcode:6;
    unsigned flags:2;
    signed p0:8;
    signed p1:8;
    signed p2:8;
    signed p3:8;
    union {
        void *data;
        kMethod *mtd;
    };
};

/* ------------------------------------------------------------------------ */

/* KCODE */
#define rshift(rbp, x_) (rbp+(x_))
#define SFP(rbp)  ((ksfp_t*)(rbp))
#define SFPIDX(n) ((n)/2)
#define RBP(sfp)  ((krbp_t*)(sfp))

#define R_NEXTIDX (K_NEXTIDX)
#define Rn_(x)    (rshift(rbp,x)->ndata)
#define Ri_(x)    (rshift(rbp,x)->ivalue)
#define Rf_(x)    (rshift(rbp,x)->fvalue)
#define Rb_(x)    (rshift(rbp,x)->bvalue)
#define Ro_(x)    (rshift(rbp,x)->o)
#define Rh_(x)    (rshift(rbp,x)->hdr)
#define Rba_(x)   (rshift(rbp,x)->ba)
#define Ra_(x)    (rshift(rbp,x)->a)
#define Rx_(x)    (rshift(rbp,x)->ox)

#define RXo_(x)    (Rx_(x.i)->fields[x.n])

/* ------------------------------------------------------------------------ */

#define OPEXEC_bNOT(ctx, c, a)     Rb_(c) = !(Rb_(a))

#define OPEXEC_iINC(a)       Ri_(a)++
#define OPEXEC_iDEC(a)       Ri_(a)--

#define OPEXEC_iNEG(c, a)     Ri_(c) = -(Ri_(a))
#define OPEXEC_iTR(c, a, f)      Ri_(c) = f((long)Ri_(a))
#define OPEXEC_iADD(c, a, b)  Ri_(c) = (Ri_(a) + Ri_(b))
#define OPEXEC_iADDC(c, a, n) Ri_(c) = (Ri_(a) + n)
#define OPEXEC_iSUB(c, a, b)  Ri_(c) = (Ri_(a) - Ri_(b))
#define OPEXEC_iSUBC(c, a, n) Ri_(c) = (Ri_(a) - n)
#define OPEXEC_iMUL(c, a, b)  Ri_(c) = (Ri_(a) * Ri_(b))
#define OPEXEC_iMULC(c, a, n) Ri_(c) = (Ri_(a) * n)
#define OPEXEC_iDIV(c, a, b)  Ri_(c) = (Ri_(a) / Ri_(b));
#define OPEXEC_iDIV2(ctx, c, a, b)  { \
		SYSLOG_iZERODIV(ctx, sfp, Ri_(b)); \
		Ri_(c) = (Ri_(a) / Ri_(b)); \
	} \

#define OPEXEC_iDIVC(c, a, n)  Ri_(c) = (Ri_(a) / n)
#define OPEXEC_iMOD(c, a, b)  Ri_(c) = (Ri_(a) % Ri_(b))
#define OPEXEC_iMOD2(c, a, b)  { \
		SYSLOG_iZERODIV(ctx, sfp, Ri_(b)); \
		Ri_(c) = (Ri_(a) % Ri_(b)); \
	} \

#define OPEXEC_iMODC(c, a, n)  Ri_(c) = (Ri_(a) % n)
#define OPEXEC_iEQ(c, a, b)  Rb_(c) = (Ri_(a) == Ri_(b));
#define OPEXEC_iEQC(c, a, n)  Rb_(c) = (Ri_(a) == n);
#define OPEXEC_iNEQ(c, a, b)  Rb_(c) = (Ri_(a) != Ri_(b));
#define OPEXEC_iNEQC(c, a, n)  Rb_(c) = (Ri_(a) != n);
#define OPEXEC_iLT(c, a, b)  Rb_(c) = (Ri_(a) < Ri_(b));
#define OPEXEC_iLTC(c, a, n)  Rb_(c) = (Ri_(a) < n);
#define OPEXEC_iLTE(c, a, b)  Rb_(c) = (Ri_(a) <= Ri_(b));
#define OPEXEC_iLTEC(c, a, n)  Rb_(c) = (Ri_(a) <= n);
#define OPEXEC_iGT(c, a, b)  Rb_(c) = (Ri_(a) > Ri_(b));
#define OPEXEC_iGTC(c, a, n)  Rb_(c) = (Ri_(a) > n);
#define OPEXEC_iGTE(c, a, b)  Rb_(c) = (Ri_(a) >= Ri_(b));
#define OPEXEC_iGTEC(c, a, n)  Rb_(c) = (Ri_(a) >= n);

#define OPEXEC_iANDC(c, a, n)  Ri_(c) = (Ri_(a) & (n))
#define OPEXEC_iORC(c, a, n)   Ri_(c) = (Ri_(a) | (n))
#define OPEXEC_iXORC(c, a, n)  Ri_(c) = (Ri_(a) ^ (n))
#define OPEXEC_iLSFTC(c, a, n) Ri_(c) = (Ri_(a) << (n))
#define OPEXEC_iRSFTC(c, a, n) Ri_(c) = (Ri_(a) >> (n))
#define OPEXEC_iAND(c, a, b)   OPEXEC_iANDC(c, a, Ri_(b))
#define OPEXEC_iOR(c, a, b)    OPEXEC_iORC(c, a, Ri_(b))
#define OPEXEC_iXOR(c, a, b)   OPEXEC_iXORC(c, a, Ri_(b))
#define OPEXEC_iLSFT(c, a, b)  OPEXEC_iLSFTC(c, a, Ri_(b))
#define OPEXEC_iRSFT(c, a, b)  OPEXEC_iRSFTC(c, a, Ri_(b))

#define BR_(EXPR, PC, JUMP) if(EXPR) {} else {OPEXEC_JMP(PC, JUMP); }

#define OPEXEC_bJNUL(PC, JUMP, a)    BR_(IS_NULL(Ro_(a)), PC, JUMP)
#define OPEXEC_bJNN(PC, JUMP, a)     BR_(IS_NOTNULL(Ro_(a)), PC, JUMP)
#define OPEXEC_bJNOT(PC, JUMP, a)     BR_(!Rb_(a), PC, JUMP)
#define OPEXEC_iJEQ(PC, JUMP, a, b)   BR_((Ri_(a) == Ri_(b)), PC, JUMP)
#define OPEXEC_iJEQC(PC, JUMP, a, n)  BR_((Ri_(a) == n), PC, JUMP)
#define OPEXEC_iJNEQ(PC, JUMP, a, b)  BR_((Ri_(a) != Ri_(b)), PC, JUMP)
#define OPEXEC_iJNEQC(PC, JUMP, a, n) BR_((Ri_(a) != n), PC, JUMP)
#define OPEXEC_iJLT(PC, JUMP, a, b)   BR_((Ri_(a) < Ri_(b)), PC, JUMP)
#define OPEXEC_iJLTC(PC, JUMP, a, n)  BR_((Ri_(a) < n), PC, JUMP)
#define OPEXEC_iJLTE(PC, JUMP, a, b)  BR_((Ri_(a) <= Ri_(b)), PC, JUMP)
#define OPEXEC_iJLTEC(PC, JUMP, a, n) BR_((Ri_(a) <= n), PC, JUMP)
#define OPEXEC_iJGT(PC, JUMP, a, b)   BR_((Ri_(a) > Ri_(b)), PC, JUMP)
#define OPEXEC_iJGTC(PC, JUMP, a, n)  BR_((Ri_(a) > n), PC, JUMP)
#define OPEXEC_iJGTE(PC, JUMP, a, b)  BR_((Ri_(a) >= Ri_(b)), PC, JUMP)
#define OPEXEC_iJGTEC(PC, JUMP, a, n) BR_((Ri_(a) >= n), PC, JUMP)

/* ------------------------------------------------------------------------ */

#define OPEXEC_fNEG(c, a)     Rf_(c) = -(Rf_(a))
#define OPEXEC_fTR(c, a, f)      Rf_(c) = f((double)Rf_(a))
#define OPEXEC_fADD(c, a, b)  Rf_(c) = (Rf_(a) + Rf_(b))
#define OPEXEC_fADDC(c, a, n) Rf_(c) = (Rf_(a) + n)
#define OPEXEC_fSUB(c, a, b)  Rf_(c) = (Rf_(a) - Rf_(b))
#define OPEXEC_fSUBC(c, a, n) Rf_(c) = (Rf_(a) - n)
#define OPEXEC_fMUL(c, a, b)  Rf_(c) = (Rf_(a) * Rf_(b))
#define OPEXEC_fMULC(c, a, n) Rf_(c) = (Rf_(a) * n)
#define OPEXEC_fDIV(c, a, b)  Rf_(c) = (Rf_(a) / Rf_(b))
#define OPEXEC_fDIV2(ctx, c, a, b)  { \
		SYSLOG_fZERODIV2(ctx, sfp, Rf_(b)); \
		Rf_(c) = (Rf_(a) / Rf_(b)); \
	} \

#define OPEXEC_fDIVC(c, a, n)  Rf_(c) = (Rf_(a) / n)
#define OPEXEC_fEQ(c, a, b) Rb_(c) = (Rf_(a) == Rf_(b))
#define OPEXEC_fEQC(c, a, n) Rb_(c) = (Rf_(a) == n)
#define OPEXEC_fNEQ(c, a, b)  Rb_(c) = (Rf_(a) != Rf_(b))
#define OPEXEC_fNEQC(c, a, n)  Rb_(c) = (Rf_(a) != n)
#define OPEXEC_fLT(c, a, b)  Rb_(c) = (Rf_(a) < Rf_(b))
#define OPEXEC_fLTC(c, a, n)  Rb_(c) = (Rf_(a) < n)

#define OPEXEC_fLTE(c, a, b)  Rb_(c) = (Rf_(a) <= Rf_(b))
#define OPEXEC_fLTEC(c, a, n) Rb_(c) = (Rf_(a) <= n)
#define OPEXEC_fGT(c, a, b)  Rb_(c) = (Rf_(a) > Rf_(b))
#define OPEXEC_fGTC(c, a, n)  Rb_(c) = (Rf_(a) > n)
#define OPEXEC_fGTE(c, a, b)  Rb_(c) = (Rf_(a) >= Rf_(b))
#define OPEXEC_fGTEC(c, a, n)  Rb_(c) = (Rf_(a) >= n)

#define OPEXEC_fJEQ(PC, JUMP, a, b)   BR_((Rf_(a) == Rf_(b)), PC, JUMP)
#define OPEXEC_fJEQC(PC, JUMP, a, n)  BR_((Rf_(a) == n), PC, JUMP)
#define OPEXEC_fJNEQ(PC, JUMP, a, b)  BR_((Rf_(a) != Rf_(b)), PC, JUMP)
#define OPEXEC_fJNEQC(PC, JUMP, a, n) BR_((Rf_(a) != n), PC, JUMP)
#define OPEXEC_fJLT(PC, JUMP, a, b)   BR_((Rf_(a) < Rf_(b)), PC, JUMP)
#define OPEXEC_fJLTC(PC, JUMP, a, n)  BR_((Rf_(a) < n), PC, JUMP)
#define OPEXEC_fJLTE(PC, JUMP, a, b)  BR_((Rf_(a) <= Rf_(b)), PC, JUMP)
#define OPEXEC_fJLTEC(PC, JUMP, a, n) BR_((Rf_(a) <= n), PC, JUMP)
#define OPEXEC_fJGT(PC, JUMP, a, b)   BR_((Rf_(a) > Rf_(b)), PC, JUMP)
#define OPEXEC_fJGTC(PC, JUMP, a, n)  BR_((Rf_(a) > n), PC, JUMP)
#define OPEXEC_fJGTE(PC, JUMP, a, b)  BR_((Rf_(a) >= Rf_(b)), PC, JUMP)
#define OPEXEC_fJGTEC(PC, JUMP, a, n) BR_((Rf_(a) >= n), PC, JUMP)

/* ------------------------------------------------------------------------ */

#define klr_mov(v1, v2) {\
	kObject *v1_ = (kObject*)v1;\
	kObject *v2_ = (kObject*)v2;\
	v1 = v2_;\
}\

#define OPEXEC_bNUL(c, a)  Rb_(c) = IS_NULL(Ro_(a))
#define OPEXEC_bNN(c, a)   Rb_(c) = IS_NOTNULL(Ro_(a))

#define OPEXEC_OMOV(A, B) rbp[(A)].ndata = rbp[(B)].ndata

#define OPEXEC_iCAST(c, a) {\
	Ri_(c) = (kint_t)Rf_(a); \
}\

#define OPEXEC_fCAST(c, a) {\
	Rf_(c) = (kfloat_t)Ri_(a); \
}\

#define OPEXEC_RET() { \
		(void)op;\
		intptr_t vshift = rbp[K_SHIFTIDX2].shift;\
		kopl_t *vpc = rbp[K_PCIDX2].pc;\
		rbp[K_MTDIDX2].mtdNC = NULL;\
		rbp = rshift(rbp, -vshift); \
		pc = vpc; \
		GOTO_PC(pc);\
	}\

#define OPEXEC_EXIT() {\
		(void)op;\
		pc = NULL; \
		goto L_RETURN;\
	}\

#endif /* end of include guard */
