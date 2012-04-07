#ifdef __cplusplus
extern "C" {
#endif

/* String */
static KMETHOD Object_toString(CTX, ksfp_t *sfp _RIX)
{
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	O_ct(sfp[0].o)->p(_ctx, sfp, 0, &wb, 0);
	kString* s = new_kString(kwb_top(&wb, 1), kwb_size(&wb), 0);
	kwb_free(&wb);
	RETURN_(s);
}

//## @Const method Boolean Boolean.opNOT();
static KMETHOD Boolean_opNOT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(!sfp[0].bvalue);
}

//## @Const method Int Int.opADD(Int x);
static KMETHOD Int_opADD(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue + sfp[1].ivalue);
}

//## @Const method Int Int.opSUB(Int x);
static KMETHOD Int_opSUB(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue - sfp[1].ivalue);
}

//## @Const method Int Int.opMUL(Int x);
static KMETHOD Int_opMUL(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue * sfp[1].ivalue);
}

//## @Const method Int Int.opDIV(Int x);
static KMETHOD Int_opDIV(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue / sfp[1].ivalue);
}

//## @Const method Int Int.opMOD(Int x);
static KMETHOD Int_opMOD(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue % sfp[1].ivalue);
}

//## @Const method Boolean Int.opEQ(Int x);
static KMETHOD Int_opEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue == sfp[1].ivalue);
}

//## @Const method Boolean Int.opNEQ(Int x);
static KMETHOD Int_opNEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue != sfp[1].ivalue);
}

//## @Const method Boolean Int.opLT(Int x);
static KMETHOD Int_opLT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue < sfp[1].ivalue);
}

//## @Const method Boolean Int.opLTE(Int x);
static KMETHOD Int_opLTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue <= sfp[1].ivalue);
}

//## @Const method Boolean Int.opGT(Int x);
static KMETHOD Int_opGT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue > sfp[1].ivalue);
}

//## @Const method Boolean Int.opGTE(Int x);
static KMETHOD Int_opGTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue >= sfp[1].ivalue);
}

//## @Const method String Int.toString();
static KMETHOD Int_toString(CTX, ksfp_t *sfp _RIX)
{
	char buf[40];
	snprintf(buf, sizeof(buf), "%ld", (intptr_t)sfp[0].ivalue);
	RETURN_(new_kString(buf, strlen(buf), SPOL_ASCII));
}

//## @Const method String String.toInt();
static KMETHOD String_toInt(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_((kint_t)strtoll(S_text(sfp[0].s), NULL, 10));
}

////## @Const method String Boolean.toString();
// sorry ide. it isn't necessary
//static KMETHOD Boolean_toString(CTX, ksfp_t *sfp _RIX)
//{
//	kbool_t b = !!(sfp[0].bvalue);
//	const char *s[] = {"false", "true"};
//	RETURN_(new_kString(s[b], strlen(s[b]), SPOL_ASCII));
//}

//## method void System.p(@Coercion String msg);
static KMETHOD System_p(CTX, ksfp_t *sfp _RIX)
{
	kline_t uline = sfp[K_RTNIDX].uline;
	kreportf(PRINT_, uline, "%s", S_text(sfp[1].s));
}

#ifdef __cplusplus
}
#endif
