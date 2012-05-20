/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef PCRE_GLUE_H_
#define PCRE_GLUE_H_

#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#define MOD_REGEX 10/*FIXME*/

/* ------------------------------------------------------------------------ */
/* regex module */
//## @Immutable class Regex Object;
//## flag Regex GlobalOption  1 - is set * *;

#define Regex_isGlobalOption(o)     (TFLAG_is(uintptr_t,(o)->h.magicflag,kObject_Local1))
#define Regex_setGlobalOption(o,b)  TFLAG_set(uintptr_t,(o)->h.magicflag,kObject_Local1,b)
typedef void kregex_t;

/* REGEX_SPI */
#ifndef KREGEX_MATCHSIZE
#define KREGEX_MATCHSIZE    16
#endif

typedef struct {
	size_t       len;
	union {
		const char *text;
		const unsigned char *utext;
		char *buf;
		//kchar_t *ubuf;
	};
} kbytes_t;

typedef struct {
	int rm_so;   /* start of match */
	int rm_eo;   /* end of match */
	kbytes_t rm_name;  /* {NULL, 0}, if not NAMED */
} kregmatch_t;

typedef struct knh_RegexSPI_t {
	const char *name;
	kregex_t* (*regmalloc)(CTX, kString *);
	int (*parse_cflags)(CTX, const char *opt);
	int (*parse_eflags)(CTX, const char *opt);
	int (*regcomp)(CTX, kregex_t *, const char *, int);
	int (*regnmatchsize)(CTX, kregex_t *);
	int (*regexec)(CTX, kregex_t *, const char *, size_t, kregmatch_t*, int);
	size_t (*regerror)(int, kregex_t *, char *, size_t);
	void (*regfree)(CTX, kregex_t *);
	// this must be defined by uh for named grouping
	//int (*regexec2)(CTX, kregex_t *, const char *, ...);
} knh_RegexSPI_t;

/* ------------------------------------------------------------------------ */
/* [pcre] */

#if defined(HAVE_PCRE_H)
#include <pcre.h>
#else
struct real_pcre;
typedef struct real_pcre pcre;
typedef void pcre_extra;
/* Request types for pcre_fullinfo() */
#define PCRE_CASELESS           0x00000001  /* Compile */
#define PCRE_MULTILINE          0x00000002  /* Compile */
#define PCRE_DOTALL             0x00000004  /* Compile */
#define PCRE_EXTENDED           0x00000008  /* Compile */
#define PCRE_ANCHORED           0x00000010  /* Compile, exec, DFA exec */
#define PCRE_DOLLAR_ENDONLY     0x00000020  /* Compile */
#define PCRE_EXTRA              0x00000040  /* Compile */
#define PCRE_NOTBOL             0x00000080  /* Exec, DFA exec */
#define PCRE_NOTEOL             0x00000100  /* Exec, DFA exec */
#define PCRE_UNGREEDY           0x00000200  /* Compile */
#define PCRE_NOTEMPTY           0x00000400  /* Exec, DFA exec */
#define PCRE_UTF8               0x00000800  /* Compile */
#define PCRE_NO_AUTO_CAPTURE    0x00001000  /* Compile */
#define PCRE_NO_UTF8_CHECK      0x00002000  /* Compile, exec, DFA exec */
#define PCRE_AUTO_CALLOUT       0x00004000  /* Compile */
#define PCRE_PARTIAL_SOFT       0x00008000  /* Exec, DFA exec */
#define PCRE_PARTIAL            0x00008000  /* Backwards compatible synonym */
#define PCRE_DFA_SHORTEST       0x00010000  /* DFA exec */
#define PCRE_DFA_RESTART        0x00020000  /* DFA exec */
#define PCRE_FIRSTLINE          0x00040000  /* Compile */
#define PCRE_DUPNAMES           0x00080000  /* Compile */
#define PCRE_NEWLINE_CR         0x00100000  /* Compile, exec, DFA exec */
#define PCRE_NEWLINE_LF         0x00200000  /* Compile, exec, DFA exec */
#define PCRE_NEWLINE_CRLF       0x00300000  /* Compile, exec, DFA exec */
#define PCRE_NEWLINE_ANY        0x00400000  /* Compile, exec, DFA exec */
#define PCRE_NEWLINE_ANYCRLF    0x00500000  /* Compile, exec, DFA exec */
#define PCRE_BSR_ANYCRLF        0x00800000  /* Compile, exec, DFA exec */
#define PCRE_BSR_UNICODE        0x01000000  /* Compile, exec, DFA exec */
#define PCRE_JAVASCRIPT_COMPAT  0x02000000  /* Compile */
#define PCRE_NO_START_OPTIMIZE  0x04000000  /* Compile, exec, DFA exec */
#define PCRE_NO_START_OPTIMISE  0x04000000  /* Synonym */
#define PCRE_PARTIAL_HARD       0x08000000  /* Exec, DFA exec */
#define PCRE_NOTEMPTY_ATSTART   0x10000000  /* Exec, DFA exec */
#define PCRE_UCP                0x20000000  /* Compile */

#define PCRE_INFO_OPTIONS            0
#define PCRE_INFO_SIZE               1
#define PCRE_INFO_CAPTURECOUNT       2
#define PCRE_INFO_BACKREFMAX         3
#define PCRE_INFO_FIRSTBYTE          4
#define PCRE_INFO_FIRSTCHAR          4  /* For backwards compatibility */
#define PCRE_INFO_FIRSTTABLE         5
#define PCRE_INFO_LASTLITERAL        6
#define PCRE_INFO_NAMEENTRYSIZE      7
#define PCRE_INFO_NAMECOUNT          8
#define PCRE_INFO_NAMETABLE          9
#define PCRE_INFO_STUDYSIZE         10
#define PCRE_INFO_DEFAULT_TABLES    11
#define PCRE_INFO_OKPARTIAL         12
#define PCRE_INFO_JCHANGED          13
#define PCRE_INFO_HASCRORLF         14
#define PCRE_INFO_MINLENGTH         15

/* Request types for pcre_config(). Do not re-arrange, in order to remain
 * compatible. */

#define PCRE_CONFIG_UTF8                    0
#define PCRE_CONFIG_NEWLINE                 1
#define PCRE_CONFIG_LINK_SIZE               2
#define PCRE_CONFIG_POSIX_MALLOC_THRESHOLD  3
#define PCRE_CONFIG_MATCH_LIMIT             4
#define PCRE_CONFIG_STACKRECURSE            5
#define PCRE_CONFIG_UNICODE_PROPERTIES      6
#define PCRE_CONFIG_MATCH_LIMIT_RECURSION   7
#define PCRE_CONFIG_BSR                     8
#endif /* defined(HAVE_PCRE_H) */

/* ------------------------------------------------------------------------ */
#define kregexmod        ((kregexmod_t*)_ctx->mod[MOD_REGEX])
#define kregexshare      ((kregexshare_t*)_ctx->modshare[MOD_REGEX])
#define CT_Regex         kregexshare->cRegex
#define TY_Regex         kregexshare->cRegex->cid

#define IS_Regex(O)      ((O)->h.ct == CT_Regex)

typedef struct {
	kmodshare_t h;
	kclass_t *cRegex;
} kregexshare_t;

typedef struct {
	kmodlocal_t h;
} kregexmod_t;

typedef struct kRegex kRegex;
struct kRegex {
	kObjectHeader h;
	kregex_t *reg;
	int eflags;      // regex flag
	const knh_RegexSPI_t *spi;
	kString *pattern;
};

#endif

