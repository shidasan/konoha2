#include "konoha2/konoha2.h"
#include "konoha2/gc.h"

void test_kString(CTX)
{
    intptr_t i;
    kString *s;
    for (i = 0; i < 100; ++i) {
        s = _ctx->lib2->Knew_String(_ctx, "abcd", 4, 0);
        assert(strcmp(S_text(s), "abcd") == 0);
        assert(S_size(s) == 4);
        assert(S_isASCII(s) == 1);
    }
    for (i = 0; i < 100; ++i) {
        s = _ctx->lib2->Knew_String(_ctx, "abcd", 4, 0);
        assert(strcmp(S_text(s), "abcd") == 0);
        assert(S_size(s) == 4);
        assert(S_text(s) == (char*)&s->hashCode);
    }
    for (i = 0; i < 100; ++i) {
        static const char *text = "12345678901234567890";
        s = _ctx->lib2->Knew_String(_ctx, text, 20, SPOL_TEXT | SPOL_UTF8);
        assert(strcmp(S_text(s), text) == 0);
        assert(S_size(s) == 20);
        assert(S_text(s) == text);
        assert(S_isASCII(s) == 0);
    }
}

int main(int argc, const char *argv[])
{
    konoha_t konoha = konoha_open();
    int i;
    for (i = 0; i < 100; ++i) {
        test_kString(konoha);
    }
    konoha_close(konoha);
    MODGC_check_malloced_size();
    return 0;
}

