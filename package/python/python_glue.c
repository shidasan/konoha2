#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>
#include<konoha2/float.h>
#include<python2.7/Python.h>

typedef const struct _kPyObject kPyObject;
struct _kPyObject {
	kObjectHeader h;
	PyObject *self;
};

static void PyObject_init(CTX, kObject *o, void *conf)
{
	struct _kPyObject *pyo = (struct _kPyObject*)o;
	if(conf == NULL) {
		pyo->self = Py_None;
		Py_INCREF(Py_None);
	}
	else {
		pyo->self = (PyObject*)conf;
	}
}

static void PyObject_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	kPyObject *pyo =  (kPyObject*)sfp[pos].o;
	if(PyInt_Check(pyo->self)) {
		kwb_printf(wb, "%ld", PyInt_AsLong(pyo->self));
	}
	else {
		kwb_printf(wb, "unknown python type");
	}
}

static void PyObject_free(CTX, kObject *o)
{
	struct _kPyObject *pyo = (struct _kPyObject*)o;
	Py_DECREF(pyo->self);
	pyo->self = NULL;
}

#define RETURN_PyObject(O)  RETURN_PyObject_(_ctx, sfp, O K_RIXPARAM)

static void RETURN_PyObject_(CTX, ksfp_t *sfp, PyObject *pyo _RIX)
{
	if(pyo != NULL) {
    	RETURN_(new_kObject(O_ct(sfp[K_RTNIDX].o), pyo));
	}
	else {
		PyErr_Print(); // looks stupid
	}
}

static KMETHOD Boolean_toPyObject(CTX, ksfp_t *sfp _RIX)
{
	RETURN_PyObject(PyBool_FromLong(sfp[0].ivalue));
}

static KMETHOD PyObject_toBoolean(CTX, ksfp_t *sfp _RIX)
{
	kPyObject *po = (kPyObject*)sfp[0].o;
	RETURNb_(po->self == Py_True ? 1 : 0);
}

static KMETHOD Int_toPyObject(CTX, ksfp_t *sfp _RIX)
{
	RETURN_PyObject(PyInt_FromLong(sfp[0].ivalue));
}

static KMETHOD PyObject_toInt(CTX, ksfp_t *sfp _RIX)
{
	kPyObject *po = (kPyObject*)sfp[0].o;
	long v = PyInt_AsLong(po->self);
	if(PyErr_Occurred()) {
		v = 0;
	}
	RETURNi_(v);
}

static KMETHOD Float_toPyObject(CTX, ksfp_t *sfp _RIX)
{
	RETURN_PyObject(PyFloat_FromDouble(sfp[0].fvalue));
}

static KMETHOD PyObject_toFloat(CTX, ksfp_t *sfp _RIX)
{
	kPyObject *po = (kPyObject*)sfp[0].o;
	double v = PyFloat_AsDouble(po->self);
	if(PyErr_Occurred()) {
		v = 0;
	}
	RETURNf_(v);
}

static KMETHOD String_toPyObject(CTX, ksfp_t *sfp _RIX)
{
	RETURN_PyObject(PyUnicode_FromString(S_text(sfp[0].s)));
}

static KMETHOD PyObject_toString(CTX, ksfp_t *sfp _RIX)
{
	kPyObject *po = (kPyObject*)sfp[0].o;
	PyObject *s = PyUnicode_AsUTF8String(po->self);
	if(s != NULL) {
		//dec
		const char *t = PyString_AsString(s);
		RETURN_(new_kString(t, strlen(t), 0));
	}
}

//## Boolean Python.eval(String script);
static KMETHOD Python_eval(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(PyRun_SimpleString(S_text(sfp[1].s)) == 0);
}

//## PyObject Python.import(String name);
static KMETHOD Python_import(CTX, ksfp_t *sfp _RIX)
{
	RETURN_PyObject(PyImport_ImportModule(S_text(sfp[1].s)));
}

//## PyObject PyObject.(PyObject o);
static KMETHOD PyObject_(CTX, ksfp_t *sfp _RIX)
{
	int argc = _ctx->esp - sfp - 2;
	kPyObject *pmod = (kPyObject*)sfp[0].o;
	PyObject  *pFunc = PyObject_GetAttrString(pmod->self, S_text(_ctx->esp[-1].s));
	PyObject  *pArgs = NULL, *pValue = NULL;
	if(pFunc != NULL) {
		if(PyCallable_Check(pFunc)) {
			int i;
			pArgs = PyTuple_New(argc);
			for (i = 0; i < argc; ++i) {
				pValue = ((kPyObject*)sfp[i+1].o)->self;
				Py_INCREF(pValue);
				PyTuple_SetItem(pArgs, i, pValue);
			}
			pValue = PyObject_CallObject(pFunc, pArgs);
		}
	}
	Py_XDECREF(pFunc);
	Py_XDECREF(pArgs);
	RETURN_PyObject(pValue);
}

// --------------------------------------------------------------------------

static int python_init_count = 0;

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _Im       kMethod_Immutable
#define _F(F)   (intptr_t)(F)

static	kbool_t python_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	python_init_count++;
	if(python_init_count == 1) {
		Py_Initialize();
	}
//	kmodfloat_t *base = (kmodfloat_t*)KNH_ZMALLOC(sizeof(kmodfloat_t));
//	base->h.name     = "float";
//	base->h.setup    = kmodfloat_setup;
//	base->h.reftrace = kmodfloat_reftrace;
//	base->h.free     = kmodfloat_free;
//	ksetModule(MOD_float, &base->h, pline);

	KDEFINE_CLASS PyObjectDef = {
		STRUCTNAME(PyObject),
#ifndef WITH_ECLIPSE
		.packid  = ks->packid,
#endif
		.cflag = 0,
		.init = PyObject_init,
		.free = PyObject_free,
		.p    = PyObject_p,
	};
	kclass_t *cPython = kaddClassDef(NULL, &PyObjectDef, pline);
	int TY_PyObject = cPython->cid;
	intptr_t MethodData[] = {
		_Public|_Const|_Im|_Coercion, _F(PyObject_toBoolean), TY_Boolean, TY_PyObject, MN_to(TY_Boolean), 0,
		_Public|_Const|_Im|_Coercion, _F(Boolean_toPyObject), TY_PyObject, TY_Boolean, MN_to(TY_PyObject), 0,
		_Public|_Const|_Im|_Coercion, _F(PyObject_toInt), TY_Int, TY_PyObject, MN_to(TY_Int), 0,
		_Public|_Const|_Im|_Coercion, _F(Int_toPyObject), TY_PyObject, TY_Int, MN_to(TY_PyObject), 0,
		_Public|_Const|_Im|_Coercion, _F(PyObject_toString), TY_String, TY_PyObject, MN_to(TY_String), 0,
		_Public|_Const|_Im|_Coercion, _F(String_toPyObject), TY_PyObject, TY_String, MN_to(TY_PyObject), 0,
		_Public|_Im, _F(Python_eval), TY_Boolean, TY_System, FN_("pyEval"), 1, TY_String, FN_("script"),
		_Public|_Im, _F(Python_import), TY_PyObject, TY_System, FN_("importPyModule"), 1, TY_String, FN_("name"),
		_Public|_Im, _F(PyObject_), TY_PyObject, TY_PyObject, 0, 1, TY_PyObject, 0,
		DEND,
	};
	kloadMethodData(ks, MethodData);
	if(IS_defineFloat()) {
		intptr_t MethodData[] = {
			_Public|_Const|_Im|_Coercion, _F(PyObject_toFloat), TY_Float, TY_PyObject, MN_to(TY_Float), 0,
			_Public|_Const|_Im|_Coercion, _F(Float_toPyObject), TY_PyObject, TY_Float, MN_to(TY_PyObject), 0,
			DEND,
		};
		kloadMethodData(ks, MethodData);
	}
	return true;
}

static kbool_t python_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t python_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t python_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#ifdef WITH_ECLIPSE
KDEFINE_PACKAGE* KX_init(CTX)
#else
KDEFINE_PACKAGE* python_init(void)
#endif
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("python", "1.0"),
		.initPackage = python_initPackage,
		.setupPackage = python_setupPackage,
		.initKonohaSpace = python_initKonohaSpace,
		.setupKonohaSpace = python_setupKonohaSpace,
	};
#ifdef WITH_ECLIPSE
	python_initPackage(_ctx, NULL, 0, NULL, 0);
#endif
	return &d;
}
