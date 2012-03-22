#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
