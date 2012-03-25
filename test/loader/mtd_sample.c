#include "mod_sample.h"

#define MTD0(m)                         void m##_
#define MTD1(m,T1)                      void m##_##T1
#define MTD2(m,T1,T2)                   void m##_##T1##T2
#define MTD3(m,T1,T2,T3)                void m##_##T1##T2##T3
#define MTD4(m,T1,T2,T3,T4)             void m##_##T1##T2##T3##T4
#define MTD5(m,T1,T2,T3,T4,T5)          void m##_##T1##T2##T3##T4##T5
#define MTD6(m,T1,T2,T3,T4,T5,T6)       void m##_##T1##T2##T3##T4##T5##T6
#define MTD7(m,T1,T2,T3,T4,T5,T6,T7)    void m##_##T1##T2##T3##T4##T5##T6##T7
#define MTD8(m,T1,T2,T3,T4,T5,T6,T7,T8) void m##_##T1##T2##T3##T4##T5##T6##T7##T8

#ifdef __cplusplus
extern "C" {
#endif


//## Sample.f(int, int);
MTD2(Sample_f, int, int) (CTX, ksfp_t *sfp, long _rix)
{
    fprintf(stderr, "bar\n");
}

//## Sample.f(int, int);
void Sample_f(CTX, ksfp_t *sfp, long _rix)
{
    fprintf(stderr, "foo\n");
}

#ifdef __cplusplus
}
#endif
