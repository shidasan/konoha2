#include "konoha2/konoha2.h"
#include "konoha2/gc.h"

void test_Kwb(CTX)
{
    karray_t a;
    kwb_t wb;
    /* if we use karray as kwb, struct_size should be sizeof(char) */
    _ctx->lib2->Karray_init(_ctx, &a, 4, sizeof(char));
    _ctx->lib2->Kwb_init(&a, &wb);
    _ctx->lib2->Kwb_write(_ctx, &wb, "abcd", 4);
    _ctx->lib2->Kwb_write(_ctx, &wb, "abcd", 4);
    const char *data = _ctx->lib2->Kwb_top(_ctx, &wb, 1);
    assert(strcmp(data, "abcdabcd") == 0);
    _ctx->lib2->Kwb_free(&wb);
    _ctx->lib2->Karray_free(_ctx, &a, sizeof(char));
}

int main(int argc, const char *argv[])
{
    konoha_ginit(argc, argv);
    konoha_t konoha = konoha_open();
    int i;
    for (i = 0; i < 100; ++i) {
        test_Kwb(konoha);
    }
    konoha_close(konoha);
    MODGC_check_malloced_size();
    return 0;
}
