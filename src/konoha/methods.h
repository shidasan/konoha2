#ifdef __cplusplus
extern "C" {
#endif

/* upcast(o) */
static KMETHOD Object_opUPCAST(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(sfp[0].o);
}

/* !boolean */
static KMETHOD Boolean_opNOT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(!sfp[0].bvalue);
}

/* int + int */
static KMETHOD Int_opADD(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue + sfp[1].ivalue);
}

/* int - int */
static KMETHOD Int_opSUB(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue - sfp[1].ivalue);
}

/* int * int */
static KMETHOD Int_opMUL(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue * sfp[1].ivalue);
}

/* int / int */
static KMETHOD Int_opDIV(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue / sfp[1].ivalue);
}

/* int % int */
static KMETHOD Int_opMOD(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue % sfp[1].ivalue);
}

/* int == int */
static KMETHOD Int_opEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue == sfp[1].ivalue);
}

/* int != int */
static KMETHOD Int_opNEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue != sfp[1].ivalue);
}

/* int < int */
static KMETHOD Int_opLT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue < sfp[1].ivalue);
}

/* int <= int */
static KMETHOD Int_opLTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue <= sfp[1].ivalue);
}

/* int > int */
static KMETHOD Int_opGT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue > sfp[1].ivalue);
}

/* int >= int */
static KMETHOD Int_opGTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue >= sfp[1].ivalue);
}

/* Int to String */
static KMETHOD Int_toString(CTX, ksfp_t *sfp _RIX)
{
	char buf[40];
	snprintf(buf, sizeof(buf), "%ld", (intptr_t)sfp[0].ivalue);
	RETURN_(new_kString(buf, strlen(buf), SPOL_ASCII));
}

/* String to Int */
static KMETHOD String_toInt(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_((kint_t)strtoll(S_text(sfp[0].s), NULL, 10));
}

/* ------------------------------------------------------------------------ */
//## @Const method String Boolean.toString();
/* Booelan to String */
static KMETHOD Boolean_toString(CTX, ksfp_t *sfp _RIX)
{
	kbool_t b = sfp[0].bvalue;
	const char *s[] = {"false", "true"};
	RETURN_(new_kString(s[b], strlen(s[b]), SPOL_ASCII));
}

// void System.p(@Coercion String msg);
static KMETHOD System_p(CTX, ksfp_t *sfp _RIX)
{
	kline_t uline = sfp[K_RTNIDX].uline;
	kreportf(PRINT_, uline, "%s\n", S_text(sfp[1].s));
}

#ifdef __cplusplus
}
#endif
