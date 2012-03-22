#ifdef __cplusplus
extern "C" {
#endif
// THIS FILE WAS AUTOMATICALLY GENERATED

static const struct codeasm_field F_KNH_HOBJECT_T[] = {
	{"magicflag", ASMCODE_T_LONG},
	{"ct", ASMCODE_T_VOIDPTR},
	{"refcuintptr_t", ASMCODE_T_LONG},
	{"meta", ASMCODE_T_VOIDPTR}
};
#define SIZE_F_KNH_HOBJECT_T 4
static const struct cstruct STRUCT_KNH_HOBJECT_T = {
	"kObjectHeader",
	SIZE_F_KNH_HOBJECT_T,
	F_KNH_HOBJECT_T
};
static const struct codeasm_field F_KNH_OBJECT_T[] = {
	{"h", ASMCODE_T_HOBJECT},
	{"ref", ASMCODE_T_VOIDPTR},
	{"ref2_unused", ASMCODE_T_VOIDPTR},
	{"ref3_unused", ASMCODE_T_VOIDPTR},
	{"ref4_tail", ASMCODE_T_OBJECTPTR}
};
#define SIZE_F_KNH_OBJECT_T 5
static const struct cstruct STRUCT_KNH_OBJECT_T = {
	"kObject",
	SIZE_F_KNH_OBJECT_T,
	F_KNH_OBJECT_T
};
static const struct codeasm_field F_KNH_ARRAY_T[] = {
	{"h", ASMCODE_T_HOBJECT},
	{"nlistkint_t*", ASMCODE_T_VOIDPTR},
	{"size", ASMCODE_T_LONG},
	{"dim", ASMCODE_T_VOIDPTR},
	{"api", ASMCODE_T_VOIDPTR}
};
#define SIZE_F_KNH_ARRAY_T 5
static const struct cstruct STRUCT_KNH_ARRAY_T = {
	"kArray",
	SIZE_F_KNH_ARRAY_T,
	F_KNH_ARRAY_T
};
static const struct codeasm_field F_KNH_OBJECTFIELD_T[] = {
	{"h", ASMCODE_T_HOBJECT},
	{"fields", ASMCODE_T_OBJECTPTRPTR},
	{"smallobject", ASMCODE_T_OBJECTPTR}
};
#define SIZE_F_KNH_OBJECTFIELD_T 3
static const struct cstruct STRUCT_KNH_OBJECTFIELD_T = {
	"kObject",
	SIZE_F_KNH_OBJECTFIELD_T,
	F_KNH_OBJECTFIELD_T
};
static const struct codeasm_field F_KNH_OUTPUTSTREAM_T[] = {
	{"h", ASMCODE_T_HOBJECT},
	{"b", ASMCODE_T_VOIDPTR},
	{"uline", ASMCODE_T_LONG},
	{"dspi", ASMCODE_T_VOIDPTR},
	{"encNULL", ASMCODE_T_VOIDPTR}
};
#define SIZE_F_KNH_OUTPUTSTREAM_T 5
static const struct cstruct STRUCT_KNH_OUTPUTSTREAM_T = {
	"kOutputStream",
	SIZE_F_KNH_OUTPUTSTREAM_T,
	F_KNH_OUTPUTSTREAM_T
};
static const struct codeasm_field F_KNH_INPUTSTREAM_T[] = {
	{"h", ASMCODE_T_HOBJECT},
	{"b", ASMCODE_T_VOIDPTR},
	{"uline", ASMCODE_T_LONG},
	{"dspi", ASMCODE_T_VOIDPTR},
	{"decNULL", ASMCODE_T_VOIDPTR}
};
#define SIZE_F_KNH_INPUTSTREAM_T 5
static const struct cstruct STRUCT_KNH_INPUTSTREAM_T = {
	"kInputStream",
	SIZE_F_KNH_INPUTSTREAM_T,
	F_KNH_INPUTSTREAM_T
};
static const struct codeasm_field F_KNH_SFP_T[] = {
	{"onh_Int_t*", ASMCODE_T_OBJECTPTR},
	{"ndatanh_bool_t", ASMCODE_T_INT64}
};
#define SIZE_F_KNH_SFP_T 2
static const struct cstruct STRUCT_KNH_SFP_T = {
	"ksfp_t",
	SIZE_F_KNH_SFP_T,
	F_KNH_SFP_T
};
static const struct codeasm_field F_KNH_SYSTEMEX_T[] = {
	{"sysid", ASMCODE_T_LONG},
	{"ctxcount", ASMCODE_T_LONG},
	{"props", ASMCODE_T_VOIDPTR},
	{"in", ASMCODE_T_INSPTR},
	{"out", ASMCODE_T_OUSPTR},
	{"err", ASMCODE_T_OUSPTR},
	{"enc", ASMCODE_T_VOIDPTR},
	{"tokenDictSet", ASMCODE_T_VOIDPTR},
	{"nameDictCaseSet", ASMCODE_T_VOIDPTR},
	{"namecapacity", ASMCODE_T_LONG},
	{"nameinfo", ASMCODE_T_VOIDPTR},
	{"urnDictSet", ASMCODE_T_VOIDPTR},
	{"urns", ASMCODE_T_ARRAYPTR},
	{"ClassNameDictSet", ASMCODE_T_VOIDPTR},
	{"EventDictCaseSet", ASMCODE_T_VOIDPTR},
	{"PackageDictMap", ASMCODE_T_VOIDPTR},
	{"URNAliasDictMap", ASMCODE_T_VOIDPTR},
	{"dspiDictSet", ASMCODE_T_VOIDPTR}
};
#define SIZE_F_KNH_SYSTEMEX_T 18
static const struct cstruct STRUCT_KNH_SYSTEMEX_T = {
	"knh_SystemEX_t",
	SIZE_F_KNH_SYSTEMEX_T,
	F_KNH_SYSTEMEX_T
};
static const struct codeasm_field F_KNH_SYSTEM_T[] = {
	{"h", ASMCODE_T_HOBJECT},
	{"b", ASMCODE_T_SYSB}
};
#define SIZE_F_KNH_SYSTEM_T 2
static const struct cstruct STRUCT_KNH_SYSTEM_T = {
	"kSystem",
	SIZE_F_KNH_SYSTEM_T,
	F_KNH_SYSTEM_T
};
static const struct codeasm_field F_KNH_METHOD_T[] = {
	{"h", ASMCODE_T_HOBJECT},
	{"b", ASMCODE_T_VOIDPTR},
	{"cid", ASMCODE_T_SHORT},
	{"mn", ASMCODE_T_SHORT},
	{"fcall_1", ASMCODE_T_FUNCTION},
	{"pc_start", ASMCODE_T_VOIDPTR}
};
#define SIZE_F_KNH_METHOD_T 6
static const struct cstruct STRUCT_KNH_METHOD_T = {
	"kMethod",
	SIZE_F_KNH_METHOD_T,
	F_KNH_METHOD_T
};
static const struct codeasm_field F_KNH_ITERATOR_T[] = {
	{"h", ASMCODE_T_HOBJECT},
	{"b", ASMCODE_T_VOIDPTR},
	{"fnext_1", ASMCODE_T_FUNCTION}
};
#define SIZE_F_KNH_ITERATOR_T 3
static const struct cstruct STRUCT_KNH_ITERATOR_T = {
	"kIterator",
	SIZE_F_KNH_ITERATOR_T,
	F_KNH_ITERATOR_T
};
static const struct codeasm_field F_KNH_CONTEXT_T[] = {
	{"sharekshare_t*", ASMCODE_T_VOIDPTR},
	{"stat", ASMCODE_T_VOIDPTR},
	{"spi", ASMCODE_T_VOIDPTR},
	{"api2", ASMCODE_T_VOIDPTR},
	{"sys", ASMCODE_T_SYSPTR},
	{"script", ASMCODE_T_VOIDPTR},
	{"stack", ASMCODE_T_SFPPTR},
	{"esp", ASMCODE_T_SFPPTR},
	{"stacksize", ASMCODE_T_LONG},
	{"stack_uplimt", ASMCODE_T_SFPPTR},
	{"cstack_bottom", ASMCODE_T_VOIDPTR},
	{"e", ASMCODE_T_VOIDPTR},
	{"freeObjectList", ASMCODE_T_OBJECTPTR},
	{"freeObjectTail", ASMCODE_T_OBJECTPTR},
	{"freeObjectListSize", ASMCODE_T_LONG},
	{"mscheck", ASMCODE_T_LONG},
	{"freeMemoryList", ASMCODE_T_VOIDPTR},
	{"freeMemoryTail", ASMCODE_T_VOIDPTR},
	{"mtdcache", ASMCODE_T_VOIDPTR},
	{"tmrcache", ASMCODE_T_VOIDPTR},
	{"refs", ASMCODE_T_OBJECTPTRPTR},
	{"ref_size", ASMCODE_T_LONG},
	{"ref_buf", ASMCODE_T_OBJECTPTRPTR},
	{"ref_capacity", ASMCODE_T_LONG},
	{"queue", ASMCODE_T_OBJECTPTRPTR},
	{"queue_capacity", ASMCODE_T_LONG},
	{"queue_log2", ASMCODE_T_LONG},
	{"enc", ASMCODE_T_VOIDPTR},
	{"in", ASMCODE_T_INSPTR},
	{"out", ASMCODE_T_OUSPTR},
	{"err", ASMCODE_T_OUSPTR},
	{"bufa", ASMCODE_T_VOIDPTR},
	{"bufw", ASMCODE_T_OUSPTR},
	{"gma", ASMCODE_T_VOIDPTR},
	{"_UNUSED_symbolDictMap", ASMCODE_T_VOIDPTR},
	{"_UNUSED_constPools", ASMCODE_T_VOIDPTR},
	{"flag", ASMCODE_T_SHORT},
	{"ctxid", ASMCODE_T_SHORT},
	{"ctxobjNC", ASMCODE_T_VOIDPTR},
	{"parent", ASMCODE_T_VOIDPTR},
	{"ctxlock", ASMCODE_T_VOIDPTR},
	{"trace", ASMCODE_T_VEC16},
	{"seq", ASMCODE_T_INT64},
	{"ehdrNC", ASMCODE_T_VOIDPTR},
	{"evaled", ASMCODE_T_OBJECTPTR},
	{"signal", ASMCODE_T_VOIDPTR},
	{"siginfo", ASMCODE_T_VOIDPTR},
	{"sighandlers", ASMCODE_T_VOIDPTR}
};
#define SIZE_F_KNH_CONTEXT_T 48
static const struct cstruct STRUCT_KNH_CONTEXT_T = {
	"kcontext_t",
	SIZE_F_KNH_CONTEXT_T,
	F_KNH_CONTEXT_T
};
#ifdef __cplusplus
}
#endif

// THIS FILE WAS AUTOMATICALLY GENERATED. DON'T EDIT.

