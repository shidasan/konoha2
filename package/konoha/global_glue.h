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

#ifndef GLOBAL_GLUE_H_
#define GLOBAL_GLUE_H_

// --------------------------------------------------------------------------

static	kbool_t global_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	return true;
}

static kbool_t global_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

// --------------------------------------------------------------------------

static KMETHOD Fmethod_ProtoGetter(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	RETURN_(kObject_getObject(sfp[0].o, key, sfp[K_RTNIDX].o));
}

static KMETHOD Fmethod_ProtoGetterN(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	RETURNd_(kObject_getUnboxedValue(sfp[0].o, key, 0));
}

static KMETHOD Fmethod_ProtoSetter(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	kObject_setObject(sfp[0].o, key, sfp[1].o);
	RETURN_(sfp[1].o);
}

static KMETHOD Fmethod_ProtoSetterN(CTX, ksfp_t *sfp _RIX)
{
	kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	ksymbol_t key = (ksymbol_t)mtd->delta;
	kObject_setUnboxedValue(sfp[0].o, key, mtd->pa->p[0].ty, sfp[1].ndata);
	RETURNd_(sfp[1].ndata);
}

static kMethod *new_ProtoGetter(CTX, kcid_t cid, ksymbol_t sym, ktype_t ty)
{
	kmethodn_t mn = ty == TY_Boolean ? MN_toISBOOL(sym) : MN_toGETTER(sym);
	knh_Fmethod f = (TY_isUnbox(ty)) ? Fmethod_ProtoGetterN : Fmethod_ProtoGetter;
	kParam *pa = new_kParam(ty, 0, NULL);
	kMethod *mtd = new_kMethod(kMethod_Public|kMethod_Immutable, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = sym;
	return mtd;
}

static kMethod *new_ProtoSetter(CTX, kcid_t cid, ksymbol_t sym, ktype_t ty)
{
	kmethodn_t mn = MN_toSETTER(sym);
	knh_Fmethod f = (TY_isUnbox(ty)) ? Fmethod_ProtoSetterN : Fmethod_ProtoSetter;
	kparam_t p = {ty, FN_("x")};
	kParam *pa = new_kParam(ty, 1, &p);
	kMethod *mtd = new_kMethod(kMethod_Public, cid, mn, pa, f);
	((struct _kMethod*)mtd)->delta = sym;
	return mtd;
}

static void CT_addMethod2(CTX, kclass_t *ct, kMethod *mtd)
{
	if(unlikely(ct->methods == K_EMPTYARRAY)) {
		KINITv(((struct _kclass*)ct)->methods, new_(MethodArray, 8));
	}
	kArray_add(ct->methods, mtd);
}

static kMethod *Object_newProtoSetterNULL(CTX, kObject *o, kKonohaSpace *ks, ktype_t ty, ksymbol_t fn, kline_t pline)
{
	USING_SUGAR;
	ktype_t cid = O_cid(o);
	kMethod *mtd = kKonohaSpace_getMethodNULL(ks, cid, MN_toSETTER(fn));
	if(mtd != NULL) {
		SUGAR p(_ctx, ERR_, pline, -1, "already defined name: %s.%s", T_CT(O_ct(o)), T_fn(fn));
		return NULL;
	}
	mtd = kKonohaSpace_getMethodNULL(ks, cid, MN_toGETTER(fn));
	if(mtd == NULL) {
		mtd = kKonohaSpace_getMethodNULL(ks, cid, MN_toISBOOL(fn));
	}
	if(mtd != NULL && mtd->pa->rtype != ty) {
		SUGAR p(_ctx, ERR_, pline, -1, "differently defined getter: %s.%s", T_CT(O_ct(o)), T_fn(fn));
		return NULL;
	}
	if(mtd == NULL) { // no getter
		CT_addMethod2(_ctx, O_ct(o), new_ProtoGetter(_ctx, cid, fn, ty));
	}
	mtd = new_ProtoSetter(_ctx, cid, fn, ty);
	CT_addMethod2(_ctx, O_ct(o), mtd);
	return mtd;
}

static ksymbol_t tosymbol(CTX, kExpr *expr)
{
	if(Expr_isTerm(expr)) {
		kToken *tk = expr->tk;
		if(tk->tt == TK_SYMBOL) {
			return ksymbol(S_text(tk->text), S_size(tk->text), FN_NEWID, SYMPOL_NAME);
		}
	}
	return FN_NONAME;
}

static KMETHOD StmtTyCheck_var(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_StmtTyCheck(stmt, syn, gma);
	DBG_P("global assignment .. ");
	kObject *scr = gma->genv->ks->scrNUL;
	if(scr == NULL) {
		SUGAR p(_ctx, ERR_, stmt->uline, -1, " global variables are not avaiable");
		RETURNb_(false);
	}
	kExpr *vexpr = kStmt_expr(stmt, KW_("var"), K_NULLEXPR);
	ksymbol_t fn = tosymbol(_ctx, vexpr);
	if(fn == FN_NONAME) {
		SUGAR p(_ctx, ERR_, stmt->uline, -1, "not variable name");
		RETURNb_(false);
	}
	kExpr *expr = kStmt_expr(stmt, KW_EXPR, K_NULLEXPR);
	DBG_P("expr kw='%s'", T_kw(expr->syn->kw));
	if(!SUGAR Stmt_tyCheckExpr(_ctx, stmt, KW_EXPR, gma, TY_var, 0)) {
		SUGAR p(_ctx, ERR_, stmt->uline, -1, "type error");
		RETURNb_(false);
	}
	/*kExpr **/expr = kStmt_expr(stmt, KW_EXPR, K_NULLEXPR);
	kMethod *mtd = Object_newProtoSetterNULL(_ctx, scr, gma->genv->ks, expr->ty, fn, stmt->uline);
	if(mtd == NULL) {
		RETURNb_(false);
	}
	SUGAR p(_ctx, INFO_, stmt->uline, -1, "%s has type %s", T_fn(fn), T_ty(expr->ty));
	expr = SUGAR new_TypedMethodCall(_ctx, TY_void, mtd, gma, 2, new_ConstValue(O_cid(scr), scr), expr);
	kObject_setObject(stmt, KW_EXPR, expr);
	kStmt_typed(stmt, EXPR);
	RETURNb_(true);
}

typedef const struct _kScript kScript;

struct _kScript {
	kObjectHeader h;
};

static kbool_t global_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("var"), TopStmtTyCheck_(var), .rule = "\"var\" var: $expr \"=\" $expr", },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	if(ks->scrNUL == NULL) {
		KDEFINE_CLASS defScript = {
			.structname = "Script",
			.cid = CLASS_newid,
			.cflag = kClass_Singleton|kClass_Final,
			.cstruct_size = sizeof(kScript),
		};
		kclass_t *cScript = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &defScript, pline);
		KINITv(((struct _kKonohaSpace*)ks)->scrNUL, knull(cScript));
	}
	return true;
}


static kbool_t global_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}


#endif /* ASSIGNMENT_GLUE_H_ */
