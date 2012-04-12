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

#include"commons.h"

#ifdef __cplusplus 
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [tokenizer] */

/* error */
#define KC_ERR       LOG_ERR
#define KC_TERROR    LOG_ERR
/* warning */
#define KC_EWARN     LOG_WARNING
#define KC_DWARN     LOG_WARNING
/* info */
#define KC_BAD       LOG_NOTICE
#define KC_NOTICE    LOG_NOTICE
#define KC_TINFO     LOG_INFO
#define KC_INFO      LOG_INFO
#define KC_DEBUG     LOG_DEBUG

/* ------------------------------------------------------------------------ */
/* @data */

static const char* KC__(int p)
{
	switch(p) {
	case LOG_EMERG:
	case LOG_ALERT:
	case LOG_CRIT:
	case LOG_ERR:  return "(error) ";
	case LOG_WARNING: return "(warning) ";
	case LOG_NOTICE:  return "(bad manner) ";
	case LOG_INFO:    return "(info) ";
	case LOG_DEBUG:   return "(debug) ";
	}
	return "(debug) ";
}

#ifdef K_USING_POSIX_
#define CTX_isTERM(_ctx)  CTX_isInteractive(_ctx)
#else
#define CTX_isTERM(_ctx)  0
#endif


static kString *GammaBuilder_vperror(CTX, int pe, const char *fmt, va_list ap)
{
	kString *msg = TS_EMPTY;
	int isPRINT = (pe <= KC_DWARN) ? 1 : 0;
	if(pe != KC_DEBUG && (CTX_isInteractive(_ctx) || knh_isCompileOnly(_ctx))) {
		isPRINT = 1;
	}
	if(GammaBuilder_isQuiet(_ctx->gma) || ctxcode->uline == 0) {
		isPRINT = 0;
	}
	if(isPRINT == 1) {
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		knh_write_uline(_ctx, cwb->w, ctxcode->uline);
		knh_write_ascii(_ctx, cwb->w, KC__(pe));
		knh_vprintf(_ctx, cwb->w, fmt, ap);
		msg = CWB_newString(_ctx, cwb, SPOL_POOLNEVER);
		kArray_add(DP(_ctx->gma)->errmsgs, msg);
		fprintf(stderr, "%s - %s%s\n", TERM_BNOTE(_ctx, pe), S_text(msg), TERM_ENOTE(_ctx, pe));
		knh_logprintf("konoha", 0, S_text(msg));
	}
	return msg;
}

static kTerm *GammaBuilder_perror(CTX, int pe, const char *fmt, ...)
{
	kString *msg;
	va_list ap;
	va_start(ap, fmt);
	msg = GammaBuilder_vperror(_ctx, pe, fmt, ap);
	va_end(ap);
	if(pe < KC_DWARN) {
		kTerm *tkERR = new_(Term);
		tkERR->tt = TT_ERR;
		tkERR->uline = (_ctx->gma)->uline;
		KSETv((tkERR)->data, msg);
		return tkERR;
	}
	return NULL;
}

static kTerm *kTermoERR(CTX, kTerm *tk, const char *fmt, ...)
{
	if(TT_(tk) != TT_ERR) {
		kString *msg;
		va_list ap;
		va_start(ap, fmt);
		ctxcode->uline = tk->uline;
		msg = GammaBuilder_vperror(_ctx, KC_ERR, fmt, ap);
		va_end(ap);
		TT_(tk) = TT_ERR;
		KSETv((tk)->data, msg);
	}
	return tk;
}

/* ------------------------------------------------------------------------ */
/* script */

kTerm* ERROR_NotFound(CTX, const char *whatis, const char *t)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("%s not found: %s"), whatis, t);
}
void WARN_NotFound(CTX, const char *whatis, const char *t)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("%s not found: %s"), whatis, t);
}
kTerm* ERROR_Incompatible(CTX, const char *whatis, const char *name)
{
	return GammaBuilder_perror(_ctx, KC_ERR, "incompatible %s: %s", whatis, name);
}
kTerm* ERROR_SingleParam(CTX)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("syntax error: always takes only one parameter"));
}
//kTerm* ErrorExtendingFinalClass(CTX, kcid_t cid)
//{
//	return GammaBuilder_perror(_ctx, KC_ERR, _("cannot extends final class %C"), cid);
//}
void WARN_MuchBetter(CTX, const char *token, const char *token2)
{
	if(token2 != NULL) {
		GammaBuilder_perror(_ctx, KC_DWARN, _("%s is better than %s"), token, token2);
	}
	else {
		GammaBuilder_perror(_ctx, KC_DWARN, _("%s is better"), token);
	}
}
void WarningMethodName(CTX, const char *name)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("%s should starts with lowercase"), name);
}

void WARN_Semicolon(CTX)
{
	if(!CTX_isInteractive(_ctx)) GammaBuilder_perror(_ctx, KC_BAD, "needs ");
}
void WARN_UnxpectedMultiByteChar(CTX, const char *ch)
{
	if(ch == NULL) ch = "???";
	GammaBuilder_perror(_ctx, KC_DWARN, _("unexpected multi-byte character: '%s'"), ch);
}
kTerm *ErrorHazardousStatement(CTX)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("hazardous statement"));
}
void WarningNoEffect(CTX)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("no effect"));
}
kTerm* ErrorMisplaced(CTX)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("misplaced"));
}
//kTerm* ErrorStaticType(CTX, const char *msg)
//{
//	kTerm* tkERR = GammaBuilder_perror(_ctx, KC_ERR, _("%s is not dynamic"), msg);
//	DBG_ABORT("stop why?");
//	return tkERR;
//}
kTerm* ERROR_UnableToAdd(CTX, kcid_t cid, const char *whatis)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("%T is unable to add new %s"), cid, whatis);
}
kTerm* ERROR_Block(CTX, const char* block)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("syntax error: wrong %s"), block);
}
kTerm* ERROR_WrongFFILink(CTX, const char* link)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("syntax error: ffi link: %s"), link);
}
kTerm* ERROR_text(CTX, const char *keyword K_TRACEARGV)
{
	kTerm *tk = GammaBuilder_perror(_ctx, KC_ERR, ("syntax error: %s"), keyword);
	KNH_HINT(_ctx, keyword);
	DBG_ABORT("why?");
	return tk;
}
kTerm* ERROR_Term(CTX, kTerm *tk K_TRACEARGV)
{
	if(TT_(tk) != TT_ERR) {
		tk = ERROR_text(_ctx, IS_String((tk)->text) ? S_text((tk)->text) : Term__(tk) K_TRACEDATA);
	}
	return tk;
}
kTerm* ERROR_Stmt(CTX, kStmtExpr *stmt K_TRACEARGV)
{
	DBG_ASSERT(STT_(stmt) != STT_ERR);
	return ERROR_text(_ctx, TT__(stmt->stt) K_TRACEDATA);
}
kTerm* ERROR_TermIsNot(CTX, kTerm *tk, const char* whatis)
{
	return GammaBuilder_perror(_ctx, KC_ERR, ("%O is not %s"), tk, whatis);
}
kTerm* ERROR_TermIs(CTX, kTerm *tk, const char* whatis)
{
	return GammaBuilder_perror(_ctx, KC_ERR, ("%O is not %s"), tk, whatis);
}
kTerm* ERROR_Required(CTX, kTerm *tk, const char *stmtexpr, const char *token)
{
	return kTermoERR(_ctx, tk, _("%s? needs %s"), stmtexpr, token);
}
void WarningNotInitialized(CTX, kTerm *tk, const char *tool)
{
	GammaBuilder_perror(_ctx, KC_EWARN, _("%s is not installed"), tool);
}
kTerm* ERROR_RegexCompilation(CTX, kTerm *tk, const char *regname, const char *regdata)
{
	return kTermoERR(_ctx, tk, _("%s compile error: %s"), regname, regdata);
}
kTerm* ERROR_Undefined(CTX, const char *whatis, kcid_t cid, kTerm *tk)
{
	if(cid != TY_unknown) {
		return kTermoERR(_ctx, tk, _("undefined %s: %T.%O"), whatis, cid, tk);
	}
	else {
		return kTermoERR(_ctx, tk, _("undefined %s: %O"), whatis, tk);
	}
}
kTerm* ERROR_UndefinedName(CTX, kTerm *tk)
{
	return kTermoERR(_ctx, tk, _("undefined name: %O"), tk);
}
void WARN_Undefined(CTX, const char *whatis, kcid_t cid, kTerm *tk)
{
	if(cid != TY_unknown) {
		GammaBuilder_perror(_ctx, KC_EWARN, _("undefined %s: %T.%O"), whatis, cid, tk);
	}
	else {
		GammaBuilder_perror(_ctx, KC_EWARN, _("undefined %s: %O"), whatis, tk);
	}
}
kTerm* ERROR_AlreadyDefined(CTX, const char *whatis, Object *o)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("already defined %s: %O"), whatis, o);
}
void WARN_AlreadyDefined(CTX, const char *whatis, Object *o)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("already defined %s: %O"), whatis, o);
}
void WARN_AlreadyDefinedClass(CTX, kcid_t cid, kcid_t oldcid)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("%C is already defined: %C"), cid, oldcid);
}
kTerm* ERROR_Denied(CTX, const char *why, kTerm *tk)
{
	return kTermoERR(_ctx, tk, _("%s: %O"), why, tk);
}
void WarningUnknownClass(CTX, kTerm *tk, kcid_t defc)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("unknown class: %L ==> %T"), tk, defc);
}
kTerm* ERROR_UnableToAssign(CTX, kTerm *tk)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("unable to make assignment"));
}
kTerm* ErrorUnsupportedConstructor(CTX, kcid_t mtd_cid)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("the constructor of %T is not supported"), mtd_cid);
}
kTerm* ERROR_UndefinedBehavior(CTX, const char *token)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("undefined behavior: %s"), token);
}
kTerm* ERROR_CompilerControlledParameter(CTX, kcid_t mtd_cid, kmethodn_t mn, int n)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("compiler controlled parameter: %C.%M(#%d)"), mtd_cid, mn, n);
}

kTerm* ERROR_RequiredParameter(CTX)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("needs a parameter to infer its type"));
}
void WARN_WrongTypeParam(CTX, kcid_t cid)
{
	if(cid != TY_unknown) {
		kbytes_t bname = C_bname(cid);
		GammaBuilder_perror(_ctx, KC_DWARN, "%B<>: wrong type parameter", bname);
	}
}
void INFO_Typing(CTX, const char *prefix, kbytes_t name, ktype_t type)
{
	GammaBuilder_perror(_ctx, KC_TINFO, "suppose %s%B has %T type", prefix, name, type);
}
void WARN_Overflow(CTX, const char *floatorint, kbytes_t t)
{
	GammaBuilder_perror(_ctx, KC_EWARN, _("%s overflow: %B"), floatorint, t);
}
void WARN_Unused(CTX, kTerm *tk, ksymbol_t fn)
{
	kline_t uline = ctxcode->uline;
	ctxcode->uline = tk->uline;
	GammaBuilder_perror(_ctx, KC_DWARN, _("unused %N"), fn);
	ctxcode->uline = uline;
}
kTerm* ERROR_AlreadyDefinedType(CTX, ksymbol_t fn, ktype_t type)
{
	return GammaBuilder_perror(_ctx, KC_TERROR, _("already defined: previous type %T %N"), type, fn);
}
kTerm* ErrorTooManyVariables(CTX)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("too many variables"));
}
//void WarningTooManyReturnValues(CTX)
//{
//	GammaBuilder_perror(_ctx, KC_DWARN, _("too many return values"));
//}
void WARN_UseDefaultValue(CTX, const char *whatis, ktype_t type)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("%s default value of %T"), whatis, type);
}
void WarningNoFmt(CTX, const char *fmt)
{
	GammaBuilder_perror(_ctx, KC_DWARN, "no such formatter: '%s'", fmt);
}
void WarningIllegalFormatting(CTX, const char *fmt)
{
	GammaBuilder_perror(_ctx, KC_DWARN, "illegal formatting at: `.. %s`", fmt);
}
kTerm* ERROR_MethodIsNot(CTX, kMethod *mtd, const char *how)
{
	if(IS_Method(mtd)) {
		return GammaBuilder_perror(_ctx, KC_ERR, _("%C.%M is not %s"), (mtd)->cid, (mtd)->mn, how);
	}
	else {
		return GammaBuilder_perror(_ctx, KC_ERR, _("method is not %s"), how);
	}
}
void WARN_MethodIs(CTX, kMethod *mtd, const char *how)
{
	if(IS_Method(mtd)) {
		GammaBuilder_perror(_ctx, KC_DWARN, _("%C.%M is %s"), (mtd)->cid, (mtd)->mn, how);
	}
	else {
		GammaBuilder_perror(_ctx, KC_DWARN, _("method is %s"), how);
	}
}
kTerm *ERROR_Unsupported(CTX, const char *whatis, kcid_t cid, const char *symbol)
{
	if(symbol == NULL) {
		return GammaBuilder_perror(_ctx, KC_ERR, "unsupported %s", whatis);
	}
	else if(cid == TY_unknown) {
		return GammaBuilder_perror(_ctx, KC_ERR, "unsupported %s: %s", whatis, symbol);
	}
	else {
		return GammaBuilder_perror(_ctx, KC_ERR, "unsupported %s: %s in %C", whatis, symbol, cid);
	}
}
void WARN_Unsupported(CTX, const char *msg)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("unsupported %s"), msg);
}
void WARN_Ignored(CTX, const char *whatis, kcid_t cid, const char *symbol)
{
	if(symbol == NULL) {
		GammaBuilder_perror(_ctx, KC_DWARN, "ignored %s", whatis, symbol);
	}
	else if(cid == TY_unknown) {
		GammaBuilder_perror(_ctx, KC_DWARN, "ignored %s: %s", whatis, symbol);
	}
	else {
		GammaBuilder_perror(_ctx, KC_DWARN, "ignored %s: %C.%s", whatis, cid, symbol);
	}
}
void WARN_Unnecesary(CTX, kTerm *tk)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("unnecessary %O"), tk);
}
void WarningUnnecessaryOperation(CTX, const char *msg)
{
	GammaBuilder_perror(_ctx, KC_DWARN, "unnecessary operation: %s", msg);
}
void WARN_TooMany(CTX, const char *whatis, const char *symbol)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("%s: too many %s, and be ignored."), symbol, whatis);
}
kTerm* ERROR_Needs(CTX, const char *whatis)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("%s is necessary"), whatis);
}
kTerm* ERROR_MustBe(CTX, const char *whatis, const char* token)
{
	if(token == NULL) {
		return GammaBuilder_perror(_ctx, KC_ERR, "must be %s", whatis);
	}
	else {
		return GammaBuilder_perror(_ctx, KC_ERR, "%s must be %s", token, whatis);
	}
}
kTerm* ERROR_OutOfIndex(CTX, kint_t s, kint_t n, kint_t e)
{
	return GammaBuilder_perror(_ctx, KC_ERR, "index must be %i <= %i < %i", s, n, e);
}
void WarningNullable(CTX, kcid_t cid)
{
	GammaBuilder_perror(_ctx, KC_DWARN, "%C doesn't take null", cid);
}
kTerm* ErrorComparedDiffrentType(CTX, ktype_t t1, ktype_t t2)
{
	return GammaBuilder_perror(_ctx, KC_TERROR, _("comparison of different type: %T %T"), t1, t2);
}
/* type error */
kTerm *TERROR_Term(CTX, kTerm *tk, kcid_t type, kcid_t reqt)
{
	return kTermoERR(_ctx, tk, ("%O has type %T, not %T"), tk, reqt, type);
}
kTerm *TypeErrorStmtNN(CTX, kStmtExpr *stmt, int n, ktype_t reqt, ktype_t type)
{
	return GammaBuilder_perror(_ctx, KC_TERROR, _("%s(%d) has type %T, not %T"), TT__(SP(stmt)->stt), n, reqt, type);
}
kTerm* TypeErrorCallParam(CTX, int n, kMethod *mtd, kcid_t reqt, kcid_t type)
{
	if(IS_Method(mtd)) {
		return GammaBuilder_perror(_ctx, KC_TERROR, _("%C.%M(#%d) has type %T, not %T"), (mtd)->cid, (mtd)->mn, n - 1, reqt, type);
	}
	else {
		KNH_ASSERT(IS_String(mtd));
		const char *fname = S_text((kString*)mtd);
		return GammaBuilder_perror(_ctx, KC_TERROR, _("%s(#d) has type %T, not %T"), fname, n - 1, reqt, type);
	}
}
void WARN_Cast(CTX, const char *whatis, kcid_t tcid, kcid_t scid)
{
	GammaBuilder_perror(_ctx, KC_EWARN, _("%s (%T)%T"), whatis, tcid, scid);
}
kTerm* ERROR_ForeachNotIterative(CTX, kcid_t p1, kcid_t type)
{
	if(p1 == CLASS_Tvar) {
		return GammaBuilder_perror(_ctx, KC_ERR, "foreach %T is not iterative", type);
	}
	else {
		return GammaBuilder_perror(_ctx, KC_ERR, "foreach: %T is not iteration of %T", p1, type);
	}
}
void WarningDuplicatedDefault(CTX)
{
	GammaBuilder_perror(_ctx, KC_EWARN, _("multiple default in switch"));
}
void WarningNotConstant(CTX)
{
GammaBuilder_perror(_ctx, KC_DWARN, _("case takes a constant value"));
}
void WarningAlwaysFalseAssertion(CTX)
{
GammaBuilder_perror(_ctx, KC_EWARN, _("always throw Assertion!!"));
}
void WarningDifferentMethodClass(CTX, kbytes_t name, kcid_t cid)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("different class: %B ==> %C"), name, cid);
}
void WarningDeprecated(CTX, const char *msg)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("depreciated %s"), msg);
}
kTerm* ErrorFinalMethod(CTX, kcid_t cid, kmethodn_t mn)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("%C.%M is final"), cid, mn);
}
kTerm* ErrorDifferentlyDefinedMethod(CTX, kcid_t mtd_cid, kmethodn_t mn)
{
	return GammaBuilder_perror(_ctx, KC_TERROR, _("%C.%M must be defined the same"), mtd_cid, mn);
}

/* ------------------------------------------------------------------------ */

kTerm* ERROR_OnlyTopLevel(CTX, const char* stmt)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("available only at the top level: %s"), stmt);
}
kTerm* ErrorUndefinedLabel(CTX, kTerm *tk)
{
	return GammaBuilder_perror(_ctx, KC_ERR, _("undefined label: %L"), tk);
}
void WarningAbstractMethod(CTX, kMethod *mtd)
{
	GammaBuilder_perror(_ctx, KC_DWARN, "abstract? %C.%M", (mtd)->cid, (mtd)->mn);
}
void WARN_DividedByZero(CTX)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("divided by zero"));
}

void WarningUndefinedFmt(CTX, kcid_t cid, kmethodn_t mn)
{
	GammaBuilder_perror(_ctx, KC_DWARN, _("undefined formatter: %M for %C"), mn, cid);
}

void NoticeInliningMethod(CTX, kMethod *mtd)
{
	GammaBuilder_perror(_ctx, KC_DEBUG, _("inlining: %C.%M"), (mtd)->cid, (mtd)->mn);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
