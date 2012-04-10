#include "konoha2/konoha2.h"
#include "konoha2/gc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Dummy {
    int x;
} kDummy;


static int __init__  = 0;
static int __trace__ = -1;
static int __free__  = 0;

static void Dummy_init(CTX, kObject *o, void *conf)
{
    assert((uintptr_t)conf == 0xdeadbeaf);
    ((kDummy*)o)->x = __init__++;
}

static void Dummy_reftrace(CTX, kObject *o)
{
    __trace__++;
}

static void Dummy_free(CTX, kObject *o)
{
    __free__++;
}

static KDEFINE_CLASS DummyDef = {
    .structname   = "Dummy",
    .cid          = 100,
    .cflag        = 0,
    .cstruct_size = sizeof(struct Dummy),
    .init     = Dummy_init,
    .reftrace = Dummy_reftrace,
    .free     = Dummy_free
};

void test_gc(CTX)
{
#define CT_Dummy ct
    int i, j;
    const kclass_t *ct = kaddClassDef(NULL, &DummyDef, 0);
    /* small size */
    for (i = 0; i < 10; ++i) {
        for (j = 0; j < 100; ++j) {
            kDummy *dummy = new_(Dummy, 0xdeadbeaf);
            assert(__init__ == dummy->x+1);
        }
        assert(__init__ == (i+1) * 100);
        assert(__trace__ == -1);
        MODGC_gc_invoke(_ctx, 0);
    }

    int small_object_count = __init__;
    /* middle size */
    for (i = 0; i < 100; ++i) {
        for (j = 0; j < 1000; ++j) {
            kDummy *dummy = new_(Dummy, 0xdeadbeaf);
            assert(__init__ == dummy->x+1);
        }
        assert(__init__ == (i+1) * 1000 + small_object_count);
        assert(__trace__ == -1);
        MODGC_gc_invoke(_ctx, 0);
    }
}

int main(int argc, const char *argv[])
{
    int ret = 0;
    konoha_t konoha = konoha_open();
    test_gc(konoha);
    konoha_close(konoha);
    assert(__free__ == __init__);
    fprintf(stderr, "alloced_object_count = %d, freed_object_count=%d\n", __init__, __free__);
    MODGC_check_malloced_size();
    return ret;
}

#ifdef __cplusplus
}
#endif
