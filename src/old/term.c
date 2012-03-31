/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved..
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

/* ************************************************************************ */

#include"commons.h"

#define TT_SIZE TT_LOR   /* |a| */


/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

void kerror(CTX, int level, kline_t uline, int lpos, const char *fmt, ...);

/* ------------------------------------------------------------------------ */
/* [term] */

kTerm* new_Term(CTX, kterm_t tt)
{
	kTerm *tk = new_(Term);
	SP(tk)->tt = tt;
	tk->uline = kcodemod->uline;
	return tk;
}

kStmtExpr* new_Stmt2(CTX, kterm_t stt, ...)
{
	kStmtExpr *stmt = new_(StmtExpr);
	kTerm *tm = NULL;
	va_list ap;
	DBG_ASSERT(stt < TT_PRAGMA);
	stmt->stt = stt;
	stmt->uline = kcodemod->uline;
	va_start(ap , stt);
	while((tm = (kTerm*)va_arg(ap, kTerm*)) != NULL) {
		DBG_ASSERT(IS_Term(tm) || IS_StmtExpr(tm));
		knh_Stmt_add_(_ctx, stmt, tm, NULL);
	}
	va_end(ap);
	return stmt;
}

void knh_Stmt_swap(CTX, kStmtExpr *stmt, size_t n, size_t m)
{
	DBG_ASSERT(n < DP(stmt)->size);
	DBG_ASSERT(m < DP(stmt)->size);
	kTerm *tm = tmNN(stmt, n);
	tmNN(stmt, n) = tmNN(stmt, m);
	tmNN(stmt, m) = tm;
}

void kStmtExprrimToSize(CTX, kStmtExpr *stmt, size_t n)
{
	size_t i;
	for(i = n; i < DP(stmt)->size; i++) {
		KNH_FINALv(_ctx, tmNN(stmt, i));
	}
	DBG_ASSERT(n <= DP(stmt)->size);
	DP(stmt)->size = n;
}

kTerm *knh_Stmt_done(CTX, kStmtExpr *stmt)
{
	knh_StmtEX_t *b = DP((kStmtExpr*)stmt);
	stmt->stt = STT_DONE;
	if(stmt->terms != NULL) {
		size_t i;
		for(i = 0; i < b->size; i++) {
			KNH_FINALv(_ctx, stmt->terms[i]);
		}
		KNH_FREE(_ctx, stmt->terms, sizeof(kTerm*) * b->capacity);
		stmt->terms = NULL;
		b->capacity = 0;
		b->size = 0;
	}
	KSETv(b->metaDictCaseMap, K_NULL);
	return (kTerm*)(stmt);
}

void kStmtExproERR(CTX, kStmtExpr *stmt, kTerm *tkERR)
{
	DBG_ASSERT(IS_StmtExpr(stmt));
	if(STT_(stmt) == STT_ERR) return;
	DBG_ASSERT(TT_(tkERR) == TT_ERR);
	DBG_ASSERT(IS_String((tkERR)->text));
	stmt->uline = tkERR->uline;
	if(DP(stmt)->size == 0) {
		TT_(tkERR) = TT_ASIS;
		knh_Stmt_add(_ctx, stmt, tkERR);
		TT_(tkERR) = TT_ERR;
	}
	else {
		KSETv(tmNN(stmt, 0), tkERR);
		kStmtExprrimToSize(_ctx, stmt, 1);
	}
	STT_(stmt) = STT_ERR;
}

static void Stmt_toSyntaxError(CTX, kStmtExpr *stmt, kTerm *tk K_TRACEARGV)
{
	kStmtExproERR(_ctx, stmt, ERROR_Term(_ctx, tk K_TRACEDATA));
}

/* ------------------------------------------------------------------------ */

typedef struct {
	struct kTerm** ts;
	int meta;
	int c;
	int e;
} tkitr_t;

#define ITR_next(itr)        (itr)->c += 1;
#define ITR_hasNext(itr)     ((itr)->c < (itr)->e)
#define ITR_size(itr)        ((itr)->e - (itr)->c)
#define ITR_uline(itr)       if((itr)->c < (itr)->e) kcodemod->uline = ((itr)->ts[(itr)->c])->uline

static tkitr_t* ITR_new(kTerm *tk, tkitr_t *buf)
{
	buf->meta = -1;
	buf->c = 0;
	if(IS_Array((tk)->data)) {
		buf->ts = ((tk)->list)->terms;
		buf->e = kArray_size((tk)->list);
	}
	else if(IS_Term((tk)->data)) {
		buf->ts = &((tk)->token);
		buf->e = 1;
	}
	else {
		buf->ts = NULL;
		buf->e = 0;
	}
	return buf;
}

static kterm_t ITR_tt(tkitr_t *itr)
{
	DBG_ASSERT(itr->c < itr->e);
	return TT_(itr->ts[itr->c]);
}

#define ITR_is(itr, tt) ITR_isN(itr, 0, tt)

static int ITR_isN(tkitr_t *itr, int shift, kterm_t tt)
{
	int c = itr->c + shift;
	return (c < itr->e && TT_(itr->ts[c]) == tt);
}

#define ITR_tk(itr)     (itr->ts[itr->c])

static kTerm *ITR_nextTK(tkitr_t *itr)
{
	DBG_ASSERT(itr->c < itr->e);
	(itr)->c += 1;
	return (itr)->ts[(itr)->c - 1];
}

static void ITR_chop(tkitr_t *itr, kterm_t tt)
{
	int e = itr->e - 1;
	if(itr->c <= e && TT_(itr->ts[e]) == tt) {
		itr->e = e;
	}
}

static int ITR_count(tkitr_t *itr, kterm_t tt)
{
	kTerm **ts = itr->ts;
	int i, cnt = 0;
	for(i = itr->c; i < itr->e; i++) {
		if(TT_(ts[i]) == tt) cnt++;
	}
	return cnt;
}

/* ------------------------------------------------------------------------ */
/* [tokenizer] */

static void TermBlock_add(CTX, kTerm *tkB, kTerm *tm);

#ifndef K_USING_SUGAR
static kterm_t TT_ch(int ch)
{
	switch(ch) {
		case '{': case '}': return TT_BRACE;
		case '(': case ')': return TT_PARENTHESIS;
		case '[': case ']': return TT_BRANCET;
#ifdef TT_SIZE
		case '|': return TT_SIZE;
#endif
		case ',': return TT_COMMA;
		case '"': return TT_STR;
		case '\'': return TT_TSTR;
		case '`': return TT_ESTR;
		case '/': return TT_REGEX;
	}
	DBG_ABORT("unknown ch=%c", ch);
	return TT_ERR;
}

static void TermB_setline(kTerm *tkB, kline_t uline)
{
	kTerm *tk = IS_Array((tkB)->data) ? (tkB)->list->terms[kArray_size((tkB)->list) - 1] : (tkB)->token;
	tk->uline = uline;
}
#endif

static int isTYPE(kTerm *tk)
{
	kterm_t tt = TT_(tk);
	return (tt == TT_PTYPE || tt == TT_UNAME || (TT_TYPEOF <= tt && tt <= TT_DYN));
}

static kTerm *new_TermCID(CTX, kcid_t cid)
{
	kTerm *tk = new_(Term);
	TT_(tk) = TT_UNAME;
	tk->uline = kcodemod->uline;
	(tk)->cid = cid;
	KSETv((tk)->text, ClassTBL(cid)->sname);
	return tk;
}

static kTerm *new_TermPTYPE(CTX, kcid_t cid, kTerm *tk)
{
	kTerm *tkT = new_Term(_ctx, TT_PTYPE);
	kTerm *tkC = new_TermCID(_ctx, cid);
	Term_setBOL(tkT, Term_isBOL(tk));
	if(cid == CLASS_Tvar) {
		TermBlock_add(_ctx, tkT, tk);
		TermBlock_add(_ctx, tkT, tkC);
	}
	else {
		TermBlock_add(_ctx, tkT, tkC);
		if(cid == CLASS_Map) {
			TermBlock_add(_ctx, tkT, new_TermCID(_ctx, CLASS_String));
		}
		TermBlock_add(_ctx, tkT, tk);
	}
	return tkT;
}

static int ITR_findPTYPE(tkitr_t *itr)
{
	int c, e = 0, f = 0;
	for(c = itr->e-2; 0 <= c; c--) {
		kTerm *tk = itr->ts[c];
		if(!isTYPE(tk)) break;
		if(e == 1) {
			itr->c = c; return 1;
		}
		c--; tk = itr->ts[c];
		if(c < 0) return 0;
		if(TT_(tk) == TT_COMMA) continue;
		if(TT_(tk) == TT_LT) { e = 1; continue; }
		if(f == 0 && TT_(tk) == TT_DARROW) {
			f = 1; continue;
		}
		break;
	}
	return 0;
}

static void Term_setTYPEOFEXPR(CTX, kTerm *tm);

static void TermBlock_add(CTX, kTerm *tkB, kTerm *tm)
{
	kTerm *tkPREV = NULL;
	kArray *a = NULL;
	int prev_idx = 0;
#ifndef K_USING_SUGAR
	DBG_ASSERT(tkB->uline != 0 && tm->uline != 0);
#endif
	if(IS_NULL((tkB)->data)) {
		KSETv((tkB)->data, tm);
		return;
	}
	if(IS_Term((tkB)->data)) {
		tkPREV = (tkB)->token;
		a = new_Array0(_ctx, 0);
		kArray_add(a, tkPREV);  // thank you nakata
		KSETv((tkB)->data, a);
	}
	else {
		DBG_ASSERT(IS_Array((tkB)->data));
		a = (tkB)->list;
		DBG_ASSERT(kArray_size(a) > 0);
		prev_idx = kArray_size(a)-1;
		tkPREV = a->terms[prev_idx];
	}

	if(TT_(tm) == TT_SEMICOLON) {
		kArray_add(a, tm);
		Term_setBOL(tm, 1);
		return;
	}
	// DBG_P("tkPREV->uline=%d,%s: tk->uline=%d,%s", (kshort_t)tkPREV->uline, TT__(tkPREV->tt), (kshort_t)tk->uline, TT__(tk->tt));
	if((tm->uline > tkPREV->uline) && TT_(tkPREV) != TT_SEMICOLON) {
		if(TT_(tkPREV) == TT_CODE) {
			kbytes_t t = S_tobytes(tkPREV->text);
			size_t i;
			kline_t uline = tkPREV->uline;
			for(i = 0; i < t.len; i++) {
				if(t.buf[i] == '\n') uline++;
			}
			DBG_P("@@@@ BLOCK uline %d => %d cur=%d", (short)tkPREV->uline, (short)uline, (short)tm->uline);
			if(uline == tm->uline) {
				Term_setSAMELINE(tkB, 1);
			}
		}
		if(!Term_isSAMELINE(tkB)) {
//			kTerm *tkSMC = new_Term(_ctx, TT_SEMICOLON);
			DBG_P("BOL: cur=%s,%d", TT__(tm->tt), (short)tm->uline);
			DBG_P("*isSAMELINE(tkB)=%d, prev=%s,%d cur=%s,%d", Term_isSAMELINE(tkB), TT__(tkPREV->tt), (short)tkPREV->uline, TT__(tm->tt), (short)tm->uline);
//			kArray_add(a, tkSMC);
//			Term_setBOL(tkSMC, 1);
			kArray_add(a, tm);
			Term_setBOL(tm, 1);
			return;
		}
	}
	kArray_add(a, tm);

	// 1. translation
	if(TT_(tm) == TT_DECLLET) {
		TT_(tm) = TT_LET;
		return;
	}

	if(prev_idx > 0 && TT_(tm) == TT_CODE && TT_(tkPREV) == TT_DARROW) {
		kTerm *tkPREV2 = a->terms[prev_idx - 1];
		if(TT_(tkPREV2) == TT_PARENTHESIS) {  // (n) => {} ==> function(n) {}
			TT_(tkPREV) = TT_FUNCTION;
			kArray_swap(_ctx, a, prev_idx-1, prev_idx);
			return;
		}
	}

	if(TT_(tm) == TT_PARENTHESIS) {
		if(TT_(tkPREV) == TT_DOT) {
			KSETv((tkPREV)->text, TS_EMPTY);
			Term_setDOT(tkPREV, 1);
			TT_(tkPREV) = (TT_(tkPREV) == TT_UNAME) ? TT_UFUNCNAME : TT_FUNCNAME;
		}
		else if(TT_(tkPREV) == TT_NAME) {
			TT_(tkPREV) = TT_FUNCNAME;
		}
		else if(TT_(tkPREV) == TT_UNAME) {
			TT_(tkPREV) = TT_UFUNCNAME;
		}
	}

	L_JOIN:;
	if(TT_(tkPREV) == TT_DOT) {
		if(TT_(tm) == TT_NAME || TT_(tm) == TT_UNAME || TT_(tm) == TT_FUNCNAME || TT_(tm) == TT_UFUNCNAME) {
			KSETv(a->list[prev_idx], tm);
			Term_setDOT(tm, 1);
			goto L_JOIN1;
		}
	}
	if(TT_isSTR(TT_(tm))) {
		if(TT_isSTR(TT_(tkPREV)) || TT_(tkPREV) == TT_URN) {
			CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
			knh_Bytes_write(_ctx, cwb->ba, S_tobytes((tkPREV)->text));
			if(tm->uline > tkPREV->uline && TT_(tkPREV) != TT_URN) {
				kwb_putc(cwb->ba, '\n');
			}
			knh_Bytes_write(_ctx, cwb->ba, S_tobytes((tm)->text));
			KSETv((tkPREV)->data, CWB_newString(_ctx, cwb, SPOL_POOLNEVER));
			//if(TT_(tk) == TT_ESTR) TT_(tkPREV) = TT_ESTR;
			goto L_JOIN1;
		}
	}
	if(TT_(tm) == TT_EXPT && TT_(tkPREV) == TT_UNAME) {
		Term_setExceptionType(tkPREV, 1);
		goto L_JOIN1;
	}
	if(TT_(tm) == TT_PARENTHESIS && TT_(tkPREV) == TT_TYPEOF) { // typeof
		KSETv(tkPREV->data, tm);
		Term_setTYPEOFEXPR(_ctx, tkPREV);
		goto L_JOIN1;
	}
	if(TT_(tkPREV) == TT_BYTE) {
		if(TT_(tm) == TT_BRANCET && IS_NULL((tm)->data)) { // byte[]
			KSETv(a->list[prev_idx], new_TermCID(_ctx, CLASS_Bytes));
			goto L_JOIN1;
		}
	}
	if(isTYPE(tkPREV)) {
		if(TT_(tm) == TT_BRANCET && IS_NULL((tm)->data)) { // String[]
			KSETv(a->list[prev_idx], new_TermPTYPE(_ctx, CLASS_Array, tkPREV));
			goto L_JOIN1;
		}
		if(TT_(tm) == TT_ITR) {  // String..
			KSETv(a->list[prev_idx], new_TermPTYPE(_ctx, CLASS_Iterator, tkPREV));
			goto L_JOIN1;
		}
		if(TT_(tm) == TT_QTN) {  // String?
			KSETv(a->list[prev_idx], new_TermPTYPE(_ctx, CLASS_KindOf, tkPREV));
			goto L_JOIN1;
		}
		if(TT_(tm) == TT_NOT) { // String! => Immutable<String>
			KSETv(a->list[prev_idx], new_TermPTYPE(_ctx, CLASS_Immutable, tkPREV));
			goto L_JOIN1;
		}
		if(TT_(tm) == TT_DIAMOND) { // Array<> => Array<var>
			KSETv(a->list[prev_idx], new_TermPTYPE(_ctx, CLASS_Tvar, tkPREV));
			goto L_JOIN1;
		}
		if(TT_(tm) == TT_DMUL) { // String** => Thunk<String>
			KSETv(a->list[prev_idx], new_TermPTYPE(_ctx, CLASS_Thunk, tkPREV));
			goto L_JOIN1;
		}
		if(TT_(tm) == TT_GT || TT_(tm) == TT_RSFT || TT_(tm) == TT_RSEND) { // String> or String>>
			tkitr_t itrbuf, *itr = ITR_new(tkB, &itrbuf);
			if(ITR_findPTYPE(itr)) {
				kTerm *tkT = new_Term(_ctx, TT_PTYPE);
				prev_idx = itr->c;
				while(ITR_hasNext(itr)) {
					kTerm *tkPT = ITR_nextTK(itr);
					TermBlock_add(_ctx, tkT, tkPT);
					if(ITR_is(itr, TT_DARROW)) {
						TermBlock_add(_ctx, tkT, ITR_tk(itr));
					}
					ITR_next(itr);
				}
				KSETv(a->list[prev_idx], tkT);
				if(TT_(tm) == TT_RSEND) {
					TT_(tm) = TT_RSFT;
					tkPREV = tkT;
					KSETv(a->list[prev_idx+1], tm);
					kArrayrimSize(_ctx, a, prev_idx+2);
					goto L_JOIN;
				}
				if(TT_(tm) == TT_RSFT) {
					TT_(tm) = TT_GT;
					tkPREV = tkT;
					KSETv(a->list[prev_idx+1], tm);
					kArrayrimSize(_ctx, a, prev_idx+2);
					goto L_JOIN;
				}
				goto L_JOIN1;
			}
		}
	}
	return;

	L_JOIN1:
	kArrayrimSize(_ctx, a, prev_idx+1);
}


static kTerm *Term_lastChildNULL(kTerm *tkB)
{
	if(IS_Array((tkB)->data)) {
		size_t n = kArray_size((tkB)->list);
		DBG_ASSERT(n>0);
		return (tkB)->list->terms[n-1];
	}
	else if(IS_Term((tkB)->data)) {
		return (tkB)->token;
	}
	return NULL;
}

static int Term_startsWithExpr(CTX, kTerm *tkB)
{
	kTerm *tk = Term_lastChildNULL(tkB);
	if(tk != NULL) {
		kterm_t tt = TT_(tk);
		if(tt == TT_COMMA || tt == TT_SEMICOLON || tt == TT_COLON) return 1;
		if(tt == TT_CASE || tt == TT_RETURN || tt == TT_YIELD
			|| tt == TT_PRINT || tt == TT_ASSERT) return 1;
		if(tt == TT_PREV || tt == TT_NEXT) return 0;  // a++ + 1
		if(TT_LET <= tt && tt <= TT_TSUB) return 1;
		return 0;
	}
	return 1;
}

static kString* KonohaSpace_getAliasNULL(CTX, kKonohaSpace* ns, kbytes_t t)
{
	kString *s = (kString*)knh_DictMap_getNULL(_ctx, ctx->wshare->sysAliasDictMap, t);
	while(1) {
		if(s == NULL) {
			if(ns->parentNULL != NULL) {
				ns = ns->parentNULL;
				continue;
			}
			if(ns != ctx->share->rootks) {
				ns = ctx->share->rootks;
				continue;
			}
		}
		break;
	}
	return s;
}

#ifndef K_USING_SUGAR

static kString *new_StringSYMBOL(CTX, kbytes_t t)
{
#ifdef K_USING_STRINGPOOL
	return new_S(t.text, t.len);
#else
	kDictMap *symbolDictMap = ctx->symbolDictMap;
	kindex_t idx = knh_DictMap_index(symbolDictMap, t);
	if(idx == -1) {
		kString *s = new_S(_ctx, t);
		knh_DictMap_set(_ctx, symbolDictMap, s, s);
		return s;
	}
	else {
		return knh_DictMap_keyAt(symbolDictMap, idx);
	}
#endif
}

static void Term_setNAME(CTX, kTerm *tk, CWB_t *cwb)
{
	kbytes_t t = CWB_tobytes(cwb);
	if(t.utext[0] == '.') {
		Term_setDOT(tk, 1);
	}
	if(t.utext[0] == '@') {  /* alias */
		t.utext = t.utext + 1; t.len = t.len - 1;
		TT_(tk) = TT_METAN;
		KSETv((tk)->data, new_StringSYMBOL(_ctx, t));
	}
	else if(isupper(t.utext[0]) || (t.utext[0] == '.' && isupper(t.utext[1]))) {
		TT_(tk) = TT_UNAME;
		if(t.utext[0] == '.') {
			t.utext = t.utext + 1; t.len = t.len - 1;
		}
		KSETv((tk)->data, new_StringSYMBOL(_ctx, t));
	}
	else {
		size_t i, u = 0;
		CWB_t cwbbuf2, *cwb2 = CWB_open(_ctx, &cwbbuf2);
		TT_(tk) = TT_NAME;
		for(i = cwb->pos; i < cwb->pos + t.len; i++) {
			const char *p = BA_totext(cwb->ba);
			int ch = p[i];
			if(ch == '.') continue;
			if(ch == '_') {
				if(!(CWB_size(cwb2) < 2)) { /* _name, __name */
					u = 1;
					continue;
				}
			}
			if(u == 1) {
				ch = toupper(ch);
				u = 0;
			}
			kwb_putc(cwb2->ba, ch);
		}
		KSETv((tk)->data, new_StringSYMBOL(_ctx, CWB_tobytes(cwb2)));
	}
}

/* ------------------------------------------------------------------------ */
/* [tokenizer] */


static void Term_setTEXT(CTX, kTerm *tk, CWB_t *cwb)
{
	kbytes_t t = CWB_tobytes(cwb);
	if(TT_(tk) == TT_UNTYPED) {
		kString *text = KonohaSpace_getAliasNULL(_ctx, K_GMANS, t);
		if(text != NULL) {
			t = S_tobytes(text);
			KSETv((tk)->data, text);
			CWB_clear(cwb, 0);
			knh_Bytes_write(_ctx, cwb->ba, t);  // alias
		}
		kDictSet *tokenDictSet = ctx->share->tokenDictSet;
		kindex_t idx = knh_DictSet_index(tokenDictSet, t);
		if(idx != -1) {
			kterm_t tt = (kterm_t)knh_DictSet_valueAt(tokenDictSet, idx);
			TT_(tk) = tt;
			if(IS_NULL((tk)->data)) {
				KSETv((tk)->data, knh_DictSet_keyAt(tokenDictSet, idx));
			}
		}
		else {
			Term_setNAME(_ctx, tk, cwb);
		}
	}
	else if(TT_(tk) == TT_METAN) {
		KSETv((tk)->data, new_StringSYMBOL(_ctx, t));
	}
	else {
		kString *s = CWB_newString(_ctx, cwb, 0);
		KSETv((tk)->data, s);
	}
}

static kTerm *addNewTerm(CTX, kTerm *tkB, kterm_t tt, int ch)
{
	kTerm *tk = new_Term(_ctx, tt);
	if(ch == ' ') {
		Term_setNWS(tk, 1);
	}
	return tk;
}

static void Term_addBuf(CTX, kTerm *tkB, CWB_t *cwb, kterm_t tt, int ch)
{
	if(CWB_size(cwb) != 0) {
		kTerm *tk = addNewTerm(_ctx, tkB, tt, ch);
		Term_setTEXT(_ctx, tk, cwb);
		TermBlock_add(_ctx, tkB, tk);  // must add after setting data
		CWB_clear(cwb, 0);
	}
	else if(tt == TT_CODE || TT_isSTR(tt) || tt == TT_REGEX) {
		kTerm *tk = addNewTerm(_ctx, tkB, tt, ch);
		KSETv((tk)->data, TS_EMPTY);
		TermBlock_add(_ctx, tkB, tk);   // must add after setting data
	}
}

static void InputStream_skipLINE(CTX, kInputStream *in, kline_t *ul)
{
	int ch;
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		if(ch == '\n') {
			ul[0] += 1;
			break;
		}
	}
}

static void InputStream_skipBLOCKCOMMENT(CTX, kInputStream *in, kline_t *ul, kBytes *ba)
{
	int ch, prev = 0, level = 1;
	if(ba != NULL) {
		while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
			if(ch == '\n') ul[0] += 1;
			if(prev == '*' && ch == '/') {
				level--;
				if(level == 0) break;
			}else if(prev == '/' && ch == '*') {
				level++;
			}
			kwb_putc(ba, ch);
			prev = ch;
		}
	}
	else {
		while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
			if(ch == '\n') ul[0] += 1;
			if(prev == '*' && ch == '/') {
				level--;
				if(level == 0) break;
			}else if(prev == '/' && ch == '*') {
				level++;
			}
			prev = ch;
		}
	}
}

static kbool_t Bytes_isTripleQuote(kBytes *ba, int quote)
{
	if(BA_size(ba) > 2 &&
		ba->bu.utext[BA_size(ba)-1] == quote
		&& ba->bu.utext[BA_size(ba)-2] == quote) return 1;
	return 0;
}

static void Bytes_addESC(CTX, kBytes *ba, kInputStream *in, kline_t *ul)
{
	int ch = knh_InputStream_getc(_ctx, in);
	if(ch == 'n') ch = '\n';
	else if(ch == 't') ch = '\t';
	else if(ch == 'r') ch = '\r';
	//if(ch == '\n') ul[0] += 1;
	kwb_putc(ba, ch);
}

static void Bytes_addQUOTE(CTX, kBytes *ba, kInputStream *in, kline_t *ul, int quote, int skip, int isRAW, int isTQUOTE)
{
	if(isRAW == 1) {
		int prev = quote;
		int ch = knh_InputStream_getc(_ctx, in);
		if(ch == '\n') ul[0] += 1;
		if(ch != EOF && ch != skip) goto L_INLOOP_RAW;
		while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
			L_INLOOP_RAW:;
			if(ch == quote && prev != '\\') {
				return;
			}
			if(ch == '\n') {
				ul[0] += 1;
				WARN_MustCloseWith(_ctx, quote);
				return ;
			}
			kwb_putc(ba, ch);
			if(prev == '\\' && ch == '\\') ch = 0;
			prev = ch;
		}
	}
	else {
		int ch = knh_InputStream_getc(_ctx, in);
		if(ch == '\n') ul[0] += 1;
		if(ch != skip) goto L_INLOOP;
		while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
			L_INLOOP:;
			if(ch == '\n' && isTQUOTE != 1) {
				WARN_MustCloseWith(_ctx, quote);
				return ;
			}
			if(ch == quote) {
				if(isTQUOTE == 1) {
					if(Bytes_isTripleQuote(ba, quote)) {
						knh_Bytes_reduce(ba, 2);
						return;
					}
				}
				else {
					return;
				}
			}
			else if(ch == '\\') {
				Bytes_addESC(_ctx, ba, in, ul);
				continue;
			}
			if(ch == '\n') ul[0] += 1;
			kwb_putc(ba, ch);
		}
	}
}

static int Term_addQUOTE(CTX, kTerm *tkB, CWB_t *cwb, kInputStream *in, kline_t *ul, int quote, int isRAW)
{
	int ch = knh_InputStream_getc(_ctx, in);
	if(ch == '\n') ul[0] += 1;
	if(quote == '#') {
		if(ch == '#') {
			Bytes_addQUOTE(_ctx, cwb->ba, in, ul, '\n', ' '/*skip*/, 1/*isRAW*/, 0/*isTQUOTE*/);
			Term_addBuf(_ctx, tkB, cwb, TT_ESTR, '\n');
			return '\n';
		}
		else if(ch == '!') {
			InputStream_skipLINE(_ctx, in, ul);
			return knh_InputStream_getc(_ctx, in);
		}
	}
	if(ch != quote) {
		if(ch == '\\' && !isRAW) {
			Bytes_addESC(_ctx, cwb->ba, in, ul);
		}
		else {
			kwb_putc(cwb->ba, ch);
		}
		Bytes_addQUOTE(_ctx, cwb->ba, in, ul, quote, -2, isRAW, 0/*isTQUOTE*/);
		ch = knh_InputStream_getc(_ctx, in);
		Term_addBuf(_ctx, tkB, cwb, TT_ch(quote), ch);
		return ch;
	}
	ch = knh_InputStream_getc(_ctx, in);
	if(quote != ch) { /* "" */
		Term_addBuf(_ctx, tkB, cwb, TT_ch(quote), ch);
	}
	else { /* triple quote */
		kterm_t tt = TT_ch(quote);
		if(tt == TT_TSTR) tt = TT_STR;
		Bytes_addQUOTE(_ctx, cwb->ba, in, ul, quote, '\n'/*skip*/, isRAW, 1/*isTQUOTE*/);
		kline_t uline = ul[0];
		ch = knh_InputStream_getc(_ctx, in);
		Term_addBuf(_ctx, tkB, cwb, tt, ch);
		TermB_setline(tkB, uline); Term_setSAMELINE(tkB, 1); /* ... hoge """ <<< EOL; */
	}
	return ch;
}


static int Term_addREGEX(CTX, kTerm *tkB, CWB_t *cwb, kInputStream *in, kline_t *ul, int ch0)
{
	int prev = 0;
	int ch = (ch0 == 0) ? knh_InputStream_getc(_ctx, in) : ch0;
	do {
		if(ch == '\n') {
			WARN_MustCloseWith(_ctx, '/');
			Term_addBuf(_ctx, tkB, cwb, TT_ch('/'), ch);
			return ch;
		}
		if(ch == '/') {
			if(prev != '\\') break;
			knh_Bytes_reduce(cwb->ba, 1);
		}
		kwb_putc(cwb->ba, ch);
		if(prev == '\\' && ch == '\\') ch = 0;
		prev = ch;
	} while((ch = knh_InputStream_getc(_ctx, in)) != EOF);
	ch = knh_InputStream_getc(_ctx, in);
	Term_addBuf(_ctx, tkB, cwb, TT_ch('/'), ch);
	return ch;
}


#define ISB1_(t, c)  (t.utext[0] == c)
#define ISB2_(t, c, c2)  (t.utext[0] == c && t.utext[1] == c2)

static int bytes_isOPR(kbytes_t t, int ch)
{
	if(t.len == 1) {
		switch(ch) {
		case '%':
			if(ISB1_(t, '%')) return 1; /* %% */
			return 0;
		case '&':
			if(ISB1_(t, '&')) return 1; /* && */
			return 0;
		case '+':
			if(ISB1_(t, '+')) return 1; /* ++ */
			if(ISB1_(t, '=')) return 1; /* =+ */
			return 0;
		case '-':
			if(ISB1_(t, '-')) return 1; /* -- */
			return 0;
		case '.':
			if(ISB1_(t, '.')) return 1;  /* .. */
			return 0;
		case '*':
			if(ISB1_(t, '*')) return 1; /* ** */
			return 0;
		case '<':
			if(ISB1_(t, '<')) return 1;  /* << */
			return 0;
		case '=':
			if(ISB1_(t, '+')) return 1;   /* += */
			if(ISB1_(t, '-')) return 1;   /* -= */
			if(ISB1_(t, '/')) return 1;   /* /= */
			if(ISB1_(t, '%')) return 1;   /* %= */
			if(ISB1_(t, '*')) return 1;   /* *= */
			if(ISB1_(t, '&')) return 1;   /* &= */
			if(ISB1_(t, '|')) return 1;   /* |= */
			if(ISB1_(t, '=')) return 1;   /* == */
			if(ISB1_(t, '!')) return 1;   /* != */
			if(ISB1_(t, '<')) return 1;   /* >= */
			if(ISB1_(t, '>')) return 1;   /* >= */
			if(ISB1_(t, ':')) return 1;   /* >= */
			return 0;
		case '>':
			if(ISB1_(t, '<')) return 1;  /* <> */
			if(ISB1_(t, '-')) return 1;  /* -> */
			if(ISB1_(t, '=')) return 1;  /* => */
			if(ISB1_(t, '>')) return 1;  /* >> */
			return 0;
		case '?':
			if(ISB1_(t, '?')) return 1;  /* ?? */
			return 0;
		case '!':
			if(ISB1_(t, '!')) return 1;  /* !! */
			return 0;
		case '|':
			if(ISB1_(t, '|')) return 1;  /* || */
			return 0;
		case '~':
			if(ISB1_(t, '=')) return 1;  /* ~= */
			return 0;
		case ':':
			if(ISB1_(t, '<')) return 1;  /* <: */
			return 0;
		}
	}
	else if(t.len == 2) {
		switch(ch) {
		case '.':
			if(ISB2_(t, '.', '.')) return 1; /* ... */
			return 0;
		case '*':
			if(ISB2_(t, '*', '*')) return 1; /* *** */
			return 0;
		case '+':
			if(ISB2_(t, '+', '+')) return 1; /* +++ */
			return 0;
		case '-':
			if(ISB2_(t, '-', '-')) return 1; /* --- */
			return 0;
		case '<':
			if(ISB2_(t, '<', '<')) return 1;  /* <<< */
			return 0;
		case '=':
			if(ISB2_(t, '<', '<')) return 1;  /* <<= */
			if(ISB2_(t, '>', '>')) return 1;  /* >>= */
			if(ISB2_(t, '?', '?')) return 1;   /* ??= */
			if(ISB2_(t, '=', '=')) return 1;   /* === */
			return 0;
		case '>':
			if(ISB2_(t, '>', '>')) return 1;  /* >>> */
			if(ISB2_(t, '=', '=')) return 1;  /* ==> */
			if(ISB2_(t, '-', '-')) return 1;  /* --> */
			return 0;
		}
	}
	return 0;
}

static int Term_addOPR(CTX, kTerm *tkB, CWB_t *cwb, kInputStream *in, kline_t *ul, int ch)
{
	if(ch == '/') {
		ch = knh_InputStream_getc(_ctx, in);
		if(ch == '*') {
			InputStream_skipBLOCKCOMMENT(_ctx, in, ul, NULL);
			return knh_InputStream_getc(_ctx, in);
		}else if(ch == '/') {
			InputStream_skipLINE(_ctx, in, ul);
			kcodemod->uline = ul[0];
			return knh_InputStream_getc(_ctx, in);
		}
		if(Term_startsWithExpr(_ctx, tkB)) {
			WARN_MuchBetter(_ctx, "$/(regex)/", "/(regex)/");
			return Term_addREGEX(_ctx, tkB, cwb, in, ul, ch);
		}
		kwb_putc(cwb->ba, '/');
		goto L_INLOOP;
	}
	kwb_putc(cwb->ba, ch);
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		L_INLOOP:;
		kbytes_t top = CWB_tobytes(cwb);
		if(bytes_isOPR(top, ch)) {
			kwb_putc(cwb->ba, ch);
		}
		else {
			int isHEAD = Term_startsWithExpr(_ctx, tkB);
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			if(isHEAD) {
				kTerm *ctk = Term_lastChildNULL(tkB);
				DBG_ASSERT(ctk != NULL);
				if(TT_(ctk) == TT_SUB) TT_(ctk) = TT_TSUB;
				if(TT_(ctk) == TT_ADD) TT_(ctk) = TT_TADD;
				if(TT_(ctk) == TT_LAND) TT_(ctk) = TT_TAND;
			}
			break;
		}
	}
	return ch;
}

static int Term_addMETAN(CTX, kTerm *tk, CWB_t *cwb, kInputStream *in, kline_t *ul)
{
	int ch;
	Term_addBuf(_ctx, tk, cwb, TT_UNTYPED, '@');
	ch = knh_InputStream_getc(_ctx, in);
	if(ch == '"' || ch == '\'' || ch == '`') {
		ch = Term_addQUOTE(_ctx, tk, cwb, in, ul, ch, /*RAW*/1);
	}
	do {
		if(!isalnum(ch) && ch != '_' && ch != '.') {
			break;
		}
		kwb_putc(cwb->ba, ch);
	}
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF);
	Term_addBuf(_ctx, tk, cwb, TT_METAN, ch);
	return ch;
}

static int Term_addPROPN(CTX, kTerm *tkB, CWB_t *cwb, kInputStream *in, kline_t *ul)
{
	int ch;
	Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, '$');
	ch = knh_InputStream_getc(_ctx, in);
	if(ch == '/') {
		return Term_addREGEX(_ctx, tkB, cwb, in, ul, 0);
	}
	do {
		if(!isalnum(ch) && ch != '_' && ch != '.' && ch != '*') {
			break;
		}
		kwb_putc(cwb->ba, ch);
	}
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF);
	Term_addBuf(_ctx, tkB, cwb, TT_PROPN, ch);
	return ch;
}

static int Term_addURN(CTX, kTerm *tk, CWB_t *cwb, kInputStream *in, kline_t *ul)
{
	int ch = knh_InputStream_getc(_ctx, in);
	if(ch == ':') {  /* hoge:: */
		ch = knh_InputStream_getc(_ctx, in);
		Term_addBuf(_ctx, tk, cwb, TT_TLINK, ch);
		return ch;
	}
	if(ch == '=') {   /* a:=1*/
		Term_addBuf(_ctx, tk, cwb, TT_UNTYPED, ':');
		kwb_putc(cwb->ba,':');
		kwb_putc(cwb->ba,'=');
		ch = knh_InputStream_getc(_ctx, in);
		Term_addBuf(_ctx, tk, cwb, TT_UNTYPED, ch);
		return ch;
	}
	if(isspace(ch) || ch == EOF || ch == ';' || ch == '"' || ch == '\'' || ch == '(' || ch == '{' || ch == '[') {
		Term_addBuf(_ctx, tk, cwb, TT_UNTYPED, ':');
		kwb_putc(cwb->ba, ':');
		Term_addBuf(_ctx, tk, cwb, TT_UNTYPED, ch);
		return ch;
	}
	kwb_putc(cwb->ba, ':');
	kwb_putc(cwb->ba, ch);
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		switch(ch) {
			case '\n': case '\r': case ' ':  case '\t': case ';': case ',':
			case '\'': case '"': case '`':
			case ')': case '}': case ']':
			case '(': case '{': case '[':
				goto L_NEWURN;
		}
		kwb_putc(cwb->ba, ch);
	}
	L_NEWURN: {
		kbytes_t t = CWB_tobytes(cwb);
		kterm_t tt = TT_URN;
#ifdef K_USING_SEMANTICS
		if(knh_bytes_startsWith_(t, STEXT("int:")) || knh_bytes_startsWith_(t, STEXT("float:"))) {
			t.ubuf[0] = toupper(t.utext[0]);
		}
#endif
		if(knh_bytes_startsWith_(t, STEXT("new:"))) {
			tt = TT_NEW;
		}
		Term_addBuf(_ctx, tk, cwb, (isupper(t.utext[0])) ? TT_UNAME : TT_URN, ch);
	}
	return ch;
}

static int Term_addNUM(CTX, kTerm *tk, CWB_t *cwb, kInputStream *in, kline_t *ul, int ch)
{
	int prev = 0, dot = 0;
	L_ADD:;
	kwb_putc(cwb->ba, ch);
	prev = ch;
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		if(isalnum(ch)) goto L_ADD;
		if(ch == '_') continue; // nothing
		if(prev == '.' && ch == '.') {  /* 1.. => 1 .. */
			knh_Bytes_reduce(cwb->ba, 1);
			Term_addBuf(_ctx, tk, cwb, TT_NUM, '.');
			kwb_putc(cwb->ba, '.');
			kwb_putc(cwb->ba, '.');
			ch = knh_InputStream_getc(_ctx, in);
			Term_addBuf(_ctx, tk, cwb, TT_UNTYPED, ch);
			return ch;
		}
		if(ch == '.') {
			dot++;
			if(dot == 1) goto L_ADD;
			continue;
		}
		if((ch == '-' || ch == '+') && (prev == 'e' || prev =='E')) {
			goto L_ADD;
		}
		break;
	}
	Term_addBuf(_ctx, tk, cwb, TT_NUM, ch);
	return ch;
}

static void Term_addBLOCKERR(CTX, kTerm *tkB, kInputStream *in, kline_t *ul, int ch)
{
	const char *block = "indent";
	if(ch == ')') block = ")";
	if(ch == ']') block = "]";
	tkB->uline = ul[0];
	(_ctx->gma)->uline = ul[0];
	TermBlock_add(_ctx, tkB, ERROR_Block(_ctx, block));
}

static void Bytes_addRAW(CTX, kBytes *ba, kInputStream *in, kline_t *ul, int end)
{
	int prev = 0, ch;
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		if(ch == '\n') ul[0] += 1;
		kwb_putc(ba, ch);
		if(ch == end && prev != '\\') {
			return;
		}
		if(prev == '\\' && ch == '\\') ch = 0;
		prev = ch;
	}
}

static void Term_addBLOCK(CTX, kTerm *tkB, CWB_t *cwb, kInputStream *in, kline_t *ul, int block_indent)
{
	int c, this_indent = 0, ch, prev = '{', level = 1;
	Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, '{');
	DBG_P("block_indent=%d", block_indent);
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		if(ch != '\t' && ch != ' ') goto L_STARTLINE;
	}
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		L_STARTLINE:;
		if(ch == '\n') ul[0] += 1;
		if(ch == '}') {
			level--;
			if(level == 0) {
				Term_addBuf(_ctx, tkB, cwb, TT_CODE, ch);
				return;
			}
		}
		kwb_putc(cwb->ba, ch);
		if(ch == '\n') {
			prev = ch;
			break;
		}
		if(ch == '{') {
			level++;
		}
		else if(prev == '#' && ch == '#') {
			Bytes_addRAW(_ctx, cwb->ba, in, ul, '\n');
		}
		else if(ch == '"' || ch == '`' || ch == '\'') {
			Bytes_addRAW(_ctx, cwb->ba, in, ul, ch);
		}
		else if(ch == '/') {
			if(prev == '$' || prev == '@') {
				Bytes_addRAW(_ctx, cwb->ba, in, ul, ch);
			}
			else if(prev == '/') {
				kbytes_t t = CWB_tobytes(cwb);
				if(knh_bytes_rindex(t, ':') == t.len - 3/* '://' */) {
					/* case 'URN://' */
				} else {
					/* case LINE_COMMENT '//' */
					knh_Bytes_reduce(cwb->ba, 2);
					InputStream_skipLINE(_ctx, in, ul);
					kwb_putc(cwb->ba, '\n');
					break;
				}
			}
		}
		else if(prev == '/' && ch == '*') {
			InputStream_skipBLOCKCOMMENT(_ctx, in, ul, cwb->ba);
			kwb_putc(cwb->ba, '/');
		}
		prev = ch;
	}
	c = 0;
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		if(ch == '\t') { c += 3; continue; }
		if(ch == ' ') { c += 1; continue; }
		break;
	}
	if(ch != EOF /* && block_indent <= c*/) {
		int i;
		if(this_indent == 0 && c > 0) this_indent = c;
		for(i = this_indent; i < c; i++) {
			kwb_putc(cwb->ba, ' ');
		}
		goto L_STARTLINE;
	}
	CWB_clear(cwb, 0);
//	DBG_P("block_indent=%d, c=%d, last=%d", block_indent, c, ch);
	Term_addBLOCKERR(_ctx, tkB, in, ul, 0);
}

static int InputStream_skipMultiByteChar(CTX, kInputStream *in, kline_t *ul, int ch)
{
	char buf[8] = {ch, 0};
	int i, size = knh_utf8len(ch);
	if(size > 2) {
		for(i = 1; i < size; i++) {
			ch = knh_InputStream_getc(_ctx, in);
			buf[i] = ch;
		}
		WARN_UnxpectedMultiByteChar(_ctx, buf);
		return ' ';
	}
	else {
		while((ch = knh_InputStream_getc(_ctx, in)) > 127);
		WARN_UnxpectedMultiByteChar(_ctx, NULL);
		return ch;
	}
}

static void InputStream_parseTerm(CTX, kInputStream *in, kline_t *ul, kTerm *tkB)
{
	int ch;
	int block_indent = 0, block_line = 0;
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	DBG_ASSERT(ul[0] != 0);
	Term_setSAMELINE(tkB, 0);
	L_NEWLINE:;
	{
		int c = 0;
		kcodemod->uline = ul[0];
		while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
			if(ch == '\t') { c += 3; }
			else if(ch == ' ') { c += 1; }
			else if(ch == '\n') {
				ul[0] += 1;
				goto L_NEWLINE;
			}
			else {
				if(block_line == 0) {
					block_indent = c;
				}
				else {
					if(block_indent < c) {
						DBG_P("indent %d < %d", block_indent, c);
						Term_setSAMELINE(tkB, 1);
					}
					else if(c < block_indent) {
						Term_addBLOCKERR(_ctx, tkB, in, ul, 0);
						CWB_close(_ctx, cwb);
						return;
					}
				}
				block_line += 1;
				goto L_AGAIN;
			}
		}
	}
	L_NEWTOKEN:;
	CWB_clear(cwb, 0);
	kcodemod->uline = ul[0];
	while((ch = knh_InputStream_getc(_ctx, in)) != EOF) {
		L_AGAIN:;
		switch(ch) {
		case '\n':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			Term_setSAMELINE(tkB, 0);
			ul[0] += 1;
			goto L_NEWLINE;

		case '\\':
			InputStream_skipLINE(_ctx, in, ul);
			Term_setSAMELINE(tkB, 1);
			goto L_NEWLINE;

		case ' ': case '\t': case '\v': case '\r':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			goto L_NEWTOKEN;

		case ';': case ',':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			kwb_putc(cwb->ba, ch);
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ' ');
			goto L_NEWTOKEN;

		case '{':
			Term_setSAMELINE(tkB, 1);
			Term_addBLOCK(_ctx, tkB, cwb, in, ul, block_indent);
			goto L_NEWTOKEN;

		case '[': case '(':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			{
				kTerm *tkSUB = new_Term(_ctx, TT_ch(ch));
				tkSUB->h.meta = tkB;
				tkB = tkSUB;
			}
			goto L_NEWTOKEN;

		case ')': case ']':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			if(TT_ch(ch) == TT_(tkB)) {
				kTerm *tkSUB = tkB;
				tkB = (kTerm*)(tkSUB)->h.meta;
				(tkSUB)->h.meta = NULL;
				TermBlock_add(_ctx, tkB, tkSUB);
				TermB_setline(tkB, kcodemod->uline);
				goto L_NEWTOKEN;
			}
		case '}':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			Term_addBLOCKERR(_ctx, tkB, in, ul, ch);
			return;
			/*goto L_NEWTOKEN;*/

		/* token */
		case '\'': case '"': case '`' :
		case '#':
			ch = Term_addQUOTE(_ctx, tkB, cwb, in, ul, ch, 0/*isRAW*/);
			goto L_AGAIN;
		case '$':
			ch = Term_addPROPN(_ctx, tkB, cwb, in, ul);
			goto L_AGAIN;
		case '@':
			ch = Term_addMETAN(_ctx, tkB, cwb, in, ul);
			goto L_AGAIN;
		case '|':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			if(TT_ch(ch) == TT_(tkB)) {
				kTerm *ctk = tkB;
				tkB = (kTerm*)(ctk)->h.meta;
				(ctk)->h.meta = NULL;
				TermBlock_add(_ctx, tkB, ctk);
				TT_(ctk) = TT_PARENTHESIS;
				knh_Bytes_write(_ctx, cwb->ba, STEXT(".size"));
				Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ' ');
				goto L_NEWTOKEN;
			}
			if(Term_startsWithExpr(_ctx, tkB)) {
				kTerm *ctk = new_Term(_ctx, TT_ch(ch));
				ctk->h.meta = tkB;
				tkB = ctk;
				goto L_NEWTOKEN;
			}
		case '?': {
			kbytes_t t = CWB_tobytes(cwb);
			if(ISB(t, "in") || ISB(t, "isa") || ISB(t, "is")) {
				kwb_putc(cwb->ba, ch);
				Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ' ');
				break;
			}
		}
		case '+': case '-': case '*': case '/': case '%':
		case '=': case '&':
		case '<': case '>': case '^': case '!': case '~':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, ch);
			ch = Term_addOPR(_ctx, tkB, cwb, in, ul, ch);
			goto L_AGAIN;

		case '.':
			Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, '.');
			ch = knh_InputStream_getc(_ctx, in);
			if(ch == '.') {
				kwb_putc(cwb->ba, '.');
				ch = Term_addOPR(_ctx, tkB, cwb, in, ul, ch);
			}
			else if(isdigit(ch)) {
				kwb_putc(cwb->ba, '0');
				kwb_putc(cwb->ba, '.');
				ch = Term_addNUM(_ctx, tkB, cwb, in, ul, ch);
			}
			else {
				kwb_putc(cwb->ba, '.');
			}
			goto L_AGAIN;

		case ':':
			ch = Term_addURN(_ctx, tkB, cwb, in, ul);
			goto L_AGAIN;

		case EOF :
			break;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if(CWB_size(cwb) == 0) {
				ch = Term_addNUM(_ctx, tkB, cwb, in, ul, ch);
				goto L_AGAIN;
			}

		default:
			if(ch > 127) {
				ch = InputStream_skipMultiByteChar(_ctx, in, ul, ch);
				goto L_AGAIN;
			}
			kwb_putc(cwb->ba, ch);
		} /* switch */
	}/*while*/
	Term_addBuf(_ctx, tkB, cwb, TT_UNTYPED, EOF);
	CWB_close(_ctx, cwb);
}
#endif

#ifdef K_USING_SUGAR

static void Term_meta(CTX, kTerm *tm)
{
	kbytes_t t = S_tobytes(tm->text);
	DBG_ASSERT(t.utext[0] == '@');
	t.utext = t.utext + 1; t.len = t.len - 1;
	TT_(tm) = TT_METAN;
	KSETv((tm)->data, new_S(t.text, t.len));
}

static void Term_op(CTX, kTerm *tm)
{
	kbytes_t t = S_tobytes(tm->text);
	kString *text = KonohaSpace_getAliasNULL(_ctx, K_GMANS, t);
	if(text != NULL) {
		KSETv((tm)->data, text);
	}
	kDictSet *tokenDictSet = ctx->share->tokenDictSet;
	kindex_t idx = knh_DictSet_index(tokenDictSet, t);
	if(idx != -1) {
		kterm_t tt = (kterm_t)knh_DictSet_valueAt(tokenDictSet, idx);
		TT_(tm) = tt;
		if(IS_NULL((tm)->data)) {
			KSETv((tm)->data, knh_DictSet_keyAt(tokenDictSet, idx));
		}
	}
}

static void Term_name(CTX, kTerm *tm)
{
	kbytes_t t = S_tobytes(tm->text);
	tm->tt = TT_NAME;
	Term_op(_ctx, tm);
	if(tm->tt != TT_NAME) return;
	if(t.utext[0] == '@') {  /* alias */
		Term_meta(_ctx, tm);
	}
	else {
		size_t i, u = 0;
		CWB_t cwbbuf2, *cwb2 = CWB_open(_ctx, &cwbbuf2);
		TT_(tm) = TT_NAME;
		for(i = 0; i < t.len; i++) {
			int ch = t.utext[i];
			if(ch == '.') continue;
			if(ch == '_') {
				if(!(CWB_size(cwb2) < 2)) { /* _name, __name */
					u = 1;
					continue;
				}
			}
			if(u == 1) {
				ch = toupper(ch);
				u = 0;
			}
			kwb_putc(cwb2->ba, ch);
		}
		t = CWB_tobytes(cwb2);
		KSETv((tm)->data, new_S(t.text, t.len));
	}
}

static kTerm *new_TokenTerm(CTX, kToken *tk)
{
	kTerm *tm = new_Term(_ctx, TT_ERR);
	tm->uline = tk->uline;
	KSETv(tm->text, tk->text);
	switch((int)tk->token) {
	case TK_CODE:      tm->tt = TT_CODE; break;
	case TK_OPERATOR:  Term_op(_ctx, tm); break;
	case TK_SYMBOL:    Term_name(_ctx, tm); break;
	case TK_USYMBOL:   tm->tt = TT_UNAME; break;
	case TK_TEXT:      tm->tt = TT_STR;   break;
	case TK_STEXT:     tm->tt = TT_TSTR;  break;
	case TK_INT:
	case TK_FLOAT:     tm->tt = TT_NUM;   break;
	case TK_URN:       tm->tt = TT_URN;   break;
	case TK_REGEX:     tm->tt = TT_REGEX; break;
	case TK_MN:      Term_meta(_ctx, tm);  break;
	case TK_TYPE:      tm->tt = TT_PROPN;  break;
	}
	if(tm->tt == TT_ERR) {
		kerror(_ctx, 0, tk->uline, tk->lpos, "unknown token %s", S_text(tk->text));
	}
	return tm;
}

static int TokenArray_parseTerm(CTX, kArray *a, int s, kTerm *tkB, int closech)
{
	int i;
	for(i = s; i < kArray_size(a); i++) {
		kToken *tk = a->tokens[i];
		if(tk->token == TK_INDENT) continue;
		if(tk->token == TK_WHITESPACE) continue;
		if(tk->topch == closech) {
			return i;
		}
		switch(tk->topch) {
		case '(': {
			kTerm *tkSUB = new_Term(_ctx, TT_PARENTHESIS);
			tkSUB->uline = tk->uline;
			if((i = TokenArray_parseTerm(_ctx, a, i + 1, tkSUB, ')'))) {
				TermBlock_add(_ctx, tkB, tkSUB);
				continue;
			}
			tkSUB->tt = TT_ERR;
			TermBlock_add(_ctx, tkB, tkSUB);
			return 0;
		}
		case '[': {
			kTerm *tkSUB = new_Term(_ctx, TT_BRANCET);
			if((i = TokenArray_parseTerm(_ctx, a, i + 1, tkSUB, ']'))) {
				TermBlock_add(_ctx, tkB, tkSUB);
				continue;
			}
			tkSUB->tt = TT_ERR;
			TermBlock_add(_ctx, tkB, tkSUB);
			return 0;
		}
		case '{': {
			kTerm *tkSUB = new_Term(_ctx, TT_BRACE);
			if((i = TokenArray_parseTerm(_ctx, a, i + 1, tkSUB, '}'))) {
				TermBlock_add(_ctx, tkB, tkSUB);
				continue;
			}
			tkSUB->tt = TT_ERR;
			TermBlock_add(_ctx, tkB, tkSUB);
			return 0;
		}
		case '|':
			if(Term_startsWithExpr(_ctx, tkB)) {
				kTerm *tkSUB = new_Term(_ctx, TT_PARENTHESIS);
				if((i = TokenArray_parseTerm(_ctx, a, i+1, tkSUB, '|'))) {
					kTerm *tm = new_Term(_ctx, TT_NAME);
					KSETv(tm->text, new_S("size", sizeof("size")));
					Term_setDOT(tm, 1);
					TermBlock_add(_ctx, tkB, tkSUB);
					TermBlock_add(_ctx, tkB, tm);
					continue;
				}
				tkSUB->tt = TT_ERR;
				TermBlock_add(_ctx, tkB, tkSUB);
				return 0;
			}
		case ')': case ']': case '}':
			kerror(_ctx, 0, tk->uline, tk->lpos, "mismatched %s", S_text(tk->text));
			return 0;
		case '_':
			TermBlock_add(_ctx, tkB, new_(Term));
			break;
		default:
			TermBlock_add(_ctx, tkB, new_TokenTerm(_ctx, tk));
		}
	}
	return 0;
}
#endif

/* ------------------------------------------------------------------------ */

#ifdef K_USING_SUGAR

kArray* new_TokenArray(CTX, const char *text, kline_t uline);

static void Term_toBRACE(CTX, kTerm *tk, int isEXPANDING)
{
	if(S_size(tk->text) > 0) {
		INIT_GCSTACK(_ctx);
		kArray *a = new_TokenArray(_ctx, S_text((tk)->text), tk->uline);
		KSETv((tk)->data, K_NULL);
		TT_(tk) = TT_BRACE;
		TokenArray_parseTerm(_ctx, a, 0, tk, -1);
		DBG_(
		if(knh_isVerboseLang() && ULINE_uri(tk->uline) == URI_EVAL) {
			knh_write_Object(_ctx, KNH_STDOUT, UPCAST(tk), FMT_dump);
			knh_flush(_ctx, KNH_STDOUT);
		});
		RESET_GCSTACK(_ctx);
	}
	else {
		KSETv((tk)->data, K_NULL);
		TT_(tk) = TT_BRACE;
	}
}
#else
static void Term_toBRACE(CTX, kTerm *tk, int isEXPANDING)
{
        if(S_size(tk->text) > 0) {
                ksfp_t *lsfp = knh_stack_local(_ctx, 1);
                int sfpidx_ = lsfp - ctx->stack;
                LOCAL_NEW(_ctx, lsfp, 0, kInputStream*, in, new_BytesInputStream(_ctx, S_text((tk)->text), S_size((tk)->text)));
                KSETv((tk)->data, K_NULL);
                TT_(tk) = TT_BRACE;
                kline_t uline = tk->uline;
                InputStream_parseTerm(_ctx, in, &uline, tk);
                DBG_(
                if(knh_isVerboseLang() && ULINE_uri(uline) == URI_EVAL) {
                        knh_write_Object(_ctx, KNH_STDOUT, UPCAST(tk), FMT_dump);
                });
                END_LOCAL(_ctx, lsfp);
        }
        else {
                KSETv((tk)->data, K_NULL);
                TT_(tk) = TT_BRACE;
        }
}
#endif

static void Stmt_expand(CTX, kStmtExpr *stmt)
{
	size_t newcapacity = DP(stmt)->capacity * 2;
	if(newcapacity == 0) {
		newcapacity = (K_FASTMALLOC_SIZE / sizeof(kObject*));
	}
	stmt->terms = (kTerm**)KNH_REALLOC(_ctx, NULL, stmt->terms, DP(stmt)->capacity, newcapacity, sizeof(kTerm*));
	DP(stmt)->capacity = newcapacity;
}

kTerm *knh_Stmt_add_(CTX, kStmtExpr *stmt, ...)
{
	va_list ap;
	kTerm *tm = NULL;
	size_t size = DP(stmt)->size;
	va_start(ap, stmt);
	while((tm = (kTerm*)va_arg(ap, kTerm*)) != NULL) {
		DBG_ASSERT(IS_Term(tm) || IS_StmtExpr(tm));
		if(!(DP(stmt)->size < DP(stmt)->capacity)) {
			Stmt_expand(_ctx, stmt);
		}
		DBG_ASSERT(tmNN(stmt, size) == NULL);
		KINITv(tmNN(stmt, size), tm);
		size++;
		DP(stmt)->size = size;
		if(STT_(stmt) == STT_ERR) continue;
		if(TT_(tm) == TT_ERR) {
			kStmtExproERR(_ctx, stmt, (kTerm*)tm);
		}
		if(TT_(tm) == STT_ERR) {
			kStmtExpr *stmt2 = (kStmtExpr*)tm;
			kTerm *tk = tkNN(stmt2, 0);
			DBG_ASSERT(TT_(tk) == TT_ERR);
			kStmtExproERR(_ctx, stmt, tk);
		}
	}
	va_end(ap);
	return (kTerm*)(stmt);
}

kTerm *new_TermMN(CTX, kmethodn_t mn)
{
	kTerm *tk = new_(Term);
	tk->uline = kcodemod->uline;
	TT_(tk) = TT_MN;
	(tk)->mn = mn;
	if(MN_isGETTER(mn)) {Term_setGetter(tk, 1);};
	if(MN_isSETTER(mn)) {Term_setSetter(tk, 1);}
	return tk;
}

/* ------------------------------------------------------------------------ */

#define ADD(stmt, FUNC) \
	if(STT_(stmt) != STT_ERR) {\
		FUNC;\
	}\

static void _EXPR(CTX, kStmtExpr *stmt, tkitr_t *itr);
static kStmtExpr *new_StmtSTMT1(CTX, tkitr_t *itr);

static int ITR_indexTT(tkitr_t *itr, kterm_t tt, int def)
{
	int i;
	for(i = itr->c; i < itr->e; i++) {
		if(TT_(itr->ts[i]) == tt) return i;
	}
	return def;
}

static void ITR_replace(tkitr_t *itr, kterm_t tt, kterm_t tt1)
{
	int i;
	for(i = itr->c; i < itr->e; i++) {
		if(TT_(itr->ts[i]) == tt) TT_(itr->ts[i]) = tt1;
	}
}

static tkitr_t *ITR_copy(tkitr_t *itr, tkitr_t *buf, int shift)
{
	*buf = *itr;
	buf->c = itr->c + shift;
	return buf;
}

static void _SEMICOLON(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_SEMICOLON)) {
		ITR_next(itr);
	}
	else if(STT_(stmt) != STT_ERR) {
		WARN_Semicolon(_ctx);
	}
}

static void ITR_skipSEMICOLON(tkitr_t *itr)
{
	while(ITR_hasNext(itr)) {
		if(TT_(ITR_tk(itr)) != TT_SEMICOLON) break;
		ITR_next(itr);
	}
}

static void ITR_nextStmt(tkitr_t *itr)
{
	int i;
	kTerm **ts = itr->ts;
	for(i = itr->c; i < itr->e; i++) {
		if(Term_isBOL(ts[i])) {
			itr->c = i;
			ITR_skipSEMICOLON(itr);
			return;
		}
	}
	itr->c = itr->e;
}

static tkitr_t *ITR_stmt(CTX, tkitr_t *itr, int pos, tkitr_t *buf, int isNeedSemicolon)
{
	int i;
	*buf = *itr;
	DBG_ASSERT(pos > 0);
	for(i = itr->c + pos; i < itr->e; i++) {
		if(Term_isBOL(itr->ts[i])) {
			if(isNeedSemicolon && TT_(itr->ts[i]) != TT_SEMICOLON) {
				WARN_Semicolon(_ctx);
			}
			buf->e = i;
			itr->c = i;
			ITR_skipSEMICOLON(itr);
			return buf;
		}
	}
	buf->e = itr->e;
	itr->c = itr->e;
	if(isNeedSemicolon && itr->e -1 >= 0) {
		WARN_Semicolon(_ctx);
	}
	return buf;
}

static tkitr_t *ITR_first(tkitr_t *titr, int idx, tkitr_t *buf, int shift)
{
	DBG_ASSERT(idx != -1 && idx <= titr->e);
	int i;
	*buf = *titr;
	buf->meta = -1;
	for(i = titr->c; i < idx; i++) {
		kterm_t tt = TT_(titr->ts[i]);
		if(tt == TT_METAN) {
			if(buf->meta == -1) buf->meta = i;
			titr->c = i + 1;
		}
		else {
			break;
		}
	}
	buf->e = idx;
	titr->c = idx + shift;
	titr->meta = -1;
	return buf;
}

static void ITR_skipUNTIL(tkitr_t *itr, kterm_t tt)
{
	int i;
	for(i = itr->c; i < itr->e; i++) {
		if(TT_(itr->ts[i]) == tt) {
			itr->e = i;
			return;
		}
	}
}

static void TT_skipMETA(CTX, tkitr_t *itr)
{
	itr->meta = -1;
	while(ITR_hasNext(itr)) {
		kterm_t tt = ITR_tt(itr);
		kcodemod->uline = ITR_tk(itr)->uline;
		if(ITR_isN(itr, +1, TT_COLON) && (tt == TT_NAME || tt == TT_UNAME) && !ITR_isN(itr, +2, TT_PARENTHESIS)) {
			if(itr->meta == -1) itr->meta = itr->c;
			ITR_next(itr);
		}
		else if(tt == TT_METAN) {
			if(ITR_isN(itr, +1, TT_STR) || ITR_isN(itr, +1, TT_TSTR) || ITR_isN(itr, +1, TT_ESTR)) {
				TT_(itr->ts[itr->c]) = TT_DOCU;
				break;
			}
			if(itr->meta == -1) itr->meta = itr->c;
		}
		else {
			break;
		}
		ITR_next(itr);
	}
}

#define ITR_ignore(_ctx, itr, n)

/* ------------------------------------------------------------------------ */

#define _ERROR(_ctx, stmt, itr, whatis)    _DBGERROR(_ctx, stmt, itr, whatis K_TRACEPOINT); return;

static void _DBGERROR(CTX, kStmtExpr *stmt, tkitr_t *itr, const char *whatis K_TRACEARGV)
{
	if(STT_(stmt) != STT_ERR) {
		kTerm* tkERR;
		if(whatis != NULL && ITR_hasNext(itr)) {
			tkERR = ITR_tk(itr);
			if(TT_(tkERR) != TT_ERR) {
				tkERR = ERROR_TermIsNot(_ctx, ITR_tk(itr), whatis);
			}
		}
		else {
			tkERR = ERROR_Stmt(_ctx, stmt, _file, _line, _func);
		}
		kStmtExproERR(_ctx, stmt, tkERR);
	}
	ITR_nextStmt(itr);
}
static void _ASIS(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	knh_Stmt_add(_ctx, stmt, new_Term(_ctx, TT_ASIS));
}

static void _VAR(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	knh_Stmt_add(_ctx, stmt, new_Term(_ctx, TT_VAR));
}
static void _DONE(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	knh_Stmt_add(_ctx, stmt, new_Stmt2(_ctx, STT_DONE, NULL));
}

typedef int (*knh_FisTerm)(kTerm *tk);

static int ITR_isT(tkitr_t *itr, knh_FisTerm f)
{
	return (ITR_hasNext(itr) && f(ITR_tk(itr)));
}

static void Stmt_tadd(CTX, kStmtExpr *stmt, tkitr_t *itr, knh_FisTerm f, const char *whatis)
{
	if(STT_(stmt) == STT_ERR) return;
	if(ITR_hasNext(itr) && f(ITR_tk(itr))) {
		knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
	}
	else {
		_ERROR(_ctx, stmt, itr, whatis);
	}
}

static void Stmt_taddASIS(CTX, kStmtExpr *stmt, tkitr_t *itr, knh_FisTerm f)
{
	if(STT_(stmt) == STT_ERR) return;
	if(ITR_hasNext(itr)) {
		kTerm *tk = ITR_tk(itr);
		if(TT_(tk) != TT_SEMICOLON && f(tk)) {
			knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
			return;
		}
	}
	_ASIS(_ctx, stmt, itr);
}

static int isVARN(kTerm* tk)
{
	return ((TT_(tk) == TT_NAME && !Term_isDOT(tk)) || TT_(tk) == TT_ASIS);
}

static void _PNAME(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kString *s = NULL;
	if(itr->c + 1 == itr->e) {
		kTerm *tk = itr->ts[itr->c];
		if(TT_(tk) == TT_NAME || TT_(tk) == TT_UNAME) {
			s = (tk)->text;
		}
	}
	if(s != NULL) {
		kTerm *tkN = new_Term(_ctx, TT_CONST);
		KSETv((tkN)->data, s);
		SP(tkN)->type = TY_String;
		Term_setPNAME(tkN, 1);
		knh_Stmt_add(_ctx, stmt, tkN);
	}
}

static void _EXPRs(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_SEMICOLON)) return;
	do {
		int idx = ITR_indexTT(itr, TT_COMMA, itr->e);
		tkitr_t ebuf, *eitr = ITR_first(itr, idx, &ebuf, +1);
		if(ITR_size(eitr) != 0) {
			if(STT_(stmt) == STT_PRINT) { /* @TEST print a */
				if(TT_(ITR_tk(eitr)) == TT_DOTS) {
					Stmt_setBreakPoint(stmt, 1);
					break;
				}
				_PNAME(_ctx, stmt, eitr);
			}
			_EXPR(_ctx, stmt, eitr);
		}
	}
	while(ITR_hasNext(itr));
}

static kindex_t ITR_indexTOUNTIL(tkitr_t *itr)
{
	kindex_t i;
	for(i = itr->c; i < itr->e; i++) {
		kterm_t tt = TT_(itr->ts[i]);
		if(tt == TT_TO || tt == TT_UNTIL) return i;
		if(tt == TT_ITR || tt == TT_COLON) { /* a[1..n] as a [1 until n] */
			TT_(itr->ts[i]) = TT_UNTIL;
			return i;
		}

	}
	return -1;
}

static void _NULLEXPRs(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	do {
		int idx = ITR_indexTT(itr, TT_COMMA, itr->e);
		tkitr_t ebuf, *eitr = ITR_first(itr, idx, &ebuf, +1);
		if(ITR_size(eitr) == 0) {
			knh_Stmt_add(_ctx, stmt, new_Term(_ctx, TT_NULL));
//			_ASIS(_ctx, stmt, eitr);
		}
		else {
			_EXPR(_ctx, stmt, eitr);
		}
	}
	while(ITR_hasNext(itr));
	(itr)->c = (itr)->e - 1;
	if(ITR_is(itr, TT_COMMA)) {
		  knh_Stmt_add(_ctx, stmt, new_Term(_ctx, TT_NULL));
//		  _ASIS(_ctx, stmt, itr);
	}
}

static int ITR_indexKEY(tkitr_t *itr, int shift)
{
	kTerm **ts = itr->ts;
	int i;
	for(i = itr->c + shift; i < itr->e - 2; i++) { /* BEGIN */
		if(TT_(ts[i+1]) == TT_COLON && IS_bString((ts[i])->text)) {
			return i;
		}
	}
	return itr->e;
}

static void _ARRAY(CTX, kStmtExpr *stmt, kmethodn_t mn, kcid_t cid, tkitr_t *itr)
{
	kTerm *tkC = new_TermCID(_ctx, cid);
	DBG_ASSERT(STT_(stmt) == STT_NEW);
	knh_Stmt_add(_ctx, stmt, new_TermMN(_ctx, mn));
	knh_Stmt_add(_ctx, stmt, tkC);
	//Term_setImmutable(tkC, ITR_isImmutable(itr));
	_EXPRs(_ctx, stmt, itr);
}

static kStmtExpr *Stmt_addNewStmt(CTX, kStmtExpr *stmt, kterm_t stt)
{
	kStmtExpr *newstmt = new_Stmt2(_ctx, stt, NULL);
	knh_Stmt_add(_ctx, stmt, newstmt);
	return newstmt;
}

static void _KEYVALUEs(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	int p = ITR_indexKEY(itr, +0), p_next;
	while(p < itr->e) {
		tkitr_t ebuf, *eitr = ITR_copy(itr, &ebuf, 0);
		eitr->c = p;
		p_next = ITR_indexKEY(eitr, +2);
		eitr->e = p_next;
		ITR_chop(eitr, TT_SEMICOLON);
		ITR_chop(eitr, TT_COMMA);
		if(ITR_size(eitr) > 2) {
			kTerm *tk = ITR_nextTK(eitr);
			TT_(tk) = TT_STR;
			knh_Stmt_add(_ctx, stmt, tk);
			DBG_ASSERT(ITR_is(eitr, TT_COLON));
			ITR_next(eitr);
			if(!ITR_hasNext(eitr)) {
				knh_Stmt_add(_ctx, stmt, new_Term(_ctx, TT_NULL));
				return;
			}
			if(ITR_count(eitr, TT_COMMA) == 0) {
				_EXPR(_ctx, stmt, eitr);
			}
			else {
				kStmtExpr *stmtARRAY = Stmt_addNewStmt(_ctx, stmt, STT_NEW);
				_ARRAY(_ctx, stmtARRAY, MN_newLIST, CLASS_Array, eitr);
			}
		}
		p = p_next;
	}
}

static void _DICT(CTX, kStmtExpr *stmt, kTerm *tkC, kTerm *tkB)
{
	DBG_ASSERT(STT_(stmt) == STT_NEW);
	if(TT_(tkB) == TT_CODE) {
		Term_toBRACE(_ctx, tkB, 1/*isEXPANDING*/);
	}
	knh_Stmt_add(_ctx, stmt, tkC);
	DBG_ASSERT(TT_(tkB) == TT_BRACE);
	{
		tkitr_t pbuf, *pitr = ITR_new(tkB, &pbuf);
		//Term_setImmutable(tkC, ITR_isImmutable(pitr));
		_KEYVALUEs(_ctx, stmt, pitr);
	}
}

static void Term_setTYPEOFEXPR(CTX, kTerm *tm)
{
	DBG_ASSERT(TT_(tm) == TT_TYPEOF);
	tkitr_t tbuf, *titr = ITR_new(tm->token, &tbuf);
	if(ITR_hasNext(titr)) {
		kStmtExpr *stmt = new_Stmt2(_ctx, STT_CALL1, NULL);
		_EXPR(_ctx, stmt, titr);
		KSETv((tm)->data, stmt);
	}
	else {
		TT_(tm) = TT_VOID;
		KSETv((tm)->data, ClassTBL(CLASS_Tvoid)->sname);
	}
}

static kStmtExpr *new_StmtREUSE(CTX, kStmtExpr *stmt, kterm_t stt)
{
	if(STT_(stmt) == STT_CALL1 || (STT_(stmt) == STT_CALL && DP(stmt)->size == 0)) {
		STT_(stmt) = stt;
		return stmt;
	}
	else {
		return Stmt_addNewStmt(_ctx, stmt, stt);
	}
}

/* ------------------------------------------------------------------------ */
/* EXPR */

kshort_t TT_to(kterm_t tt);

static int ITR_indexLET(tkitr_t *itr)
{
	kTerm **ts = itr->ts; int i;
	for(i = itr->c; i < itr->e; i++) {
		kterm_t tt = TT_(ts[i]);
		if(tt == TT_LET || tt == TT_DECLLET) return i;
		if(TT_DECLLET < tt && tt < TT_LET) {
			TT_(ts[i]) = TT_to(tt);
			return i;
		}
	}
	return -1;
}

static void _EXPRLET(CTX, kStmtExpr *stmt, tkitr_t *itr, kindex_t idx)
{
	kTerm *tkCUR = itr->ts[idx];
	tkitr_t lbuf, *litr = ITR_first(itr, idx, &lbuf, +1);
	stmt = new_StmtREUSE(_ctx, stmt, STT_LET);
	_VAR(_ctx, stmt, litr); // var is the first element
	if(TT_(tkCUR) != TT_LET) { /* i += 1 ==> i = i + 1 */
		int c = litr->c, e = litr->e;
		itr->c = litr->c;
		_EXPR(_ctx, stmt, litr);
		while(c < e) {
			kTerm *otk = itr->ts[c];
			kTerm *ntk = new_Term(_ctx, TT_(otk));
			KSETv(ntk->data, otk->data);
			ntk->uline = otk->uline;
			ntk->flag0 = otk->flag0;
			ntk->index = otk->index;
			KSETv(itr->ts[c], ntk);
			c++;
		}
	}
	else {
		_EXPR(_ctx, stmt, litr);
	}
	_EXPR(_ctx, stmt, itr);
}

static int ITR_isDOTNAME(tkitr_t *itr, int shift)
{
	int c = itr->c + shift;
	if(c < itr->e) {
		kTerm *tk = itr->ts[c];
		if(!Term_isDOT(tk)) return 0;
		if(TT_(tk) == TT_NAME || TT_(tk) == TT_UNAME ||
			TT_(tk) == TT_FUNCNAME || TT_(tk) == TT_UFUNCNAME) return 1;
	}
	return 0;
}

static void ITR_CASTOP(tkitr_t *itr)
{
	kTerm *tk = itr->ts[itr->c+1];
	if(TT_(tk) == TT_SUB) TT_(tk) = TT_TSUB;
	else if(TT_(tk) == TT_ADD) TT_(tk) = TT_TADD;
	else if(TT_(tk) == TT_AND) TT_(tk) = TT_TAND;
	else if(TT_(tk) == TT_MUL) TT_(tk) = TT_TMUL;
}

static int ITR_isCAST(tkitr_t *itr)
{
	DBG_ASSERT(itr->c + 1 < itr->e);
	if(ITR_is(itr, TT_PARENTHESIS) && !ITR_isDOTNAME(itr, 1)) {
		tkitr_t cbuf, *citr = ITR_new(ITR_tk(itr), &cbuf);
		if(ITR_is(citr, TT_TO)) {
			ITR_next(citr)
		}
		if((ITR_isT(citr, isTYPE) || ITR_is(itr, TT_MUL)) && ITR_size(citr) == 1) {
			ITR_CASTOP(itr);
			return 1;
		}
	}
	else if(ITR_is(itr, TT_TLINK)) {
		ITR_CASTOP(itr);
		return 1;
	}
	return 0;
}

static void _EXPRCAST(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kStmtExpr *stmtT = new_StmtREUSE(_ctx, stmt, STT_TCAST);
	DBG_ASSERT(itr->c + 1 < itr->e);
	if(ITR_is(itr, TT_TLINK)) {
		kTerm *tkTL = ITR_nextTK(itr);
		STT_(stmtT) = STT_TLINK;
		_ASIS(_ctx, stmtT, itr);
		knh_Stmt_add(_ctx, stmtT, tkTL);
		_EXPR(_ctx, stmtT, itr);
	}
	else {
		tkitr_t cbuf, *citr = ITR_new(ITR_nextTK(itr), &cbuf);
		if(ITR_is(citr, TT_TO)) {
			Stmt_setTRANS(stmtT, 1);
			ITR_next(citr)
		}
		if(ITR_isT(citr, isTYPE)) {
			knh_Stmt_add(_ctx, stmtT, ITR_nextTK(citr));
		}
		if(ITR_is(citr, TT_MUL)) {
			knh_Stmt_add(_ctx, stmtT, new_TermCID(_ctx, CLASS_Tdynamic));
		}
		_EXPR(_ctx, stmtT, itr);
	}
}

int TT_priority(kterm_t tt);

static int ITR_indexOPR(CTX, tkitr_t *itr, int offset)
{
	int i, oppri = 98, idx = -1;
	kterm_t tt_found = TT_EOT;
	kTerm **ts = itr->ts;
	int isTEXTPARAM = 0;
	for(i = itr->c + offset; i < itr->e; i++) {
		if(TT_(ts[i]) == TT_NAME || TT_(ts[i]) == TT_UNAME) {
			isTEXTPARAM = 1;
			continue;
		}
		if(isTEXTPARAM == 1 && TT_isSTR(TT_(ts[i]))) {
			return -1;
		}
		break;
	}
	for(i = itr->c; i < itr->e; i++) {
		kterm_t tt = TT_(ts[i]);
		int p = TT_priority(tt);
		if(p == 0) {
			DBG_P("p=%d, TT=%s", TT__(tt));
			KSETv(ts[i], ERROR_Term(_ctx, ts[i] K_TRACEPOINT));
			itr->c = i;
			return i;
		}
		if(p <= oppri) {
			oppri = p;
			tt_found = tt;
		}
	}
	if(tt_found != TT_EOT) {
		for(i = itr->c; i < itr->e; i++) {
			if(TT_(ts[i]) == tt_found) {
				idx = i; /* last idx */
				if(!TT_isBINARY(tt_found)) break; /* fisrt idx */
			}
		}
	}
	//DBG_P("idx=%d=<%d<%d %s", itr->c, idx, itr->e, TT__(ts[idx]->tt));
	return idx;
}

static void ITR_toLET(CTX, tkitr_t *itr, kTerm *tk)
{
	int i;
	kTerm *tkC = NULL;
	KSETv((tk)->data, new_Array0(_ctx, 0));
	for(i = itr->c; i < itr->e; i++) {
		tkC = new_Term(_ctx, TT_(itr->ts[i]));
		KSETv((tkC)->data, (itr->ts[i])->data);
		(tkC)->index = (itr->ts[i])->index;
		kArray_add((tk)->list, tkC);
	}
	kArray_add((tk)->list, new_Term(_ctx, TT_LET));
	for(i = itr->c; i < itr->e; i++) {
		tkC = new_Term(_ctx, TT_(itr->ts[i]));
		KSETv((tkC)->data, (itr->ts[i])->data);
		(tkC)->index = (itr->ts[i])->index;
		kArray_add((tk)->list, tkC);
	}
	tkC = new_Term(_ctx, (TT_(tk)==TT_NEXT) ? TT_ADD : TT_SUB);
	kArray_add((tk)->list, tkC);
	tkC = new_Term(_ctx, TT_NUM);
	KSETv((tkC)->data, TS_ONE);
	kArray_add((tk)->list, tkC);
}

static void _EXPROP(CTX, kStmtExpr *stmt, tkitr_t *itr, int idx)
{
	kterm_t stt = STT_OPR;
	kTerm *tkOP = itr->ts[idx];
	kterm_t tt = TT_(tkOP);
	if(ITR_size(itr) == 1) {
		Stmt_toSyntaxError(_ctx, stmt, tkOP K_TRACEPOINT);
		return;
	}
	switch(tt){
	case TT_EXISTS: /* @CODE: exists expr */
	case TT_LNOT:   /* @CODE: ~expr */
	case TT_NOT:    /* @CODE: not expr */
	case TT_TSUB:   /* @CODE: -expr */
	case TT_TADD:   /* @CODE: +expr */
	case TT_TAND:   /* @CODE: &expr */ {
		if(itr->c == idx) {
			kStmtExpr *stmtOPR = new_StmtREUSE(_ctx, stmt, stt);
			knh_Stmt_add(_ctx, stmtOPR, tkOP);
			ITR_next(itr);
			_EXPR(_ctx, stmtOPR, itr);
		}
		else {
			Stmt_toSyntaxError(_ctx, stmt, tkOP K_TRACEPOINT);
		}
		return;
	}
	case TT_NEXT: /* @CODE: expr++, ++expr */
	case TT_PREV: /* @CODE: expr--, --expr */ {
		tkitr_t lbuf, *litr;
		if(ITR_is(itr, tt)) { /* @CODE: ++expr */
			itr->c += 1;
			ITR_toLET(_ctx, itr, tkOP);
			litr = ITR_new(tkOP, &lbuf);
			_EXPR(_ctx, stmt, litr);
		}
		else { /* @CODE expr++ */
			int c = itr->c, e = itr->e -1;
			itr->e -= 1;
			if(STT_(stmt) != STT_CALL1) {
				stmt = Stmt_addNewStmt(_ctx, stmt, STT_CALL1);
			}
			_EXPR(_ctx, stmt, itr);
			if(STT_(stmt) != STT_ERR) {
				kStmtExpr *stmtPOST = new_Stmt2(_ctx, STT_CALL1, NULL);
				KSETv(DP(stmt)->stmtPOST, stmtPOST);
				itr->c = c; itr->e = e;
				ITR_toLET(_ctx, itr, tkOP);
				litr = ITR_new(tkOP, &lbuf);
				_EXPR(_ctx, stmtPOST, litr);
			}
		}
		return;
	}
	case TT_QTN: {/* @CODE: expr ? expr : expr */
		int idx2 = ITR_indexTT(itr, TT_COLON, -1);
		if(idx2 != -1) {
			stmt = new_StmtREUSE(_ctx, stmt, STT_TRI);
			TT_(tkOP) = TT_COMMA;
			TT_(itr->ts[idx2]) = TT_COMMA;
			_EXPRs(_ctx, stmt, itr);
		}
		else {
			kStmtExproERR(_ctx, stmt, ERROR_Required(_ctx, tkOP, _("trinary"), ":"));
		}
		return;
	}
	case TT_AND: stt = STT_AND; goto L_OPR;
	case TT_OR:  stt = STT_OR; goto L_OPR;
	case TT_ALT: stt = STT_ALT; goto L_OPR;
	default: {
		L_OPR:;
		kStmtExpr *stmtOPR = new_StmtREUSE(_ctx, stmt, stt);
		if(stt == STT_OPR) {
			knh_Stmt_add(_ctx, stmtOPR, tkOP);
		}
		if(TT_isBINARY(tt)) {
			TT_(tkOP) = TT_COMMA;
		}
		else {
			ITR_replace(itr, tt, TT_COMMA);
		}
		DBG_ASSERT(!ITR_is(itr, TT_COMMA));
		_EXPRs(_ctx, stmtOPR, itr);
		TT_(tkOP) = tt;
		return;
		}
	}
}

void knh_Regex_setGlobalOption(CTX, kRegex *re, const char *opt)
{
	const char *p = opt;
	while(*p != 0) {
		if(*p == 'g') {
			Regex_setGlobalOption(re, 1);
			break;
		}
		p++;
	}
}

static void _REGEX(CTX, kStmtExpr *stmt, tkitr_t *itr, kTerm *tk)
{
	kRegex *re = new_H(Regex);
	const char *opt = "";
	int cflags = 0;
	KINITv(re->pattern, (tk)->text);
	re->spi = knh_getRegexSPI();
	if(re->spi == knh_getStrRegexSPI()) {
		WarningNotInitialized(_ctx, tk, "regex");
	}
	re->reg = re->spi->regmalloc(_ctx, (tk)->text);
	if(ITR_is(itr, TT_NAME)) {
		opt = S_text((ITR_nextTK(itr))->text);
		knh_Regex_setGlobalOption(_ctx, re, opt);
	}
	cflags = re->spi->parse_cflags(_ctx, opt);
	if(re->spi->regcomp(_ctx, re->reg, S_text(re->pattern), cflags) != 0) {
		kStmtExproERR(_ctx, stmt, ERROR_RegexCompilation(_ctx, tk, re->spi->name, S_text((tk)->text)));
	}
	else {
		re->eflags = re->spi->parse_eflags(_ctx, opt);
		KSETv((tk)->data, re);
		knh_Stmt_add(_ctx, stmt, tk);
		ITR_ignore(_ctx, itr, +0);
	}
}

static void _CODEDOC(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(STT_(stmt) == STT_ERR) return;
	kTerm *tkDOC = new_Term(_ctx, TT_DOC);
	if(ITR_is(itr, TT_CODE)) {
		KSETv((tkDOC)->data, (ITR_tk(itr))->text);
	}
	else {
		KSETv((tkDOC)->data, TS_EMPTY);
	}
	knh_Stmt_add(_ctx, stmt, tkDOC);
}

static void _RETURNEXPR(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kStmtExpr *stmtRTN = new_Stmt2(_ctx, STT_RETURN, NULL);
	knh_Stmt_add(_ctx, stmt, stmtRTN);
	if(ITR_hasNext(itr)) {
		_EXPR(_ctx, stmtRTN, itr);
	}
}

static void _PARAM(CTX, kStmtExpr *stmt, tkitr_t *itr);
static void _PARAMs(CTX, kStmtExpr *stmt, tkitr_t *itr);
static void _STMT1(CTX, kStmtExpr *stmt, tkitr_t *itr);

static void _EXPR1(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	//if(ITR_hasNext(itr)) {
		kTerm *tkCUR = ITR_nextTK(itr);
		switch(TT_(tkCUR)) {
			case TT_NAME:    /* @CODE: name */
			case TT_UNAME:   /* @CODE: NAME */
				DBG_ASSERT(!ITR_hasNext(itr));  // to avoid name. hoge
			case TT_NULL:    /* @CODE: null */
			case TT_TRUE:    /* @CODE: true */
			case TT_FALSE:   /* @CODE: false */
			case TT_PTYPE:    /* @CODE: T<T> */
			case TT_PROPN:   /* @CODE: $NAME */
			case TT_TYPEOF:  /* @CODE: typeof(expr) */
			case TT_STR:     /* @CODE: "hoge" */
			case TT_TSTR:    /* @CODE: 'hoge' */
			case TT_ESTR:    /* @CODE: `hoge` */
			case TT_NUM:     /* @CODE: 123m */
			case TT_URN:     /* @CODE: URL */
			case TT_TLINK:   /* @CODE: link:: */
			case TT_DYN:
				knh_Stmt_add(_ctx, stmt, tkCUR);
				break;
			case TT_BYTE:    /* @CODE: byte */
				TT_(tkCUR) = TT_NAME;
				knh_Stmt_add(_ctx, stmt, tkCUR);
				break;
			case TT_REGEX:
				_REGEX(_ctx, stmt, itr, tkCUR);
				break;
			case TT_PARENTHESIS: /* @CODE: () */ {
				tkitr_t pbuf, *pitr = ITR_new(tkCUR, &pbuf);
				int c = ITR_count(pitr, TT_COMMA);
				if(c == 0) {
					if(ITR_hasNext(pitr)) {   /* @CODE: (expr) => expr */
						_EXPR(_ctx, stmt, pitr);
					}
					else { /* @CODE: () => null */
						TT_(tkCUR) = TT_NULL;
						knh_Stmt_add(_ctx, stmt, tkCUR);
					}
				}
				else {  /* @CODE: (1, 2) */
					stmt = new_StmtREUSE(_ctx, stmt, STT_NEW);
					_ARRAY(_ctx, stmt, MN_newTUPLE, CLASS_Tuple, pitr);
				}
				break;
			}
			case TT_BRANCET: {  /* @CODE: [] */
				tkitr_t pbuf, *pitr = ITR_new(tkCUR, &pbuf);
				kcid_t cid = CLASS_Array;
				kindex_t idx = ITR_indexTT(pitr, TT_TO, -1);
				if(idx != -1) { /* [1 to 2] => [1, 2] */
					cid = CLASS_Range;
					TT_(pitr->ts[idx]) = TT_COMMA;
				}
				stmt = new_StmtREUSE(_ctx, stmt, STT_NEW);
				_ARRAY(_ctx, stmt, MN_newLIST, cid, pitr);
				break;
			}
			case TT_CODE: {
				kbytes_t t = S_tobytes(tkCUR->text);
				if(t.len == 0 || knh_bytes_index(t, ':') != -1) {
					stmt = new_StmtREUSE(_ctx, stmt, STT_NEW);
					knh_Stmt_add(_ctx, stmt, new_TermMN(_ctx, MN_newMAP));
					_DICT(_ctx, stmt, new_TermCID(_ctx, CLASS_Map), tkCUR);
				}
				else {
					stmt = new_StmtREUSE(_ctx, stmt, STT_FUNCTION);
					_ASIS(_ctx, stmt, itr);
					knh_Stmt_add(_ctx, stmt, new_Stmt2(_ctx, STT_DECL, NULL));
					kTerm *tkDOC = new_Term(_ctx, TT_DOC);
					KSETv((tkDOC)->data, (tkCUR)->text);
					knh_Stmt_add(_ctx, stmt, tkDOC);
					itr->c -= 1;
					_STMT1(_ctx, stmt, itr);
					DBG_P("SIZE=%d", DP(stmt)->size);
				}
				break;
			}
			case TT_ERR:
				kStmtExproERR(_ctx, stmt, tkCUR);
				break;
			default: {
				Stmt_toSyntaxError(_ctx, stmt, tkCUR K_TRACEPOINT);
			}
		}
//	}
//	else {
//		kStmtExproERR(_ctx, stmt, ERROR_text(_ctx, TT__(stmt->stt) K_TRACEPOINT));
//	}
}

static kStmtExpr *Stmt_addFUNC(CTX, kStmtExpr *stmt, kTerm *tkF)
{
	size_t size = DP(stmt)->size;
	if(TT_(tkF) == TT_NAME || TT_(tkF) == TT_UNAME) {
		Term_setGetter(tkF, 1);
	}
	if(size == 0) {
		DBG_ASSERT(STT_(stmt) == STT_CALL);
		knh_Stmt_add(_ctx, stmt, tkF);
	}
	else if(DP(stmt)->size == 1) {
		DBG_ASSERT(STT_(stmt) == STT_CALL);
		knh_Stmt_add(_ctx, stmt, tkF);
		knh_Stmt_swap(_ctx, stmt, 0, 1);
	}
	else {
		kStmtExpr *stmtIN = new_Stmt2(_ctx, STT_(stmt), NULL);
		size_t i;
		for(i = 0; i < DP(stmt)->size; i++) {
			knh_Stmt_add(_ctx, stmtIN, tmNN(stmt, i), NULL);
		}
		knh_Stmt_done(_ctx, stmt);
		STT_(stmt) = STT_CALL;
		knh_Stmt_add(_ctx, stmt, tkF, stmtIN);
	}
	if(TT_(tkF) == TT_ASIS) {
		STT_(stmt) = STT_FUNCCALL;
		knh_Stmt_swap(_ctx, stmt, 0, 1);  // (_ f) = > (f _)
	}
	DBG_ASSERT(DP(stmt)->size == 2);
	return stmt;
}

#define Stmt_addFUNCMN(_ctx, stmt, mn) Stmt_addFUNC(_ctx, stmt, new_TermMN(_ctx, mn))

static void _CALLPARAM(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	while(ITR_hasNext(itr)) {
		while(ITR_isDOTNAME(itr, 0)) {
			stmt = Stmt_addFUNC(_ctx, stmt, ITR_nextTK(itr));
		}
		if(ITR_is(itr, TT_BRANCET)) {   /* @CODE: expr[...] */
			tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
			int idx = ITR_indexTOUNTIL(pitr);
			if(idx == -1) {
				kmethodn_t mn = MN_get;
				switch(ITR_count(pitr, TT_COMMA)) {
					case 1: mn = MN_get2; break;  /* @CODE expr[x,y] */
					case 2: mn = MN_get3; break;  /* @CODE expr[x,y,z] */
					case 3: mn = MN_get4; break;  /* @CODE expr[x,y,z,w] */
				}
				stmt = Stmt_addFUNCMN(_ctx, stmt, mn);
				_EXPRs(_ctx, stmt, pitr);
			}
			else {
				kmethodn_t mn = (TT_(pitr->ts[idx]) == TT_TO) ? MN_opTO : MN_opUNTIL;
				stmt = Stmt_addFUNCMN(_ctx, stmt, mn);
				TT_((pitr)->ts[idx]) = TT_COMMA;  // replace a, b
				_NULLEXPRs(_ctx, stmt, pitr);
			}
			continue;
		}
		if(DP(stmt)->size != 2) {
			stmt = Stmt_addFUNC(_ctx, stmt, new_(Term) /*ASIS*/);
		}
		if(ITR_is(itr, TT_PARENTHESIS)) {
			tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
			_EXPRs(_ctx, stmt, pitr);
		}
		else if(ITR_hasNext(itr)) { /* CODE: f n + 1 =>  f (n+1) */
			Term_setGetter(tkNN(stmt, 0), 0);  // name.f a
			_EXPR(_ctx, stmt, itr);
			return;
		}
	}
}

static void _EXPRCALL(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kterm_t tt = ITR_tt(itr);
	kTerm *tkCUR = ITR_nextTK(itr);
	int isCALL = 0;
	DBG_ASSERT(ITR_hasNext(itr));
	stmt = new_StmtREUSE(_ctx, stmt, STT_CALL);
	switch(tt) {
		case TT_STR: case TT_TSTR: // "%s"(a)
		case TT_FUNCNAME: // f()
		case TT_UFUNCNAME: // F()
		case TT_UNAME:
		case TT_NAME:  // a .f .c
			knh_Stmt_add(_ctx, stmt, tkCUR);
			isCALL = 1;
			break;
		case TT_REGEX: {
			_REGEX(_ctx, stmt, itr, tkCUR);
			break;
		}
		case TT_FUNCTION: {
			STT_(stmt) = STT_FUNCTION;
			if(ITR_is(itr, TT_FUNCNAME) || ITR_is(itr, TT_UFUNCNAME)) {
				kTerm *tkN = ITR_nextTK(itr);
				WARN_Ignored(_ctx, _("function name"), CLASS_unknown, S_text(tkN->text));
			}
			if(ITR_is(itr, TT_PARENTHESIS) && ITR_isN(itr, +1, TT_CODE)) {
				tkCUR = new_Term(_ctx, TT_DOC);
				KSETv((tkCUR)->data, (ITR_tk(itr))->text);
				_ASIS(_ctx, stmt, itr);
				_PARAM(_ctx, stmt, itr);
				knh_Stmt_add(_ctx, stmt, tkCUR);
				_STMT1(_ctx, stmt, itr);
			}
			else {
				kStmtExproERR(_ctx, stmt, ERROR_text(_ctx, "syntax error: function() {...}" K_TRACEPOINT));
				return;
			}
			break;
		}
		case TT_NEW: {
			STT_(stmt) = STT_NEW;
			isCALL = 1;
			knh_Stmt_add(_ctx, stmt, tkCUR);
			if(ITR_is(itr, TT_UFUNCNAME) /* new T() */ /* thanks, ide */
				|| (ITR_isT(itr, isTYPE) && ITR_isN(itr, +1, TT_PARENTHESIS)) /* new T<T>() */) {
				knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
				break;
			}
			if(ITR_is(itr, TT_PARENTHESIS)) { /* new () */
				_ASIS(_ctx, stmt, itr);
				break;
			}
			if(ITR_is(itr, TT_BYTE) && ITR_isN(itr, +1, TT_BRANCET)) {
				TT_(tkCUR) = TT_MN; /* new byte[10] */
				(tkCUR)->mn = MN_newARRAY;
				knh_Stmt_add(_ctx, stmt, new_TermCID(_ctx, CLASS_Bytes));
				ITR_nextTK(itr);
				TT_(itr->ts[itr->c]) = TT_PARENTHESIS;
				break;
			}
			if(ITR_isT(itr, isTYPE) && ITR_isN(itr, +1, TT_BRANCET)) {
				kTerm *tkC = ITR_nextTK(itr);
				TT_(tkCUR) = TT_MN; /* new C[10] */
				(tkCUR)->mn = MN_newARRAY;
				knh_Stmt_add(_ctx, stmt, new_TermPTYPE(_ctx, CLASS_Array, tkC));
				TT_(itr->ts[itr->c]) = TT_PARENTHESIS;
				break;
			}
			if(ITR_is(itr, TT_BRANCET)) { /* new [] */
				TT_(tkCUR) = TT_MN; /* new [10] */
				(tkCUR)->mn = MN_newARRAY;
				_ASIS(_ctx, stmt, itr);
				TT_(itr->ts[itr->c]) = TT_PARENTHESIS;
				break;
			}
			if(ITR_isT(itr, isTYPE) && ITR_isN(itr, +1, TT_CODE)) {
				kTerm *tkC = ITR_nextTK(itr);
				TT_(tkCUR) = TT_MN; /* new C {} */
				(tkCUR)->mn = MN_newMAP;
				_DICT(_ctx, stmt, tkC, ITR_nextTK(itr));
				break;
			}
			if(ITR_is(itr, TT_CODE)) {
				(tkCUR)->mn = MN_newMAP;  /* new {} */
				_DICT(_ctx, stmt, new_Term(_ctx, TT_ASIS), ITR_nextTK(itr));
				break;
			}
			if(ITR_is(itr, TT_URN)) {    /* new file:text.txt */
				STT_(stmt) = STT_TLINK;
				knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
				_ASIS(_ctx, stmt, itr);
				break;
			}
			if(ITR_is(itr, TT_TLINK)) {    /* new file:: text.txt */
				STT_(stmt) = STT_TLINK;
				WARN_Unnecesary(_ctx, tkCUR);
				knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
				_EXPR(_ctx, stmt, itr);
				break;
			}
			kStmtExproERR(_ctx, stmt, ERROR_Undefined(_ctx, "class for new", CLASS_unknown, ITR_nextTK(itr)/*tkCUR*/));
			return;
		}/*TT_NEW*/
		default: {
			itr->c -= 1;
			_EXPR1(_ctx, stmt, itr);
		}
	}
	if(isCALL || ITR_isDOTNAME(itr, 0) || ITR_is(itr, TT_PARENTHESIS) || ITR_is(itr, TT_BRANCET)) {
		_CALLPARAM(_ctx, stmt, itr);
	}
	else {
		if(STT_(stmt) != STT_FUNCTION) {
			STT_(stmt) = STT_CALL1;
		}
		if(ITR_hasNext(itr)) {
			WARN_Unnecesary(_ctx, itr->ts[itr->c]);
			itr->c = itr->e;
		}
	}
}

static int IS_BIN(kterm_t tt)
{
	return (TT_isBINARY(tt) || tt == TT_AND || tt == TT_OR);
}

static void _EXPR(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	size_t n = ITR_size(itr);
	if(n == 1) {
		_EXPR1(_ctx, stmt, itr);
	}
	else if(n > 1) {
		int idx = ITR_indexLET(itr);
		if(idx != -1) { /* a = b */
			_EXPRLET(_ctx, stmt, itr, idx);
			return;
		}
		int isCAST = ITR_isCAST(itr);
		idx = ITR_indexOPR(_ctx, itr, 0);
		if(idx != -1 && (!isCAST || IS_BIN(TT_(itr->ts[idx])))) {
			_EXPROP(_ctx, stmt, itr, idx);
		}
		else if(isCAST) {
			_EXPRCAST(_ctx, stmt, itr);
		}
		else {  /*f -1*/
			_EXPRCALL(_ctx, stmt, itr);
		}
	}
	else {
		_ASIS(_ctx, stmt, itr);
	}
}

typedef void (*knh_Fstmt)(CTX, kStmtExpr *stmt, tkitr_t *itr);

static void _CHECKEXPR(CTX, kStmtExpr *stmt, tkitr_t *itr, int s, int e)
{
	kTerm **ts = itr->ts;
	int i;
	DBG_ASSERT(itr->c <= s && e <= itr->e);
	for(i = s; i < e; i++) {
		kterm_t tt = TT_(ts[i]);
		if(tt <= TT_ASIS) {
			kStmtExproERR(_ctx, stmt, ERROR_TermIsNot(_ctx, ts[i], _("value")));
			itr->e = i;
			return;
		}
	}
}

static void _SAFEEXPR(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kTerm **ts = itr->ts;
	int i;
	for(i = itr->c; i < itr->e; i++) {
		kterm_t tt = TT_(ts[i]);
		if(tt <= TT_ASIS) {
			kStmtExproERR(_ctx, stmt, ERROR_TermIsNot(_ctx, ts[i], _("value")));
			itr->e = i;
			return;
		}
	}
	_EXPR(_ctx, stmt, itr);
}

static void _PEXPR(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_PARENTHESIS)) {
		tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
		_SAFEEXPR(_ctx, stmt, pitr);
	}
	else {
		_ERROR(_ctx, stmt, itr, "()");
	}
}

static void _ONEEXPR(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	int e = itr->e;
	itr->e = itr->c + 1;
	_EXPR(_ctx, stmt, itr);
	itr->e = e;
}

/* ------------------------------------------------------------------------ */

static void _DECL2(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	DBG_ASSERT(ITR_isT(itr, isTYPE));
	kTerm *tkT = ITR_nextTK(itr);
	knh_Stmt_add(_ctx, stmt, tkT);
	while(ITR_hasNext(itr)) {
		int idx = ITR_indexTT(itr, TT_COMMA, itr->e);
		tkitr_t abuf, *aitr = ITR_first(itr, idx, &abuf, +1);
		_CHECKEXPR(_ctx, stmt, aitr, aitr->c, aitr->e);
		if(STT_(stmt) == STT_ERR) return;

		DBG_ASSERT(isVARN(ITR_tk(aitr)));  // already checked
		knh_Stmt_add(_ctx, stmt, ITR_nextTK(aitr));

		if(ITR_is(aitr, TT_BRANCET)) {  /* @ac(DeclareCStyleArray) */
			WARN_MuchBetter(_ctx, "Type[]", NULL);  // int a[] // C-sytle array
			KSETv(tmNN(stmt, 0), new_TermPTYPE(_ctx, CLASS_Array, tkT));
			ITR_next(aitr);
		}
		if(ITR_is(aitr, TT_LET)) {
			ITR_next(aitr);
			_EXPR(_ctx, stmt, aitr);
		}
		else {
			_ASIS(_ctx, stmt, aitr);
		}
		if(ITR_hasNext(itr)) {
			kStmtExpr *stmtDECL = new_Stmt2(_ctx, STT_DECL, tkT, NULL);
			KINITv(DP(stmt)->nextNULL, stmtDECL);
			stmt = stmtDECL;
		}
	}
}

static void _PARAMs(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kStmtExpr *stmtP = new_Stmt2(_ctx, STT_DECL, NULL);
	int e = itr->e, ridx = ITR_indexTT(itr, TT_DARROW, -1);  /* int n, int b */
	if(ridx != -1) itr->e = ridx;
	int varidx = ITR_indexTT(itr, TT_DOTS, -1);
	if(varidx != -1) {
		DBG_P("******** @Vargs **********");
		StmtMETHOD_setVARGs(stmt,  1);
		itr->e = varidx;
	}
	knh_Stmt_add(_ctx, stmt, stmtP);

	while(ITR_hasNext(itr)) {
		int idx = ITR_indexTT(itr, TT_COMMA, itr->e);
		tkitr_t abuf, *aitr = ITR_first(itr, idx, &abuf, +1);
		while(ITR_is(aitr, TT_METAN)) {
			DBG_P("TODO: parameter annotation");
			ITR_next(aitr);
		}
		if(ITR_is(aitr, TT_VOID)) {
			break;
		}
		if(ITR_isT(aitr, isTYPE)) {
			knh_Stmt_add(_ctx, stmtP, ITR_nextTK(aitr));
		}
		if(DP(stmtP)->size % 3 != 1) {
			_VAR(_ctx, stmtP, aitr);
		}
		if(ITR_isT(aitr, isVARN)) {
			kTerm *tkN = ITR_nextTK(aitr);
			knh_Stmt_add(_ctx, stmtP, tkN);
		}
		else {
			_ERROR(_ctx, stmt, aitr, _("variable"));
		}
		if(ITR_is(aitr, TT_LET)) {
			ITR_next(aitr);
			_EXPR(_ctx, stmtP, aitr);
		}
		else {
			_ASIS(_ctx, stmtP, aitr);
		}
	}
	if(ridx != -1) itr->e = e;
}

static void _PARAM(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_PARENTHESIS)) {
		tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
		int funcidx = ITR_indexTT(pitr, TT_DARROW, -1);
		_PARAMs(_ctx, stmt, pitr);
		if(funcidx != -1) {
			_CODEDOC(_ctx, stmt, itr);
			pitr->c = funcidx + 1;
			_RETURNEXPR(_ctx, stmt, pitr);
		}
	}
	else {
		_ERROR(_ctx, stmt, itr, _("argument definition"));
	}
}

static kbool_t Stmt_checkDepth(kStmtExpr *stmt, int depth, int max)
{
	size_t i;
	if(depth > max) {
		DBG_P("depth=%d > max=%d", depth, max);
		DBG_ASSERT(depth < max);
		return 1;
	}
	for(i = 0; i < DP(stmt)->size; i++) {
		kStmtExpr *stmt2 = stmtNN(stmt, i);
		if(IS_StmtExpr(stmt2)) {
			//DBG_P("CHILD %p stmt=%s", stmt2, TT__(STT_(stmt2)));
			if(Stmt_checkDepth(stmt2, depth+1, max)) return 1;
		}
	}
	return 0;
}

static kStmtExpr *Stmt_norm(CTX, kStmtExpr *stmt)
{
	kStmtExpr *stmtITR = stmt;
	kStmtExpr *stmtPREV = NULL, *stmtLAST = NULL;
	while(stmtITR != NULL) {
		//DBG_P("PARENT %p stmt=%s", stmtITR, TT__(STT_(stmtITR)));
		if(Stmt_checkDepth(stmtITR, 0, 64)) {
			kStmtExproERR(_ctx, stmtITR, NULL); // TODO
		}
		if(STT_(stmtITR) == STT_RETURN || STT_(stmtITR) == STT_THROW) {
			if(DP(stmtITR)->nextNULL != NULL) {
				KNH_FINALv(_ctx, DP(stmtITR)->nextNULL);
			}
			stmtLAST = NULL;
			break;
		}
		stmtPREV = stmtLAST;
		stmtLAST = stmtITR;
		stmtITR = DP(stmtITR)->nextNULL;
	}
	if(stmtLAST != NULL) {
		kcodemod->uline = stmtLAST->uline;
		//DBG_P("@@@@ STT=%s", TT__(stmtLAST->stt));
		if(STT_(stmtLAST) == STT_CALL1 && !IS_StmtExpr(DP(stmtLAST)->stmtPOST)) {
			STT_(stmtLAST) = STT_RETURN;
		}
//		else if(stmt_isExpr(STT_(stmtLAST)) && STT_(stmtLAST) != STT_LET) {
//			kStmtExpr *stmtRETURN = new_Stmt2(_ctx, STT_RETURN, stmtLAST, NULL);
//			if(stmtPREV != NULL) {
//				KSETv(DP(stmtPREV)->nextNULL, stmtRETURN);
//			}
//			else {
//				stmt = stmtRETURN;
//			}
//		}
	}
	return stmt;
}

static void _STMTs(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kStmtExpr *stmtHEAD = NULL, *stmtTAIL = NULL;
	while(ITR_hasNext(itr)) {
		kStmtExpr *stmtCUR = new_StmtSTMT1(_ctx, itr);
		if(stmtHEAD == NULL) {
			stmtHEAD = stmtCUR;
		}
		else {
			KINITv(DP(stmtTAIL)->nextNULL, stmtCUR);
		}
		stmtTAIL = stmtCUR;
		while(DP(stmtTAIL)->nextNULL != NULL) stmtTAIL = DP(stmtTAIL)->nextNULL;
	}
	if(stmtHEAD == NULL) {
		stmtHEAD = new_Stmt2(_ctx, STT_DONE, NULL);
	}
	knh_Stmt_add(_ctx, stmt, Stmt_norm(_ctx, stmtHEAD));
}

static void _STMT1(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_CODE)) {
		Term_toBRACE(_ctx, ITR_tk(itr), 1/*isEXPANDING*/);
	}
	if(ITR_is(itr, TT_BRACE)) {
		tkitr_t stmtbuf, *stmtitr = ITR_new(ITR_nextTK(itr), &stmtbuf);
		_STMTs(_ctx, stmt, stmtitr);
	}
	else {
//		knh_Stmt_add(_ctx, stmt, Stmt_norm(_ctx, new_StmtSTMT1(_ctx, itr)));
		DBG_P("itr->c =%d, TT=%s", itr->c, TT__(itr->ts[itr->c]->tt));
		knh_Stmt_add(_ctx, stmt, Stmt_norm(_ctx, new_StmtSTMT1(_ctx, itr)));
	}
}

static void _ELSE(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_ELSE)) {
		ITR_next(itr);
		_STMT1(_ctx, stmt, itr);
	}
	else {
		knh_Stmt_add(_ctx, stmt, new_Stmt2(_ctx, STT_DONE, NULL));
	}
}

static void _DOWHILE(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_CODE)) {
		Term_toBRACE(_ctx, ITR_tk(itr), 1/*isEXPANDING*/);
	}
	if(ITR_is(itr, TT_BRACE)) {
		tkitr_t stmtbuf, *stmtitr = ITR_new(ITR_nextTK(itr), &stmtbuf);
		_STMTs(_ctx, stmt, stmtitr);
		if(ITR_is(itr, TT_WHILE)) {
			ITR_next(itr);
		}
		return;
	}
	_ERROR(_ctx, stmt, itr, _("{} for while body"));
}

static void _CASESTMT(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kStmtExpr *stmtHEAD = NULL, *stmtLAST = NULL;
	tkitr_t stmtbuf;
	if(ITR_is(itr, TT_CODE)) {
		Term_toBRACE(_ctx, ITR_tk(itr), 1/*isEXPANDING*/);
	}
	if(ITR_is(itr, TT_BRACE)) {
		itr = ITR_new(ITR_nextTK(itr), &stmtbuf);
	}
	else {
		_ERROR(_ctx, stmt, itr, "{}");
		return;
	}
	while(ITR_hasNext(itr)) {
		kterm_t tt = TT_(ITR_nextTK(itr));
		kStmtExpr *stmtCASE;
		if(tt == TT_CASE || tt == TT_DEFAULT) {
			int idx = ITR_indexTT(itr, TT_COLON, -1);
			if(idx == -1) {
				_ERROR(_ctx, stmt, itr, ":");
				goto L_RETURN;
			}
			stmtCASE = new_Stmt2(_ctx, STT_CASE, NULL);
			if(stmtHEAD == NULL) {
				stmtHEAD = stmtCASE;
			}
			else {
				KINITv(DP(stmtLAST)->nextNULL, stmtCASE);
			}
			stmtLAST = stmtCASE;
			if(tt == TT_CASE) {
				int e = itr->e;
				itr->e = idx;
				_EXPR(_ctx, stmtCASE, itr);
				itr->e = e;
			}
			else {
				_ASIS(_ctx, stmtCASE, itr);
			}
			{
				int i, e = itr->e;
				itr->c = idx + 1;
				for(i = itr->c; i < itr->e; i++) {
					tt = TT_(itr->ts[i]);
					if(tt == TT_CASE || tt == TT_DEFAULT) {
						itr->e = i;
						break;
					}
				}
				_STMTs(_ctx, stmtCASE, itr);
				itr->c = i;
				itr->e = e;
			}
		}
		else {
			_ERROR(_ctx, stmt, itr, NULL);
			goto L_RETURN;
		}
	}
	if(stmtHEAD == NULL) {
		stmtHEAD = new_Stmt2(_ctx, STT_DONE, NULL);
	}
	L_RETURN:;
	if(stmtHEAD != NULL) {
		knh_Stmt_add(_ctx, stmt, stmtHEAD);
	}
}

static int ITR_indexINFROM(tkitr_t *itr)
{
	if(ITR_size(itr) > 2) {
		int i, c = itr->c + 1;
		if(TT_(itr->ts[c]) == TT_UNAME) c++;  // skip InputSteam in
		for(i = c; i < itr->e; i++) {
			kterm_t tt = TT_(itr->ts[i]);
			if(tt == TT_FROM || tt == TT_COLON) {
				return i;
			}
			if(tt == TT_NAME && ISB(S_tobytes((itr->ts[i])->text), "in")) {
				return i;
			}
		}
	}
	return -1;
}

static void _EACH(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	int idx = ITR_indexINFROM(itr);
	if(idx != -1) {
		tkitr_t pbuf, *pitr = ITR_first(itr, idx, &pbuf, +1);
		_PARAMs(_ctx, stmt, pitr);
		_EXPR(_ctx, stmt, itr);
		_ASIS(_ctx, stmt, itr);
	}
	else {
		itr->c += 1;
		_ERROR(_ctx, stmt, itr, "in");
	}
}

static void _PEACH(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_PARENTHESIS)) {
		tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
		_EACH(_ctx, stmt, pitr);
	}
	else {
		int idx = ITR_indexTT(itr, TT_CODE, -1);
		if(idx != -1) {
			tkitr_t pbuf, *pitr = ITR_first(itr, idx, &pbuf, 0);
			_EACH(_ctx, stmt, pitr);
		}
		else {
			_ERROR(_ctx, stmt, itr, "(... in ...)");
		}
	}
}

static void _PSTMT3(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_is(itr, TT_PARENTHESIS)) {
		tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
		if(ITR_count(pitr, TT_SEMICOLON) == 2) {
			int idx = ITR_indexTT(pitr, TT_SEMICOLON, itr->e);
			tkitr_t stmtbuf, *stmtitr = ITR_first(pitr, idx, &stmtbuf, +1);
			stmtitr->e += 1; // including ";"
			knh_Stmt_add(_ctx, stmt, new_StmtSTMT1(_ctx, stmtitr));
			idx = ITR_indexTT(pitr, TT_SEMICOLON, itr->e);
			stmtitr = ITR_first(pitr, idx, &stmtbuf, +1);
			if(!ITR_hasNext(stmtitr)) {
				knh_Stmt_add(_ctx, stmt, new_Term(_ctx, TT_TRUE));
			}
			else {
				_EXPR(_ctx, stmt, stmtitr);
			}
			knh_Stmt_add(_ctx, stmt, new_StmtSTMT1(_ctx, pitr));
		}
		else if(ITR_indexINFROM(pitr) != -1) {
			STT_(stmt) = STT_FOREACH;
			_EACH(_ctx, stmt, pitr);
		}
	}
	else {
		_ERROR(_ctx, stmt, itr, _("(...; ...; ...) in for statement"));
	}
}

static int isLABEL(kTerm *tk)
{
	kterm_t tt = TT_(tk);
	return (tt == TT_UNAME || tt == TT_NAME);
}

static void _LABEL(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	if(ITR_hasNext(itr) && isLABEL(ITR_tk(itr))) {
		knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
		return;
	}
	_ASIS(_ctx, stmt, itr);
}

static int isEVENT(kTerm *tk)
{
	if(Term_isExceptionType(tk)) return 1;
	return 0;
}

static kStmtExpr *Stmt_tail(CTX, kStmtExpr *o)
{
	kStmtExpr *tail = o;
	while(DP(tail)->nextNULL != NULL) {
		tail = DP(tail)->nextNULL;
	}
	return tail;
}

static kStmtExpr* StmtNULL_append(CTX, kStmtExpr *o, kStmtExpr *stmt)
{
	if(o == NULL) {
		return stmt;
	}
	else {
		kStmtExpr *tail = Stmt_tail(_ctx, o);
		KINITv(DP(tail)->nextNULL, stmt);
		return o;
	}
}

static void _CATCH(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kStmtExpr *stmtHEAD = NULL;
	while(ITR_is(itr, TT_CATCH)) {
		ITR_next(itr);
		if(ITR_is(itr, TT_PARENTHESIS)) {
			tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
			kStmtExpr *stmtCATCH = new_Stmt2(_ctx, STT_CATCH, NULL);
			Stmt_tadd(_ctx, stmtCATCH, pitr, isEVENT, _("exception"));
			Stmt_tadd(_ctx, stmtCATCH, pitr, isVARN, _("variable"));
			_STMT1(_ctx, stmtCATCH, itr);
			stmtHEAD = StmtNULL_append(_ctx, stmtHEAD, stmtCATCH);
		}
		else {
			_ERROR(_ctx, stmt, itr, _("(Exception e)"));
		}
	}
	if(stmtHEAD == NULL) {
		stmtHEAD = new_Stmt2(_ctx, STT_DONE, NULL);
	}
	knh_Stmt_add(_ctx, stmt, stmtHEAD);
	if(ITR_is(itr, TT_FINALLY)) {
		ITR_next(itr);
		_STMT1(_ctx, stmt, itr);
	}
	else {
		_DONE(_ctx, stmt, itr);
	}
}

static void _PRAGMA(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	while(ITR_hasNext(itr)) {
		knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
	}
}

static void _USING(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	while(ITR_hasNext(itr)) {
		kTerm *tk = ITR_nextTK(itr);
		if(TT_(tk) == TT_OR) _ASIS(_ctx, stmt, itr);
		knh_Stmt_add(_ctx, stmt, tk);
	}
	_ASIS(_ctx, stmt, itr);
}

static int isCLASSAME(kTerm* tk)
{
	if(TT_(tk) == TT_UNAME || TT_(tk) == TT_UFUNCNAME) {
//		kbytes_t t = S_tobytes((tk)->text);
//		if(knh_bytes_index(t, ':') == -1) {
//			return isupper(t.utext[0]);
//		}
		TT_(tk) = TT_UNAME;
		return 1;
	}
	return 0;
}

static void _CLASS(CTX, kStmtExpr *stmt, tkitr_t *itr)
{	// CNAME:0 NEW:1 EXTENDS:2 IMPLEMENTS:3 STMT:4
	Stmt_tadd(_ctx, stmt, itr, isCLASSAME, _("class name")); /*0*/
	if(ITR_is(itr, TT_PARENTHESIS)) {  // Scala style
		tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
		_PARAMs(_ctx, stmt, pitr);
	}
	else {
		_ASIS(_ctx, stmt, itr);
	}
	if(ITR_is(itr, TT_EXTENDS) || ITR_is(itr, TT_OF) || ITR_is(itr, TT_COLON)) {
		ITR_next(itr);
		if(ITR_isN(itr, +1, TT_NEXT)) {  // extends c++
			ITR_next(itr);
			knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
		}
		else {
			Stmt_tadd(_ctx, stmt, itr, isCLASSAME, _("class name")); /*2*/
		}
	}
	else { /* Object */
		knh_Stmt_add(_ctx, stmt, new_TermCID(_ctx, CLASS_Object));
	}
	if(ITR_is(itr, TT_IMPLEMENTS)) {
		ITR_skipUNTIL(itr, TT_CODE);
		_ASIS(_ctx, stmt, itr); /* implements for future 3*/
	}
	else {
		_ASIS(_ctx, stmt, itr);
	}
	if(ITR_is(itr, TT_CODE)) {
		knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
	}
	else {
		_SEMICOLON(_ctx, stmt, itr);
	}
}

static int isMN(kTerm* tk)
{
	if(TT_(tk) == TT_UFUNCNAME) TT_(tk) = TT_UFUNCNAME;
	return (TT_(tk) == TT_FUNCNAME  /* || TT_(tk) == TT_PROPN*/);
}

static void _CODE(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	size_t i, hasCODE = 0;;
	for(i = 0; i < DP(stmt)->size; i++) {
		if(TT_(tkNN(stmt, i)) == TT_DOC) {
			hasCODE = 1; break;
		}
	}
	if(ITR_is(itr, TT_CODE)) {
		if(hasCODE) {
			WARN_Ignored(_ctx, "block", CLASS_unknown, S_text(ITR_nextTK(itr)->text));
		}
		else {
			_CODEDOC(_ctx, stmt, itr);
			knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
		}
	}
	else if(ITR_is(itr, TT_DARROW) || ITR_is(itr, TT_COLON)) {
		tkitr_t stmtbuf, *stmtitr = ITR_stmt(_ctx, itr, +1, &stmtbuf, 1);
		if(hasCODE) {
			WARN_Ignored(_ctx, "=>", CLASS_unknown, NULL);
		}
		else {
			_CODEDOC(_ctx, stmt, stmtitr);
			stmtitr->c += 1;
			_RETURNEXPR(_ctx, stmt, stmtitr);
		}
	}
	else if(ITR_is(itr, TT_WITH)) {
		tkitr_t stmtbuf, *stmtitr = ITR_stmt(_ctx, itr, +1, &stmtbuf, 1);
		if(hasCODE) {
			WARN_Ignored(_ctx, "with", CLASS_unknown, NULL);
		}
		else {
			StmtMETHOD_setFFI(stmt, 1);
			stmtitr->c += 1;
			_EXPR(_ctx, stmt, stmtitr);
			DBG_P("WITH DP(stmt)->size=%d", DP(stmt)->size);
		}
	}
	else {
		_SEMICOLON(_ctx, stmt, itr);
	}
}

static void _METHOD(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kTerm *tkT = ITR_nextTK(itr);
	knh_Stmt_add(_ctx, stmt, tkT);
	if(TT_(tkT) == TT_FUNCTION) TT_(tkT) = TT_ASIS;
	if(ITR_is(itr, TT_UNAME)) {
		Stmt_tadd(_ctx, stmt, itr, isCLASSAME, _("class name"));
	}
	else {
		_ASIS(_ctx, stmt, itr);
	}
	Stmt_taddASIS(_ctx, stmt, itr, isMN);
	ADD(stmt, _PARAM(_ctx, stmt, itr));
	ADD(stmt, _CODE(_ctx, stmt, itr));
}


static void _CONSTRUCTOR(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kTerm *tkT = ITR_tk(itr);
	TT_(tkT) = TT_UNAME;
	Stmt_tadd(_ctx, stmt, itr, isCLASSAME, _("class name"));
	_ASIS(_ctx, stmt, itr);
	knh_Stmt_add(_ctx, stmt, new_TermMN(_ctx, MN_new));
	ADD(stmt, _PARAM(_ctx, stmt, itr));
	ADD(stmt, _CODE(_ctx, stmt, itr));
}

static void _TYPEMAP(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	_ASIS(_ctx, stmt, itr);    // TLINK or ASIS
	knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));  /* T */
	ITR_nextTK(itr); // skip ':'
	ADD(stmt, _PARAM(_ctx, stmt, itr));
	_ASIS(_ctx, stmt, itr);    // TLINK or ASIS
	ADD(stmt, _CODE(_ctx, stmt, itr));
}

static void _FORMAT(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	kTerm *tkT = ITR_nextTK(itr);
	knh_Stmt_add(_ctx, stmt, tkT);  // void
	ITR_nextTK(itr); // skip %
	_ASIS(_ctx, stmt, itr);  // class name
	knh_Stmt_add(_ctx, stmt, ITR_nextTK(itr));
	ADD(stmt, _PARAM(_ctx, stmt, itr));
	ADD(stmt, _CODE(_ctx, stmt, itr));
}

/* ------------------------------------------------------------------------ */
/* META */

static void Stmt_addMETA(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	int i, e;
	kTerm **ts = itr->ts;
	if(itr->meta == -1 || STT_(stmt) == STT_DONE || STT_(stmt) == STT_ERR) {
		return ;
	}
	e = (itr->meta < itr->c) ? itr->c : itr->e;
	if(IS_NULL(DP(stmt)->metaDictCaseMap)) {
		KSETv(DP(stmt)->metaDictCaseMap, new_DictMap0(_ctx, 2, 1/*isCaseMap*/, "Stmt.MetaDictMap"));
	}
	for(i = itr->meta; i < e; i++) {
		if(TT_(ts[i]) == TT_NAME || TT_(ts[i]) == TT_UNAME) {
			knh_DictMap_set(_ctx, DP(stmt)->metaDictCaseMap, TS_ATlabel, ts[i]);
			i++;
			DBG_ASSERT(TT_(ts[i]) == TT_COLON);
		}
		else if(TT_(ts[i]) == TT_METAN) {
			knh_DictMap_set(_ctx, DP(stmt)->metaDictCaseMap, (ts[i])->text, UPCAST(ts[i]));
		}
	}
	if(e == itr->e) itr->c = i;
	itr->meta = -1;
}

static kStmtExpr *new_StmtMETA(CTX, kterm_t stt, tkitr_t *itr, int shift, ...)
{
	knh_Fstmt func = NULL;
	va_list ap;
	kStmtExpr *stmt = new_Stmt2(_ctx, stt, NULL);
	Stmt_addMETA(_ctx, stmt, itr);
	itr->c = itr->c + shift;
	va_start(ap , shift);
	while((func = (knh_Fstmt)va_arg(ap, knh_Fstmt)) != NULL) {
		if(STT_(stmt) == STT_ERR) {
			goto L_end;
		}
		func(_ctx, stmt, itr);
	}
	L_end:;
	va_end(ap);
	return stmt;
}

/* ------------------------------------------------------------------------ */
/* [STMT1] */

#define CASE_(XX, N, ...) case TT_##XX : { \
		stmt = new_StmtMETA(_ctx, STT_##XX , itr, N, ## __VA_ARGS__, NULL); \
		break;\
	}\

#define CASE_L(XX, N, CHECKSEMI, ...) case TT_##XX : { \
		tkitr_t stmtbuf, *stmtitr = ITR_stmt(_ctx, itr, N, &stmtbuf, CHECKSEMI);\
		stmt = new_StmtMETA(_ctx, STT_##XX , stmtitr, N, ## __VA_ARGS__, NULL);\
		break;\
	}\



static int ITR_findSTMTOPR(tkitr_t *itr, int *op)
{
	int i;
	for(i = itr->c; i < itr->e; i++) {
		kterm_t tt = TT_(itr->ts[i]);
		if(tt == TT_LET) {
			op[0] = 0; return i;
		}
		if(tt == TT_FROM) {
			op[0] = 1; return i;
		}
		if(tt == TT_LSFT) {
			op[0] = 2; return i;
		}
		if(tt == TT_LSEND) {
			op[0] = 3; return i;
		}
		if(tt == TT_RSEND) {
			op[0] = 4; return i;
		}
	}
	return -1;
}

static void _STMTEXPR(CTX, kStmtExpr *stmt, tkitr_t *itr)
{
	int op = -1;
	int idx = ITR_findSTMTOPR(itr, &op);
	switch(op) {
	case 0: {
		int comma = ITR_count(itr, TT_COMMA);
		if(comma > 0) {  //  check multiple assingment
			tkitr_t lbuf, *litr = ITR_first(itr, idx, &lbuf, +1);
			int lcomma = ITR_count(litr, TT_COMMA);
			if(lcomma == comma) {  // @CODE: a, b = t
				STT_(stmt) = STT_LETM;
				_EXPRs(_ctx, stmt, litr);
				_EXPR(_ctx, stmt, itr);
			}
			else if(lcomma * 2 == comma) {   // @CODE: a, b = b, a
				STT_(stmt) = STT_SWAP;
				_EXPRs(_ctx, stmt, litr);
				_EXPRs(_ctx, stmt, itr);
			}
			else {
				kStmtExproERR(_ctx, stmt, ERROR_text(_ctx,  "mismatched assignment" K_TRACEPOINT));
			}
		}
		else if(ITR_is(itr, TT_UNAME) && itr->c+1 == idx) {
			STT_(stmt) = STT_CONST;
			ITR_replace(itr, TT_LET, TT_COMMA);
			_EXPRs(_ctx, stmt, itr);
		}
		else {
			_EXPR(_ctx, stmt, itr);
		}
		break;
	}
	case 1: { /* a, b from expr */
		tkitr_t lbuf, *litr = ITR_first(itr, idx, &lbuf, +1);
		STT_(stmt) = STT_SELECT;
		_EXPRs(_ctx, stmt, litr);
		_EXPR(_ctx, stmt, itr);
		break;
	}
	case 2: {  /* TT_LSHT */
		Stmt_setLSHIFT(stmt, 1);
		ITR_replace(itr, TT_LSFT, TT_COMMA);
	}
	case 3: {
		STT_(stmt) = STT_SEND;
		knh_Stmt_add(_ctx, stmt, new_TermMN(_ctx, MN_send));
		ITR_replace(itr, TT_LSEND, TT_COMMA);
		_EXPRs(_ctx, stmt, itr);
		break;
	}
	case 4: {
		int e = itr->e;
		itr->e = idx;
		_EXPR(_ctx, stmt, itr);
		if(STT_(stmt) == STT_FUNCCALL) {
			itr->c = idx + 1; itr->e = e;
			KSETv(stmtNN(stmt, 1), new_StmtMETA(_ctx, STT_CALL1, itr, 0, _EXPR, NULL));
		}
		else {
			if(STT_(stmt) != STT_ERR) {
				kStmtExproERR(_ctx, stmt, ERROR_text(_ctx,  "message should be sent" K_TRACEPOINT));
			}
		}
		break;
	}
	default: {
		_EXPR(_ctx, stmt, itr);
	}
	}/*switch*/
}

static int Term_isMAP(CTX, kTerm *tk)
{
	DBG_ASSERT(TT_(tk) == TT_BRACE);
	tkitr_t tbuf, *titr = ITR_new(tk, &tbuf);
	int i, colon = 0, comma = 0;
	int isMAP = titr->e == 0 ? 1 : 0;
	for(i = 0; i < titr->e; i++) {
		kterm_t tt = TT_(titr->ts[i]);
		if(tt <= TT_FINALLY || tt == TT_LSEND || tt == TT_RSEND || tt == TT_LET) {
			isMAP = 0;
			goto L_RETURN;
		}
		else if(tt == TT_SEMICOLON && colon == 0) {
			isMAP = 0;
			goto L_RETURN;
		}
		else if(tt == TT_COLON) colon++;
		else if(tt == TT_COMMA) comma++;
	}
	if(colon == 0 && titr->e > 0) {
		TT_(tk) = TT_BRANCET;
		isMAP = 1;
	}
	if(colon > 0) {
		isMAP = 1;
	}
	L_RETURN:;
	return isMAP;
}

static kStmtExpr *new_StmtSTMT1(CTX, tkitr_t *itr)
{
	kterm_t tt;
	kStmtExpr *stmt = NULL;
	kTerm *tkLINE = NULL;
	L_TAIL:;
	TT_skipMETA(_ctx, itr);
	if(!ITR_hasNext(itr)) {
		stmt = new_Stmt2(_ctx, STT_DONE, NULL);
		goto L_RETURN;
	}
	tkLINE = ITR_tk(itr);
	tt = ITR_tt(itr);
	switch(tt) {
		case TT_SEMICOLON: {
			ITR_next(itr);
			goto L_TAIL;
		}
		CASE_L(PRAGMA, +1, 1/*;*/, _PRAGMA)
		CASE_(NAMESPACE, +1, _STMT1);
		CASE_L(INCLUDE, +1, 1/*;*/, _EXPRs);
		CASE_L(USING, +1, 1/*;*/, _USING);
		CASE_(CLASS, +1, _CLASS);
		CASE_L(BREAK, +1, 1/*;*/, _LABEL);
		CASE_L(CONTINUE, +1, 1/*;*/, _LABEL);
		CASE_L(RETURN, +1, 1/*;*/, _EXPRs);
		CASE_L(THROW, +1, 1/*;*/, _EXPRs);
		CASE_L(REGISTER, +1, 1/*;*/, _EXPRs);
		CASE_L(PRINT, +1, 1/*;*/, _EXPRs);
		CASE_L(YIELD, +1, 1/*;*/, _EXPRs);
		CASE_(IF, +1, _PEXPR, _STMT1, _ELSE);
		CASE_(SWITCH, +1, _PEXPR, _CASESTMT, _ASIS);  /* it */
		CASE_(WHILE, +1, _PEXPR, _STMT1);
		CASE_(DO, +1, _DOWHILE, _PEXPR, _SEMICOLON);
		CASE_(FOR, +1, _PSTMT3, _STMT1);
		CASE_(FOREACH, +1, _PEACH, _STMT1);
		CASE_(TRY, +1, _STMT1, _CATCH, _ASIS);  /* it */
		CASE_(ASSURE, +1, _ONEEXPR, _STMT1, _ASIS/*it*/);
		CASE_L(ASSERT, +1, 1/*;*/, _EXPR);
		case TT_CODE:
			Term_toBRACE(_ctx, ITR_tk(itr), 1/*isEXPANDING*/);
		case TT_BRACE:
			if(!Term_isMAP(_ctx, ITR_tk(itr))) {
				tkitr_t pbuf, *pitr = ITR_new(ITR_nextTK(itr), &pbuf);
				stmt = new_StmtMETA(_ctx, STT_BLOCK, pitr, 0, _STMTs, NULL);
			}
			break /*L_EXPR*/;
		case TT_VOID: {
			if(ITR_isN(itr, +1, TT_MOD) && (ITR_isN(itr, +2, TT_FUNCNAME) || ITR_isN(itr, +2, TT_UFUNCNAME))) {
				stmt = new_StmtMETA(_ctx, STT_FORMAT, itr, 0, _FORMAT, NULL);
				break;
			}
		}
		case TT_VAR: case TT_PTYPE: case TT_DYN:
		case TT_TYPEOF: case TT_BYTE:
		case TT_UNAME: {
			tkitr_t mbuf, *mitr = ITR_copy(itr, &mbuf, +1);
			if(ITR_isT(mitr, isVARN) && tt != TT_VOID) {
				tkitr_t dbuf, *ditr = ITR_stmt(_ctx, itr, /*pos*/+1, &dbuf, 1/*needs;*/);
				stmt = new_StmtMETA(_ctx, STT_DECL, ditr, 0, _DECL2, NULL);
				break;
			}
			if(ITR_is(mitr, TT_UNAME)) {
				ITR_next(mitr);
				if(ITR_is(mitr, TT_UFUNCNAME)) {
					kTerm *tkUF = ITR_tk(mitr); TT_(tkUF) = TT_FUNCNAME;
					WarningMethodName(_ctx, S_text(tkUF->text));
				}
				if(ITR_is(mitr, TT_FUNCNAME)) {
					stmt = new_StmtMETA(_ctx, STT_METHOD, itr, 0, _METHOD, NULL);
					break;
				}
			}
			if(ITR_is(mitr, TT_FUNCNAME) && !Term_isDOT(ITR_tk(mitr))) {
				stmt = new_StmtMETA(_ctx, STT_METHOD, itr, 0, _METHOD, NULL);
				break;
			}
			if(ITR_is(mitr, TT_UFUNCNAME) && !Term_isDOT(ITR_tk(mitr))) {
				kTerm *tkUF = ITR_tk(mitr); TT_(tkUF) = TT_FUNCNAME;
				WarningMethodName(_ctx, S_text(tkUF->text));
				stmt = new_StmtMETA(_ctx, STT_METHOD, itr, 0, _METHOD, NULL);
				break;
			}
			if(ITR_is(mitr, TT_COLON) && ITR_isN(mitr, +1, TT_PARENTHESIS) && tt != TT_VAR && tt != TT_DYN && tt != TT_VOID) {
				stmt = new_StmtMETA(_ctx, STT_TYPEMAP, itr, 0, _TYPEMAP, NULL);
			}
			break /*L_EXPR*/;
		}
		case TT_FUNCTION: {
			if(ITR_isN(itr, +1, TT_FUNCNAME) || ITR_isN(itr, +1, TT_UFUNCNAME)) {
				stmt = new_StmtMETA(_ctx, STT_METHOD, itr, 0, _METHOD, NULL);
				break;
			}
			break /*L_EXPR*/;
		}
		case TT_UFUNCNAME: {
			if(ITR_isN(itr, +2, TT_CODE)) {
				stmt = new_StmtMETA(_ctx, STT_METHOD, itr, 0, _CONSTRUCTOR, NULL);
				break;
			}
		}
		case TT_PROPN:
		case TT_NAME:
		case TT_FUNCNAME:
		case TT_FMT:
		case TT_PARENTHESIS:
		case TT_BRANCET:
		case TT_TRUE: case TT_FALSE: case TT_NULL:
		case TT_NOT: case TT_EXISTS: case TT_TLINK:
		case TT_LNOT:
		case TT_NEXT: case TT_PREV:  /* Prev */
		case TT_ITR: case TT_NEW:
		case TT_TADD: case TT_TAND: case TT_TSUB:
		case TT_NUM:
		case TT_STR: case TT_TSTR: /* case TT_FMTSTR: */
		case TT_ESTR: case TT_REGEX:
		case TT_URN:
			break; // EXPR
		case TT_ERR:  default: {
			tkitr_t sbuf, *sitr = ITR_stmt(_ctx, itr, /*pos*/+1, &sbuf, 0/*needs;*/);
			stmt = new_StmtMETA(_ctx, STT_CALL1, sitr, 0, NULL);
			_DBGERROR(_ctx, stmt, sitr, "value" K_TRACEPOINT);
	 		break;
		}
	}
	if(stmt == NULL) {
		tkitr_t sbuf, *sitr = ITR_stmt(_ctx, itr, /*pos*/+1, &sbuf, 0/*needs;*/);
		stmt = new_StmtMETA(_ctx, STT_CALL1, sitr, 0, _STMTEXPR, NULL);
	}
	L_RETURN:;
	return stmt;
}

/* ------------------------------------------------------------------------ */

#ifdef K_USING_SUGAR

kStmtExpr *knh_parseStmt(CTX, const char *script, kline_t uline)
{
	kStmtExpr *rVALUE = new_Stmt2(_ctx, STT_BLOCK, NULL);
	rVALUE->uline	= uline;
	PUSH_GCSTACK(_ctx, rVALUE);
	{
		INIT_GCSTACK(_ctx);
		kArray *a = new_TokenArray(_ctx, script, uline);
		kTerm *tm = new_Term(_ctx, TT_BRACE);
		PUSH_GCSTACK(_ctx, tm);
		TokenArray_parseTerm(_ctx, a, 0, tm, -1);
		DBG_(if(knh_isVerboseLang() /*&& DP(in)->uri == URI_EVAL*/) {
			knh_write_Object(_ctx, KNH_STDOUT, UPCAST(tm), FMT_dump);
			knh_flush(_ctx, KNH_STDOUT);

		})
		if(TT_(tm) != TT_ERR) {
			tkitr_t tbuf, *titr = ITR_new(tm, &tbuf);
			_STMTs(_ctx, rVALUE, titr);
			DBG_ASSERT(DP(rVALUE)->size == 1);
			if(IS_StmtExpr(stmtNN(rVALUE, 0))) {
				rVALUE = stmtNN(rVALUE, 0);
				DBG_(if(knh_isVerboseLang() /*&& DP(in)->uri == URI_EVAL*/) {
					knh_write_Object(_ctx, KNH_STDOUT, UPCAST(rVALUE), FMT_dump);
					knh_flush(_ctx, KNH_STDOUT);
				})
				goto L_OK;
			}
		}
		kStmtExproERR(_ctx, rVALUE, tm);
		L_OK: ;
		RESET_GCSTACK(_ctx);
	}
	return rVALUE;
}
#else

kStmtExpr *knh_InputStream_parseStmt(CTX, kInputStream *in, kline_t *ul)
{
	BEGIN_LOCAL(_ctx, lsfp, 2);
	DBG_ASSERT(ul[0] != 0);
	kcodemod->uline = ul[0];
	kStmtExpr *rVALUE = new_Stmt2(_ctx, STT_BLOCK, NULL);
	kTerm *tk = new_Term(_ctx, TT_BRACE);
	KSETv(lsfp[0].o, rVALUE);
	KSETv(lsfp[1].o, tk);
	InputStream_parseTerm(_ctx, in, ul, tk);
	DBG_(if(knh_isVerboseLang() /*&& DP(in)->uri == URI_EVAL*/) {
		knh_write_Object(_ctx, KNH_STDOUT, UPCAST(tk), FMT_dump);
	})
	if(TT_(tk) != TT_ERR) {
		tkitr_t tbuf, *titr = ITR_new(tk, &tbuf);
		_STMTs(_ctx, rVALUE, titr);
		DBG_ASSERT(DP(rVALUE)->size == 1);
		if(IS_StmtExpr(stmtNN(rVALUE, 0))) {
			rVALUE = stmtNN(rVALUE, 0);
			DBG_(if(knh_isVerboseLang() /*&& DP(in)->uri == URI_EVAL*/) {
				knh_write_Object(_ctx, KNH_STDOUT, UPCAST(rVALUE), FMT_dump);
			})
			goto L_RETURN;
		}
	}
	kStmtExproERR(_ctx, rVALUE, tk);
	L_RETURN:;
	END_LOCAL(_ctx, lsfp);
	return rVALUE;
}
#endif

/* ------------------------------------------------------------------------ */

kStmtExpr *knh_Term_parseStmt(CTX, kline_t uline, kTerm *tk)
{
	BEGIN_LOCAL(_ctx, lsfp, 1);
	//DBG_P("uline=%d, tk->uline=%d src='''%s'''", (kshort_t)uline, (kshort_t)tk->uline, S_text(tk->text));
	kcodemod->uline = tk->uline;
	kStmtExpr *rVALUE = new_Stmt2(_ctx, STT_BLOCK, NULL);
	KSETv(lsfp[0].o, rVALUE);
	Term_toBRACE(_ctx, tk, 1/*isEXPANDING*/);
	if(TT_(tk) != TT_ERR) {
		tkitr_t tbuf, *titr = ITR_new(tk, &tbuf);
		_STMTs(_ctx, rVALUE, titr);
		DBG_ASSERT(DP(rVALUE)->size == 1);
		if(STT_(rVALUE) != STT_ERR) {
			rVALUE = stmtNN(rVALUE, 0);
		}
		DBG_ASSERT(IS_StmtExpr(rVALUE));
	}
	else {
		kStmtExproERR(_ctx, rVALUE, tk);
	}
	END_LOCAL(_ctx, lsfp);
	return rVALUE;
}

kStmtExpr *knh_bytes_parseStmt(CTX, kbytes_t expr, kline_t uline)
{
	kcodemod->uline = uline;
	BEGIN_LOCAL(_ctx, lsfp, 2);
	LOCAL_NEW(_ctx, lsfp, 0, kStmtExpr*, rVALUE, new_Stmt2(_ctx, STT_BLOCK, NULL));
	LOCAL_NEW(_ctx, lsfp, 1, kTerm*, tk, new_Term(_ctx, TT_CODE));
	KSETv((tk)->data, new_kString(expr.text, expr.len, 0));
	Term_toBRACE(_ctx, tk, 1/*isEXPANDING*/);
	if(TT_(tk) != TT_ERR) {
		tkitr_t tbuf, *titr = ITR_new(tk, &tbuf);
		_STMTs(_ctx, rVALUE, titr);
		DBG_ASSERT(DP(rVALUE)->size > 0);
		rVALUE = stmtNN(rVALUE, 0);
	}
	else {
		kStmtExproERR(_ctx, rVALUE, tk);
	}
	END_LOCAL(_ctx, lsfp);
	return rVALUE;
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
