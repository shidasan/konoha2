#include "konoha2/konoha2.h"

int main(int argc, const char *argv[])
{
    assert(sizeof(kObject) <= 64);
    assert(sizeof(knh_Fmethod) == sizeof(void*));
    assert(sizeof(float) <= sizeof(void*));
    assert(sizeof(kint_t) == sizeof(kfloat_t));
    assert(sizeof(kint_t) == sizeof(void*));
    assert(sizeof(ksfp_t) == sizeof(krbp_t) * 2);
    assert(sizeof(krbp_t) == sizeof(void*));
    assert(sizeof(intptr_t) == sizeof(kint_t));
    assert(sizeof(kshort_t) * 2 == sizeof(intptr_t));
    fprintf(stderr, "%ld\n", sizeof(kObject));
    return 0;
}
