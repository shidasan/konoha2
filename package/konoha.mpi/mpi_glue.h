#ifndef MPIKONOHA_H
#define MPIKONOHA_H

#include <mpi.h>

typedef struct {
	MPI_Comm comm;
} kMPIComm;

enum {
	KMPI_BYTES,
	KMPI_FARRAY,
	KMPI_IARRAY,
};

typedef struct {
	union {
		kBytes *b;
		kArray *a;
		kfloat_t *fa; //FIXME
		kint_t *ia;   //FIXME
	};
	int cid;//FIXME
	MPI_Datatype type;
	size_t offset;
	size_t size; // for fa/ia
} kMPIData;

typedef struct {
	MPI_Request req;
} kMPIRequest;

extern kMPIComm *g_comm_world;

#endif

