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

#define TEXT(T)  T, (sizeof(T)-1)

typedef ksymbol_t keyword_t;
#define T_kw(X)   Pkeyword_(_ctx, X)
#define Skeyword(X)   Skeyword_(_ctx, X)


#define FLAG_METHOD_LAZYCOMPILE (0)

//static inline kflag_t kflag_set(kflag_t flag, int idx)
//{
//	kflag_t mask = 1ULL << idx;
//	return flag | mask;
//}
//static inline kflag_t kflag_unset(kflag_t flag, int idx)
//{
//	kflag_t mask = 1ULL << idx;
//	return flag ^ mask;
//}
//
//static inline int kflag_test(kflag_t flag, int idx)
//{
//	kflag_t mask = 1ULL << idx;
//	return (flag & mask) == mask;
//}

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
	kbool_t (*initPackage)(CTX, struct kLingo *, int, const char**, kline_t);
	kbool_t (*setupPackage)(CTX, struct kLingo *, kline_t);
	kbool_t (*initLingo)(CTX,  struct kLingo *, kline_t);
	kbool_t (*setupLingo)(CTX, struct kLingo *, kline_t);
	int konoha_revision;
} KPACKDEF_;

typedef const KPACKDEF_ KPACKDEF;
typedef KPACKDEF* (*Fpackageinit)(void);

typedef struct {
	kpkg_t               pid;
	kString             *name;
	struct kLingo       *lgo;
	KPACKDEF            *packdef;
	kline_t              export_script;
} kpackage_t;

#define VAR_StmtAdd(STMT, NAME, TLS, S, E) \
		kStmt *STMT = (kStmt*)sfp[0].o;\
		ksymbol_t NAME = (ksymbol_t)sfp[1].ivalue;\
		kArray *TLS = (kArray*)sfp[2].o;\
		int S = (int)sfp[3].ivalue;\
		int E = (int)sfp[4].ivalue;\
		(void)STMT; (void)NAME; (void)TLS; (void)S; (void)E;\

// Expr Stmt.tycheck(Gamma gma)

#define VAR_StmtTyCheck(STMT, GMA) \
		kStmt *STMT = (kStmt*)sfp[0].o;\
		kGamma *GMA = (kGamma*)sfp[1].o;\
		(void)STMT; (void)GMA;\

// Expr Expr.tycheck(Gamma gma, int t)

#define VAR_ExprTyCheck(EXPR, GMA, TY) \
		kExpr *EXPR = (kExpr*)sfp[0].o;\
		kGamma *GMA = (kGamma*)sfp[1].o;\
		ktype_t TY = (ktype_t)sfp[2].ivalue;\
		(void)EXPR; (void)GMA; (void)TY;\

typedef struct ksyntax_t {
	const char *token;
	keyword_t keyid;
	kflag_t   flag;
	kArray   *syntaxRule;
	kMethod  *StmtAdd;
	kMethod  *ExprTyCheck;
	kMethod  *StmtTyCheck;
	// binary
	kshort_t priority;   kshort_t right;//
	kmethodn_t op2;      // a if b
	kmethodn_t op1;      // & a
	ktype_t    ty;       kshort_t dummy;
} ksyntax_t ;

typedef struct ksyntaxdef_t {
	int keyid;
	const char *name;
	size_t      namelen;
	const char *rule;
	const char *op2;
	const char *op1;
	int priority_op2; int right;
	int type;
	knh_Fmethod StmtAdd;
	knh_Fmethod StmtTyCheck;
	knh_Fmethod ExprTyCheck;
} ksyntaxdef_t;

typedef struct kLingo kLingo;

struct kLingo {
	kObjectHeader h;
	kpkg_t pid; kpkg_t nsid;
	struct kLingo   *parentNULL;
	struct kmap_t   *syntaxMapNN;
	struct kmap_t   *symtblMapSO;
	//
	void                *gluehdr;
	struct kObject      *script;
	kcid_t static_cid;    // kLingo_getStaticMethodNULL
	kcid_t function_cid;
	struct kArray*       methodsNULL;
};

typedef kshort_t    ksugar_t;
typedef kushort_t   kexpr_t;

typedef enum {
	TK_NONE,
	TK_CODE,
	TK_INDENT,
	TK_WHITESPACE,
	TK_OPERATOR,
	TK_SYMBOL,
	TK_USYMBOL,
	TK_KEYWORD,
	TK_TEXT,
	TK_STEXT,
	TK_BTEXT,
	TK_INT,
	TK_FLOAT,
	TK_URN,
	TK_REGEX,
	TK_METANAME,
	TK_MN,
	TK_TYPE,
	// ast
	AST_PARENTHESIS,
	AST_BRACE,
	AST_BRANCET,
	AST_OPTIONAL,  // for syntax sugar
	AST_TYPE,
	AST_EXPR,
	AST_STMT,
	AST_BLOCK
} ktoken_t ;

typedef struct kToken kToken;
struct kToken {
	kObjectHeader h;
	ktoken_t tt;
	union {
		struct kString *text;
		struct kArray  *sub;
		struct kExpr   *expr;
		struct kStmt   *stmt;
		struct kBlock  *bk;
	};
	kline_t     uline;
	union {
		kushort_t lpos;
		kshort_t  closech;
		ksymbol_t nameid;
	};
	union {
		kshort_t  topch;
		keyword_t keyid;
		ksymbol_t symbol;
		ktype_t   ty;
		kmethodn_t mn;
	};
};

#define TEXPR_BLOCKLOCAL_   -2   /*THIS IS NEVER PASSED*/
#define TEXPR_UNTYPED       -1   /*THIS MUST NOT HAPPEN*/
#define TEXPR_CONST          0
#define TEXPR_NCONST         1
#define TEXPR_LOCAL          2
#define TEXPR_FIELD          3
#define TEXPR_NEW            4
#define TEXPR_NULL           5
#define TEXPR_BOX            6
#define TEXPR_UNBOX          7
#define TEXPR_CALL           8
#define TEXPR_AND            9
#define TEXPR_OR            10
#define TEXPR_LET           11

#define Expr_isTerm(o)      (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local1))
#define Expr_setTerm(o,B)   TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local1,B)

typedef struct kExpr kExpr;
struct kExpr {
	kObjectHeader h;
	ktype_t ty; kexpr_t build;
	kToken *tkNUL;     // Term
	union {
		struct kObject* dataNUL;
		struct kArray*  consNUL;  // Term
	};
	union {
		ksyntax_t *syn;
		kint_t     ivalue;
		kfloat_t   fvalue;
		uintptr_t  ndata;
		union {
			kshort_t   index;
			kcid_t     cid;
			kmethodn_t mn;
		};
		kflag_t xindex;
	};
};

#define TSTMT_UNDEFINED      0
#define TSTMT_ERR            1
#define TSTMT_EXPR           2
#define TSTMT_BLOCK          3
#define TSTMT_RETURN         4
#define TSTMT_IF             5
#define TSTMT_LOOP           6

typedef struct kStmt kStmt;
struct kStmt {
	kObjectHeader h;
	kline_t uline;
	ksyntax_t *syn;
	struct kBlock     *parentNULL;
	kushort_t build;
};

typedef struct kBlock kBlock;
struct kBlock {
	kObjectHeader h;
	struct kLingo        *lgo;
	struct kStmt         *parentNULL;
	struct kArray        *blockS;
	struct kExpr         *esp;
};

typedef struct kGamma kGamma;

typedef struct {
	kflag_t    flag;  kushort_t  index;
	ktype_t    ty;    ksymbol_t  fn;
	union {
		kToken   *name;
	};
	union {
		kExpr    *value;
	};
} gammastack_t ;

typedef struct gmabuf_t {
	kflag_t                 flag;
	kflag_t                 cflag;

	struct kLingo           *lgo;
	struct kScript          *scrNUL;

	kcid_t                   this_cid;
	kcid_t                   static_cid;
	struct kMethod*          mtd;

	gammastack_t* fvars;
	int fvarsize; int fcapacity; int fallocsize;
	gammastack_t* lvars;
	int lvarsize; int lcapacity; int lallocsize;
	struct kArray           *lvarlst;
	size_t lvarlst_top;
} gmabuf_t;

struct kGamma {
	kObjectHeader h;
	struct gmabuf_t *genv;
};

/* ------------------------------------------------------------------------ */

#define kevalmod    ((kevalmod_t*)_ctx->mod[MOD_EVAL])
#define kevalshare  ((kevalshare_t*)_ctx->modshare[MOD_EVAL])
#define CT_Token    kevalshare->cToken
#define CT_Expr     kevalshare->cExpr
#define CT_Stmt     kevalshare->cStmt
#define CT_Block    kevalshare->cBlock
#define CT_Lingo    kevalshare->cLingo
#define CT_Gamma    kevalshare->cGamma

#define IS_Token(O)  ((O)->h.ct == CT_Token)
#define IS_Expr(O)  ((O)->h.ct == CT_Expr)
#define IS_Stmt(O)  ((O)->h.ct == CT_Stmt)
#define IS_Block(O)  ((O)->h.ct == CT_Block)
#define IS_Gamma(O)  ((O)->h.ct == CT_Gamma)

#define K_NULLTOKEN  kevalshare->nullToken
#define K_NULLEXPR   kevalshare->nullExpr
#define K_NULLBLOCK  kevalshare->nullBlock

#define KW_(T)      keyword(_ctx, T, sizeof(T)-1, FN_NONAME)
#define KW_EXPR     1
#define KW_BLOCK    2
#define KW_DOT      kevalshare->kw_dot
#define KW_COMMA    kevalshare->kw_comma
#define KW_COLON    kevalshare->kw_colon
#define KW_DECLMETHOD  kevalshare->kw_declmethod
#define KW_DECLTYPE    kevalshare->kw_decltype
#define KW_PARAMS      kevalshare->kw_params
#define KW_THEN        kevalshare->kw_then
#define KW_ELSE        kevalshare->kw_else

#define TK_SHIFT    10000
#define KW_TK(N)    (((keyword_t)N)+TK_SHIFT)

#define SYN_ERR      kevalshare->syn_err
#define SYN_EXPR     kevalshare->syn_expr
#define SYN_CALL     kevalshare->syn_call
#define SYN_INVOKE   kevalshare->syn_invoke
#define SYN_PARAMS   kevalshare->syn_params
#define SYN_RETURN   kevalshare->syn_return
#define SYN_BREAK    kevalshare->syn_break
#define SYN_TYPEDECL kevalshare->syn_typedecl

#define FN_this      FN_("this")

struct kLingo;
struct ksyntaxdef_t;
#define kLingo_defineSyntax(L, S)  kevalshare->KLingo_defineSyntax(_ctx, L, S)

typedef struct {
	kmodshare_t h;
	const kclass_t *cToken;
	const kclass_t *cExpr;
	const kclass_t *cStmt;
	const kclass_t *cBlock;
	const kclass_t *cLingo;
	const kclass_t *cGamma;
	//
	struct kArray         *keywordList;
	struct kmap_t         *keywordMapNN;
	struct kArray         *packageList;
	struct kmap_t         *packageMapNO;
	struct kLingo         *rootlgo;
	struct kArray         *aBuffer;

	struct kToken *nullToken;
	struct kExpr  *nullExpr;
	struct kBlock *nullBlock;

	keyword_t kw_dot;
	keyword_t kw_comma;
	keyword_t kw_colon;
	keyword_t kw_declmethod;
	keyword_t kw_decltype;
	keyword_t kw_params;
	keyword_t kw_then;
	keyword_t kw_else;

	struct ksyntax_t *syn_err;
	struct ksyntax_t *syn_expr;
	struct ksyntax_t *syn_call;
	struct ksyntax_t *syn_invoke;
	struct ksyntax_t *syn_params;
	struct ksyntax_t *syn_return;
	struct ksyntax_t *syn_break;
	struct ksyntax_t *syn_typedecl;

	// export
	keyword_t  (*keyword)(CTX, const char*, size_t, ksymbol_t);

	kExpr* (*Expr_setConstValue)(CTX, kExpr *expr, ktype_t ty, kObject *o);
	kExpr* (*Expr_setNConstValue)(CTX, kExpr *expr, ktype_t ty, uintptr_t ndata);
	kExpr* (*Expr_setVariable)(CTX, kExpr *expr, kexpr_t build, ktype_t ty, int index, int xindex, kGamma *gma);

	kbool_t    (*Stmt_tyCheckExpr)(CTX, kStmt*, ksymbol_t, kGamma *, ktype_t, int);
	kBlock*    (*Stmt_getBlock)(CTX, kStmt *, ksymbol_t, kBlock*);
	kbool_t    (*Block_tyCheckAll)(CTX, kBlock *, kGamma *);
	void       (*parseSyntaxRule)(CTX, const char*, kline_t, kArray *);
	ksyntax_t* (*Lingo_syntax)(CTX, kLingo *, ksymbol_t, int);
	void       (*Lingo_defineSyntax)(CTX, kLingo *, ksyntaxdef_t *);
	kMethod*   (*Lingo_getMethodNULL)(CTX, kLingo *, kcid_t, kmethodn_t);
} kevalshare_t;

typedef struct {
	kmod_t h;
	struct kArray *tokens;
	karray_t       cwb;
	struct kArray *errors;
	struct kBlock *singleBlock;
	struct kGamma *gma;
	struct kArray *lvarlst;
	struct kArray *definedMethods;
	kshort_t iseval;
	ktype_t evalty;
	int     evalidx;
	kjmpbuf_t* evaljmpbuf;
	kflag_t flags;
} kevalmod_t;

#ifdef USING_SUGAR_AS_BUILTIN
#define new_ConstValue(T, O)  Expr_setConstValue(_ctx, NULL, T, UPCAST(O))
#define kExpr_setConstValue(EXPR, T, O)  Expr_setConstValue(_ctx, EXPR, T, UPCAST(O))
#define new_NConstValue(T, D)  Expr_setNConstValue(_ctx, NULL, T, D)
#define kExpr_setNConstValue(EXPR, T, D)  Expr_setNConstValue(_ctx, EXPR, T, D)
#define new_Variable(B, T, I, I2, G)         Expr_setVariable(_ctx, NULL, B, T, I, I2, G)
#define kExpr_setVariable(E, B, T, I, I2, G) Expr_setVariable(_ctx, E, B, T, I, I2, G)

#else/*SUGAR_EXPORTS*/
#define USING_SUGAR                          const kevalshare_t *_e = (const kevalshare_t *)kevalshare
#define SUGAR                                _e->
#define new_ConstValue(T, O)                 _e->Expr_setConstValue(_ctx, NULL, T, UPCAST(O))
#define kExpr_setConstValue(EXPR, T, O)      _e->Expr_setConstValue(_ctx, EXPR, T, UPCAST(O))
#define new_NConstValue(T, D)                _e->Expr_setNConstValue(_ctx, NULL, T, D)
#define kExpr_setNConstValue(EXPR, T, D)     _e->Expr_setNConstValue(_ctx, EXPR, T, D)
#define new_Variable(B, T, I, I2, G)         _e->Expr_setVariable(_ctx, NULL, B, T, I, I2, G)
#define kExpr_setVariable(E, B, T, I, I2, G) _e->Expr_setVariable(_ctx, E, B, T, I, I2, G)


#endif/*SUGAR_EXPORTS*/
///* ------------------------------------------------------------------------ */
///* Sugar API */
//extern kMethod* Lingo_getMethodNULL(CTX, kLingo *ns, kcid_t cid, kmethodn_t mn);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------------ */


#endif /* SUGAR_H_ */
