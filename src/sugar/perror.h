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

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [perror] */

#define S_cid(X)  S_cid_(_ctx, X)
#define T_cid(X)  S_text(S_cid(X))

static inline kString* S_cid_(CTX, ktype_t ty)
{
	return CT_(ty)->name;
}

#define S_ty(X)  S_ty_(_ctx, X)
#define T_ty(X)  S_text(S_ty(X))

static inline kString* S_ty_(CTX, ktype_t ty)
{
	return CT_(ty)->name;
}

#define S_fn(fn)   S_fn_(_ctx, fn)
#define T_fn(fn)   S_text(S_fn(fn))
static inline kString* S_fn_(CTX, ksymbol_t sym)
{
	int index = MN_UNMASK(sym);
	if(index < kArray_size(_ctx->share->symbolList)) {
		return _ctx->share->symbolList->strings[index];
	}
	DBG_P("undefined symbol sym=%d", sym);
	return TS_EMPTY;
}

#define T_mn(B, X)  Tsymbol(_ctx, B, sizeof(B), X)
static const char* Tsymbol(CTX, char *buf, size_t bufsiz, ksymbol_t sym)
{
	int index = MN_UNMASK(sym);
	if(index < kArray_size(_ctx->share->symbolList)) {
		const char *name = S_text(_ctx->share->symbolList->strings[index]);
		if(MN_isISBOOL(sym)) {
			snprintf(buf, bufsiz, "is%s", name);
			buf[2] = toupper(buf[2]);
		}
		else if(MN_isGETTER(sym)) {
			snprintf(buf, bufsiz, "get%s", name);
			buf[3] = toupper(buf[3]);
		}
		else if(MN_isSETTER(sym)) {
			snprintf(buf, bufsiz, "set%s", name);
			buf[3] = toupper(buf[3]);
		}
		else {
			snprintf(buf, bufsiz, "%s", name);
		}
	}
	else {
		snprintf(buf, bufsiz, "unknown symbol=%d !< %ld", index, kArray_size(_ctx->share->symbolList));
	}
	return (const char*)buf;
}

/* ------------------------------------------------------------------------ */
/* [perror] */

static void kvperror(CTX, int pe, const char *msg, kline_t uline, int lpos, const char *fmt, va_list ap)
{
	kevalmod_t *base = kevalmod;
	kwb_t wb;
	kwb_init(&base->cwb, &wb);
	kwb_write(&wb, msg, strlen(msg));
	if(uline > 0) {
		const char *file = S_text(S_uri(uline));
		if(lpos != -1) {
			kwb_printf(&wb, "(%s:%d+%d) " , filename(file), (kushort_t)uline, (int)lpos+1);
		}
		else {
			kwb_printf(&wb, "(%s:%d) " , filename(file), (kushort_t)uline);
		}
	}
	kwb_vprintf(&wb, fmt, ap);
	msg = kwb_top(&wb, 1);
	kString *emsg = new_kString(msg, strlen(msg), 0);
	kArray_add(base->errors, emsg);
	kreport(pe, S_text(emsg));
}

static void kerror(CTX, int level, kline_t uline, int lpos, const char *fmt, ...)
{
	int isPRINT = 0;
	const char *emsg = "(error) ";
	switch(level) {
		case ERR_:
			isPRINT = 1;
			break;
		case WARN_:
			emsg = "(warning) "; isPRINT = 1;
			break;
		case INFO_:
			emsg = "(info) ";
//			if((CTX_isInteractive() || CTX_isCompileOnly() || CTX_isDebug()) {
				isPRINT = 1;
//			}
			break;
		case DEBUG_:
			emsg = "(debug) ";
			if(CTX_isDebug()) {
				isPRINT = 1;
			}
	}
	if(isPRINT) {
		va_list ap;
		va_start(ap, fmt);
		kvperror(_ctx, level, emsg, uline, lpos, fmt, ap);
		va_end(ap);
	}
}

#define kerrno   Kerrno(_ctx)
#define kstrerror(ENO)  Kstrerror(_ctx, ENO)

static int Kerrno(CTX)
{
	return kArray_size(kevalmod->errors);
}

static kString* Kstrerror(CTX, int eno)
{
	kevalmod_t *base = kevalmod;
	size_t i;
	for(i = eno; i < kArray_size(base->errors); i++) {
		kString *emsg = base->errors->strings[i];
		if(strstr(S_text(emsg), "(error)") != NULL) {
			return emsg;
		}
	}
	DBG_P("kerrno=%d, |errmsgs|=%d", kerrno, kArray_size(base->errors));
	abort();
	return TS_EMPTY;
}

static void WARN_MustCloseWith(CTX, kline_t uline, int ch)
{
	kerror(_ctx, WARN_, uline, 0, "must close with %c", ch);
}

static void IGNORE_UnxpectedMultiByteChar(CTX, kline_t uline, int lpos, char *text, size_t len)
{
	int ch = text[len];
	text[len] = 0;
	kerror(_ctx, WARN_, uline, lpos, "unexpected multi-byte character: %s", text);
	text[len] = ch; // danger a little
}

//static void DEBUG_TokenAlias(CTX, kline_t uline, int lpos, kbytes_t t, kString *alias)
//{
//	char buf[256] = {0};
//	if(t.len < 256) {
//		memcpy(buf, t.buf, t.len);
//		kerror(_ctx, INFO_, uline, lpos, "rewrite '%s' to '%s'", buf, S_text(alias));
//	}
//}
//
//static void WARN_LiteralMustCloseWith(CTX, kline_t uline, int lpos, int quote)
//{
//	char buf[8];
//	knh_snprintf(buf, sizeof(buf), "%c", quote);
//	kerror(_ctx, WARN_, uline, lpos, "Literal must close with %s", buf);
//}
//
//
//static void ERROR_Expected(CTX, kToken *tk, int closech, const char *token)
//{
//	if(token == NULL) {
//		char buf[8];
//		knh_snprintf(buf, sizeof(buf), "%c", closech);
//	}
//	else {
//		kerror(_ctx, ERR_, tk->uline, 0, "%s is expected", token);
//	}
//}
//
//static kExpr* ERROR_UnexpectedToken(CTX, kToken *tk, const char *token)
//{
//	if(IS_String(tk->text)) {
//		kerror(_ctx, ERR_, tk->uline, tk->lpos, "unexpected %s; %s is expected", S_text(tk->text), token);
//	}
//	else {
//		kerror(_ctx, ERR_, tk->uline, tk->lpos, "unexpected token; %s is expected", token);
//	}
//	return NULL;
//}
//
//static void ERROR_UndefinedToken(CTX, kToken *tk, const char *whatis)
//{
//	kerror(_ctx, ERR_, tk->uline, tk->lpos, "undefined %s: %s", whatis, S_text(tk->text));
//}
//
//
//static kbool_t ERROR_SyntaxError(CTX, kline_t uline)
//{
//	kerror(_ctx, ERR_, uline, 0, "syntax error");
//	return 0;
//}
//
//static kbool_t ERROR_TokenError(CTX, kToken *tk)
//{
//	kerror(_ctx, ERR_, tk->uline, tk->lpos, "syntax error: token '%s' is unavailable", S_text(tk->text));
//	return 0;
//}
//
//static kbool_t ERROR_TokenMustBe(CTX, kToken *tk, const char *token)
//{
//	kerror(_ctx, ERR_, tk->uline, tk->lpos, "syntax error: '%s' must be %s", S_text(tk->text), token);
//	return 0;
//}
//
//static kExpr *ERROR_TokenUndefinedMethod(CTX, kToken *tk, kcid_t cid)
//{
//	kerror(_ctx, ERR_, tk->uline, tk->lpos, "undefined method: %T.%s", cid, S_text(tk->text));
//	return NULL;
//}
//
//static kExpr* ERROR_TokenUndefined(CTX, kToken *tk, const char *whatis, kcid_t cid)
//{
//	if(cid != CLASS_unknown) {
//		kerror(_ctx, ERR_, tk->uline, tk->lpos, _("undefined %s: %T.%O"), whatis, cid, tk);
//	}
//	else {
//		kerror(_ctx, ERR_, tk->uline, tk->lpos, _("undefined %s: %O"), whatis, tk);
//	}
//	return NULL;
//}
//
//void WARN_TokenMuchBetter(CTX, kToken *tk, const char *token)
//{
//	kerror(_ctx, ERR_, tk->uline, tk->lpos, "%s is much better than %s", S_text(tk->text), token);
//}
//
//void WARN_TokenOverflow(CTX, kToken *tk)
//{
//	kerror(_ctx, ERR_, tk->uline, tk->lpos, "%s is overflow", S_text(tk->text));
//}


#ifdef __cplusplus
}
#endif
