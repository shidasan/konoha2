#include "konoha2/konoha2.h"
#include "../src/gc/gc_api.h"

int main(int argc, const char *argv[])
{
    klib2_t lib;
    klib2_init(&lib);
    int i;
    void *malloced[100];
    for (i = 0; i < 100; ++i) {
        malloced[i] = lib.Kmalloc(0, i);
    }
    for (i = 0; i < 100; ++i) {
        lib.Kfree(0, malloced[i], i);
    }
    klib2_check_malloced_size();
    for (i = 0; i < 100; ++i) {
        malloced[i] = lib.Kzmalloc(0, i);
        int j;
        char *p = malloced[i];
        for (j = 0; j < i; ++j) {
            assert(p[0] == 0);
        }
    }
    for (i = 0; i < 100; ++i) {
        lib.Kfree(0, malloced[i], i);
    }
    klib2_check_malloced_size();
    return 0;
}
