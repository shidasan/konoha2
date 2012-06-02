#ifndef TINYVM_OPCODES_H
#define TINYVM_OPCODES_H

#include "tinyvm.h"

#define OPCODE_NSET ((kopcode_t)0)
typedef struct klr_NSET_t {
	int8_t opcode;
	int8_t/* rn */ a;
	int32_t/* int */ n;
	uint16_t/* cid */ ty;
} klr_NSET_t;

#define OPCODE_NMOV ((kopcode_t)1)
typedef struct klr_NMOV_t {
	int8_t opcode;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
	uint16_t/* cid */ ty;
} klr_NMOV_t;

#define OPCODE_NMOVx ((kopcode_t)2)
typedef struct klr_NMOVx_t {
	int8_t opcode;
	int8_t/* rn */ a;
	int8_t/* ro */ b;
	uint16_t/* u */ bx;
	uint16_t/* cid */ ty;
} klr_NMOVx_t;

#define OPCODE_XNMOV ((kopcode_t)3)
typedef struct klr_XNMOV_t {
	int8_t opcode;
	int8_t/* ro */ a;
	uint16_t/* u */ ax;
	int8_t/* rn */ b;
	uint16_t/* cid */ ty;
} klr_XNMOV_t;

#define OPCODE_NEW ((kopcode_t)4)
typedef struct klr_NEW_t {
	int8_t opcode;
	int8_t/* ro */ a;
	uint16_t/* u */ p;
	uint16_t/* cid */ ty;
} klr_NEW_t;

#define OPCODE_NULL ((kopcode_t)5)
typedef struct klr_NULL_t {
	int8_t opcode;
	int8_t/* ro */ a;
	uint16_t/* cid */ ty;
} klr_NULL_t;

#define OPCODE_BOX ((kopcode_t)6)
typedef struct klr_BOX_t {
	int8_t opcode;
	int8_t/* ro */ a;
	int8_t/* rn */ b;
	uint16_t/* cid */ ty;
} klr_BOX_t;

#define OPCODE_UNBOX ((kopcode_t)7)
typedef struct klr_UNBOX_t {
	int8_t opcode;
	int8_t/* rn */ a;
	int8_t/* ro */ b;
	uint16_t/* cid */ ty;
} klr_UNBOX_t;

#define OPCODE_CALL ((kopcode_t)8)
typedef struct klr_CALL_t {
	int8_t opcode;
	uint16_t/* u */ uline;
	int8_t/* ro */ thisidx;
	int8_t/* ro */ espshift;
	uint16_t/* co */ tyo;
} klr_CALL_t;

#define OPCODE_BNOT ((kopcode_t)9)
typedef struct klr_BNOT_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
} klr_BNOT_t;

#define OPCODE_JMP ((kopcode_t)10)
typedef struct klr_JMP_t {
	int8_t opcode;
	uint16_t jumppc;
} klr_JMP_t;

#define OPCODE_JMPF ((kopcode_t)11)
typedef struct klr_JMPF_t {
	int8_t opcode;
	uint16_t jumppc;
	int8_t/* rn */ a;
} klr_JMPF_t;

#define OPCODE_SAFEPOINT ((kopcode_t)12)
typedef struct klr_SAFEPOINT_t {
	int8_t opcode;
	int8_t/* ro */ espshift;
} klr_SAFEPOINT_t;

#define OPCODE_ERROR ((kopcode_t)13)
typedef struct klr_ERROR_t {
	int8_t opcode;
	int8_t/* ro */ start;
	kString* msg;
} klr_ERROR_t;

#define OPCODE_bNUL ((kopcode_t)14)
typedef struct klr_bNUL_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* ro */ a;
} klr_bNUL_t;

#define OPCODE_bNN ((kopcode_t)15)
typedef struct klr_bNN_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* ro */ a;
} klr_bNN_t;

#define OPCODE_iNEG ((kopcode_t)16)
typedef struct klr_iNEG_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
} klr_iNEG_t;

#define OPCODE_fNEG ((kopcode_t)17)
typedef struct klr_fNEG_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
} klr_fNEG_t;

#define OPCODE_iADD ((kopcode_t)18)
typedef struct klr_iADD_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iADD_t;

#define OPCODE_iSUB ((kopcode_t)19)
typedef struct klr_iSUB_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iSUB_t;

#define OPCODE_iMUL ((kopcode_t)20)
typedef struct klr_iMUL_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iMUL_t;

#define OPCODE_iDIV ((kopcode_t)21)
typedef struct klr_iDIV_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iDIV_t;

#define OPCODE_iMOD ((kopcode_t)22)
typedef struct klr_iMOD_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iMOD_t;

#define OPCODE_iEQ ((kopcode_t)23)
typedef struct klr_iEQ_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iEQ_t;

#define OPCODE_iNEQ ((kopcode_t)24)
typedef struct klr_iNEQ_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iNEQ_t;

#define OPCODE_iLT ((kopcode_t)25)
typedef struct klr_iLT_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iLT_t;

#define OPCODE_iLTE ((kopcode_t)26)
typedef struct klr_iLTE_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iLTE_t;

#define OPCODE_iGT ((kopcode_t)27)
typedef struct klr_iGT_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iGT_t;

#define OPCODE_iGTE ((kopcode_t)28)
typedef struct klr_iGTE_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iGTE_t;

#define OPCODE_fADD ((kopcode_t)29)
typedef struct klr_fADD_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fADD_t;

#define OPCODE_fSUB ((kopcode_t)30)
typedef struct klr_fSUB_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fSUB_t;

#define OPCODE_fMUL ((kopcode_t)31)
typedef struct klr_fMUL_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fMUL_t;

#define OPCODE_fDIV ((kopcode_t)32)
typedef struct klr_fDIV_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fDIV_t;

#define OPCODE_fEQ ((kopcode_t)33)
typedef struct klr_fEQ_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fEQ_t;

#define OPCODE_fNEQ ((kopcode_t)34)
typedef struct klr_fNEQ_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fNEQ_t;

#define OPCODE_fLT ((kopcode_t)35)
typedef struct klr_fLT_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fLT_t;

#define OPCODE_fLTE ((kopcode_t)36)
typedef struct klr_fLTE_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fLTE_t;

#define OPCODE_fGT ((kopcode_t)37)
typedef struct klr_fGT_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fGT_t;

#define OPCODE_fGTE ((kopcode_t)38)
typedef struct klr_fGTE_t {
	int8_t opcode;
	int8_t/* rn */ c;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fGTE_t;

#define OPCODE_OSET ((kopcode_t)39)
typedef struct klr_OSET_t {
	int8_t opcode;
	int8_t/* ro */ a;
	int8_t/* rn */ n;
} klr_OSET_t;

#define OPCODE_OMOV ((kopcode_t)40)
typedef struct klr_OMOV_t {
	int8_t opcode;
	int8_t/* ro */ a;
	int8_t/* ro */ b;
} klr_OMOV_t;

#define OPCODE_SCALL ((kopcode_t)41)
typedef struct klr_SCALL_t {
	int8_t opcode;
	uint16_t/* u */ uline;
	int8_t/* ro */ thisidx;
	int8_t/* ro */ espshift;
	//kMethod* mtd;
	int8_t/* cid */ cid;
	int8_t/* mn */ mn;
	uint16_t/* co */ tyo;
} klr_SCALL_t;

#define OPCODE_VCALL ((kopcode_t)42)
typedef struct klr_VCALL_t {
	int8_t opcode;
	uint16_t/* u */ uline;
	int8_t/* ro */ thisidx;
	int8_t/* ro */ espshift;
	//kMethod* mtd;
	int8_t/* cid */ cid;
	int8_t/* mn */ mn;
	uint16_t/* co */ tyo;
} klr_VCALL_t;

#define OPCODE_iCAST ((kopcode_t)43)
typedef struct klr_iCAST_t {
	int8_t opcode;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_iCAST_t;

#define OPCODE_fCAST ((kopcode_t)44)
typedef struct klr_fCAST_t {
	int8_t opcode;
	int8_t/* rn */ a;
	int8_t/* rn */ b;
} klr_fCAST_t;

#define OPCODE_RET ((kopcode_t)45)
typedef struct klr_RET_t {
	int8_t opcode;
} klr_RET_t;

#define OPCODE_EXIT ((kopcode_t)46)
typedef struct klr_EXIT_t {
	int8_t opcode;
} klr_EXIT_t;

//#define OPCODE_NGETIDX ((kopcode_t)46)
//typedef struct klr_NGETIDX_t {
//	int8_t opcode;
//	int8_t/* rn */ c;
//	int8_t/* ro */ a;
//	int8_t/* rn */ n;
//} klr_NGETIDX_t;
//
//#define OPCODE_NSETIDX ((kopcode_t)47)
//typedef struct klr_NSETIDX_t {
//	int8_t opcode;
//	int8_t/* rn */ c;
//	int8_t/* ro */ a;
//	int8_t/* rn */ n;
//	int8_t/* rn */ v;
//} klr_NSETIDX_t;
//
//#define OPCODE_OGETIDX ((kopcode_t)48)
//typedef struct klr_OGETIDX_t {
//	int8_t opcode;
//	int8_t/* ro */ c;
//	int8_t/* ro */ a;
//	int8_t/* rn */ n;
//} klr_OGETIDX_t;
//
//#define OPCODE_OSETIDX ((kopcode_t)49)
//typedef struct klr_OSETIDX_t {
//	int8_t opcode;
//	int8_t/* ro */ c;
//	int8_t/* ro */ a;
//	int8_t/* rn */ n;
//	int8_t/* ro */ v;
//} klr_OSETIDX_t;

typedef union kopl_u {
	kopl_t op;
	klr_NSET_t opNSET;
	klr_NMOV_t opNMOV;
	klr_NMOVx_t opNMOVx;
	klr_XNMOV_t opXNMOV;
	klr_NEW_t opNEW;
	klr_NULL_t opNULL;
	klr_BOX_t opBOX;
	klr_UNBOX_t opUNBOX;
	klr_CALL_t opCALL;
	klr_BNOT_t opBNOT;
	klr_JMP_t opJMP;
	klr_JMPF_t opJMPF;
	klr_SAFEPOINT_t opSAFEPOINT;
	klr_ERROR_t opERROR;
	klr_bNUL_t opbNUL;
	klr_bNN_t opbNN;
	klr_iNEG_t opiNEG;
	klr_fNEG_t opfNEG;
	klr_iADD_t opiADD;
	klr_iSUB_t opiSUB;
	klr_iMUL_t opiMUL;
	klr_iDIV_t opiDIV;
	klr_iMOD_t opiMOD;
	klr_iEQ_t opiEQ;
	klr_iNEQ_t opiNEQ;
	klr_iLT_t opiLT;
	klr_iLTE_t opiLTE;
	klr_iGT_t opiGT;
	klr_iGTE_t opiGTE;
	klr_fADD_t opfADD;
	klr_fSUB_t opfSUB;
	klr_fMUL_t opfMUL;
	klr_fDIV_t opfDIV;
	klr_fEQ_t opfEQ;
	klr_fNEQ_t opfNEQ;
	klr_fLT_t opfLT;
	klr_fLTE_t opfLTE;
	klr_fGT_t opfGT;
	klr_fGTE_t opfGTE;
	klr_OSET_t opOSET;
	klr_OMOV_t opOMOV;
	klr_SCALL_t opSCALL;
	klr_VCALL_t opVCALL;
	klr_iCAST_t opiCAST;
	klr_fCAST_t opfCAST;
	klr_RET_t opRET;
	klr_EXIT_t opEXIT;
	//klr_NGETIDX_t opNGETIDX;
	//klr_NSETIDX_t opNSETIDX;
	//klr_OGETIDX_t opOGETIDX;
	//klr_OSETIDX_t opOSETIDX;
}kopl_u;

typedef struct kconstdata_t {
	kcid_t cid;
	void *conf;
}kconstdata_t;

typedef struct kmethoddecl_t {
	kcid_t cid;
	kmethodn_t mn;
	kconstdata_t *constdata;
	kopl_u *opline;
}kmethoddecl_t;

#define KOPCODE_MAX ((kopcode_t)47)

#define VMT_VOID     0
#define VMT_ADDR     1
#define VMT_R        2
#define VMT_RN       2
#define VMT_RO       2
#define VMT_U        3
#define VMT_I        4
#define VMT_CID      5
#define VMT_CO       6
#define VMT_INT      7
#define VMT_FLOAT    8
#define VMT_HCACHE   9
#define VMT_F        10/*function*/
#define VMT_STRING   11
#define VMT_METHOD   12
#define VMT_OBJECT   13
#define VMT_SFPIDX   14


/* ------------------------------------------------------------------------ */
/* [data] */

typedef struct {
	const char *name;
	kushort_t size;
	kushort_t types[6];
} kOPDATA_t;

static const kOPDATA_t OPDATA[] = {
	{"NSET", 3, { VMT_RN, VMT_INT, VMT_CID, VMT_VOID}},
	{"NMOV", 3, { VMT_RN, VMT_RN, VMT_CID, VMT_VOID}},
	{"NMOVx", 4, { VMT_RN, VMT_RO, VMT_U, VMT_CID, VMT_VOID}},
	{"XNMOV", 4, { VMT_RO, VMT_U, VMT_RN, VMT_CID, VMT_VOID}},
	{"NEW", 3, { VMT_RO, VMT_U, VMT_CID, VMT_VOID}},
	{"NULL", 2, { VMT_RO, VMT_CID, VMT_VOID}},
	{"BOX", 3, { VMT_RO, VMT_RN, VMT_CID, VMT_VOID}},
	{"UNBOX", 3, { VMT_RN, VMT_RO, VMT_CID, VMT_VOID}},
	{"CALL", 4, { VMT_U, VMT_RO, VMT_RO, VMT_CO, VMT_VOID}},
	{"BNOT", 2, { VMT_RN, VMT_RN, VMT_VOID}},
	{"JMP", 1, { VMT_ADDR, VMT_VOID}},
	{"JMPF", 2, { VMT_ADDR, VMT_RN, VMT_VOID}},
	{"SAFEPOINT", 1, { VMT_RO, VMT_VOID}},
	{"ERROR", 2, { VMT_RO, VMT_STRING, VMT_VOID}},
	{"bNUL", 2, { VMT_RN, VMT_RO, VMT_VOID}},
	{"bNN", 2, { VMT_RN, VMT_RO, VMT_VOID}},
	{"iNEG", 2, { VMT_RN, VMT_RN, VMT_VOID}},
	{"fNEG", 2, { VMT_RN, VMT_RN, VMT_VOID}},
	{"iADD", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iSUB", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iMUL", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iDIV", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iMOD", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iEQ", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iNEQ", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iLT", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iLTE", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iGT", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"iGTE", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fADD", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fSUB", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fMUL", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fDIV", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fEQ", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fNEQ", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fLT", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fLTE", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fGT", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"fGTE", 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}},
	{"OSET", 2, { VMT_RO, VMT_RN, VMT_VOID}},
	{"OMOV", 2, { VMT_RO, VMT_RO, VMT_VOID}},
	{"SCALL", 5, { VMT_U, VMT_RO, VMT_RO, VMT_CO, VMT_METHOD, VMT_VOID}},
	{"VCALL", 5, { VMT_U, VMT_RO, VMT_RO, VMT_CO, VMT_METHOD, VMT_VOID}},
	{"iCAST", 2, { VMT_RN, VMT_RN, VMT_VOID}},
	{"fCAST", 2, { VMT_RN, VMT_RN, VMT_VOID}},
	{"EXIT", 0, { VMT_VOID}},
	//{"NGETIDX", 3, { VMT_RN, VMT_RO, VMT_RN, VMT_VOID}},
	//{"NSETIDX", 4, { VMT_RN, VMT_RO, VMT_RN, VMT_RN, VMT_VOID}},
	//{"OGETIDX", 3, { VMT_RO, VMT_RO, VMT_RN, VMT_VOID}},
	//{"OSETIDX", 4, { VMT_RO, VMT_RO, VMT_RN, VMT_RO, VMT_VOID}},
};

static void opcode_check(void)
{
	assert(sizeof(klr_NSET_t) <= sizeof(kopl_t));
	assert(sizeof(klr_NMOV_t) <= sizeof(kopl_t));
	assert(sizeof(klr_NMOVx_t) <= sizeof(kopl_t));
	assert(sizeof(klr_XNMOV_t) <= sizeof(kopl_t));
	assert(sizeof(klr_NEW_t) <= sizeof(kopl_t));
	assert(sizeof(klr_NULL_t) <= sizeof(kopl_t));
	assert(sizeof(klr_BOX_t) <= sizeof(kopl_t));
	assert(sizeof(klr_UNBOX_t) <= sizeof(kopl_t));
	assert(sizeof(klr_CALL_t) <= sizeof(kopl_t));
	assert(sizeof(klr_BNOT_t) <= sizeof(kopl_t));
	assert(sizeof(klr_JMP_t) <= sizeof(kopl_t));
	assert(sizeof(klr_JMPF_t) <= sizeof(kopl_t));
	assert(sizeof(klr_SAFEPOINT_t) <= sizeof(kopl_t));
	assert(sizeof(klr_ERROR_t) <= sizeof(kopl_t));
	assert(sizeof(klr_bNUL_t) <= sizeof(kopl_t));
	assert(sizeof(klr_bNN_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iNEG_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fNEG_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iADD_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iSUB_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iMUL_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iDIV_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iMOD_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iEQ_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iNEQ_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iLT_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iLTE_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iGT_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iGTE_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fADD_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fSUB_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fMUL_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fDIV_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fEQ_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fNEQ_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fLT_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fLTE_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fGT_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fGTE_t) <= sizeof(kopl_t));
	assert(sizeof(klr_OSET_t) <= sizeof(kopl_t));
	assert(sizeof(klr_OMOV_t) <= sizeof(kopl_t));
	assert(sizeof(klr_SCALL_t) <= sizeof(kopl_t));
	assert(sizeof(klr_VCALL_t) <= sizeof(kopl_t));
	assert(sizeof(klr_iCAST_t) <= sizeof(kopl_t));
	assert(sizeof(klr_fCAST_t) <= sizeof(kopl_t));
	assert(sizeof(klr_EXIT_t) <= sizeof(kopl_t));
	//assert(sizeof(klr_NGETIDX_t) <= sizeof(kopl_t));
	//assert(sizeof(klr_NSETIDX_t) <= sizeof(kopl_t));
	//assert(sizeof(klr_OGETIDX_t) <= sizeof(kopl_t));
	//assert(sizeof(klr_OSETIDX_t) <= sizeof(kopl_t));
}

static const char *T_opcode(kopcode_t opcode)
{
	if(opcode < KOPCODE_MAX) {
		return OPDATA[opcode].name;
	}
	else {
		fprintf(stderr, "opcode=%d\n", (int)opcode);
		return "OPCODE_??";
	}
}

/* ------------------------------------------------------------------------ */
/* [exec] */

#define CASE(x)     L_##x :
#define NEXT_OP     (pc->opcode)
#define JUMP        *OPJUMP[NEXT_OP]
#define GOTO_NEXT() goto *OPJUMP[NEXT_OP]
#define DISPATCH_START(pc) goto *OPJUMP[pc->opcode]
#define DISPATCH_END(pc)
#define GOTO_PC(pc)        GOTO_NEXT()

static kopl_t* VirtualMachine_run(CTX, ksfp_t *sfp0, kopl_t *pc)
{
	static void *OPJUMP[] = {
		&&L_NSET, &&L_NMOV, &&L_NMOVx, &&L_XNMOV, 
		&&L_NEW, &&L_NULL, &&L_BOX, &&L_UNBOX, 
		&&L_CALL, &&L_BNOT, &&L_JMP, &&L_JMPF, 
		&&L_SAFEPOINT, &&L_ERROR, &&L_bNUL, &&L_bNN, 
		&&L_iNEG, &&L_fNEG, &&L_iADD, &&L_iSUB, 
		&&L_iMUL, &&L_iDIV, &&L_iMOD, &&L_iEQ, 
		&&L_iNEQ, &&L_iLT, &&L_iLTE, &&L_iGT, 
		&&L_iGTE, &&L_fADD, &&L_fSUB, &&L_fMUL, 
		&&L_fDIV, &&L_fEQ, &&L_fNEQ, &&L_fLT, 
		&&L_fLTE, &&L_fGT, &&L_fGTE, &&L_OSET, 
		&&L_OMOV,
		&&L_SCALL, &&L_VCALL, &&L_iCAST, &&L_fCAST, 
		&&L_RET, &&L_EXIT,
		//&&L_NGETIDX, &&L_NSETIDX, &&L_OGETIDX, &&L_OSETIDX, 
	};
	krbp_t *rbp = (krbp_t*)sfp0;
	DISPATCH_START(pc);

	CASE(NSET) {
		klr_NSET_t *op = (klr_NSET_t*)pc;
		OPEXEC_NSET(op->a, op->n, op->ty); pc++;
		GOTO_NEXT();
	} 
	CASE(NMOV) {
		klr_NMOV_t *op = (klr_NMOV_t*)pc;
		OPEXEC_NMOV(op->a, op->b, op->ty); pc++;
		GOTO_NEXT();
	} 
	CASE(NMOVx) {
		klr_NMOVx_t *op = (klr_NMOVx_t*)pc;
		OPEXEC_NMOVx(op->a, op->b, op->bx, op->ty); pc++;
		GOTO_NEXT();
	} 
	CASE(XNMOV) {
		klr_XNMOV_t *op = (klr_XNMOV_t*)pc;
		OPEXEC_XNMOV(op->a, op->ax, op->b, op->ty); pc++;
		GOTO_NEXT();
	} 
	CASE(NEW) {
		klr_NEW_t *op = (klr_NEW_t*)pc;
		OPEXEC_NEW(op->a, (intptr_t)op->p, CT_(op->ty)); pc++;
		GOTO_NEXT();
	} 
	CASE(NULL) {
		klr_NULL_t *op = (klr_NULL_t*)pc;
		OPEXEC_NULL(op->a, CT_(op->ty)); pc++;
		GOTO_NEXT();
	} 
	CASE(BOX) {
		klr_BOX_t *op = (klr_BOX_t*)pc;
		OPEXEC_BOX(op->a, op->b, CT_(op->ty)); pc++;
		GOTO_NEXT();
	} 
	CASE(UNBOX) {
		klr_UNBOX_t *op = (klr_UNBOX_t*)pc;
		OPEXEC_UNBOX(op->a, op->b, op->ty); pc++;
		GOTO_NEXT();
	} 
	CASE(CALL) {
		klr_CALL_t *op = (klr_CALL_t*)pc;
		OPEXEC_CALL(op->uline, op->thisidx, op->espshift, knull(CT_(op->tyo))); pc++;
		GOTO_NEXT();
	} 
	CASE(BNOT) {
		klr_BNOT_t *op = (klr_BNOT_t*)pc;
		OPEXEC_BNOT(op->c, op->a); pc++;
		GOTO_NEXT();
	} 
	CASE(JMP) {
		klr_JMP_t *op = (klr_JMP_t*)pc;
		OPEXEC_JMP(pc += op->jumppc, JUMP); pc++;
		GOTO_NEXT();
	} 
	CASE(JMPF) {
		klr_JMPF_t *op = (klr_JMPF_t*)pc;
		OPEXEC_JMPF(pc += op->jumppc, JUMP, op->a); pc++;
		GOTO_NEXT();
	} 
	CASE(SAFEPOINT) {
		klr_SAFEPOINT_t *op = (klr_SAFEPOINT_t*)pc;
		OPEXEC_SAFEPOINT(op->espshift); pc++;
		GOTO_NEXT();
	} 
	CASE(ERROR) {
		klr_ERROR_t *op = (klr_ERROR_t*)pc;
		OPEXEC_ERROR(op->start, op->msg); pc++;
		GOTO_NEXT();
	} 
	CASE(bNUL) {
		klr_bNUL_t *op = (klr_bNUL_t*)pc;
		OPEXEC_bNUL(op->c, op->a); pc++;
		GOTO_NEXT();
	} 
	CASE(bNN) {
		klr_bNN_t *op = (klr_bNN_t*)pc;
		OPEXEC_bNN(op->c, op->a); pc++;
		GOTO_NEXT();
	} 
	CASE(iNEG) {
		klr_iNEG_t *op = (klr_iNEG_t*)pc;
		OPEXEC_iNEG(op->c, op->a); pc++;
		GOTO_NEXT();
	} 
	CASE(fNEG) {
		klr_fNEG_t *op = (klr_fNEG_t*)pc;
		OPEXEC_fNEG(op->c, op->a); pc++;
		GOTO_NEXT();
	} 
	CASE(iADD) {
		klr_iADD_t *op = (klr_iADD_t*)pc;
		OPEXEC_iADD(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iSUB) {
		klr_iSUB_t *op = (klr_iSUB_t*)pc;
		OPEXEC_iSUB(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iMUL) {
		klr_iMUL_t *op = (klr_iMUL_t*)pc;
		OPEXEC_iMUL(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iDIV) {
		klr_iDIV_t *op = (klr_iDIV_t*)pc;
		OPEXEC_iDIV(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iMOD) {
		klr_iMOD_t *op = (klr_iMOD_t*)pc;
		OPEXEC_iMOD(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iEQ) {
		klr_iEQ_t *op = (klr_iEQ_t*)pc;
		OPEXEC_iEQ(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iNEQ) {
		klr_iNEQ_t *op = (klr_iNEQ_t*)pc;
		OPEXEC_iNEQ(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iLT) {
		klr_iLT_t *op = (klr_iLT_t*)pc;
		OPEXEC_iLT(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iLTE) {
		klr_iLTE_t *op = (klr_iLTE_t*)pc;
		OPEXEC_iLTE(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iGT) {
		klr_iGT_t *op = (klr_iGT_t*)pc;
		OPEXEC_iGT(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(iGTE) {
		klr_iGTE_t *op = (klr_iGTE_t*)pc;
		OPEXEC_iGTE(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fADD) {
		klr_fADD_t *op = (klr_fADD_t*)pc;
		OPEXEC_fADD(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fSUB) {
		klr_fSUB_t *op = (klr_fSUB_t*)pc;
		OPEXEC_fSUB(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fMUL) {
		klr_fMUL_t *op = (klr_fMUL_t*)pc;
		OPEXEC_fMUL(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fDIV) {
		klr_fDIV_t *op = (klr_fDIV_t*)pc;
		OPEXEC_fDIV(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fEQ) {
		klr_fEQ_t *op = (klr_fEQ_t*)pc;
		OPEXEC_fEQ(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fNEQ) {
		klr_fNEQ_t *op = (klr_fNEQ_t*)pc;
		OPEXEC_fNEQ(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fLT) {
		klr_fLT_t *op = (klr_fLT_t*)pc;
		OPEXEC_fLT(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fLTE) {
		klr_fLTE_t *op = (klr_fLTE_t*)pc;
		OPEXEC_fLTE(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fGT) {
		klr_fGT_t *op = (klr_fGT_t*)pc;
		OPEXEC_fGT(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fGTE) {
		klr_fGTE_t *op = (klr_fGTE_t*)pc;
		OPEXEC_fGTE(op->c, op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(OSET) {
		klr_OSET_t *op = (klr_OSET_t*)pc;
		OPEXEC_OSET(op->a, op->n); pc++;
		GOTO_NEXT();
	} 
	CASE(OMOV) {
		klr_OMOV_t *op = (klr_OMOV_t*)pc;
		OPEXEC_OMOV(op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(SCALL) {
		klr_SCALL_t *op = (klr_SCALL_t*)pc;
		kMethod *mtd = kKonohaSpace_getMethodNULL(NULL, op->cid, op->mn);
		OPEXEC_SCALL(op->uline, op->thisidx, op->espshift, mtd);
		pc++;
		GOTO_NEXT();
	} 
	CASE(VCALL) {
		klr_VCALL_t *op = (klr_VCALL_t*)pc;
		kMethod *mtd = kKonohaSpace_getMethodNULL(NULL, op->cid, op->mn);
		//OPEXEC_VCALL(op->uline, op->thisidx, op->espshift, mtd);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iCAST) {
		klr_iCAST_t *op = (klr_iCAST_t*)pc;
		OPEXEC_iCAST(op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(fCAST) {
		klr_fCAST_t *op = (klr_fCAST_t*)pc;
		OPEXEC_fCAST(op->a, op->b); pc++;
		GOTO_NEXT();
	} 
	CASE(RET) {
		klr_RET_t *op = (klr_RET_t*)pc;
		OPEXEC_RET(); pc++;
		GOTO_NEXT();
	}
	CASE(EXIT) {
		klr_EXIT_t *op = (klr_EXIT_t*)pc;
		OPEXEC_EXIT(); pc++;
		GOTO_NEXT();
	}
	//CASE(NGETIDX) {
	//	klr_NGETIDX_t *op = (klr_NGETIDX_t*)pc;
	//	OPEXEC_NGETIDX(op->c, op->a, op->n); pc++;
	//	GOTO_NEXT();
	//} 
	//CASE(NSETIDX) {
	//	klr_NSETIDX_t *op = (klr_NSETIDX_t*)pc;
	//	OPEXEC_NSETIDX(op->c, op->a, op->n, op->v); pc++;
	//	GOTO_NEXT();
	//} 
	//CASE(OGETIDX) {
	//	klr_OGETIDX_t *op = (klr_OGETIDX_t*)pc;
	//	OPEXEC_OGETIDX(op->c, op->a, op->n); pc++;
	//	GOTO_NEXT();
	//} 
	//CASE(OSETIDX) {
	//	klr_OSETIDX_t *op = (klr_OSETIDX_t*)pc;
	//	OPEXEC_OSETIDX(op->c, op->a, op->n, op->v); pc++;
	//	GOTO_NEXT();
	//} 
	DISPATCH_END(pc);
	L_RETURN:;
	return pc;
}

#endif /* TINYVM_OPCODES_H */
