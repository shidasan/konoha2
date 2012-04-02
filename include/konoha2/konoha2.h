/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
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

#ifndef KONOHA2_H_
#define KONOHA2_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define K_CLASSTABLE_INIT 64
#define K_PAGESIZE        4096

#ifndef K_OSDLLEXT
#define K_OSDLLEXT        ".dylib"
#endif

#ifndef K_REVISION
#define K_REVISION  1
#endif

//#include"konoha2/konoha_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>

#if defined(K_USING_SIGNAL)
#include <signal.h>
#endif

#if defined(K_USING_PTHREAD)
#include <pthread.h>
#endif

/* ------------------------------------------------------------------------ */
/* type */

#include <limits.h>
#include <float.h>

#include <stdbool.h>
#include <stdint.h>

#if defined(__LP64__) || defined(_WIN64)
#define K_USING_SYS64_    1
typedef int32_t           kshort_t;
typedef uint32_t          kushort_t;
#ifdef K_USING_NOFLOAT
typedef uintptr_t         kfloat_t;
#else
typedef double            kfloat_t;
#endif
#else
typedef int16_t           kshort_t;
typedef uint16_t          kushort_t;
#ifdef K_USING_NOFLOAT
typedef uintptr_t         kfloat_t;
#else
typedef float             kfloat_t;
#endif
#endif

typedef bool             kbool_t;

typedef enum {
	K_FAILED, K_CONTINUE, K_BREAK
} kstatus_t;

typedef intptr_t         kint_t;
typedef uintptr_t        kuint_t;

#ifdef K_USING_SYS64_

#ifndef LLONG_MIN
#define LLONG_MIN -9223372036854775807LL
#define LLONG_MAX  9223372036854775807LL
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif

#define KINT_MAX               LLONG_MAX
#define KINT_MIN               LLONG_MIN
#define KINT_FMT               "%lld"
//#define KINT_XFMT              "%llx"
//#define KINT0                  0ULL
//#define KUINT_MAX              ULLONG_MAX
//#define KUINT_MIN              0ULL
//#define KUINT_FMT              "%llu"
//#define knh_abs(n)              llabs(n)

#else/*K_USING_SYS64_*/

#define KINT_MAX               LONG_MAX
#define KINT_MIN               LONG_MIN
#define KINT0                  0UL
#define KINT_FMT               "%ld"
#define KINT_XFMT              "%lx"
#define KUINT_MAX              ULONG_MAX
#define KUINT_MIN              0
#define KUINT_FMT              "%lu"
#define knh_abs(n)              labs(n)
#define VMTX_INT
#define VMTSIZE_int 0

#endif/*K_USING_SYS64_*/

typedef intptr_t         kindex_t;
typedef kushort_t        kflag_t;    /* flag field */

#ifdef __GCC__
#define __PRINT_FMT(idx1, idx2) __attribute__((format(printf, idx1, idx2)))
#else
#define __PRINT_FMT(idx1, idx2)
#endif

#define KFLAG_H(N)               ((sizeof(kflag_t)*8)-N)
#define KFLAG_H0                 ((kflag_t)(1 << KFLAG_H(1)))
#define KFLAG_H1                 ((kflag_t)(1 << KFLAG_H(2)))
#define KFLAG_H2                 ((kflag_t)(1 << KFLAG_H(3)))
#define KFLAG_H3                 ((kflag_t)(1 << KFLAG_H(4)))
#define KFLAG_H4                 ((kflag_t)(1 << KFLAG_H(5)))
#define KFLAG_H5                 ((kflag_t)(1 << KFLAG_H(6)))
#define KFLAG_H6                 ((kflag_t)(1 << KFLAG_H(7)))
#define KFLAG_H7                 ((kflag_t)(1 << KFLAG_H(8)))

#define TFLAG_is(T,f,op)          (((T)(f) & (T)(op)) == (T)(op))
#define TFLAG_set1(T,f,op)        f = (((T)(f)) | ((T)(op)))
#define TFLAG_set0(T,f,op)        f = (((T)(f)) & (~((T)(op))))
#define TFLAG_set(T,f,op,b)       if(b) {TFLAG_set1(T,f,op);} else {TFLAG_set0(T,f,op);}

#define FLAG_set(f,op)            TFLAG_set1(kflag_t,f,op)
#define FLAG_unset(f,op)          TFLAG_set0(kflag_t,f,op)
#define FLAG_is(f,op)             TFLAG_is(kflag_t,f,op)

/* uline */

typedef kushort_t                 kuri_t;
#define URI_unknown               ((kuri_t)-1)
#define URI_EVAL                  ((kuri_t)0)
#define URI_UNMASK(uri)           (uri)

#define URI__(uri) S_text(knh_getURN(_ctx, uri))
#define FILENAME__(uri) knh_sfile(URI__(uri))

typedef uintptr_t                 kline_t;
#define new_ULINE(uri, line)       ((((kline_t)uri) << (sizeof(kuri_t) * 8)) | ((kushort_t)line))
#define ULINE_setURI(line, uri)    line |= (((kline_t)uri) << (sizeof(kuri_t) * 8))
#define ULINE_uri(line)            ((kuri_t)(line >> (sizeof(kuri_t) * 8)))
#define ULINE_line(line)           (line & (kline_t)((kuri_t)-1))

#define kjmpbuf_t       sigjmp_buf
#define ksetjmp(B)      sigsetjmp(B, 0)
#define klongjmp(B, N)  siglongjmp(B, N)

/* ------------------------------------------------------------------------ */

#define REF_t           struct kObject*

typedef struct {
	union {
		void  *body;
		char  *buf;
		const struct kclass_t **ClassTBL;
		REF_t *refhead;
	};
	size_t size;
	size_t max;
} karray_t ;


typedef struct kwb_t {
	karray_t *w;
	size_t pos;
} kwb_t;

// kmap

typedef struct kmape_t {
	uintptr_t hcode;
	struct kmape_t *next;
	union {
		struct kString  *skey;
		uintptr_t        ukey;
		void            *pkey;
	};
	union {
		struct kObject  *ovalue;
		void            *pvalue;
		uintptr_t        uvalue;
	};
} kmape_t;

typedef struct kmap_t {
	kmape_t *arena;
	kmape_t *unused;
	kmape_t **hentry;
	size_t arenasize;
	size_t size;
	size_t hmax;
} kmap_t;

/* ------------------------------------------------------------------------ */

typedef const struct kcontext_t* konoha_t;

struct  kcontext_t;
typedef const struct kcontext_t *const CTX_t;
#define CTX      CTX_t _ctx

#define K_PKGMATRIX 128
struct kObject;

#define MOD_CODE  0
#define MOD_EVAL  1
#define MOD_FLOAT 2

struct kmod_t;
typedef struct kmod_t {
	uintptr_t unique;
	void (*reftrace)(CTX, struct kmod_t *);
	void (*free)(CTX, struct kmod_t *);
} kmod_t;

struct kmodshare_t;
typedef struct kmodshare_t {
	const char *name;
	int mod_id;
	void (*setup)(CTX, struct kmodshare_t *);
	void (*reftrace)(CTX, struct kmodshare_t *);
	void (*free)(CTX, struct kmodshare_t *);
} kmodshare_t;

#define CTX_isInteractive()  1
#define CTX_isCompileOnly()  0
#define CTX_isDebug()        1

typedef struct kcontext_t {
	int						          safepoint; // set to 1
	struct ksfp_t                    *esp;
	struct klib2_t                   *lib2;

	struct kmemshare_t                *memshare;
	struct kmemlocal_t                *memlocal;
	struct kshare_t                   *share;
	struct klocal_t                   *local;
	struct kstack_t                   *stack;
	struct klogger_t                  *logger;
	struct kmodshare_t               **modshare;
	struct kmod_t                    **mod;

} kcontext_t ;

typedef struct kshare_t {
	karray_t ca;
	struct kmap_t         *classnameMapNN;
	/* system shared const */
	struct kObject       *constNull;
	struct kBoolean      *constTrue;
	struct kBoolean      *constFalse;
	struct kString       *emptyString;
	struct kArray        *emptyArray;
	struct kParam        *nullParam;

	struct kArray         *uriList;
	struct kmap_t         *uriMapNN;
	struct kArray         *symbolList;
	struct kmap_t         *symbolMapNN;
	struct kArray         *pkgList;
	struct kmap_t         *pkgMapNN;
} kshare_t ;

#define K_FRAME_NCMEMBER \
		uintptr_t   ndata;  \
		kbool_t     bvalue; \
		kint_t      ivalue; \
		kuint_t     uvalue; \
		kfloat_t    fvalue; \
		intptr_t    shift;  \
		uintptr_t   uline; \
		struct kopl_t  *pc; \
		struct kMethod *mtdNC; \
		const char     *fname \

#define K_FRAME_MEMBER \
		struct kObject *o; \
		struct kInt    *i; \
		struct kFloat  *f; \
		struct kClass  *c; \
		struct kString *s; \
		struct kDate *dt;\
		struct kBytes  *ba; \
		struct kRegex  *re; \
		struct kRange  *range; \
		struct kArray  *a; \
		struct kIterator *it; \
		struct kMap           *m;    \
		struct kFunc         *fo; \
		struct kPath         *pth; \
		struct kInputStream  *in; \
		struct kOutputStream *w;  \
		struct kView *rel;\
		struct kMethod            *mtd;\
		struct kTypeMap           *tmr;\
		struct kException         *e;\
		struct kExceptionHandler  *hdr; \
		struct kKonohaSpace             *ks;\
		struct kRawPtr   *p; \
		struct kObject *ox; \
		struct kConverter         *conv;\
		struct kContext           *cx;\
		struct kScript            *scr;\
		struct kToken             *tk;\
		struct kStmt              *stmt;\
		struct kExpr              *expr;\
		struct kBlock             *bk;\
		struct kGamma             *gma;\
		kint_t     dummy_ivalue;\
		kfloat_t   dummy_fvalue \

typedef struct ksfp_t {
	union {
		K_FRAME_MEMBER;
	};
	union {
		K_FRAME_NCMEMBER;
	};
} ksfp_t;

typedef struct krbp_t {
	union {
		K_FRAME_NCMEMBER;
		K_FRAME_MEMBER;
	};
} krbp_t;

typedef struct kstack_t {
	struct ksfp_t*               stack;
	size_t                       stacksize;
	struct ksfp_t*               stack_uplimit;
	struct kArray                *gcstack;
	karray_t                     cwb;
	CTX_t                        *rootctx;
	void*                        cstack_bottom;  // for GC
	karray_t                     ref;   // reftrace
	REF_t                        *tail;
} kstack_t;

// classdef_t

typedef kushort_t       kpkg_t;   /* package id*/
typedef kushort_t       kcid_t;    /* class id */
typedef kushort_t       ktype_t;     /* extended ktype_t */
typedef kushort_t       ksymbol_t;
typedef kushort_t       kmethodn_t;

/* kcid_t */
#define CLASS_newid                ((kcid_t)-1)
#define CLASS_unknown              ((kcid_t)-2)

#define CT_(t)              (_ctx->share->ca.ClassTBL[t])
#define TY_isUnbox(t)       FLAG_is(CT_(t)->cflag, kClass_UnboxType)

#define TY_T0             ((ktype_t)KFLAG_H2)
#define TY_This           TY_T0
#define TY_T(n)           (TY_T0+(n))
#define TY_T1             TY_T(1)
#define TY_T2             TY_T(2)

#define FN_NONAME          ((ksymbol_t)-1)
#define FN_NEWID           ((ksymbol_t)-2)
#define FN_UNMASK(fnq)     (fnq & (~(KFLAG_H0|KFLAG_H1|KFLAG_H2)))
#define FN_COERCION        KFLAG_H0
#define FN_Coersion        FN_COERCION
#define FN_isCOERCION(fn)  ((fn & FN_COERCION) == FN_COERCION)

#define MN_NONAME    ((kmethodn_t)-1)
#define MN_NEWID     ((kmethodn_t)-2)

#define MN_UNMASK(fnq)       (fnq & (~(KFLAG_H0|KFLAG_H1|KFLAG_H2)))
#define MN_ISBOOL     KFLAG_H0
#define MN_GETTER     KFLAG_H1
#define MN_SETTER     KFLAG_H2
#define MN_TOCID      (KFLAG_H0|KFLAG_H1)
#define MN_ASCID      (KFLAG_H0|KFLAG_H1|KFLAG_H2)

#define MN_isISBOOL(mn)   ((mn & MN_ISBOOL) == MN_ISBOOL)
#define MN_toISBOOL(mn)   (mn | MN_ISBOOL)
#define MN_isGETTER(mn)   ((mn & MN_GETTER) == MN_GETTER)
#define MN_toGETTER(mn)   (mn | MN_GETTER)
#define MN_isSETTER(mn)   ((mn & MN_SETTER) == MN_SETTER)
#define MN_toSETTER(mn)   (mn | MN_SETTER)

#define MN_to(cid)        (cid | MN_TOCID)
#define MN_isTOCID(mn)    ((mn & MN_TOCID) == MN_TOCID)
#define MN_as(cid)        (cid | MN_ASCID)
#define MN_isASCID(mn)    ((mn & MN_ASCID) == MN_ASCID)

typedef struct kfield_t {
	kflag_t    flag    ;
	kshort_t   isobj   ;
	ktype_t    ty      ;
	ksymbol_t  fn      ;
} kfield_t ;

struct kp_api;

#define KP_STR    0
#define KP_DUMP   1

#define KCLASSSPI \
		void (*init)(CTX, struct kRawPtr*, void *conf);\
		void (*reftrace)(CTX, struct kRawPtr*);\
		void (*free)(CTX, struct kRawPtr*);\
		struct kObject* (*fnull)(CTX, const struct kclass_t *);\
		void (*p)(CTX, ksfp_t *, int, kwb_t *, int);\
		uintptr_t (*unbox)(CTX, struct kObject*);\
		int  (*compareTo)(struct kRawPtr*, struct kRawPtr*);\
		struct kString* (*getkey)(CTX, struct kRawPtr*);\
		kuint_t (*hashCode)(CTX, struct kRawPtr*);\
		void (*initdef)(CTX, struct kclass_t *, kline_t)

typedef struct KCLASSDEF {
	const char *structname;
	kcid_t     cid;         kflag_t    cflag;
	kcid_t     bcid;        kcid_t     supcid;
	size_t     cstruct_size;
	kfield_t   *fields;
	kushort_t  fsize;        kushort_t fallocsize;
	kpkg_t     packid;       kpkg_t packdom;
	KCLASSSPI;
} KCLASSDEF;

//typedef const KCLASSDEF KCLASSDEF;

#define STRUCTNAME(C) \
	.structname = #C,\
	.cid = CLASS_newid,\
	.cstruct_size = sizeof(k##C)\

struct kString;
struct kRawPtr;
struct kclass_t;

typedef uintptr_t kmagicflag_t;

typedef struct kclass_t {
	KCLASSSPI;
	kcid_t   cid;          kflag_t  cflag;
	kcid_t   bcid;         kcid_t   supcid;
	kcid_t   p1;           kcid_t   p2;
	kpkg_t   packid;       kpkg_t   packdom;
	kmagicflag_t magicflag;
	size_t     cstruct_size;
	kfield_t  *fields;
	kushort_t  fsize;      kushort_t fallocsize;
//	void *fieldinit;
	//
	struct kClass            *typeNUL;
	struct kParam            *cparam;
	struct kString           *name;
	struct kString           *fullnameNUL;
	struct kArray            *methods;
	union {
		struct kObject       *nulvalNUL;
//		struct kFunc         *deffunc;
	};
	struct kmap_t            *constNameMapSO;
	struct kmap_t            *constPoolMapNO;
	const struct kclass_t    *simbody;
	const char               *DBG_NAME;
} kclass_t;

/* ----------------------------------------------------------------------- */
/* CLASS */

/* ------------------------------------------------------------------------ */
/* mini konoha */

#define CLASS_Tvoid             ((kcid_t)0)
#define CLASS_Tvar              ((kcid_t)1)
#define CLASS_Object            ((kcid_t)2)
#define CLASS_Boolean           ((kcid_t)3)
#define CLASS_Int               ((kcid_t)4)
#define CLASS_String            ((kcid_t)5)
#define CLASS_Array             ((kcid_t)6)
#define CLASS_Param             ((kcid_t)7)
#define CLASS_Method            ((kcid_t)8)
#define CLASS_System            ((kcid_t)9)
#define CLASS_Tdynamic          ((kcid_t)10)

#define CT_Object               CT_(CLASS_Object)
#define CT_Boolean              CT_(CLASS_Boolean)
#define CT_Int                  CT_(CLASS_Int)
#define CT_String               CT_(CLASS_String)
#define CT_Array                CT_(CLASS_Array)
#define CT_Param                CT_(CLASS_Param)
#define CT_Method               CT_(CLASS_Method)

#define kClass_Ref              ((kflag_t)(1<<0))
#define kClass_Prototype        ((kflag_t)(1<<1))
#define kClass_Immutable        ((kflag_t)(1<<2))
#define kClass_Private          ((kflag_t)(1<<4))
#define kClass_Final            ((kflag_t)(1<<5))
#define kClass_Singleton        ((kflag_t)(1<<6))
#define kClass_UnboxType        ((kflag_t)(1<<7))
#define kClass_Interface        ((kflag_t)(1<<8))
#define kClass_TypeVar          ((kflag_t)(1<<9))
#define kClass_UNDEF            ((kflag_t)(1<<10))

//#define T_isRef(t)          (TFLAG_is(kflag_t,(ClassTBL(t))->cflag, kClass_Ref))
//#define T_isPrototype(t)    (TFLAG_is(kflag_t,(ClassTBL(t))->cflag, kClass_Expando))
//#define T_isImmutable(t)    (TFLAG_is(kflag_t,(ClassTBL(t))->cflag, kClass_Immutable))
//#define T_isPrivate(t)      (TFLAG_is(kflag_t,(ClassTBL(t))->cflag, kClass_Private))
//#define T_isFinal(t)        (TFLAG_is(kflag_t,(ClassTBL(t))->cflag, kClass_Final))

#define TY_isSingleton(T)     (TFLAG_is(kflag_t,(CT_(T))->cflag, kClass_Singleton))
#define CT_isSingleton(ct)    (TFLAG_is(kflag_t,(ct)->cflag, kClass_Singleton))

#define CT_isFinal(ct)         (TFLAG_is(kflag_t,(ct)->cflag, kClass_Final))
#define TY_isUNDEF(T)         (TFLAG_is(kflag_t,(CT_(T))->cflag, kClass_UNDEF))
#define CT_isUNDEF(ct)        (TFLAG_is(kflag_t,(ct)->cflag, kClass_UNDEF))
#define CT_setUNDEF(ct, B)    TFLAG_set(kflag_t, (ct)->cflag, kClass_UNDEF, B)

//#define TY_isUnboxType(t)    (TFLAG_is(kflag_t,(ClassTBL(t))->cflag, kClass_UnboxType))
//#define T_isInterface(t)    (TFLAG_is(kflag_t,(ClassTBL(t))->cflag, kClass_Interface))
//#define T_isTypeVar(t)      (TFLAG_is(kflag_t,(ClassTBL(t))->cflag, kClass_TypeVar))

/* magic flag */
#define MAGICFLAG(f)             (K_OBJECT_MAGIC | ((kmagicflag_t)(f) & K_CFLAGMASK))

#define kObject_NullObject       ((kmagicflag_t)(1<<0))

#define kObject_Local6           ((kmagicflag_t)(1<<10))
#define kObject_Local5           ((kmagicflag_t)(1<<11))
#define kObject_Local4           ((kmagicflag_t)(1<<12))
#define kObject_Local3           ((kmagicflag_t)(1<<13))
#define kObject_Local2           ((kmagicflag_t)(1<<14))
#define kObject_Local1           ((kmagicflag_t)(1<<15))

#define kObject_is(O,A)            (TFLAG_is(kmagicflag_t,(O)->h.magicflag,A))
#define kObject_set(O,A,B)         TFLAG_set(kmagicflag_t,(O)->h.magicflag,A,B)

#define kField_Hidden          ((kflag_t)(1<<0))
#define kField_Protected       ((kflag_t)(1<<1))
#define kField_Getter          ((kflag_t)(1<<2))
#define kField_Setter          ((kflag_t)(1<<3))
#define kField_Key             ((kflag_t)(1<<4))
#define kField_Volatile        ((kflag_t)(1<<5))
#define kField_ReadOnly        ((kflag_t)(1<<6))
#define kField_Property        ((kflag_t)(1<<7))

/* ------------------------------------------------------------------------ */
/* Type Variable */
//## @TypeVariable class Tvoid Tvoid;
//## @TypeVariable class Tvar  Tvoid;

#define OFLAG_Tvoid              MAGICFLAG(0)
#define CFLAG_Tvoid              kClass_TypeVar|kClass_UnboxType|kClass_Singleton|kClass_Final
#define TY_void                  CLASS_Tvoid
#define OFLAG_Tvar               MAGICFLAG(0)
#define CFLAG_Tvar               CFLAG_Tvoid
#define TY_var                   CLASS_Tvar

/* ------------------------------------------------------------------------ */

#define CFLAG_Object               0
#define OFLAG_Object               MAGICFLAG(0)
#define TY_Object                  CLASS_Object

#define kObject_NullObject         ((kmagicflag_t)(1<<0))
#define kObject_isNullObject(o)    (TFLAG_is(kmagicflag_t,(o)->h.magicflag,kObject_NullObject))
#define kObject_setNullObject(o,b) TFLAG_set(kmagicflag_t,(o)->h.magicflag,kObject_NullObject,b)
#define IS_NULL(o)                 ((((o)->h.magicflag) & kObject_NullObject) == kObject_NullObject)
#define IS_NOTNULL(o)              ((((o)->h.magicflag) & kObject_NullObject) != kObject_NullObject)

#define K_FASTMALLOC_SIZE  (sizeof(void*) * 8)

#define K_OBJECT_MAGIC           (578L << ((sizeof(kflag_t)*8)))
#define K_CFLAGMASK              (FLAG_Object_Ref)
#define KNH_MAGICFLAG(f)         (K_OBJECT_MAGIC | ((kmagicflag_t)(f) & K_CFLAGMASK))
#define DBG_ASSERT_ISOBJECT(o)   DBG_ASSERT(TFLAG_is(uintptr_t,(o)->h.magicflag, K_OBJECT_MAGIC))

typedef struct kObjectHeader {
	kmagicflag_t magicflag;
	const struct kclass_t *ct;  //@RENAME
	union {
		uintptr_t refc;  // RCGC
		void *gcinfo;
	};
	struct kpromap_t *proto;
} kObjectHeader ;

typedef struct kObject kObject;
//#if defined(K_INTERNAL) || defined(USE_STRUCT_Object)
struct kObject {
	kObjectHeader h;
	union {
		struct kObject *fields[4];
		uintptr_t       ndata[4];
	};
};
//#endif

typedef struct kprodata_t {
	ksymbol_t key;
	ktype_t   type;
	union {
		uintptr_t val;
		kObject *oval;
	};
} kprodata_t;

typedef struct kpromap_t {
	kprodata_t *datamap;
	size_t size;
	size_t capacity;
} kpromap_t;

extern void kpromap_each(CTX, kpromap_t *p, void *arg, void (*f)(CTX, void *, kprodata_t *d));
#define OBJECT_MASK KFLAG_H0
#define O_cid(o)            (((o)->h.ct)->cid)
#define O_bcid(o)           (((o)->h.ct)->bcid)
#define O_ct(o)             ((o)->h.ct)
#define O_unbox(o)          ((o)->h.ct->unbox(_ctx, o))
#define O_p1(o)             (((o)->h.ct)->p1)
#define O_p2(o)             (((o)->h.ct)->p2)
//#define IS_ObjectField(o)   (((o)->h.ct)->bcid == CLASS_Object)

/* ------------------------------------------------------------------------ */
//## @Immutable class Boolean Object;

#define CFLAG_Boolean              kClass_Immutable|kClass_UnboxType|kClass_Final
#define OFLAG_Boolean              MAGICFLAG(0)
#define TY_Boolean                 CLASS_Boolean
#define IS_Boolean(o)              (O_cid(o) == CLASS_Boolean)

typedef union kunbox_union {
	uintptr_t   data;
	kbool_t    bvalue;
	kint_t     ivalue;
	kfloat_t   fvalue;
} kunbox_union;

typedef struct kBoolean kBoolean;
struct kBoolean {
	kObjectHeader h;
	kunbox_union  n;
};

#define IS_TRUE(o)             (O_bcid(o) == CLASS_Boolean && N_tobool(o))
#define IS_FALSE(o)            (O_bcid(o) == CLASS_Boolean && N_tobool(o) == 0)
#define new_Boolean(_ctx, c)    ((c) ? K_TRUE : K_FALSE)
//#define O_ndata(o)             (((kBoolean*)o)->n.data)
#define N_toint(o)             (((kBoolean*)o)->n.ivalue)
#define N_tofloat(o)           (((kBoolean*)o)->n.fvalue)
#define N_tobool(o)            (((kBoolean*)o)->n.bvalue)

/* ------------------------------------------------------------------------ */
//## @Immutable class Int Number;

#define CFLAG_Int              kClass_Immutable|kClass_UnboxType|kClass_Final
#define OFLAG_Int              MAGICFLAG(0)
#define TY_Int                 CLASS_Int
#define IS_Int(o)              (O_cid(o) == CLASS_Int)

typedef struct kInt kInt;
struct kInt {
	kObjectHeader h;
	kunbox_union  n;
};

/* ------------------------------------------------------------------------ */
/* String */

#define CFLAG_String              kClass_Immutable|kClass_Final
#define OFLAG_String              MAGICFLAG(0)
#define TY_String                 CLASS_String
#define IS_String(o)              (O_cid(o) == CLASS_String)
/*
 * Bit encoding for Rope String
 * 5432109876543210
 * 000xxxxxxxxxxxxx ==> magicflag bit representation
 * 001xxxxxxxxxxxxx LinerString
 * 011xxxxxxxxxxxxx ExterenalString
 * 010xxxxxxxxxxxxx InlinedString
 * 100xxxxxxxxxxxxx RopeString
 */
#define S_FLAG_MASK_BASE (13)
#define S_FLAG_LINER     ((1UL << (0)))
#define S_FLAG_NOFREE    ((1UL << (1)))
#define S_FLAG_ROPE      ((1UL << (2)))
#define S_FLAG_INLINE    (S_FLAG_NOFREE)
#define S_FLAG_EXTERNAL  (S_FLAG_LINER | S_FLAG_NOFREE)

#define S_isRope(o)          (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local1))
#define S_isTextSgm(o)       (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local2))
#define S_setTextSgm(o,b)    TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local2,b)
#define S_isMallocText(o)    (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local3))
#define S_setMallocText(o,b) TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local3,b)
#define S_isASCII(o)         (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local4))
#define S_setASCII(o,b)      TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local4,b)
#define S_isPooled(o)        (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local5))
#define S_setPooled(o,b)     TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local5,b)
#define SIZE_INLINE_TEXT (64 - sizeof(kObjectHeader)-sizeof(kbytes_t)-sizeof(kuint_t))

typedef struct {
	size_t       len;
	union {
		const char *text;
		const unsigned char *utext;
		char *buf;
		unsigned char *ubuf;
	};
} kbytes_t;

typedef struct kString kString;
struct kString {
	kObjectHeader h;
	kbytes_t str;
	kuint_t hashCode;
	const char _reserved[SIZE_INLINE_TEXT];
};

#define SPOL_TEXT          (1<<0)
#define SPOL_ASCII         (1<<1)
#define SPOL_UTF8          (1<<2)
#define SPOL_POOL          (1<<3)

#define new_T(t)            new_kString(t, knh_strlen(t), SPOL_TEXT|SPOL_ASCII|SPOL_POOL)
#define new_S(T, L)         new_kString(T, L, SPOL_ASCII|SPOL_POOL)
#define S_text(s)             ((const char*) (O_ct(s)->unbox(_ctx, (kObject*)s)))
#define S_size(s)             ((s)->str.len)

//#define S_equals(s, b)        knh_bytes_equals(S_tobytes(s), b)
//#define S_startsWith(s, b)    knh_bytes_startsWith_(S_tobytes(s), b)
//#define S_endsWith(s, b)      knh_bytes_endsWith_(S_tobytes(s), b)

/* ------------------------------------------------------------------------ */
//## class Array   Object;

#define CFLAG_Array              kClass_Final
#define OFLAG_Array              MAGICFLAG(0)
#define TY_Array                 CLASS_Array
#define IS_Array(o)              (O_cid(o) == CLASS_Array)

#define kArray_isUnboxData(o)    (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local1))
#define kArray_setUnboxData(o,b) TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local1,b)

typedef struct kArray kArray;

struct kArray {
	kObjectHeader h;
	union {
		intptr_t               *nlist;
		kint_t                 *ilist;
#ifdef K_USING_FLOAT
		kfloat_t               *flist;
#endif
		struct kObject        **list;
		struct kRawPtr        **ptrs;
		struct kString        **strings;
		struct kInt           **ints;
		struct kMethod        **methods;
		struct kToken         **tts;
		struct kExpr          **exprs;
		struct kStmt          **stmts;
	};
	size_t size;
	size_t capacity;
};

/* ------------------------------------------------------------------------ */
//## @Private @Immutable class Param Object;
//## flag Param VARGs  1 - is set * *;
//## flag Param RVAR   2 - is set * *;

#define CFLAG_Param              kClass_Final
#define OFLAG_Param              MAGICFLAG(0)
#define TY_Param                 CLASS_Param
#define IS_Param(o)              (O_cid(o) == CLASS_Param)

typedef struct kparam_t {
	ktype_t    ty;  ksymbol_t  fn;
} kparam_t;

typedef struct kParam kParam;
struct kParam {
	kObjectHeader h;
	ktype_t rtype; kushort_t psize;
	kparam_t p[3];
};

/* ------------------------------------------------------------------------ */
//## class Method Object;

#define CFLAG_Method              kClass_Final
#define OFLAG_Method              MAGICFLAG(0)
#define TY_Method                 CLASS_Method
#define IS_Method(o)              (O_cid(o) == CLASS_Method)

typedef struct kMethod kMethod;

#define kMethod_Public               ((uintptr_t)(1<<0))
#define kMethod_Virtual              ((uintptr_t)(1<<1))
#define kMethod_Hidden               ((uintptr_t)(1<<2))
#define kMethod_Const                ((uintptr_t)(1<<3))
#define kMethod_Static               ((uintptr_t)(1<<4))
#define kMethod_Immutable            ((uintptr_t)(1<<5))
#define kMethod_Restricted           ((uintptr_t)(1<<6))
#define kMethod_Overloaded           ((uintptr_t)(1<<7))
#define kMethod_CALLCC               ((uintptr_t)(1<<8))
#define kMethod_FASTCALL             ((uintptr_t)(1<<9))
#define kMethod_D                    ((uintptr_t)(1<<10))
#define kMethod_Abstract             ((uintptr_t)(1<<11))
#define kMethod_Coercion             ((uintptr_t)(1<<12))

#define kMethod_isPublic(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Public))
//#define kMethod_setPublic(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Public,B)
#define kMethod_isVirtual(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Virtual))
//#define kMethod_setVirtual(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Virtual,B)
#define kMethod_isHidden(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Hidden))
#define kMethod_setHidden(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Hidden,B)
#define kMethod_isStatic(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Static))
#define kMethod_setStatic(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Static,B)
#define kMethod_isConst(o)      (TFLAG_is(uintptr_t, (o)->flag,kMethod_Const))
//#define kMethod_setConst(o,B)   TFLAG_set(uintptr_t, (o)->flag,kMethod_Const,B)
#define kMethod_isVirtual(o)     (TFLAG_is(uintptr_t, (o)->flag,kMethod_Virtual))
#define kMethod_setVirtual(o,B)  TFLAG_set(uintptr_t, (o)->flag,kMethod_Virtual,B)

#define kMethod_isTransCast(mtd)    MN_isTOCID(mtd->mn)
#define kMethod_isCast(mtd)         MN_isASCID(mtd->mn)
#define kMethod_isCoercion(mtd)    (TFLAG_is(uintptr_t, (mtd)->flag,kMethod_Coercion))

//#define kMethod_isOverload(o)  (TFLAG_is(uintptr_t,DP(o)->flag,kMethod_Overload))
//#define kMethod_setOverload(o,b) TFLAG_set(uintptr_t,DP(o)->flag,kMethod_Overload,b)

/* method data */
#define DEND     (-1)

#if 1
#define _RIX         ,long _rix
#define K_RIX        _rix
#define K_RIXPARAM    ,K_RTNIDX
#define _KFASTCALL   ,long _rix
#define K_FASTRIX    _rix
#else
#define _RIX
#define K_RIX (-4)
#define K_RIXPARAM
#define _KFASTCALL
#define K_FASTRIX K_RIXPARAM
#endif

#ifdef K_USING_WIN32_
//#define KMETHOD  void CC_EXPORT
//#define ITRNEXT int   CC_EXPORT
//typedef void (CC_EXPORT *knh_Fmethod)(CTX, ksfp_t* _RIX);
//typedef int  (CC_EXPORT *knh_Fitrnext)(CTX, ksfp_t * _RIX);
#else
#define KMETHOD    void  /*CC_FASTCALL_*/
#define KMETHODCC  int  /*CC_FASTCALL_*/
typedef KMETHOD   (*knh_Fmethod)(CTX, ksfp_t* _RIX);
typedef KMETHOD   (*FmethodFastCall)(CTX, ksfp_t * _KFASTCALL);
typedef KMETHODCC (*FmethodCallCC)(CTX, ksfp_t *, int, int, struct kopl_t*);
#endif

struct kMethod {
	kObjectHeader     h;
	uintptr_t         flag;
	kcid_t            cid;   kmethodn_t  mn;
	struct kParam     *pa;
	kshort_t delta;          kpkg_t packid;
	union {
		struct kToken *tcode;
	};
	union {
		kObject            *objdata;
		struct kKonohaCode *kcode;
		struct kKonohaSpace      *lazyns;       // lazy compilation
		struct kMethod     *proceed;      // during typing, asm
	};
	union {
		knh_Fmethod       fcall_1;
		FmethodFastCall   fastcall_1;
	};
	union {/* body*/
		struct kopl_t    *pc_start;
		FmethodCallCC     callcc_1;
	};
};

/* ------------------------------------------------------------------------ */
//## @Singleton class System Object;

#define CFLAG_System              kClass_Singleton|kClass_Final
#define OFLAG_System              MAGICFLAG(0)
#define TY_System                 CLASS_System
#define IS_System(o)              (O_cid(o) == CLASS_System)

typedef struct kSystem kSystem;

struct kSystem {
	kObjectHeader h;
};

/* ------------------------------------------------------------------------ */
//## class Tdynamic Object;

#define CFLAG_Tdynamic              kClass_Final
#define OFLAG_Tdynamic              MAGICFLAG(0)
#define TY_dynamic                  CLASS_Tdynamic
#define CLASS_RawPtr                CLASS_Tdynamic
#define T_RawPtr                    CLASS_Tdynamic

typedef struct kRawPtr {
	kObjectHeader h;
	void         *rawptr;
	kArray       *gcbuf;
	void         (*rawfree)(void *);
} kRawPtr ;

//#define new_ReturnRawPtr(_ctx, sfp, p)  new_ReturnCppObject(_ctx, sfp, p, NULL)

#define K_CALLDELTA   4
#define K_RTNIDX    (-4)
#define K_SHIFTIDX  (-3)
#define K_PCIDX     (-2)
#define K_MTDIDX    (-1)
#define K_TMRIDX    (0)
#define K_SELFIDX   0

//#define K_NEXTIDX    2
#define K_ULINEIDX2  (-7)
#define K_SHIFTIDX2  (-5)
#define K_PCIDX2     (-3)
#define K_MTDIDX2    (-1)

#define klr_setesp(_ctx, newesp)  ((kcontext_t*)_ctx)->esp = (newesp)
#define klr_setmtdNC(sfpA, mtdO)   sfpA.mtdNC = mtdO

//#define Method_isKonohaCode(mtd) ((mtd)->fcall_1 == Fmethod_runVM)
#define Method_isKonohaCode(mtd) (0)

#define BEGIN_LOCAL(V,N) \
	ksfp_t *V = _ctx->esp, *esp_ = _ctx->esp; (void)V;\

#define END_LOCAL() ((kcontext_t*)_ctx)->esp = esp_;

#define KCALL(LSFP, RIX, MTD, ARGC) { \
		ksfp_t *tsfp = LSFP + RIX + K_CALLDELTA;\
		tsfp[K_MTDIDX].mtdNC = MTD;\
		tsfp[K_PCIDX].fname = __FILE__;\
		tsfp[K_SHIFTIDX].shift = 0;\
		tsfp[K_RTNIDX].uline = __LINE__;\
		klr_setesp(_ctx, tsfp + ARGC + 1);\
		(MTD)->fastcall_1(_ctx, tsfp K_RIXPARAM);\
		tsfp[K_MTDIDX].mtdNC = NULL;\
	} \


/* ----------------------------------------------------------------------- */
// klib2

struct kKonohaSpace;
struct ksyntaxdef_t;

typedef struct klib2_t {
	void* (*Kmalloc)(CTX, size_t);
	void* (*Kzmalloc)(CTX, size_t);
	void  (*Kfree)(CTX, void *, size_t);

	void  (*Karray_init)(CTX, karray_t *, size_t, size_t);
	void  (*Karray_expand)(CTX, karray_t *, size_t, size_t);
	void  (*Karray_free)(CTX, karray_t *, size_t);

	void (*Kwb_init)(karray_t *, kwb_t *);
	void (*Kwb_write)(CTX, kwb_t *, const char *, size_t);
	void (*Kwb_putc)(CTX, kwb_t *, ...);
	void (*Kwb_vprintf)(CTX, kwb_t *, const char *fmt, va_list ap);
	void (*Kwb_printf)(CTX, kwb_t *, const char *fmt, ...);
	const char* (*Kwb_top)(CTX, kwb_t *, int);
	void (*Kwb_free)(kwb_t *);

	kmap_t*  (*Kmap_init)(CTX, size_t);
	kmape_t* (*Kmap_newentry)(CTX, kmap_t *, uintptr_t);
	kmape_t* (*Kmap_get)(kmap_t *, uintptr_t);
	void (*Kmap_add)(kmap_t *, kmape_t *);
	void (*Kmap_remove)(kmap_t *, kmape_t *);
	void (*Kmap_reftrace)(CTX, kmap_t *, void (*)(CTX, kmape_t*));
	void (*Kmap_free)(CTX, kmap_t *, void (*)(CTX, void *));

	const kclass_t*  (*Kclass)(CTX, kcid_t, kline_t);
	ksymbol_t (*Ksymbol)(CTX, const char *, size_t, ksymbol_t def, int);
	const char* (*KTsymbol)(CTX, char *, size_t, ksymbol_t mn);
	kuri_t    (*Kuri)(CTX, const char *, size_t);

	struct kObject* (*Knew_Object)(CTX, const kclass_t *, void *);
	struct kObject* (*Knull)(CTX, const kclass_t *);
	kObject* (*KObject_getObjectNULL)(CTX, kObject *, ksymbol_t);
	void (*KObject_setObject)(CTX, kObject *, ksymbol_t, ktype_t, kObject *);

	struct kString* (*Knew_String)(CTX, const char *, size_t, int);

	void (*KArray_add)(CTX, kArray *, kObject *);
	void (*KArray_insert)(CTX, kArray *, size_t, kObject *);
	void (*KArray_clear)(CTX, kArray *, size_t);

	struct kParam * (*Knew_Param)(CTX, ktype_t, int, kparam_t *);
	struct kMethod * (*Knew_Method)(CTX, uintptr_t, kcid_t, kmethodn_t, struct kParam*, knh_Fmethod);
	void (*KMethod_setFunc)(CTX, kMethod*, knh_Fmethod);

	kbool_t (*KsetModule)(CTX, int, struct kmodshare_t *, kline_t);
	const kclass_t* (*KaddClassDef)(CTX, kString *, KCLASSDEF *, kline_t);

	kcid_t  (*KKonohaSpace_getcid)(CTX, struct kKonohaSpace *, const char *, size_t, kcid_t def);
	void    (*KloadMethodData)(CTX, struct kKonohaSpace *, intptr_t *d);

	void  (*KCodeGen)(CTX, struct kMethod *, struct kBlock *);
	void (*Kreport)(CTX, int level, const char *msg);
	void (*Kreportf)(CTX, int level, kline_t, const char *fmt, ...);
	void (*Kraise)(CTX, int isContinue);     // module

	void (*Kp)(const char *file, const char *func, int line, const char *fmt, ...) __PRINT_FMT(4, 5);

	void (*KKonohaSpace_defineSyntax)(CTX, struct kKonohaSpace*, struct ksyntaxdef_t *);

} klib2_t;

#define K_NULL            (_ctx->share->constNull)
#define K_TRUE            (_ctx->share->constTrue)
#define K_FALSE           (_ctx->share->constFalse)
#define K_NULLPARAM       (_ctx->share->nullParam)
#define K_EMPTYARRAY      (_ctx->share->emptyArray)
#define TS_EMPTY          (_ctx->share->emptyString)

#define UPCAST(o)         ((kObject*)o)

#define KPI                     (_ctx->lib2)

#define KNH_MALLOC(size)       (KPI)->Kmalloc(_ctx, size)
#define KNH_ZMALLOC(size)      (KPI)->Kzmalloc(_ctx, size)
#define KNH_FREE(p, size)      (KPI)->Kfree(_ctx, p, size)

#define KARRAY_INIT(VAR, init, STRUCT)      (KPI)->Karray_init(_ctx, (karray_t*)&(VAR), init, sizeof(STRUCT))
#define KARRAY_EXPAND(VAR, min, STRUCT)     (KPI)->Karray_expand(_ctx, (karray_t*)&(VAR), min, sizeof(STRUCT))
#define KARRAY_FREE(VAR, STRUCT)            (KPI)->Karray_free(_ctx, (karray_t*)&(VAR), sizeof(STRUCT))


#define kwb_init(M,W)            (KPI)->Kwb_init(M,W)
#define kwb_write(W,B,S)         (KPI)->Kwb_write(_ctx,W,B,S)
#define kwb_putc(W,...)          (KPI)->Kwb_putc(_ctx,W, ## __VA_ARGS__, EOF)
#define kwb_vprintf(W,FMT,ARG)   (KPI)->Kwb_vprintf(_ctx,W, FMT, ARG)
#define kwb_printf(W,FMT,...)    (KPI)->Kwb_printf(_ctx,W, FMT, ## __VA_ARGS__)

#define kwb_top(W,IS)            (KPI)->Kwb_top(_ctx,W, IS)
#define kwb_size(W)              ((W)->w->size - (W)->pos)
#define kwb_free(W)              (KPI)->Kwb_free(W)

#define kmap_init(INIT)           (KPI)->Kmap_init(_ctx, INIT)
#define kmap_newentry(M, H)       (KPI)->Kmap_newentry(_ctx, M, H)
#define kmap_get(M, K)            (KPI)->Kmap_get(M, K)
#define kmap_add(M, E)            (KPI)->Kmap_add(M, E)
#define kmap_remove(M, E)         (KPI)->Kmap_remove(_ctx, M, E)
#define kmap_reftrace(M, F)       (KPI)->Kmap_reftrace(_ctx, M, F)
#define kmap_free(M, F)           (KPI)->Kmap_free(_ctx, M, F)

#define kclass(CID, UL)           (KPI)->Kclass(_ctx, CID, UL)
#define SYMPOL_RAW                0
#define SYMPOL_NAME               1
#define SYMPOL_METHOD             2
#define ksymbol(T,L,D,P)          (KPI)->Ksymbol(_ctx, T, L, D, P)
#define KSYMBOL(T)                (KPI)->Ksymbol(_ctx, T, sizeof(T)-1, FN_NEWID, SYMPOL_RAW)
#define FN_(T)                    ksymbol(T, (sizeof(T)-1), FN_NEWID, SYMPOL_NAME)
#define MN_(T)                    ksymbol(T, (sizeof(T)-1), FN_NEWID, SYMPOL_METHOD)
#define T_mn(B, X)                (KPI)->KTsymbol(_ctx, B, sizeof(B), X)


#define kuri(T,L)                 (KPI)->Kuri(_ctx, T, L)
#define KURI(T)                   (KPI)->Kuri(_ctx, T, sizeof(T)-1)
#define kpkg(T,L)                 (KPI)->Kpkg(_ctx, T, L)

#define new_kObject(C, A)         (KPI)->Knew_Object(_ctx, C, (void*)(A))
#define new_(C, A)                (k##C*)(KPI)->Knew_Object(_ctx, CT_##C, (void*)(A))
#define knull(C)                  (KPI)->Knull(_ctx, C)
#define KNULL(C)                  (k##C*)(KPI)->Knull(_ctx, CT_##C)

#define kObject_getObjectNULL(O, K)      (KPI)->KObject_getObjectNULL(_ctx, UPCAST(O), K)
#define kObject_setObject(O, K, V)       (KPI)->KObject_setObject(_ctx, UPCAST(O), K, O_cid(V), UPCAST(V))

#define new_kString(T,S,P)        (KPI)->Knew_String(_ctx, T, S, P)

#define kArray_size(A)            (A)->size
#define kArray_add(A, V)          (KPI)->KArray_add(_ctx, A, UPCAST(V))
#define kArray_insert(A, N, V)    (KPI)->KArray_insert(_ctx, A, N, UPCAST(V))
#define kArray_clear(A, S)        (KPI)->KArray_clear(_ctx, A, S)

#define new_kParam(R,S,P)        (KPI)->Knew_Param(_ctx, R, S, P)
#define new_kMethod(F,C,M,P,FF)  (KPI)->Knew_Method(_ctx, F, C, M, P, FF)
#define kMethod_setFunc(M,F)     (KPI)->KMethod_setFunc(_ctx, M, F)

#define KCLASS(cid)              S_text(CT(cid)->name)
#define kKonohaSpace_getcid(NS, S, L, C)  (KPI)->KKonohaSpace_getcid(_ctx, NS, S, L, C)
#define ksetModule(N,D,P)        (KPI)->KsetModule(_ctx, N, D, P)
#define kaddClassDef(NAME, DEF, UL)    (KPI)->KaddClassDef(_ctx, NAME, DEF, UL)
#define kloadMethodData(NS, DEF)   (KPI)->KloadMethodData(_ctx, NS, DEF)

#define CRIT_  0
#define ERR_   1
#define WARN_  2
#define INFO_  3
#define PRINT_ 4
#define DEBUG_ 5

#define kreport(LEVEL, MSG)            (KPI)->Kreport(_ctx, LEVEL, MSG)
#define kreportf(LEVEL, UL, fmt, ...)  (KPI)->Kreportf(_ctx, LEVEL, UL, fmt, ## __VA_ARGS__)
#define kraise(isContinue)             (KPI)->Kraise(_ctx, isContinue)
// gc

#define KTODO(MSG) do { fprintf(stderr, "TODO: %s\n", MSG);abort(); } while (0)

#if defined(_MSC_VER)
#define OBJECT_SET(var, val) do {\
	kObject **var_ = (kObject**)&val; \
	var_[0] = (val_); \
} while (0)
#else
#define OBJECT_SET(var, val) var = (typeof(var))(val)
#endif /* defined(_MSC_VER) */


#define INIT_GCSTACK()         size_t gcstack_ = kArray_size(_ctx->stack->gcstack)
#define PUSH_GCSTACK(o)        kArray_add(_ctx->stack->gcstack, o)
#define RESET_GCSTACK()        kArray_clear(_ctx->stack->gcstack, gcstack_)

#define KINITv(VAR, VAL)   OBJECT_SET(VAR, VAL)
#define KSETv(VAR, VAL)    OBJECT_SET(VAR, VAL)
#define KINITp(parent, v, o) KINITv(v, o)
#define KSETp(parent,  v, o) KSETv(v, o)
#define KUNUSEv(V)  (V)->h.ct->free(_ctx, (V))

REF_t *kstack_tail(CTX, size_t min);
#define BEGIN_REFTRACE(SIZE)  int _ref_ = SIZE; REF_t* _tail = kstack_tail(_ctx, SIZE);
#define END_REFTRACE()        (void)_ref_; _ctx->stack->tail = _tail;

#define KREFTRACEv(p)  do {\
	DBG_ASSERT(p != NULL);\
	_tail[0] = (REF_t)p;\
	_tail++;\
} while (0)

#define KREFTRACEn(p) do {\
	if(p != NULL) {\
		_tail[0] = (REF_t)p;\
		_tail++;\
	}\
} while (0)

// method macro

#define KNH_SAFEPOINT(_ctx, sfp)

#define RETURN_(vv) do {\
	KSETv(sfp[K_RIX].o, vv);\
	KNH_SAFEPOINT(_ctx, sfp);\
	return; \
} while (0)

#define RETURNd_(d) do {\
	sfp[K_RIX].ndata = d; \
	return; \
} while (0)

#define RETURNb_(c) do {\
	sfp[K_RIX].bvalue = c; \
	return; \
} while(0)

#define RETURNi_(c) do {\
	sfp[K_RIX].ivalue = c; \
	return; \
} while (0)

#define RETURNf_(c) do {\
	sfp[K_RIX].fvalue = c; \
	return; \
} while (0)

#define RETURNvoid_() do {\
	(void)_rix;\
	return; \
} while (0)

#ifndef K_NODEBUG
#define KNH_ASSERT(a)    assert(a)
#define DBG_ASSERT(a)    assert(a)
#define DBG_P(fmt, ...)  _ctx->lib2->Kp(__FILE__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
#define DBG_ABORT(fmt, ...) _ctx->lib2->Kp(__FILE__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__); abort()
#define DUMP_P(fmt, ...)  fprintf(stderr, fmt, ## __VA_ARGS__)
#else
#define KNH_ASSERT(a)
#define DBG_ASSERT(a)
#define DBG_P(fmt, ...)
#define DBG_ABORT(fmt, ...)
#define DUMP_P(fmt, ...)
#endif

#ifndef unlikely
#define unlikely(x)   __builtin_expect(!!(x), 0)
#define likely(x)     __builtin_expect(!!(x), 1)

#endif /*unlikely*/

///* Konoha API */
extern void konoha_ginit(int argc, const char **argv);
extern konoha_t konoha_open(void);
extern void konoha_close(konoha_t konoha);
extern int konoha_main(konoha_t konoha, int argc, const char **argv);
extern void knh_beginContext(CTX, void **bottom);
extern void knh_endContext(CTX);
#define BEGIN_CONTEXT(_ctx) knh_beginContext(_ctx, (void**)&_ctx)
#define END_CONTEXT(_ctx) knh_endContext(_ctx)
extern void klib2_init(klib2_t *klib2);
extern void kshare_init(CTX, kcontext_t *ctx);
extern void MODEVAL_init(CTX, kcontext_t *ctx);
extern void kshare_reftrace(CTX, kcontext_t *ctx);
extern void kshare_free(CTX, kcontext_t *ctx);
extern void kpromap_free(CTX, struct kpromap_t *p);
extern void kpromap_reftrace(CTX, struct kpromap_t *p);
extern void kshare_init_methods(CTX);

/* for debug mode */
extern int konoha_debug;

#endif /* KONOHA2_H_ */
