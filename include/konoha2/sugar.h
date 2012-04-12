/*
 * sugar.h
 *
 *  Created on: Feb 8, 2012
 *      Author: kimio
 */

#ifndef SUGAR_H_
#define SUGAR_H_

/* ------------------------------------------------------------------------ */
/* sugar.h */

//## class Token Object;
//## class Expr  Object;
//## class Stmt  Object;
//## class Block Object;
//## class Gamma Object;

#include <dlfcn.h>
#include "konoha2.h"
#include "klib.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define TEXT(T)  T, (sizeof(T)-1)

typedef ksymbol_t keyword_t;
#define T_kw(X)   Pkeyword_(_ctx, X)
#define Skeyword(X)   Skw_(_ctx, X)

//#define FLAG_METHOD_LAZYCOMPILE (0)

#define kflag_clear(flag)  (flag) = 0
#define K_CHECKSUM 1

#define KPACKNAME(N, V) \
	.name = N, .version = V, .konoha_checksum = K_CHECKSUM, .konoha_revision = K_REVISION

#define KPACKLIB(N, V) \
	.libname = N, .libversion = V

typedef struct {
	int konoha_checksum;
	const char *name;
	const char *version;
	const char *libname;
	const char *libversion;
	const char *note;
	kbool_t (*initPackage)(CTX, const struct _kKonohaSpace *, int, const char**, kline_t);
	kbool_t (*setupPackage)(CTX, const struct _kKonohaSpace *, kline_t);
	kbool_t (*initKonohaSpace)(CTX, const struct _kKonohaSpace *, kline_t);
	kbool_t (*setupKonohaSpace)(CTX, const struct _kKonohaSpace *, kline_t);
	int konoha_revision;
} KPACKDEF_;

typedef const KPACKDEF_ KPACKDEF;
typedef KPACKDEF* (*Fpackageinit)(void);

typedef struct {
	kpack_t                      packid;
	const struct _kKonohaSpace  *ks;
	KPACKDEF                    *packdef;
	kline_t                      export_script;
} kpackage_t;

// StmtAdd
#define VAR_StmtAdd(STMT, SYN, NAME, TLS, S, E) \
		kStmt *STMT = (kStmt*)sfp[0].o;\
		ksyntax_t *SYN = (ksyntax_t*)sfp[0].ndata;\
		ksymbol_t NAME = (ksymbol_t)sfp[1].ivalue;\
		kArray *TLS = (kArray*)sfp[2].o;\
		int S = (int)sfp[3].ivalue;\
		int E = (int)sfp[4].ivalue;\
		(void)STMT; (void)SYN; (void)NAME; (void)TLS; (void)S; (void)E;\

// Expr Stmt.parseExpr(Token[] tls, int s, int c, int e)
#define VAR_StmtParseExpr(STMT, SYN, TLS, S, C, E) \
		kStmt *STMT = (kStmt*)sfp[0].o;\
		ksyntax_t *SYN = (ksyntax_t*)sfp[0].ndata;\
		kArray *TLS = (kArray*)sfp[1].o;\
		int S = (int)sfp[2].ivalue;\
		int C = (int)sfp[3].ivalue;\
		int E = (int)sfp[4].ivalue;\
		(void)STMT; (void)SYN; (void)TLS; (void)S; (void)C; (void)E;\

// Expr Stmt.tycheck(Gamma gma)

#define VAR_StmtTyCheck(STMT, SYN, GMA) \
		kStmt *STMT = (kStmt*)sfp[0].o;\
		ksyntax_t *SYN = (ksyntax_t*)sfp[0].ndata;\
		kGamma *GMA = (kGamma*)sfp[1].o;\
		(void)STMT; (void)SYN; (void)GMA;\

// Expr Expr.tycheck(Gamma gma, int t)

#define VAR_ExprTyCheck(EXPR, SYN, GMA, TY) \
		kExpr *EXPR = (kExpr*)sfp[0].o;\
		ksyntax_t *SYN = (ksyntax_t*)sfp[0].ndata;\
		kGamma *GMA = (kGamma*)sfp[1].o;\
		ktype_t TY = (ktype_t)sfp[2].ivalue;\
		(void)EXPR; (void)SYN; (void)GMA; (void)TY;\

#define SYN_ExprFlag      1
#define SYN_isExpr(syn)   TFLAG_is(kflag_t, syn->flag, SYN_ExprFlag)

typedef const struct _ksyntax ksyntax_t;
struct _ksyntax {
	const char *token;
	keyword_t kw;  kflag_t flag;
	kArray   *syntaxRule;
	kMethod  *StmtAdd;
	kMethod  *StmtParseExpr;
	kMethod  *TopStmtTyCheck;
	kMethod  *StmtTyCheck;
	kMethod  *ExprTyCheck;
	// binary
	kshort_t priority;   kshort_t right;//
	kmethodn_t op2;      // a if b
	kmethodn_t op1;      // & a
	ktype_t    ty;       kshort_t dummy;
};

#define TOKEN(T)  .name = T

typedef struct DEFINE_SYNTAX_SUGAR {
	const char *name;
//	size_t      namelen;
	keyword_t kw;  kflag_t flag;
	const char *rule;
	const char *op2;
	const char *op1;
	int priority_op2; int right;
	int type;
	knh_Fmethod StmtAdd;
	knh_Fmethod StmtParseExpr;
	knh_Fmethod TopStmtTyCheck;
	knh_Fmethod StmtTyCheck;
	knh_Fmethod ExprTyCheck;
} DEFINE_SYNTAX_SUGAR;


typedef const struct _kKonohaSpace kKonohaSpace;

struct _kKonohaSpace {
	kObjectHeader h;
	kpack_t packid;  kpack_t packdom;
	const struct _kKonohaSpace   *parentNULL;
	struct kmap_t   *syntaxMapNN;
	//
	void                *gluehdr;
	kObject      *script;
	kcid_t static_cid;   kcid_t function_cid;
	kArray*       methodsNULL;
	karray_t cl;
};

typedef kshort_t    ksugar_t;
typedef kshort_t    kexpr_t;

typedef enum {
	TK_NONE,          // KW_ERR
	TK_INDENT,        // KW_EXPR
	TK_SYMBOL,        // KW_SYMBOL
	TK_USYMBOL,       // KW_USYMBOL
	TK_TEXT,
	TK_STEXT,
	TK_BTEXT,
	TK_INT,
	TK_FLOAT,
	TK_URN,
	TK_REGEX,
	TK_TYPE,
	AST_PARENTHESIS,
	AST_BRANCET,
	AST_BRACE,

	TK_OPERATOR,
	TK_CODE,          //
	TK_WHITESPACE,    //
	TK_METANAME,
	TK_MN,
	AST_OPTIONAL,  // for syntax sugar
//	// ast
//	AST_TYPE,
//	AST_EXPR,
//	AST_STMT,
//	AST_BLOCK
} ktoken_t ;

typedef const struct _kToken kToken;
struct _kToken {
	kObjectHeader h;
	kushort_t tt;   ksymbol_t kw;
	union {
		kString *text;
		kArray  *sub;
//		struct kExpr   *expr;
//		struct kStmt   *stmt;
//		struct kBlock  *bk;
	};
	kline_t     uline;
	union {
		kushort_t lpos;
		kshort_t  closech;  // ast
		ksymbol_t nameid;   // sugar
	};
	union {
		kshort_t   topch;
		ksymbol_t  symbol;
		ktype_t    ty;
		kmethodn_t mn;
	};
};

#define TEXPR_BLOCKLOCAL_   -2   /*THIS IS NEVER PASSED*/
#define TEXPR_UNTYPED       -1   /*THIS MUST NOT HAPPEN*/
#define TEXPR_CONST          0
#define TEXPR_NEW            1
#define TEXPR_NULL           2
#define TEXPR_NCONST         3
#define TEXPR_LOCAL          4
#define TEXPR_FIELD          5
#define TEXPR_BOX            6
#define TEXPR_UNBOX          7
#define TEXPR_CALL           8
#define TEXPR_AND            9
#define TEXPR_OR            10
#define TEXPR_LET           11

#define Expr_isCONST(o)     (TEXPR_CONST <= (o)->build && (o)->build <= TEXPR_NCONST)
#define Expr_isTerm(o)      (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local1))
#define Expr_setTerm(o,B)   TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local1,B)
#define kExpr_at(E,N)        ((E)->consNUL->exprs[(N)])

typedef const struct _kExpr kExpr;
struct _kExpr {
	kObjectHeader h;
	ktype_t ty; kexpr_t build;
	kToken *tkNUL;     // Term
	union {
		kObject* dataNUL;
		kArray*  consNUL;  // Cons
		kExpr*   singleNUL;
	};
	union {
		ksyntax_t *syn;
		kint_t     ivalue;
		kfloat_t   fvalue;
		uintptr_t  ndata;
		intptr_t   index;
		uintptr_t     cid;
//		uintptr_t	   mn;
	};
};

#define TSTMT_UNDEFINED      0
#define TSTMT_ERR            1
#define TSTMT_EXPR           2
#define TSTMT_BLOCK          3
#define TSTMT_RETURN         4
#define TSTMT_IF             5
#define TSTMT_LOOP           6

typedef const struct _kStmt kStmt;
struct _kStmt {
	kObjectHeader h;
	kline_t uline;
	ksyntax_t *syn;
	const struct _kBlock *parentNULL;
	kushort_t build;
};

typedef const struct _kBlock kBlock;
struct _kBlock {
	kObjectHeader h;
	kKonohaSpace        *ks;
	kStmt         *parentNULL;
	kArray        *blockS;
	kExpr         *esp;
};

typedef struct _kGamma kGamma;

typedef struct {
	ktype_t    ty;    ksymbol_t  fn;
} gammastack_t ;

#define kGamma_TOPLEVEL        (kflag_t)(1)
#define kGamma_isTOPLEVEL(GMA)  TFLAG_is(kflag_t, GMA->genv->flag, kGamma_TOPLEVEL)
#define kGamma_ERROR           (kflag_t)(1<<1)
#define kGamma_isERROR(GMA)    TFLAG_is(kflag_t, GMA->genv->flag, kGamma_ERROR)
#define kGamma_setERROR(GMA,B) TFLAG_set(kflag_t, GMA->genv->flag, kGamma_ERROR, B)

typedef struct {
	gammastack_t *vars;
	size_t varsize;
	size_t capacity;
	size_t allocsize;
} gstack_t ;

typedef struct gmabuf_t {
	kflag_t  flag;    kflag_t  cflag;

	kKonohaSpace     *ks;
	kObject          *scrNUL;

	kcid_t                   this_cid;
	kcid_t                   static_cid;
	kMethod*          mtd;
	gstack_t f;
	gstack_t l;
	kArray           *lvarlst;
	size_t lvarlst_top;
} gmabuf_t;

struct _kGamma {
	kObjectHeader h;
	struct gmabuf_t *genv;
};

/* ------------------------------------------------------------------------ */

#define ctxsugar    ((ctxsugar_t*)_ctx->modlocal[MOD_sugar])
#define kmodsugar  ((kmodsugar_t*)_ctx->modshare[MOD_sugar])
#define CT_Token    kmodsugar->cToken
#define CT_Expr     kmodsugar->cExpr
#define CT_Stmt     kmodsugar->cStmt
#define CT_Block    kmodsugar->cBlock
#define CT_KonohaSpace    kmodsugar->cKonohaSpace
#define CT_Gamma    kmodsugar->cGamma

#define IS_Token(O)  ((O)->h.ct == CT_Token)
#define IS_Expr(O)  ((O)->h.ct == CT_Expr)
#define IS_Stmt(O)  ((O)->h.ct == CT_Stmt)
#define IS_Block(O)  ((O)->h.ct == CT_Block)
#define IS_Gamma(O)  ((O)->h.ct == CT_Gamma)

#define K_NULLTOKEN  (kToken*)((CT_Token)->nulvalNUL)
#define K_NULLEXPR   (kExpr*)((CT_Expr)->nulvalNUL)
#define K_NULLBLOCK  (kBlock*)((CT_Block)->nulvalNUL)

#define TK_SHIFT    10000
#define KW_TK(N)    (((keyword_t)N)+TK_SHIFT)

#define KW_ERR     0
#define KW_EXPR    1
#define KW_SYMBOL  2
#define KW_name    2
#define KW_USYMBOL 3
#define KW_cname   3
#define KW_TEXT    4
#define KW_STEXT   5
#define KW_BTEXT   6
#define KW_INT     7
#define KW_FLOAT   8
#define KW_URN     9
#define KW_REGEX   10
#define KW_TYPE    11
#define KW_type    11
#define KW_PARENTHESIS  12
#define KW_BRANCET      13
#define KW_BRACE        14

//#define KW_BLOCK   15
#define KW_block   15
//#define KW_PARAMS  16
#define KW_params  16

#define KW_DOT     17
#define KW_DIV     (1+KW_DOT)
#define KW_MOD     (2+KW_DOT)
#define KW_MUL     (3+KW_DOT)
#define KW_ADD     (4+KW_DOT)
#define KW_SUB     (5+KW_DOT)
#define KW_LT      (6+KW_DOT)
#define KW_LTE     (7+KW_DOT)
#define KW_GT      (8+KW_DOT)
#define KW_GTE     (9+KW_DOT)
#define KW_EQ      (10+KW_DOT)
#define KW_NEQ     (11+KW_DOT)
#define KW_AND     (12+KW_DOT)
#define KW_OR      (13+KW_DOT)
#define KW_NOT     (14+KW_DOT)
#define KW_COLON   (15+KW_DOT)
#define KW_LET     (16+KW_DOT)
#define KW_COMMA   (17+KW_DOT)

#define KW_void      (18+KW_DOT)
#define KW_var       (1+KW_void)
#define KW_boolean   (2+KW_void)
#define KW_int       (3+KW_void)
#define KW_null      (4+KW_void)
#define KW_true      (5+KW_void)
#define KW_false     (6+KW_void)
#define KW_if        (7+KW_void)
#define KW_else      (8+KW_void)
#define KW_return    (9+KW_void)


#define SYN_ERR      kmodsugar->syn_err
#define SYN_EXPR     kmodsugar->syn_expr
#define SYN_CALL     kmodsugar->syn_expr

#define FN_this      FN_("this")

struct _kKonohaSpace;

#define kKonohaSpace_defineSyntax(L, S)  kmodsugar->KKonohaSpace_defineSyntax(_ctx, L, S)

typedef struct {
	kmodshare_t h;
	const kclass_t *cToken;
	const kclass_t *cExpr;
	const kclass_t *cStmt;
	const kclass_t *cBlock;
	const kclass_t *cKonohaSpace;
	const kclass_t *cGamma;
	//
	kArray         *keywordList;
	struct kmap_t         *keywordMapNN;
	kArray         *packageList;
	struct kmap_t         *packageMapNO;
	kKonohaSpace         *rootks;
	kArray         *aBuffer;

//	struct _kMethod *UndefinedStmtAdd;
	kMethod *UndefinedStmtParseExpr;
	kMethod *UndefinedStmtTyCheck;
	kMethod *UndefinedExprTyCheck;

	ksyntax_t *syn_err;
	ksyntax_t *syn_expr;

	// export
	keyword_t  (*keyword)(CTX, const char*, size_t, ksymbol_t);

	kExpr* (*Expr_setConstValue)(CTX, kExpr *expr, ktype_t ty, kObject *o);
	kExpr* (*Expr_setNConstValue)(CTX, kExpr *expr, ktype_t ty, uintptr_t ndata);
	kExpr* (*Expr_setVariable)(CTX, kExpr *expr, kexpr_t build, ktype_t ty, intptr_t index, kGamma *gma);

	kToken* (*Stmt_token)(CTX, kStmt *stmt, keyword_t kw, kToken *def);
	kExpr* (*Stmt_expr)(CTX, kStmt *stmt, keyword_t kw, kExpr *def);
	const char* (*Stmt_text)(CTX, kStmt *stmt, keyword_t kw, const char *def);
	kBlock* (*Stmt_block)(CTX, kStmt *stmt, keyword_t kw, kBlock *def);

	kExpr*     (*Expr_tyCheckAt)(CTX, kExpr *, size_t, kGamma *, ktype_t, int);
	kbool_t    (*Stmt_tyCheckExpr)(CTX, kStmt*, ksymbol_t, kGamma *, ktype_t, int);
	kbool_t    (*Block_tyCheckAll)(CTX, kBlock *, kGamma *);
	void       (*Stmt_toExprCall)(CTX, kStmt *stmt, kMethod *mtd, int n, ...);
	void       (*parseSyntaxRule)(CTX, const char*, kline_t, kArray *);
	ksyntax_t* (*KonohaSpace_syntax)(CTX, kKonohaSpace *, ksymbol_t, int);
	void       (*KonohaSpace_defineSyntax)(CTX, kKonohaSpace *, DEFINE_SYNTAX_SUGAR *);
	kMethod*   (*KonohaSpace_getMethodNULL)(CTX, kKonohaSpace *, kcid_t, kmethodn_t);
} kmodsugar_t;

typedef struct {
	kmodlocal_t h;
	kArray *tokens;
	karray_t cwb;
	kArray *errors;
	kBlock *singleBlock;
	kGamma *gma;
	kArray *lvarlst;
	kArray *definedMethods;
} ctxsugar_t;

#define TPOL_NOCHECK              1
#define TPOL_ALLOWVOID      (1 << 1)
#define TPOL_COERCION       (1 << 2)

#ifdef USING_SUGAR_AS_BUILTIN

#define KW_(T)      keyword(_ctx, T, sizeof(T)-1, FN_NONAME)
#define SYN_(KS, KW)                KonohaSpace_syntax(_ctx, KS, KW, 0)

#define kStmt_token(STMT, KW, DEF)  Stmt_token(_ctx, STMT, KW, DEF)
#define kStmt_expr(STMT, KW, DEF)   Stmt_expr(_ctx, STMT, KW, DEF)
#define kStmt_text(STMT, KW, DEF)   Stmt_text(_ctx, STMT, KW, DEF)
#define kStmt_block(STMT, KW, DEF)  Stmt_block(_ctx, STMT, KW, DEF)

#define new_ConstValue(T, O)  Expr_setConstValue(_ctx, NULL, T, UPCAST(O))
#define kExpr_setConstValue(EXPR, T, O)  Expr_setConstValue(_ctx, EXPR, T, UPCAST(O))
#define new_NConstValue(T, D)  Expr_setNConstValue(_ctx, NULL, T, D)
#define kExpr_setNConstValue(EXPR, T, D)  Expr_setNConstValue(_ctx, EXPR, T, D)
#define new_Variable(B, T, I, G)          Expr_setVariable(_ctx, NULL, B, T, I, G)
#define kExpr_setVariable(E, B, T, I, G)  Expr_setVariable(_ctx, E, B, T, I, G)
#define kExpr_tyCheckAt(E, N, GMA, T, P)     Expr_tyCheckAt(_ctx, E, N, GMA, T, P)
#define kStmt_tyCheck(E, NI, GMA, T, P)      Stmt_tyCheck(_ctx, STMT, NI, GMA, T, P)

#else/*SUGAR_EXPORTS*/
#define USING_SUGAR                          const kmodsugar_t *_e = (const kmodsugar_t *)kmodsugar
#define SUGAR                                _e->
#define TY_KonohaSpace                       _e->cKonohaSpace->cid
#define TY_Token                             _e->cToken->cid
#define TY_Stmt                              _e->cStmt->cid
#define TY_Block                             _e->cBlock->cid
#define TY_Expr                              _e->cExpr->cid
#define TY_Gamma                             _e->cGamma->cid

#define KW_(T)                               _e->keyword(_ctx, T, sizeof(T)-1, FN_NONAME)
#define SYN_(KS, KW)                         _e->KonohaSpace_syntax(_ctx, KS, KW, 0)
#define NEWSYN_(KS, KW)                      (struct _ksyntax*)_e->KonohaSpace_syntax(_ctx, KS, KW, 1)

#define kStmt_token(STMT, KW, DEF)           _e->Stmt_token(_ctx, STMT, KW, DEF)
#define kStmt_expr(STMT, KW, DEF)            _e->Stmt_expr(_ctx, STMT, KW, DEF)
#define kStmt_text(STMT, KW, DEF)            _e->Stmt_text(_ctx, STMT, KW, DEF)
#define kStmt_block(STMT, KW, DEF)           _e->Stmt_block(_ctx, STMT, KW, DEF)

#define new_ConstValue(T, O)                 _e->Expr_setConstValue(_ctx, NULL, T, UPCAST(O))
#define kExpr_setConstValue(EXPR, T, O)      _e->Expr_setConstValue(_ctx, EXPR, T, UPCAST(O))
#define new_NConstValue(T, D)                _e->Expr_setNConstValue(_ctx, NULL, T, D)
#define kExpr_setNConstValue(EXPR, T, D)     _e->Expr_setNConstValue(_ctx, EXPR, T, D)
#define new_Variable(B, T, I, I2, G)         _e->Expr_setVariable(_ctx, NULL, B, T, I, I2, G)
#define kExpr_setVariable(E, B, T, I, I2, G) _e->Expr_setVariable(_ctx, E, B, T, I, I2, G)
#define kExpr_tyCheckAt(E, N, GMA, T, P)     _e->Expr_tyCheckAt(_ctx, E, N, GMA, T, P)
#define kStmt_tyCheck(E, NI, GMA, T, P)      _e->Stmt_tyCheck(_ctx, STMT, NI, GMA, T, P)

#endif/*SUGAR_EXPORTS*/
///* ------------------------------------------------------------------------ */
///* Sugar API */
kstatus_t MODSUGAR_loadscript(CTX, const char *path, size_t len, kline_t pline);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------------ */


#endif /* SUGAR_H_ */
