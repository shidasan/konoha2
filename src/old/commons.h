#ifndef COMMONS_H_
#define COMMONS_H_

#define K_INTERNAL
#include"../../include/konoha2.h"
#include"../../include/konoha2/konoha_code_.h"
#include"../../include/konoha2/inlinelibs.h"
#include"../../include/konoha2/konohalang.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

#define TT_UNTYPED   TT_EOT
#define TT_DYN       TT_DYNAMIC

#define MN_newLIST   MN_new__LIST
#define MN_newARRAY  MN_new__ARRAY
#define MN_newTUPLE  MN_new__TUPLE
#define MN_newMAP    MN_new__MAP

#define T_stmtexpr  TY_void

#define K_SYSVAL_CTX    0
#define K_SYSVAL_CTXIN  1
#define K_SYSVAL_CTXOUT 2
#define K_SYSVAL_CTXERR 3
#define K_SYSVAL_STDIN  4
#define K_SYSVAL_STDOUT 5
#define K_SYSVAL_STDERR 6
#define K_SYSVAL_OS     7
#define K_SYSVAL_SCRIPT 8
#define K_SYSVAL_MAX    9

#define STT_isExpr(stt)   ((STT_LET <= stt && stt <= STT_CALL1) || stt == STT_FUNCTION)
const char* TT__(kterm_t tt);
#define Term__(tk)          TT__((tk)->tt)
#define Stmt__(stmt)      TT__((stmt)->stt)

#define TK_tobytes(tk)           S_tobytes((tk)->text)

#define TK_typing(tk, tt0, type0, index0) { \
		TT_(tk) = tt0;\
		SP(tk)->type = type0;\
		(tk)->index = index0;\
	}\

#define ADD_FLAG(flag, T, F) \
	if(knh_DictMap_getNULL(_ctx, DP(o)->metaDictCaseMap, STEXT(T)) != NULL) {\
		flag |= F;\
	}\

#define isSINGLEFOREACH(stmt)   (DP(stmt)->size == 3)

#define tmNN(stmt, n)        (stmt)->terms[(n)]
#define tkNN(stmt, n)        (stmt)->terms[(n)]
#define stmtNN(stmt, n)      (stmt)->stmts[(n)]

#define Tn__(stmt, n)        TT__(TT_(tmNN(stmt, n)))
#define TT_TERMs(stmt, n)    TT_(tmNN(stmt, n))
#define Tn_isCONST(stmt,n)   (TT_(tmNN(stmt, n)) == TT_CONST)
#define Tn_isASIS(stmt,n)    (TT_(tmNN(stmt, n)) == TT_ASIS)
#define Tn_isCID(stmt,n)     (TT_(tmNN(stmt, n)) == TT_CID)
#define Tn_const(stmt, n)    (tkNN(stmt, n))->data
#define Tn_isNULL(stmt, n)   (TT_TERMs(stmt, n) == TT_NULL)

#define Tn_isTRUE(stmt, n)   (Tn_isCONST(stmt, n) && IS_TRUE(Tn_const(stmt, n)))
#define Tn_isFALSE(stmt, n)  (Tn_isCONST(stmt, n) && IS_FALSE(Tn_const(stmt, n)))
#define Tn_type(stmt, n)     SP(tmNN(stmt, n))->type
#define Tn_cid(stmt, n)      CLASS_t(Tn_type(stmt, n))
//#define Tn_bcid(stmt, n)     ctx->share->ClassTBL[Tn_cid(stmt,n)].bcid

#define knh_Stmt_flag(_ctx, stmt, text, flag)   knh_Stmt_flag_(_ctx, stmt, STEXT(text), flag)
#define knh_StmtMETA_is(_ctx, stmt, anno)       StmtMETA_is_(_ctx, stmt, STEXT(anno))

#define GammaBuilder_isCompilingFmt(_ctx)    MN_isFMT((DP(_ctx->gma)->mtd)->mn)

/* ------------------------------------------------------------------------ */

#define TM(o)             ((kTerm*)o)

#define TT_(tk)        SP(tk)->tt
#define TT_isSTR(tt)   (tt == TT_STR || tt == TT_TSTR || tt == TT_ESTR)
#define TT_isBINARY(tt) (TT_IS <= (tt) && (tt) <= TT_RSFT)

#define STT_(stmt)   SP(stmt)->stt
#define knh_Stmt_add(_ctx, stmt, ...)   knh_Stmt_add_(_ctx, stmt, ## __VA_ARGS__, NULL)

/* ------------------------------------------------------------------------ */

#define FL(o)            ((Object*)o)
#define _(s)           s
#define KNH_HINT(_ctx, token)

/* ------------------------------------------------------------------------ */

#define SEPARATOR_IT    2
#define FOREACH_ITR     (FOREACH_loop+1)
#define FOREACH_SPR     (FOREACH_loop+2)
#define TRY_HDR         3
#define TEST_IT         2

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

#endif /*COMMONS_H_*/
