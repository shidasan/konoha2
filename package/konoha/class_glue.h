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

#ifndef CLASS_GLUE_H_
#define CLASS_GLUE_H_

static KMETHOD Fmethod_FieldGetter(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	RETURN_((sfp[0].o)->fields[delta]);
}
static KMETHOD Fmethod_FieldGetterN(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	RETURNd_((sfp[0].o)->ndata[delta]);
}
static KMETHOD Fmethod_FieldSetter(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	KSETv((sfp[0].Wo)->fields[delta], sfp[1].o);
	RETURN_(sfp[1].o);
}
static KMETHOD Fmethod_FieldSetterN(CTX, ksfp_t *sfp _RIX)
{
	size_t delta = sfp[K_MTDIDX].mtdNC->delta;
	(sfp[0].Wo)->ndata[delta] = sfp[1].ndata;
	RETURNd_(sfp[1].ndata);
}

static kMethod *new_FieldGetter(CTX, kcid_t cid, ksymbol_t sym, ktype_t ty, int idx)
{
	kmethodn_t mn = ty == TY_Boolean ? MN_toISBOOL(sym) : MN_toGETTER(sym);
	knh_Fmethod f = (TY_isUnbox(ty)) ? Fmethod_FieldGetterN : Fmethod_FieldGetter;
	kParam *pa = new_kParam(ty, 0, NULL);
	kMethod *mtd = new_kMethod(kMethod_Public|kMethod_Immutable, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = idx;  // FIXME
	return mtd;
}

static kMethod *new_FieldSetter(CTX, kcid_t cid, kmethodn_t mn, ktype_t ty, int idx)
{
	knh_Fmethod f = (TY_isUnbox(ty)) ? Fmethod_FieldSetterN : Fmethod_FieldSetter;
	kparam_t p = {ty, FN_("x")};
	kParam *pa = new_kParam(ty, 1, &p);
	kMethod *mtd = new_kMethod(kMethod_Public, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = idx;   // FIXME
	return mtd;
}

static intptr_t KLIB2_Method_indexOfField(kMethod *mtd)
{
	knh_Fmethod f = mtd->fcall_1;
	if(f== Fmethod_FieldGetter || f == Fmethod_FieldGetterN || f == Fmethod_FieldSetter || f == Fmethod_FieldSetterN) {
		return (intptr_t)mtd->delta;
	}
	return -1;
}

static void CT_addMethod(CTX, kclass_t *ct, kMethod *mtd)
{
	if(unlikely(ct->methods == K_EMPTYARRAY)) {
		KINITv(((struct _kclass*)ct)->methods, new_(MethodArray, 8));
	}
	kArray_add(ct->methods, mtd);
}

static void KLIB2_setGetterSetter(CTX, kclass_t *ct)
{
	size_t i, fsize = ct->fsize;
	for(i=0; i < fsize; i++) {
		if(FLAG_is(ct->fields[i].flag, kField_Getter)) {
			FLAG_unset(ct->fields[i].flag, kField_Getter);
			kMethod *mtd = new_FieldGetter(_ctx, ct->cid, ct->fields[i].fn, ct->fields[i].ty, i);
			CT_addMethod(_ctx, ct, mtd);
		}
		if(FLAG_is(ct->fields[i].flag, kField_Setter)) {
			FLAG_unset(ct->fields[i].flag, kField_Setter);
			kMethod *mtd = new_FieldSetter(_ctx, ct->cid, ct->fields[i].fn, ct->fields[i].ty, i);
			CT_addMethod(_ctx, ct, mtd);
		}
	}
}

// --------------------------------------------------------------------------

// int KonohaSpace.getCid(String name, int defval)
static KMETHOD KonohaSpace_getCid(CTX, ksfp_t *sfp _RIX)
{
	kclass_t *ct = kKonohaSpace_getCT(sfp[0].ks, NULL/*fixme*/, S_text(sfp[1].s), S_size(sfp[1].s), (kcid_t)sfp[2].ivalue);
	kint_t cid = ct != NULL ? ct->cid : sfp[2].ivalue;
	RETURNi_(cid);
}

static void setfield(CTX, KDEFINE_CLASS *ct, int fctsize, kclass_t *supct)
{
	size_t fsize = supct->fsize + fctsize;
	ct->cstruct_size = fctsize * sizeof(kObject*); //size64((fsize * sizeof(void*)) + sizeof(kObjectHeader));
	DBG_P("supct->fsize=%d, fctsize=%d, cstruct_size=%d", supct->fsize, fctsize, ct->cstruct_size);
	if(fsize > 0) {
		ct->fields = (kfield_t*)KCALLOC(fsize, sizeof(kfield_t));
		ct->fsize = supct->fsize;
		ct->fallocsize = fsize;
		if(supct->fsize > 0) {
			memcpy(ct->fields, supct->fields, sizeof(kfield_t)*ct->fsize);
		}
	}
}

// int KonohaSpace.defineClass(int flag, String name, int supcid, int fieldsize);
static KMETHOD KonohaSpace_defineClass(CTX, ksfp_t *sfp _RIX)
{
	ktype_t supcid = sfp[3].ivalue == 0 ? TY_Object :(ktype_t)sfp[3].ivalue;
	kclass_t *supct = kclass(supcid, sfp[K_RTNIDX].uline);
	if(CT_isFinal(supct)) {
		kreportf(CRIT_, sfp[K_RTNIDX].uline, "%s is final", T_cid(supcid));
	}
	if(CT_isDefined(supct)) {
		kreportf(CRIT_, sfp[K_RTNIDX].uline, "%s has undefined field(s)", T_cid(supcid));
	}
	KDEFINE_CLASS defNewClass = {
		.cflag  = (kflag_t)sfp[1].ivalue,
		.cid    = CLASS_newid,
		.bcid   = CLASS_Object,
		.supcid = supcid,
	};
	setfield(_ctx, &defNewClass, (int)sfp[4].ivalue, supct);
	kKonohaSpace *ks = sfp[0].ks;
	kclass_t *c = Konoha_addClassDef(ks->packid, ks->packdom, sfp[2].s, &defNewClass, sfp[K_RTNIDX].uline);
	RETURNi_(c->cid);
}

// int KonohaSpace.defineClassField(int cid, int flag, int ty, String name, Object *value);
static KMETHOD KonohaSpace_defineClassField(CTX, ksfp_t *sfp _RIX)
{
	kcid_t cid = (kcid_t)sfp[1].ivalue;
	kflag_t flag = (kflag_t)sfp[2].ivalue;
	ktype_t ty = (ktype_t)sfp[3].ivalue;
	kString *name = sfp[4].s;
	kObject *value = sfp[5].o;
	struct _kclass *ct = (struct _kclass*)kclass(cid, sfp[K_RTNIDX].uline);
	if(CT_isDefined(ct)) {
		kreportf(CRIT_, sfp[K_RTNIDX].uline, "%s has no undefined field", T_cid(ct->cid));
	}
	int pos = ct->fsize;
	ct->fsize += 1;
	ct->fields[pos].flag = flag;
	ct->fields[pos].ty = ty;
	ct->fields[pos].fn = ksymbol(S_text(name), S_size(name), FN_NEWID, SYMPOL_NAME);
	if(TY_isUnbox(ty)) {
		ct->WnulvalNUL->ndata[pos] = O_unbox(value);
	}
	else {
		kObject *v = (IS_NULL(value)) ? knull(O_ct(value)) : value;
		KSETv(ct->WnulvalNUL->ndata[pos], v);
		ct->fields[pos].isobj = 1;
	}
	if(CT_isDefined(ct)) {
		DBG_P("all fields are set");
		KLIB2_setGetterSetter(_ctx, ct);
	}
}

/**
sugar "new" ...

Expr Stmt.ParseNewExpr(Token[] tls, int s, int c, int e)
{
	Token tkNEW = tls[s];
	if(s + 2 < e) {
		Token tk1 = tls[s+1];
		Token tk2 = tls[s+2];
		if(tk1.isTypeName() && tk2.isParenthesis()) {
			Expr expr = newMethodCallExpr(tkNEW, tk1);
			expr = addExprParam(expr, tk2);
			return expr;
		}
	}
	return tk.error();
}

Expr Expr.tyCheckNewExpr(Gamma gma, int ty)
{
	return tyCheckAsMethodCall(gma, ty);
}

KonohaSpace.addParseExpr("new", "ParseNewExpr");
KonohaSpace.addExprTyCheck("new", "tyCheckNewExpr");
**/

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t class_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	int FN_flag = FN_("flag"), FN_cid = FN_("cid"), FN_name = FN_("name"), FN_defval = FN_("defval");
	intptr_t MethodData[] = {
		_Public, _F(KonohaSpace_getCid), TY_Int, TY_KonohaSpace, MN_("cid"), 2, TY_String, FN_name, TY_Int, FN_defval,
		_Public, _F(KonohaSpace_defineClass), TY_Int, TY_KonohaSpace, MN_("defineClass"), 4, TY_Int, FN_flag, TY_String, FN_name, TY_Int, FN_("supcid"), TY_Int, FN_("fieldSize"),
		_Public, _F(KonohaSpace_defineClassField), TY_Int, TY_KonohaSpace, MN_("defineClassField"), 5, TY_Int, FN_cid, TY_Int, FN_flag, TY_Int, FN_("type"), TY_String, FN_name, TY_Object, FN_defval,
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t class_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

// --------------------------------------------------------------------------

static kExpr* NewExpr(CTX, ksyntax_t *syn, kToken *tk, ktype_t ty, uintptr_t val)
{
	struct _kExpr *expr = new_W(Expr, syn);
	KSETv(expr->tk, tk);
	Expr_setTerm(expr, 1);
	expr->build = TEXPR_NEW;
	expr->ty = ty;
	expr->ndata = val;
	return (kExpr*)expr;
}

static KMETHOD ParseExpr_new(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ParseExpr(stmt, syn, tls, s, c, e);
	assert(s == c);
	kToken *tkNEW = tls->toks[s];
	if(s + 2 < kArray_size(tls)) {
		kToken *tk1 = tls->toks[s+1];
		kToken *tk2 = tls->toks[s+2];
		if(TK_isType(tk1) && tk2->tt == AST_PARENTHESIS) {
			ksyntax_t *syn = SYN_(kStmt_ks(stmt), KW_CALL);
			kExpr *expr = SUGAR new_ConsExpr(_ctx, syn, 2, tkNEW, NewExpr(_ctx, syn, tk1, TK_type(tk1), 0));
			DBG_P("NEW syn=%p", expr->syn);
			RETURN_(expr);
		}
	}
	SUGAR p(_ctx, ERR_, stmt->uline, -1, "syntax error: %s", S_text(tkNEW->text));
}

static ksymbol_t tosymbolUM(CTX, kToken *tk)
{
	DBG_ASSERT(tk->tt == TK_SYMBOL || tk->tt == TK_USYMBOL || tk->tt == TK_MSYMBOL);
	return ksymbol(S_text(tk->text), S_size(tk->text), FN_NEWID, SYMPOL_NAME);
}

static KMETHOD ExprTyCheck_Getter(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, reqty);
	kToken *tkN = expr->cons->toks[0];
	ksymbol_t fn = tosymbolUM(_ctx, tkN);
	kExpr *self = SUGAR Expr_tyCheckAt(_ctx, expr, 1, gma, TY_var, 0);
	if(self != K_NULLEXPR) {
		kMethod *mtd = kKonohaSpace_getMethodNULL(gma->genv->ks, self->ty, MN_toSETTER(fn));
		if(mtd == NULL) {
			mtd = kKonohaSpace_getMethodNULL(gma->genv->ks, self->ty, MN_toISBOOL(fn));
		}
		if(mtd != NULL) {
			KSETv(expr->cons->methods[0], mtd);
			RETURN_(SUGAR Expr_tyCheckCallParams(_ctx, expr, mtd, gma, reqty));
		}
		SUGAR p(_ctx, ERR_, tkN->uline, tkN->lpos, "undefined field accessor: %s", S_text(tkN->text));
	}
	RETURN_(K_NULLEXPR);
}

static kbool_t class_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("new"), ParseExpr_(new), },
//		{ TOKEN("."), .op2 = "*", .priority_op2 = 16, .right = 1, ExprTyCheck_(Getter) },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t class_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#endif /* CLASS_GLUE_H_ */
