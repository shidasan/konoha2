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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef K_INCLUDE_BUILTINAPI

/* ------------------------------------------------------------------------ */

static kbool_t bytes_startsWithLink(kbytes_t t, kbytes_t scheme)
{
	if(knh_bytes_startsWith_(t, scheme)) {
		if(t.text[scheme.len] == ':') return 1;
	}
	return 0;
}

static kbytes_t bytes_trim(kbytes_t t)
{
	while(isspace(t.utext[0])) {
		t.utext++;
		t.len--;
	}
	if(t.len == 0) return t;
	while(isspace(t.utext[t.len-1])) {
		t.len--;
		if(t.len == 0) return t;
	}
	return t;
}

/* ------------------------------------------------------------------------ */
/* [new] */

//## @Hidden method This Object.new();

static KMETHOD Object_new(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(sfp[0].o);
}

/* ------------------------------------------------------------------------ */

//static void ObjectField_setValue(CTX, kObject *of, kindex_t idx, ktype_t type, Object *value)
//{
//	kcid_t tcid = CLASS_t(type);
//	kcid_t scid = O_cid(value);
//	DBG_ASSERT_cid(tcid);
//	DBG_ASSERT_cid(scid);
//	if(scid == tcid || knh_class_instanceof(_ctx, scid, tcid)) {
//		goto L_SETVAL;
//	}
//	DBG_P("COERCION %s -> %s", CLASS__(scid), CLASS__(tcid));
//	TODO();
////		BEGIN_LOCAL(_ctx, lsfp);
////		KNH_LPUSH(_ctx, o);
////		VM_MAP(_ctx, tcid);
////		o = ctx->esp[0].o;
////		END_LOCAL(_ctx, lsfp);
//	return ;
//
//	L_SETVAL:;
//	if(IS_Tint(type)) {
//		kint_t *data = (kint_t*)(of->fields + idx);
//		data[0] = N_toint(value);
//	}
//	else if(IS_Tfloat(type)) {
//		kfloat_t *data = (kfloat_t*)(of->fields + idx);
//		data[0] = N_tofloat(value);
//	}
//	else if(IS_Tbool(type)) {
//		kbool_t *data = (kbool_t*)(of->fields +idx);
//		data[0] = N_tobool(value);
//	}
//	else {
//		DBG_ASSERT(of->fields[idx] != NULL);
//		KSETv(of->fields[idx], value);
//	}
//}

/* ------------------------------------------------------------------------ */
//## @Hidden method This Object.new:MAP(dynamic value, ...);

static KMETHOD Object_newMAP(CTX, ksfp_t *sfp _RIX)
{
//	kObject *of = (kObject*)sfp[0].o;
//	kcid_t cid = O_cid(of);
//	ksfp_t *v = sfp + 1;
//	size_t i, ac = knh_stack_argc(_ctx, v);
//	for(i = 0; i < ac; i+= 2) {
//		if(IS_bString(v[i].s)) {
//			ksymbol_t fn = knh_getfnq(_ctx, S_tobytes(v[i].s), FN_NONAME);
//			if(fn == FN_NONAME) continue;
//			kindex_t idx = knh_Class_queryField(_ctx, cid, fn);
//			if(idx == -1) continue;
//			kfieldinfo_t *cf = knh_Class_fieldAt(_ctx, cid, idx);
//			ktype_t type = ktype_tocid(_ctx, cf->type, cid);
//			if(type == TY_void) continue;
//			DBG_P("[%d] %s %s", (int)(idx), T__(type), S_text(v[i].s));
//			ObjectField_setValue(_ctx, of, idx, type, v[i+1].o);
//		}
//	}
//	RETURN_(of);
}

/* ------------------------------------------------------------------------ */
//## @Const @FastCall method Class! Object.getClass();

static KMETHOD Object_getClass(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_Type(_ctx, O_cid(sfp[0].o)));
}

/* ------------------------------------------------------------------------ */
//## @FastCall @Const method Int Object.hashCode();

static KMETHOD Object_hashCode(CTX, ksfp_t *sfp _RIX)
{
	kuint_t h = ClassTBL(O_bcid(sfp[0].o))->cdef->hashCode(_ctx, sfp[0].p);
	RETURNi_(h);
}

/* ------------------------------------------------------------------------ */
//## @Const @Hidden @FastCall method Boolean Object.isNull();

static KMETHOD Object_isNull(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(IS_NULL(sfp[0].o));
}

/* ------------------------------------------------------------------------ */
//## @Const @Hidden @FastCall method Boolean Object.isNotNull();

static KMETHOD Object_isNotNull(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(IS_NOTNULL(sfp[0].o));
}

///* ------------------------------------------------------------------------ */
////## @Const @Hidden @FastCall method String Object.getKey();
//
//static KMETHOD Object_getKey(CTX, ksfp_t *sfp _RIX)
//{
//	kString *s = ClassTBL(O_bcid(sfp[0].o))->cdef->getkey(_ctx, sfp);
//	KNH_ASSERT(IS_String(s));
//	RETURN_(s);
//}
//
///* ------------------------------------------------------------------------ */
////## @Const @Hidden method This Object.copy();
//
//static KMETHOD Object_copy(CTX, ksfp_t *sfp _RIX)
//{
//	kObject *src = sfp[0].o;
//	kcid_t cid = O_cid(src);
//	if(knh_class_canObjectCopy(_ctx, cid) && IS_NOTNULL(src)) {
//		const kclass_t *ct = O_ct(src);
//		kObject *o = new_hObject_(_ctx, ct);
//		o->h.magicflag = src->h.magicflag;
//		ct->cdef->initcopy(_ctx, RAWPTR(o), RAWPTR(src));
//		src = o;
//	}
//	sfp[K_RIX].ndata = sfp[0].ndata;
//	RETURN_(src);
//}

/* ------------------------------------------------------------------------ */
//## @Const @Hidden @Private method dyn Object.cast(TypeMap tmr);

static KMETHOD Object_cast(CTX, ksfp_t *sfp _RIX)
{
	sfp[0].ndata = O_ndata(sfp[0].o); // UNBOX
	knh_TypeMap_exec(_ctx, sfp[1].tmr, sfp, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Const @Hidden @Private method dynamic Object.to(Class auto);

static KMETHOD Object_to(CTX, ksfp_t *sfp _RIX)
{
	kcid_t scid = O_cid(sfp[0].o), tcid = (sfp[1].c)->cid;
	if(scid != tcid) {
		kTypeMap *tmr = knh_findTypeMapNULL(_ctx, scid, tcid);
		if(tmr != NULL) {
			sfp[0].ndata = O_ndata(sfp[0].i); // UNBOX
			knh_TypeMap_exec(_ctx, tmr, sfp, K_RIX);
		}
		else {
			sfp[K_RIX].ivalue = 0;
			RETURN_(KNH_NULVAL(tcid));
		}
	}
	else {
		RETURN_(sfp[0].o);
	}
}

/* ------------------------------------------------------------------------ */
//## @Const @Hidden @Private method dynamic Object.typeCheck(Class auto);

static KMETHOD Object_typeCheck(CTX, ksfp_t *sfp _RIX)
{
	kcid_t scid = O_cid(sfp[0].o), tcid = (sfp[1].c)->cid;
	if(scid != tcid && !class_isa(scid, tcid)) {
		kTypeMap *tmr = knh_findTypeMapNULL(_ctx, scid, tcid);
		if(tmr != NULL && TypeMap_isSemantic(tmr)) {
			sfp[0].ndata = O_ndata(sfp[0].i); // UNBOX
			knh_TypeMap_exec(_ctx, tmr, sfp, K_RIX);
		}
		else {
			DBG_P("reqt=%s", T__(tcid));
			THROW_TypeError(_ctx, sfp, tcid, scid);
		}
	}
	else {
		sfp[K_RIX].ndata = O_ndata(sfp[0].o);
		RETURN_(sfp[0].o);
	}
}

/* ------------------------------------------------------------------------ */
//## @Hidden method dynamic Object.opWITH(Map data, Lingo _, Boolean _);

static KMETHOD Object_opWITH(CTX, ksfp_t *sfp _RIX)
{
	knh_Object_setData(_ctx, sfp[0].o, sfp[1].m, sfp[2].ns, sfp[3].bvalue);
	RETURN_(sfp[0].o);
}

/* ------------------------------------------------------------------------ */
/* %empty */

//## method void Object.%empty();
static KMETHOD Object__empty(CTX, ksfp_t *sfp _RIX)
{
	if(CTX_isDebug(_ctx)) {
		KNH_LOG("%%empty(%s)", O__(sfp[1].o));
	}
}

/* ------------------------------------------------------------------------ */
/* %s */

//## method void Object.%s();
static KMETHOD Object__s(CTX, ksfp_t *sfp _RIX)
{
	knh_write_Object(_ctx, sfp[0].w, sfp[1].o, FMT_s);
}

/* ------------------------------------------------------------------------ */
/* %s */

//## method void Object.%k();
static KMETHOD Object__k(CTX, ksfp_t *sfp _RIX)
{
	knh_write_Object(_ctx, sfp[0].w, sfp[1].o, FMT_line);
}

/* ------------------------------------------------------------------------ */
/* %s */

//## method void Object.%data();
static KMETHOD Object__data(CTX, ksfp_t *sfp _RIX)
{
	knh_write_Object(_ctx, sfp[0].w, sfp[1].o, FMT_data);
}

/* ------------------------------------------------------------------------ */
/* %s */

//## method void Object.%dump();
static KMETHOD Object__dump(CTX, ksfp_t *sfp _RIX)
{
	knh_write_Object(_ctx, sfp[0].w, sfp[1].o, FMT_dump);
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Static @Const method Tdynamic Tdynamic.opLINK(String path, Lingo _);

static KMETHOD Tdynamic_opLINK(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = knh_bytes_next(S_tobytes(sfp[1].s), ':');
	const kclass_t *ct = knh_Lingo_getLinkClassTBLNULL(_ctx, sfp[2].ns, t, CLASS_Tdynamic);
	if(ct != NULL) {
		RETURN_(new_Type(_ctx, ct->cid));
	}
	RETURN_(K_NULL);
}

/* ------------------------------------------------------------------------ */
//## method void Boolean.%s();
//## method void Boolean.%k();
//## method void Boolean.%data();
//## method void Boolean.%dump();

static KMETHOD Boolean__s(CTX, ksfp_t *sfp _RIX)
{
	knh_write_bool(_ctx, sfp[0].w, sfp[1].bvalue);
}

/* ------------------------------------------------------------------------ */
//## method void Int.%s();
//## method void Int.%d();

static KMETHOD Int__s(CTX, ksfp_t *sfp _RIX)
{
	knh_write_ifmt(_ctx, sfp[0].w, KINT_FMT, sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## method void Float.%s();
//## method void Float.%f();

static KMETHOD Float__s(CTX, ksfp_t *sfp _RIX)
{
	knh_write_ffmt(_ctx, sfp[0].w, KFLOAT_FMT, sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## method void Int.%k();
//## method void Int.%data();
//## method void Int.%dump();

static KMETHOD Int__k(CTX, ksfp_t *sfp _RIX)
{
#if defined(K_USING_SEMANTICS)
	kSemantics *u = knh_getSemantics(_ctx, O_cid(sfp[1].o));
	knh_write_intx(_ctx, sfp[0].w, u, sfp[1].ivalue);
#else
	knh_write_ifmt(_ctx, sfp[0].w, KINT_FMT, sfp[1].ivalue);
#endif
}

/* ------------------------------------------------------------------------ */
//## method void Float.%k();
//## method void Float.%data();
//## method void Float.%dump();

static KMETHOD Float__k(CTX, ksfp_t *sfp _RIX)
{
#if defined(K_USING_SEMANTICS)
	kSemantics *u = knh_getSemantics(_ctx, O_cid(sfp[1].o));
	knh_write_floatx(_ctx, sfp[0].w, u, sfp[1].fvalue);
#else
	knh_write_ffmt(_ctx, sfp[0].w, KFLOAT_FMT, sfp[1].fvalue);
#endif
}

/* ------------------------------------------------------------------------ */
//## @Static @FastCall method void System.setRandomSeed(Int seed);

static KMETHOD System_setRandomSeed(CTX, ksfp_t *sfp _RIX)
{
	kuint_t seed = Int_to(kuint_t, sfp[1]);
	knh_srand(seed);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @Static @FastCall method Int Int.random(Int n);

static KMETHOD Int_random(CTX, ksfp_t *sfp _RIX)
{
	kuint_t n = knh_rand();
	kuint_t max = Int_to(kuint_t, sfp[1]);
	if(max > 0) {
		n = n % max;
	}
	RETURNi_(n);
}

/* ------------------------------------------------------------------------ */
//## @Static @FastCall method Float! Float.random();

static KMETHOD Float_random(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(kfloat_rand());
}

/* ------------------------------------------------------------------------ */
//## method void Int.%c();

static KMETHOD Int__c(CTX, ksfp_t *sfp _RIX)
{
	char buf[16];
	kuint_t c = (kuint_t) sfp[1].ivalue;
	knh_format_utf8(buf, sizeof(buf), c);
	knh_write(_ctx, sfp[0].w, B(buf));
}

/* ------------------------------------------------------------------------ */
//## method void Int.%u();

static KMETHOD Int__u(CTX, ksfp_t *sfp _RIX)
{
	knh_write_ifmt(_ctx, sfp[0].w, KUINT_FMT, sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## method void Int.%f();

static KMETHOD Int__f(CTX, ksfp_t *sfp _RIX)
{
	knh_write_ffmt(_ctx, sfp[0].w, KFLOAT_FMT, (kfloat_t)sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## method void Int.%x();

static KMETHOD Int__x(CTX, ksfp_t *sfp _RIX)
{
	knh_write_ifmt(_ctx, sfp[0].w, KINT_XFMT, sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */

static void knh_write_bits(CTX, kOutputStream *w, uint64_t n, size_t bits)
{
	size_t i;
	uint64_t flag = 1ULL << (bits - 1);
	for(i = 0; i < bits; i++) {
		if(i > 0 && i % 8 == 0) {
			kwb_putc(wb, ' ');
		}
		if((flag & n) == flag) {
			kwb_putc(wb, '1');
		}else{
			kwb_putc(wb, '0');
		}
		flag = flag >> 1;
	}
}

/* ------------------------------------------------------------------------ */
//## method void Int.%bits();

static KMETHOD Int__bits(CTX, ksfp_t *sfp _RIX)
{
	knh_write_bits(_ctx, sfp[0].w, sfp[1].ivalue, sizeof(kint_t) * 8);
}

/* ------------------------------------------------------------------------ */
//## method void Float.%d();

static KMETHOD Float__d(CTX, ksfp_t *sfp _RIX)
{
	knh_write_ifmt(_ctx, sfp[0].w, KINT_FMT, (kint_t)sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## method void Float.%bits();

static KMETHOD Float__bits(CTX, ksfp_t *sfp _RIX)
{
	knh_write_bits(_ctx, sfp[0].w, sfp[1].ndata, sizeof(kfloat_t) * 8);
}

/* ------------------------------------------------------------------------ */
//## @Const mapper Boolean String;

static TYPEMAP Boolean_String(CTX, ksfp_t *sfp _RIX)
{
	kString *s = (Boolean_to(int, (sfp[0]))) ? new_T("true") : new_T("false");
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const mapper Int String;

static TYPEMAP Int_String(CTX, ksfp_t *sfp _RIX)
{
	char buf[80];
	knh_snprintf(buf, sizeof(buf), KINT_FMT, sfp[0].ivalue);
	RETURN_(new_kString(buf, knh_strlen(buf), SPOL_ASCII|SPOL_POOL));
}

/* ------------------------------------------------------------------------ */
//## @Const  mapper Float String;

static TYPEMAP Float_String(CTX, ksfp_t *sfp _RIX)
{
	char buf[256];
	knh_snprintf(buf, sizeof(buf), KFLOAT_FMT, sfp[0].fvalue);
	RETURN_(new_kString(buf, knh_strlen(buf), SPOL_ASCII|SPOL_POOL));
}

/* ------------------------------------------------------------------------ */
//## @Const @Semantic mapper Float Int;

static TYPEMAP Float_Int(CTX, ksfp_t *sfp _RIX)
{
	kint_t v = (kint_t)sfp[0].fvalue;
	RETURNi_(v);
}

/* ------------------------------------------------------------------------ */
//## @Const @Semantic mapper Int Float;

static TYPEMAP Int_Float(CTX, ksfp_t *sfp _RIX)
{
	kfloat_t v = (kfloat_t)sfp[0].ivalue;
	RETURNf_(v);
}

/* ------------------------------------------------------------------------ */

static void Date_reset(kdate_t *dt)
{
	dt->year  = (kshort_t)0;
	dt->month = (kshort_t)0;
	dt->day   = (kshort_t)0;
	dt->hour  = (kshort_t)0;
	dt->min   = (kshort_t)0;
	dt->sec   = (kshort_t)0;
}

static void Date_setsfp(kdate_t *dt, ksfp_t *sfp)
{
	dt->year  = (kshort_t)sfp[1].ivalue;
	dt->month = (kshort_t)sfp[2].ivalue;
	dt->day   = (kshort_t)sfp[3].ivalue;
	dt->hour  = (kshort_t)(sfp[4].ivalue % 60);
	dt->min   = (kshort_t)(sfp[5].ivalue % 60);
	dt->sec   = (kshort_t)(sfp[6].ivalue % 60);
}

static kbool_t bytes_parsedt(kbytes_t *t, kshort_t *value, int delim, kshort_t def)
{
	if(t->len == 0) {
		value[0] = def;
		return 1;
	}
	int p = 0, ch = t->text[0], num = 0;
	while(p < t->len) {
		if(!isdigit(ch)) return 0;
		num += (ch - '0'); p++; ch = t->text[p];
		if(ch == delim) {
			p++;
			t->text += (p);
			t->len  -= (p);
			value[0] = (kshort_t)num;
			return 1;
		}
		if(ch == 0) {
			t->text = "";
			t->len  = 0;
			value[0] = (kshort_t)num;
			return 1;
		}
		num *= 10;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */
//## method Date Date.new(Int year, Int month, Int day, Int hour, Int min, Int sec);

static KMETHOD Date_new(CTX, ksfp_t *sfp _RIX)
{
	kDate *dt = sfp[0].dt;
	if(sfp[2].ivalue > 0) {
		Date_setsfp(&dt->dt, sfp);
		if(sfp[1].ivalue != 0) {
			Date_reset(&dt->dt);
			dt->dt.year = (kshort_t)sfp[1].ivalue;
		}
	}
	RETURN_(dt);
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Static @Const method Date Date.opLINK(String path, Lingo _);

static KMETHOD Date_opLINK(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = knh_bytes_next(S_tobytes(sfp[1].s), ':');
	kDate *dt = (kDate*)new_Object_init2(_ctx, ClassTBL(CLASS_Date));
	KSETv(sfp[2].o, dt); // TO AVOIDGC
	if(bytes_parsedt(&t, &dt->dt.year, '-', 0)   &&
		bytes_parsedt(&t, &dt->dt.month, '-', 1) &&
		bytes_parsedt(&t, &dt->dt.day, 'T', 1) &&
		bytes_parsedt(&t, &dt->dt.hour, ':', 0) &&
		bytes_parsedt(&t, &dt->dt.min, ':', 0) &&
		bytes_parsedt(&t, &dt->dt.sec, '+', 0)) {
		RETURN_(dt);
	}
	else {
		RETURN_(KNH_TNULL(Date));
	}
}

/* ------------------------------------------------------------------------ */
//## @Const mapper Date String;
static TYPEMAP Date_String(CTX, ksfp_t *sfp _RIX)
{
	kDate *dt = (kDate*)sfp[0].o;
	char buf[80];
	knh_snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d%+02d:%02d",
		(int)(dt->dt.year), (int)(dt->dt.month), (int)dt->dt.day,
		(int)(dt->dt.hour), (int)(dt->dt.min), (int)dt->dt.sec, (int)(dt->dt.gmtoff / 60), (int)(dt->dt.gmtoff % 60));
	RETURN_(new_String(_ctx, buf));
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Static @Const method Path Path.opLINK(String path, Lingo _);

static KMETHOD Path_opLINK(CTX, ksfp_t *sfp _RIX)
{
	const knh_PathDPI_t *dpi = knh_Lingo_getStreamDPINULL(_ctx, sfp[2].ns, S_tobytes(sfp[1].s));
	kPath *pth = new_(Path);
	KSETv(pth->urn, sfp[1].s);
	pth->dpi = dpi;
	dpi->ospath(_ctx, pth, sfp[2].ns);
	RETURN_(pth);
}

/* ------------------------------------------------------------------------ */
//## @Const @Semantic mapper String Path;

static TYPEMAP String_Path(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_Path(_ctx, sfp[K_TMRIDX].s));
}

/* ------------------------------------------------------------------------ */
//## @Const @Semantic mapper Path String;

static TYPEMAP Path_String(CTX, ksfp_t *sfp _RIX)
{
	kPath *pth = (kPath*)sfp[K_TMRIDX].o;
	RETURN_(pth->urn);
}

/* ------------------------------------------------------------------------ */
//## mapper Path Boolean;

static TYPEMAP Path_Boolean(CTX, ksfp_t *sfp _RIX)
{
	kPath *pth = (kPath*)sfp[K_TMRIDX].o;
	RETURNb_(pth->dpi->existsSPI(_ctx, pth));
}

/* ------------------------------------------------------------------------ */
//## mapper Path InputStream;

static TYPEMAP Path_InputStream(CTX, ksfp_t *sfp _RIX)
{
	kPath *path = (kPath*)sfp[K_TMRIDX].o;
	kInputStream *in = new_InputStream(_ctx, NULL, path);
	if(in == NULL) in = KNH_TNULL(InputStream);
	RETURN_(in);
}

/* ------------------------------------------------------------------------ */
//## method Bytes Bytes.new(Int init);

static KMETHOD Bytes_new(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t init = sfp[1].ivalue <= K_FASTMALLOC_SIZE ? K_FASTMALLOC_SIZE : k_goodsize(Int_to(size_t, sfp[1]));
	DBG_ASSERT(ba->dim->capacity == 0);
	ba->bu.ubuf = (kchar_t*)KNH_ZMALLOC(init);
	knh_bzero(ba->bu.ubuf, init);
	ba->dim = new_dim(_ctx, init, 1);
	ba->bu.len = 0;
	RETURN_(ba);
}

/* ------------------------------------------------------------------------ */
//## method Bytes Bytes.new:ARRAY(Int init);

static KMETHOD Bytes_newARRAY(CTX, ksfp_t *sfp _RIX)
{
	size_t size = sfp[1].ivalue <=0 ? 0 : Int_to(size_t, sfp[1]);
	Bytes_new(_ctx, sfp, K_RIX);
	sfp[K_RIX].ba->bu.len = size;
}

/* ------------------------------------------------------------------------ */
//## @Final mapper Bytes Pointer;

static TYPEMAP Bytes_Pointer(CTX, ksfp_t *sfp _RIX)
{
	kPointer *p = new_(Pointer);
	KSETv(p->gcref, sfp[0].o);
	p->ptr = (void*)BA_totext(sfp[0].ba);
	p->size = BA_size(sfp[0].ba);
	p->wsize = 1;
	RETURN_(p);
}

/* ------------------------------------------------------------------------ */
//## method Pointer Pointer.opADD(int n);

static KMETHOD Pointer_opADD(CTX, ksfp_t *sfp _RIX)
{
	kPointer *p = (kPointer*)sfp[0].o;
	kPointer *np = new_(Pointer);
	intptr_t n = (intptr_t)sfp[1].ivalue * p->wsize;
	np->uptr = p->uptr + n;
	np->wsize = p->wsize;
	np->size = p->size - n;
	KSETv(np->gcref, p->gcref);
	RETURN_(np);
}

/* ------------------------------------------------------------------------ */
//## method String String.new(Bytes buf, StringDecoder c);

static KMETHOD String_new(CTX, ksfp_t *sfp _RIX)
{
	kString *s;
	if(IS_NULL(sfp[2].o)) {
		s = new_kString(BA_totext(sfp[1].ba), BA_size(sfp[1].ba), 0);
	}
	else {
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		kStringDecoder *c = (kStringDecoder*)sfp[2].o;
		c->dpi->dec(_ctx, c->conv, BA_totext(sfp[1].ba), BA_size(sfp[1].ba), cwb->ba);
		s = CWB_newString(_ctx, cwb, 0);
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method Regex Regex.new(String pattern, String option);

static KMETHOD Regex_new(CTX, ksfp_t *sfp _RIX)
{
	kRegex *re = (kRegex*)sfp[0].o;
	const char *ptn = S_text(sfp[1].s);
	const char *opt = IS_NULL(sfp[2].o) ? "" : S_text(sfp[2].s);
	knh_Regex_setGlobalOption(_ctx, re, opt);
	KSETv(re->pattern, sfp[1].s);
	re->spi = knh_getRegexSPI();
	re->reg = re->spi->regmalloc(_ctx, sfp[1].s);
	re->spi->regcomp(_ctx, re->reg, ptn, re->spi->parse_cflags(_ctx, opt));
	re->eflags = re->spi->parse_eflags(_ctx, opt);
	RETURN_(sfp[0].o);
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Private method dynamic String.opLINK(String path, Lingo _, Class _);

static KMETHOD String_opLINK(CTX, ksfp_t *sfp _RIX)
{
	kcid_t cid = (sfp[3].c)->cid;
	DBG_ASSERT(IS_Lingo(sfp[2].ns));
	DBG_ASSERT(IS_String(sfp[1].s));
	if(!bytes_startsWithLink(S_tobytes(sfp[1].s), S_tobytes(sfp[0].s))) {
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		knh_Bytes_write(_ctx, cwb->ba, S_tobytes(sfp[0].s));
		kwb_putc(cwb->ba, ':');
		knh_Bytes_write(_ctx, cwb->ba, S_tobytes(sfp[1].s));
		KSETv(sfp[1].s, CWB_newString(_ctx, cwb, SPOL_POOL));
	}
	kObject* v = knh_Lingo_newObject(_ctx, sfp[2].ns, sfp[1].s, cid);
	DBG_P("v=%p, true=%p, false=%p", v, K_TRUE, K_FALSE);
	RETURN_(v);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method Boolean String.opEXISTS(Lingo _);

static KMETHOD String_opEXISTS(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(IS_Lingo(sfp[1].ns));
	kObject* btf = knh_Lingo_newObject(_ctx, sfp[1].ns, sfp[0].s, CLASS_Boolean);
	DBG_P("btf=%p, true=%p, false=%p", btf, K_TRUE, K_FALSE);
	RETURNb_(btf == K_TRUE);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.equals(String s);

static KMETHOD String_equals(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_((S_size(sfp[0].s) == S_size(sfp[1].s) &&
		knh_bytes_strcmp(S_tobytes(sfp[0].s), S_tobytes(sfp[1].s)) == 0));
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.startsWith(String s);

static KMETHOD String_startsWith(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_strncmp(S_text(sfp[0].s), S_text(sfp[1].s), S_size(sfp[1].s)) == 0);
	//RETURNb_(knh_bytes_startsWith_(S_tobytes(sfp[0].s), S_tobytes(sfp[1].s))); older
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.endsWith(String s);

static KMETHOD String_endsWith(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_bytes_endsWith_(S_tobytes(sfp[0].s), S_tobytes(sfp[1].s)));
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.concat(Object value, ...);
//## @Const method String String.opADD(dynamic value);

static KMETHOD String_concat(CTX, ksfp_t *sfp _RIX)
{
	int i, ac = knh_stack_argc(_ctx, sfp);
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	for(i = 0; i < ac; i++) {
		if(IS_bString(sfp[i].o)) {
			knh_Bytes_write(_ctx, cwb->ba, S_tobytes(sfp[i].s));
		}
		else {
			knh_write_Object(_ctx, cwb->w, sfp[i].o, FMT_s);
		}
	}
	RETURN_(CWB_newString(_ctx, cwb, 0));
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.indexOf(String s);

static KMETHOD String_indexOf(CTX, ksfp_t *sfp _RIX)
{
	long loc = -1;
	kbytes_t base = S_tobytes(sfp[0].s);
	char *p = strstr(base.text, S_text(sfp[1].s));
	if (p != NULL) {
		loc = p - base.text;
		if(!String_isASCII(sfp[0].s)) {
			base.len = (size_t)loc;
			loc = knh_bytes_mlen(base);
		}
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.lastIndexOf(String s);

static KMETHOD String_lastIndexOf(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t base = S_tobytes(sfp[0].s);
	if (IS_NULL(sfp[1].o)) RETURNi_(-1);
	kbytes_t delim = S_tobytes(sfp[1].s);
	kindex_t loc = base.len - delim.len;
	if(delim.len == 0) loc--;
	for(; loc >= 0; loc--) {
		if(base.utext[loc] == delim.utext[0]) {
			kbytes_t sub = {{base.text + loc}, delim.len};
			if(knh_bytes_strcmp(sub, delim) == 0) break;
		}
	}
	if (loc >= 0 && !String_isASCII(sfp[0].s)) {
		base.len = (size_t)loc;
		loc = knh_bytes_mlen(base);
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */

static void regmatch_init(kregmatch_t *pmatch, int nmatch)
{
	int idx;
	for (idx = 0; idx < nmatch; idx++) {
		pmatch[idx].rm_so = -1;
		pmatch[idx].rm_eo = -1;
		pmatch[idx].rm_name.len = 0;
	}
}

static void LOG_regex(CTX, ksfp_t *sfp, int res, kRegex *re, const char *str)
{
	char ebuf[512] = {0};
	if (re->spi->regerror(res, re->reg, ebuf, 512) > 0) {
		KNH_NTRACE2(_ctx, "regex", K_FAILED, KNH_LDATA(LOG_s("driver", re->spi->name), LOG_s("pattern", S_text(re->pattern)), LOG_s("text", str), LOG_msg(ebuf)));
	}
}

static void CWB_write_regexfmt(CTX, CWB_t *cwb, kbytes_t *fmt, const char *base, kregmatch_t *r, size_t matched)
{
	const char *ch = fmt->text;
	const char *eof = ch + fmt->len; // end of fmt
	for (; ch < eof; ch++) {
		if (*ch == '\\') {
			kwb_putc(cwb->ba, *ch);
			ch++;
		} else if (*ch == '$' && isdigit(ch[1])) {
			size_t grpidx = (size_t)ch[1] - '0'; // get head of grourp_index
			if (grpidx < matched) {
				ch++;
				while (isdigit(ch[1])) {
					size_t nidx = grpidx * 10 + (ch[1] - '0');
					if (nidx < matched) {
						grpidx = nidx;
						ch++;
						if (ch < eof) {
							continue;
						}
					}
				}
				kregmatch_t *rp = &r[grpidx];
				knh_Bytes_write(_ctx, cwb->ba, new_bytes2(base + rp->rm_so, rp->rm_eo - rp->rm_so));
				continue; // skip putc
			}
		}
		kwb_putc(cwb->ba, *ch);
	}
}

static size_t knh_regex_matched(kregmatch_t* r, size_t maxmatch)
{
	size_t n = 0;
	for (; n < maxmatch && r[n].rm_so != -1; n++) ;
	return n;
}

#define _ALWAYS SPOL_POOL
#define _NEVER  SPOL_POOL
#define _ASCII  SPOL_ASCII
#define _UTF8   SPOL_UTF8
#define _SUB(s0) (String_isASCII(s0) ? _ASCII|_ALWAYS : _ALWAYS)
#define _SUBCHAR(s0) (String_isASCII(s0) ? _ASCII : 0)
#define _CHARSIZE(len) (len==1 ? _ASCII : _UTF8)


static kArray *kStringoCharArray(CTX, kString *bs, int istrim)
{
	kbytes_t base = S_tobytes(bs);
	size_t i, n = base.len;
	kArray *a = new_Array(_ctx, CLASS_String, n);
	if(String_isASCII(bs)) {
		for(i = 0; i < n; i++) {
			if(istrim && isspace(base.utext[i])) continue;
			kArray_add(a, new_kString(base.text+i, 1, _ALWAYS|_ASCII));
		}
	}
	else {
		n = knh_bytes_mlen(base);
		for(i = 0; i < n; i++) {
			if(istrim && isspace(base.utext[i])) continue;
			kbytes_t sub = knh_bytes_mofflen(base, i, 1);
			kArray_add(a, new_kString(sub.text, sub.len, _ALWAYS|((sub.len == 1) ? _ASCII:_UTF8)));
		}
	}
	return a;
}

/* ------------------------------------------------------------------------ */
//## @Const method Int String.search(Regex re);

static KMETHOD String_search(CTX, ksfp_t *sfp _RIX)
{
	kRegex *re = sfp[1].re;
	kindex_t loc = -1;
	if(!IS_NULL(re) && S_size(re->pattern) > 0) {
		kregmatch_t pmatch[2]; // modified by @utrhira
		const char *str = S_text(sfp[0].s);  // necessary
		int res = re->spi->regexec(_ctx, re->reg, str, 1, pmatch, re->eflags);
		if(res == 0) {
			loc = pmatch[0].rm_so;
			if (loc != -1 && !String_isASCII(sfp[0].s)) {
				kbytes_t base = {{str}, loc};
				loc = knh_bytes_mlen(base);
			}
		}
		else {
			LOG_regex(_ctx, sfp, res, re, str);
		}
	}
	RETURNi_(loc);
}

/* ------------------------------------------------------------------------ */
//## @Const method String[] String.match(Regex re);

static KMETHOD String_match(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	kRegex *re = sfp[1].re;
	kArray *a = NULL;
	if(IS_NOTNULL(re) && S_size(re->pattern) > 0) {
		const char *str = S_text(s0);  // necessary
		const char *base = str;
		const char *eos = base + S_size(s0);
		size_t nmatch = re->spi->regnmatchsize(_ctx, re->reg);
		kregmatch_t *p, pmatch[nmatch+1];
		int i, isGlobalOption = Regex_isGlobalOption(re);
		a = new_Array(_ctx, CLASS_String, nmatch);
		KSETv(sfp[2].o, a); WCTX(_ctx)->esp = sfp+3; //FIXME
		do {
			int res = re->spi->regexec(_ctx, re->reg, str, nmatch, pmatch, re->eflags);
			if(res != 0) {
				LOG_regex(_ctx, sfp, res, re, str);
				break;
			}
			for(p = pmatch, i = 0; i < nmatch; p++, i++) {
				if (p->rm_so == -1) break;
				//DBG_P("[%d], rm_so=%d, rm_eo=%d", i, p->rm_so, p->rm_eo);
				kbytes_t sub = {{str + (p->rm_so)}, ((p->rm_eo) - (p->rm_so))};
				kArray_add(a, new_kString(sub.text, sub.len, _SUB(s0)));
			}
			if(isGlobalOption) {
				size_t eo = pmatch[0].rm_eo; // shift matched pattern
				str += (eo > 0) ? eo : 1;
				if(!(str < eos)) isGlobalOption = 0; // stop iteration
			}
		} while(isGlobalOption);
	}
	else {
		a = new_Array(_ctx, CLASS_String, 0);
	}
	RETURN_(a);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.replace(Regex re, String s);

static KMETHOD String_replace(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	kRegex *re = sfp[1].re;
	kbytes_t fmt = S_tobytes(sfp[2].s);
	kString *s = s0;
	if(IS_NOTNULL(re) && S_size(re->pattern) > 0) {
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		const char *str = S_text(s0);  // necessary
		const char *base = str;
		const char *eos = str + S_size(s0); // end of str
		kregmatch_t pmatch[KREGEX_MATCHSIZE+1];
		while (str < eos) {
			int res = re->spi->regexec(_ctx, re->reg, str, KREGEX_MATCHSIZE, pmatch, re->eflags);
			if(res != 0) {
				LOG_regex(_ctx, sfp, res, re, str);
				break;
			}
			size_t len = pmatch[0].rm_eo;
			if (pmatch[0].rm_so > 0) {
				knh_Bytes_write(_ctx, cwb->ba, new_bytes2(str, pmatch[0].rm_so));
			}
			size_t matched = knh_regex_matched(pmatch, KREGEX_MATCHSIZE);
			if (len > 0) {
				CWB_write_regexfmt(_ctx, cwb, &fmt, base, pmatch, matched);
				str += len;
			} else {
				if (str == base) { // 0-length match at head of string
					CWB_write_regexfmt(_ctx, cwb, &fmt, base, pmatch, matched);
				}
				break;
			}
		}
		knh_Bytes_write(_ctx, cwb->ba, new_bytes((char*)str)); // write out remaining string
		s = CWB_newString(_ctx, cwb, 0); // close cwb
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method String[] String.split(Regex re);

static KMETHOD String_split(CTX, ksfp_t *sfp _RIX)
{
	kString *s0 = sfp[0].s;
	kRegex *re = sfp[1].re;
	kArray *a = NULL;
	if (IS_NOTNULL(re) && S_size(re->pattern) > 0) {
		const char *str = S_text(s0);  // necessary
		const char *eos = str + S_size(s0);
		kregmatch_t pmatch[KREGEX_MATCHSIZE+1];
		if (str < eos) {
			a = new_Array(_ctx, CLASS_String, 0);
			KSETv(sfp[2].o, a); WCTX(_ctx)->esp = sfp+3; //FIXME
			while (str <= eos) {
				int res = re->spi->regexec(_ctx, re->reg, str, KREGEX_MATCHSIZE, pmatch, re->eflags);
				if (res == 0) {
					size_t len = pmatch[0].rm_eo;
					if (len > 0) {
						kbytes_t sub = {{str},  pmatch[0].rm_so};
						kArray_add(a, new_kString(sub.text, sub.len, _SUB(s0)));
						str += len;
						continue;
					}
				}
				kArray_add(a, new_String(_ctx, str)); // append remaining string to array
				break;
			}
		} else { // for 0-length patterh
			a = kStringoCharArray(_ctx, new_String(_ctx, str), 0);
		}
	}
	else {
		a = kStringoCharArray(_ctx, s0, 0);
	}
	RETURN_(a);
}

/* ------------------------------------------------------------------------ */
//## @Const method Map String.extract(Regex re);

static KMETHOD String_extract(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[0].s;
	kRegex *re = sfp[1].re;
	kDictMap *m = new_DictMap0(_ctx, 0, 0/*isCase*/, "regex");
	KSETv(sfp[2].o, m); WCTX(_ctx)->esp = sfp+3; //FIXME
	if (IS_NOTNULL(re) && S_size(re->pattern) > 0) {
		size_t nmatch = re->spi->regnmatchsize(_ctx, re->reg);  //
		const char *str = s->str.text;
		kregmatch_t p[nmatch + 1];
		regmatch_init(p, sizeof(p)/sizeof(kregmatch_t));
		int res = re->spi->regexec(_ctx, re->reg, str, nmatch, p, re->eflags);
		if (res == 0) {
			int idx, matched = knh_regex_matched(p, nmatch);
			for (idx = 0; idx < matched && p[idx].rm_so != -1; idx++) {
				kregmatch_t *rp = &p[idx];
				if (rp->rm_name.len > 0) {
					kString *s_name = new_kString(rp->rm_name.text, rp->rm_name.len, _SUB(s));
					kString *s_value = new_kString(&str[rp->rm_so], (rp->rm_eo - rp->rm_so), _SUB(s));
					knh_DictMap_set(_ctx, m, s_name, s_value);
				}
			}
		}
		else {
			LOG_regex(_ctx, sfp, res, re, str);
		}
	}
	RETURN_(m);
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Static @Const method Converter Converter.opLINK(String path, Lingo _);

static KMETHOD Converter_opLINK(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = S_tobytes(sfp[1].s);
	const knh_ConverterDPI_t *dpi = knh_Lingo_getConverterDPINULL(_ctx, sfp[2].ns, t);
	if(dpi != NULL && dpi->conv != NULL) {
		kConverter *c = new_(Converter);
		c->dpi  = dpi;
		c->conv = NULL;
		RETURN_(c);
	}
	RETURN_(KNH_TNULL(Converter));
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Static @Const method StringEncoder StringEncoder.opLINK(String path, Lingo _);

static KMETHOD StringEncoder_opLINK(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = S_tobytes(sfp[1].s);
	const knh_ConverterDPI_t *dpi = knh_Lingo_getConverterDPINULL(_ctx, sfp[2].ns, t);
	if(dpi != NULL && dpi->enc != NULL) {
		kStringEncoder *c = new_(StringEncoder);
		c->dpi  = dpi;
		c->conv = NULL;
		RETURN_(c);
	}
	RETURN_(KNH_TNULL(StringEncoder));
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Static @Const method StringDecoder StringDecoder.opLINK(String path, Lingo _);

static KMETHOD StringDecoder_opLINK(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = S_tobytes(sfp[1].s);
	const knh_ConverterDPI_t *dpi = knh_Lingo_getConverterDPINULL(_ctx, sfp[2].ns, t);
	if(dpi != NULL && dpi->dec != NULL) {
		kStringDecoder *c = new_(StringDecoder);
		c->dpi  = dpi;
		c->conv = NULL;
		RETURN_(c);
	}
	RETURN_(KNH_TNULL(StringDecoder));
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Static @Const method StringConverter StringConverter.opLINK(String path, Lingo _);

static KMETHOD StringConverter_opLINK(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = S_tobytes(sfp[1].s);
	const knh_ConverterDPI_t *dpi = knh_Lingo_getConverterDPINULL(_ctx, sfp[2].ns, t);
	if(dpi != NULL && dpi->sconv != NULL) {
		kStringConverter *c = new_(StringConverter);
		c->dpi  = dpi;
		c->conv = NULL;
		RETURN_(c);
	}
	RETURN_(KNH_TNULL(StringConverter));
}

/* ------------------------------------------------------------------------ */
//## @Const method Bytes Bytes.(Converter enc);

static KMETHOD Bytes_convert(CTX, ksfp_t *sfp _RIX)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	kConverter *c = sfp[1].conv;
	kbytes_t t = BA_tobytes(sfp[0].ba);
	c->dpi->conv(_ctx, c->conv, t.text, t.len, cwb->ba);
	kBytes *ba = new_Bytes(_ctx, NULL, CWB_size(cwb));
	knh_Bytes_write(_ctx, ba, CWB_tobytes(cwb));
	CWB_close(_ctx, cwb);
	RETURN_(ba);
}

/* ------------------------------------------------------------------------ */
//## @Const method Bytes String.encode(StringEncoder enc);

static KMETHOD String_encode(CTX, ksfp_t *sfp _RIX)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	kConverter *c = sfp[1].conv;
	//fprintf(stderr, "%s, c=%p, c->dpi=%p, c->dpi->enc=****\n", CLASS__(c->h.cid), c, c->dpi);
	c->dpi->enc(_ctx, c->conv, S_text(sfp[0].s), S_size(sfp[0].s), cwb->ba);
	kBytes *ba = new_Bytes(_ctx, NULL, CWB_size(cwb));
	knh_Bytes_write(_ctx, ba, CWB_tobytes(cwb));
	CWB_close(_ctx, cwb);
	RETURN_(ba);
}

/* ------------------------------------------------------------------------ */
//## @Const method String Bytes.decode(StringDecoder c);

static KMETHOD Bytes_toString(CTX, ksfp_t *sfp _RIX)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	kConverter *c = sfp[1].conv;
	c->dpi->dec(_ctx, c->conv, BA_totext(sfp[0].ba), BA_size(sfp[0].ba), cwb->ba);
	RETURN_(CWB_newString(_ctx, cwb, 0));
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.(StringConverter c);
//## @Const method String String.convert(StringConverter c);

static KMETHOD String_convert(CTX, ksfp_t *sfp _RIX)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	kConverter *c = sfp[1].conv;
	if (!IS_NULL(c)) {
		c->dpi->sconv(_ctx, c->conv, S_text(sfp[0].s), S_size(sfp[0].s), cwb->ba);
	}
	RETURN_(CWB_newString(_ctx, cwb, 0));
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.trim();

static KMETHOD String_trim(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[0].s;
	kbytes_t t = S_tobytes(s);
	kbytes_t t2 = bytes_trim(t);
	if(t.len > t2.len) {
		s = new_kString(t2.text, t2.len, _SUB(s));
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Regex.opHAS(String s);

static KMETHOD Regex_opHAS(CTX, ksfp_t *sfp _RIX)
{
	kRegex *re = sfp[0].re;
	kregmatch_t pmatch[2]; // modified by @utrhira
	const char *str = S_text(sfp[1].s);
	int res = re->spi->regexec(_ctx, re->reg, str, 1, pmatch, re->eflags);
	RETURNb_(res == 0);
}

/* ------------------------------------------------------------------------ */
//## @Const mapper String Int;

static TYPEMAP String_Int(CTX, ksfp_t *sfp _RIX)
{
	kint_t v = 0;
	if(!knh_bytes_parseint(S_tobytes(sfp[K_TMRIDX].s), &v)) {
		KSETv(sfp[K_RIX].o, KNH_NULVAL(CLASS_Int));
	}
	RETURNi_(v);
}

/* ------------------------------------------------------------------------ */
//## @Const mapper String Float;

static TYPEMAP String_Float(CTX, ksfp_t *sfp _RIX)
{
	kfloat_t v = 0.0;
	if(!knh_bytes_parsefloat(S_tobytes(sfp[K_TMRIDX].s), &v)) {
		KSETv(sfp[K_RIX].o, KNH_NULVAL(CLASS_Float));
	}
	RETURNf_(v);
}

/* ------------------------------------------------------------------------ */
//## @Const mapper String Bytes;

static TYPEMAP String_Bytes(CTX, ksfp_t *sfp _RIX)
{
	kBytes *b = new_Bytes(_ctx, "UTF8", S_size(sfp[K_TMRIDX].s) + 1);
	knh_Bytes_write(_ctx, b, S_tobytes(sfp[K_TMRIDX].s));
	RETURN_(b);
}

/* ------------------------------------------------------------------------ */

static ITRNEXT String_nextChar(CTX, ksfp_t *sfp _RIX)
{
	kIterator *itr = ITR(sfp);
	kString *s = (kString*)DP(itr)->source;
	kbytes_t base = S_tobytes(s);
	size_t pos = (size_t)DP(itr)->m.index;
	if(pos < knh_bytes_mlen(base)) {
		kbytes_t sub = knh_bytes_mofflen(base, pos, 1);
		s = new_kString(sub.text, sub.len, _SUB(s));
		DP(itr)->m.index = pos + 1;
		ITRNEXT_(s);
	}
	ITREND_();
}

/* ------------------------------------------------------------------------ */
//## method String.. String.opITR();

static TYPEMAP String_Iterator(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_IteratorG(_ctx, CLASS_StringITR, sfp[0].o, String_nextChar));
}

/* ------------------------------------------------------------------------ */
//## mapper Bytes InputStream;

static TYPEMAP Bytes_InputStream(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_BytesInputStream(_ctx, sfp[0].ba->bu.text, sfp[0].ba->bu.len));
}

/* ------------------------------------------------------------------------ */
//## @Const @Semantic mapper String Regex!;

static KMETHOD String_Regex(CTX, ksfp_t *sfp _RIX)
{
	kRegex *re = new_(Regex);
	const char *ptn = S_text(sfp[0].s);
	KSETv(re->pattern, sfp[0].s);
	re->reg = re->spi->regmalloc(_ctx, sfp[0].s);
	re->spi->regcomp(_ctx, re->reg, ptn, 0);
	re->eflags = 0;
	RETURN_(re);
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Const method This Tuple.new:TUPLE(dynamic value, ...);

static KMETHOD Tuple_newTUPLE(CTX, ksfp_t *sfp _RIX)
{
	kTuple *tpl = (kTuple*)sfp[0].o;
	ksfp_t *v = sfp + 1;
	size_t i, ti = 0, ac = knh_stack_argc(_ctx, v);
	kfieldinfo_t *tf = O_ct(tpl)->fields;
	for(i = 0; i < ac; i++) {
		if(tf[ti].israw == 1) {
			kunbox_t *n = (kunbox_t*)(tpl->fields + ti);
			n[0] = v[i].ndata;
			DBG_P("i=%d, ti=%d, n=%d", i, ti, n[0]);
			ti++;
			DBLNDATA_(ti++);
		}
		else {
			KSETv(tpl->fields[ti], v[i].o);
			ti++;
		}
	}
	RETURN_(tpl);
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Const method This Range.new:LIST(T1 start, T1 end);

static KMETHOD Range_newLIST(CTX, ksfp_t *sfp _RIX)
{
	kRange *rng = (kRange*)sfp[0].o;
	if(Range_isNDATA(rng)) {
		rng->nstart = sfp[1].ndata;
		rng->nend =   sfp[2].ndata;
	}
	else {
		KSETv(rng->ostart, sfp[1].o);
		KSETv(rng->oend, sfp[2].o);
	}
	RETURN_(rng);
}

/* ------------------------------------------------------------------------ */
/* [Array] */

//## method This Array.new(Int initCapacity);

static KMETHOD Array_new(CTX, ksfp_t *sfp _RIX)
{
	kArray_grow(_ctx, sfp[0].a, Int_to(size_t, sfp[1]), 0);
	DBG_ASSERT((sfp[0].a)->size == 0);
	RETURN_(sfp[0].a);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method This Array.new:ARRAY(Int size, ...);

static KMETHOD Array_newARRAY(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	kint_t init, x = 0, y = 1, z = 1, w = 1;
	size_t dim = knh_stack_argc(_ctx, sfp+1);
	switch(dim) {
		case 4: w = sfp[4].ivalue;
		case 3: z = sfp[3].ivalue;
		case 2: y = sfp[2].ivalue;
		case 1: x = sfp[1].ivalue;
	}
	init = x * y * z * w;
	DBG_P("x=%d,y=%d,z=%d,w=%d,dim=%d,init=%d",x,y,z,w,dim, init);
	if(0 <= init && init < LONG_MAX) {
		kArray_grow(_ctx, a, (size_t)init, 0);
		a->size = (size_t)init;
		if(!kArray_isUnboxData(a)) {
			kcid_t p1 = O_p1(a);
			Object *v = knh_getClassDefaultValue(_ctx, p1);
			size_t i;
			for(i = 0; i < a->size; i++) {
				KINITv(a->list[i], v);
			}
		}
	}
	else if(unlikely(init < 0)) {
		THROW_OutOfRange(_ctx, sfp, init, 0);
	}
	if(init > 0 && dim > 1) {
		((kdim_t*)a->dim)->dim = dim;
		((kdim_t*)a->dim)->x = (size_t)x;
		((kdim_t*)a->dim)->xy = (size_t)(x * y);
		((kdim_t*)a->dim)->xyz = (size_t)(x * y * z);
	}
	RETURN_(a);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method This Array.new:LIST(T1 value, ...);

static KMETHOD Array_newLIST(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	a->api->multiadd(_ctx, a, sfp+1);
	RETURN_(a);
}

/* ------------------------------------------------------------------------ */

static ITRNEXT Iterator_next(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(IS_bIterator(sfp[0].it));
	kIterator *itr = ITR(sfp);
	ksfp_t *lsfp = ctx->esp;
	DBG_ASSERT(sfp < lsfp);
	long rtnidx_ = 0, thisidx = rtnidx_ + K_CALLDELTA;
	KSETv(lsfp[thisidx].o, DP(itr)->source);
	KNH_SCALL(_ctx, lsfp, rtnidx_, DP(itr)->mtdNULL, 0);
	if(IS_NULL(lsfp[rtnidx_].o)) {
		ITREND_();
	}
	else {
		ITRNEXT_(lsfp[rtnidx_].o);
	}
}

/* ------------------------------------------------------------------------ */
//## @Hidden method This Iterator.new(Object value, Method mtd);

static KMETHOD Iterator_new(CTX, ksfp_t *sfp _RIX)
{
	kIterator *it = sfp[0].it;
	KSETv(DP(it)->source, sfp[1].o);
	KINITv(DP(it)->mtdNULL, sfp[2].mtd);
	it->fnext_1 = Iterator_next;
	RETURN_(it);
}


/* ------------------------------------------------------------------------ */
//## @Immutable method T1 Array.get(Int n);

static KMETHOD Array_get(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	size_t n2 = a->api->index(_ctx, sfp, Int_to(kint_t, ctx->esp[-1]), a->size);
	a->api->fastget(_ctx, sfp, n2, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Immutable @Hidden method T1 Array.get2(Int x, Int y);

static KMETHOD Array_get2(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	const kdim_t *dim = a->dim;
	kint_t n = sfp[1].ivalue + (sfp[2].ivalue * dim->x);
	size_t n2 = a->api->index(_ctx, sfp, n, a->size);
	a->api->fastget(_ctx, sfp, n2, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Immutable @Hidden method T1 Array.get3(Int x, Int y, Int z);

static KMETHOD Array_get3(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	const kdim_t *dim = a->dim;
	kint_t n = sfp[1].ivalue + (sfp[2].ivalue * dim->x) + (sfp[3].ivalue * dim->xy);
	size_t n2 = a->api->index(_ctx, sfp, n, a->size);
	a->api->fastget(_ctx, sfp, n2, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Immutable @Hidden method T1 Array.get4(Int x, Int y, Int z, Int w);

static KMETHOD Array_get4(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	const kdim_t *dim = a->dim;
	kint_t n = sfp[1].ivalue + (sfp[2].ivalue * dim->x) + (sfp[3].ivalue * dim->xy) + (sfp[4].ivalue * dim->xyz);
	size_t n2 = a->api->index(_ctx, sfp, n, a->size);
	a->api->fastget(_ctx, sfp, n2, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## method T1 Array.set(Int n, T1 v);

static KMETHOD Array_set(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	kint_t n = sfp[1].ivalue;
	size_t n2 = a->api->index(_ctx, sfp, n, a->size);
	a->api->set(_ctx, sfp[0].a, n2, sfp+2);
	a->api->fastget(_ctx, sfp, n2, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method T1 Array.set2(Int x, Int y, T1 v);

static KMETHOD Array_set2(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	const kdim_t *dim = (sfp[0].a)->dim;
	kint_t n = sfp[1].ivalue + (sfp[2].ivalue * dim->x);
	size_t n2 = a->api->index(_ctx, sfp, n, a->size);
	a->api->set(_ctx, a, n2, sfp+3);
	a->api->fastget(_ctx, sfp, n2, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method T1 Array.set3(Int x, Int y, Int z, T1 v);

static KMETHOD Array_set3(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	const kdim_t *dim = (sfp[0].a)->dim;
	kint_t n = sfp[1].ivalue + (sfp[2].ivalue * dim->x) + (sfp[3].ivalue * dim->xy);
	size_t n2 = a->api->index(_ctx, sfp, n, a->size);
	a->api->set(_ctx, a, n2, sfp+4);
	a->api->fastget(_ctx, sfp, n2, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method T1 Array.set4(Int x, Int y, Int z, Int, w, T1 v);

static KMETHOD Array_set4(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	const kdim_t *dim = (sfp[0].a)->dim;
	kint_t n = sfp[1].ivalue + (sfp[2].ivalue * dim->x) + (sfp[3].ivalue * dim->xy) + (sfp[4].ivalue * dim->xyz);
	size_t n2 = a->api->index(_ctx, sfp, n, a->size);
	a->api->set(_ctx, a, n2, sfp+5);
	a->api->fastget(_ctx, sfp, n2, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## method T1 Array.setAll(T1 v);

static KMETHOD Array_setAll(CTX, ksfp_t *sfp _RIX)
{
	size_t i;
	for(i = 0; i < (sfp[0].a)->size; i++) {
		(sfp[0].a)->api->set(_ctx, sfp[0].a, i, sfp+1);
	}
	if((sfp[0].a)->size > 0) {
		(sfp[0].a)->api->fastget(_ctx, sfp, 0, K_RIX);
	}
	else {
		sfp[K_RIX].ndata = sfp[1].ndata;
		RETURN_(sfp[1].o);
	}
}

/* ------------------------------------------------------------------------ */
//## method void Array.add(T1 value, ...);
//## method void Array.send(T1 value, ...);

static KMETHOD Array_add(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	a->api->multiadd(_ctx, a, sfp+1);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void Array.insert(Int n, T1 value);

static KMETHOD Array_insert(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	size_t n = a->api->index(_ctx, sfp, Int_to(kint_t, sfp[1]), a->size);
	const kdim_t *dim = a->dim;
//	BEGIN_LOCAL(_ctx, lsfp, 1);
	if(a->size == dim->capacity) {
		kArray_grow(_ctx, a, k_grow(dim->capacity), a->size + 1);
	}
	if(kArray_isUnboxData(a)) {
		knh_memmove(a->nlist+(n+1), a->nlist+n, sizeof(kunbox_t) * (a->size - n));
	}else {
		knh_memmove(a->list+(n+1), a->list+n, sizeof(kObject*) * (a->size - n));
		KINITv(a->list[n], K_NULL); // for RCGC
	}
	a->size++;
	a->api->set(_ctx, a, n, sfp+2);
//	END_LOCAL(_ctx, lsfp);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @FastCall method void Array.clear();

static KMETHOD Array_clear(CTX, ksfp_t *sfp _RIX)
{
	kArray_clear(sfp[0].a, 0);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void Array.remove(Int n);

static KMETHOD Array_remove(CTX, ksfp_t *sfp _RIX)
{
	kArray *o = (kArray*)sfp[0].o;
	size_t n = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), o->size);
	kArray_remove_(_ctx, o, n);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @FastCall method T1! Array.pop();

static KMETHOD Array_pop(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	if (a->size > 0) {
		(a)->api->fastget(_ctx, sfp, a->size - 1, K_RIX);
		kArray_clear(a, a->size - 1);
	} else {
		kcid_t p1 = O_p1(a);
		kObject *o = KNH_NULVAL(p1);
		if (TY_isUnbox(p1)) {
			RETURNd_(O_data(o));
		} else {
			RETURN_(o);
		}
	}
}
/* ------------------------------------------------------------------------ */
//## method Int Array.indexOf(T1 value);

static KMETHOD Array_indexOf(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	kint_t res = -1;
	size_t i;
	if(kArray_isUnboxData(a)) {
		kunbox_t ndata = sfp[1].ndata;
		for(i = 0; i < a->size; i++) {
			if(a->nlist[i] == ndata) {
				res = i; break;
			}
		}
	}else {
		kObject *o = sfp[1].o;
		for(i = 0; i < a->size; i++) {
			if(knh_Object_compareTo(a->list[i], o) == 0) {
				res = i; break;
			}
		}
	}
	RETURNi_(res);
}
/* ------------------------------------------------------------------------ */
//## method Int Array.lastIndexOf(T1 value);

static KMETHOD Array_lastIndexOf(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	long i;
	if(kArray_isUnboxData(a)) {
		kunbox_t ndata = sfp[1].ndata;
		for(i = a->size - 1; i >= 0; i--) {
			if(a->nlist[i] == ndata) {
				break;
			}
		}
	}else {
		kObject *o = sfp[1].o;
		for(i = a->size - 1; i >= 0; i--) {
			if(knh_Object_compareTo(a->list[i], o) == 0) {
				break;
			}
		}
	}
	RETURNi_(i);
}

/* ------------------------------------------------------------------------ */

static int qsort_icmp(const void* ap, const void* bp)
{
	kint_t a = *((kint_t*)ap);
	kint_t b = *((kint_t*)bp);
	if(a < b) return -1;
	return (a > b);
}

static int qsort_fcmp(const void* ap, const void* bp)
{
	kfloat_t a = *((kfloat_t*)ap);
	kfloat_t b = *((kfloat_t*)bp);
	if(a < b) return -1;
	return (a > b);
}

static int qsort_ocmp(const void *ap, const void* bp)
{
	kRawPtr* o1 = *((kRawPtr**)ap);
	kRawPtr* o2 = *((kRawPtr**)bp);
	kcid_t bcid1 = O_bcid(o1);
	kcid_t bcid2 = O_bcid(o2);
	int res;
	if(bcid1 == bcid2) {
		res = O_ct(o1)->cdef->compareTo(o1, o2);
	}
	else {
		res = (int)(o1 - o2);
	}
	return res;
}

// added by @shinpei_NKT
static int knh_compare_i(kFunc *fo, const void *v1, const void *v2)
{
	kint_t a = *((kint_t*)v1);
	kint_t b = *((kint_t*)v2);
	CLOSURE_start(2);
	CLOSURE_putArg(1, Int, a);
	CLOSURE_putArg(2, Int, b);
	CLOSURE_call(fo);
	int ret = CLOSURE_getReturn(Int);
	CLOSURE_end(return ret);
}

static int dummyCallbackCompareInt(const void *v1, const void *v2)
{
  return knh_compare_i((kFunc*)CALLBACK_MARKER, v1, v2);
}

static int knh_compare_f(kFunc *fo, const void *v1, const void *v2)
{
	kfloat_t a = *((kfloat_t*)v1);
	kfloat_t b = *((kfloat_t*)v2);
	CLOSURE_start(2);
	CLOSURE_putArg(1, Float, a);
	CLOSURE_putArg(2, Float, b);
	CLOSURE_call(fo);
	int ret = CLOSURE_getReturn(Int);
	CLOSURE_end(return ret);
}

static int dummyCallbackCompareFloat(const void *v1, const void *v2)
{
	return knh_compare_f((kFunc*)CALLBACK_MARKER, v1, v2);
}

static int knh_compare_o(kFunc *fo, const void *v1, const void *v2)
{
	kObject *a = *((kObject**)v1);
	kObject *b = *((kObject**)v2);
	CLOSURE_start(2);
	lsfp[1].o = a;
	lsfp[2].o = b;
	CLOSURE_call(fo);
	int ret = CLOSURE_getReturn(Int);
	CLOSURE_end(return ret);
}

static int dummyCallbackCompareObject(const void *v1, const void *v2)
{
	return knh_compare_o((kFunc*)CALLBACK_MARKER, v1, v2);
}

typedef int (*fcompare)(const void *, const void *);
typedef int (*fcompare2)(kFunc *, const void *, const void *);
struct asortf {
	size_t size; /* sizeof(T) */
	fcompare  fcmp1;
	fcompare  fdummy;
	fcompare2 fcmp2;
	kMap *map; /* TODO Map<Func, fcmp2(generated)> */
};
struct asortf asorts[] = {
	{sizeof(kbool_t), qsort_icmp, dummyCallbackCompareInt   , knh_compare_i, NULL},
	{sizeof(kint_t)    , qsort_icmp, dummyCallbackCompareInt   , knh_compare_i, NULL},
	{sizeof(kfloat_t)  , qsort_fcmp, dummyCallbackCompareFloat , knh_compare_f, NULL},
	{sizeof(kObject*), qsort_ocmp, dummyCallbackCompareObject, knh_compare_o, NULL},
};

static struct asortf *getasortf(CTX, kcid_t cid)
{
	switch (cid) {
		case CLASS_Boolean: return &asorts[0];
		case CLASS_Int:     return &asorts[1];
		case CLASS_Float:   return &asorts[2];
		default:            return &asorts[3];
	}
}

/* ------------------------------------------------------------------------ */
//## method void Array.sort(CmprT1 cmr);

static KMETHOD Array_sort(CTX, ksfp_t *sfp _RIX)
{
	kArray *a  = sfp[0].a;
	kFunc  *fo = sfp[1].fo;
	struct asortf *sortf = getasortf(_ctx, O_p1(a));
	fcompare cmp;
	if(IS_NULL(fo)) {
		cmp = sortf->fcmp1;
	}
	else {
		cmp = (fcompare) knh_copyCallbackFunc(_ctx, (void*)sortf->fdummy, (void*)sortf->fcmp2, fo);
	}
	knh_qsort(a->list, a->size, sortf->size, cmp);
}

/* ------------------------------------------------------------------------ */

static inline void NArray_swap(CTX, kArray *a, size_t n, size_t m)
{
	kunbox_t temp = a->nlist[n];
	a->nlist[n] = a->nlist[m];
	a->nlist[m] = temp;
}

static inline void OArray_swap(CTX, kArray *a, size_t n, size_t m)
{
	ksfp_t *esp1 = ctx->esp + 1;
	esp1[0].o = a->list[n];
	a->list[n] = a->list[m];
	a->list[m] = esp1[0].o;
}

/* ------------------------------------------------------------------------ */
//## method void Array.swap(Int m, Int n);

static KMETHOD Array_swap(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	size_t m = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), a->size);
	size_t n = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), a->size);
	if(kArray_isUnboxData(a)) {
		NArray_swap(_ctx, a, n, m);
	}
	else {
		OArray_swap(_ctx, a, n, m);
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void Array.shuffle();

static KMETHOD Array_shuffle(CTX, ksfp_t *sfp _RIX)
{
	size_t i;
	kArray *a = sfp[0].a;
	if(kArray_isUnboxData(a)) {
		for(i = 0; i < a->size * 2; i++) {
			size_t m = knh_rand() % a->size;
			size_t n = knh_rand() % a->size;
			NArray_swap(_ctx, a, m, n);
		}
	}
	else {
		for(i = 0; i < a->size * 2; i++) {
			size_t m = knh_rand() % a->size;
			size_t n = knh_rand() % a->size;
			OArray_swap(_ctx, a, m, n);
		}
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void Array.reverse();

static KMETHOD Array_reverse(CTX, ksfp_t *sfp _RIX)
{
	size_t i;
	kArray *a = sfp[0].a;
	if(kArray_isUnboxData(a)) {
		for(i = 0; i < a->size / 2; i++) {
			size_t n = a->size - i - 1;
			NArray_swap(_ctx, a, i, n);
		}
	}
	else {
		for(i = 0; i < a->size / 2; i++) {
			size_t n = a->size - i - 1;
			OArray_swap(_ctx, a, i, n);
		}
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */

static ITRNEXT ITR_where(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(IS_bIterator(sfp[0].it));
	kIterator *itr = ITR(sfp);
	ksfp_t *lsfp = ctx->esp;
	DBG_ASSERT(sfp < lsfp);
	long rtnidx_ = 0, thisidx = rtnidx_ + K_CALLDELTA;
	kIterator *itrIN = (kIterator*)DP(itr)->source;
	KSETv(lsfp[thisidx+2].o, itrIN);
	klr_setesp(_ctx, lsfp + thisidx+2);
	while(itrIN->fnext_1(_ctx, lsfp+thisidx+2, -1)) {
		kFunc *fo = DP(itr)->funcNULL;
		KNH_FINVOKE(_ctx, lsfp, rtnidx_, fo, 1);
		if(lsfp[rtnidx_].bvalue) {
			sfp[K_RIX].ndata = lsfp[thisidx+1].ndata;
			ITRNEXT_(lsfp[thisidx+1].o);
		}
		KSETv(lsfp[thisidx+2].o, itrIN);
		klr_setesp(_ctx, lsfp + thisidx+2);
	}
	ITREND_();
}

static ITRNEXT ITR_each(CTX, ksfp_t *sfp _RIX)
{
	DBG_ASSERT(IS_bIterator(sfp[0].it));
	kIterator *itr = ITR(sfp);
	ksfp_t *lsfp = ctx->esp;
	DBG_ASSERT(sfp < lsfp);
	long rtnidx_ = 0, thisidx = rtnidx_ + K_CALLDELTA;
	kIterator *itrIN = (kIterator*)DP(itr)->source;
	KSETv(lsfp[thisidx+2].o, itrIN);
	klr_setesp(_ctx, lsfp + thisidx+2);
	if(!itrIN->fnext_1(_ctx, lsfp+thisidx+2, -1)) {
		ITREND_();
	}
	kFunc *fo = DP(itr)->funcNULL;
	KNH_FINVOKE(_ctx, lsfp, rtnidx_, fo, 1);
	sfp[K_RIX].ndata = lsfp[rtnidx_].ndata;
	ITRNEXT_(lsfp[rtnidx_].o);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method This Iterator.opWHERE(FuncWhere f);

static KMETHOD Iterator_opWHERE(CTX, ksfp_t *sfp _RIX)
{
	kIterator *it = (kIterator*)new_Object_init2(_ctx, O_ct(sfp[0].it));
	KSETv(DP(it)->source, sfp[1].o);
	KINITv(DP(it)->funcNULL, sfp[1].o);
	it->fnext_1 = ITR_where;
	RETURN_(it);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method This Iterator.opEACH(FuncEach f);

static KMETHOD Iterator_opEACH(CTX, ksfp_t *sfp _RIX)
{
	kIterator *it = (kIterator*)new_Object_init2(_ctx, O_ct(sfp[0].it));
	KSETv(DP(it)->source, sfp[1].o);
	KINITv(DP(it)->funcNULL, sfp[1].o);
	it->fnext_1 = ITR_each;
	RETURN_(it);
}


/* ------------------------------------------------------------------------ */
//## method This Map.new(Int init);

static KMETHOD Map_new(CTX, ksfp_t *sfp _RIX)
{
	kMap *m = (kMap *)sfp[0].o;
	size_t init = sfp[1].ivalue <= 0 ? 0: Int_to(size_t, sfp[1]);
	m->spi = knh_getDefaultMapDSPI(_ctx, O_p1(m), O_p2(m));
	m->mapptr = m->spi->init(_ctx, init, NULL, NULL);
	RETURN_(m);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method This Map.new:MAP(T1 value, ...);

static KMETHOD Map_newMAP(CTX, ksfp_t *sfp _RIX)
{
	kMap *m = sfp[0].m;
	ksfp_t *v = sfp + 1;
	size_t i, ac = knh_stack_argc(_ctx, v);
	m->spi = knh_getDictMapDSPI(_ctx, O_p1(m), O_p2(m));
	KNH_ASSERT(m->spi != NULL); // if NULL, it is unsupported
	m->mapptr  = m->spi->init(_ctx, 0, NULL, NULL);
	for(i = 0; i < ac; i+=2) {
		ksfp_t *kvsfp = v + i;
		m->spi->set(_ctx, m->mapptr, kvsfp);
		knh_writeBarrier(m, kvsfp[1].o);
	}
	RETURN_(m);
}

/* ------------------------------------------------------------------------ */
//## @Static @Throwable @Smart method Map Map.open(String path, Map _, Lingo _, Class _);

static KMETHOD Map_open(CTX, ksfp_t *sfp _RIX)
{
	const kclass_t *ct = (sfp[4].c)->ct;
	const knh_MapDPI_t *spi = knh_Lingo_getMapDPINULL(_ctx, sfp[3].ns, S_tobytes(sfp[1].s));
	kMap *m = NULL;
	DBG_P("reqt = %s", CLASS__(ct->cid));
	if(ct->bcid != CLASS_Map) ct = ClassTBL(CLASS_Map);
	if(spi == NULL) {
		LANG_LOG("unknown Map driver: %s", S_text(sfp[1].s));
		goto L_RETURN;
	}
	spi = spi->config(_ctx, ct->p1, ct->p2);
	if(spi == NULL) {
		LANG_LOG("Map driver %s does not support for %s", S_tobytes(sfp[1].s), T__(ct->cid));
		goto L_RETURN;
	}
	kDictMap *opt = knh_toDictMap(_ctx, sfp[2].m, 1/*isCreation*/);
	KSETv(sfp[2].o, opt);
	kmapptr_t *mapptr = spi->init(_ctx, 0, S_text(sfp[1].s), opt);
	if(mapptr != NULL) {
		m = (kMap*)new_Object_init2(_ctx, ct);
		m->mapptr = mapptr;
		m->spi = spi;
	}
	L_RETURN: ;
	if(m == NULL) {
		m = (kMap*)KNH_NULVAL(ct->cid);
	}
	RETURN_(m);
}


/* ------------------------------------------------------------------------ */
//## method Exception! Exception.new(String event, String msg);

static KMETHOD Exception_new(CTX, ksfp_t *sfp _RIX)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	knh_write(_ctx, cwb->w, S_tobytes(sfp[1].s));
	if(IS_NULL(sfp[2].s)) {
		knh_write_ascii(_ctx, cwb->w, "!!");
	}
	else {
		knh_write_ascii(_ctx, cwb->w, "!!: ");
		knh_write(_ctx, cwb->w, S_tobytes(sfp[2].s));
	}
	KSETv((sfp[0].e)->emsg, CWB_newString(_ctx, cwb, SPOL_POOLNEVER));
	RETURN_(sfp[0].e);
}

/* ------------------------------------------------------------------------ */
//## method This Func.new(dynamic base, Method method);

static KMETHOD Func_new(CTX, ksfp_t *sfp _RIX)
{
	kFunc   *fo = sfp[0].fo;
	kMethod *mtd = sfp[2].mtd;
	KSETv((fo)->mtd, sfp[2].mtd);
	if(!Method_isStatic(mtd)) {
		KINITv((fo)->baseNULL, sfp[1].o);
	}
	//(sfp[0].cc)->envsfp = NULL;
	RETURN_(fo);
}

/* ------------------------------------------------------------------------ */
//## @Const method Assurance Assurance.new(String msg);

static KMETHOD Assurance_new(CTX, ksfp_t *sfp _RIX)
{
	kAssurance *Assurance = sfp[0].as;
	KSETv(Assurance->msg, sfp[1].s);
	RETURN_(sfp[0].o);
}

/* ------------------------------------------------------------------------ */
/* [Operator] */

/* ------------------------------------------------------------------------ */
//## @Const method Int Object.opADDR();

static KMETHOD Object_opADDR(CTX, ksfp_t *sfp _RIX)
{
	void *p = NULL;
	if(IS_Boolean(sfp[0].o)) {
		p = (sfp[0].bvalue) ? (void*)(K_TRUE) : (void*)(K_FALSE);
	}
	else {
		p = (void*)sfp[0].o;
	}
	RETURNi_((uintptr_t)p);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Object.opOF(Class c);

static KMETHOD Object_opOF(CTX, ksfp_t *sfp _RIX)
{
	ktype_t reqt = Class_tocid(sfp[1]);
	DBG_P("cid=%s", O__(sfp[0].o));
	RETURNb_(ClassTBL_isa(O_ct(sfp[0].o), reqt));
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Object.opEQ(dynamic value);

static KMETHOD Object_opEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_Object_compareTo(sfp[0].o, sfp[1].o) == 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Object.opNOTEQ(dynamic value);

static KMETHOD Object_opNOTEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_Object_compareTo(sfp[0].o, sfp[1].o) != 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Object.opLT(dynamic value);

static KMETHOD Object_opLT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_Object_compareTo(sfp[0].o, sfp[1].o) < 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Object.opLTE(dynamic value);

static KMETHOD Object_opLTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_Object_compareTo(sfp[0].o, sfp[1].o) <= 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Object.opGT(dynamic value);

static KMETHOD Object_opGT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_Object_compareTo(sfp[0].o, sfp[1].o) > 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Object.opGTE(dynamic value);

static KMETHOD Object_opGTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(knh_Object_compareTo(sfp[0].o, sfp[1].o) >= 0);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Int.opEQ(Int value);

static KMETHOD Int_opEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue == sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const  method Boolean Int.opNOTEQ(Int value);

static KMETHOD Int_opNOTEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue != sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Int.opLT(Int value);

static KMETHOD Int_opLT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue < sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Int.opLTE(Int value);

static KMETHOD Int_opLTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue <= sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Int.opGT(Int value);

static KMETHOD Int_opGT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue > sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Int.opGTE(Int value);

static KMETHOD Int_opGTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].ivalue >= sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Float.opEQ(Float value);

static KMETHOD Float_opEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue == sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Float.opNOTEQ(Float value);

static KMETHOD Float_opNOTEQ(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue != sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Float.opLT(Float! value);

static KMETHOD Float_opLT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue < sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Float.opLTE(Float! value);

static KMETHOD Float_opLTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue <= sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Float.opGT(Float! value);

static KMETHOD Float_opGT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue > sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Float.opGTE(Float! value);

static KMETHOD Float_opGTE(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(sfp[0].fvalue >= sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
/* [opHAS] */

/* ------------------------------------------------------------------------ */
//## @Const method Boolean String.opHAS(String s);

static KMETHOD String_opHAS(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(strstr(S_text(sfp[0].s), S_text(sfp[1].s)) != NULL);
}

///* ------------------------------------------------------------------------ */
////## method Boolean Range.opHAS(dynamic v);
//
//static KMETHOD Range_opHAS(CTX, ksfp_t *sfp _RIX)
//{
//	int res = 0/*NotFound*/;
//	kRange *o = sfp[0].range;
//	if(IS_Int((o)->start) && IS_Float(sfp[1].o)) {
//		KSETv(sfp[1].o, new_Int_(_ctx, CLASS_Int, (kint_t)sfp[1].fvalue));
//	}
//	else if(IS_Float((o)->start) && IS_Int(sfp[1].o)) {
//		KSETv(sfp[1].o, new_Float_(_ctx, CLASS_Float, (kfloat_t)sfp[1].ivalue));
//	}
//	else {
//		knh_stack_boxing(_ctx, sfp + 1);
//	}
//	if(knh_Object_compareTo(_ctx, o->start, sfp[1].o) <= 0) {
//		if(Range_isInclusive(o)) {
//			if(knh_Object_compareTo(_ctx, sfp[1].o, o->end) <= 0) {
//				res = 1;
//			}
//		}
//		else {
//			if(knh_Object_compareTo(_ctx, sfp[1].o, o->end) < 0) {
//				res = 1;
//			}
//		}
//	}
//	RETURNb_(res);
//}

/* ------------------------------------------------------------------------ */
//## method Boolean Array.opHAS(dynamic v);

static KMETHOD Array_opHAS(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = (kArray*)sfp[0].o;
	size_t i, res = 0/*NotFound*/;
	if(kArray_isUnboxData(a)) {
		kunbox_t d = O_data(sfp[1].o);
		for(i = 0; i < kArray_size(a); i++) {
			if(d == a->nlist[i]) { res = 1; break; }
		}
	}
	else {
		for(i = 0; i < kArray_size(a); i++) {
			if(knh_Object_compareTo(a->list[i], sfp[1].o) == 0) {
				res = 1; break;
			}
		}
	}
	RETURNb_(res);
}

///* ------------------------------------------------------------------------ */
////## method Boolean Tuple.opHAS(dynamic v);

//static KMETHOD Tuple_opHAS(CTX, ksfp_t *sfp _RIX)
//{
//	if(Tuple_isTriple(sfp[0].tuple)) {
//		kTuple *t = sfp[0].tuple;
//		int res;
//		knh_stack_boxing(_ctx, sfp + 1);
//		res = ((knh_Object_compareTo(_ctx, t->first, sfp[1].o) == 0)
//				|| (knh_Object_compareTo(_ctx, t->second, sfp[1].o) == 0)
//				|| (t->thirdNULL != NULL && (knh_Object_compareTo(_ctx, t->thirdNULL, sfp[1].o) == 0)));
//		RETURNb_(res);
//	}
//	else {
//		Array_opHAS(_ctx, sfp, rix); // reuse
//	}
//}

/* ------------------------------------------------------------------------ */
/* [Semantic Comparator] */

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Object.opIS(dynamic v);

static KMETHOD Object_opIS(CTX, ksfp_t *sfp _RIX)
{
//	kcid_t scid = O_cid(sfp[0].o);
//	kcid_t tcid = O_cid(sfp[1].o);
//	DBG_P("Semantic Matching %s === %s", CLASS__(scid), CLASS__(tcid));
//	if(scid == tcid) {
//		Object_opEQ(_ctx, sfp. rix);
//	}
//	else if(IS_NULL(sfp[1].o)) {
//		RETURNb_(IS_NULL(sfp[0].o));
//	}
//	else
	{
		TODO();
		RETURNb_(0);
//		BEGIN_LOCAL(_ctx, lsfp);
//		KNH_LPUSH(_ctx, sfp[1].o);
//		VM_MAP(_ctx, scid);
//		KSETv(sfp[1].o, ctx->esp[0].o);
//		END_LOCAL(_ctx, lsfp);
//		if(IS_NULL(sfp[1].o)) {
//			RETURNb_(0);
//		}
//		else {
//			Object_opEQ(_ctx, sfp);
//		}
	}
}

/* ------------------------------------------------------------------------ */
//## method Boolean Map.opHAS(T1 key);

static KMETHOD Map_opHAS(CTX, ksfp_t *sfp _RIX)
{
	kMap *m = sfp[0].m;
	RETURNb_(m->spi->get(_ctx, m->mapptr, sfp + 1, sfp + K_RIX));
}

/* ------------------------------------------------------------------------ */
//## method T2 Map.get(T1 key);

static KMETHOD Map_get(CTX, ksfp_t *sfp _RIX)
{
	kMap *m = sfp[0].m;
	if(!m->spi->get(_ctx, m->mapptr, sfp + 1, sfp + K_RIX)) {
		kcid_t cid = O_ct(m)->p2;
		sfp[K_RIX].ndata = 0;
		RETURN_(KNH_NULVAL(cid));
	}
}

/* ------------------------------------------------------------------------ */
//## method void Map.set(T1 key, T2 value);

static KMETHOD Map_set(CTX, ksfp_t *sfp _RIX)
{
	kMap *m = sfp[0].m;
	m->spi->set(_ctx, m->mapptr, sfp + 1);
	knh_writeBarrier(m, (sfp+1+1)->o);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void Map.remove(T1 key);

static KMETHOD Map_remove(CTX, ksfp_t *sfp _RIX)
{
	kMap *m = sfp[0].m;
	m->spi->remove(_ctx, m->mapptr, sfp + 1);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method T1[] Map.keys();

static KMETHOD Map_keys(CTX, ksfp_t *sfp _RIX)
{
	kMap *m = sfp[0].m;
	size_t size = m->spi->size(_ctx, m->mapptr);
	kcid_t p1 = O_ct(m)->p1;
	kArray *a = new_Array(_ctx, p1, size);
	ksfp_t *lsfp = ctx->esp;
	knitr_t mitrbuf = K_NITR_INIT, *mitr = &mitrbuf;
	klr_setesp(_ctx, lsfp+1);
	while(m->spi->next(_ctx, m->mapptr, mitr, lsfp)) {
		a->api->add(_ctx, a, lsfp);
		klr_setesp(_ctx, lsfp+1);
	}
	RETURN_(a);
}

/* ------------------------------------------------------------------------ */
/* [Arithemetic] */

/* ------------------------------------------------------------------------ */
/* [Int] */

//## @Const method Int Int.opADD(Int v);

static KMETHOD Int_opADD(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue + sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opNEG();

static KMETHOD Int_opNEG(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(-(sfp[0].ivalue));
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opSUB(Int n);

static KMETHOD Int_opSUB(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue - sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opMUL(Int n);

static KMETHOD Int_opMUL(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue * sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opDIV(Int n);

static KMETHOD Int_opDIV(CTX, ksfp_t *sfp _RIX)
{
	SYSLOG_iZERODIV(_ctx, sfp, sfp[1].ivalue);
	RETURNi_(sfp[0].ivalue / sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opMOD(Int n);

static KMETHOD Int_opMOD(CTX, ksfp_t *sfp _RIX)
{
	SYSLOG_iZERODIV(_ctx, sfp, sfp[1].ivalue);
	RETURNi_(sfp[0].ivalue % sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
/* [Float] */
//## @Const method Float! Float.opADD(Float! v);

static KMETHOD Float_opADD(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sfp[0].fvalue + sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Float! Float.opNEG();

static KMETHOD Float_opNEG(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(-(sfp[0].fvalue));
}

/* ------------------------------------------------------------------------ */
//## @Const method Float! Float.opSUB(Float! n);

static KMETHOD Float_opSUB(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sfp[0].fvalue - sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Float! Float.opMUL(Float! n);

static KMETHOD Float_opMUL(CTX, ksfp_t *sfp _RIX)
{
	RETURNf_(sfp[0].fvalue * sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Float! Float.opDIV(Float! n);

static KMETHOD Float_opDIV(CTX, ksfp_t *sfp _RIX)
{
	SYSLOG_fZERODIV(_ctx, sfp, sfp[1].fvalue);
	RETURNf_(sfp[0].fvalue / sfp[1].fvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.opSUB(String s);

static KMETHOD String_opSUB(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t base = S_tobytes(sfp[0].s);
	kbytes_t t = S_tobytes(sfp[1].s);
	kchar_t c = t.utext[0];
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	size_t i;
	for(i = 0; i < base.len; i++) {
		if(base.utext[i] == c) {
			size_t j;
			for(j = 1; j < t.len; j++) {
				if(base.utext[i+j] != t.utext[j]) break;
			}
			if(j == t.len) {
				i += t.len - 1;
				continue;
			}
		}
		kwb_putc(cwb->ba, base.utext[i]);
	}
	if(base.len == CWB_size(cwb)) {
		CWB_close(_ctx, cwb);
		RETURN_(sfp[0].o);
	}
	else {
		RETURN_(CWB_newString(_ctx, cwb, 0));
	}
}

/* ------------------------------------------------------------------------ */
//## @Const method Boolean Boolean.opNOT();

static KMETHOD Boolean_opNOT(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(!sfp[0].bvalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opLAND(Int n, ...);

static KMETHOD Int_opLAND(CTX, ksfp_t *sfp _RIX)
{
	size_t i, ac = knh_stack_argc(_ctx, sfp);
	kint_t n = sfp[0].ivalue;
	for(i = 1; i < ac; i++) {
		n = n & sfp[i].ivalue;
	}
	RETURNi_(n);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opLOR(Int n, ...);

static KMETHOD Int_opLOR(CTX, ksfp_t *sfp _RIX)
{
	size_t i, ac = knh_stack_argc(_ctx, sfp);
	kint_t n = sfp[0].ivalue;
	for(i = 1; i < ac; i++) {
		n = n | sfp[i].ivalue;
	}
	RETURNi_(n);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opLXOR(Int n);

static KMETHOD Int_opLXOR(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue ^ sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opLNOT();

static KMETHOD Int_opLNOT(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(~(sfp[0].ivalue));
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opLSFT(Int n);
//## @Const @Hidden method Int Int.send(Int n);

static KMETHOD Int_opLSFT(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue << sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## @Const method Int Int.opRSFT(Int n);

static KMETHOD Int_opRSFT(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(sfp[0].ivalue >> sfp[1].ivalue);
}

/* ------------------------------------------------------------------------ */
//## method Int Bytes.getSize();
//## method Int Array.getSize();

KMETHOD Bytes_getSize(CTX, ksfp_t *sfp _RIX)
{
	//fprintf(stderr, "** rix=%ld sfp[K_RIX]=%p\n", rix, sfp + rix);
	RETURNi_((sfp[0].ba)->bu.len);
}

/* ------------------------------------------------------------------------ */
//## method Int Tuple.getSize();

KMETHOD Tuple_getSize(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(O_ct(sfp[0].o)->cparam->psize);
}

/* ------------------------------------------------------------------------ */
//## method @Const Int String.getSize();

static KMETHOD String_getSize(CTX, ksfp_t *sfp _RIX)
{
	size_t size = IS_bString(sfp[0].s) ? S_size(sfp[0].s) : 0;
	if(!String_isASCII(sfp[0].s)) {
		size = knh_bytes_mlen(S_tobytes(sfp[0].s));
	}
	RETURNi_(size);
}

/* ------------------------------------------------------------------------ */
//## method Int Map.getSize();

KMETHOD Map_getSize(CTX, ksfp_t *sfp _RIX)
{
	kMap *m = sfp[0].m;
	RETURNi_(m->spi->size(_ctx, m->mapptr));
}

/* ------------------------------------------------------------------------ */
/* [get,set] */

/* ------------------------------------------------------------------------ */
//## method Int Bytes.get(Int n);

static KMETHOD Bytes_get(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t n2 = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), ba->bu.len);
	RETURNi_(ba->bu.utext[n2]);
}

/* ------------------------------------------------------------------------ */
//## method Int Bytes.set(Int n, Int c);

static KMETHOD Bytes_set(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t n2 = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), ba->bu.len);
	ba->bu.ubuf[n2] = Int_to(kchar_t, sfp[2]);
	RETURNi_(ba->bu.utext[n2]);
}

/* ------------------------------------------------------------------------ */
//## method void Bytes.setAll(Int c);

static KMETHOD Bytes_setAll(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[0].ba;
	size_t i, n = Int_to(size_t, sfp[1]);
	for(i = 0; i < ba->bu.len; i++) {
		ba->bu.ubuf[i] = n;
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.get(Int n);

static KMETHOD String_get(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t base = S_tobytes(sfp[0].s);
	kString *s;
	if(String_isASCII(sfp[0].s)) {
		size_t n = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), S_size(sfp[0].s));
		s = new_kString(base.text + n, 1, _ALWAYS|_ASCII);
	}
	else {
		size_t off = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), knh_bytes_mlen(base));
		kbytes_t sub = knh_bytes_mofflen(base, off, 1);
		s = new_kString(sub.text, sub.len, _ALWAYS|_CHARSIZE(sub.len));
	}
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
/* [range] */

static void _rangeUNTIL(CTX, ksfp_t *sfp, size_t size, size_t *s, size_t *e)
{
	*s = sfp[1].ivalue == 0 ? 0 : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), size);
	*e = sfp[2].ivalue == 0 ? (size) : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), size);
}

static void _rangeTO(CTX, ksfp_t *sfp, size_t size, size_t *s, size_t *e)
{
	*s = sfp[1].ivalue == 0 ? 0 : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), size);
	*e = sfp[2].ivalue == 0 ? (size) : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), size) + 1;
}

/* ------------------------------------------------------------------------ */

static kBytes *new_BytesRANGE(CTX, kBytes *ba, size_t s, size_t e)
{
	kBytes *newa = new_(Bytes);
	if(e < s) {
		size_t t = s; s = e; e = t;
	}
	DBG_ASSERT(e <= BA_size(ba));
	if(s < e) {
		size_t newsize = e - s;
		size_t capacity = newsize;
		if(newsize > 0) {
			if(capacity < 256) capacity = 256;
			newa->bu.ubuf = (kchar_t*)KNH_ZMALLOC(capacity);
			knh_bzero(newa->bu.ubuf, capacity);
			knh_memcpy(newa->bu.ubuf, ba->bu.utext + s, newsize);
		}
		else {
			newa->bu.ubuf = NULL;
		}
		newa->dim = new_dim(_ctx, capacity, 1);
		newa->bu.len = newsize;
	}
	return newa;
}

/* ------------------------------------------------------------------------ */
//## method Bytes Bytes.opUNTIL(Int s, Int e);

static KMETHOD Bytes_opUNTIL(CTX, ksfp_t *sfp _RIX)
{
	size_t s, e;
	_rangeUNTIL(_ctx, sfp, (sfp[0].ba)->bu.len, &s, &e);
	RETURN_(new_BytesRANGE(_ctx, sfp[0].ba, s, e));
}

/* ------------------------------------------------------------------------ */
//## method Bytes Bytes.opTO(Int s, Int e);

static KMETHOD Bytes_opTO(CTX, ksfp_t *sfp _RIX)
{
	size_t s, e;
	_rangeTO(_ctx, sfp, (sfp[0].ba)->bu.len, &s, &e);
	RETURN_(new_BytesRANGE(_ctx, sfp[0].ba, s, e));
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.substring(Int offset, Int length);

static KMETHOD String_substring(CTX, ksfp_t *sfp _RIX)
{
	kString *s;
	kbytes_t base = S_tobytes(sfp[0].s);
	kbytes_t t;
	if(String_isASCII(sfp[0].s)) {
		size_t offset = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), base.len);
		t = knh_bytes_last(base, offset);
		if(sfp[2].ivalue != 0) {
			size_t len = (size_t)sfp[2].ivalue;
			if(len < t.len) t = knh_bytes_first(t, len);
		}
	}
	else { // multibytes
		size_t mlen = knh_bytes_mlen(base);
		size_t offset = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), mlen);
		size_t length = sfp[2].ivalue == 0  ? (mlen - offset) : (size_t)sfp[2].ivalue;
		t = knh_bytes_mofflen(base, offset, length);
	}
	s = new_kString(t.text, t.len, _SUBCHAR(sfp[0].s));
	RETURN_(s);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.opUNTIL(Int s, Int e);

static KMETHOD String_opUNTIL(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = S_tobytes(sfp[0].s);
	if(sfp[2].ivalue != 0) {
		if(!String_isASCII(sfp[0].s)) {
			size_t mlen = knh_bytes_mlen(t);
			size_t offset = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), mlen);
			size_t length = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), mlen) - offset;
			t = knh_bytes_mofflen(t, offset, length);
			RETURN_(new_kString(t.text, t.len, 0));
		}
		else {
			size_t offset = Int_to(size_t, sfp[1]);
			sfp[2].ivalue = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), (sfp[0].s)->str.len) - offset;
		}
	}
	String_substring(_ctx, sfp, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Const method String String.opTO(Int s, Int e);

static KMETHOD String_opTO(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = S_tobytes(sfp[0].s);
	if(sfp[2].ivalue != 0) {
		if(!String_isASCII(sfp[0].s)) {
			size_t mlen = knh_bytes_mlen(t);
			size_t offset = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[1]), mlen);
			size_t length = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), mlen) - offset + 1;
			t = knh_bytes_mofflen(t, offset, length);
			RETURN_(new_kString(t.text, t.len, 0));
		}
		else {
			size_t offset = Int_to(size_t, sfp[1]);
			sfp[2].ivalue = knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), (sfp[0].s)->str.len) - offset + 1;
		}
	}
	String_substring(_ctx, sfp, K_RIX);
}

/* ------------------------------------------------------------------------ */

static kArray *new_ArrayRANGE(CTX, kArray *a, size_t s, size_t e /*until*/)
{
	kArray *newa = new_O(Array, O_cid(a));
	if(e < s) {
		size_t t = s; s = e; e = t;
	}
	if(s < e) {
		DBG_ASSERT(e <= a->size);
		size_t newsize = e - s;
		size_t sizeofdata = a->dim->wsize;
		if(newsize > 0) {
			char *p = (char*)a->list;
			kArray_grow(_ctx, newa, newsize, newsize);
			knh_memcpy(newa->list, p + (s * sizeofdata), newsize * sizeofdata);
			if(!kArray_isUnboxData(a)) {
				size_t i;
				for(i = 0; i < newsize; i++) {
					knh_Object_RCinc(newa->list[i]);
				}
			}
		}
		newa->size = newsize;
	}
	return newa;
}

/* ------------------------------------------------------------------------ */
//## method This Array.opUNTIL(Int s, Int e);

static KMETHOD Array_opUNTIL(CTX, ksfp_t *sfp _RIX)
{
	size_t s, e;
	_rangeUNTIL(_ctx, sfp, kArray_size(sfp[0].a), &s, &e);
	RETURN_(new_ArrayRANGE(_ctx, sfp[0].a, s, e));
}

/* ------------------------------------------------------------------------ */
//## method This Array.opTO(Int s, Int e);

static KMETHOD Array_opTO(CTX, ksfp_t *sfp _RIX)
{
	size_t s, e;
	_rangeTO(_ctx, sfp, kArray_size(sfp[0].a), &s, &e);
	RETURN_(new_ArrayRANGE(_ctx, sfp[0].a, s, e));
}

/* ------------------------------------------------------------------------ */
//## @Hidden method void Func.();

static KMETHOD Func_invoke(CTX, ksfp_t *sfp _RIX)
{
	kFunc* fo = sfp[0].fo;
	if(fo->baseNULL != NULL) {
		KSETv(sfp[0].o, fo->baseNULL);
	}
	klr_setmtdNC(_ctx, sfp[K_MTDIDX], fo->mtd);
	KNH_SELFCALL(_ctx, sfp, fo->mtd, K_RIX);
}

/* ------------------------------------------------------------------------ */
//## @Hidden method T1 Thunk.();

static KMETHOD Fmethod_returnConst(CTX, ksfp_t *sfp _RIX)
{
	// return values are always set at sfp[_rix] correctly
}

/* ------------------------------------------------------------------------ */
//## method T1 Thunk.eval();

static KMETHOD Thunk_eval(CTX, ksfp_t *sfp _RIX)
{

	kThunk *thk = (kThunk*)sfp[0].o;
	ksfp_t *lsfp = ctx->esp;
	//DBG_P("rix=%d, sfpidx=%d, espidx=%d", rix, sfp-ctx->stack, ctx->esp - ctx->stack);
	kMethod *mtd = (thk)->envsfp[K_CALLDELTA+K_MTDIDX].mtdNC;
	size_t i;
	for(i = 0; i < (thk)->envsize; i++) {
		KSETv(lsfp[i].o, (thk)->envsfp[i].o);
		lsfp[i].ndata = (thk)->envsfp[i].ndata;
	}
	KNH_SCALL(_ctx, lsfp, 0, mtd, ((thk)->envsize-K_CALLDELTA));
	KSETv((thk)->envsfp[0].o, lsfp[0].o);
	(thk)->envsfp[0].ndata = lsfp[0].ndata;
	Thunk_setEvaluated(thk, 1);
	KSETv(sfp[K_RIX].o, (thk)->envsfp[0].o);
	sfp[K_RIX].ndata = (thk)->envsfp[0].ndata;
}

/* ------------------------------------------------------------------------ */
//## method T1 Thunk.getValue();
//## mapper Thunk Tvoid;

static KMETHOD Thunk_value(CTX, ksfp_t *sfp _RIX)
{
	kThunk *thk = (kThunk*)sfp[0].o;
	if(Thunk_isEvaluated(thk)) {
		KSETv(sfp[K_RIX].o, (thk)->envsfp[0].o);
		sfp[K_RIX].ndata = (thk)->envsfp[0].ndata;
	}
	else {
		Thunk_eval(_ctx, sfp, K_RIX);
	}
}

/* ------------------------------------------------------------------------ */
//## @Throwable method InputStream InputStream.new(Path urn, String mode, Map _);

static KMETHOD InputStream_new(CTX, ksfp_t *sfp _RIX)
{
	kInputStream *in = sfp[0].in;
	kPath *pth = sfp[1].pth;
	const char *mode = IS_NULL(sfp[2].s) ? "r" : S_text(sfp[2].s);
	KSETv(in->path, pth);
	kio_t *io2 = pth->dpi->io2openNULL(_ctx, pth, mode, knh_toDictMap(_ctx, sfp[3].m, 0/*isCreation*/));
	if(io2 != NULL) {
		in->io2 = io2;
	}
	RETURN_(in);
}

/* ------------------------------------------------------------------------ */
//## method void InputStream.close();

static KMETHOD InputStream_close(CTX, ksfp_t *sfp _RIX)
{
	io2_close(_ctx, (sfp[0].in)->io2);
}

/* ------------------------------------------------------------------------ */
//## @Iterative method String InputStream.readLine();

static KMETHOD InputStream_readLine(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(io2_readLine(_ctx, (sfp[0].in)->io2, (sfp[0].in)->decNULL));
}

/* ------------------------------------------------------------------------ */
/* [iterators] */

static ITRNEXT InputStream_nextLine(CTX, ksfp_t *sfp _RIX)
{
	kIterator *itr = sfp[0].it;
	kInputStream *in = (kInputStream*)DP(itr)->source;
	kString *line = io2_readLine(_ctx, in->io2, in->decNULL);
	if(IS_NULL(line)) {
		ITREND_();
	}
	ITRNEXT_(line);
}

/* ------------------------------------------------------------------------ */
//## @Final mapper InputStream String..;
//## method String.. InputStream.opITR();

static TYPEMAP InputStream_String__(CTX, ksfp_t *sfp _RIX)
{
	kIterator *itr = new_IteratorG(_ctx, CLASS_StringITR, sfp[0].o, InputStream_nextLine);
	RETURN_(itr);
}

/* ------------------------------------------------------------------------ */
//## @Final mapper Path String..;
//## method String.. Path.opITR();

static TYPEMAP Path_String__(CTX, ksfp_t *sfp _RIX)
{
	kPath *path = (kPath*)sfp[K_TMRIDX].o;
	kIterator *itr;
	if(path->ospath != NULL && knh_isdir(_ctx, path->ospath)) {
		itr = new_ArrayIterator(_ctx, knh_PathDir_toArray(_ctx, path));
	}
	else {
		kInputStream *in = new_InputStream(_ctx, NULL, path);
		itr = new_IteratorG(_ctx, CLASS_StringITR, (Object*)in, InputStream_nextLine);
	}
	RETURN_(itr);
}

/* ------------------------------------------------------------------------ */
/* [OutputStream] */

//## @Throwable method OutputStream OutputStream.new(Path path, String mode, Map _);

static KMETHOD OutputStream_new(CTX, ksfp_t *sfp _RIX)
{
	kOutputStream *w = sfp[0].w;
	kPath *pth = sfp[1].pth;
	const char *mode = IS_NULL(sfp[2].s) ? "w" : S_text(sfp[2].s);
	KSETv(w->path, pth);
	kio_t *io2 = pth->dpi->io2openNULL(_ctx, pth, mode, knh_toDictMap(_ctx, sfp[3].m, 0/*isCeation*/));
	if(io2 != NULL) {
		w->io2 = io2;
	}
	RETURN_(w);
}

/* ------------------------------------------------------------------------ */
//## method void OutputStream.write(Bytes buf, Int offset, Int length);

static KMETHOD OutputStream_write(CTX, ksfp_t *sfp _RIX)
{
	kbytes_t t = BA_tobytes(sfp[1].ba);
	size_t offset = (sfp[2].ivalue == 0) ? 0 : knh_array_index(_ctx, sfp, Int_to(kint_t, sfp[2]), t.len);
	size_t len = (sfp[3].ivalue == 0) ? (t.len - offset) : Int_to(size_t, sfp[3]);
	if(offset + len > t.len) len = t.len - offset;
	t.utext = &(t.utext[offset]);
	t.len = len;
	knh_OutputStream_write(_ctx, sfp[0].w, t);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void OutputStream.print(Object value, ...);

static KMETHOD OutputStream_print(CTX, ksfp_t *sfp _RIX)
{
	kOutputStream *w = sfp[0].w;
	ksfp_t *v = sfp + 1;
	size_t i, ac = knh_stack_argc(_ctx, v);
	for(i = 0; i < ac; i++) {
		knh_write_Object(_ctx, w, v[i].o, FMT_s);
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void OutputStream.println(dynamic value, ...);

static KMETHOD OutputStream_println(CTX, ksfp_t *sfp _RIX)
{
	kOutputStream *w = sfp[0].w;
	ksfp_t *v = sfp + 1;
	size_t i, ac = knh_stack_argc(_ctx, v);
	for(i = 0; i < ac; i++) {
		knh_write_Object(_ctx, w, v[i].o, FMT_s);
	}
	knh_write_EOL(_ctx, w);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## @Hidden method void OutputStream.send(String s, ...);

static KMETHOD OutputStream_send(CTX, ksfp_t *sfp _RIX)
{
	kOutputStream *w = sfp[0].w;
	ksfp_t *v = sfp + 1;
	size_t i, ac = knh_stack_argc(_ctx, v);
	for(i = 0; i < ac; i++) {
		kString *s = v[i].s;
		if(s == TS_EOL) {
			knh_write_EOL(_ctx, w);
		}
		else {
			knh_write_utf8(_ctx, w, S_tobytes(s), !String_isASCII(s));
		}
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void OutputStream.flush();

static KMETHOD OutputStream_flush(CTX, ksfp_t *sfp _RIX)
{
	knh_OutputStream_flush(_ctx, sfp[0].w);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
//## method void OutputStream.close();

static KMETHOD OutputStream_close(CTX, ksfp_t *sfp _RIX)
{
	io2_close(_ctx, sfp[0].w->io2);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */
/* [format] */

static const char *newfmt(char *buf, size_t bufsiz, kbytes_t fmt, const char *t)
{
	char *p = buf + (fmt.len - 1);
	strncpy(buf, fmt.text, bufsiz);
	strncpy(p, t, bufsiz - fmt.len);
	DBG_P("FMT='%s'", buf);
	return (const char*)buf;
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Const method String Int.format(String fmt);

static KMETHOD Int_format(CTX, ksfp_t *sfp _RIX)
{
	ksfp_t *arg = ctx->esp - 1;
	DBG_ASSERT(arg == sfp+1);
	kbytes_t fmt = S_tobytes(arg[0].s);
	L_RETRY:;
	int ch = fmt.utext[fmt.len - 1];
	if(fmt.utext[0] == '%' && (ch == 'u' || ch == 'd' || ch == 'x')) {
		char fmtbuf[40], buf[80];
		const char *ifmt = (ch == 'd') ? KINT_FMT : ((ch == 'x') ? KINT_XFMT : KUINT_FMT);
		knh_snprintf(buf, sizeof(buf), newfmt(fmtbuf, sizeof(fmtbuf), fmt, ifmt + 1), sfp[0].ivalue);
		RETURN_(new_kString(buf, knh_strlen(buf), _ALWAYS|_ASCII));
	}
	if(fmt.len != 0) {
		KNH_NTRACE2(_ctx, "konoha:format", K_FAILED, KNH_LDATA(LOG_msg("invalid format"), LOG_s("format", fmt.text)));
	}
	fmt = STEXT("%d");
	goto L_RETRY;
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Const method String Float.format(String fmt);

static KMETHOD Float_format(CTX, ksfp_t *sfp _RIX)
{
	ksfp_t *arg = ctx->esp - 1;
	DBG_ASSERT(arg == sfp+1);
	kbytes_t fmt = S_tobytes(arg[0].s);
	L_RETRY:;
	int ch = fmt.utext[fmt.len - 1];
	if(fmt.utext[0] == '%' && (ch == 'f' || ch == 'e')) {
		char fmtbuf[40], buf[80];
		const char *ifmt = (ch == 'f') ? KFLOAT_FMT : KFLOAT_FMTE;
		knh_snprintf(buf, sizeof(buf), newfmt(fmtbuf, sizeof(fmtbuf), fmt, ifmt + strlen(ifmt)-1), sfp[0].fvalue);
		RETURN_(new_kString(buf, knh_strlen(buf), _ALWAYS|_ASCII));
	}
	if(fmt.len != 0) {
		KNH_NTRACE2(_ctx, "konoha:format", K_FAILED, KNH_LDATA(LOG_msg("invalid format"), LOG_s("format", fmt.text)));
	}
	fmt = STEXT("%f");
	goto L_RETRY;
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Const method String String.format(String fmt);

static KMETHOD String_format(CTX, ksfp_t *sfp _RIX)
{
	ksfp_t *arg = ctx->esp - 1;
	DBG_ASSERT(arg == sfp+1);
	kbytes_t fmt = S_tobytes(arg[0].s);
	if(fmt.utext[0] == '%' && fmt.utext[fmt.len-1] == 's') {
		char buf[256];
		knh_snprintf(buf, sizeof(buf), fmt.text, S_text(sfp[0].s));
		RETURN_(new_kString(buf, knh_strlen(buf), _ALWAYS|_ASCII));
	}
	if(fmt.len != 0) {
		KNH_NTRACE2(_ctx, "konoha:format", K_FAILED, KNH_LDATA(LOG_msg("invalid format"), LOG_s("format", fmt.text)));
	}
	RETURN_(sfp[0].s);
}

/* ------------------------------------------------------------------------ */

static kbool_t knh_Lingo_addLinkObject(CTX, kLingo *ns, kString *name, kObject *o, int typeCheck)
{
	if(DP(ns)->linkDictMapNULL == NULL) {
		KINITv(DP(ns)->linkDictMapNULL, new_DictMap0(_ctx, 0, 1/*isCaseMap*/, "linkDictMap"));
	}
	if(typeCheck) {

	}
	knh_DictMap_set(_ctx, DP(ns)->linkDictMapNULL, name, o);
	return 1;
}

static Object *knh_Lingo_getLinkObjectNULL(CTX, kLingo *ns, kbytes_t path)
{
	while(ns != NULL) {
		if(DP(ns)->linkDictMapNULL != NULL) {
			kObject *o = knh_DictMap_getNULL(_ctx, DP(ns)->linkDictMapNULL, path);
			if(o != NULL) return o;
		}
		ns = ns->parentNULL;
	}
	return NULL;
}

void knh_DataMap_log(CTX, kDictMap *conf, ktype_t type, const char *key)
{
	if(conf->uline != 0) {
		kuri_t uri = ULINE_uri(conf->uline);
		uintptr_t line = ULINE_line(conf->uline);
		knh_logprintf("CONFIG", 1, "(%s:%ld) key='%s' must have type %s", FILENAME__(uri), line, key, T__(type));
	}
	else {
		knh_logprintf("CONFIG", 1, "key='%s' must have type %s", key, T__(type));
	}
}

kbool_t knh_DataMap_check(CTX, kDictMap *conf, ktype_t type, const char *key, const char *key2)
{
	kbytes_t t = {{key}, knh_strlen(key)};
	kObject *v = knh_DictMap_getNULL(_ctx, conf, t);
	if(v == NULL) {
		if(key2 != NULL) {
			t.text = key2;
			t.len = knh_strlen(key2);
			v = knh_DictMap_getNULL(_ctx, conf, t);
			if(v == NULL) {
				knh_DataMap_log(_ctx, conf, type, key);
				return 0;
			}
			key = key2;
		}
	}
	if(O_cid(v) == type) return 1;
	knh_DataMap_log(_ctx, conf, type, key);
	return 0;
}

kString *knh_DataMap_getString(CTX, kDictMap *conf, const char *key, const char *key2, kString *def)
{
	kbytes_t t = {{key}, knh_strlen(key)};
	kString *v = (kString*)knh_DictMap_getNULL(_ctx, conf, t);
	if(v == NULL) {
		if(key2 != NULL) {
			t.text = key2;
			t.len = knh_strlen(key2);
			v = (kString*)knh_DictMap_getNULL(_ctx, conf, t);
		}
		if(v == NULL) {
			v = def;
		}
	}
	if(!IS_bString(v)) {
		v = def;
	}
	return def;
}

kString *knh_View_getQuery(CTX, kView *view)
{
	return knh_DataMap_getString(_ctx, view->conf, "query", NULL, TS_EMPTY);
}

static void THROW_Undefined(CTX, ksfp_t *sfp, const char *whatis, const char *what)
{
//	KNH_NTHROW(_ctx, sfp, "Script!!", "Undefined", KNH_LDATA(LOG_s("driver", what)));
}

/* ------------------------------------------------------------------------ */
//## @Static method void View.addView(String name, Lingo _, Map _);

static KMETHOD View_addView(CTX, ksfp_t *sfp _RIX)
{
	kbool_t tf = 0;
	kDictMap *conf = knh_toDictMap(_ctx, sfp[3].m, 1/*isCreation*/);
	KSETv(sfp[3].o, conf);
	tf = knh_DataMap_check(_ctx, conf, T_Path, "path", NULL);
	tf &= knh_DataMap_check(_ctx, conf, TY_String, "query", NULL);
	if(tf == 1) {
		kPath *path = (kPath*)knh_DictMap_getNULL(_ctx, conf, STEXT("path"));
		kString *dtype = knh_DataMap_getString(_ctx, conf, "type", "driver", NULL);
		if(dtype != NULL) {
			const knh_PathDPI_t *dpi = knh_Lingo_getStreamDPINULL(_ctx, sfp[2].ns, S_tobytes(dtype));
			if(dpi != NULL) {
				path->dpi = dpi;
			}
			else {
				THROW_Undefined(_ctx, sfp, "driver", S_text(dtype));
			}
		}
//		if(!dpi->existsSPI(_ctx, path)) {
//			THROW_NotFound(_ctx, sfp, "path", S_text(path->urn));
//		}
		kView *view = new_(View);
		KSETv(view->conf, conf);
		KSETv(view->path, path);
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		knh_Bytes_write(_ctx, cwb->ba, STEXT("view:"));
		knh_Bytes_write(_ctx, cwb->ba, S_tobytes(sfp[1].s));
		tf = knh_Lingo_addLinkObject(_ctx, sfp[2].ns, CWB_newString(_ctx, cwb, SPOL_POOL), UPCAST(view), 0);
		CWB_close(_ctx, cwb);
	}
	RETURNb_(tf);
}

/* ------------------------------------------------------------------------ */
//## @Hidden @Static @Const method View View.opLINK(String path, Lingo _);

static KMETHOD View_opLINK(CTX, ksfp_t *sfp _RIX)
{
	Object *o = knh_Lingo_getLinkObjectNULL(_ctx, sfp[2].ns, S_tobytes(sfp[1].s));
	if(o != NULL) {
		RETURN_(o);
	}
	RETURN_(KNH_TNULL(View));
}

/* ------------------------------------------------------------------------ */
//## @Hidden method DataITR View.opITR();

static KMETHOD View_opITR(CTX, ksfp_t *sfp _RIX)
{
	kView *view = sfp[0].rel;
	kPath *path = view->path;
	knh_Fitrnext fnext = NULL;
	if(!IS_NULL(path)) {
		fnext = path->dpi->qnextData;
	}
	RETURN_(new_IteratorG(_ctx, CLASS_DataITR, UPCAST(view), fnext));
}

/* ------------------------------------------------------------------------ */
//## @Static method InputStream System.getIn();

static KMETHOD System_getIn(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(_ctx->share->in);
}

/* ------------------------------------------------------------------------ */
//## @Static method OutputStream System.getOut();

static KMETHOD System_getOut(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(_ctx->share->out);
}

/* ------------------------------------------------------------------------ */
//## @Static method OutputStream System.getErr();

static KMETHOD System_getErr(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(_ctx->share->err);
}

/* ------------------------------------------------------------------------ */
//## @Static method dynamic System.getProperty(String key);

static KMETHOD System_getProperty(CTX, ksfp_t *sfp _RIX)
{
	kObject *v = (kObject*)knh_getPropertyNULL(_ctx, S_tobytes(sfp[1].s));
	if(v == NULL) {
		v = KNH_NULVAL(CLASS_String);
		knh_setProperty(_ctx, sfp[1].s, v);
	}
	sfp[K_RIX].ndata = O_ndata(v);
	RETURN_(v);
}

/* ------------------------------------------------------------------------ */
//## @Static method dynamic System.setProperty(String key, dynamic value);

static KMETHOD System_setProperty(CTX, ksfp_t *sfp _RIX)
{
	knh_setProperty(_ctx, sfp[1].s, sfp[2].o);
	sfp[K_RIX].ndata = O_ndata(sfp[2].o);
	RETURN_(sfp[2].o);
}

/* ------------------------------------------------------------------------ */

static kbool_t bytes_matchWildCard(kbytes_t t, kbytes_t p)
{
	if(p.utext[0] == '*') {
		p.utext = p.utext + 1;
		p.len = p.len - 1;
		return knh_bytes_endsWith_(t, p);
	}
	else if(p.utext[p.len-1] == '*') {
		p.len -= 1;
		return knh_bytes_startsWith_(t, p);
	}
	else {
		kindex_t idx = knh_bytes_index(p, '*');
		if(idx == -1) {
			return knh_bytes_startsWith_(t, p);
		}
		else {
			return knh_bytes_startsWith_(t, knh_bytes_first(p, idx)) &&
				knh_bytes_endsWith_(t, knh_bytes_last(p, idx+1));
		}
	}
}

/* ------------------------------------------------------------------------ */
//## @Hidden method String[] System.listProperties(String key);

static KMETHOD System_listProperties(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = new_Array(_ctx, CLASS_String, 0);
	kbytes_t prefix = IS_NULL(sfp[1].s) ? STEXT("") : S_tobytes(sfp[1].s);
	kDictMap *map = ctx->share->props;
	size_t i;
	for(i = 0; i < knh_Map_size(map); i++) {
		kString *key = knh_DictMap_keyAt(map, i);
		if(bytes_matchWildCard(S_tobytes(key), prefix)) {
			kArray_add(a, key);
		}
	}
	RETURN_(a);
}

/* ------------------------------------------------------------------------ */
//## @Static method void System.gc();

static KMETHOD System_gc(CTX, ksfp_t *sfp _RIX)
{
	knh_System_gc(_ctx, 0/*needsStackTrace*/ GC_TENURE);
}

/* ------------------------------------------------------------------------ */
//## method Int System.getTime();

static KMETHOD System_getTime(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(knh_getTimeMilliSecond());
}

/* ------------------------------------------------------------------------ */
//## @Restricted method void System.exit(Int status);

static KMETHOD System_exit(CTX, ksfp_t *sfp _RIX)
{
	int status = IS_NULL(sfp[1].o) ? 0 : Int_to(size_t, sfp[1]);
	KNH_NTRACE2(_ctx, "exit", K_NOTICE, KNH_LDATA(LOG_i("user_specified_status", status)));
	exit(status);
}

///* ------------------------------------------------------------------------ */
////## @Static method InputStream Context.setIn(InputStream? in);
//
//static KMETHOD CTX_setIn(CTX, ksfp_t *sfp _RIX)
//{
//	KSETv(((kcontext_t*)ctx)->in, sfp[1].o);
//	RETURN_(sfp[1].o);
//}
//
///* ------------------------------------------------------------------------ */
////## @Static method OutputStream Context.setOut(OutputStream? out);
//
//static KMETHOD CTX_setOut(CTX, ksfp_t *sfp _RIX)
//{
//	KSETv(((kcontext_t*)ctx)->out, sfp[1].o);
//	RETURN_(sfp[1].o);
//}
//
///* ------------------------------------------------------------------------ */
////## @Static method OutputStream Context.setErr(OutputStream? out);
//
//static KMETHOD CTX_setErr(CTX, ksfp_t *sfp _RIX)
//{
//	KSETv(((kcontext_t*)ctx)->err, sfp[1].o);
//	RETURN_(sfp[1].o);
//}

/* ------------------------------------------------------------------------ */
//## method String Context.getTrace();

static KMETHOD Context_getTrace(CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_String(_ctx, ctx->trace));
}

/* ------------------------------------------------------------------------ */
//## method Int Context.getSeq();

static KMETHOD Context_getSeq(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(_ctx->seq);
}

/* ------------------------------------------------------------------------ */
//## method Boolean Exception.opOF(String event);

static KMETHOD Exception_opOF(CTX, ksfp_t *sfp _RIX)
{
	kevent_t eid = knh_geteid(_ctx, S_tobytes(sfp[1].s));
	kevent_t eid0 = knh_geteid(_ctx, S_tobytes((sfp[0].e)->emsg));
	int isa = event_isa(_ctx, eid0, eid);
	RETURNb_(isa);
}

/* ------------------------------------------------------------------------ */
//## @Static @Restricted method String System.exec(String cmd);

static KMETHOD System_exec(CTX, ksfp_t *sfp _RIX)
{
#ifdef K_DEOS_TRACE
	char cmd[1024];
	knh_snprintf(cmd, sizeof(cmd), "%s=%s %s", K_DEOS_TRACE, ctx->trace, S_text(sfp[1].s));
#else
	const char *cmd = S_text(sfp[1].s);
#endif
	//NOTE_(_ctx, "fork command='%s'", cmd);
#ifdef K_USING_POSIX_
#ifndef K_USING_LINUX_
	FILE *fp = popen((const char*)cmd, "r+");
#else
	FILE *fp = popen((const char*)cmd, "r");
#endif
	if(fp != NULL) {
		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
		char buf[K_PAGESIZE];
		while(1) {
			size_t size = fread(buf, 1, sizeof(buf), fp);
			if(size > 0) {
				kbytes_t t = {{buf}, size};
				knh_Bytes_write(_ctx, cwb->ba, t);
			}
			else {
				break;
			}
		};
		pclose(fp);
		RETURN_(CWB_newString(_ctx, cwb, SPOL_POOLNEVER));
	}
	else {
		KNH_LOG("command failed: %s", cmd);
	}
#endif
	RETURN_(KNH_NULVAL(CLASS_String));
}

/* ------------------------------------------------------------------------ */
//## @Restricted method Tvar System.eval(String cmd, Script _, Lingo _, Class _);

static KMETHOD System_eval(CTX, ksfp_t *sfp _RIX)
{
//	fprintf(stderr, "TESTING: '%s'\n", 	S_text(sfp[1].s));
//	fprintf(stderr, "RETURN VALUE: '%s'\n", CLASS__(sfp[4].c->cid));
	kScript *scr = ctx->gma->scr;
	kLingo *ns = K_GMANS;
	kcid_t tcid = sfp[4].c->cid;
	if(scr != sfp[2].scr) {
		KSETv(ctx->gma->scr, sfp[2].scr);
		sfp[2].scr = scr;
	}
	if(ns != sfp[3].ns) {
		KSETv(K_GMANS, sfp[3].ns);
		sfp[3].ns = ns;
	}
	KSETv(((kcontext_t*)ctx)->evaled, K_NULL);
	KSETv(((kcontext_t*)ctx)->e, K_NULL);
#ifdef K_USING_SUGAR
	knh_beval2(_ctx, S_text(sfp[1].s), 1);
#else
	kInputStream *bin = new_BytesInputStream(_ctx, S_text(sfp[1].s), S_size(sfp[1].s));
	knh_beval(_ctx, bin, 1);
#endif
	scr = ctx->gma->scr;
	ns = K_GMANS;
	if(scr != sfp[2].scr) {
		KSETv(ctx->gma->scr, sfp[2].scr);
		sfp[2].scr = scr;
	}
	if(ns != sfp[3].ns) {
		KSETv(K_GMANS, sfp[3].ns);
		sfp[3].ns = ns;
	}
	if(_ctx->isEvaled == 1) {
		kObject *v = ctx->evaled;
		WCTX(_ctx)->isEvaled = 0;
		if(tcid == CLASS_Tvoid) return;
		if(v != K_NULL && ClassTBL_isa(O_ct(v), tcid)) {
			if(TY_isUnbox(tcid)) {
				RETURNi_(N_toint(v));
			}
			else {
				RETURN_(v);
			}
		}
	}
	sfp[K_RIX].ndata = 0;
	RETURN_(KNH_NULVAL(tcid));
}

/* ------------------------------------------------------------------------ */
//## method String System.readLine(String cmd)

static KMETHOD System_readLine(CTX, ksfp_t *sfp _RIX)
{
	const char *line = ctx->spi->readline(S_text(sfp[1].s));
	kString *s = new_String(_ctx, line);
	free((void*)line);
	RETURN_(s);
}

#endif/*K_INCLUDE_BUILTINAPI*/

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
