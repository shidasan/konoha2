#ifndef TYNY_VM_H
#define TYNY_VM_H

struct vmop {
    unsigned opcode:6;
    unsigned flags:2;
    signed p0:8;
    signed p1:8;
    signed p2:8;
    signed p3:8;
    union {
        void *data;
        kMethod *mtd;
    };
};

#endif /* end of include guard */
