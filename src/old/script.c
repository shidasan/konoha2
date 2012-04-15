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

/* ************************************************************************ */

#include"commons.h"

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

#define _RETURN(s)    status = s; goto L_RETURN;

/* ------------------------------------------------------------------------ */
/* [namespace] */

kKonohaSpace* new_KonohaSpace(CTX, kKonohaSpace *parent)
{
	kKonohaSpace* ns = new_(KonohaSpace);
	DBG_ASSERT(IS_KonohaSpace(parent));
	KINITv(ns->parentNULL, parent);
	KSETv(DP(ns)->nsname, DP(parent)->nsname);
	KSETv(ns->path, parent->path);
	//LANG_LOG("ns=%p, rpath='%s'", ns, S_text(ns->path->urn));
	return ns;
}

kcid_t knh_KonohaSpace_getcid(CTX, kKonohaSpace *ns, kbytes_t sname)
{
	DBG_ASSERT(IS_KonohaSpace(ns));
	if(knh_bytes_equals(sname, STEXT("Script"))) {
		return O_cid(K_GMASCR);
	}
	L_TAIL:
	if(DP(ns)->name2ctDictSetNULL != NULL) {
		kclass_t *ct = (struct _kclass*)knh_DictSet_get(_ctx, DP(ns)->name2ctDictSetNULL, sname);
		if(ct != NULL) return ct->cid;
	}
	if(ns->parentNULL != NULL) {
		ns = ns->parentNULL;
		goto L_TAIL;
	}
	return knh_getcid(_ctx, sname);
}

kbool_t knh_KonohaSpace_isIpackdomeScope(CTX, kKonohaSpace *ns, kcid_t cid)
{
	return (knh_bytes_startsWith_(S_tobytes(ClassTBL(cid)->lname), S_tobytes(DP(ns)->nsname)));
}

/* ------------------------------------------------------------------------ */
/* [stmt] */

kflag_t knh_Stmt_flag_(CTX, kStmtExpr *stmt, kbytes_t name, kflag_t flag)
{
	if(IS_Map(DP(stmt)->metaDictCaseMap)) {
		Object *v = knh_DictMap_getNULL(_ctx, DP(stmt)->metaDictCaseMap, name);
		return (v != NULL) ? flag : 0;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */
/* [function] */

//static
//void KonohaSpace_setFuncClass(CTX, kKonohaSpace *ns, kmethodn_t mn, kcid_t c)
//{
//	if(!MN_isGETTER(mn) && !MN_isSETTER(mn)) {
//		if(!IS_DictSet(DP(ns)->func2cidDictSet)) {
//			KSETv(DP(ns)->func2cidDictSet, new_DictSet(_ctx, 0));
//		}
//		knh_DictSet_set(_ctx, DP(ns)->func2cidDictSet, knh_getFieldName(_ctx, MN_toFN(mn)), (uintptr_t)(c+1));
//	}
//}

/* ------------------------------------------------------------------------ */

kcid_t knh_KonohaSpace_getFuncClass(CTX, kKonohaSpace *ns, kmethodn_t mn)
{
//	if(!MN_isGETTER(mn) && !MN_isSETTER(mn)) {
//		kbytes_t name = S_tobytes(knh_getFieldName(_ctx, MN_toFN(mn)));
//		L_TAIL:
//		if(DP(ns)->name2ctDictSetNULL != NULL) {
//			kclass_t *ct = (kclass_t*)knh_DictSet_get(_ctx, DP(ns)->name2ctDictSetNULL, sname);
//			if(ct != NULL) return ct->cid;
//		}
//		if(ns->parentNULL != NULL) {
//			ns = ns->parentNULL;
//			goto L_TAIL;
//		}
//	}
	return TY_unknown; /* if not found */
}

ktype_t knh_KonohaSpace_gettype(CTX, kKonohaSpace *ns, kbytes_t name)
{
	if(name.utext[0] == 'v') {
		if(name.len == 4 && name.utext[1] == 'o' &&
				name.utext[2] == 'i' && name.utext[3] == 'd') return TY_void;
		if(name.len == 3 && name.utext[1] == 'a' && name.utext[2] == 'r') {
			return TY_var;
		}
	}
	return knh_KonohaSpace_getcid(_ctx, ns, name);
}

ktype_t kKonohaSpaceagcid(CTX, kKonohaSpace *o, kcid_t cid, kbytes_t tag)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	kcid_t bcid = ClassTBL(cid)->bcid;
	knh_printf(_ctx, cwb->w, "%C:%B", bcid, tag);
	cid = knh_KonohaSpace_getcid(_ctx, o, CWB_tobytes(cwb));
	CWB_close(_ctx, cwb);
	return cid;
}

/* ------------------------------------------------------------------------ */
/* [include] */

static kTerm * new_TermEVALED(CTX)
{
	kTerm *tk = new_(Term);
	TT_(tk) = TT_CONST;
	KSETv(tk->data, ctx->evaled);
	tk->type = O_cid(_ctx->evaled);
	return tk;
}

static void *knh_open_gluelink(CTX, kStmtExpr *stmt, kKonohaSpace *ns, kbytes_t libname)
{
	void *p = NULL;
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	knh_buff_addpath(_ctx, cwb->ba, cwb->pos, 0, B(ns->path->ospath));
	knh_buff_trim(_ctx, cwb->ba, cwb->pos, '.');
	knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, STEXT(K_OSDLLEXT));
	kwb_putc(cwb->ba, 0); // to avoid concat;
	p = knh_dlopen(_ctx, CWB_totext(_ctx, cwb));
	if(p == NULL) {
		kStmtExproERR(_ctx, stmt, ERROR_NotFound(_ctx, "gluelink", CWB_totext(_ctx, cwb)));
	}
	if(p != NULL) {
		knh_Fpkginit pkginit = (knh_Fpkginit)knh_dlsym(_ctx, p, "init", libname.text, 1/*isTest*/);
		if(pkginit != NULL) {
			const knh_PackageDef_t *pkgdef = pkginit(_ctx, knh_getLoaderAPI());
			if((long)pkgdef->crc32 == (long)K_API2_CRC32) {
				KNH_NTRACE2(_ctx, "konoha:opengluelink", K_OK,
						KNH_LDATA(LOG_s("package_name", pkgdef->name),
							LOG_i("package_buildid", pkgdef->buildid),
							LOG_u("package_crc32", pkgdef->crc32)));
			}
			else {
				KNH_NTRACE2(_ctx, "konoha:opengluelink", K_FAILED,
						KNH_LDATA(LOG_s("package_name", pkgdef->name),
							LOG_i("package_buildid", pkgdef->buildid),
							LOG_u("package_crc32", pkgdef->crc32)));
				p = NULL;
			}
		}
		else {
			p = NULL;
		}
		if(p == NULL) {
			kStmtExproERR(_ctx, stmt, ERROR_NotFound(_ctx, "compatible gluelink", CWB_totext(_ctx, cwb)));
		}
		else {
			knh_Stmt_done(_ctx, stmt);
		}
	}
	CWB_close(_ctx, cwb);
	return p;
}

static void *knh_open_ffilink(CTX, kKonohaSpace *ns, kbytes_t libname)
{
	void *p = NULL;
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	libname = CWB_ensure(_ctx, cwb, libname, K_PATHMAX);
	knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, libname);
	knh_buff_trim(_ctx, cwb->ba, cwb->pos, '.');
	knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, STEXT(K_OSDLLEXT));
	if(p == NULL && !knh_bytes_startsWith_(libname, STEXT("lib"))) {
		CWB_clear(cwb, 0);
		knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, STEXT("lib"));
		knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, libname);
		knh_buff_trim(_ctx, cwb->ba, cwb->pos, '.');
		knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, STEXT(K_OSDLLEXT));
		p = knh_dlopen(_ctx, CWB_totext(_ctx, cwb));
	}
	CWB_close(_ctx, cwb);
	return p;
}

static void INCLUDE_ffilink(CTX, kStmtExpr *stmt, kKonohaSpace *ns, kbytes_t path)
{
	kbytes_t libname = knh_bytes_next(path, ':');
	if(libname.text[0] == '*' || knh_bytes_equals(libname, STEXT("gluelink"))) {
		ns->gluehdr = knh_open_gluelink(_ctx, stmt, ns, libname);
	}
	else {
		void *p = knh_open_ffilink(_ctx, ns, libname);
		if(p != NULL) {
			if(DP(ns)->ffilinksNULL == NULL) {
				KINITv(DP(ns)->ffilinksNULL, new_Array0(_ctx, 0));
			}
			kArray_add(DP(ns)->ffilinksNULL, new_Pointer(_ctx, "dl", p, NULL));
			knh_Stmt_done(_ctx, stmt);
		}
		else {
			kStmtExproERR(_ctx, stmt, ERROR_NotFound(_ctx, "library", libname.text));
		}
	}
}

static void KonohaSpace_beginINCLUDE(CTX, kKonohaSpace *newns, kKonohaSpace *oldns)
{
	knh_KonohaSpaceEX_t *tb = DP(newns);
	void *tdlhdr = newns->gluehdr;
#ifdef K_USING_BMGC
	knh_KonohaSpaceEX_t tmp;
	knh_memcpy(&tmp, tb, sizeof(knh_KonohaSpaceEX_t));
	knh_memcpy(DP(newns), DP(oldns), sizeof(knh_KonohaSpaceEX_t));
	knh_memcpy(DP(oldns), &tmp, sizeof(knh_KonohaSpaceEX_t));
	newns->gluehdr = oldns->gluehdr;
	oldns->gluehdr = tdlhdr;
#else
	newns->b = oldns->b;
	newns->gluehdr = oldns->gluehdr;
	oldns->b = tb;
	oldns->gluehdr = tdlhdr;
#endif
}

static void KonohaSpace_endINCLUDE(CTX, kKonohaSpace *newns, kKonohaSpace *oldns)
{
	knh_KonohaSpaceEX_t *tb = DP(newns);
	void *tdlhdr = newns->gluehdr;
#ifdef K_USING_BMGC
	knh_KonohaSpaceEX_t tmp;
	knh_memcpy(&tmp, tb, sizeof(knh_KonohaSpaceEX_t));
	knh_memcpy(DP(newns), DP(oldns), sizeof(knh_KonohaSpaceEX_t));
	knh_memcpy(DP(oldns), &tmp, sizeof(knh_KonohaSpaceEX_t));
	newns->gluehdr = oldns->gluehdr;
	oldns->gluehdr = tdlhdr;
#else
	newns->b = oldns->b;
	newns->gluehdr = oldns->gluehdr;
	oldns->b = tb;
	oldns->gluehdr = tdlhdr;
#endif
}

kbool_t knh_KonohaSpace_include(CTX, kKonohaSpace *ns, kPath *pth)
{
	kKonohaSpace *newns = new_KonohaSpace(_ctx, ns);
	KSETv(ctx->gma->scr->ns, newns);
	KonohaSpace_beginINCLUDE(_ctx, newns, ns);
	kstatus_t res = knh_load(_ctx, pth);
	KonohaSpace_endINCLUDE(_ctx, newns, ns);
	KSETv(ctx->gma->scr->ns, ns);
	return (res == K_CONTINUE);
}

static void SCRIPT_eval(CTX, kStmtExpr *stmt, int isCompileOnly);

static void INCLUDE_file(CTX, kStmtExpr *stmt)
{
	kTerm *tkRES = Tn_typing(_ctx, stmt, 0, CLASS_Path, 0);
	if(TT_(tkRES) != TT_ERR) {
		if(!Tn_isCONST(stmt, 0)) {
			kStmtExpr *stmt2 = new_Stmt2(_ctx, STT_RETURN, stmtNN(stmt, 0), NULL);
			SCRIPT_eval(_ctx, stmt2, 0/*isCompileOnly*/);
			if(STT_(stmt2) == STT_ERR) {
				kStmtExproERR(_ctx, stmt, tkNN(stmt2, 0));
				return;
			}
			KSETv(tkNN(stmt, 0), new_TermEVALED(_ctx));
		}
		kPath *pth = (kPath*)tkNN(stmt,0)->data;
		if(!knh_KonohaSpace_include(_ctx, K_GMANS, pth)) {
			kStmtExproERR(_ctx, stmt, ERROR_NotFound(_ctx, "include path:", S_text(pth->urn)));
		}
		else {
			knh_Stmt_done(_ctx, stmt);
		}
	}
	else {
		kStmtExproERR(_ctx, stmt, tkRES);
	}
}

static void INCLUDE_eval(CTX, kStmtExpr *stmt)
{
	kTerm *tkPATH = tkNN(stmt, 0);
	if(IS_bString(tkPATH->text) && knh_bytes_startsWith_(S_tobytes(tkPATH->text), STEXT("lib:"))) {
		INCLUDE_ffilink(_ctx, stmt, K_GMANS, S_tobytes(tkPATH->text));
	}
	else {
		INCLUDE_file(_ctx, stmt);
	}
}

/* ------------------------------------------------------------------------ */
/* [package] */

static kbool_t isFoundPackage(CTX, kBytes* ba, size_t pos, kbytes_t tpath, kbytes_t bpath)
{
	knh_Bytes_clear(ba, pos);
	knh_buff_addospath(_ctx, ba, pos, 0, tpath);
	knh_buff_addospath(_ctx, ba, pos, 1, bpath); // konoha.math
	knh_buff_addospath(_ctx, ba, pos, 1, knh_bytes_rnext(bpath, '.')); // math
	knh_buff_addospath(_ctx, ba, pos, 0, STEXT(".k"));
	return knh_buff_isfile(_ctx, ba, pos);
}

static kbool_t knh_buff_addPackagePath(CTX, kBytes *ba, size_t pos, kbytes_t pkgname)
{
	char *epath = knh_getenv("KONOHA_PACKAGE");
	if(epath != NULL && isFoundPackage(_ctx, ba, pos, B(epath), pkgname)) {
		return 1;
	}
	kString *tpath = knh_getPropertyNULL(_ctx, STEXT("konoha.package.path"));
	if(tpath != NULL && isFoundPackage(_ctx, ba, pos, S_tobytes(tpath), pkgname)) {
		return 1;
	}
	tpath = knh_getPropertyNULL(_ctx, STEXT("user.package.path"));
	if(tpath != NULL && isFoundPackage(_ctx, ba, pos, S_tobytes(tpath), pkgname)) {
		return 1;
	}
	return 0;
}

void knh_Script_setNSName(CTX, kScript* scr, kString *nsname)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	KSETv(DP(scr->ns)->nsname, nsname);
	knh_Bytes_write(_ctx, cwb->ba, S_tobytes(nsname));
	knh_Bytes_write(_ctx, cwb->ba, STEXT(".Script"));
	KSETv(((struct _kclass*)O_ct(scr))->lname, CWB_newString(_ctx, cwb, SPOL_ASCII));
}

typedef void (*knh_Fpkgcomplete)(CTX);

/* ------------------------------------------------------------------------ */
kstatus_t knh_loadPackage(CTX, kbytes_t pkgname)
{
	kstatus_t status = K_CONTINUE;
	kDictMap *dmap = ctx->share->packageDictMap;
	kScript *scr = (kScript*)knh_DictMap_getNULL(_ctx, dmap, pkgname);
	if(scr == NULL) {
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		if(knh_buff_addPackagePath(_ctx, cwb->ba, cwb->pos, pkgname)) {
			FILE *fp = fopen(CWB_totext(_ctx, cwb), "r");
			if(fp != NULL) {
				kString *pname = new_S(pkgname.text, pkgname.len);
				kScript *newscr = new_(Script);
				knh_Script_setNSName(_ctx, newscr, pname);
				knh_DictMap_set(_ctx, dmap, pname, newscr);
				kfileid_t fileid = knh_getURI(_ctx, CWB_tobytes(cwb));
				kline_t uline = 1;
				ULINE_setURI(uline, fileid);
				KSETv(newscr->ns->path, new_Path(_ctx, knh_buff_newRealPathString(_ctx, cwb->ba, cwb->pos)));
				/* */
				kInputStream *in = new_InputStream(_ctx, new_FILE(_ctx, fp, 256), newscr->ns->path);
				DBG_P("rpath='%s'", newscr->ns->path->ospath);
				scr = ctx->gma->scr;
				KSETv(ctx->gma->scr, newscr);
				status = knh_InputStream_load(_ctx, in, uline);
				if (newscr->ns->gluehdr) {
					void *p = newscr->ns->gluehdr;
					knh_Fpkgcomplete pkgcomplete = (knh_Fpkgcomplete) knh_dlsym(_ctx, p, "complete", NULL, 1/*isTest*/);
					if (pkgcomplete)
						pkgcomplete(_ctx);
				}
				KSETv(ctx->gma->scr, scr);
			}
		}
		else {
			ERROR_NotFound(_ctx, "package", pkgname.text);
			status = K_BREAK;
		}
		CWB_close(_ctx, cwb);
	}
	return status;
}

/* ------------------------------------------------------------------------ */

static void KonohaSpace_setcid(CTX, kKonohaSpace *ns, kString *name, kcid_t cid)
{
	if(DP(ns)->name2ctDictSetNULL == NULL) {
		KINITv(DP(ns)->name2ctDictSetNULL, new_DictSet0(_ctx, 0, 1/*isCaseMap*/, "KonohaSpace.name2cid"));
	}
	knh_DictSet_set(_ctx, DP(ns)->name2ctDictSetNULL, name, (uintptr_t)ClassTBL(cid));
}

static void KonohaSpace_setDPI(CTX, kKonohaSpace *ns, kbytes_t pkgname, kDictSet *ds)
{
	size_t i, size = knh_Map_size(ds);
	for(i = 0; i < size; i++) {
		kString *name = knh_DictSet_keyAt(ds, i);
		kbytes_t key = S_tobytes(name);
		if(key.text[pkgname.len] == '.' && knh_bytes_startsWith_(key, pkgname)) {
			kbytes_t t = knh_bytes_last(key, pkgname.len+1);
			if(knh_bytes_index(t, '.') == -1) {
				knh_DictMap_set(_ctx, DP(ns)->name2dpiNameDictMapNULL, new_kString(t.text, t.len, SPOL_POOL), name);
			}
		}
	}
}

static void KonohaSpace_addDPI(CTX, kKonohaSpace *ns, kbytes_t pkgname)
{
	if(DP(ns)->name2dpiNameDictMapNULL == NULL) {
		KINITv(DP(ns)->name2dpiNameDictMapNULL, new_DictSet0(_ctx, 0, 1/*isCaseMap*/, "KonohaSpace.name2dpiName"));
	}
	KonohaSpace_setDPI(_ctx, ns, pkgname, ctx->share->streamDpiDictSet);
	KonohaSpace_setDPI(_ctx, ns, pkgname, ctx->share->mapDpiDictSet);
	KonohaSpace_setDPI(_ctx, ns, pkgname, ctx->share->convDpiDictSet);
}

static int StmtUSINGCLASS_eval(CTX, kStmtExpr *stmt, size_t n)
{
	kTerm *tkPKG = tkNN(stmt, n), *tkN;
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	knh_Bytes_write(_ctx, cwb->ba, S_tobytes((tkPKG)->text));
	while(1) {
		tkN = tkNN(stmt, ++n);
		if(TT_(tkN) == TT_ASIS) break;
		if(TT_(tkN) == TT_DOT) continue;
		if(TT_(tkN) == TT_NAME) {
			kwb_putc(cwb->ba, '.');
			knh_Bytes_write(_ctx, cwb->ba, S_tobytes((tkN)->text));
			continue;
		}
		break;
	}
	KSETv((tkPKG)->data, CWB_newString(_ctx, cwb, SPOL_ASCII));
	if(knh_loadPackage(_ctx, S_tobytes((tkPKG)->text)) == K_CONTINUE) {
		kKonohaSpace *ns = K_GMANS;
		if(TT_(tkN) == TT_MUL) {
			kbytes_t pkgname = S_tobytes((tkPKG)->text);
			size_t cid;
			for(cid = 0; cid < ctx->share->sizeClassTBL; cid++) {
				if(ClassTBL(cid)->lname == NULL) continue;
				if(class_isPrivate(cid) && C_isGenerics(cid)) continue;
				kbytes_t cname = S_tobytes(ClassTBL(cid)->lname);
				if(cname.utext[pkgname.len] == '.' && isupper(cname.utext[pkgname.len+1]) && knh_bytes_startsWith_(cname, pkgname)) {
					KonohaSpace_setcid(_ctx, ns, ClassTBL(cid)->sname, (kcid_t)cid);
				}
			}
			KonohaSpace_addDPI(_ctx, ns, pkgname);
		}
		else if(TT_(tkN) == TT_UNAME) {
			kcid_t newcid;
			kString* cname = (tkN)->text;
			CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
			knh_Bytes_write(_ctx, cwb->ba, S_tobytes((tkPKG)->text));
			kwb_putc(cwb->ba, '.');
			knh_Bytes_write(_ctx, cwb->ba, S_tobytes(cname));
			newcid = knh_getcid(_ctx, CWB_tobytes(cwb));
			if(newcid == TY_unknown) {
				KSETv((tkPKG)->data, CWB_newString(_ctx, cwb, SPOL_ASCII));
				CWB_close(_ctx, cwb);
				goto L_ERROR;
			}
			else {
				KonohaSpace_setcid(_ctx, ns, cname, newcid);
			}
			CWB_close(_ctx, cwb);
		}
		knh_Stmt_done(_ctx, stmt);
		return 1;
	}
	L_ERROR:;
	while(n + 1 < DP(stmt)->size) {
		if(TT_(tkNN(stmt, n)) == TT_OR) return 0;
		n++;
	}
	kStmtExproERR(_ctx, stmt, ERROR_NotFound(_ctx, "package", S_text((tkPKG)->text)));
	return 0;
}

static void USING_eval(CTX, kStmtExpr *stmt)
{
	size_t n = 0;
	L_TRYAGAIN:; {
		kTerm *tkF = tkNN(stmt, n);
		if(TT_(tkF) == TT_NAME) {
			if(StmtUSINGCLASS_eval(_ctx, stmt, n)) {
				goto L_RETURN;
			}
		}
	}
	while(n + 1 < DP(stmt)->size) {  // find next or
		kTerm *tkF = tkNN(stmt, n);
		n++;
		if(TT_(tkF) == TT_OR) goto L_TRYAGAIN;
	}
	L_RETURN:;
	if(STT_(stmt) != STT_ERR) {
		knh_Stmt_done(_ctx, stmt);
	}
}

/* ------------------------------------------------------------------------ */

static kMethod *Script_getEvalMethod(CTX, kScript *scr, ktype_t it_type)
{
	kMethod *mtd = knh_KonohaSpace_getMethodNULL(_ctx, K_GMANS, O_cid(scr), MN_);
	if(mtd == NULL) {
		kParam *pa = new_(Param);
		knh_Param_addParam(_ctx, pa, it_type, FN_it);
		knh_Param_addReturnType(_ctx, pa, TY_void);
		mtd = new_Method(_ctx, FLAG_Method_Hidden, O_cid(scr), MN_LAMBDA, NULL);
		KSETv(DP(mtd)->mp, pa);
		knh_KonohaSpace_defineMethod(_ctx, O_cid(scr), mtd);
	}
	else {
		kparam_t *p = knh_Param_get(DP(mtd)->mp, 0);
		p->type = it_type;
		//p = knh_Param_rget(DP(mtd)->mp, 0);
		/*p->type = TY_void;*/
	}
	return mtd;
}

void SCRIPT_asm(CTX, kStmtExpr *stmtITR);

static void SCRIPT_eval(CTX, kStmtExpr *stmtORIG, int isCompileOnly)
{
	BEGIN_LOCAL(_ctx, lsfp, 5);
	kScript *scr = K_GMASCR;
	kcid_t cid =  O_cid(_ctx->evaled);
	kMethod *mtd = Script_getEvalMethod(_ctx, scr, cid);
	kStmtExpr *stmt = stmtORIG;
	WCTX(_ctx)->isEvaled = 0;
	if(STT_isExpr(STT_(stmt)) && STT_(stmt) != STT_LET) {
		stmt = new_Stmt2(_ctx, STT_RETURN, stmt, NULL);
	}
	if(STT_(stmt) == STT_RETURN) {
		Stmt_setImplicit(stmt, 1);
	}
	KSETv(lsfp[0].o, stmt);
	// CompilerAPI
	{
		void *compilerAPI = ctx->share->compilerAPI;
		ctx->wshare->compilerAPI = NULL;
		knh_Method_asm(_ctx, mtd, stmt, typingMethod2);
		ctx->wshare->compilerAPI = compilerAPI;
	}
	if(STT_(stmt) == STT_ERR) {
		if(stmt != stmtORIG) {
			kStmtExproERR(_ctx, stmtORIG, tkNN(stmt, 0));
		}
		END_LOCAL(_ctx, lsfp);
		return;
	}
	if(!isCompileOnly) {
		int rtnidx=3+1, thisidx = rtnidx + K_CALLDELTA;
		DP(mtd)->fileid = ULINE_fileid(stmt->uline);
		KSETv(lsfp[1+1].o, DP(mtd)->kcode);
		lsfp[thisidx+K_PCIDX].pc = NULL;
		klr_setmtdNC(_ctx, lsfp[thisidx+K_MTDIDX], mtd);
		KSETv(lsfp[thisidx].o, scr);
		KSETv(lsfp[thisidx+1].o, ctx->evaled);
		lsfp[thisidx+1].ndata = O_data(_ctx->evaled);
		klr_setesp(_ctx, lsfp + thisidx+2);
		if(knh_VirtualMachine_launch(_ctx, lsfp + thisidx)) {
			//DBG_P("returning sfpidx=%d, rtnidx=%d, %s %lld %ld %f", sfpidx_, sfpidx_ + rtnidx, O__(lsfp[rtnidx].o), lsfp[rtnidx].ivalue, lsfp[rtnidx].bvalue, lsfp[rtnidx].fvalue);
			if(STT_(stmt) == STT_RETURN && !Stmt_isImplicit(stmt)) {
				cid = O_cid(lsfp[rtnidx].o);
				KSETv(WCTX(_ctx)->evaled, lsfp[rtnidx].o);
				WCTX(_ctx)->isEvaled = 1;
			}
		}
		else {
			KNH_TODO("VirtualMachine return error status");
		}
	}
	knh_Stmt_done(_ctx, stmtORIG);
	END_LOCAL(_ctx, lsfp);
}

static void StmtITR_eval(CTX, kStmtExpr *stmtITR);

static void IF_eval(CTX, kStmtExpr *stmt)
{
	kTerm *tkRES = Tn_typing(_ctx, stmt, 0, TY_Boolean, 0);
	if(TT_(tkRES) != TT_ERR) {
		int isTrue = 1;
		if(!Tn_isCONST(stmt, 0)) {
			kStmtExpr *stmt2 = new_Stmt2(_ctx, STT_RETURN, stmtNN(stmt, 0), NULL);
			SCRIPT_eval(_ctx, stmt2, 0/*isCompileOnly*/);
			if(STT_(stmt2) == STT_ERR) {
				kStmtExproERR(_ctx, stmt, tkNN(stmt2, 0)); return;
			}
			isTrue = IS_TRUE(_ctx->evaled) ? 1 : 0;
		}
		else {
			isTrue = (Tn_isTRUE(stmt, 0)) ? 1: 0;
		}
		kStmtExpr *stmtIN = stmtNN(stmt, isTrue ? 1 : 2);
		StmtITR_eval(_ctx, stmtIN);
		if(STT_(stmtIN) == STT_ERR) {
			kStmtExproERR(_ctx, stmt, tkNN(stmtIN, 0));
		}
		else {
			knh_Stmt_done(_ctx, stmt);
		}
	}
	else {
		kStmtExproERR(_ctx, stmt, tkRES);
	}
}

Object *knh_KonohaSpace_getConstNULL(CTX, kKonohaSpace *ns, kbytes_t name)
{
	L_TAIL:
	if(DP(ns)->constDictCaseMapNULL != NULL) {
		kindex_t idx = knh_DictMap_index(DP(ns)->constDictCaseMapNULL, name);
		if(idx != -1) return knh_DictMap_valueAt(DP(ns)->constDictCaseMapNULL, idx);
	}
	if(ns->parentNULL != NULL) {
		ns = ns->parentNULL;
		goto L_TAIL;
	}
	return knh_getClassConstNULL(_ctx, DP(_ctx->gma)->this_cid, name);
}

static void CONST_decl(CTX, kStmtExpr *stmt)
{
	kTerm *tkN = tkNN(stmt, 0), *tkRES = NULL;
	kcid_t cid = knh_Term_cid(_ctx, tkN, TY_unknown);
	kKonohaSpace *ns = K_GMANS;
	Object *value = knh_KonohaSpace_getConstNULL(_ctx, ns, TK_tobytes(tkN));
	if(cid != TY_unknown || value != NULL) {
		WARN_AlreadyDefined(_ctx, "const", UPCAST(tkN));
		knh_Stmt_done(_ctx, stmt);
		return;
	}
	tkRES = Tn_typing(_ctx, stmt, 1, T_dyn, 0);
	if(TT_(tkRES) != TT_ERR) {
		if(Tn_isCONST(stmt, 1)) {
			value = Tn_const(stmt, 1);
		}
		else {
			kStmtExpr *stmt2 = new_Stmt2(_ctx, STT_RETURN, stmtNN(stmt, 1), NULL);
			SCRIPT_eval(_ctx, stmt2, 0/*isCompileOnly*/);
			if(STT_(stmt2) == STT_ERR) {
				kStmtExproERR(_ctx, stmt, tkNN(stmt2, 0)); return;
			}
			value = ctx->evaled;
		}
		if(IS_Class(value)) {
			KonohaSpace_setcid(_ctx, ns, tkN->text, ((kClass*)value)->cid);
		}
		else {
			if(DP(ns)->constDictCaseMapNULL == NULL) {
				KINITv(DP(ns)->constDictCaseMapNULL,
					new_DictMap0(_ctx, 0, 1/*isCaseMap*/, "KonohaSpace.lconstDictMap"));
			}
			knh_DictMap_set_(_ctx, DP(ns)->constDictCaseMapNULL, tkN->text, value);
		}
		knh_Stmt_done(_ctx, stmt);
	}
	else {
		kStmtExproERR(_ctx, stmt, tkRES);
	}
}

//static kstatus_t METHODWITH_eval(CTX, kStmtExpr *stmt)
//{
//	kstatus_t status = K_CONTINUE;
//	if(StmtMETHOD_isFFI(stmt)) {
//		kTerm *tkRES = Tn_typing(_ctx, stmt, 4, T_Map, 0);
//		if(TT_(tkRES) != TT_ERR) {
//			if(!Tn_isCONST(stmt, 4)) {
//				kStmtExpr *stmt2 = new_Stmt2(_ctx, STT_RETURN, stmtNN(stmt, 4), NULL);
//				status = SCRIPT_eval(_ctx, stmt2, 0/*isCompileOnly*/);
//				if(status != K_CONTINUE) return status;
//				KSETv(tkNN(stmt, 4), new_TermEVALED(_ctx));
//			}
//		}
//		else {
//			kStmtExproERR(_ctx, stmt, tkRES);
//			return K_BREAK;
//		}
//	}
//	return status;
//}

/* ------------------------------------------------------------------------ */
/* [CLASS] */

static kflag_t knh_StmtCLASS_flag(CTX, kStmtExpr *stmt)
{
	kflag_t flag = 0;
	if(IS_Map(DP(stmt)->metaDictCaseMap)) {
		flag |= knh_Stmt_flag(_ctx, stmt, "Final",     FLAG_Class_Final);
		flag |= knh_Stmt_flag(_ctx, stmt, "Singleton", FLAG_Class_Singleton);
		flag |= knh_Stmt_flag(_ctx, stmt, "Immutable", FLAG_Class_Immutable);
		flag |= knh_Stmt_flag(_ctx, stmt, "Expando",   FLAG_Class_Expando);
	}
	return flag;
}


static void knh_loadNativeClass(CTX, const char *cname, kclass_t *ct)
{
	char fname[256];
	kKonohaSpace *ns = K_GMANS;
	kclass_t *cdef = NULL;
	if(ns->gluehdr != NULL) {
		knh_snprintf(fname, sizeof(fname), "def%s", cname);
		knh_Fclassdef classdef = (knh_Fclassdef)knh_dlsym(_ctx, ns->gluehdr, fname, cname, 0/*isTest*/);
		if(classdef != NULL) {
			kclass_t *cdefbuf = (struct _kclass*)KNH_ZMALLOC(sizeof(kclass_t));
			knh_memcpy(cdefbuf, knh_getDefaultClassDef(), sizeof(kclass_t));
			LANG_LOG("loading glue func: %s", fname);
			classdef(_ctx, ct->cid, cdefbuf);
			cdefbuf->asize = sizeof(kclass_t);
			cdef = (kclass_t*)cdefbuf;
		}
	}
	if(cdef == NULL) {
		cdef = knh_getCppClassDef(); // this might be C++ Object
		WARN_NotFound(_ctx, _("ClassDef function"), fname);
	}
	ct->bcid = ct->cid;
	ct->baseTBL = ct;
	ct->cflag = ct->cflag | cdef->cflag;
	knh_setClassDef(_ctx, ct, cdef);
	ct->magicflag = KNH_MAGICFLAG(ct->cflag);
	if(ns->gluehdr != NULL) {
		knh_snprintf(fname, sizeof(fname), "const%s", cname);
		knh_Fconstdef constdef = (knh_Fconstdef)knh_dlsym(_ctx, ns->gluehdr, fname, NULL, 0/*isTest*/);
		if(constdef != NULL) {
			LANG_LOG("loading glue func: %s", fname);
			constdef(_ctx, ct->cid, knh_getLoaderAPI());
		}
	}
}

/* ------------------------------------------------------------------------ */

static void ClassTBL_inherit(CTX, kclass_t *ct, kclass_t *supct) {
	ct->supTBL = ClassTBL(ct->supcid);
	ct->kwx = supct->kwx;
	ct->xdataidx = supct->xdataidx;
	((struct _kclass*)supct)->subclass += 1;
	ct->bcid = supct->bcid;
	ct->baseTBL = ClassTBL(supct->bcid);

}

static kclass_t *CLASSNAME_decl(CTX, kStmtExpr *stmt, kTerm *tkC, kTerm *tkE)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	knh_Bytes_write(_ctx, cwb->ba, S_tobytes(DP(K_GMANS)->nsname));
	kwb_putc(cwb->ba, '.');
	knh_Bytes_write(_ctx, cwb->ba, TK_tobytes(tkC));
	kcid_t cid = knh_getcid(_ctx, CWB_tobytes(cwb));
	kclass_t *ct = NULL;
	if(cid == TY_unknown) {  // new class //
		cid = new_ClassId(_ctx);
		ct = varClassTBL(cid);
		knh_setClassName(_ctx, cid, CWB_newString(_ctx, cwb, SPOL_ASCII), (tkC)->text);
		ct->cflag  = knh_StmtCLASS_flag(_ctx, stmt);
		ct->magicflag  = KNH_MAGICFLAG(ct->cflag);
		KonohaSpace_setcid(_ctx, K_GMANS, (tkC)->text, cid);
		KINITv(ct->methods, K_EMPTYARRAY);
		KINITv(ct->typemaps, K_EMPTYARRAY);

		// class C extends E ..
		if(TT_(tkE) == TT_NEXT) { // class C extends C++
			ct->supcid = CLASS_Tdynamic;
			ClassTBL_inherit(_ctx, ct, ClassTBL(ct->supcid));
			knh_setClassDef(_ctx, ct, knh_getCppClassDef());
		}
		else {
			ct->supcid = knh_Term_cid(_ctx, tkE, TY_unknown);
			if(ct->supcid == TY_unknown) {
				kStmtExproERR(_ctx, stmt, ERROR_Undefined(_ctx, "class", ct->supcid, tkE));
				return ct;
			}
			if(class_isFinal(ct->supcid)) {
				kStmtExproERR(_ctx, stmt, ERROR_TermIs(_ctx, tkE, "final"));
				return ct;
			}
			ClassTBL_inherit(_ctx, ct, ClassTBL(ct->supcid));
			knh_setClassDef(_ctx, ct, ct->baseTBL->cdef);
		}
	}
	else {
		ct = varClassTBL(cid);
		if(ct->bcid == CLASS_Object || ct->bcid == CLASS_CppObject) {
			if(ct->supTBL->fsize == ct->fsize) goto L_RETURN;  // class C;
		}
		kStmtExproERR(_ctx, stmt, ERROR_AlreadyDefined(_ctx, "class", (tkC)->data));
	}
	L_RETURN:;
	(tkC)->cid = cid;
	CWB_close(_ctx, cwb);
	return ct;
}

static void CLASS_decl(CTX, kStmtExpr *stmt)
{
	kTerm *tkC = tkNN(stmt, 0); // CNAME
	kTerm *tkE = tkNN(stmt, 2); // extends
	kclass_t *ct = CLASSNAME_decl(_ctx, stmt, tkC, tkE);
	if(STT_(stmt) == STT_ERR) return;
	if(knh_StmtMETA_is(_ctx, stmt, "Native")) {
		knh_loadNativeClass(_ctx, S_text((tkC)->text), ct);
	}
	if(ct->protoNULL == NULL && (ct->bcid == CLASS_Object || ct->bcid == CLASS_CppObject)) {
		kObject *nulval = new_hObject_(_ctx, ct);
		kObject *defval = new_hObject_(_ctx, ct);
		KNH_ASSERT(nulval->fields == NULL);
		KNH_ASSERT(defval->fields == NULL);
		Object_setNullObject(nulval, 1);
		knh_setClassDefaultValue(_ctx, ct->cid, nulval, NULL);
		KINITv(ct->protoNULL, defval);
	}
	if(DP(stmt)->size == 4 && TT_(tkNN(stmt, 1)) == TT_ASIS) {
		knh_Stmt_done(_ctx, stmt);
	}
}

static void StmtITR_eval(CTX, kStmtExpr *stmtITR)
{
	BEGIN_LOCAL(_ctx, lsfp, 3);
	kStmtExpr *stmt = stmtITR;
	KSETv(lsfp[0].o, stmtITR); // lsfp[1] stmtNEXT
	while(stmt != NULL) {
		kStmtExpr *stmtNEXT = DP(stmt)->nextNULL;
		if(stmtNEXT != NULL) {
			KSETv(lsfp[1].o, stmt);
			KSETv(lsfp[2].o, stmtNEXT);
			KNH_FINALv(_ctx, DP(stmt)->nextNULL);
			DP(stmt)->nextNULL = NULL;
		}
		ctxcode->uline = stmt->uline;
		switch(STT_(stmt)) {
		case STT_NAMESPACE:
		{
			kKonohaSpace *ns = new_KonohaSpace(_ctx, K_GMANS);
			kStmtExpr *stmtIN = stmtNN(stmt, 0);
			KSETv(K_GMANS, ns);
			StmtITR_eval(_ctx, stmtIN);
			DBG_ASSERT(K_GMANS == ns);
			DBG_ASSERT(ns->parentNULL != NULL);
			KSETv(K_GMANS, ns->parentNULL);
			if(STT_(stmtIN) == STT_ERR) {
				kStmtExproERR(_ctx, stmtITR, tkNN(stmtIN, 0));
				goto L_RETURN;
			}
			knh_Stmt_done(_ctx, stmt);
			break;
		}
		case STT_IF: /* Conditional Compilation */
			IF_eval(_ctx, stmt); break;
		case STT_INCLUDE:
			INCLUDE_eval(_ctx, stmt); break;
		case STT_USING:
			USING_eval(_ctx, stmt); break;
		case STT_CLASS:
			CLASS_decl(_ctx, stmt); break;
//		case STT_METHOD:  /* with clause */
//			status = METHODWITH_eval(_ctx, stmt); break;
		case STT_CONST:
			CONST_decl(_ctx, stmt); break;
		case STT_BREAK:
			knh_Stmt_done(_ctx, stmt);
			goto L_RETURN;
		}
		if(STT_(stmt) == STT_ERR) {
			if(stmt != stmtITR) {
				kStmtExproERR(_ctx, stmtITR, tkNN(stmt, 0));
			}
			goto L_RETURN;
		}
		if(STT_(stmt) != STT_DONE) {
			SCRIPT_typing(_ctx, stmt);
			SCRIPT_asm(_ctx, stmt);
		}
		if(STT_(stmt) != STT_DONE) {
			SCRIPT_eval(_ctx, stmt, knh_isCompileOnly(_ctx));
			if(STT_(stmt) == STT_ERR) {
				if(stmt != stmtITR) {
					kStmtExproERR(_ctx, stmtITR, tkNN(stmt, 0));
				}
				goto L_RETURN;
			}
		}
		stmt = stmtNEXT;
	}

	L_RETURN:;
	END_LOCAL(_ctx, lsfp);
	ctxcode->uline = 0;
}

#ifdef K_USING_SUGAR

kbool_t knh_beval2(CTX, const char *script, kline_t uline)
{
	kbool_t tf;
	INIT_GCSTACK(_ctx);
	kStmtExpr *stmt = knh_parseStmt(_ctx, script, uline);
	WCTX(_ctx)->isEvaled = 0;
	StmtITR_eval(_ctx, stmt);
	tf = (STT_(stmt) != STT_ERR);
	RESET_GCSTACK(_ctx);
	return tf;
}

#else
kbool_t knh_beval(CTX, kInputStream *in, kline_t uline)
{
	BEGIN_LOCAL(_ctx, lsfp, 2);
	kbool_t tf;
	KSETv(lsfp[0].o, in);
	LOCAL_NEW(_ctx, lsfp, 1, kStmtExpr *, stmt, knh_InputStream_parseStmt(_ctx, in, &uline));
	WCTX(_ctx)->isEvaled = 0;
	StmtITR_eval(_ctx, stmt);
	tf = (STT_(stmt) != STT_ERR);
	END_LOCAL(_ctx, lsfp);
	return tf;
}

#endif

KNHAPI2(kbool_t) knh_eval(CTX, const char *script, kline_t uline, kOutputStream *w)
{
#ifdef K_USING_SUGAR
	KSETv(((kcontext_t*)ctx)->e, K_NULL);
	kbool_t tf = knh_beval2(_ctx, script, uline);
	if(w != NULL && tf && ctx->isEvaled == 1) {
		knh_write_Object(_ctx, w, ctx->evaled, FMT_dump);
	}
#else
	kInputStream *bin = new_BytesInputStream(_ctx, script, knh_strlen(script));
	KSETv(((kcontext_t*)ctx)->e, K_NULL);
	kbool_t tf = knh_beval(_ctx, bin, uline);
	if(w != NULL && ctx->isEvaled == 1) {
		knh_write_Object(_ctx, w, ctx->evaled, FMT_dump);
	}
#endif
	return tf;

}

/* ------------------------------------------------------------------------ */


#ifdef K_USING_SUGAR
kstatus_t knh_InputStream_load(CTX, kInputStream *in, kline_t uline)
{
	kstatus_t status = K_BREAK;
	kBytes*ba = new_Bytes(_ctx, "chunk", K_PAGESIZE);
	PUSH_GCSTACK(_ctx, ba);
	kline_t linenum = uline;
	do {
		knh_Bytes_clear(ba, 0);
		if(!io2_isClosed(_ctx, in->io2)) {
			status = K_CONTINUE;
			uline = linenum;
			linenum = readchunk(_ctx, in, linenum, ba);
		}
		if(!bytes_isempty(ba->bu)) {
			DBG_(if(knh_isVerboseLang()) {
				fprintf(stderr, "\n>>>--------------------------------\n");
				fprintf(stderr, "%s<<<--------------------------------\n", knh_Bytes_ensureZero(_ctx, ba));
			});
			status  = (kstatus_t)knh_beval2(_ctx, knh_Bytes_ensureZero(_ctx, ba), uline);
		}
	} while(BA_size(ba) > 0 && status == K_CONTINUE);
	if(!knh_isCompileOnly(_ctx)) {
		KNH_NTRACE2(_ctx, "konoha:load", K_NOTICE, KNH_LDATA(LOG_s("urn", S_text(in->path->urn))));
	}
	return status;
}
#else
kstatus_t knh_InputStream_load(CTX, kInputStream *in, kline_t uline)
{
	BEGIN_LOCAL(_ctx, lsfp, 3);
	kstatus_t status = K_BREAK;
	LOCAL_NEW(_ctx, lsfp, 0, kBytes*, ba, new_Bytes(_ctx, "chunk", K_PAGESIZE));
	KSETv(lsfp[1].o, in);
	kline_t linenum = uline;
	do {
		knh_Bytes_clear(ba, 0);
		if(!io2_isClosed(_ctx, in->io2)) {
			status = K_CONTINUE;
			uline = linenum;
			linenum = readchunk(_ctx, in, linenum, ba);
		}
		if(!bytes_isempty(ba->bu)) {
			kInputStream *bin = new_BytesInputStream(_ctx, BA_totext(ba), BA_size(ba));
			KSETv(lsfp[2].o, bin);
			DBG_(if(knh_isVerboseLang()) {
				fprintf(stderr, "\n>>>--------------------------------\n");
				fprintf(stderr, "%s<<<--------------------------------\n", knh_Bytes_ensureZero(_ctx, ba));
			});
			status  = (kstatus_t)knh_beval(_ctx, bin, uline);
		}
	} while(BA_size(ba) > 0 && status == K_CONTINUE);
	if(!knh_isCompileOnly(_ctx)) {
		KNH_NTRACE2(_ctx, "konoha:load", K_NOTICE, KNH_LDATA(LOG_s("urn", S_text(in->path->urn))));
	}
	END_LOCAL(_ctx, lsfp);
	return status;
}
#endif

kstatus_t knh_load(CTX, kPath *pth)
{
	kio_t *io2 = pth->dpi->io2openNULL(_ctx, pth, "r", NULL);
	if(io2 != NULL) {
		kline_t uline = 1;
		kInputStream *in = new_InputStream(_ctx, io2, pth);
		kfileid_t fileid = knh_getURI(_ctx, S_tobytes(pth->urn));
		ULINE_setURI(uline, fileid);
		KSETv((K_GMANS)->path, pth);
		return knh_InputStream_load(_ctx, in, uline);
	}
	return K_BREAK;
}

/* ------------------------------------------------------------------------ */

static void knh_buff_addStartPath(CTX, kBytes *ba, size_t pos, kbytes_t path)
{
	if(!knh_isfile(_ctx, path.text) && !knh_bytes_endsWith_(path, STEXT(".k"))) {
		kString *s = knh_getPropertyNULL(_ctx, STEXT("konoha.script.path"));
		if(s != NULL) {
			knh_buff_addospath(_ctx, ba, pos, 0, S_tobytes(s));
			knh_buff_addospath(_ctx, ba, pos, 1/*sep*/, path);
			if(knh_isfile(_ctx, knh_Bytes_ensureZero(_ctx, ba) + pos)) {
				return ;
			}
			knh_Bytes_clear(ba, pos);
		}
	}
	knh_buff_addospath(_ctx, ba, pos, 0, path);
}

kstatus_t knh_startScript(CTX, const char *path)
{
	kstatus_t status = K_BREAK;
	KONOHA_BEGIN(_ctx);
	kKonohaSpace *ns = K_GMANS;
	kline_t uline = 1;
	if(path[0] == '-' && path[1] == 0) {
		kInputStream *in = KNH_STDIN;
		kfileid_t fileid = knh_getURI(_ctx, STEXT("stdin"));
		ULINE_setURI(uline, fileid);
		status = knh_InputStream_load(_ctx, in, uline);
	}
	else {
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		knh_buff_addStartPath(_ctx, cwb->ba, cwb->pos, B(path));
		FILE *fp = fopen(CWB_totext(_ctx, cwb), "r");
		if(fp != NULL) {
			kfileid_t fileid = knh_getURI(_ctx, CWB_tobytes(cwb));
			KSETv(ns->path, new_Path(_ctx, knh_buff_newRealPathString(_ctx, cwb->ba, cwb->pos)));
			kInputStream *in = new_InputStream(_ctx, new_FILE(_ctx, fp, 256), ns->path);
			ULINE_setURI(uline, fileid);
			status = knh_InputStream_load(_ctx, in, uline);
		}
		else {
			KNH_NOTE("script not found: %s", path);
		}
		CWB_close(_ctx, cwb);
	}
	knh_stack_clear(_ctx, ctx->stack);
	KONOHA_END(_ctx);
	return status;
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
