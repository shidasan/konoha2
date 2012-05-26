#define KVPROTO_INIT  8
#define KVPROTO_DELTA 7

static inline karray_t* kvproto_null(void)  // for proto_get safe null
{
	static kvs_t dnull[KVPROTO_DELTA] = {};
	static karray_t pnull = {
		.bytesize = sizeof(kvs_t), .bytemax = 0,
	};
	pnull.kvs = dnull;
	return &pnull;
}

static kObject *DEFAULT_fnull(CTX, kclass_t *ct)
{
	DBG_ASSERT(ct->nulvalNUL != NULL);
	return ct->nulvalNUL;
}

static void DEFAULT_init(CTX, kObject *o, void *conf)
{
	(void)_ctx;(void)o;(void)conf;
}

static void DEFAULT_reftrace(CTX, kObject *o)
{
	(void)_ctx;(void)o;
}

static void DEFAULT_free(CTX, kObject *o)
{
	(void)_ctx;(void)o;
}

static void DEFAULT_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	(void)_ctx;(void)sfp;(void)pos;(void)wb;(void)level;
}

static uintptr_t DEFAULT_unbox(CTX, kObject *o)
{
	return 0;
}

static kObject *new_Object(CTX, kclass_t *ct, void *conf)
{
	DBG_ASSERT(ct->cstruct_size > 0);
	struct _kObject *o = (struct _kObject*) MODGC_omalloc(_ctx, ct->cstruct_size);
	o->h.magicflag = ct->magicflag;
	o->h.ct = ct;
	o->h.kvproto = kvproto_null();
	ct->init(_ctx, (kObject*)o, conf);
	return (kObject*)o;
}

static void Object_init(CTX, kObject *o, void *conf)
{
	struct _kObject *of = (struct _kObject*)o;
	of->ndata[0] = 0;
	of->ndata[1] = 0;
}

static void Object_reftrace(CTX, kObject *o)
{
	kObject *of = (kObject*)o;
	kclass_t *ct = O_ct(of);
	BEGIN_REFTRACE(ct->fsize);
	size_t i;
	for(i = 0; i < ct->fsize; i++) {
		if(ct->fields[i].isobj) {
			KREFTRACEv(of->fields[i]);
		}
	}
	END_REFTRACE();
}

static void ObjectX_init(CTX, kObject *o, void *conf)
{
	struct _kObject *of = (struct _kObject*)o;
	kclass_t *ct = O_ct(of);
	assert(CT_isDefined(ct));
	assert(ct->nulvalNUL != NULL);
	memcpy(of->fields, ct->nulvalNUL->fields, ct->cstruct_size - sizeof(kObjectHeader));
}

static void Object_initdef(CTX, struct _kclass *ct, kline_t pline)
{
	if(ct->cid == TY_Object) return;
	DBG_P("new object initialization ct->cstruct_size=%d", ct->cstruct_size);
	KSETv(ct->nulvalNUL, new_kObject(ct, NULL));
	if(ct->fsize > 0) {  // this is size of super class
		kclass_t *supct = CT_(ct->supcid);
		assert(ct->fsize == supct->fsize);
		memcpy(ct->WnulvalNUL->fields, supct->nulvalNUL->fields, sizeof(kObject*) * ct->fsize);
	}
	if(ct->fallocsize > 0) {
		ct->init = ObjectX_init;
	}
	ct->fnull = DEFAULT_fnull;
}

static uintptr_t Number_unbox(CTX, kObject *o)
{
	kNumber *n = (kNumber*)o;
	return (uintptr_t) n->ndata;
}

// Boolean
static void Number_init(CTX, kObject *o, void *conf)
{
	struct _kNumber *n = (struct _kNumber*)o;
	n->ndata = (uintptr_t)conf;
}

static void Boolean_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	//kwb_printf(wb, sfp[pos].bvalue ? "true" : "false");
}

static kObject* Boolean_fnull(CTX, kclass_t *ct)
{
	return (kObject*)K_FALSE;
}

static void Int_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	//kwb_printf(wb, KINT_FMT, sfp[pos].ivalue);
}

// String
static void String_init(CTX, kObject *o, void *conf)
{
	struct _kString *s = (struct _kString*)o;
	s->text = "";
	s->bytesize = 0;
	S_setTextSgm(s, 1);
}

static void String_free(CTX, kObject *o)
{
	kString *s = (kString*)o;
	if(S_isMallocText(s)) {
		KFREE(s->buf, S_size(s) + 1);
	}
}

static void String_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	//if(level == 0) {
	//	kwb_printf(wb, "%s", S_text(sfp[pos].o));
	//}
	//else {
	//	kwb_printf(wb, "\"%s\"", S_text(sfp[pos].o));
	//}
}

static uintptr_t String_unbox(CTX, kObject *o)
{
	kString *s = (kString*)o;
	DBG_ASSERT(IS_String(s));
	return (uintptr_t) s->text;
}

static void String_checkASCII(CTX, kString *s)
{
	unsigned char ch = 0;
	long len = S_size(s), n = (len + 3) / 4;
	const unsigned char*p = (const unsigned char *)S_text(s);
	switch(len % 4) { /* Duff's device written by ide */
		case 0: do{ ch |= *p++;
		case 3:     ch |= *p++;
		case 2:     ch |= *p++;
		case 1:     ch |= *p++;
		} while(--n>0);
	}
	S_setASCII(s, (ch < 128));
}

static kString* new_String(CTX, const char *text, size_t len, int spol)
{
	kclass_t *ct = CT_(CLASS_String);
	struct _kString *s = NULL; //knh_PtrMap_getS(_ctx, ct->constPoolMapNULL, text, len);
	if(s != NULL) return s;
	if(TFLAG_is(int, spol, SPOL_TEXT)) {
		s = (struct _kString*)new_Object(_ctx, ct, NULL);
		s->text = text;
		s->bytesize = len;
		S_setTextSgm(s, 1);
	}
	else if(len + 1 < sizeof(void*) * 2) {
		s = (struct _kString*)new_Object(_ctx, ct, NULL);
		s->text = s->inline_text;
		s->bytesize = len;
		S_setTextSgm(s, 1);
		if(text != NULL) {
			DBG_ASSERT(!TFLAG_is(int, spol, SPOL_NOCOPY));
			memcpy(s->ubuf, text, len);
		}
		s->buf[len] = '\0';
	}
	else {
		s = (struct _kString*)new_Object(_ctx, ct, NULL);
		s->bytesize = len;
		s->buf = (char*)KMALLOC(len+1);
		S_setTextSgm(s, 0);
		S_setMallocText(s, 1);
		if(text != NULL) {
			DBG_ASSERT(!TFLAG_is(int, spol, SPOL_NOCOPY));
			memcpy(s->ubuf, text, len);
		}
		s->buf[len] = '\0';
	}
	if(TFLAG_is(int, spol, SPOL_ASCII)) {
		S_setASCII(s, 1);
	}
	else if(TFLAG_is(int, spol, SPOL_UTF8)) {
		S_setASCII(s, 0);
	}
	else {
		String_checkASCII(_ctx, s);
	}
//	if(TFLAG_is(int, policy, SPOL_POOL)) {
//		kmapSN_add(_ctx, ct->constPoolMapNO, s);
//		S_setPooled(s, 1);
//	}
	return s;
}

static void Param_init(CTX, kObject *o, void *conf)
{
	struct _kParam *pa = (struct _kParam*)o;
	pa->psize = 0;
	pa->rtype = TY_void;
}

static inline size_t size64(size_t s)
{
	size_t base = sizeof(struct _kObject);
	while(base < s) {
		base *= 2;
	}
	return base;
}

static struct _kclass *new_CT(CTX, kclass_t *bct, KDEFINE_CLASS *s, kline_t pline)
{
	kshare_t *share = _ctx->share;
	kcid_t newid = share->ca.bytesize / sizeof(struct _kclass*);
	if (share->ca.bytesize == share->ca.bytemax) {
		KARRAY_EXPAND(&share->ca, share->ca.bytemax * 2);
	}
	share->ca.bytesize += sizeof(struct _kclass*);
	struct _kclass *ct = (struct _kclass*)KCALLOC(sizeof(kclass_t), 1);
	share->ca.cts[newid] = (kclass_t*)ct;
	//kcid_t newcid = share->casize;
	//share->casize++;
	//struct _kclass *ct = share->ca[newcid];
	if (bct != NULL) {
		DBG_ASSERT(s == NULL);
		memcpy(ct, bct, offsetof(kclass_t, cparam));
		ct->cid = newid;
		//if (ct->fnull == DEFAULT_fnull) ct->fnull = DEFAULT_fnullinit;
	} else {
		DBG_ASSERT(s != NULL);
		ct->cflag = s->cflag;
		ct->cid = newid;
		ct->bcid = newid;
		ct->supcid = (s->supcid == 0) ? CLASS_Object : s->supcid;
		ct->fields = s->fields;
		ct->fsize = s->fsize;
		ct->fallocsize = s->fallocsize;
		ct->cstruct_size = size64(s->cstruct_size);
		if (s->cparams != NULL) {
			DBG_P("params");
			//KINITv(ct->cparam, new_kParam2(s->rtype, s->psize, s->cparams));
		}
		ct->init = (s->init != NULL) ? s->init : DEFAULT_init;
		ct->reftrace = (s->reftrace != NULL) ? s->reftrace : DEFAULT_reftrace;
		ct->p = (s->p != NULL) ? s->p : DEFAULT_p;
		ct->unbox = (s->unbox != NULL) ? s->unbox : DEFAULT_unbox;
		ct->free = (s->free != NULL) ? s->free : DEFAULT_free;
		//ct->fnull = (s->fnull != NULL) ? s->fnull : DEFAULT_fnullinit;
		//ct->realtype = (s->realtype != NULL) ? s->realtype : DEFAULT_realtype;
		//ct->isSubType = (s->isSubType != NULL) ? s->isSubType : DEFAULT_isSubType;
		ct->initdef = s->initdef;
	}
	if (ct->initdef != NULL) {
		ct->initdef(_ctx, ct, pline);
	}
	return ct;
}

static kclass_t *CT_body(CTX, kclass_t *ct, size_t head, size_t body)
{
	kclass_t *bct = ct;
	while(ct->cstruct_size < sizeof(kObjectHeader) + head + body) {
		//DBG_P("ct->cstruct_size =%d, request_size = %d", ct->cstruct_size, head+body);
		if(ct->searchSimilarClassNULL == NULL) {
			struct _kclass *newct = new_CT(_ctx, bct, NULL, NOPLINE);
			newct->cflag |= kClass_Private;
			newct->cstruct_size = ct->cstruct_size * 2;
			KINITv(newct->cparam, ct->cparam);
			KINITv(newct->methods, ct->methods);
			((struct _kclass*)ct)->searchSimilarClassNULL = (kclass_t*)newct;
		}
		ct = ct->searchSimilarClassNULL;
	}
	return ct;
}

static kParam *new_Param(CTX, ktype_t rtype, int psize, kparam_t *p)
{
	kclass_t *ct = CT_(CLASS_Param);
	ct = CT_body(_ctx, ct, sizeof(void*), psize * sizeof(kparam_t));
	struct _kParam *pa = (struct _kParam*)new_Object(_ctx, ct, (void*)0);
	pa->rtype = rtype;
	pa->psize = psize;
	if(psize > 0) {
		memcpy(pa->p, p, sizeof(kparam_t) * psize);
	}
	return pa;
}

static void Method_init(CTX, kObject *o, void *conf)
{
	struct _kMethod *mtd = (struct _kMethod*)o;
	bzero(&mtd->fcall_1, sizeof(kMethod) - sizeof(kObjectHeader));
	KINITv(mtd->tcode, (struct kToken*)K_NULL);
	KINITv(mtd->kcode, K_NULL);
}

static void Method_reftrace(CTX, kObject *o)
{
	BEGIN_REFTRACE(3);
	kMethod *mtd = (kMethod*)o;
	KREFTRACEn(mtd->tcode);
	KREFTRACEn(mtd->kcode);
	KREFTRACEn(mtd->proceedNUL);
	END_REFTRACE();
}

static kMethod* new_Method(CTX, uintptr_t flag, kcid_t cid, kmethodn_t mn, knh_Fmethod func)
{
	struct _kMethod* mtd = new_W(Method, NULL);
	mtd->flag  = flag;
	mtd->cid     = cid;
	mtd->mn      = mn;
	kMethod_setFunc(mtd, func);
	return mtd;
}

struct _kAbstractArray {
	kObjectHeader h;
	karray_t a;
} ;

static void Array_init(CTX, kObject *o, void *conf)
{
	struct _kAbstractArray *a = (struct _kAbstractArray*)o;
	a->a.bytebuf     = NULL;
	a->a.bytesize    = 0;
	a->a.bytemax = ((size_t)conf * sizeof(void*));
	if(a->a.bytemax > 0) {
		KARRAY_INIT(&a->a, a->a.bytemax);
	}
	if(TY_isUnbox(O_p0(a))) {
		kArray_setUnboxData(a, 1);
	}
}

static void Array_reftrace(CTX, kObject *o)
{
	kArray *a = (kArray*)o;
	if(!kArray_isUnboxData(a)) {
		size_t i;
		BEGIN_REFTRACE(kArray_size(a));
		TDBG_i("array", (int)kArray_size(a));
		for(i = 0; i < kArray_size(a); i++) {
			KREFTRACEv(a->list[i]);
		}
		END_REFTRACE();
	}
}

static void Array_free(CTX, kObject *o)
{
	struct _kAbstractArray *a = (struct _kAbstractArray*)o;
	KARRAY_FREE(&a->a);
}

static kclass_t *T_realtype(CTX, kclass_t *ct, kclass_t *self)
{
	DBG_ASSERT(ct->optvalue < self->cparam->psize);
	kclass_t *pct = CT_(self->cparam->p[ct->optvalue].ty);
	return pct->realtype(_ctx, pct, self);
}

#define TYPENAME(C) \
	.structname = #C,\
	.cid = CLASS_T##C,\
	.cflag = CFLAG_T##C\

#define CLASSNAME(C) \
	.structname = #C,\
	.cid = CLASS_##C,\
	.cflag = CFLAG_##C,\
	.cstruct_size = sizeof(k##C)\

static void loadInitStructData(CTX)
{
	KDEFINE_CLASS defTvoid = {
		TYPENAME(void),
	};
	KDEFINE_CLASS defTvar = {
		TYPENAME(var),
	};
	KDEFINE_CLASS defObject = {
		CLASSNAME(Object),
		.init = Object_init,
		.reftrace = Object_reftrace,
		.initdef = Object_initdef,
	};
	KDEFINE_CLASS defBoolean = {
		CLASSNAME(Boolean),
		.init = Number_init,
		.unbox = Number_unbox,
		.p    = Boolean_p,
		.fnull = Boolean_fnull,
	};
	KDEFINE_CLASS defInt = {
		CLASSNAME(Int),
		.init  = Number_init,
		.unbox = Number_unbox,
		.p     = Int_p,
	};
	KDEFINE_CLASS defString = {
		CLASSNAME(String),
		.init = String_init,
		.free = String_free,
		.p    = String_p,
		.unbox = String_unbox
	};
	KDEFINE_CLASS defParam = {
		CLASSNAME(Param),
		.init = Param_init,
	};
	KDEFINE_CLASS defMethod = {
		CLASSNAME(Method),
		.init = Method_init,
		.reftrace = Method_reftrace,
	};
	kparam_t ArrayCparam = {TY_Object, 1};
	KDEFINE_CLASS defArray = {
		CLASSNAME(Array),
		.init = Array_init,
		.reftrace = Array_reftrace,
		.free = Array_free,
//		.psize = 1, .cparams = &ArrayCparam,
	};
	KDEFINE_CLASS defSystem = {
		CLASSNAME(System),
		.init = DEFAULT_init,
	};
	KDEFINE_CLASS defT0 = {
		TYPENAME(0),
		.init = DEFAULT_init,
		.realtype = T_realtype,
	};
	KDEFINE_CLASS *DATATYPES[] = {
		&defTvoid,
		&defTvar,
		&defObject,
		&defBoolean,
		&defInt,
		&defString,
		&defParam,
		&defMethod,
		&defArray,
		&defSystem,
		&defT0,
		NULL,
	};
	KDEFINE_CLASS **dd = DATATYPES;
	while(dd[0] != NULL) {
		new_CT(_ctx, NULL, dd[0], 0);
		dd++;
	}
	struct _kclass *ct = (struct _kclass *)CT_Array;
	ct->cparam = new_Param(_ctx, TY_void, 1, &ArrayCparam);
}

static kclass_t *addClassDef(CTX, kpack_t packid, kpack_t packdom, kString *name, KDEFINE_CLASS *cdef, kline_t pline)
{
	struct _kclass *ct = new_CT(_ctx, NULL, cdef, pline);
	ct->packid = packid;
	ct->packdom = packdom;
	if (name == NULL) {
		const char *n = cdef->structname;
		//ct->nameid = kuname(n, strlen(n), SPOL_ASCII|SPOL_POOL|SPOL_TEXT, _NEWID);
	} else {
		//ct->nameid = kuname(S_text(name), S_size(name), 0, _NEWID);
	}
	//CT_setName(_ctx, ct, pline);
	return (kclass_t*)ct;
}

static KMETHOD Fmethod_abstract(CTX, ksfp_t *sfp _RIX)
{
	//kMethod *mtd = sfp[K_MTDIDX].mtdNC;
	//char mbuf[128];
	//kreportf(WARN_, sfp[K_RTNIDX].uline, "calling abstract method: %s.%s", T_cid(mtd->cid), T_mn(mbuf, mtd->mn));
	RETURNi_(0); //necessary
}

static void Method_setFunc(CTX, kMethod *mtd, knh_Fmethod func)
{
	func = (func == NULL) ? Fmethod_abstract : func;
	((struct _kMethod*)mtd)->fcall_1 = func;
	//((struct _kMethod*)mtd)->pc_start = CODE_NCALL;

}

static void Array_ensureMinimumSize(CTX, struct _kAbstractArray *a, size_t min)
{
	if(!((min * sizeof(void*)) < a->a.bytemax)) {
		if(min < sizeof(kObject)) min = sizeof(kObject);
		KARRAY_EXPAND(&a->a, min);
	}
}

static void Array_add(CTX, kArray *o, kObject *value)
{
	size_t asize = kArray_size(o);
	struct _kAbstractArray *a = (struct _kAbstractArray*)o;
	Array_ensureMinimumSize(_ctx, a, asize+1);
	DBG_ASSERT(a->a.objects[asize] == NULL);
	KINITv(a->a.objects[asize], value);
	a->a.bytesize = (asize+1) * sizeof(void*);
}

static void KCLASSTABLE_initklib2(struct _klib2 *l)
{
	l->Knew_Object = new_Object;
	l->Knew_Method   = new_Method;
	l->KaddClassDef = addClassDef;
	l->KMethod_setFunc = Method_setFunc;
	l->KArray_add = Array_add;
}

static void CT_setName(CTX, struct _kclass *ct, kline_t pline)
{
	//uintptr_t lname = longid(ct->packdom, ct->nameid);
	//kreportf(DEBUG_, pline, "new class domain=%s, name='%s.%s'", T_PN(ct->packdom), T_PN(ct->packid), T_UN(ct->nameid));
	//kclass_t *ct2 = (kclass_t*)map_getu(_ctx, _ctx->share->lcnameMapNN, lname, (uintptr_t)NULL);
	//if(ct2 == NULL) {
	//	map_addu(_ctx, _ctx->share->lcnameMapNN, lname, (uintptr_t)ct);
	//}
	if(ct->methods == NULL) {
		KINITv(ct->methods, K_EMPTYARRAY);
		if(ct->cid > CLASS_Object) {
			ct->searchSuperMethodClassNULL = CT_(ct->supcid);
		}
	}
	//if(ct->cparam == NULL) {
	//	KINITv(ct->cparam, K_NULLPARAM);
	//}
}

static void initStructData(CTX)
{
	kclass_t **ctt = (kclass_t**)_ctx->share->ca.cts;
	size_t i;
	for (i = 0; i <= CLASS_T0; i++) {
		struct _kclass *ct = (struct _kclass *)ctt[i];
		//const char *name = ct->DBG_NAME;
		//ct->nameid = kuname(name, strlen(name), SPOL_ASCII|SPOL_POOL|SPOL_TEXT, _NEWID);
		CT_setName(_ctx, ct, 0);
	}
}

static void KCLASSTABLE_init(kcontext_t *_ctx)
{
	static kshare_t share;
	_ctx->share = &share;
	KCLASSTABLE_initklib2((struct _klib2*)_ctx->lib2);
	KARRAY_INIT(&(share.ca), MAX_CT * sizeof(kclass_t));
	loadInitStructData(_ctx);
	KINITv(share.constNull, new_(Object, NULL));
	kObject_setNullObject(share.constNull, 1);
	//
	KINITv(share.emptyArray, new_(Array, 0));
	initStructData(_ctx);
}

#define _Public    kMethod_Public
#define _Const     kMethod_Const
#define _Immutable kMethod_Immutable
#define _F(F)      (intptr_t)(F)

static void KCLASSTABLE_loadMethod(CTX)
{
	//int FN_x = FN_("x");
	intptr_t MethodData[] = {
		_F(Object_toString), TY_Object, MN_to(TY_String),
		_F(Boolean_opNOT), TY_Boolean, MN_("opNOT"),
		_F(Int_opMINUS), TY_Int, MN_("opMINUS"),
		_F(Int_opADD), TY_Int, MN_("opADD"),
		_F(Int_opSUB), TY_Int, MN_("opSUB"),
		_F(Int_opMUL), TY_Int, MN_("opMUL"),
		/* opDIV and opMOD raise zero divided exception. Don't set _Const */
		_F(Int_opDIV), TY_Int, MN_("opDIV"),
		_F(Int_opMOD), TY_Int, MN_("opMOD"),
		_F(Int_opEQ),  TY_Int, MN_("opEQ"),
		_F(Int_opNEQ), TY_Int, MN_("opNEQ"),
		_F(Int_opLT),  TY_Int, MN_("opLT"),
		_F(Int_opLTE), TY_Int, MN_("opLTE"),
		_F(Int_opGT),  TY_Int, MN_("opGT"),
		_F(Int_opGTE), TY_Int, MN_("opGTE"),
		_F(Int_toString), TY_Int, MN_to(TY_String),
		_F(String_toInt), TY_String, MN_to(TY_Int),
		_F(String_opADD), TY_String, MN_("opADD"),
		_F(System_assert), TY_System, MN_("assert"),
		_F(System_p), TY_System, MN_("p"),
		_F(System_gc), TY_System, MN_("gc"),
		DEND,
		//_Public|_Immutable|_Const, _F(Object_toString), TY_String, TY_Object, MN_to(TY_String), 0,
		//_Public|_Immutable|_Const, _F(Boolean_opNOT), TY_Boolean, TY_Boolean, MN_("opNOT"), 0,
		//_Public|_Immutable|_Const, _F(Int_opMINUS), TY_Int, TY_Int, MN_("opMINUS"), 0,
		//_Public|_Immutable|_Const, _F(Int_opADD), TY_Int, TY_Int, MN_("opADD"), 1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_opSUB), TY_Int, TY_Int, MN_("opSUB"), 1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_opMUL), TY_Int, TY_Int, MN_("opMUL"), 1, TY_Int, FN_x,
		///* opDIV and opMOD raise zero divided exception. Don't set _Const */
		//_Public|_Immutable, _F(Int_opDIV), TY_Int, TY_Int, MN_("opDIV"), 1, TY_Int, FN_x,
		//_Public|_Immutable, _F(Int_opMOD), TY_Int, TY_Int, MN_("opMOD"), 1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_opEQ),  TY_Boolean, TY_Int, MN_("opEQ"),  1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_opNEQ), TY_Boolean, TY_Int, MN_("opNEQ"), 1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_opLT),  TY_Boolean, TY_Int, MN_("opLT"),  1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_opLTE), TY_Boolean, TY_Int, MN_("opLTE"), 1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_opGT),  TY_Boolean, TY_Int, MN_("opGT"),  1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_opGTE), TY_Boolean, TY_Int, MN_("opGTE"), 1, TY_Int, FN_x,
		//_Public|_Immutable|_Const, _F(Int_toString), TY_String, TY_Int, MN_to(TY_String), 0,
		//_Public|_Immutable|_Const, _F(String_toInt), TY_Int, TY_String, MN_to(TY_Int), 0,
		//_Public|_Immutable|_Const, _F(String_opADD), TY_String, TY_String, MN_("opADD"), 1, TY_String, FN_x | FN_COERCION,
		//_Public|_Immutable, _F(System_assert), TY_void, TY_System, MN_("assert"), 1, TY_Boolean, FN_x,
		//_Public|_Immutable, _F(System_p), TY_void, TY_System, MN_("p"), 1, TY_String, FN_("s") | FN_COERCION,
		//_Public|_Immutable, _F(System_gc), TY_void, TY_System, MN_("gc"), 0,
		//DEND,
	};
	kKonohaSpace_loadMethodData(NULL, MethodData);
}
