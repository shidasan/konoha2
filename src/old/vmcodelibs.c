/****************************************************************************
 * KONOHA2 COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 * 
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 * 
 * You may choose one of the following two licenses when you use konoha.
 * If you want to use the latter license, please contact us.
 * 
 * (1) GNU General Public License 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure License 1.0
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 ****************************************************************************/

#include"commons.h"


#ifdef __cplusplus 
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* [common] */

#define K_USING_VMINLINE
#include "vminline.c"

/* ------------------------------------------------------------------------ */
/* [data] */

#define _CONST 1
#define _JIT   (1<<1)
#define _DEF   (1<<2)
typedef struct {
	const char *name;
	kflag_t   flag;
	kushort_t size;
	kushort_t types[6];
} kOPDATA_t;

static const kOPDATA_t OPDATA[] = {
	{"HALT", 0, 0, { VMT_VOID}}, 
	{"THCODE", 0, 2, { VMT_F, VMT_U, VMT_VOID}}, 
	{"ENTER", 0, 0, { VMT_VOID}}, 
	{"VEXEC", 0, 0, { VMT_VOID}}, 
	{"YIELD", 0, 1, { VMT_SFPIDX, VMT_VOID}}, 
	{"FUNCCALL", 0, 0, { VMT_VOID}}, 
	{"TRY", 0, 2, { VMT_ADDR, VMT_RO, VMT_VOID}}, 
	{"TRYEND", 0, 1, { VMT_RO, VMT_VOID}}, 
	{"THROW", 0, 1, { VMT_SFPIDX, VMT_VOID}}, 
	{"ASSERT", 0, 2, { VMT_SFPIDX, VMT_U, VMT_VOID}}, 
	{"CATCH", _CONST, 3+VMTSIZE_int, { VMT_ADDR, VMT_RO, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"CHKIN", 0, 2, { VMT_RO, VMT_F, VMT_VOID}}, 
	{"CHKOUT", 0, 2, { VMT_RO, VMT_F, VMT_VOID}}, 
	{"ERROR", _CONST, 2, { VMT_SFPIDX, VMT_STRING, VMT_VOID}}, 
	{"P", _CONST, 4, { VMT_F, VMT_U, VMT_STRING, VMT_SFPIDX2, VMT_VOID}}, 
	{"PROBE", 0, 4, { VMT_SFPIDX2, VMT_F, VMT_U, VMT_U, VMT_VOID}}, 
	{"EXIT", 0, 0, { VMT_VOID}}, 
	{"NSET", _DEF|_JIT, 2+VMTSIZE_int, { VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"NMOV", _DEF|_JIT, 2, { VMT_RN, VMT_RN, VMT_VOID}}, 
	{"NNMOV", _DEF, 4, { VMT_RN, VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"NSET2", _DEF|_JIT, 3+VMTSIZE_int+VMTSIZE_int, { VMT_RN, VMT_INT VMTX_INT, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"NSET3", _DEF|_JIT, 4, { VMT_RN, VMT_U, VMT_U, VMT_U, VMT_VOID}}, 
	{"NSET4", _DEF|_JIT, 5, { VMT_RN, VMT_U, VMT_U, VMT_U, VMT_U, VMT_VOID}}, 
	{"NMOVx", _DEF, 2+1, { VMT_RN, VMT_SFX, VMT_VOID, VMT_VOID}}, 
	{"XNSET", 0, 2+1+VMTSIZE_int, { VMT_SFX, VMT_VOID, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"XNMOV", 0, 2+1, { VMT_SFX, VMT_VOID, VMT_RN, VMT_VOID}}, 
	{"XNMOVx", 0, 2+1+1, { VMT_SFX, VMT_VOID, VMT_SFX, VMT_VOID, VMT_VOID}}, 
	{"iINC", _DEF|_JIT, 1, { VMT_RN, VMT_VOID}}, 
	{"iDEC", _DEF|_JIT, 1, { VMT_RN, VMT_VOID}}, 
	{"bNUL", _DEF|_JIT, 2, { VMT_RN, VMT_RO, VMT_VOID}}, 
	{"bNN", _DEF|_JIT, 2, { VMT_RN, VMT_RO, VMT_VOID}}, 
	{"bNOT", _DEF|_JIT, 2, { VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iNEG", _DEF|_JIT, 2, { VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fNEG", _DEF|_JIT, 2, { VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iTR", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_F, VMT_VOID}}, 
	{"fTR", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_F, VMT_VOID}}, 
	{"iADD", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iSUB", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iMUL", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iDIV", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iMOD", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iEQ", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iNEQ", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iLT", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iLTE", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iGT", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iGTE", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iAND", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iOR", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iXOR", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iLSFT", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iRSFT", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iADDC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iSUBC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iMULC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iDIVC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iMODC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iEQC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iNEQC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iLTC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iLTEC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iGTC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iGTEC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iANDC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iORC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iXORC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iLSFTC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iRSFTC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_RN, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"fADD", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fSUB", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fMUL", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fDIV", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fEQ", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fNEQ", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fLT", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fLTE", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fGT", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fGTE", _DEF|_JIT, 3, { VMT_RN, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fADDC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fSUBC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fMULC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fDIVC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fEQC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fNEQC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fLTC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fLTEC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fGTC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fGTEC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_RN, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"RCINC", _JIT, 1, { VMT_RO, VMT_VOID}}, 
	{"RCDEC", _JIT, 1, { VMT_RO, VMT_VOID}}, 
	{"OSET", _DEF|_JIT|_CONST, 2, { VMT_RO, VMT_OBJECT, VMT_VOID}}, 
	{"OMOV", _DEF|_JIT, 2, { VMT_RO, VMT_RO, VMT_VOID}}, 
	{"OOMOV", _DEF|_JIT, 4, { VMT_RO, VMT_RO, VMT_RO, VMT_RO, VMT_VOID}}, 
	{"ONMOV", _DEF|_JIT, 4, { VMT_RO, VMT_RO, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"OSET2", _JIT|_CONST, 3, { VMT_RO, VMT_OBJECT, VMT_OBJECT, VMT_VOID}}, 
	{"OSET3", _JIT|_CONST, 4, { VMT_RO, VMT_OBJECT, VMT_OBJECT, VMT_OBJECT, VMT_VOID}}, 
	{"OSET4", _JIT|_CONST, 5, { VMT_RO, VMT_OBJECT, VMT_OBJECT, VMT_OBJECT, VMT_OBJECT, VMT_VOID}}, 
	{"RCINCx", 0, 1+1, { VMT_SFX, VMT_VOID, VMT_VOID}}, 
	{"RCDECx", 0, 1+1, { VMT_SFX, VMT_VOID, VMT_VOID}}, 
	{"OMOVx", _DEF|_JIT, 2+1, { VMT_RO, VMT_SFX, VMT_VOID, VMT_VOID}}, 
	{"XMOV", _JIT, 2+1, { VMT_SFX, VMT_VOID, VMT_RO, VMT_VOID}}, 
	{"XOSET", _JIT|_CONST, 2+1, { VMT_SFX, VMT_VOID, VMT_OBJECT, VMT_VOID}}, 
	{"XMOVx", 0, 2+1+1, { VMT_SFX, VMT_VOID, VMT_SFX, VMT_VOID, VMT_VOID}}, 
	{"CHKSTACK", 0, 1, { VMT_SFPIDX, VMT_VOID}}, 
	{"LDMTD", 0, 4, { VMT_SFPIDX, VMT_F, VMT_HCACHE, VMT_MTD, VMT_VOID}}, 
	{"CALL", _DEF|_JIT, 3, { VMT_R, VMT_SFPIDX, VMT_SFPIDX, VMT_VOID}}, 
	{"SCALL", _DEF|_JIT, 4, { VMT_R, VMT_SFPIDX, VMT_SFPIDX, VMT_MTD, VMT_VOID}}, 
	{"VCALL", _DEF|_JIT, 4, { VMT_R, VMT_SFPIDX, VMT_SFPIDX, VMT_MTD, VMT_VOID}}, 
	{"VCALL", _DEF|_JIT, 4, { VMT_R, VMT_SFPIDX, VMT_SFPIDX, VMT_MTD, VMT_VOID}}, 
	{"FASTCALL0", _DEF|_JIT, 5, { VMT_R, VMT_SFPIDX, VMT_I, VMT_SFPIDX, VMT_F, VMT_VOID}}, 
	{"RET", _JIT, 0, { VMT_VOID}}, 
	{"TR", _DEF|_JIT, 5, { VMT_R, VMT_SFPIDX, VMT_I, VMT_CID, VMT_F, VMT_VOID}}, 
	{"SCAST", _DEF, 5, { VMT_R, VMT_SFPIDX, VMT_I, VMT_SFPIDX, VMT_TMR, VMT_VOID}}, 
	{"TCAST", _DEF, 5, { VMT_R, VMT_SFPIDX, VMT_I, VMT_SFPIDX, VMT_TMR, VMT_VOID}}, 
	{"ACAST", _DEF, 5, { VMT_R, VMT_SFPIDX, VMT_I, VMT_SFPIDX, VMT_TMR, VMT_VOID}}, 
	{"iCAST", _DEF|_JIT, 2, { VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fCAST", _DEF|_JIT, 2, { VMT_RN, VMT_RN, VMT_VOID}}, 
	{"SAFEPOINT", _JIT, 1, { VMT_SFPIDX, VMT_VOID}}, 
	{"JMP", _JIT, 1, { VMT_ADDR, VMT_VOID}}, 
	{"JMP", _JIT, 1, { VMT_ADDR, VMT_VOID}}, 
	{"JMPF", 0, 2, { VMT_ADDR, VMT_RN, VMT_VOID}}, 
	{"NEXT", _DEF, 5, { VMT_ADDR, VMT_R, VMT_SFPIDX, VMT_I, VMT_SFPIDX, VMT_VOID}}, 
	{"BGETIDX", _DEF|_JIT, 3, { VMT_RN, VMT_RO, VMT_RN, VMT_VOID}}, 
	{"BSETIDX", _DEF|_JIT, 4, { VMT_RN, VMT_RO, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"BGETIDXC", _DEF|_JIT, 3, { VMT_RN, VMT_RO, VMT_U, VMT_VOID}}, 
	{"BSETIDXC", _DEF|_JIT, 4, { VMT_RN, VMT_RO, VMT_U, VMT_RN, VMT_VOID}}, 
	{"NGETIDX", _DEF|_JIT, 3, { VMT_RN, VMT_RO, VMT_RN, VMT_VOID}}, 
	{"NSETIDX", _DEF|_JIT, 4, { VMT_RN, VMT_RO, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"NGETIDXC", _DEF|_JIT, 3, { VMT_RN, VMT_RO, VMT_U, VMT_VOID}}, 
	{"NSETIDXC", _DEF|_JIT, 4, { VMT_RN, VMT_RO, VMT_U, VMT_RN, VMT_VOID}}, 
	{"OGETIDX", _DEF|_JIT, 3, { VMT_RO, VMT_RO, VMT_RN, VMT_VOID}}, 
	{"OSETIDX", _DEF|_JIT, 4, { VMT_RO, VMT_RO, VMT_RN, VMT_RO, VMT_VOID}}, 
	{"OGETIDXC", _DEF|_JIT, 3, { VMT_RO, VMT_RO, VMT_U, VMT_VOID}}, 
	{"OSETIDXC", _DEF|_JIT, 4, { VMT_RO, VMT_RO, VMT_U, VMT_RO, VMT_VOID}}, 
	{"bJNUL", _DEF|_JIT, 2, { VMT_ADDR, VMT_RO, VMT_VOID}}, 
	{"bJNN", _DEF|_JIT, 2, { VMT_ADDR, VMT_RO, VMT_VOID}}, 
	{"bJNOT", _DEF|_JIT, 2, { VMT_ADDR, VMT_RN, VMT_VOID}}, 
	{"iJEQ", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iJNEQ", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iJLT", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iJLTE", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iJGT", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iJGTE", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"iJEQC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_ADDR, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iJNEQC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_ADDR, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iJLTC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_ADDR, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iJLTEC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_ADDR, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iJGTC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_ADDR, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"iJGTEC", _DEF|_JIT, 3+VMTSIZE_int, { VMT_ADDR, VMT_RN, VMT_INT VMTX_INT, VMT_VOID}}, 
	{"fJEQ", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fJNEQ", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fJLT", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fJLTE", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fJGT", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fJGTE", _DEF|_JIT, 3, { VMT_ADDR, VMT_RN, VMT_RN, VMT_VOID}}, 
	{"fJEQC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_ADDR, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fJNEQC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_ADDR, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fJLTC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_ADDR, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fJLTEC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_ADDR, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fJGTC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_ADDR, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"fJGTEC", _DEF|_JIT, 3+VMTSIZE_float, { VMT_ADDR, VMT_RN, VMT_FLOAT VMTX_FLOAT, VMT_VOID}}, 
	{"CHKIDX", _JIT, 2, { VMT_RO, VMT_RN, VMT_VOID}}, 
	{"CHKIDXC", _JIT, 2, { VMT_RO, VMT_U, VMT_VOID}}, 
	{"NOP", 0, 0, { VMT_VOID}}, 
};

void knh_opcode_check(void)
{
	KNH_ASSERT(sizeof(klr_HALT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_THCODE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_ENTER_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_VEXEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_YIELD_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_FUNCCALL_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_TRY_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_TRYEND_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_THROW_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_ASSERT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_CATCH_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_CHKIN_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_CHKOUT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_ERROR_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_P_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_PROBE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_EXIT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NSET_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NMOV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NNMOV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NSET2_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NSET3_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NSET4_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NMOVx_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_XNSET_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_XNMOV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_XNMOVx_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iINC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iDEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_bNUL_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_bNN_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_bNOT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iNEG_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fNEG_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iTR_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fTR_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iADD_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iSUB_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iMUL_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iDIV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iMOD_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iEQ_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iNEQ_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iLT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iLTE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iGT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iGTE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iAND_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iOR_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iXOR_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iLSFT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iRSFT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iADDC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iSUBC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iMULC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iDIVC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iMODC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iEQC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iNEQC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iLTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iLTEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iGTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iGTEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iANDC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iORC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iXORC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iLSFTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iRSFTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fADD_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fSUB_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fMUL_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fDIV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fEQ_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fNEQ_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fLT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fLTE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fGT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fGTE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fADDC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fSUBC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fMULC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fDIVC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fEQC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fNEQC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fLTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fLTEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fGTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fGTEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_RCINC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_RCDEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OSET_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OMOV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OOMOV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_ONMOV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OSET2_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OSET3_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OSET4_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_RCINCx_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_RCDECx_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OMOVx_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_XMOV_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_XOSET_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_XMOVx_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_CHKSTACK_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_LDMTD_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_CALL_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_SCALL_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_VCALL_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_VCALL__t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_FASTCALL0_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_RET_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_TR_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_SCAST_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_TCAST_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_ACAST_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iCAST_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fCAST_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_SAFEPOINT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_JMP_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_JMP__t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_JMPF_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NEXT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_BGETIDX_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_BSETIDX_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_BGETIDXC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_BSETIDXC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NGETIDX_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NSETIDX_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NGETIDXC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NSETIDXC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OGETIDX_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OSETIDX_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OGETIDXC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_OSETIDXC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_bJNUL_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_bJNN_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_bJNOT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJEQ_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJNEQ_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJLT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJLTE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJGT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJGTE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJEQC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJNEQC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJLTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJLTEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJGTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_iJGTEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJEQ_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJNEQ_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJLT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJLTE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJGT_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJGTE_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJEQC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJNEQC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJLTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJLTEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJGTC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_fJGTEC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_CHKIDX_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_CHKIDXC_t) <= sizeof(kopl_t));
	KNH_ASSERT(sizeof(klr_NOP_t) <= sizeof(kopl_t));
}

/* ------------------------------------------------------------------------ */
const char *T_opcode(kopcode_t opcode)
{
	if(opcode < KOPCODE_MAX) {
		return OPDATA[opcode].name;
	}
	else {
		DBG_P("opcode=%d", (int)opcode);
		return "OPCODE_??";
	}
}

/* ------------------------------------------------------------------------ */
size_t kopcode_size(kopcode_t opcode)
{
	return OPDATA[opcode].size;
}

/* ------------------------------------------------------------------------ */
kbool_t kopcode_hasjump(kopcode_t opcode)
{
	return (OPDATA[opcode].types[0] == VMT_ADDR);
}
/* ------------------------------------------------------------------------ */
kbool_t kopcode_usedef(kopcode_t opcode, int i)
{
	kushort_t type = OPDATA[opcode].types[i];
	if ((type == VMT_SFPIDX) || (type == VMT_R) ||
			(type == VMT_RN) || (type == VMT_RO)) {
		return 1;
	}
	return 0;
}
/* ------------------------------------------------------------------------ */

kObject** kopl_reftrace(CTX, kopl_t *c)
{
	if(FLAG_is(OPDATA[c->opcode].flag, _CONST)) {
		size_t i, size = OPDATA[c->opcode].size;
		for(i = 0; i < size; i++) {
			kushort_t vtype = OPDATA[c->opcode].types[i];
			if(vtype == VMT_OBJECT || vtype == VMT_STRING) {
				KREFTRACEv(c->p[i]);
			}
		}
	}
	return tail_;
}
/* ------------------------------------------------------------------------ */

#define RBP_ASSERT0(N) do {\
	if((N % 2) != 0) {\
		DBG_P("r=%d", N); \
		DBG_ASSERT((N % 2) == 0);\
	}\
} while (0)

#define RBP_ASSERT1(N) do {\
	if((N % 2) == 0) {\
		DBG_P("r=%d", N);\
		DBG_ASSERT((N % 2) != 0);\
	}\
} while (0)

void knh_opcode_dump(CTX, kopl_t *c, kOutputStream *w, kopl_t *pc_start)
{
	size_t i, size = OPDATA[c->opcode].size;
	const kushort_t *vmt = OPDATA[c->opcode].types;
	if(pc_start == NULL) {
		knh_printf(_ctx, w, "[%p:%d] %s(%d)", c, c->line, T_opcode(c->opcode), (intptr_t)c->opcode);
	}
	else {
		knh_printf(_ctx, w, "L%d(%d): %s(%d)", c - pc_start, c->line, T_opcode(c->opcode), (intptr_t)c->opcode);
	}
	for(i = 0; i < size; i++) {
		kwb_putc(wb, ' '); 
		switch(vmt[i]) {
		case VMT_VOID: break;
		case VMT_ADDR: 
			if(pc_start == NULL) {
				knh_printf(_ctx, w, "%p", c->p[i]); break;
			}
			else {
				knh_printf(_ctx, w, "L%d", (kopl_t*)c->p[i] - pc_start); break;
			}
		case VMT_SFPIDX2:
			knh_printf(_ctx, w, "sfp[%d]", c->data[i]); 
			break;
		case VMT_RN: 
			RBP_ASSERT1(c->data[i]);
			if(((intptr_t)c->data[i]) < 0) {
				knh_printf(_ctx, w, "r(%d)", c->data[i]); 
			}
			else {
				knh_printf(_ctx, w, "r%d", c->data[i]); 
			}
			break;
		case VMT_SFX:
			RBP_ASSERT0(c->data[i]);
			knh_printf(_ctx, w, "r%d(+%d)", c->data[i], c->data[i+1]); 
			break;
		case VMT_SFPIDX: 
		case VMT_RO:
			RBP_ASSERT0(c->data[i]);
		case VMT_R: 
			if(((intptr_t)c->data[i]) < 0) {
				knh_printf(_ctx, w, "r(%d)", c->data[i]); 
			}
			else {
				knh_printf(_ctx, w, "r%d", c->data[i]); 
			}
			break;
		case VMT_U: case VMT_I:
			knh_write_dfmt(_ctx, w, K_INTPTR_FMT, c->data[i]); break;
		case VMT_F:
			knh_write_vmfunc(_ctx, w, c->p[i]); break;
		case VMT_CID:
			knh_write_cname(_ctx, w, ((struct _kclass*)c->data[i])->cid); break;
		case VMT_HCACHE: {
			kcachedata_t *hc = (kcachedata_t*)&(c->p[i]);
			knh_write_cname(_ctx, w, hc->cid); 
			kwb_putc(wb, '/');
			knh_write_mn(_ctx, w, hc->mn); 
		}
		break;
		case VMT_MTD: if(c->p[i] != NULL) {
			kMethod *mtd = (kMethod*)c->p[i];
			knh_write_cname(_ctx, w, (mtd)->cid); kwb_putc(wb, '.');
			knh_write_mn(_ctx, w, (mtd)->mn); 
		}
		else {
			knh_write_ascii(_ctx, w, "NULL");
		}
		break;
		case VMT_TMR:
		case VMT_OBJECT:
		case VMT_STRING: {
			knh_write_Object(_ctx, w, UPCAST(c->p[i]), FMT_line);
			break;
		}
		case VMT_INT: {
			kint_t n = ((kint_t*)(&(c->p[i])))[0];
			knh_write_ifmt(_ctx, w, KINT_FMT, n); break;
		}
		case VMT_FLOAT:
			knh_write_ffmt(_ctx, w, KFLOAT_FMT, *((kfloat_t*)&(c->p[i]))); break;
		}
	}
	knh_write_EOL(_ctx, w);
}

void knh_opcode_shift(kopl_t *c, int shift)
{
	size_t i, size = OPDATA[c->opcode].size;
	const kushort_t *vmt = OPDATA[c->opcode].types;
	for(i = 0; i < size; i++) {
		switch(vmt[i]) {
			case VMT_SFPIDX: case VMT_R: case VMT_RN: case VMT_RO: case VMT_SFX: 
				c->data[i] = c->data[i] + (shift * 2); break;
			case VMT_SFPIDX2:
				c->data[i] = c->data[i] + (shift); break;
		}
	}
}


/* ------------------------------------------------------------------------ */
/* [exec] */


#include"../../include/konoha2/konoha_vm.h"

#ifdef K_USING_VMCOUNT_
#define VMCOUNT(op)    ((op)->count)++;
#else
#define VMCOUNT(op)
#endif

//#if (defined(K_USING_LINUX_) && (defined(__i386__) || defined(__x86_64__)) && (defined(__GNUC__) && __GNUC__ >= 3))
//#define K_USING_VMASMDISPATCH 1
//#endif

#ifdef K_USING_THCODE_
#define CASE(x)  L_##x : 
#define NEXT_OP   (pc->codeaddr)
#define JUMP      *(NEXT_OP)
#ifdef K_USING_VMASMDISPATCH
#define GOTO_NEXT()     \
	asm volatile("jmp *%0;": : "g"(NEXT_OP));\
	goto *(NEXT_OP)

#else
#define GOTO_NEXT()     goto *(NEXT_OP)
#endif
#define TC(c) 
#define DISPATCH_START(pc) goto *OPJUMP[pc->opcode]
#define DISPATCH_END(pc)
#define GOTO_PC(pc)        GOTO_NEXT()
#else/*K_USING_THCODE_*/
#define OPJUMP      NULL
#define CASE(x)     case OPCODE_##x : 
#define NEXT_OP     L_HEAD
#define GOTO_NEXT() goto NEXT_OP
#define JUMP        L_HEAD
#define TC(c)
#define DISPATCH_START(pc) L_HEAD:;switch(pc->opcode) {
#define DISPATCH_END(pc)   } KNH_DIE("unknown opcode=%d", (int)pc->opcode); 
#define GOTO_PC(pc)         GOTO_NEXT()
#endif/*K_USING_THCODE_*/

kopl_t* knh_VirtualMachine_run(CTX, ksfp_t *sfp0, kopl_t *pc)
{
#ifdef K_USING_THCODE_
	static void *OPJUMP[] = {
		&&L_HALT, &&L_THCODE, &&L_ENTER, &&L_VEXEC, 
		&&L_YIELD, &&L_FUNCCALL, &&L_TRY, &&L_TRYEND, 
		&&L_THROW, &&L_ASSERT, &&L_CATCH, &&L_CHKIN, 
		&&L_CHKOUT, &&L_ERROR, &&L_P, &&L_PROBE, 
		&&L_EXIT, &&L_NSET, &&L_NMOV, &&L_NNMOV, 
		&&L_NSET2, &&L_NSET3, &&L_NSET4, &&L_NMOVx, 
		&&L_XNSET, &&L_XNMOV, &&L_XNMOVx, &&L_iINC, 
		&&L_iDEC, &&L_bNUL, &&L_bNN, &&L_bNOT, 
		&&L_iNEG, &&L_fNEG, &&L_iTR, &&L_fTR, 
		&&L_iADD, &&L_iSUB, &&L_iMUL, &&L_iDIV, 
		&&L_iMOD, &&L_iEQ, &&L_iNEQ, &&L_iLT, 
		&&L_iLTE, &&L_iGT, &&L_iGTE, &&L_iAND, 
		&&L_iOR, &&L_iXOR, &&L_iLSFT, &&L_iRSFT, 
		&&L_iADDC, &&L_iSUBC, &&L_iMULC, &&L_iDIVC, 
		&&L_iMODC, &&L_iEQC, &&L_iNEQC, &&L_iLTC, 
		&&L_iLTEC, &&L_iGTC, &&L_iGTEC, &&L_iANDC, 
		&&L_iORC, &&L_iXORC, &&L_iLSFTC, &&L_iRSFTC, 
		&&L_fADD, &&L_fSUB, &&L_fMUL, &&L_fDIV, 
		&&L_fEQ, &&L_fNEQ, &&L_fLT, &&L_fLTE, 
		&&L_fGT, &&L_fGTE, &&L_fADDC, &&L_fSUBC, 
		&&L_fMULC, &&L_fDIVC, &&L_fEQC, &&L_fNEQC, 
		&&L_fLTC, &&L_fLTEC, &&L_fGTC, &&L_fGTEC, 
		&&L_RCINC, &&L_RCDEC, &&L_OSET, &&L_OMOV, 
		&&L_OOMOV, &&L_ONMOV, &&L_OSET2, &&L_OSET3, 
		&&L_OSET4, &&L_RCINCx, &&L_RCDECx, &&L_OMOVx, 
		&&L_XMOV, &&L_XOSET, &&L_XMOVx, &&L_CHKSTACK, 
		&&L_LDMTD, &&L_CALL, &&L_SCALL, &&L_VCALL, 
		&&L_VCALL_, &&L_FASTCALL0, &&L_RET, &&L_TR, 
		&&L_SCAST, &&L_TCAST, &&L_ACAST, &&L_iCAST, 
		&&L_fCAST, &&L_SAFEPOINT, &&L_JMP, &&L_JMP_, 
		&&L_JMPF, &&L_NEXT, &&L_BGETIDX, &&L_BSETIDX, 
		&&L_BGETIDXC, &&L_BSETIDXC, &&L_NGETIDX, &&L_NSETIDX, 
		&&L_NGETIDXC, &&L_NSETIDXC, &&L_OGETIDX, &&L_OSETIDX, 
		&&L_OGETIDXC, &&L_OSETIDXC, &&L_bJNUL, &&L_bJNN, 
		&&L_bJNOT, &&L_iJEQ, &&L_iJNEQ, &&L_iJLT, 
		&&L_iJLTE, &&L_iJGT, &&L_iJGTE, &&L_iJEQC, 
		&&L_iJNEQC, &&L_iJLTC, &&L_iJLTEC, &&L_iJGTC, 
		&&L_iJGTEC, &&L_fJEQ, &&L_fJNEQ, &&L_fJLT, 
		&&L_fJLTE, &&L_fJGT, &&L_fJGTE, &&L_fJEQC, 
		&&L_fJNEQC, &&L_fJLTC, &&L_fJLTEC, &&L_fJGTC, 
		&&L_fJGTEC, &&L_CHKIDX, &&L_CHKIDXC, &&L_NOP, 
	};
#endif
	krbp_t *rbp = (krbp_t*)sfp0;
	USE_PROF(
	uint64_t _utime = knh_getTime();
	static uint64_t _UTIME[OPCODE_NOP+1] = {0};
	static size_t _UC[OPCODE_NOP+1] = {0};)
	DISPATCH_START(pc);

	CASE(HALT) {
		klr_HALT_t *op = (klr_HALT_t*)pc; (void)op;
		KLR_HALT(_ctx);
		pc++;
		GOTO_NEXT();
	} 
	CASE(THCODE) {
		klr_THCODE_t *op = (klr_THCODE_t*)pc; (void)op;
		KLR_THCODE(_ctx, op->th, op->fileid);
		pc++;
		GOTO_NEXT();
	} 
	CASE(ENTER) {
		klr_ENTER_t *op = (klr_ENTER_t*)pc; (void)op;
		KLR_ENTER(_ctx);
		pc++;
		GOTO_NEXT();
	} 
	CASE(VEXEC) {
		klr_VEXEC_t *op = (klr_VEXEC_t*)pc; (void)op;
		KLR_VEXEC(_ctx);
		pc++;
		GOTO_NEXT();
	} 
	CASE(YIELD) {
		klr_YIELD_t *op = (klr_YIELD_t*)pc; (void)op;
		KLR_YIELD(_ctx, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(FUNCCALL) {
		klr_FUNCCALL_t *op = (klr_FUNCCALL_t*)pc; (void)op;
		KLR_FUNCCALL(_ctx);
		pc++;
		GOTO_NEXT();
	} 
	CASE(TRY) {
		klr_TRY_t *op = (klr_TRY_t*)pc; (void)op;
		KLR_TRY(_ctx, pc = op->jumppc, JUMP, op->hn);
		pc++;
		GOTO_NEXT();
	} 
	CASE(TRYEND) {
		klr_TRYEND_t *op = (klr_TRYEND_t*)pc; (void)op;
		KLR_TRYEND(_ctx, op->hn);
		pc++;
		GOTO_NEXT();
	} 
	CASE(THROW) {
		klr_THROW_t *op = (klr_THROW_t*)pc; (void)op;
		KLR_THROW(_ctx, op->start);
		pc++;
		GOTO_NEXT();
	} 
	CASE(ASSERT) {
		klr_ASSERT_t *op = (klr_ASSERT_t*)pc; (void)op;
		KLR_ASSERT(_ctx, op->start, op->uline);
		pc++;
		GOTO_NEXT();
	} 
	CASE(CATCH) {
		klr_CATCH_t *op = (klr_CATCH_t*)pc; (void)op;
		KLR_CATCH(_ctx, pc = op->jumppc, JUMP, op->en, op->eid);
		pc++;
		GOTO_NEXT();
	} 
	CASE(CHKIN) {
		klr_CHKIN_t *op = (klr_CHKIN_t*)pc; (void)op;
		KLR_CHKIN(_ctx, op->on, op->checkin);
		pc++;
		GOTO_NEXT();
	} 
	CASE(CHKOUT) {
		klr_CHKOUT_t *op = (klr_CHKOUT_t*)pc; (void)op;
		KLR_CHKOUT(_ctx, op->on, op->checkout);
		pc++;
		GOTO_NEXT();
	} 
	CASE(ERROR) {
		klr_ERROR_t *op = (klr_ERROR_t*)pc; (void)op;
		KLR_ERROR(_ctx, op->start, op->msg);
		pc++;
		GOTO_NEXT();
	} 
	CASE(P) {
		klr_P_t *op = (klr_P_t*)pc; (void)op;
		KLR_P(_ctx, op->print, op->flag, op->msg, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(PROBE) {
		klr_PROBE_t *op = (klr_PROBE_t*)pc; (void)op;
		KLR_PROBE(_ctx, op->sfpidx, op->probe, op->n, op->n2);
		pc++;
		GOTO_NEXT();
	} 
	CASE(EXIT) {
		klr_EXIT_t *op = (klr_EXIT_t*)pc; (void)op;
		KLR_EXIT(_ctx);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NSET) {
		klr_NSET_t *op = (klr_NSET_t*)pc; (void)op;
		KLR_NSET(_ctx, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NMOV) {
		klr_NMOV_t *op = (klr_NMOV_t*)pc; (void)op;
		KLR_NMOV(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NNMOV) {
		klr_NNMOV_t *op = (klr_NNMOV_t*)pc; (void)op;
		KLR_NNMOV(_ctx, op->a, op->b, op->c, op->d);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NSET2) {
		klr_NSET2_t *op = (klr_NSET2_t*)pc; (void)op;
		KLR_NSET2(_ctx, op->a, op->n, op->n2);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NSET3) {
		klr_NSET3_t *op = (klr_NSET3_t*)pc; (void)op;
		KLR_NSET3(_ctx, op->a, op->n, op->n2, op->n3);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NSET4) {
		klr_NSET4_t *op = (klr_NSET4_t*)pc; (void)op;
		KLR_NSET4(_ctx, op->a, op->n, op->n2, op->n3, op->n4);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NMOVx) {
		klr_NMOVx_t *op = (klr_NMOVx_t*)pc; (void)op;
		KLR_NMOVx(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(XNSET) {
		klr_XNSET_t *op = (klr_XNSET_t*)pc; (void)op;
		KLR_XNSET(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(XNMOV) {
		klr_XNMOV_t *op = (klr_XNMOV_t*)pc; (void)op;
		KLR_XNMOV(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(XNMOVx) {
		klr_XNMOVx_t *op = (klr_XNMOVx_t*)pc; (void)op;
		KLR_XNMOVx(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iINC) {
		klr_iINC_t *op = (klr_iINC_t*)pc; (void)op;
		KLR_iINC(_ctx, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iDEC) {
		klr_iDEC_t *op = (klr_iDEC_t*)pc; (void)op;
		KLR_iDEC(_ctx, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(bNUL) {
		klr_bNUL_t *op = (klr_bNUL_t*)pc; (void)op;
		KLR_bNUL(_ctx, op->c, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(bNN) {
		klr_bNN_t *op = (klr_bNN_t*)pc; (void)op;
		KLR_bNN(_ctx, op->c, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(bNOT) {
		klr_bNOT_t *op = (klr_bNOT_t*)pc; (void)op;
		KLR_bNOT(_ctx, op->c, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iNEG) {
		klr_iNEG_t *op = (klr_iNEG_t*)pc; (void)op;
		KLR_iNEG(_ctx, op->c, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fNEG) {
		klr_fNEG_t *op = (klr_fNEG_t*)pc; (void)op;
		KLR_fNEG(_ctx, op->c, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iTR) {
		klr_iTR_t *op = (klr_iTR_t*)pc; (void)op;
		KLR_iTR(_ctx, op->c, op->a, op->inttr);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fTR) {
		klr_fTR_t *op = (klr_fTR_t*)pc; (void)op;
		KLR_fTR(_ctx, op->c, op->a, op->floattr);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iADD) {
		klr_iADD_t *op = (klr_iADD_t*)pc; (void)op;
		KLR_iADD(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iSUB) {
		klr_iSUB_t *op = (klr_iSUB_t*)pc; (void)op;
		KLR_iSUB(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iMUL) {
		klr_iMUL_t *op = (klr_iMUL_t*)pc; (void)op;
		KLR_iMUL(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iDIV) {
		klr_iDIV_t *op = (klr_iDIV_t*)pc; (void)op;
		KLR_iDIV(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iMOD) {
		klr_iMOD_t *op = (klr_iMOD_t*)pc; (void)op;
		KLR_iMOD(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iEQ) {
		klr_iEQ_t *op = (klr_iEQ_t*)pc; (void)op;
		KLR_iEQ(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iNEQ) {
		klr_iNEQ_t *op = (klr_iNEQ_t*)pc; (void)op;
		KLR_iNEQ(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iLT) {
		klr_iLT_t *op = (klr_iLT_t*)pc; (void)op;
		KLR_iLT(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iLTE) {
		klr_iLTE_t *op = (klr_iLTE_t*)pc; (void)op;
		KLR_iLTE(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iGT) {
		klr_iGT_t *op = (klr_iGT_t*)pc; (void)op;
		KLR_iGT(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iGTE) {
		klr_iGTE_t *op = (klr_iGTE_t*)pc; (void)op;
		KLR_iGTE(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iAND) {
		klr_iAND_t *op = (klr_iAND_t*)pc; (void)op;
		KLR_iAND(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iOR) {
		klr_iOR_t *op = (klr_iOR_t*)pc; (void)op;
		KLR_iOR(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iXOR) {
		klr_iXOR_t *op = (klr_iXOR_t*)pc; (void)op;
		KLR_iXOR(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iLSFT) {
		klr_iLSFT_t *op = (klr_iLSFT_t*)pc; (void)op;
		KLR_iLSFT(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iRSFT) {
		klr_iRSFT_t *op = (klr_iRSFT_t*)pc; (void)op;
		KLR_iRSFT(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iADDC) {
		klr_iADDC_t *op = (klr_iADDC_t*)pc; (void)op;
		KLR_iADDC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iSUBC) {
		klr_iSUBC_t *op = (klr_iSUBC_t*)pc; (void)op;
		KLR_iSUBC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iMULC) {
		klr_iMULC_t *op = (klr_iMULC_t*)pc; (void)op;
		KLR_iMULC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iDIVC) {
		klr_iDIVC_t *op = (klr_iDIVC_t*)pc; (void)op;
		KLR_iDIVC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iMODC) {
		klr_iMODC_t *op = (klr_iMODC_t*)pc; (void)op;
		KLR_iMODC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iEQC) {
		klr_iEQC_t *op = (klr_iEQC_t*)pc; (void)op;
		KLR_iEQC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iNEQC) {
		klr_iNEQC_t *op = (klr_iNEQC_t*)pc; (void)op;
		KLR_iNEQC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iLTC) {
		klr_iLTC_t *op = (klr_iLTC_t*)pc; (void)op;
		KLR_iLTC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iLTEC) {
		klr_iLTEC_t *op = (klr_iLTEC_t*)pc; (void)op;
		KLR_iLTEC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iGTC) {
		klr_iGTC_t *op = (klr_iGTC_t*)pc; (void)op;
		KLR_iGTC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iGTEC) {
		klr_iGTEC_t *op = (klr_iGTEC_t*)pc; (void)op;
		KLR_iGTEC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iANDC) {
		klr_iANDC_t *op = (klr_iANDC_t*)pc; (void)op;
		KLR_iANDC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iORC) {
		klr_iORC_t *op = (klr_iORC_t*)pc; (void)op;
		KLR_iORC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iXORC) {
		klr_iXORC_t *op = (klr_iXORC_t*)pc; (void)op;
		KLR_iXORC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iLSFTC) {
		klr_iLSFTC_t *op = (klr_iLSFTC_t*)pc; (void)op;
		KLR_iLSFTC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iRSFTC) {
		klr_iRSFTC_t *op = (klr_iRSFTC_t*)pc; (void)op;
		KLR_iRSFTC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fADD) {
		klr_fADD_t *op = (klr_fADD_t*)pc; (void)op;
		KLR_fADD(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fSUB) {
		klr_fSUB_t *op = (klr_fSUB_t*)pc; (void)op;
		KLR_fSUB(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fMUL) {
		klr_fMUL_t *op = (klr_fMUL_t*)pc; (void)op;
		KLR_fMUL(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fDIV) {
		klr_fDIV_t *op = (klr_fDIV_t*)pc; (void)op;
		KLR_fDIV(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fEQ) {
		klr_fEQ_t *op = (klr_fEQ_t*)pc; (void)op;
		KLR_fEQ(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fNEQ) {
		klr_fNEQ_t *op = (klr_fNEQ_t*)pc; (void)op;
		KLR_fNEQ(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fLT) {
		klr_fLT_t *op = (klr_fLT_t*)pc; (void)op;
		KLR_fLT(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fLTE) {
		klr_fLTE_t *op = (klr_fLTE_t*)pc; (void)op;
		KLR_fLTE(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fGT) {
		klr_fGT_t *op = (klr_fGT_t*)pc; (void)op;
		KLR_fGT(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fGTE) {
		klr_fGTE_t *op = (klr_fGTE_t*)pc; (void)op;
		KLR_fGTE(_ctx, op->c, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fADDC) {
		klr_fADDC_t *op = (klr_fADDC_t*)pc; (void)op;
		KLR_fADDC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fSUBC) {
		klr_fSUBC_t *op = (klr_fSUBC_t*)pc; (void)op;
		KLR_fSUBC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fMULC) {
		klr_fMULC_t *op = (klr_fMULC_t*)pc; (void)op;
		KLR_fMULC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fDIVC) {
		klr_fDIVC_t *op = (klr_fDIVC_t*)pc; (void)op;
		KLR_fDIVC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fEQC) {
		klr_fEQC_t *op = (klr_fEQC_t*)pc; (void)op;
		KLR_fEQC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fNEQC) {
		klr_fNEQC_t *op = (klr_fNEQC_t*)pc; (void)op;
		KLR_fNEQC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fLTC) {
		klr_fLTC_t *op = (klr_fLTC_t*)pc; (void)op;
		KLR_fLTC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fLTEC) {
		klr_fLTEC_t *op = (klr_fLTEC_t*)pc; (void)op;
		KLR_fLTEC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fGTC) {
		klr_fGTC_t *op = (klr_fGTC_t*)pc; (void)op;
		KLR_fGTC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fGTEC) {
		klr_fGTEC_t *op = (klr_fGTEC_t*)pc; (void)op;
		KLR_fGTEC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(RCINC) {
		klr_RCINC_t *op = (klr_RCINC_t*)pc; (void)op;
		KLR_RCINC(_ctx, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(RCDEC) {
		klr_RCDEC_t *op = (klr_RCDEC_t*)pc; (void)op;
		KLR_RCDEC(_ctx, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OSET) {
		klr_OSET_t *op = (klr_OSET_t*)pc; (void)op;
		KLR_OSET(_ctx, op->a, op->o);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OMOV) {
		klr_OMOV_t *op = (klr_OMOV_t*)pc; (void)op;
		KLR_OMOV(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OOMOV) {
		klr_OOMOV_t *op = (klr_OOMOV_t*)pc; (void)op;
		KLR_OOMOV(_ctx, op->a, op->b, op->c, op->d);
		pc++;
		GOTO_NEXT();
	} 
	CASE(ONMOV) {
		klr_ONMOV_t *op = (klr_ONMOV_t*)pc; (void)op;
		KLR_ONMOV(_ctx, op->a, op->b, op->c, op->d);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OSET2) {
		klr_OSET2_t *op = (klr_OSET2_t*)pc; (void)op;
		KLR_OSET2(_ctx, op->a, op->v, op->v2);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OSET3) {
		klr_OSET3_t *op = (klr_OSET3_t*)pc; (void)op;
		KLR_OSET3(_ctx, op->a, op->v, op->v2, op->v3);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OSET4) {
		klr_OSET4_t *op = (klr_OSET4_t*)pc; (void)op;
		KLR_OSET4(_ctx, op->a, op->v, op->v2, op->v3, op->v4);
		pc++;
		GOTO_NEXT();
	} 
	CASE(RCINCx) {
		klr_RCINCx_t *op = (klr_RCINCx_t*)pc; (void)op;
		KLR_RCINCx(_ctx, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(RCDECx) {
		klr_RCDECx_t *op = (klr_RCDECx_t*)pc; (void)op;
		KLR_RCDECx(_ctx, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OMOVx) {
		klr_OMOVx_t *op = (klr_OMOVx_t*)pc; (void)op;
		KLR_OMOVx(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(XMOV) {
		klr_XMOV_t *op = (klr_XMOV_t*)pc; (void)op;
		KLR_XMOV(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(XOSET) {
		klr_XOSET_t *op = (klr_XOSET_t*)pc; (void)op;
		KLR_XOSET(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(XMOVx) {
		klr_XMOVx_t *op = (klr_XMOVx_t*)pc; (void)op;
		KLR_XMOVx(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(CHKSTACK) {
		klr_CHKSTACK_t *op = (klr_CHKSTACK_t*)pc; (void)op;
		KLR_CHKSTACK(_ctx, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(LDMTD) {
		klr_LDMTD_t *op = (klr_LDMTD_t*)pc; (void)op;
		KLR_LDMTD(_ctx, op->thisidx, op->loadmtd, op->cache, op->mtdNC);
		pc++;
		GOTO_NEXT();
	} 
	CASE(CALL) {
		klr_CALL_t *op = (klr_CALL_t*)pc; (void)op;
		KLR_CALL(_ctx, op->a, op->thisidx, op->espshift);
		pc++;
		GOTO_NEXT();
	} 
	CASE(SCALL) {
		klr_SCALL_t *op = (klr_SCALL_t*)pc; (void)op;
		KLR_SCALL(_ctx, op->a, op->thisidx, op->espshift, op->mtdNC);
		pc++;
		GOTO_NEXT();
	} 
	CASE(VCALL) {
		klr_VCALL_t *op = (klr_VCALL_t*)pc; (void)op;
		KLR_VCALL(_ctx, op->a, op->thisidx, op->espshift, op->mtdNC);
		pc++;
		GOTO_NEXT();
	} 
	CASE(VCALL_) {
		klr_VCALL__t *op = (klr_VCALL__t*)pc; (void)op;
		KLR_VCALL_(_ctx, op->a, op->thisidx, op->espshift, op->mtdNC);
		pc++;
		GOTO_NEXT();
	} 
	CASE(FASTCALL0) {
		klr_FASTCALL0_t *op = (klr_FASTCALL0_t*)pc; (void)op;
		KLR_FASTCALL0(_ctx, op->a, op->thisidx, op->rix, op->espshift, op->fcall);
		pc++;
		GOTO_NEXT();
	} 
	CASE(RET) {
		klr_RET_t *op = (klr_RET_t*)pc; (void)op;
		KLR_RET(_ctx);
		pc++;
		GOTO_NEXT();
	} 
	CASE(TR) {
		klr_TR_t *op = (klr_TR_t*)pc; (void)op;
		KLR_TR(_ctx, op->a, op->b, op->rix, op->cid, op->tr);
		pc++;
		GOTO_NEXT();
	} 
	CASE(SCAST) {
		klr_SCAST_t *op = (klr_SCAST_t*)pc; (void)op;
		KLR_SCAST(_ctx, op->a, op->b, op->rix, op->espshift, op->cast);
		pc++;
		GOTO_NEXT();
	} 
	CASE(TCAST) {
		klr_TCAST_t *op = (klr_TCAST_t*)pc; (void)op;
		KLR_TCAST(_ctx, op->a, op->b, op->rix, op->espshift, op->cast);
		pc++;
		GOTO_NEXT();
	} 
	CASE(ACAST) {
		klr_ACAST_t *op = (klr_ACAST_t*)pc; (void)op;
		KLR_ACAST(_ctx, op->a, op->b, op->rix, op->espshift, op->cast);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iCAST) {
		klr_iCAST_t *op = (klr_iCAST_t*)pc; (void)op;
		KLR_iCAST(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fCAST) {
		klr_fCAST_t *op = (klr_fCAST_t*)pc; (void)op;
		KLR_fCAST(_ctx, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(SAFEPOINT) {
		klr_SAFEPOINT_t *op = (klr_SAFEPOINT_t*)pc; (void)op;
		KLR_SAFEPOINT(_ctx, op->espshift);
		pc++;
		GOTO_NEXT();
	} 
	CASE(JMP) {
		klr_JMP_t *op = (klr_JMP_t*)pc; (void)op;
		KLR_JMP(_ctx, pc = op->jumppc, JUMP);
		pc++;
		GOTO_NEXT();
	} 
	CASE(JMP_) {
		klr_JMP__t *op = (klr_JMP__t*)pc; (void)op;
		KLR_JMP_(_ctx, pc = op->jumppc, JUMP);
		pc++;
		GOTO_NEXT();
	} 
	CASE(JMPF) {
		klr_JMPF_t *op = (klr_JMPF_t*)pc; (void)op;
		KLR_JMPF(_ctx, pc = op->jumppc, JUMP, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NEXT) {
		klr_NEXT_t *op = (klr_NEXT_t*)pc; (void)op;
		KLR_NEXT(_ctx, pc = op->jumppc, JUMP, op->a, op->b, op->rix, op->espshift);
		pc++;
		GOTO_NEXT();
	} 
	CASE(BGETIDX) {
		klr_BGETIDX_t *op = (klr_BGETIDX_t*)pc; (void)op;
		KLR_BGETIDX(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(BSETIDX) {
		klr_BSETIDX_t *op = (klr_BSETIDX_t*)pc; (void)op;
		KLR_BSETIDX(_ctx, op->c, op->a, op->n, op->v);
		pc++;
		GOTO_NEXT();
	} 
	CASE(BGETIDXC) {
		klr_BGETIDXC_t *op = (klr_BGETIDXC_t*)pc; (void)op;
		KLR_BGETIDXC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(BSETIDXC) {
		klr_BSETIDXC_t *op = (klr_BSETIDXC_t*)pc; (void)op;
		KLR_BSETIDXC(_ctx, op->c, op->a, op->n, op->v);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NGETIDX) {
		klr_NGETIDX_t *op = (klr_NGETIDX_t*)pc; (void)op;
		KLR_NGETIDX(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NSETIDX) {
		klr_NSETIDX_t *op = (klr_NSETIDX_t*)pc; (void)op;
		KLR_NSETIDX(_ctx, op->c, op->a, op->n, op->v);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NGETIDXC) {
		klr_NGETIDXC_t *op = (klr_NGETIDXC_t*)pc; (void)op;
		KLR_NGETIDXC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NSETIDXC) {
		klr_NSETIDXC_t *op = (klr_NSETIDXC_t*)pc; (void)op;
		KLR_NSETIDXC(_ctx, op->c, op->a, op->n, op->v);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OGETIDX) {
		klr_OGETIDX_t *op = (klr_OGETIDX_t*)pc; (void)op;
		KLR_OGETIDX(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OSETIDX) {
		klr_OSETIDX_t *op = (klr_OSETIDX_t*)pc; (void)op;
		KLR_OSETIDX(_ctx, op->c, op->a, op->n, op->v);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OGETIDXC) {
		klr_OGETIDXC_t *op = (klr_OGETIDXC_t*)pc; (void)op;
		KLR_OGETIDXC(_ctx, op->c, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(OSETIDXC) {
		klr_OSETIDXC_t *op = (klr_OSETIDXC_t*)pc; (void)op;
		KLR_OSETIDXC(_ctx, op->c, op->a, op->n, op->v);
		pc++;
		GOTO_NEXT();
	} 
	CASE(bJNUL) {
		klr_bJNUL_t *op = (klr_bJNUL_t*)pc; (void)op;
		KLR_bJNUL(_ctx, pc = op->jumppc, JUMP, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(bJNN) {
		klr_bJNN_t *op = (klr_bJNN_t*)pc; (void)op;
		KLR_bJNN(_ctx, pc = op->jumppc, JUMP, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(bJNOT) {
		klr_bJNOT_t *op = (klr_bJNOT_t*)pc; (void)op;
		KLR_bJNOT(_ctx, pc = op->jumppc, JUMP, op->a);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJEQ) {
		klr_iJEQ_t *op = (klr_iJEQ_t*)pc; (void)op;
		KLR_iJEQ(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJNEQ) {
		klr_iJNEQ_t *op = (klr_iJNEQ_t*)pc; (void)op;
		KLR_iJNEQ(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJLT) {
		klr_iJLT_t *op = (klr_iJLT_t*)pc; (void)op;
		KLR_iJLT(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJLTE) {
		klr_iJLTE_t *op = (klr_iJLTE_t*)pc; (void)op;
		KLR_iJLTE(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJGT) {
		klr_iJGT_t *op = (klr_iJGT_t*)pc; (void)op;
		KLR_iJGT(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJGTE) {
		klr_iJGTE_t *op = (klr_iJGTE_t*)pc; (void)op;
		KLR_iJGTE(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJEQC) {
		klr_iJEQC_t *op = (klr_iJEQC_t*)pc; (void)op;
		KLR_iJEQC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJNEQC) {
		klr_iJNEQC_t *op = (klr_iJNEQC_t*)pc; (void)op;
		KLR_iJNEQC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJLTC) {
		klr_iJLTC_t *op = (klr_iJLTC_t*)pc; (void)op;
		KLR_iJLTC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJLTEC) {
		klr_iJLTEC_t *op = (klr_iJLTEC_t*)pc; (void)op;
		KLR_iJLTEC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJGTC) {
		klr_iJGTC_t *op = (klr_iJGTC_t*)pc; (void)op;
		KLR_iJGTC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(iJGTEC) {
		klr_iJGTEC_t *op = (klr_iJGTEC_t*)pc; (void)op;
		KLR_iJGTEC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJEQ) {
		klr_fJEQ_t *op = (klr_fJEQ_t*)pc; (void)op;
		KLR_fJEQ(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJNEQ) {
		klr_fJNEQ_t *op = (klr_fJNEQ_t*)pc; (void)op;
		KLR_fJNEQ(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJLT) {
		klr_fJLT_t *op = (klr_fJLT_t*)pc; (void)op;
		KLR_fJLT(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJLTE) {
		klr_fJLTE_t *op = (klr_fJLTE_t*)pc; (void)op;
		KLR_fJLTE(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJGT) {
		klr_fJGT_t *op = (klr_fJGT_t*)pc; (void)op;
		KLR_fJGT(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJGTE) {
		klr_fJGTE_t *op = (klr_fJGTE_t*)pc; (void)op;
		KLR_fJGTE(_ctx, pc = op->jumppc, JUMP, op->a, op->b);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJEQC) {
		klr_fJEQC_t *op = (klr_fJEQC_t*)pc; (void)op;
		KLR_fJEQC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJNEQC) {
		klr_fJNEQC_t *op = (klr_fJNEQC_t*)pc; (void)op;
		KLR_fJNEQC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJLTC) {
		klr_fJLTC_t *op = (klr_fJLTC_t*)pc; (void)op;
		KLR_fJLTC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJLTEC) {
		klr_fJLTEC_t *op = (klr_fJLTEC_t*)pc; (void)op;
		KLR_fJLTEC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJGTC) {
		klr_fJGTC_t *op = (klr_fJGTC_t*)pc; (void)op;
		KLR_fJGTC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(fJGTEC) {
		klr_fJGTEC_t *op = (klr_fJGTEC_t*)pc; (void)op;
		KLR_fJGTEC(_ctx, pc = op->jumppc, JUMP, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(CHKIDX) {
		klr_CHKIDX_t *op = (klr_CHKIDX_t*)pc; (void)op;
		KLR_CHKIDX(_ctx, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(CHKIDXC) {
		klr_CHKIDXC_t *op = (klr_CHKIDXC_t*)pc; (void)op;
		KLR_CHKIDXC(_ctx, op->a, op->n);
		pc++;
		GOTO_NEXT();
	} 
	CASE(NOP) {
		klr_NOP_t *op = (klr_NOP_t*)pc; (void)op;
		KLR_NOP(_ctx);
		pc++;
		GOTO_NEXT();
	} 
	DISPATCH_END(pc);
	L_RETURN:;
	return pc;
}


#ifdef __cplusplus
}
#endif
