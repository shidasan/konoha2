/*
 * assignment_glue.h
 *
 *  Created on: Apr 10, 2012
 *      Author: kimio
 */

#ifndef ASSIGNMENT_GLUE_H_
#define ASSIGNMENT_GLUE_H_

// --------------------------------------------------------------------------

static kbool_t Expr_isGetter(CTX, kExpr *expr)
{
	DBG_P("setter is unsupported .. ");
	return false;
}

static kExpr *Expr_toSetter(CTX, kExpr *expr, kExpr *vexpr)
{
	return K_NULLEXPR;
}

// --------------------------------------------------------------------------

// Expr Expr.tyCheckStub(Gamma gma, int req_tyid);
static KMETHOD ExprTyCheck_assignment(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, req_ty);
	DBG_P("typing: assignment .. ");
	kExpr *lexpr = kExpr_tyCheckAt(expr, 1, gma, TY_var, TPOL_ALLOWVOID);
	if(lexpr != K_NULLEXPR) {
		kExpr *rexpr = kExpr_tyCheckAt(expr, 2, gma, lexpr->ty, 0);
		if(lexpr->build == TEXPR_LOCAL || lexpr->build == TEXPR_LOCAL_ || lexpr->build == TEXPR_FIELD) {
			if(rexpr != K_NULLEXPR) {
				((struct _kExpr*)expr)->build = TEXPR_LET;
				((struct _kExpr*)rexpr)->ty = lexpr->ty;
			}
			RETURN_(expr);
		}
		if(Expr_isGetter(_ctx, lexpr)) {
			RETURN_(Expr_toSetter(_ctx, lexpr, rexpr));
		}
	}
	RETURN_(K_NULLEXPR);
}

// --------------------------------------------------------------------------

static kbool_t local_initassignment(CTX, kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		{ TOKEN("="), .op2 = "*", .priority_op2 = 4096, .ExprTyCheck = ExprTyCheck_assignment},
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

#endif /* ASSIGNMENT_GLUE_H_ */
