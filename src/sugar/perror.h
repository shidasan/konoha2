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

static const char* T_emsg(CTX, int pe)
{
	switch(pe) {
		case ERR_: return "(error)";
		case WARN_: return "(warning)";
		case INFO_:
			if(CTX_isInteractive() || CTX_isCompileOnly() || CTX_isDebug()) {
				return "(info)";
			}
			return NULL;
		case DEBUG_:
			if(CTX_isDebug()) {
				return "(debug)";
			}
			return NULL;
	}
	return "(unknown)";
}

static void vperrorf(CTX, int pe, kline_t uline, int lpos, const char *fmt, va_list ap)
{
	const char *msg = T_emsg(_ctx, pe);
	if(msg != NULL) {
		kevalmod_t *base = kevalmod;
		kwb_t wb;
		kwb_init(&base->cwb, &wb);
		if(uline > 0) {
			const char *file = T_file(uline);
			if(lpos != -1) {
				kwb_printf(&wb, "%s (%s:%d+%d) " , msg, filename(file), (kushort_t)uline, (int)lpos+1);
			}
			else {
				kwb_printf(&wb, "%s (%s:%d) " , msg, filename(file), (kushort_t)uline);
			}
		}
		else {
			kwb_printf(&wb, "%s " , msg);
		}
		kwb_vprintf(&wb, fmt, ap);
		msg = kwb_top(&wb, 1);
		kString *emsg = new_kString(msg, strlen(msg), 0);
		kArray_add(base->errors, emsg);
		kreport(pe, S_text(emsg));
	}
}

#define SUGAR_P(PE, UL, POS, FMT, ...)  Kpef(_ctx, PE, UL, POS, FMT,  ## __VA_ARGS__)

#define ERR_SyntaxError(UL)  SUGAR_P(ERR_, UL, -1, "syntax sugar error at %s:%d", __FUNCTION__, __LINE__)

static void Kpef(CTX, int pe, kline_t uline, int lpos, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vperrorf(_ctx, pe, uline, lpos, fmt, ap);
	va_end(ap);
}

#define kToken_p(TK, PE, FMT, ...)   Token_p(_ctx, TK, PE, FMT, ## __VA_ARGS__)
#define kExpr_p(E, PE, FMT, ...)     Expr_p(_ctx, E, PE, FMT, ## __VA_ARGS__)
static kExpr* Token_p(CTX, kToken *tk, int pe, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vperrorf(_ctx, pe, tk->uline, tk->lpos, fmt, ap);
	va_end(ap);
	return K_NULLEXPR;
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
	SUGAR_P(WARN_, uline, 0, "must close with %c", ch);
}

static void IGNORE_UnxpectedMultiByteChar(CTX, kline_t uline, int lpos, char *text, size_t len)
{
	int ch = text[len];
	text[len] = 0;
	SUGAR_P(WARN_, uline, lpos, "unexpected multi-byte character: %s", text);
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
