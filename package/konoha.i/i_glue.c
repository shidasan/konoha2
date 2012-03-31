#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>

// Expr Expr.tyCheckStub(Gamma gma, int req_tyid);
//static KMETHOD ExprTyCheck_stub(CTX, ksfp_t *sfp _RIX)
//{
//	VAR_ExprTyCheck(expr, gma, req_ty);
//	DBG_P("stub: size=%d", kArray_size(expr->consNUL));
//	RETURN_(K_NULLEXPR);
//}

static void Method_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kMethod *mtd = sfp[pos].mtd;
	char mbuf[128];
	if(level != 0) {
//		if(!(IS_FMTline(level))) {
//			if(Method_isAbstract(mtd)) {
//				knh_write(_ctx, w, STEXT("@Abstract")); kwb_putc(wb, ' ');
//			}
//			if(Method_isPrivate(mtd)) {
//				knh_write(_ctx, w, STEXT("@Private"));  kwb_putc(wb, ' ');
//			}
//			if(Method_isStatic(mtd)) {
//				knh_write(_ctx, w, STEXT("@Static"));   kwb_putc(wb, ' ');
//			}
//			knh_write_type(_ctx, w, knh_Param_rtype(DP(mtd)->mp));
//			kwb_putc(wb, ' ');
//		}
	}
	kwb_printf(wb, "%s.%s", T_cid(mtd->cid), T_mn(mbuf, mtd->mn));
	if(level != 0) {

	}
//	if(!(IS_FMTline(level))) {
//		size_t i;
//		kwb_putc(wb, '(');
//		for(i = 0; i < knh_Method_psize(mtd); i++) {
//			kparam_t *p = knh_Param_get(DP(mtd)->mp, i);
//			if(i > 0) {
//				knh_write_delim(_ctx, w);
//			}
//			knh_write_type(_ctx, w, p->type);
//			kwb_putc(wb, ' ');
//			knh_write(_ctx, w, B(FN__(p->fn)));
//		}
//		if(Param_isVARGs(DP(mtd)->mp)) {
//			knh_write_delimdots(_ctx, w);
//		}
//		kwb_putc(wb, ')');
//	}
//	if(IS_FMTdump(level)) {
//		if(!IS_NULL(DP(mtd)->objdata)) {
//			knh_write_EOL(_ctx, w);
//			knh_write_InObject(_ctx, w, DP(mtd)->objdata, level);
//		}
//	}
//
//	kwb_printf(wb, sfp[pos].bvalue ? "true" : "false");
}

// --------------------------------------------------------------------------

static void copyMethodList(CTX, kcid_t cid, kArray *s, kArray *d)
{
	size_t i;
	for(i = 0; i < kArray_size(s); i++) {
		kMethod *mtd = s->methods[i];
		if(mtd->cid != cid) continue;
		kArray_add(d, mtd);
	}
}

static void dumpMethod(CTX, ksfp_t *sfp, kMethod *mtd)
{
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	O_ct(mtd)->p(_ctx, sfp, 2, &wb, 1);
	fprintf(stdout, "%s\n", kwb_top(&wb, 1));
	kwb_free(&wb);
	return;
}

static void dumpMethodList(CTX, ksfp_t *sfp, size_t start, kArray *list)
{
	size_t i;
	for(i = start; i < kArray_size(list); i++) {
		dumpMethod(_ctx, sfp, list->methods[i]);
	}
}

static KMETHOD Lingo_man(CTX, ksfp_t *sfp _RIX)
{
	INIT_GCSTACK();
	kArray *list = _ctx->stack->gcstack;
	size_t start = kArray_size(list);
	kLingo *lgo = sfp[0].lgo;
	const kclass_t *ct = O_ct(sfp[1].o);
	while(lgo != NULL) {
		if(lgo->methodsNULL != NULL) {
			copyMethodList(_ctx, ct->cid, lgo->methodsNULL, list);
		}
		lgo = lgo->parentNULL;
	}
	copyMethodList(_ctx, ct->cid, ct->methods, list);
	dumpMethodList(_ctx, sfp, start, list);
	RESET_GCSTACK();
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static	kbool_t i_initPackage(CTX, struct kLingo *lgo, int argc, const char**args, kline_t pline)
{
	USING_SUGAR;
	kclass_t *ct = (kclass_t*)kclass(TY_Method, pline);
	ct->p = Method_p;
	intptr_t methoddata[] = {
		_Public, _F(Lingo_man), TY_void, TY_Lingo, MN_("man"), 1, TY_Object, FN_("x"),
		DEND,
	};
	kloadMethodData(NULL, methoddata);
	return true;
}

static kbool_t i_setupPackage(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

#define TOKEN(T)  .name = T, .namelen = (sizeof(T)-1)

static kbool_t i_initLingo(CTX,  struct kLingo *lgo, kline_t pline)
{
//	USING_SUGAR;
//	ksyntaxdef_t SYNTAX[] = {
//		{ TOKEN("float"), .type = TY_Float, },
//		{ TOKEN("double"), .type = TY_Float, },
//		{ TOKEN("$FLOAT"), .keyid = KW_TK(TK_FLOAT), .ExprTyCheck = TokenTyCheck_FLOAT, },
//		{ .name = NULL, },
//	};
//	SUGAR Lingo_defineSyntax(_ctx, lgo, SYNTAX);
	return true;
}

static kbool_t i_setupLingo(CTX, struct kLingo *lgo, kline_t pline)
{
	return true;
}

KPACKDEF* i_init(void)
{
	static KPACKDEF d = {
		KPACKNAME("konoha.i", "1.0"),
		.initPackage = i_initPackage,
		.setupPackage = i_setupPackage,
		.initLingo = i_initLingo,
		.setupPackage = i_setupLingo,
	};
	return &d;
}
