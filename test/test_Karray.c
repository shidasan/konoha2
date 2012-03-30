#include "konoha2/konoha2.h"
#include "konoha2/gc.h"

void test_Karray(CTX)
{
    intptr_t i;
    karray_t a;
    _ctx->lib2->Karray_init(_ctx, &a, 4, sizeof(intptr_t));
    for (i = 0; i < 10; ++i) {
        if (a.size == a.max) {
            _ctx->lib2->Karray_expand(_ctx, &a, a.size+1, sizeof(intptr_t));
        }
        ((int*)a.body)[i] = i;
        a.size += 1;
    }
    for (i = 0; i < 10; ++i) {
        assert (i < a.size);
        assert(((int*)a.body)[i] == i);
    }
    _ctx->lib2->Karray_free(_ctx, &a, sizeof(intptr_t));
}

int main(int argc, const char *argv[])
{
    konoha_ginit(argc, argv);
    konoha_t konoha = konoha_open();
    int i;
    for (i = 0; i < 100; ++i) {
        test_Karray(konoha);
    }
    konoha_close(konoha);
    MODGC_check_malloced_size();
    return 0;
}
