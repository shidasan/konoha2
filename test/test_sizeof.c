#include "konoha2/konoha2.h"

int main(int argc, const char *argv[])
{
    assert(sizeof(kObject) <= 64);
    assert(sizeof(knh_Fmethod) == sizeof(void*));
    return 0;
}
