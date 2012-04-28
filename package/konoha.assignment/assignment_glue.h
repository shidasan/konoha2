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

// Expr Expr.tyCheckStub(Gamma gma, int reqtyid);
static KMETHOD ExprTyCheck_assignment(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ExprTyCheck(expr, syn, gma, reqty);
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
