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
/* [term] */

//#define knh_String_equals(STR, T)   (knh_bytes_equals(S_tobytes(STR), STEXT(T)))

#define _PERROR      (1)
#define _UNFOLD      (1<<1)
#define _WHITESPACE  (1<<2)
#define _INDENT      (1<<3)

#define _TOPLEVEL    _INDENT|_UNFOLD|_PERROR
#define _METHOD      _INDENT|_PERROR

typedef struct {
	const char   *source;
	kline_t       uline;
	kArray       *list;
	const char   *bol;     // begin of line
	int           indent_tab;
	int           policy;
} tenv_t;

static inline int lpos(tenv_t *tenv, const char *s)
{
	return (tenv->bol == NULL) ? -1 : s - tenv->bol;
}

#define CLASS_Token 0

static kToken *new_Token(CTX, ktoken_t tt, kline_t uline, int lpos, kString *text)
{
	kToken *tk = new_(Token, 0);
	tk->tt = tt;
	tk->uline = uline;
	tk->lpos = (kushort_t)lpos;
	tk->topch = 0;
	DBG_ASSERT(text != NULL);
	KSETv(tk->text, text);
	return tk;
}

static void addToken(CTX, tenv_t *tenv, kToken *tk)
{
	kArray_add(tenv->list, tk);
}

static size_t skipLine(CTX, tenv_t *tenv, size_t pos)
{
	int ch;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\n') {
			tenv->uline += 1;
			return pos;
		}
	}
	return pos-1;/*EOF*/
}

static size_t skipComment(CTX, tenv_t *tenv, size_t pos)
{
	int ch, prev = 0, level = 1;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\n') {
			tenv->uline += 1;
		}
		if(prev == '*' && ch == '/') {
			level--;
			if(level == 0) break;
		}else if(prev == '/' && ch == '*') {
			level++;
		}
		prev = ch;
	}
	return pos-1;/*EOF*/
}

static size_t skipQuote(CTX, tenv_t *tenv, size_t pos, int quote)
{
	int ch, prev = 0;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\n') {
			tenv->uline += 1;
		}
		if(ch == quote) {
			if(prev == '\\') continue;
			return pos;
		}
	}
	return pos-1;/*EOF*/
}

#define DUMMY 1
#define skipURN(_ctx, tenv, pos)  addURN(_ctx, tenv, pos, pos, 1)

static int addURN(CTX, tenv_t *tenv, size_t pos, size_t tok_start, int skip)
{
	int ch, pol = SPOL_ASCII;
	while((ch = tenv->source[pos++]) != 0) {
		switch(ch) {
			case '\n': case '\r': case ' ':  case '\t': case ';': case ',':
			case '\'': case '"': case '`':
			case ')': case '}': case ']':
			case '(': case '{': case '[':
				break;
		}
		if(ch > 0) {
			pol = SPOL_UTF8;
		}
	}
	if(!skip) {
		const char *s = tenv->source + tok_start;
		kString *text = new_kString(s, ((pos-1)-tok_start), pol);
		addToken(_ctx, tenv, new_Token(_ctx, TK_URN, tenv->uline, lpos(tenv, s), text));
	}
	return pos-1;
}

static size_t addBlock(CTX, tenv_t *tenv, size_t pos, int lpos)
{
	int c, this_indent = 0, ch, prev = '{', level = 1;
	size_t tok_start = pos;
	kline_t uline = tenv->uline;
	while((ch = tenv->source[pos++]) != 0) {
		L_STARTLINE:;
		if(ch == '}' && prev != '\\') {
			level--;
			if(level == 0) {
				kString *text = new_kString(tenv->source + tok_start, ((pos-1)-tok_start), 0);
				addToken(_ctx, tenv, new_Token(_ctx, TK_CODE, uline, lpos, text));
				return pos;
			}
		}
		if(ch == '{' && prev != '\\') {
			level++;
		}
		else if(prev == ':' && !(ch == ' ' || ch == '\t')) {
			pos = skipURN(_ctx, tenv, pos); prev = 0; continue;
		}
		else if((prev == '#' && ch == '#') || (prev == '/' && ch == '/')) {
			pos = skipLine(_ctx, tenv, pos); prev = 0; continue;
		}
		else if(ch == '"' || ch == '`' || ch == '\'') {
			pos = skipQuote(_ctx, tenv, pos, ch); prev = 0; continue;
		}
		else if(prev == '/' && ch == '*') {
			pos = skipComment(_ctx, tenv, pos); prev = 0; continue;
		}
		prev = ch;
		if(ch == '\n') {
			tenv->uline +=1;
			prev = ch;
			break;
		}
	}
	c = 0;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\t') { c += tenv->indent_tab; continue; }
		if(ch == ' ') { c += 1; continue; }
		break;
	}
	if(ch != 0) {
		if(this_indent == 0 && c > 0) this_indent = c;
		goto L_STARTLINE;
	}
	return pos-1;
}

static void addSymbol(CTX, tenv_t *tenv, size_t s, size_t e)
{
	if(s < e) {
		kString *text = new_kString(tenv->source + s, (e-s), SPOL_ASCII|SPOL_POOL);
		int topch = S_text(text)[0];
		ktoken_t ttype = (isupper(topch)) ? TK_USYMBOL : TK_SYMBOL;
		if(!isalpha(topch)) ttype = TK_OPERATOR;
		kToken *tk = new_Token(_ctx, ttype, tenv->uline, lpos(tenv, tenv->source + s), text);
		if(S_size(text) == 1) {
			tk->topch = topch;
		}
		addToken(_ctx, tenv, tk);
	}
}

//static size_t addQuoteEsc(CTX, tenv_t *tenv, size_t pos)
//{
//	int ch = tenv->source[pos++];
//	if(ch == 'n') ch = '\n';
//	else if(ch == 't') ch = '\t';
//	else if(ch == 'r') ch = '\r';
//	else if(ch == 0) return pos-1;
//	kwb_putc(&tenv->wb, ch);
//	return pos;
//}
//
//static size_t addQuote(CTX, tenv_t *tenv, size_t pos, int quote)
//{
//	int ch, isTriple = 0;
//	kline_t uline = tenv->uline;
//	const char *qs = tenv->source - 1;
//	if(tenv->source[pos] == quote && tenv->source[pos+1] == quote) {
//		if(tenv->source[pos+2] == '\n') pos += 3; else pos += 2;
//		isTriple = 1;
//	}
//	else if(quote == '#' && tenv->source[pos] == '#') {
//		if(tenv->source[pos+1] == ' ') pos += 2; else pos += 1;
//		quote = '\n';
//	}
//	size_t tok_start = pos;
//	while((ch = tenv->source[pos++]) != 0) {
//		if(ch == '\n') {
//			if(!isTriple && quote != '\n') {
//				if(tenv->lang != NULL) {
//					WARN_LiteralMustCloseWith(_ctx, uline, lpos(tenv, qs), quote);
//				}
//			}
//			tenv->uline += 1;
//		}
//		if(ch == '\\') {
//			pos = addQuoteEsc(_ctx, tenv, pos);
//			continue;
//		}
//		// %s{a}
//		if(ch == '%') {
//		}
//		if(ch == quote) {
//			if(!isTriple || (pos-3 >= tok_start && tenv->source[pos-2] == quote && tenv->source[pos-3] == quote)) {
//				const char *s1 = kwb_top(&tenv->wb, 0);
//				size_t len = kwb_size(&tenv->wb);
//				kString *text = new_kString(s1, len, 0);
//				ktoken_t ttype = (quote == '"') ? TK_TEXT : TK_STEXT;
//				addToken(_ctx, tenv, new_Token(_ctx, ttype, uline, lpos(tenv, qs), text));
//				kwb_free((&tenv->wb));
//				return pos;
//			}
//		}
//		kwb_putc(&tenv->wb, ch);  // SLOW
//	}
//	if(tenv->lang != NULL) {
//		WARN_LiteralMustCloseWith(_ctx, uline, lpos(tenv, qs), quote);
//	}
//	return pos-1;
//}

static size_t addRawQuote(CTX, tenv_t *tenv, size_t pos, int quote)
{
	int ch, isTriple = 0;
	kline_t uline = tenv->uline;
	if(tenv->source[pos] == quote && tenv->source[pos+1] == quote) {
		if(tenv->source[pos+2] == '\n') pos += 3; else pos += 2;
		isTriple = 1;
	}
	else if(quote == '#' && tenv->source[pos] == '#') {
		if(tenv->source[pos+1] == ' ') pos += 2; else pos += 1;
		quote = '\n';
	}
	size_t tok_start = pos;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\n') {
			if(!isTriple || quote != '\n') {
				WARN_MustCloseWith(_ctx, tenv->uline, quote);
			}
			tenv->uline += 1;
		}
		if(ch == quote) {
			if(!isTriple || (pos-3 >= tok_start && tenv->source[pos-2] == quote && tenv->source[pos-3] == quote)) {
				kString *text = new_kString(tenv->source + tok_start, (pos-1)-tok_start, 0);
				ktype_t ttype = (quote == '"') ? TK_TEXT : TK_STEXT;
				addToken(_ctx, tenv, new_Token(_ctx, ttype, uline, lpos(tenv, tenv->source + tok_start), text));
				return pos;
			}
		}
	}
	return pos-1;
}

static size_t checkRegex(CTX, tenv_t *tenv, size_t pos)
{
	DBG_ASSERT(pos > 0);
	if(pos == 1) return 1;
	int i, ch,  prev = tenv->source[pos-2], next = tenv->source[pos];
	for(i = pos - 2; i >= 0; i--) {
		ch = tenv->source[i];
		if(ch == ' ' || ch == '\t') continue;
		switch(ch) {
			case '\n': case '=': case ',': case ':':
			case '(': case ')': case '[': case ']':
			case '{': case '}': case ';':
			case '+': case '-': case '*':
			case '<': case '>': case '%': case '$':
			case '!': case '&': case '?':
				return 1;  // regex
		}
	}
	int p=0, p2=0, p3 = 0;
	while((ch = tenv->source[pos++]) != '/') {
		if(ch > 127) return 1; // non ascii
		switch(ch) {
			case 0: case '\n': case ' ': return 0;  // Not
			case '(':  p++;  break;
			case '[':  p2++; break;
			case '{':  p3++; break;
			case ')':  p--; if(p < 0) return 0;   break; // Not
			case ']':  p2--; if(p2 < 0) return 0; break; // Not
			case '}':  p3--; if(p3 < 0) return 0; break; // Not
			case '\\':  return 1; // regex;
		}
	}
	if(isalnum(prev) && isalnum(next)) return 0; /* a/a+b/i */
	if(isspace(prev)) return 1; /* return /a+b/i */
	return 0;
}

static size_t addRegex(CTX,  tenv_t *tenv, size_t pos)
{
	int ch, prev = 0;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\n') {
			WARN_MustCloseWith(_ctx, tenv->uline, '/');
			tenv->uline += 1;
			break;
		}
		if(ch == '/') {
			if(prev != '\\') break;
		}
		if(prev == '\\' && ch == '\\') ch = 0;
		prev = ch;
	}
	return pos-1;
}

static int addOperator(CTX, tenv_t *tenv, int tok_start)
{
	int ch, pos = tok_start;
	while((ch = tenv->source[pos++]) != 0) {
		if(isalnum(ch)) break;
		switch(ch) {
			case '<': case '>':
			case '+': case '-': case '*': case '%': case '/':
			case '=': case '&': case '?': case ':': case '.':
			case '^': case '!': case '~': case '|':
			continue;
		}
		break;
	}
	{
		const char *s = tenv->source + tok_start;
		kString *text = new_kString(s, (pos-1)-tok_start, SPOL_ASCII|SPOL_POOL);
		kToken *tk = new_Token(_ctx, TK_OPERATOR, tenv->uline, lpos(tenv, s), text);
		if(S_size(text) == 1) {
			tk->topch = S_text(text)[0];
		}
		addToken(_ctx, tenv, tk);
	}
	return pos-1;
}

static size_t addNumber(CTX, tenv_t *tenv, int tok_start)
{
	int ch, dot = 0, pos = tok_start;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '_') continue; // nothing
		if(ch == '.') {
			if(!isdigit(tenv->source[pos])) {
				pos--;
				break;
			}
			dot++;
			continue;
		}
		if((ch == 'e' || ch == 'E') && (tenv->source[pos] == '+' || tenv->source[pos] =='-')) {
			pos++;
			continue;
		}
		if(!isalnum(ch)) break;
	}
	{
		const char *s = tenv->source + tok_start;
		kString *text = new_kString(s, (pos-1)-tok_start, SPOL_ASCII);
		int ttype = (dot == 0) ? TK_INT : TK_FLOAT;
		addToken(_ctx, tenv, new_Token(_ctx, ttype, tenv->uline, lpos(tenv, s), text));
	}
	return pos-1;
}

static int skipMultiByteChar(CTX, tenv_t *tenv, size_t tok_start)
{
	int ch, pos = tok_start;
	while((ch = tenv->source[pos]) > 127) pos++;
	const char *s = tenv->source + tok_start;
	if(FLAG_is(tenv->policy, _PERROR)) {
		IGNORE_UnxpectedMultiByteChar(_ctx, tenv->uline, lpos(tenv, s), (char*)s, pos - tok_start);
	}
	return pos;
}

static size_t addWhiteSpace(CTX, tenv_t *tenv, size_t pos)
{
	int ch, tok_start = pos;
	do {
		ch = tenv->source[pos++];
	}
	while(ch == ' ' || ch == '\t');
	addToken(_ctx, tenv, new_Token(_ctx, TK_WHITESPACE, tenv->uline, lpos(tenv, tenv->source + tok_start), TS_EMPTY));
	return pos-1;
}

static size_t addNewLine(CTX, tenv_t *tenv, size_t pos)
{
	int ch, c = 0;
	while((ch = tenv->source[pos++]) != 0) {
		if(ch == '\t') { c += tenv->indent_tab; }
		else if(ch == ' ') { c += 1; }
		break;
	}
	if(FLAG_is(tenv->policy, _INDENT)) {
		addToken(_ctx, tenv, new_Token(_ctx, TK_INDENT, tenv->uline, c, TS_EMPTY));
	}
	return pos - 1;
}

static void tokenize(CTX, tenv_t *tenv)
{
	int ch;
	size_t pos = addNewLine(_ctx, tenv, 0), tok_start = 0;

	L_NEWTOKEN:;
	tok_start = pos;
	//DBG_P("tenv->source=%d, %s", tok_start, tenv->source + tok_start);

	while((ch = tenv->source[pos++]) != 0) {
		switch(ch) {
		case '\n':
			addSymbol(_ctx, tenv, tok_start, pos-1);
			tenv->uline += 1;
			tenv->bol = tenv->source + pos;
			pos = addNewLine(_ctx, tenv, pos);
			goto L_NEWTOKEN;

		case ' ': case '\t':
			addSymbol(_ctx, tenv, tok_start, pos-1);
			if(FLAG_is(tenv->policy, _WHITESPACE)) {
				pos = addWhiteSpace(_ctx, tenv, pos);
			}
			goto L_NEWTOKEN;

		case '{':
			if(FLAG_is(tenv->policy, _UNFOLD)) {
				addSymbol(_ctx, tenv, tok_start, pos-1);
				pos = addBlock(_ctx, tenv, pos, lpos(tenv, tenv->source + (pos - 1)));
				goto L_NEWTOKEN;
			}
		case '}':
		case ';': case ',':
		case '(':  case ')':  case '[': case ']':
		case '$':  case '@':
			addSymbol(_ctx, tenv, tok_start, pos-1);
			addSymbol(_ctx, tenv, pos-1, pos);
			goto L_NEWTOKEN;

		/* token */
		case '\'': case '"': case '`' :
		case '#':
			addSymbol(_ctx, tenv, tok_start, pos-1);
			pos = addRawQuote(_ctx, tenv, pos, ch);
			goto L_NEWTOKEN;

		case '/':
			addSymbol(_ctx, tenv, tok_start, pos-1);
			ch = tenv->source[pos];  // look ahead
			if(ch == '/') {
				pos = skipLine(_ctx, tenv, pos);
			}
			else if(ch == '*') {
				pos = skipComment(_ctx, tenv, pos);
			}
			else if(checkRegex(_ctx, tenv, pos)) {
				pos = addRegex(_ctx, tenv, pos);
			}
			else {
				pos = addOperator(_ctx, tenv, pos-1);
			}
			goto L_NEWTOKEN;

		case '.':
			ch = tenv->source[pos];
			if(isdigit(ch)) {
				addSymbol(_ctx, tenv, tok_start, pos-1);
				pos = addNumber(_ctx, tenv, pos-1);
				goto L_NEWTOKEN;
			}
		case '+': case '-': case '*': case '%':
		case '<': case '>':
		case '=': case '&': case '?': case '|':
		case '^': case '!': case '~':
			addSymbol(_ctx, tenv, tok_start, pos-1);
			pos = addOperator(_ctx, tenv, pos-1);
			goto L_NEWTOKEN;

		case ':':
			ch = tenv->source[pos];
			if(isalnum(ch) || ch == '/' || ch == '-') {
				pos = addURN(_ctx, tenv, pos+1, tok_start, 0);
				goto L_NEWTOKEN;
			}
			else {
				addSymbol(_ctx, tenv, tok_start, pos-1);
				addSymbol(_ctx, tenv, pos-1, pos);
			}
			goto L_NEWTOKEN;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if(pos == tok_start+1) {
				pos = addNumber(_ctx, tenv, tok_start);
				goto L_NEWTOKEN;
			}

		default:
			if(ch > 127) {
				addSymbol(_ctx, tenv, tok_start, pos-1);
				pos = skipMultiByteChar(_ctx, tenv, pos-1);
				goto L_NEWTOKEN;
			}
		} /* switch */
	}/*while*/
	addSymbol(_ctx, tenv, tok_start, pos-1);
}


static void ktokenize(CTX, const char *source, kline_t uline, int policy, kArray *a)
{
	size_t i, pos = kArray_size(a);
	tenv_t tenv = {
		.source = source,
		.uline  = uline,
		.list   = a,
		.bol    = source,
		.indent_tab = 4,
		.policy = policy,
	};
	tokenize(_ctx, &tenv);
	if(uline == 0) {
		for(i = pos; i < kArray_size(a); i++) {
			a->tts[i]->uline = 0;
		}
	}
	//dumpTokenArray(_ctx, 0, a, pos, kArray_size(a));
}

static int findTopCh(CTX, kArray *tls, int s, int e, ktoken_t tt, int closech)
{
	int i;
	for(i = s; i < e; i++) {
		kToken *tk = tls->tts[i];
		if(tk->tt == tt && S_text(tk->text)[0] == closech) return i;
	}
	DBG_ASSERT(i != e);  // Must not happen
	return e;
}

static void makeSyntaxTree(CTX, kArray *tls, int s, int e, kArray *adst)
{
	int i;
	char nbuf[80];
	ksymbol_t nameid = 0;
	//dumpTokenArray(_ctx, 0, tls, s, e);
	for(i = s; i < e; i++) {
		kToken *tk = tls->tts[i];
		if(tk->tt == TK_TEXT || tk->tt == TK_STEXT) {
			const char *t = S_text(tk->text);
			if(t[0] == '(') {
				int ne = findTopCh(_ctx, tls, i+1, e, tk->tt, ')');
				tk->tt = AST_PARENTHESIS;
				KSETv(tk->sub, new_(Array, 0)); tk->topch = '('; tk->closech = ')';
				makeSyntaxTree(_ctx, tls, i+i, ne, tk->sub);
				i = ne;
			}
			else if(t[0] == '[') {
				int ne = findTopCh(_ctx, tls, i+1, e, tk->tt, ']');
				tk->tt = AST_BRANCET;
				KSETv(tk->sub, new_(Array, 0)); tk->topch = '['; tk->closech = ']';
				makeSyntaxTree(_ctx, tls, i+i, ne, tk->sub);
				i = ne;
			}
			else if(t[0] == '{') {
				int ne = findTopCh(_ctx, tls, i+1, e, tk->tt, '}');
				tk->tt = AST_BRACE;
				KSETv(tk->sub, new_(Array, 0)); tk->topch = '{'; tk->closech = '}';
				makeSyntaxTree(_ctx, tls, i+i, ne, tk->sub);
				i = ne;
			}
			else {
				keyword_t keyid = keyword(_ctx, S_text(tk->text), S_size(tk->text), FN_NEWID);
				tk->keyid = keyid;
				tk->tt = TK_KEYWORD;
				nameid = keyid;
			}
			kArray_add(adst, tk);
			continue;
		}
		if(tk->tt == TK_SYMBOL || tk->tt == TK_USYMBOL) {
			if(i > 0 && tls->tts[i-1]->topch == '$') {
				snprintf(nbuf, sizeof(nbuf), "$%s", S_text(tk->text));
				keyword_t keyid = keyword(_ctx, (const char*)nbuf, strlen(nbuf), FN_NEWID);
				tk->tt = TK_OPERATOR;
				tk->keyid = keyid;
				if(nameid == 0) {
					nameid = keyid;
				}
				tk->nameid = nameid;
				nameid = 0;
				kArray_add(adst, tk); continue;
			}
			if(i + 1 < e && tls->tts[i+1]->topch == ':') {
				kToken *tk = tls->tts[i];
				i++;
				nameid = keyword(_ctx, S_text(tk->text), S_size(tk->text), FN_NEWID);
				continue;
			}
		}
		if(tk->tt == TK_OPERATOR) {
			if(tk->topch == '[') {
				int ne = findTopCh(_ctx, tls, i+1, e, tk->tt, ']');
				tk->tt = AST_OPTIONAL;
				KSETv(tk->sub, new_(Array, 0)); tk->topch = '['; tk->closech = ']';
				makeSyntaxTree(_ctx, tls, i+1, ne, tk->sub);
				i = ne;
				kArray_add(adst, tk);
				continue;
			}
		}
	}
}

static void parseSyntaxRule(CTX, const char *rule, kline_t uline, kArray *a)
{
	kArray *tls = kevalshare->aBuffer;
	size_t pos = kArray_size(tls);
	ktokenize(_ctx, rule, uline, _TOPLEVEL, tls);
	makeSyntaxTree(_ctx, tls, pos, kArray_size(tls), a);
	kArray_clear(tls, pos);
	//dumpTokenArray(_ctx, 0, a, 0, kArray_size(a));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
