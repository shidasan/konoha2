#include "konoha2/konoha2.h"
#include "../src/gc/gc_api.h"

static int _sum_  = 0;
static int _sum2_ = 0;
static void reftrace(CTX, kmape_t *e)
{
    _sum_ += e->uvalue;
}
static void reftrace2(CTX, void *e)
{
    _sum2_ += (uintptr_t)e;
}

void test_Kmap(CTX)
{
    int i;
    kmap_t* map = _ctx->lib2->Kmap_init(_ctx, 4);
    for (i = 0; i < 10; ++i) {
        kmape_t *entry = _ctx->lib2->Kmap_newentry(_ctx, map, i);
        assert(entry->hcode == i);
        entry->ukey = i*2;
        entry->uvalue = i;
        _ctx->lib2->Kmap_add(map, entry);
    }
    for (i = 0; i < 10; ++i) {
        kmape_t *entry = _ctx->lib2->Kmap_get(map, i);
        assert(entry != NULL);
        assert(entry->uvalue == i);
    }
    _ctx->lib2->Kmap_reftrace(_ctx, map, reftrace);
    fprintf(stderr, "%d\n", _sum_);
    assert(_sum_ == 45);

    for (i = 0; i < 10; i+=2) {
        kmape_t *entry = _ctx->lib2->Kmap_get(map, i);
        assert(entry != NULL);
        _ctx->lib2->Kmap_remove(map, entry);
    }
    for (i = 0; i < 10; i+=2) {
        kmape_t *entry = _ctx->lib2->Kmap_get(map, i);
        assert(entry == NULL);
    }
    for (i = 0; i < 10; ++i) {
        kmape_t *entry = _ctx->lib2->Kmap_get(map, i);
        if (i % 2 == 0) {
            assert(entry == NULL);
        } else {
            assert(entry->uvalue == i);
        }
    }
    _ctx->lib2->Kmap_free(_ctx, map, reftrace2);
    assert(_sum2_ == 25);
    fprintf(stderr, "%d\n", _sum2_);
    _sum_ = _sum2_ = 0;
}

int main(int argc, const char *argv[])
{
    konoha_ginit(argc, argv);
    konoha_t konoha = konoha_open();
    int i;
    for (i = 0; i < 100; ++i) {
        test_Kmap(konoha);
    }
    konoha_close(konoha);
    klib2_check_malloced_size();
    return 0;
}
