#define BEGIN_EXTERN_C \
	extern "C" {

#define END_EXTERN_C \
	}

#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include <unistd.h>
#include <mpi.h>
#define Int_to(T, a)               ((T)a.ivalue)

/* ------------------------------------------------------------------------ */
//## method int MPI_Abort(MPI_Comm comm, int errorcode);
static KMETHOD k_MPI_Abort(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Abort(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Accumulate(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win);
static KMETHOD k_MPI_Accumulate(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	MPI_Aint p4 = sfp[4].ivalue;
	int p5 = sfp[5].ivalue;
	MPI_Datatype p6 = sfp[6].ivalue;
	MPI_Op p7 = sfp[7].ivalue;
	MPI_Win p8 = sfp[8].ivalue;
	RETURNi_(MPI_Accumulate(p0, p1, p2, p3, p4, p5, p6, p7, p8));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Add_error_class(int *errorclass);
static KMETHOD k_MPI_Add_error_class(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Add_error_class(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Add_error_code(int errorclass, int *errorcode);
static KMETHOD k_MPI_Add_error_code(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Add_error_code(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Add_error_string(int errorcode, char *string);
static KMETHOD k_MPI_Add_error_string(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	RETURNi_(MPI_Add_error_string(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Address(void *location, MPI_Aint *address);
static KMETHOD k_MPI_Address(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	MPI_Aint* p1 = (MPI_Aint*)sfp[1].p;
	RETURNi_(MPI_Address(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
static KMETHOD k_MPI_Allgather(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	void* p3 = (void*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	MPI_Datatype p5 = sfp[5].ivalue;
	MPI_Comm p6 = sfp[6].ivalue;
	RETURNi_(MPI_Allgather(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Allgatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcount, int *displs, MPI_Datatype recvtype, MPI_Comm comm);
static KMETHOD k_MPI_Allgatherv(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	void* p3 = (void*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	int* p5 = (int*)sfp[5].p;
	MPI_Datatype p6 = sfp[6].ivalue;
	MPI_Comm p7 = sfp[7].ivalue;
	RETURNi_(MPI_Allgatherv(p0, p1, p2, p3, p4, p5, p6, p7));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Alloc_mem(MPI_Aint size, MPI_Info info, void *baseptr);
static KMETHOD k_MPI_Alloc_mem(CTX, ksfp_t *sfp _RIX)
{
	MPI_Aint p0 = sfp[0].ivalue;
	MPI_Info p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	RETURNi_(MPI_Alloc_mem(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Allreduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
static KMETHOD k_MPI_Allreduce(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Op p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	RETURNi_(MPI_Allreduce(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Alltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
static KMETHOD k_MPI_Alltoall(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	void* p3 = (void*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	MPI_Datatype p5 = sfp[5].ivalue;
	MPI_Comm p6 = sfp[6].ivalue;
	RETURNi_(MPI_Alltoall(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Alltoallv(void *sendbuf, int *sendcounts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
static KMETHOD k_MPI_Alltoallv(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	MPI_Datatype p3 = sfp[3].ivalue;
	void* p4 = (void*)sfp[4].p;
	int* p5 = (int*)sfp[5].p;
	int* p6 = (int*)sfp[6].p;
	MPI_Datatype p7 = sfp[7].ivalue;
	MPI_Comm p8 = sfp[8].ivalue;
	RETURNi_(MPI_Alltoallv(p0, p1, p2, p3, p4, p5, p6, p7, p8));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Alltoallw(void *sendbuf, int *sendcounts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcounts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm);
static KMETHOD k_MPI_Alltoallw(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	MPI_Datatype* p3 = (MPI_Datatype*)sfp[3].p;
	void* p4 = (void*)sfp[4].p;
	int* p5 = (int*)sfp[5].p;
	int* p6 = (int*)sfp[6].p;
	MPI_Datatype* p7 = (MPI_Datatype*)sfp[7].p;
	MPI_Comm p8 = sfp[8].ivalue;
	RETURNi_(MPI_Alltoallw(p0, p1, p2, p3, p4, p5, p6, p7, p8));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Attr_delete(MPI_Comm comm, int keyval);
static KMETHOD k_MPI_Attr_delete(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Attr_delete(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Attr_put(MPI_Comm comm, int keyval, void *attribute_val);
static KMETHOD k_MPI_Attr_put(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	RETURNi_(MPI_Attr_put(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Barrier(MPI_Comm comm);
static KMETHOD k_MPI_Barrier(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	RETURNi_(MPI_Barrier(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
static KMETHOD k_MPI_Bcast(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	MPI_Comm p4 = sfp[4].ivalue;
	RETURNi_(MPI_Bcast(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Bsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
static KMETHOD k_MPI_Bsend(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	RETURNi_(MPI_Bsend(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Bsend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Bsend_init(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Bsend_init(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Buffer_attach(void *buf, int size);
static KMETHOD k_MPI_Buffer_attach(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Buffer_attach(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Buffer_detach(void *buf, int *size);
static KMETHOD k_MPI_Buffer_detach(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Buffer_detach(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cancel(MPI_Request *request);
static KMETHOD k_MPI_Cancel(CTX, ksfp_t *sfp _RIX)
{
	MPI_Request* p0 = (MPI_Request*)sfp[0].p;
	RETURNi_(MPI_Cancel(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int *coords);
static KMETHOD k_MPI_Cart_coords(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	int* p3 = (int*)sfp[3].p;
	RETURNi_(MPI_Cart_coords(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cart_create(MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart);
static KMETHOD k_MPI_Cart_create(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	MPI_Comm* p5 = (MPI_Comm*)sfp[5].p;
	RETURNi_(MPI_Cart_create(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cart_get(MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords);
static KMETHOD k_MPI_Cart_get(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	RETURNi_(MPI_Cart_get(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cart_map(MPI_Comm comm, int ndims, int *dims, int *periods, int *newrank);
static KMETHOD k_MPI_Cart_map(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	RETURNi_(MPI_Cart_map(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cart_rank(MPI_Comm comm, int *coords, int *rank);
static KMETHOD k_MPI_Cart_rank(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Cart_rank(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cart_shift(MPI_Comm comm, int direction, int disp, int *rank_source, int *rank_dest);
static KMETHOD k_MPI_Cart_shift(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	int* p3 = (int*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	RETURNi_(MPI_Cart_shift(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cart_sub(MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new);
static KMETHOD k_MPI_Cart_sub(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	MPI_Comm* p2 = (MPI_Comm*)sfp[2].p;
	RETURNi_(MPI_Cart_sub(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Cartdim_get(MPI_Comm comm, int *ndims);
static KMETHOD k_MPI_Cartdim_get(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Cartdim_get(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Close_port(char *port_name);
static KMETHOD k_MPI_Close_port(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	RETURNi_(MPI_Close_port(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_accept(char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm);
static KMETHOD k_MPI_Comm_accept(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	MPI_Info p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	MPI_Comm p3 = sfp[3].ivalue;
	MPI_Comm* p4 = (MPI_Comm*)sfp[4].p;
	RETURNi_(MPI_Comm_accept(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Comm MPI_Comm_f2c(MPI_Fint comm);
static KMETHOD k_MPI_Comm_f2c(CTX, ksfp_t *sfp _RIX)
{
	MPI_Fint p0 = sfp[0].ivalue;
	RETURNi_(MPI_Comm_f2c(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Fint MPI_Comm_c2f(MPI_Comm comm);
static KMETHOD k_MPI_Comm_c2f(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	RETURNi_(MPI_Comm_c2f(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_File MPI_File_f2c(MPI_Fint file);
static KMETHOD k_MPI_File_f2c(CTX, ksfp_t *sfp _RIX)
{
	MPI_Fint p0 = sfp[0].ivalue;
	RETURN_(MPI_File_f2c(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Fint MPI_File_c2f(MPI_File file);
static KMETHOD k_MPI_File_c2f(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	RETURNi_(MPI_File_c2f(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Group MPI_Group_f2c(MPI Fint group);
static KMETHOD k_MPI_Group_f2c(CTX, ksfp_t *sfp _RIX)
{
	MPI_Fint p0 = sfp[0].ivalue;
	RETURNi_(MPI_Group_f2c(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Fint MPI_Group_c2f(MPI Group group);
static KMETHOD k_MPI_Group_c2f(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	RETURNi_(MPI_Group_c2f(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Info MPI_Info_f2c(MPI_Fint info);
static KMETHOD k_MPI_Info_f2c(CTX, ksfp_t *sfp _RIX)
{
	MPI_Fint p0 = sfp[0].ivalue;
	RETURNi_(MPI_Info_f2c(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Fint MPI_Info_c2f(MPI_Info info);
static KMETHOD k_MPI_Info_c2f(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	RETURNi_(MPI_Info_c2f(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Op MPI_Op_f2c(MPI_Fint op);
static KMETHOD k_MPI_Op_f2c(CTX, ksfp_t *sfp _RIX)
{
	MPI_Fint p0 = sfp[0].ivalue;
	RETURNi_(MPI_Op_f2c(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Fint MPI_Op_c2f(MPI_Op op);
static KMETHOD k_MPI_Op_c2f(CTX, ksfp_t *sfp _RIX)
{
	MPI_Op p0 = sfp[0].ivalue;
	RETURNi_(MPI_Op_c2f(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Request MPI_Request_f2c(MPI_Fint request);
static KMETHOD k_MPI_Request_f2c(CTX, ksfp_t *sfp _RIX)
{
	MPI_Fint p0 = sfp[0].ivalue;
	RETURNi_(MPI_Request_f2c(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Fint MPI_Request_c2f(MPI_Request request);
static KMETHOD k_MPI_Request_c2f(CTX, ksfp_t *sfp _RIX)
{
	MPI_Request p0 = sfp[0].ivalue;
	RETURNi_(MPI_Request_c2f(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Datatype MPI_Type_f2c(MPI_Fint datatype);
static KMETHOD k_MPI_Type_f2c(CTX, ksfp_t *sfp _RIX)
{
	MPI_Fint p0 = sfp[0].ivalue;
	RETURNi_(MPI_Type_f2c(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Fint MPI_Type_c2f(MPI_Datatype datatype);
static KMETHOD k_MPI_Type_c2f(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	RETURNi_(MPI_Type_c2f(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Win MPI_Win_f2c(MPI_Fint win);
static KMETHOD k_MPI_Win_f2c(CTX, ksfp_t *sfp _RIX)
{
	MPI_Fint p0 = sfp[0].ivalue;
	RETURNi_(MPI_Win_f2c(p0));
}

/* ------------------------------------------------------------------------ */
//## method MPI_Fint MPI_Win_c2f(MPI_Win win);
static KMETHOD k_MPI_Win_c2f(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	RETURNi_(MPI_Win_c2f(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_call_errhandler(MPI_Comm comm, int errorcode);
static KMETHOD k_MPI_Comm_call_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Comm_call_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result);
static KMETHOD k_MPI_Comm_compare(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Comm p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Comm_compare(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_connect(char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm);
static KMETHOD k_MPI_Comm_connect(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	MPI_Info p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	MPI_Comm p3 = sfp[3].ivalue;
	MPI_Comm* p4 = (MPI_Comm*)sfp[4].p;
	RETURNi_(MPI_Comm_connect(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
static KMETHOD k_MPI_Comm_create(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Group p1 = sfp[1].ivalue;
	MPI_Comm* p2 = (MPI_Comm*)sfp[2].p;
	RETURNi_(MPI_Comm_create(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_create_errhandler(MPI_Comm_errhandler_function *function, MPI_Errhandler *errhandler);
static KMETHOD k_MPI_Comm_create_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm_errhandler_function* p0 = (MPI_Comm_errhandler_function*)sfp[0].p;
	MPI_Errhandler* p1 = (MPI_Errhandler*)sfp[1].p;
	RETURNi_(MPI_Comm_create_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state);
static KMETHOD k_MPI_Comm_create_keyval(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm_copy_attr_function* p0 = (MPI_Comm_copy_attr_function*)sfp[0].p;
	MPI_Comm_delete_attr_function* p1 = (MPI_Comm_delete_attr_function*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	void* p3 = (void*)sfp[3].p;
	RETURNi_(MPI_Comm_create_keyval(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_delete_attr(MPI_Comm comm, int comm_keyval);
static KMETHOD k_MPI_Comm_delete_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Comm_delete_attr(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_disconnect(MPI_Comm *comm);
static KMETHOD k_MPI_Comm_disconnect(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm* p0 = (MPI_Comm*)sfp[0].p;
	RETURNi_(MPI_Comm_disconnect(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm);
static KMETHOD k_MPI_Comm_dup(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Comm* p1 = (MPI_Comm*)sfp[1].p;
	RETURNi_(MPI_Comm_dup(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_free(MPI_Comm *comm);
static KMETHOD k_MPI_Comm_free(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm* p0 = (MPI_Comm*)sfp[0].p;
	RETURNi_(MPI_Comm_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_free_keyval(int *comm_keyval);
static KMETHOD k_MPI_Comm_free_keyval(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Comm_free_keyval(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_get_attr(MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag);
static KMETHOD k_MPI_Comm_get_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	RETURNi_(MPI_Comm_get_attr(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_get_errhandler(MPI_Comm comm, MPI_Errhandler *errhandler);
static KMETHOD k_MPI_Comm_get_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Errhandler* p1 = (MPI_Errhandler*)sfp[1].p;
	RETURNi_(MPI_Comm_get_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_get_name(MPI_Comm comm, char *comm_name, int *resultlen);
static KMETHOD k_MPI_Comm_get_name(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Comm_get_name(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_get_parent(MPI_Comm *parent);
static KMETHOD k_MPI_Comm_get_parent(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm* p0 = (MPI_Comm*)sfp[0].p;
	RETURNi_(MPI_Comm_get_parent(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_group(MPI_Comm comm, MPI_Group *group);
static KMETHOD k_MPI_Comm_group(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Group* p1 = (MPI_Group*)sfp[1].p;
	RETURNi_(MPI_Comm_group(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_join(int fd, MPI_Comm *intercomm);
static KMETHOD k_MPI_Comm_join(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Comm* p1 = (MPI_Comm*)sfp[1].p;
	RETURNi_(MPI_Comm_join(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_rank(MPI_Comm comm, int *rank);
static KMETHOD k_MPI_Comm_rank(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Comm_rank(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_remote_group(MPI_Comm comm, MPI_Group *group);
static KMETHOD k_MPI_Comm_remote_group(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Group* p1 = (MPI_Group*)sfp[1].p;
	RETURNi_(MPI_Comm_remote_group(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_remote_size(MPI_Comm comm, int *size);
static KMETHOD k_MPI_Comm_remote_size(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Comm_remote_size(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_set_attr(MPI_Comm comm, int comm_keyval, void *attribute_val);
static KMETHOD k_MPI_Comm_set_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	RETURNi_(MPI_Comm_set_attr(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_set_errhandler(MPI_Comm comm, MPI_Errhandler errhandler);
static KMETHOD k_MPI_Comm_set_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Errhandler p1 = sfp[1].ivalue;
	RETURNi_(MPI_Comm_set_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_set_name(MPI_Comm comm, char *comm_name);
static KMETHOD k_MPI_Comm_set_name(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	RETURNi_(MPI_Comm_set_name(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_size(MPI_Comm comm, int *size);
static KMETHOD k_MPI_Comm_size(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Comm_size(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_spawn(char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
static KMETHOD k_MPI_Comm_spawn(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	char** p1 = (char**)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Info p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Comm* p6 = (MPI_Comm*)sfp[6].p;
	int* p7 = (int*)sfp[7].p;
	RETURNi_(MPI_Comm_spawn(p0, p1, p2, p3, p4, p5, p6, p7));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
static KMETHOD k_MPI_Comm_spawn_multiple(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	char** p1 = (char**)sfp[1].p;
	char*** p2 = (char***)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	MPI_Info* p4 = (MPI_Info*)sfp[4].p;
	int p5 = sfp[5].ivalue;
	MPI_Comm p6 = sfp[6].ivalue;
	MPI_Comm* p7 = (MPI_Comm*)sfp[7].p;
	int* p8 = (int*)sfp[8].p;
	RETURNi_(MPI_Comm_spawn_multiple(p0, p1, p2, p3, p4, p5, p6, p7, p8));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
static KMETHOD k_MPI_Comm_split(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	MPI_Comm* p3 = (MPI_Comm*)sfp[3].p;
	RETURNi_(MPI_Comm_split(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Comm_test_inter(MPI_Comm comm, int *flag);
static KMETHOD k_MPI_Comm_test_inter(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Comm_test_inter(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Dims_create(int nnodes, int ndims, int *dims);
static KMETHOD k_MPI_Dims_create(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Dims_create(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Errhandler_create(MPI_Handler_function *function, MPI_Errhandler *errhandler);
static KMETHOD k_MPI_Errhandler_create(CTX, ksfp_t *sfp _RIX)
{
	MPI_Handler_function* p0 = (MPI_Handler_function*)sfp[0].p;
	MPI_Errhandler* p1 = (MPI_Errhandler*)sfp[1].p;
	RETURNi_(MPI_Errhandler_create(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Errhandler_free(MPI_Errhandler *errhandler);
static KMETHOD k_MPI_Errhandler_free(CTX, ksfp_t *sfp _RIX)
{
	MPI_Errhandler* p0 = (MPI_Errhandler*)sfp[0].p;
	RETURNi_(MPI_Errhandler_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Errhandler_get(MPI_Comm comm, MPI_Errhandler *errhandler);
static KMETHOD k_MPI_Errhandler_get(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Errhandler* p1 = (MPI_Errhandler*)sfp[1].p;
	RETURNi_(MPI_Errhandler_get(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Errhandler_set(MPI_Comm comm, MPI_Errhandler errhandler);
static KMETHOD k_MPI_Errhandler_set(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	MPI_Errhandler p1 = sfp[1].ivalue;
	RETURNi_(MPI_Errhandler_set(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Error_class(int errorcode, int *errorclass);
static KMETHOD k_MPI_Error_class(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Error_class(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Error_string(int errorcode, char *string, int *resultlen);
static KMETHOD k_MPI_Error_string(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Error_string(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Exscan(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
static KMETHOD k_MPI_Exscan(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Op p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	RETURNi_(MPI_Exscan(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_call_errhandler(MPI_File fh, int errorcode);
static KMETHOD k_MPI_File_call_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_File_call_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_close(MPI_File *fh);
static KMETHOD k_MPI_File_close(CTX, ksfp_t *sfp _RIX)
{
	MPI_File* p0 = (MPI_File*)sfp[0].p;
	RETURNi_(MPI_File_close(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_create_errhandler(MPI_File_errhandler_function *function, MPI_Errhandler *errhandler);
static KMETHOD k_MPI_File_create_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_File_errhandler_function* p0 = (MPI_File_errhandler_function*)sfp[0].p;
	MPI_Errhandler* p1 = (MPI_Errhandler*)sfp[1].p;
	RETURNi_(MPI_File_create_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_delete(char *filename, MPI_Info info);
static KMETHOD k_MPI_File_delete(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	MPI_Info p1 = sfp[1].ivalue;
	RETURNi_(MPI_File_delete(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_amode(MPI_File fh, int *amode);
static KMETHOD k_MPI_File_get_amode(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_File_get_amode(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_atomicity(MPI_File fh, int *flag);
static KMETHOD k_MPI_File_get_atomicity(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_File_get_atomicity(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_byte_offset(MPI_File fh, MPI_Offset offset, MPI_Offset *disp);
static KMETHOD k_MPI_File_get_byte_offset(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	MPI_Offset* p2 = (MPI_Offset*)sfp[2].p;
	RETURNi_(MPI_File_get_byte_offset(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_errhandler(MPI_File file, MPI_Errhandler *errhandler);
static KMETHOD k_MPI_File_get_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Errhandler* p1 = (MPI_Errhandler*)sfp[1].p;
	RETURNi_(MPI_File_get_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_group(MPI_File fh, MPI_Group *group);
static KMETHOD k_MPI_File_get_group(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Group* p1 = (MPI_Group*)sfp[1].p;
	RETURNi_(MPI_File_get_group(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_info(MPI_File fh, MPI_Info *info_used);
static KMETHOD k_MPI_File_get_info(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Info* p1 = (MPI_Info*)sfp[1].p;
	RETURNi_(MPI_File_get_info(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_position(MPI_File fh, MPI_Offset *offset);
static KMETHOD k_MPI_File_get_position(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset* p1 = (MPI_Offset*)sfp[1].p;
	RETURNi_(MPI_File_get_position(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_position_shared(MPI_File fh, MPI_Offset *offset);
static KMETHOD k_MPI_File_get_position_shared(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset* p1 = (MPI_Offset*)sfp[1].p;
	RETURNi_(MPI_File_get_position_shared(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_size(MPI_File fh, MPI_Offset *size);
static KMETHOD k_MPI_File_get_size(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset* p1 = (MPI_Offset*)sfp[1].p;
	RETURNi_(MPI_File_get_size(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_type_extent(MPI_File fh, MPI_Datatype datatype, MPI_Aint *extent);
static KMETHOD k_MPI_File_get_type_extent(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Datatype p1 = sfp[1].ivalue;
	MPI_Aint* p2 = (MPI_Aint*)sfp[2].p;
	RETURNi_(MPI_File_get_type_extent(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_get_view(MPI_File fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep);
static KMETHOD k_MPI_File_get_view(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset* p1 = (MPI_Offset*)sfp[1].p;
	MPI_Datatype* p2 = (MPI_Datatype*)sfp[2].p;
	MPI_Datatype* p3 = (MPI_Datatype*)sfp[3].p;
	char* p4 = (char*)sfp[4].p;
	RETURNi_(MPI_File_get_view(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_iread(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static KMETHOD k_MPI_File_iread(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Request* p4 = (MPI_Request*)sfp[4].p;
	RETURNi_(MPI_File_iread(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_iread_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static KMETHOD k_MPI_File_iread_at(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int p3 = sfp[3].ivalue;
	MPI_Datatype p4 = sfp[4].ivalue;
	MPI_Request* p5 = (MPI_Request*)sfp[5].p;
	RETURNi_(MPI_File_iread_at(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_iread_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static KMETHOD k_MPI_File_iread_shared(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Request* p4 = (MPI_Request*)sfp[4].p;
	RETURNi_(MPI_File_iread_shared(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_iwrite(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static KMETHOD k_MPI_File_iwrite(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Request* p4 = (MPI_Request*)sfp[4].p;
	RETURNi_(MPI_File_iwrite(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_iwrite_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static KMETHOD k_MPI_File_iwrite_at(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int p3 = sfp[3].ivalue;
	MPI_Datatype p4 = sfp[4].ivalue;
	MPI_Request* p5 = (MPI_Request*)sfp[5].p;
	RETURNi_(MPI_File_iwrite_at(p0, p1, p2, p3, p4, p5));
}

///* ------------------------------------------------------------------------ */
////## method int MPI_File_(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
//static KMETHOD k_MPI_File_(CTX, ksfp_t *sfp _RIX)
//{
//	MPI_File p0 = (MPI_File)sfp[0].p;
//	void* p1 = (void*)sfp[1].p;
//	int p2 = sfp[2].ivalue;
//	MPI_Datatype p3 = sfp[3].ivalue;
//	MPI_Request* p4 = (MPI_Request*)sfp[4].p;
//	RETURNi_(MPI_File_(p0, p1, p2, p3, p4));
//}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_open(MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh);
static KMETHOD k_MPI_File_open(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Info p3 = sfp[3].ivalue;
	MPI_File* p4 = (MPI_File*)sfp[4].p;
	RETURNi_(MPI_File_open(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_preallocate(MPI_File fh, MPI_Offset size);
static KMETHOD k_MPI_File_preallocate(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	RETURNi_(MPI_File_preallocate(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_read(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_File_read(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_read_all(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_File_read_all(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_all_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype);
static KMETHOD k_MPI_File_read_all_begin(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	RETURNi_(MPI_File_read_all_begin(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_all_end(MPI_File fh, void *buf, MPI_Status *status);
static KMETHOD k_MPI_File_read_all_end(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_File_read_all_end(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_read_at(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int p3 = sfp[3].ivalue;
	MPI_Datatype p4 = sfp[4].ivalue;
	MPI_Status* p5 = (MPI_Status*)sfp[5].p;
	RETURNi_(MPI_File_read_at(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_read_at_all(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int p3 = sfp[3].ivalue;
	MPI_Datatype p4 = sfp[4].ivalue;
	MPI_Status* p5 = (MPI_Status*)sfp[5].p;
	RETURNi_(MPI_File_read_at_all(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_at_all_begin(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype);
static KMETHOD k_MPI_File_read_at_all_begin(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int p3 = sfp[3].ivalue;
	MPI_Datatype p4 = sfp[4].ivalue;
	RETURNi_(MPI_File_read_at_all_begin(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_at_all_end(MPI_File fh, void *buf, MPI_Status *status);
static KMETHOD k_MPI_File_read_at_all_end(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_File_read_at_all_end(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_ordered(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_read_ordered(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_File_read_ordered(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_ordered_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype);
static KMETHOD k_MPI_File_read_ordered_begin(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	RETURNi_(MPI_File_read_ordered_begin(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_ordered_end(MPI_File fh, void *buf, MPI_Status *status);
static KMETHOD k_MPI_File_read_ordered_end(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_File_read_ordered_end(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_read_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_read_shared(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_File_read_shared(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_seek(MPI_File fh, MPI_Offset offset, int whence);
static KMETHOD k_MPI_File_seek(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	RETURNi_(MPI_File_seek(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_seek_shared(MPI_File fh, MPI_Offset offset, int whence);
static KMETHOD k_MPI_File_seek_shared(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	RETURNi_(MPI_File_seek_shared(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_set_atomicity(MPI_File fh, int flag);
static KMETHOD k_MPI_File_set_atomicity(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_File_set_atomicity(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_set_errhandler(MPI_File file, MPI_Errhandler errhandler);
static KMETHOD k_MPI_File_set_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Errhandler p1 = sfp[1].ivalue;
	RETURNi_(MPI_File_set_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_set_info(MPI_File fh, MPI_Info info);
static KMETHOD k_MPI_File_set_info(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Info p1 = sfp[1].ivalue;
	RETURNi_(MPI_File_set_info(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_set_size(MPI_File fh, MPI_Offset size);
static KMETHOD k_MPI_File_set_size(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	RETURNi_(MPI_File_set_size(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info);
static KMETHOD k_MPI_File_set_view(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	char* p4 = (char*)sfp[4].p;
	MPI_Info p5 = sfp[5].ivalue;
	RETURNi_(MPI_File_set_view(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_sync(MPI_File fh);
static KMETHOD k_MPI_File_sync(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	RETURNi_(MPI_File_sync(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_write(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_File_write(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_write_all(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_File_write_all(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_all_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype);
static KMETHOD k_MPI_File_write_all_begin(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	RETURNi_(MPI_File_write_all_begin(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_all_end(MPI_File fh, void *buf, MPI_Status *status);
static KMETHOD k_MPI_File_write_all_end(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_File_write_all_end(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_write_at(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int p3 = sfp[3].ivalue;
	MPI_Datatype p4 = sfp[4].ivalue;
	MPI_Status* p5 = (MPI_Status*)sfp[5].p;
	RETURNi_(MPI_File_write_at(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_write_at_all(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int p3 = sfp[3].ivalue;
	MPI_Datatype p4 = sfp[4].ivalue;
	MPI_Status* p5 = (MPI_Status*)sfp[5].p;
	RETURNi_(MPI_File_write_at_all(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_at_all_begin(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype);
static KMETHOD k_MPI_File_write_at_all_begin(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	MPI_Offset p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int p3 = sfp[3].ivalue;
	MPI_Datatype p4 = sfp[4].ivalue;
	RETURNi_(MPI_File_write_at_all_begin(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_at_all_end(MPI_File fh, void *buf, MPI_Status *status);
static KMETHOD k_MPI_File_write_at_all_end(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_File_write_at_all_end(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_ordered(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_write_ordered(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_File_write_ordered(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_ordered_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype);
static KMETHOD k_MPI_File_write_ordered_begin(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	RETURNi_(MPI_File_write_ordered_begin(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_ordered_end(MPI_File fh, void *buf, MPI_Status *status);
static KMETHOD k_MPI_File_write_ordered_end(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_File_write_ordered_end(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_File_write_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static KMETHOD k_MPI_File_write_shared(CTX, ksfp_t *sfp _RIX)
{
	MPI_File p0 = (MPI_File)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_File_write_shared(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Finalize();
static KMETHOD k_MPI_Finalize(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(MPI_Finalize());
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Finalized(int *flag);
static KMETHOD k_MPI_Finalized(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Finalized(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Free_mem(void *base);
static KMETHOD k_MPI_Free_mem(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	RETURNi_(MPI_Free_mem(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
static KMETHOD k_MPI_Gather(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	void* p3 = (void*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	MPI_Datatype p5 = sfp[5].ivalue;
	int p6 = sfp[6].ivalue;
	MPI_Comm p7 = sfp[7].ivalue;
	RETURNi_(MPI_Gather(p0, p1, p2, p3, p4, p5, p6, p7));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Gatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
static KMETHOD k_MPI_Gatherv(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	void* p3 = (void*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	int* p5 = (int*)sfp[5].p;
	MPI_Datatype p6 = sfp[6].ivalue;
	int p7 = sfp[7].ivalue;
	MPI_Comm p8 = sfp[8].ivalue;
	RETURNi_(MPI_Gatherv(p0, p1, p2, p3, p4, p5, p6, p7, p8));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Get_address(void *location, MPI_Aint *address);
static KMETHOD k_MPI_Get_address(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	MPI_Aint* p1 = (MPI_Aint*)sfp[1].p;
	RETURNi_(MPI_Get_address(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count);
static KMETHOD k_MPI_Get_count(CTX, ksfp_t *sfp _RIX)
{
	MPI_Status* p0 = (MPI_Status*)sfp[0].p;
	MPI_Datatype p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Get_count(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Get_elements(MPI_Status *status, MPI_Datatype datatype, int *count);
static KMETHOD k_MPI_Get_elements(CTX, ksfp_t *sfp _RIX)
{
	MPI_Status* p0 = (MPI_Status*)sfp[0].p;
	MPI_Datatype p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Get_elements(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Get_processor_name(char *name, int *resultlen);
static KMETHOD k_MPI_Get_processor_name(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Get_processor_name(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Get_version(int *version, int *subversion);
static KMETHOD k_MPI_Get_version(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Get_version(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Graph_create(MPI_Comm comm_old, int nnodes, int *index, int *edges, int reorder, MPI_Comm *comm_graph);
static KMETHOD k_MPI_Graph_create(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	MPI_Comm* p5 = (MPI_Comm*)sfp[5].p;
	RETURNi_(MPI_Graph_create(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Graph_get(MPI_Comm comm, int maxindex, int maxedges, int *index, int *edges);
static KMETHOD k_MPI_Graph_get(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	int* p3 = (int*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	RETURNi_(MPI_Graph_get(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Graph_map(MPI_Comm comm, int nnodes, int *index, int *edges, int *newrank);
static KMETHOD k_MPI_Graph_map(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	RETURNi_(MPI_Graph_map(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Graph_neighbors(MPI_Comm comm, int rank, int maxneighbors, int *neighbors);
static KMETHOD k_MPI_Graph_neighbors(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	int* p3 = (int*)sfp[3].p;
	RETURNi_(MPI_Graph_neighbors(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Graph_neighbors_count(MPI_Comm comm, int rank, int *nneighbors);
static KMETHOD k_MPI_Graph_neighbors_count(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Graph_neighbors_count(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Graphdims_get(MPI_Comm comm, int *nnodes, int *nedges);
static KMETHOD k_MPI_Graphdims_get(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Graphdims_get(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Grequest_complete(MPI_Request request);
static KMETHOD k_MPI_Grequest_complete(CTX, ksfp_t *sfp _RIX)
{
	MPI_Request p0 = sfp[0].ivalue;
	RETURNi_(MPI_Grequest_complete(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Grequest_start(MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *request);
static KMETHOD k_MPI_Grequest_start(CTX, ksfp_t *sfp _RIX)
{
	MPI_Grequest_query_function* p0 = (MPI_Grequest_query_function*)sfp[0].p;
	MPI_Grequest_free_function* p1 = (MPI_Grequest_free_function*)sfp[1].p;
	MPI_Grequest_cancel_function* p2 = (MPI_Grequest_cancel_function*)sfp[2].p;
	void* p3 = (void*)sfp[3].p;
	MPI_Request* p4 = (MPI_Request*)sfp[4].p;
	RETURNi_(MPI_Grequest_start(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_compare(MPI_Group group1, MPI_Group group2, int *result);
static KMETHOD k_MPI_Group_compare(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	MPI_Group p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Group_compare(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_difference(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
static KMETHOD k_MPI_Group_difference(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	MPI_Group p1 = sfp[1].ivalue;
	MPI_Group* p2 = (MPI_Group*)sfp[2].p;
	RETURNi_(MPI_Group_difference(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_excl(MPI_Group group, int n, int *ranks, MPI_Group *newgroup);
static KMETHOD k_MPI_Group_excl(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	MPI_Group* p3 = (MPI_Group*)sfp[3].p;
	RETURNi_(MPI_Group_excl(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_free(MPI_Group *group);
static KMETHOD k_MPI_Group_free(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group* p0 = (MPI_Group*)sfp[0].p;
	RETURNi_(MPI_Group_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_incl(MPI_Group group, int n, int *ranks, MPI_Group *newgroup);
static KMETHOD k_MPI_Group_incl(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	MPI_Group* p3 = (MPI_Group*)sfp[3].p;
	RETURNi_(MPI_Group_incl(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_intersection(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
static KMETHOD k_MPI_Group_intersection(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	MPI_Group p1 = sfp[1].ivalue;
	MPI_Group* p2 = (MPI_Group*)sfp[2].p;
	RETURNi_(MPI_Group_intersection(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_range_excl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup);
static KMETHOD k_MPI_Group_range_excl(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int** p2 = (int**)sfp[2].p;
	MPI_Group* p3 = (MPI_Group*)sfp[3].p;
	RETURNi_(MPI_Group_range_excl(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_range_incl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup);
static KMETHOD k_MPI_Group_range_incl(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int** p2 = (int**)sfp[2].p;
	MPI_Group* p3 = (MPI_Group*)sfp[3].p;
	RETURNi_(MPI_Group_range_incl(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_rank(MPI_Group group, int *rank);
static KMETHOD k_MPI_Group_rank(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Group_rank(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_size(MPI_Group group, int *size);
static KMETHOD k_MPI_Group_size(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Group_size(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_translate_ranks(MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2);
static KMETHOD k_MPI_Group_translate_ranks(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	MPI_Group p3 = sfp[3].ivalue;
	int* p4 = (int*)sfp[4].p;
	RETURNi_(MPI_Group_translate_ranks(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
static KMETHOD k_MPI_Group_union(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	MPI_Group p1 = sfp[1].ivalue;
	MPI_Group* p2 = (MPI_Group*)sfp[2].p;
	RETURNi_(MPI_Group_union(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Ibsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Ibsend(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Ibsend(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_create(MPI_Info *info);
static KMETHOD k_MPI_Info_create(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info* p0 = (MPI_Info*)sfp[0].p;
	RETURNi_(MPI_Info_create(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_delete(MPI_Info info, char *key);
static KMETHOD k_MPI_Info_delete(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	RETURNi_(MPI_Info_delete(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_dup(MPI_Info info, MPI_Info *newinfo);
static KMETHOD k_MPI_Info_dup(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	MPI_Info* p1 = (MPI_Info*)sfp[1].p;
	RETURNi_(MPI_Info_dup(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_free(MPI_Info *info);
static KMETHOD k_MPI_Info_free(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info* p0 = (MPI_Info*)sfp[0].p;
	RETURNi_(MPI_Info_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_get(MPI_Info info, char *key, int valuelen, char *value, int *flag);
static KMETHOD k_MPI_Info_get(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	char* p3 = (char*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	RETURNi_(MPI_Info_get(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_get_nkeys(MPI_Info info, int *nkeys);
static KMETHOD k_MPI_Info_get_nkeys(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Info_get_nkeys(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_get_nthkey(MPI_Info info, int n, char *key);
static KMETHOD k_MPI_Info_get_nthkey(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	char* p2 = (char*)sfp[2].p;
	RETURNi_(MPI_Info_get_nthkey(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_get_valuelen(MPI_Info info, char *key, int *valuelen, int *flag);
static KMETHOD k_MPI_Info_get_valuelen(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	RETURNi_(MPI_Info_get_valuelen(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Info_set(MPI_Info info, char *key, char *value);
static KMETHOD k_MPI_Info_set(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	char* p2 = (char*)sfp[2].p;
	RETURNi_(MPI_Info_set(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Init(int *argc, char ***argv);
static KMETHOD k_MPI_Init(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	char*** p1 = (char***)sfp[1].p;
	RETURNi_(MPI_Init(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Init_thread(int *argc, char ***argv, int required, int *provided);
static KMETHOD k_MPI_Init_thread(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	char*** p1 = (char***)sfp[1].p;
	int p2 = sfp[2].ivalue;
	int* p3 = (int*)sfp[3].p;
	RETURNi_(MPI_Init_thread(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Initialized(int *flag);
static KMETHOD k_MPI_Initialized(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Initialized(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
static KMETHOD k_MPI_Intercomm_create(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	MPI_Comm p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm* p5 = (MPI_Comm*)sfp[5].p;
	RETURNi_(MPI_Intercomm_create(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
static KMETHOD k_MPI_Iprobe(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	MPI_Comm p2 = sfp[2].ivalue;
	int* p3 = (int*)sfp[3].p;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_Iprobe(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Irecv(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Irecv(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Irsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Irsend(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Irsend(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Is_thread_main(int *flag);
static KMETHOD k_MPI_Is_thread_main(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Is_thread_main(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Isend(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Isend(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Issend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Issend(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Issend(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Keyval_create(MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state);
static KMETHOD k_MPI_Keyval_create(CTX, ksfp_t *sfp _RIX)
{
	MPI_Copy_function* p0 = (MPI_Copy_function*)sfp[0].p;
	MPI_Delete_function* p1 = (MPI_Delete_function*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	void* p3 = (void*)sfp[3].p;
	RETURNi_(MPI_Keyval_create(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Keyval_free(int *keyval);
static KMETHOD k_MPI_Keyval_free(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Keyval_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Lookup_name(char *service_name, MPI_Info info, char *port_name);
static KMETHOD k_MPI_Lookup_name(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	MPI_Info p1 = sfp[1].ivalue;
	char* p2 = (char*)sfp[2].p;
	RETURNi_(MPI_Lookup_name(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Op_create(MPI_User_function *function, int commute, MPI_Op *op);
static KMETHOD k_MPI_Op_create(CTX, ksfp_t *sfp _RIX)
{
	MPI_User_function* p0 = (MPI_User_function*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Op* p2 = (MPI_Op*)sfp[2].p;
	RETURNi_(MPI_Op_create(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Op_free(MPI_Op *op);
static KMETHOD k_MPI_Op_free(CTX, ksfp_t *sfp _RIX)
{
	MPI_Op* p0 = (MPI_Op*)sfp[0].p;
	RETURNi_(MPI_Op_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Open_port(MPI_Info info, char *port_name);
static KMETHOD k_MPI_Open_port(CTX, ksfp_t *sfp _RIX)
{
	MPI_Info p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	RETURNi_(MPI_Open_port(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Pack(void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outsize, int *position, MPI_Comm comm);
static KMETHOD k_MPI_Pack(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	void* p3 = (void*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	int* p5 = (int*)sfp[5].p;
	MPI_Comm p6 = sfp[6].ivalue;
	RETURNi_(MPI_Pack(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Pack_external(char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outsize, MPI_Aint *position);
static KMETHOD k_MPI_Pack_external(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	void* p4 = (void*)sfp[4].p;
	MPI_Aint p5 = sfp[5].ivalue;
	MPI_Aint* p6 = (MPI_Aint*)sfp[6].p;
	RETURNi_(MPI_Pack_external(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Pack_external_size(char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size);
static KMETHOD k_MPI_Pack_external_size(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	MPI_Aint* p3 = (MPI_Aint*)sfp[3].p;
	RETURNi_(MPI_Pack_external_size(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, int *size);
static KMETHOD k_MPI_Pack_size(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Datatype p1 = sfp[1].ivalue;
	MPI_Comm p2 = sfp[2].ivalue;
	int* p3 = (int*)sfp[3].p;
	RETURNi_(MPI_Pack_size(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status);
static KMETHOD k_MPI_Probe(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	MPI_Comm p2 = sfp[2].ivalue;
	MPI_Status* p3 = (MPI_Status*)sfp[3].p;
	RETURNi_(MPI_Probe(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Publish_name(char *service_name, MPI_Info info, char *port_name);
static KMETHOD k_MPI_Publish_name(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	MPI_Info p1 = sfp[1].ivalue;
	char* p2 = (char*)sfp[2].p;
	RETURNi_(MPI_Publish_name(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Query_thread(int *provided);
static KMETHOD k_MPI_Query_thread(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Query_thread(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
static KMETHOD k_MPI_Recv(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Status* p6 = (MPI_Status*)sfp[6].p;
	RETURNi_(MPI_Recv(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Recv_init(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Recv_init(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Reduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
static KMETHOD k_MPI_Reduce(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Op p4 = sfp[4].ivalue;
	int p5 = sfp[5].ivalue;
	MPI_Comm p6 = sfp[6].ivalue;
	RETURNi_(MPI_Reduce(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Reduce_local(void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op);
static KMETHOD k_MPI_Reduce_local(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Op p4 = sfp[4].ivalue;
	RETURNi_(MPI_Reduce_local(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Reduce_scatter(void *sendbuf, void *recvbuf, int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
static KMETHOD k_MPI_Reduce_scatter(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Op p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	RETURNi_(MPI_Reduce_scatter(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Register_datarep(char *datarep, MPI_Datarep_conversion_function *read_conversion_fn, MPI_Datarep_conversion_function *write_conversion_fn, MPI_Datarep_extent_function *dtype_file_extent_fn, void *extra_state);
static KMETHOD k_MPI_Register_datarep(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	MPI_Datarep_conversion_function* p1 = (MPI_Datarep_conversion_function*)sfp[1].p;
	MPI_Datarep_conversion_function* p2 = (MPI_Datarep_conversion_function*)sfp[2].p;
	MPI_Datarep_extent_function* p3 = (MPI_Datarep_extent_function*)sfp[3].p;
	void* p4 = (void*)sfp[4].p;
	RETURNi_(MPI_Register_datarep(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Request_free(MPI_Request *request);
static KMETHOD k_MPI_Request_free(CTX, ksfp_t *sfp _RIX)
{
	MPI_Request* p0 = (MPI_Request*)sfp[0].p;
	RETURNi_(MPI_Request_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status);
static KMETHOD k_MPI_Request_get_status(CTX, ksfp_t *sfp _RIX)
{
	MPI_Request p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_Request_get_status(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Rsend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
static KMETHOD k_MPI_Rsend(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	RETURNi_(MPI_Rsend(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Rsend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Rsend_init(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Rsend_init(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Scan(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
static KMETHOD k_MPI_Scan(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Op p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	RETURNi_(MPI_Scan(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
static KMETHOD k_MPI_Scatter(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	void* p3 = (void*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	MPI_Datatype p5 = sfp[5].ivalue;
	int p6 = sfp[6].ivalue;
	MPI_Comm p7 = sfp[7].ivalue;
	RETURNi_(MPI_Scatter(p0, p1, p2, p3, p4, p5, p6, p7));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Scatterv(void *sendbuf, int *sendcounts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
static KMETHOD k_MPI_Scatterv(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	MPI_Datatype p3 = sfp[3].ivalue;
	void* p4 = (void*)sfp[4].p;
	int p5 = sfp[5].ivalue;
	MPI_Datatype p6 = sfp[6].ivalue;
	int p7 = sfp[7].ivalue;
	MPI_Comm p8 = sfp[8].ivalue;
	RETURNi_(MPI_Scatterv(p0, p1, p2, p3, p4, p5, p6, p7, p8));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
static KMETHOD k_MPI_Send(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	RETURNi_(MPI_Send(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Send_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Send_init(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Send_init(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Sendrecv(void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
static KMETHOD k_MPI_Sendrecv(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	void* p5 = (void*)sfp[5].p;
	int p6 = sfp[6].ivalue;
	MPI_Datatype p7 = sfp[7].ivalue;
	int p8 = sfp[8].ivalue;
	int p9 = sfp[9].ivalue;
	MPI_Comm p10 = sfp[10].ivalue;
	MPI_Status* p11 = (MPI_Status*)sfp[11].p;
	RETURNi_(MPI_Sendrecv(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
static KMETHOD k_MPI_Sendrecv_replace(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	int p5 = sfp[5].ivalue;
	int p6 = sfp[6].ivalue;
	MPI_Comm p7 = sfp[7].ivalue;
	MPI_Status* p8 = (MPI_Status*)sfp[8].p;
	RETURNi_(MPI_Sendrecv_replace(p0, p1, p2, p3, p4, p5, p6, p7, p8));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Ssend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
static KMETHOD k_MPI_Ssend(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	RETURNi_(MPI_Ssend(p0, p1, p2, p3, p4, p5));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Ssend_init(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
static KMETHOD k_MPI_Ssend_init(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	MPI_Datatype p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int p4 = sfp[4].ivalue;
	MPI_Comm p5 = sfp[5].ivalue;
	MPI_Request* p6 = (MPI_Request*)sfp[6].p;
	RETURNi_(MPI_Ssend_init(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Start(MPI_Request *request);
static KMETHOD k_MPI_Start(CTX, ksfp_t *sfp _RIX)
{
	MPI_Request* p0 = (MPI_Request*)sfp[0].p;
	RETURNi_(MPI_Start(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Startall(int count, MPI_Request *array_of_requests);
static KMETHOD k_MPI_Startall(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Request* p1 = (MPI_Request*)sfp[1].p;
	RETURNi_(MPI_Startall(p0, p1));
}

///* ------------------------------------------------------------------------ */
////## method int MPI_Status_f2c(MPI_Fint *f_status, MPI_Status *c_status);
//static KMETHOD k_MPI_Status_f2c(CTX, ksfp_t *sfp _RIX)
//{
//	MPI_Fint* p0 = (MPI_Fint*)sfp[0].p;
//	MPI_Status* p1 = (MPI_Status*)sfp[1].p;
//	RETURNi_(MPI_Status_f2c(p0, p1));
//}

///* ------------------------------------------------------------------------ */
////## method int MPI_Status_c2f(MPI_Status *c_status, MPI_Fint *f_status);
//static KMETHOD k_MPI_Status_c2f(CTX, ksfp_t *sfp _RIX)
//{
//	MPI_Status* p0 = (MPI_Status*)sfp[0].p;
//	MPI_Fint* p1 = (MPI_Fint*)sfp[1].p;
//	RETURNi_(MPI_Status_c2f(p0, p1));
//}

/* ------------------------------------------------------------------------ */
//## method int MPI_Status_set_cancelled(MPI_Status *status, int flag);
static KMETHOD k_MPI_Status_set_cancelled(CTX, ksfp_t *sfp _RIX)
{
	MPI_Status* p0 = (MPI_Status*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Status_set_cancelled(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Status_set_elements(MPI_Status *status, MPI_Datatype datatype, int count);
static KMETHOD k_MPI_Status_set_elements(CTX, ksfp_t *sfp _RIX)
{
	MPI_Status* p0 = (MPI_Status*)sfp[0].p;
	MPI_Datatype p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	RETURNi_(MPI_Status_set_elements(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status);
static KMETHOD k_MPI_Test(CTX, ksfp_t *sfp _RIX)
{
	MPI_Request* p0 = (MPI_Request*)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_Test(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Test_cancelled(MPI_Status *status, int *flag);
static KMETHOD k_MPI_Test_cancelled(CTX, ksfp_t *sfp _RIX)
{
	MPI_Status* p0 = (MPI_Status*)sfp[0].p;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Test_cancelled(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Testall(int count, MPI_Request *array_of_requests, int *flag, MPI_Status *array_of_statuses);
static KMETHOD k_MPI_Testall(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Request* p1 = (MPI_Request*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	MPI_Status* p3 = (MPI_Status*)sfp[3].p;
	RETURNi_(MPI_Testall(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Testany(int count, MPI_Request *array_of_requests, int *index, int *flag, MPI_Status *status);
static KMETHOD k_MPI_Testany(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Request* p1 = (MPI_Request*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_Testany(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Testsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
static KMETHOD k_MPI_Testsome(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Request* p1 = (MPI_Request*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_Testsome(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Topo_test(MPI_Comm comm, int *top_type);
static KMETHOD k_MPI_Topo_test(CTX, ksfp_t *sfp _RIX)
{
	MPI_Comm p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Topo_test(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_commit(MPI_Datatype *datatype);
static KMETHOD k_MPI_Type_commit(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype* p0 = (MPI_Datatype*)sfp[0].p;
	RETURNi_(MPI_Type_commit(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_contiguous(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Datatype p1 = sfp[1].ivalue;
	MPI_Datatype* p2 = (MPI_Datatype*)sfp[2].p;
	RETURNi_(MPI_Type_contiguous(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_create_darray(int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_create_darray(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	int* p3 = (int*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	int* p5 = (int*)sfp[5].p;
	int* p6 = (int*)sfp[6].p;
	int p7 = sfp[7].ivalue;
	MPI_Datatype p8 = sfp[8].ivalue;
	MPI_Datatype* p9 = (MPI_Datatype*)sfp[9].p;
	RETURNi_(MPI_Type_create_darray(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9));
}

///* ------------------------------------------------------------------------ */
////## method int MPI_Type_create_f90_complex(int p, int r, MPI_Datatype *newtype);
//static KMETHOD k_MPI_Type_create_f90_complex(CTX, ksfp_t *sfp _RIX)
//{
//	int p0 = sfp[0].ivalue;
//	int p1 = sfp[1].ivalue;
//	MPI_Datatype* p2 = (MPI_Datatype*)sfp[2].p;
//	RETURNi_(MPI_Type_create_f90_complex(p0, p1, p2));
//}

///* ------------------------------------------------------------------------ */
////## method int MPI_Type_create_f90_integer(int r, MPI_Datatype *newtype);
//static KMETHOD k_MPI_Type_create_f90_integer(CTX, ksfp_t *sfp _RIX)
//{
//	int p0 = sfp[0].ivalue;
//	MPI_Datatype* p1 = (MPI_Datatype*)sfp[1].p;
//	RETURNi_(MPI_Type_create_f90_integer(p0, p1));
//}

///* ------------------------------------------------------------------------ */
////## method int MPI_Type_create_f90_real(int p, int r, MPI_Datatype *newtype);
//static KMETHOD k_MPI_Type_create_f90_real(CTX, ksfp_t *sfp _RIX)
//{
//	int p0 = sfp[0].ivalue;
//	int p1 = sfp[1].ivalue;
//	MPI_Datatype* p2 = (MPI_Datatype*)sfp[2].p;
//	RETURNi_(MPI_Type_create_f90_real(p0, p1, p2));
//}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_create_hindexed(int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_create_hindexed(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	MPI_Aint* p2 = (MPI_Aint*)sfp[2].p;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_create_hindexed(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_create_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_create_hvector(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	MPI_Aint p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_create_hvector(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_create_indexed_block(int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_create_indexed_block(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_create_indexed_block(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state);
static KMETHOD k_MPI_Type_create_keyval(CTX, ksfp_t *sfp _RIX)
{
	MPI_Type_copy_attr_function* p0 = (MPI_Type_copy_attr_function*)sfp[0].p;
	MPI_Type_delete_attr_function* p1 = (MPI_Type_delete_attr_function*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	void* p3 = (void*)sfp[3].p;
	RETURNi_(MPI_Type_create_keyval(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_create_resized(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	MPI_Aint p1 = sfp[1].ivalue;
	MPI_Aint p2 = sfp[2].ivalue;
	MPI_Datatype* p3 = (MPI_Datatype*)sfp[3].p;
	RETURNi_(MPI_Type_create_resized(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_create_struct(int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_create_struct(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	MPI_Aint* p2 = (MPI_Aint*)sfp[2].p;
	MPI_Datatype* p3 = (MPI_Datatype*)sfp[3].p;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_create_struct(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_create_subarray(int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_create_subarray(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	MPI_Datatype p5 = sfp[5].ivalue;
	MPI_Datatype* p6 = (MPI_Datatype*)sfp[6].p;
	RETURNi_(MPI_Type_create_subarray(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_delete_attr(MPI_Datatype type, int type_keyval);
static KMETHOD k_MPI_Type_delete_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Type_delete_attr(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_dup(MPI_Datatype type, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_dup(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	MPI_Datatype* p1 = (MPI_Datatype*)sfp[1].p;
	RETURNi_(MPI_Type_dup(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_extent(MPI_Datatype datatype, MPI_Aint *extent);
static KMETHOD k_MPI_Type_extent(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	MPI_Aint* p1 = (MPI_Aint*)sfp[1].p;
	RETURNi_(MPI_Type_extent(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_free(MPI_Datatype *datatype);
static KMETHOD k_MPI_Type_free(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype* p0 = (MPI_Datatype*)sfp[0].p;
	RETURNi_(MPI_Type_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_free_keyval(int *type_keyval);
static KMETHOD k_MPI_Type_free_keyval(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Type_free_keyval(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_get_attr(MPI_Datatype type, int type_keyval, void *attribute_val, int *flag);
static KMETHOD k_MPI_Type_get_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	RETURNi_(MPI_Type_get_attr(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_get_contents(MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[]);
static KMETHOD k_MPI_Type_get_contents(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	int p3 = sfp[3].ivalue;
	int* p4 = (int*)sfp[4].p;
	MPI_Aint* p5 = (MPI_Aint*)sfp[5].p;
	MPI_Datatype* p6 = (MPI_Datatype*)sfp[6].p;
	RETURNi_(MPI_Type_get_contents(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_get_envelope(MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner);
static KMETHOD k_MPI_Type_get_envelope(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	int* p4 = (int*)sfp[4].p;
	RETURNi_(MPI_Type_get_envelope(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_get_extent(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent);
static KMETHOD k_MPI_Type_get_extent(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	MPI_Aint* p1 = (MPI_Aint*)sfp[1].p;
	MPI_Aint* p2 = (MPI_Aint*)sfp[2].p;
	RETURNi_(MPI_Type_get_extent(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_get_name(MPI_Datatype type, char *type_name, int *resultlen);
static KMETHOD k_MPI_Type_get_name(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Type_get_name(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_get_true_extent(MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent);
static KMETHOD k_MPI_Type_get_true_extent(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	MPI_Aint* p1 = (MPI_Aint*)sfp[1].p;
	MPI_Aint* p2 = (MPI_Aint*)sfp[2].p;
	RETURNi_(MPI_Type_get_true_extent(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_hindexed(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_hindexed(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	MPI_Aint* p2 = (MPI_Aint*)sfp[2].p;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_hindexed(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_hvector(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	MPI_Aint p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_hvector(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_indexed(int count, int *array_of_blocklengths, int *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_indexed(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_indexed(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_lb(MPI_Datatype datatype, MPI_Aint *displacement);
static KMETHOD k_MPI_Type_lb(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	MPI_Aint* p1 = (MPI_Aint*)sfp[1].p;
	RETURNi_(MPI_Type_lb(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_match_size(int typeclass, int size, MPI_Datatype *type);
static KMETHOD k_MPI_Type_match_size(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	MPI_Datatype* p2 = (MPI_Datatype*)sfp[2].p;
	RETURNi_(MPI_Type_match_size(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_set_attr(MPI_Datatype type, int type_keyval, void *attribute_val);
static KMETHOD k_MPI_Type_set_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	RETURNi_(MPI_Type_set_attr(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_set_name(MPI_Datatype type, char *type_name);
static KMETHOD k_MPI_Type_set_name(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	RETURNi_(MPI_Type_set_name(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_size(MPI_Datatype datatype, int *size);
static KMETHOD k_MPI_Type_size(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Type_size(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_struct(int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype *array_of_types, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_struct(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	MPI_Aint* p2 = (MPI_Aint*)sfp[2].p;
	MPI_Datatype* p3 = (MPI_Datatype*)sfp[3].p;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_struct(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_ub(MPI_Datatype datatype, MPI_Aint *displacement);
static KMETHOD k_MPI_Type_ub(CTX, ksfp_t *sfp _RIX)
{
	MPI_Datatype p0 = sfp[0].ivalue;
	MPI_Aint* p1 = (MPI_Aint*)sfp[1].p;
	RETURNi_(MPI_Type_ub(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
static KMETHOD k_MPI_Type_vector(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	MPI_Datatype p3 = sfp[3].ivalue;
	MPI_Datatype* p4 = (MPI_Datatype*)sfp[4].p;
	RETURNi_(MPI_Type_vector(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Unpack(void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm);
static KMETHOD k_MPI_Unpack(CTX, ksfp_t *sfp _RIX)
{
	void* p0 = (void*)sfp[0].p;
	int p1 = sfp[1].ivalue;
	int* p2 = (int*)sfp[2].p;
	void* p3 = (void*)sfp[3].p;
	int p4 = sfp[4].ivalue;
	MPI_Datatype p5 = sfp[5].ivalue;
	MPI_Comm p6 = sfp[6].ivalue;
	RETURNi_(MPI_Unpack(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Unpack_external(char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype);
static KMETHOD k_MPI_Unpack_external(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	void* p1 = (void*)sfp[1].p;
	MPI_Aint p2 = sfp[2].ivalue;
	MPI_Aint* p3 = (MPI_Aint*)sfp[3].p;
	void* p4 = (void*)sfp[4].p;
	int p5 = sfp[5].ivalue;
	MPI_Datatype p6 = sfp[6].ivalue;
	RETURNi_(MPI_Unpack_external(p0, p1, p2, p3, p4, p5, p6));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Unpublish_name(char *service_name, MPI_Info info, char *port_name);
static KMETHOD k_MPI_Unpublish_name(CTX, ksfp_t *sfp _RIX)
{
	char* p0 = (char*)sfp[0].p;
	MPI_Info p1 = sfp[1].ivalue;
	char* p2 = (char*)sfp[2].p;
	RETURNi_(MPI_Unpublish_name(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Wait(MPI_Request *request, MPI_Status *status);
static KMETHOD k_MPI_Wait(CTX, ksfp_t *sfp _RIX)
{
	MPI_Request* p0 = (MPI_Request*)sfp[0].p;
	MPI_Status* p1 = (MPI_Status*)sfp[1].p;
	RETURNi_(MPI_Wait(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses);
static KMETHOD k_MPI_Waitall(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Request* p1 = (MPI_Request*)sfp[1].p;
	MPI_Status* p2 = (MPI_Status*)sfp[2].p;
	RETURNi_(MPI_Waitall(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Waitany(int count, MPI_Request *array_of_requests, int *index, MPI_Status *status);
static KMETHOD k_MPI_Waitany(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Request* p1 = (MPI_Request*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	MPI_Status* p3 = (MPI_Status*)sfp[3].p;
	RETURNi_(MPI_Waitany(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, int *array_of_indices, MPI_Status *array_of_statuses);
static KMETHOD k_MPI_Waitsome(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Request* p1 = (MPI_Request*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	MPI_Status* p4 = (MPI_Status*)sfp[4].p;
	RETURNi_(MPI_Waitsome(p0, p1, p2, p3, p4));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_call_errhandler(MPI_Win win, int errorcode);
static KMETHOD k_MPI_Win_call_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Win_call_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_create_errhandler(MPI_Win_errhandler_function *function, MPI_Errhandler *errhandler);
static KMETHOD k_MPI_Win_create_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win_errhandler_function* p0 = (MPI_Win_errhandler_function*)sfp[0].p;
	MPI_Errhandler* p1 = (MPI_Errhandler*)sfp[1].p;
	RETURNi_(MPI_Win_create_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state);
static KMETHOD k_MPI_Win_create_keyval(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win_copy_attr_function* p0 = (MPI_Win_copy_attr_function*)sfp[0].p;
	MPI_Win_delete_attr_function* p1 = (MPI_Win_delete_attr_function*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	void* p3 = (void*)sfp[3].p;
	RETURNi_(MPI_Win_create_keyval(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_delete_attr(MPI_Win win, int win_keyval);
static KMETHOD k_MPI_Win_delete_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	RETURNi_(MPI_Win_delete_attr(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_fence(int assert, MPI_Win win);
static KMETHOD k_MPI_Win_fence(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Win p1 = sfp[1].ivalue;
	RETURNi_(MPI_Win_fence(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_free(MPI_Win *win);
static KMETHOD k_MPI_Win_free(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win* p0 = (MPI_Win*)sfp[0].p;
	RETURNi_(MPI_Win_free(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_free_keyval(int *win_keyval);
static KMETHOD k_MPI_Win_free_keyval(CTX, ksfp_t *sfp _RIX)
{
	int* p0 = (int*)sfp[0].p;
	RETURNi_(MPI_Win_free_keyval(p0));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_get_attr(MPI_Win win, int win_keyval, void *attribute_val, int *flag);
static KMETHOD k_MPI_Win_get_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	int* p3 = (int*)sfp[3].p;
	RETURNi_(MPI_Win_get_attr(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_get_errhandler(MPI_Win win, MPI_Errhandler *errhandler);
static KMETHOD k_MPI_Win_get_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	MPI_Errhandler* p1 = (MPI_Errhandler*)sfp[1].p;
	RETURNi_(MPI_Win_get_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_get_name(MPI_Win win, char *win_name, int *resultlen);
static KMETHOD k_MPI_Win_get_name(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	int* p2 = (int*)sfp[2].p;
	RETURNi_(MPI_Win_get_name(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win);
static KMETHOD k_MPI_Win_lock(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	int p2 = sfp[2].ivalue;
	MPI_Win p3 = sfp[3].ivalue;
	RETURNi_(MPI_Win_lock(p0, p1, p2, p3));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_post(MPI_Group group, int assert, MPI_Win win);
static KMETHOD k_MPI_Win_post(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	MPI_Win p2 = sfp[2].ivalue;
	RETURNi_(MPI_Win_post(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_set_attr(MPI_Win win, int win_keyval, void *attribute_val);
static KMETHOD k_MPI_Win_set_attr(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	void* p2 = (void*)sfp[2].p;
	RETURNi_(MPI_Win_set_attr(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_set_errhandler(MPI_Win win, MPI_Errhandler errhandler);
static KMETHOD k_MPI_Win_set_errhandler(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	MPI_Errhandler p1 = sfp[1].ivalue;
	RETURNi_(MPI_Win_set_errhandler(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_set_name(MPI_Win win, char *win_name);
static KMETHOD k_MPI_Win_set_name(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	char* p1 = (char*)sfp[1].p;
	RETURNi_(MPI_Win_set_name(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_start(MPI_Group group, int assert, MPI_Win win);
static KMETHOD k_MPI_Win_start(CTX, ksfp_t *sfp _RIX)
{
	MPI_Group p0 = sfp[0].ivalue;
	int p1 = sfp[1].ivalue;
	MPI_Win p2 = sfp[2].ivalue;
	RETURNi_(MPI_Win_start(p0, p1, p2));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_test(MPI_Win win, int *flag);
static KMETHOD k_MPI_Win_test(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	int* p1 = (int*)sfp[1].p;
	RETURNi_(MPI_Win_test(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_unlock(int rank, MPI_Win win);
static KMETHOD k_MPI_Win_unlock(CTX, ksfp_t *sfp _RIX)
{
	int p0 = sfp[0].ivalue;
	MPI_Win p1 = sfp[1].ivalue;
	RETURNi_(MPI_Win_unlock(p0, p1));
}

/* ------------------------------------------------------------------------ */
//## method int MPI_Win_wait(MPI_Win win);
static KMETHOD k_MPI_Win_wait(CTX, ksfp_t *sfp _RIX)
{
	MPI_Win p0 = sfp[0].ivalue;
	RETURNi_(MPI_Win_wait(p0));
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Static   kMethod_Static
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

static kbool_t mpi_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	int init = 0;
	MPI_Initialized(&init);
	if (init) {
		MPI_Errhandler errfn;
		//MPI_Comm_create_errhandler(knh_MPI_errhandler, &errfn);
		MPI_Errhandler_set(MPI_COMM_WORLD, errfn);
	} else {
		DBG_P("process is not initialized for MPI: MPI functions are NOT available");
	}
	//knh_MPI_initArrayFuncData(_ctx);
	//knh_MPI_initArrayPrintFunc(_ctx);

	static KDEFINE_CLASS MPIDef = {
		"MPI"/*structname*/,
		CLASS_newid/*cid*/,  0/*cflag*/,
		0/*bcid*/, 0/*supcid*/, 0/*cstruct_size*/,
		NULL/*fields*/, 0/*fsize*/, 0/*fallocsize*/,
		0/*packid*/, 0/*packdom*/,
		0/*init*/,
		0/*reftrace*/,
		0/*free*/,
		0/*fnull*/,
		0/*p*/, 0/*unbox*/,
		0/*compareTo*/,
		0/*getkey*/,
		0/*hashCode*/,
		0/*initdef*/
	};
	const kclass_t *CT_MPI = kaddClassDef(NULL, &MPIDef, pline);
#define TY_MPI    (CT_MPI->cid)

	intptr_t methoddata[] = {
		_Public|_Static, _F(k_MPI_Abort), TY_Int, TY_MPI, MN_("MPI_Abort"), 2, TY_Int, MN_("comm"), TY_Int, MN_("errorcode"), 
		_Public|_Static, _F(k_MPI_Accumulate), TY_Int, TY_MPI, MN_("MPI_Accumulate"), 9, TY_Array, MN_("origin_addr"), TY_Int, MN_("origin_count"), TY_Int, MN_("origin_datatype"), TY_Int, MN_("target_rank"), TY_Int, MN_("target_disp"), TY_Int, MN_("target_count"), TY_Int, MN_("target_datatype"), TY_Int, MN_("op"), TY_Int, MN_("win"), 
		_Public|_Static, _F(k_MPI_Add_error_class), TY_Int, TY_MPI, MN_("MPI_Add_error_class"), 1, TY_Array, MN_("errorclass"), 
		_Public|_Static, _F(k_MPI_Add_error_code), TY_Int, TY_MPI, MN_("MPI_Add_error_code"), 2, TY_Int, MN_("errorclass"), TY_Array, MN_("errorcode"), 
		_Public|_Static, _F(k_MPI_Add_error_string), TY_Int, TY_MPI, MN_("MPI_Add_error_string"), 2, TY_Int, MN_("errorcode"), TY_Array, MN_("string"), 
		_Public|_Static, _F(k_MPI_Address), TY_Int, TY_MPI, MN_("MPI_Address"), 2, TY_Array, MN_("location"), TY_Array, MN_("address"), 
		_Public|_Static, _F(k_MPI_Allgather), TY_Int, TY_MPI, MN_("MPI_Allgather"), 7, TY_Array, MN_("sendbuf"), TY_Int, MN_("sendcount"), TY_Int, MN_("sendtype"), TY_Array, MN_("recvbuf"), TY_Int, MN_("recvcount"), TY_Int, MN_("recvtype"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Allgatherv), TY_Int, TY_MPI, MN_("MPI_Allgatherv"), 8, TY_Array, MN_("sendbuf"), TY_Int, MN_("sendcount"), TY_Int, MN_("sendtype"), TY_Array, MN_("recvbuf"), TY_Array, MN_("recvcount"), TY_Array, MN_("displs"), TY_Int, MN_("recvtype"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Alloc_mem), TY_Int, TY_MPI, MN_("MPI_Alloc_mem"), 3, TY_Int, MN_("size"), TY_Int, MN_("info"), TY_Array, MN_("baseptr"), 
		_Public|_Static, _F(k_MPI_Allreduce), TY_Int, TY_MPI, MN_("MPI_Allreduce"), 6, TY_Array, MN_("sendbuf"), TY_Array, MN_("recvbuf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("op"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Alltoall), TY_Int, TY_MPI, MN_("MPI_Alltoall"), 7, TY_Array, MN_("sendbuf"), TY_Int, MN_("sendcount"), TY_Int, MN_("sendtype"), TY_Array, MN_("recvbuf"), TY_Int, MN_("recvcount"), TY_Int, MN_("recvtype"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Alltoallv), TY_Int, TY_MPI, MN_("MPI_Alltoallv"), 9, TY_Array, MN_("sendbuf"), TY_Array, MN_("sendcounts"), TY_Array, MN_("sdispls"), TY_Int, MN_("sendtype"), TY_Array, MN_("recvbuf"), TY_Array, MN_("recvcounts"), TY_Array, MN_("rdispls"), TY_Int, MN_("recvtype"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Alltoallw), TY_Int, TY_MPI, MN_("MPI_Alltoallw"), 9, TY_Array, MN_("sendbuf"), TY_Array, MN_("sendcounts"), TY_Array, MN_("sdispls"), TY_Array, MN_("sendtypes"), TY_Array, MN_("recvbuf"), TY_Array, MN_("recvcounts"), TY_Array, MN_("rdispls"), TY_Array, MN_("recvtypes"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Attr_delete), TY_Int, TY_MPI, MN_("MPI_Attr_delete"), 2, TY_Int, MN_("comm"), TY_Int, MN_("keyval"), 
		_Public|_Static, _F(k_MPI_Attr_put), TY_Int, TY_MPI, MN_("MPI_Attr_put"), 3, TY_Int, MN_("comm"), TY_Int, MN_("keyval"), TY_Array, MN_("attribute_val"), 
		_Public|_Static, _F(k_MPI_Barrier), TY_Int, TY_MPI, MN_("MPI_Barrier"), 1, TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Bcast), TY_Int, TY_MPI, MN_("MPI_Bcast"), 5, TY_Array, MN_("buffer"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("root"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Bsend), TY_Int, TY_MPI, MN_("MPI_Bsend"), 6, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Bsend_init), TY_Int, TY_MPI, MN_("MPI_Bsend_init"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Buffer_attach), TY_Int, TY_MPI, MN_("MPI_Buffer_attach"), 2, TY_Array, MN_("buf"), TY_Int, MN_("size"), 
		_Public|_Static, _F(k_MPI_Buffer_detach), TY_Int, TY_MPI, MN_("MPI_Buffer_detach"), 2, TY_Array, MN_("buf"), TY_Array, MN_("size"), 
		_Public|_Static, _F(k_MPI_Cancel), TY_Int, TY_MPI, MN_("MPI_Cancel"), 1, TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Cart_coords), TY_Int, TY_MPI, MN_("MPI_Cart_coords"), 4, TY_Int, MN_("comm"), TY_Int, MN_("rank"), TY_Int, MN_("maxdims"), TY_Array, MN_("coords"), 
		_Public|_Static, _F(k_MPI_Cart_create), TY_Int, TY_MPI, MN_("MPI_Cart_create"), 6, TY_Int, MN_("comm_old"), TY_Int, MN_("ndims"), TY_Array, MN_("dims"), TY_Array, MN_("periods"), TY_Int, MN_("reorder"), TY_Array, MN_("comm_cart"), 
		_Public|_Static, _F(k_MPI_Cart_get), TY_Int, TY_MPI, MN_("MPI_Cart_get"), 5, TY_Int, MN_("comm"), TY_Int, MN_("maxdims"), TY_Array, MN_("dims"), TY_Array, MN_("periods"), TY_Array, MN_("coords"), 
		_Public|_Static, _F(k_MPI_Cart_map), TY_Int, TY_MPI, MN_("MPI_Cart_map"), 5, TY_Int, MN_("comm"), TY_Int, MN_("ndims"), TY_Array, MN_("dims"), TY_Array, MN_("periods"), TY_Array, MN_("newrank"), 
		_Public|_Static, _F(k_MPI_Cart_rank), TY_Int, TY_MPI, MN_("MPI_Cart_rank"), 3, TY_Int, MN_("comm"), TY_Array, MN_("coords"), TY_Array, MN_("rank"), 
		_Public|_Static, _F(k_MPI_Cart_shift), TY_Int, TY_MPI, MN_("MPI_Cart_shift"), 5, TY_Int, MN_("comm"), TY_Int, MN_("direction"), TY_Int, MN_("disp"), TY_Array, MN_("rank_source"), TY_Array, MN_("rank_dest"), 
		_Public|_Static, _F(k_MPI_Cart_sub), TY_Int, TY_MPI, MN_("MPI_Cart_sub"), 3, TY_Int, MN_("comm"), TY_Array, MN_("remain_dims"), TY_Array, MN_("comm_new"), 
		_Public|_Static, _F(k_MPI_Cartdim_get), TY_Int, TY_MPI, MN_("MPI_Cartdim_get"), 2, TY_Int, MN_("comm"), TY_Array, MN_("ndims"), 
		_Public|_Static, _F(k_MPI_Close_port), TY_Int, TY_MPI, MN_("MPI_Close_port"), 1, TY_Array, MN_("port_name"), 
		_Public|_Static, _F(k_MPI_Comm_accept), TY_Int, TY_MPI, MN_("MPI_Comm_accept"), 5, TY_Array, MN_("port_name"), TY_Int, MN_("info"), TY_Int, MN_("root"), TY_Int, MN_("comm"), TY_Array, MN_("newcomm"), 
		_Public|_Static, _F(k_MPI_Comm_f2c), TY_Int, TY_MPI, MN_("MPI_Comm_f2c"), 1, TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Comm_c2f), TY_Int, TY_MPI, MN_("MPI_Comm_c2f"), 1, TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_File_f2c), TY_Object, TY_MPI, MN_("MPI_File_f2c"), 1, TY_Int, MN_("file"), 
		_Public|_Static, _F(k_MPI_File_c2f), TY_Int, TY_MPI, MN_("MPI_File_c2f"), 1, TY_Object, MN_("file"), 
		_Public|_Static, _F(k_MPI_Group_f2c), TY_Int, TY_MPI, MN_("MPI_Group_f2c"), 1, TY_Int, MN_("group"), 
		_Public|_Static, _F(k_MPI_Group_c2f), TY_Int, TY_MPI, MN_("MPI_Group_c2f"), 1, TY_Int, MN_("group"), 
		_Public|_Static, _F(k_MPI_Info_f2c), TY_Int, TY_MPI, MN_("MPI_Info_f2c"), 1, TY_Int, MN_("info"), 
		_Public|_Static, _F(k_MPI_Info_c2f), TY_Int, TY_MPI, MN_("MPI_Info_c2f"), 1, TY_Int, MN_("info"), 
		_Public|_Static, _F(k_MPI_Op_f2c), TY_Int, TY_MPI, MN_("MPI_Op_f2c"), 1, TY_Int, MN_("op"), 
		_Public|_Static, _F(k_MPI_Op_c2f), TY_Int, TY_MPI, MN_("MPI_Op_c2f"), 1, TY_Int, MN_("op"), 
		_Public|_Static, _F(k_MPI_Request_f2c), TY_Int, TY_MPI, MN_("MPI_Request_f2c"), 1, TY_Int, MN_("request"), 
		_Public|_Static, _F(k_MPI_Request_c2f), TY_Int, TY_MPI, MN_("MPI_Request_c2f"), 1, TY_Int, MN_("request"), 
		_Public|_Static, _F(k_MPI_Type_f2c), TY_Int, TY_MPI, MN_("MPI_Type_f2c"), 1, TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_Type_c2f), TY_Int, TY_MPI, MN_("MPI_Type_c2f"), 1, TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_Win_f2c), TY_Int, TY_MPI, MN_("MPI_Win_f2c"), 1, TY_Int, MN_("win"), 
		_Public|_Static, _F(k_MPI_Win_c2f), TY_Int, TY_MPI, MN_("MPI_Win_c2f"), 1, TY_Int, MN_("win"), 
		_Public|_Static, _F(k_MPI_Comm_call_errhandler), TY_Int, TY_MPI, MN_("MPI_Comm_call_errhandler"), 2, TY_Int, MN_("comm"), TY_Int, MN_("errorcode"), 
		_Public|_Static, _F(k_MPI_Comm_compare), TY_Int, TY_MPI, MN_("MPI_Comm_compare"), 3, TY_Int, MN_("comm1"), TY_Int, MN_("comm2"), TY_Array, MN_("result"), 
		_Public|_Static, _F(k_MPI_Comm_connect), TY_Int, TY_MPI, MN_("MPI_Comm_connect"), 5, TY_Array, MN_("port_name"), TY_Int, MN_("info"), TY_Int, MN_("root"), TY_Int, MN_("comm"), TY_Array, MN_("newcomm"), 
		_Public|_Static, _F(k_MPI_Comm_create), TY_Int, TY_MPI, MN_("MPI_Comm_create"), 3, TY_Int, MN_("comm"), TY_Int, MN_("group"), TY_Array, MN_("newcomm"), 
		_Public|_Static, _F(k_MPI_Comm_create_errhandler), TY_Int, TY_MPI, MN_("MPI_Comm_create_errhandler"), 2, TY_Array, MN_("function"), TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Comm_create_keyval), TY_Int, TY_MPI, MN_("MPI_Comm_create_keyval"), 4, TY_Array, MN_("comm_copy_attr_fn"), TY_Array, MN_("comm_delete_attr_fn"), TY_Array, MN_("comm_keyval"), TY_Array, MN_("extra_state"), 
		_Public|_Static, _F(k_MPI_Comm_delete_attr), TY_Int, TY_MPI, MN_("MPI_Comm_delete_attr"), 2, TY_Int, MN_("comm"), TY_Int, MN_("comm_keyval"), 
		_Public|_Static, _F(k_MPI_Comm_disconnect), TY_Int, TY_MPI, MN_("MPI_Comm_disconnect"), 1, TY_Array, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Comm_dup), TY_Int, TY_MPI, MN_("MPI_Comm_dup"), 2, TY_Int, MN_("comm"), TY_Array, MN_("newcomm"), 
		_Public|_Static, _F(k_MPI_Comm_free), TY_Int, TY_MPI, MN_("MPI_Comm_free"), 1, TY_Array, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Comm_free_keyval), TY_Int, TY_MPI, MN_("MPI_Comm_free_keyval"), 1, TY_Array, MN_("comm_keyval"), 
		_Public|_Static, _F(k_MPI_Comm_get_attr), TY_Int, TY_MPI, MN_("MPI_Comm_get_attr"), 4, TY_Int, MN_("comm"), TY_Int, MN_("comm_keyval"), TY_Array, MN_("attribute_val"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Comm_get_errhandler), TY_Int, TY_MPI, MN_("MPI_Comm_get_errhandler"), 2, TY_Int, MN_("comm"), TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Comm_get_name), TY_Int, TY_MPI, MN_("MPI_Comm_get_name"), 3, TY_Int, MN_("comm"), TY_Array, MN_("comm_name"), TY_Array, MN_("resultlen"), 
		_Public|_Static, _F(k_MPI_Comm_get_parent), TY_Int, TY_MPI, MN_("MPI_Comm_get_parent"), 1, TY_Array, MN_("parent"), 
		_Public|_Static, _F(k_MPI_Comm_group), TY_Int, TY_MPI, MN_("MPI_Comm_group"), 2, TY_Int, MN_("comm"), TY_Array, MN_("group"), 
		_Public|_Static, _F(k_MPI_Comm_join), TY_Int, TY_MPI, MN_("MPI_Comm_join"), 2, TY_Int, MN_("fd"), TY_Array, MN_("intercomm"), 
		_Public|_Static, _F(k_MPI_Comm_rank), TY_Int, TY_MPI, MN_("MPI_Comm_rank"), 2, TY_Int, MN_("comm"), TY_Array, MN_("rank"), 
		_Public|_Static, _F(k_MPI_Comm_remote_group), TY_Int, TY_MPI, MN_("MPI_Comm_remote_group"), 2, TY_Int, MN_("comm"), TY_Array, MN_("group"), 
		_Public|_Static, _F(k_MPI_Comm_remote_size), TY_Int, TY_MPI, MN_("MPI_Comm_remote_size"), 2, TY_Int, MN_("comm"), TY_Array, MN_("size"), 
		_Public|_Static, _F(k_MPI_Comm_set_attr), TY_Int, TY_MPI, MN_("MPI_Comm_set_attr"), 3, TY_Int, MN_("comm"), TY_Int, MN_("comm_keyval"), TY_Array, MN_("attribute_val"), 
		_Public|_Static, _F(k_MPI_Comm_set_errhandler), TY_Int, TY_MPI, MN_("MPI_Comm_set_errhandler"), 2, TY_Int, MN_("comm"), TY_Int, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Comm_set_name), TY_Int, TY_MPI, MN_("MPI_Comm_set_name"), 2, TY_Int, MN_("comm"), TY_Array, MN_("comm_name"), 
		_Public|_Static, _F(k_MPI_Comm_size), TY_Int, TY_MPI, MN_("MPI_Comm_size"), 2, TY_Int, MN_("comm"), TY_Array, MN_("size"), 
		_Public|_Static, _F(k_MPI_Comm_spawn), TY_Int, TY_MPI, MN_("MPI_Comm_spawn"), 8, TY_Array, MN_("command"), TY_Array, MN_("argv"), TY_Int, MN_("maxprocs"), TY_Int, MN_("info"), TY_Int, MN_("root"), TY_Int, MN_("comm"), TY_Array, MN_("intercomm"), TY_Array, MN_("array_of_errcodes"), 
		_Public|_Static, _F(k_MPI_Comm_spawn_multiple), TY_Int, TY_MPI, MN_("MPI_Comm_spawn_multiple"), 9, TY_Int, MN_("count"), TY_Array, MN_("array_of_commands"), TY_Array, MN_("array_of_argv"), TY_Array, MN_("array_of_maxprocs"), TY_Array, MN_("array_of_info"), TY_Int, MN_("root"), TY_Int, MN_("comm"), TY_Array, MN_("intercomm"), TY_Array, MN_("array_of_errcodes"), 
		_Public|_Static, _F(k_MPI_Comm_split), TY_Int, TY_MPI, MN_("MPI_Comm_split"), 4, TY_Int, MN_("comm"), TY_Int, MN_("color"), TY_Int, MN_("key"), TY_Array, MN_("newcomm"), 
		_Public|_Static, _F(k_MPI_Comm_test_inter), TY_Int, TY_MPI, MN_("MPI_Comm_test_inter"), 2, TY_Int, MN_("comm"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Dims_create), TY_Int, TY_MPI, MN_("MPI_Dims_create"), 3, TY_Int, MN_("nnodes"), TY_Int, MN_("ndims"), TY_Array, MN_("dims"), 
		_Public|_Static, _F(k_MPI_Errhandler_create), TY_Int, TY_MPI, MN_("MPI_Errhandler_create"), 2, TY_Array, MN_("function"), TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Errhandler_free), TY_Int, TY_MPI, MN_("MPI_Errhandler_free"), 1, TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Errhandler_get), TY_Int, TY_MPI, MN_("MPI_Errhandler_get"), 2, TY_Int, MN_("comm"), TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Errhandler_set), TY_Int, TY_MPI, MN_("MPI_Errhandler_set"), 2, TY_Int, MN_("comm"), TY_Int, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Error_class), TY_Int, TY_MPI, MN_("MPI_Error_class"), 2, TY_Int, MN_("errorcode"), TY_Array, MN_("errorclass"), 
		_Public|_Static, _F(k_MPI_Error_string), TY_Int, TY_MPI, MN_("MPI_Error_string"), 3, TY_Int, MN_("errorcode"), TY_Array, MN_("string"), TY_Array, MN_("resultlen"), 
		_Public|_Static, _F(k_MPI_Exscan), TY_Int, TY_MPI, MN_("MPI_Exscan"), 6, TY_Array, MN_("sendbuf"), TY_Array, MN_("recvbuf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("op"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_File_call_errhandler), TY_Int, TY_MPI, MN_("MPI_File_call_errhandler"), 2, TY_Object, MN_("fh"), TY_Int, MN_("errorcode"), 
		_Public|_Static, _F(k_MPI_File_close), TY_Int, TY_MPI, MN_("MPI_File_close"), 1, TY_Array, MN_("fh"), 
		_Public|_Static, _F(k_MPI_File_create_errhandler), TY_Int, TY_MPI, MN_("MPI_File_create_errhandler"), 2, TY_Array, MN_("function"), TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_File_delete), TY_Int, TY_MPI, MN_("MPI_File_delete"), 2, TY_Array, MN_("filename"), TY_Int, MN_("info"), 
		_Public|_Static, _F(k_MPI_File_get_amode), TY_Int, TY_MPI, MN_("MPI_File_get_amode"), 2, TY_Object, MN_("fh"), TY_Array, MN_("amode"), 
		_Public|_Static, _F(k_MPI_File_get_atomicity), TY_Int, TY_MPI, MN_("MPI_File_get_atomicity"), 2, TY_Object, MN_("fh"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_File_get_byte_offset), TY_Int, TY_MPI, MN_("MPI_File_get_byte_offset"), 3, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("disp"), 
		_Public|_Static, _F(k_MPI_File_get_errhandler), TY_Int, TY_MPI, MN_("MPI_File_get_errhandler"), 2, TY_Object, MN_("file"), TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_File_get_group), TY_Int, TY_MPI, MN_("MPI_File_get_group"), 2, TY_Object, MN_("fh"), TY_Array, MN_("group"), 
		_Public|_Static, _F(k_MPI_File_get_info), TY_Int, TY_MPI, MN_("MPI_File_get_info"), 2, TY_Object, MN_("fh"), TY_Array, MN_("info_used"), 
		_Public|_Static, _F(k_MPI_File_get_position), TY_Int, TY_MPI, MN_("MPI_File_get_position"), 2, TY_Object, MN_("fh"), TY_Array, MN_("offset"), 
		_Public|_Static, _F(k_MPI_File_get_position_shared), TY_Int, TY_MPI, MN_("MPI_File_get_position_shared"), 2, TY_Object, MN_("fh"), TY_Array, MN_("offset"), 
		_Public|_Static, _F(k_MPI_File_get_size), TY_Int, TY_MPI, MN_("MPI_File_get_size"), 2, TY_Object, MN_("fh"), TY_Array, MN_("size"), 
		_Public|_Static, _F(k_MPI_File_get_type_extent), TY_Int, TY_MPI, MN_("MPI_File_get_type_extent"), 3, TY_Object, MN_("fh"), TY_Int, MN_("datatype"), TY_Array, MN_("extent"), 
		_Public|_Static, _F(k_MPI_File_get_view), TY_Int, TY_MPI, MN_("MPI_File_get_view"), 5, TY_Object, MN_("fh"), TY_Array, MN_("disp"), TY_Array, MN_("etype"), TY_Array, MN_("filetype"), TY_Array, MN_("datarep"), 
		_Public|_Static, _F(k_MPI_File_iread), TY_Int, TY_MPI, MN_("MPI_File_iread"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_File_iread_at), TY_Int, TY_MPI, MN_("MPI_File_iread_at"), 6, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_File_iread_shared), TY_Int, TY_MPI, MN_("MPI_File_iread_shared"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_File_iwrite), TY_Int, TY_MPI, MN_("MPI_File_iwrite"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_File_iwrite_at), TY_Int, TY_MPI, MN_("MPI_File_iwrite_at"), 6, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("request"), 
		//_Public|_Static, _F(k_MPI_File_), TY_Int, TY_MPI, MN_("MPI_File_"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_File_open), TY_Int, TY_MPI, MN_("MPI_File_open"), 5, TY_Int, MN_("comm"), TY_Array, MN_("filename"), TY_Int, MN_("amode"), TY_Int, MN_("info"), TY_Array, MN_("fh"), 
		_Public|_Static, _F(k_MPI_File_preallocate), TY_Int, TY_MPI, MN_("MPI_File_preallocate"), 2, TY_Object, MN_("fh"), TY_Int, MN_("size"), 
		_Public|_Static, _F(k_MPI_File_read), TY_Int, TY_MPI, MN_("MPI_File_read"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_read_all), TY_Int, TY_MPI, MN_("MPI_File_read_all"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_read_all_begin), TY_Int, TY_MPI, MN_("MPI_File_read_all_begin"), 4, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_File_read_all_end), TY_Int, TY_MPI, MN_("MPI_File_read_all_end"), 3, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_read_at), TY_Int, TY_MPI, MN_("MPI_File_read_at"), 6, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_read_at_all), TY_Int, TY_MPI, MN_("MPI_File_read_at_all"), 6, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_read_at_all_begin), TY_Int, TY_MPI, MN_("MPI_File_read_at_all_begin"), 5, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_File_read_at_all_end), TY_Int, TY_MPI, MN_("MPI_File_read_at_all_end"), 3, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_read_ordered), TY_Int, TY_MPI, MN_("MPI_File_read_ordered"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_read_ordered_begin), TY_Int, TY_MPI, MN_("MPI_File_read_ordered_begin"), 4, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_File_read_ordered_end), TY_Int, TY_MPI, MN_("MPI_File_read_ordered_end"), 3, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_read_shared), TY_Int, TY_MPI, MN_("MPI_File_read_shared"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_seek), TY_Int, TY_MPI, MN_("MPI_File_seek"), 3, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Int, MN_("whence"), 
		_Public|_Static, _F(k_MPI_File_seek_shared), TY_Int, TY_MPI, MN_("MPI_File_seek_shared"), 3, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Int, MN_("whence"), 
		_Public|_Static, _F(k_MPI_File_set_atomicity), TY_Int, TY_MPI, MN_("MPI_File_set_atomicity"), 2, TY_Object, MN_("fh"), TY_Int, MN_("flag"), 
		_Public|_Static, _F(k_MPI_File_set_errhandler), TY_Int, TY_MPI, MN_("MPI_File_set_errhandler"), 2, TY_Object, MN_("file"), TY_Int, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_File_set_info), TY_Int, TY_MPI, MN_("MPI_File_set_info"), 2, TY_Object, MN_("fh"), TY_Int, MN_("info"), 
		_Public|_Static, _F(k_MPI_File_set_size), TY_Int, TY_MPI, MN_("MPI_File_set_size"), 2, TY_Object, MN_("fh"), TY_Int, MN_("size"), 
		_Public|_Static, _F(k_MPI_File_set_view), TY_Int, TY_MPI, MN_("MPI_File_set_view"), 6, TY_Object, MN_("fh"), TY_Int, MN_("disp"), TY_Int, MN_("etype"), TY_Int, MN_("filetype"), TY_Array, MN_("datarep"), TY_Int, MN_("info"), 
		_Public|_Static, _F(k_MPI_File_sync), TY_Int, TY_MPI, MN_("MPI_File_sync"), 1, TY_Object, MN_("fh"), 
		_Public|_Static, _F(k_MPI_File_write), TY_Int, TY_MPI, MN_("MPI_File_write"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_write_all), TY_Int, TY_MPI, MN_("MPI_File_write_all"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_write_all_begin), TY_Int, TY_MPI, MN_("MPI_File_write_all_begin"), 4, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_File_write_all_end), TY_Int, TY_MPI, MN_("MPI_File_write_all_end"), 3, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_write_at), TY_Int, TY_MPI, MN_("MPI_File_write_at"), 6, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_write_at_all), TY_Int, TY_MPI, MN_("MPI_File_write_at_all"), 6, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_write_at_all_begin), TY_Int, TY_MPI, MN_("MPI_File_write_at_all_begin"), 5, TY_Object, MN_("fh"), TY_Int, MN_("offset"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_File_write_at_all_end), TY_Int, TY_MPI, MN_("MPI_File_write_at_all_end"), 3, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_write_ordered), TY_Int, TY_MPI, MN_("MPI_File_write_ordered"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_write_ordered_begin), TY_Int, TY_MPI, MN_("MPI_File_write_ordered_begin"), 4, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_File_write_ordered_end), TY_Int, TY_MPI, MN_("MPI_File_write_ordered_end"), 3, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_File_write_shared), TY_Int, TY_MPI, MN_("MPI_File_write_shared"), 5, TY_Object, MN_("fh"), TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Finalize), TY_Int, TY_MPI, MN_("MPI_Finalize"), 0, 
		_Public|_Static, _F(k_MPI_Finalized), TY_Int, TY_MPI, MN_("MPI_Finalized"), 1, TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Free_mem), TY_Int, TY_MPI, MN_("MPI_Free_mem"), 1, TY_Array, MN_("base"), 
		_Public|_Static, _F(k_MPI_Gather), TY_Int, TY_MPI, MN_("MPI_Gather"), 8, TY_Array, MN_("sendbuf"), TY_Int, MN_("sendcount"), TY_Int, MN_("sendtype"), TY_Array, MN_("recvbuf"), TY_Int, MN_("recvcount"), TY_Int, MN_("recvtype"), TY_Int, MN_("root"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Gatherv), TY_Int, TY_MPI, MN_("MPI_Gatherv"), 9, TY_Array, MN_("sendbuf"), TY_Int, MN_("sendcount"), TY_Int, MN_("sendtype"), TY_Array, MN_("recvbuf"), TY_Array, MN_("recvcounts"), TY_Array, MN_("displs"), TY_Int, MN_("recvtype"), TY_Int, MN_("root"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Get_address), TY_Int, TY_MPI, MN_("MPI_Get_address"), 2, TY_Array, MN_("location"), TY_Array, MN_("address"), 
		_Public|_Static, _F(k_MPI_Get_count), TY_Int, TY_MPI, MN_("MPI_Get_count"), 3, TY_Array, MN_("status"), TY_Int, MN_("datatype"), TY_Array, MN_("count"), 
		_Public|_Static, _F(k_MPI_Get_elements), TY_Int, TY_MPI, MN_("MPI_Get_elements"), 3, TY_Array, MN_("status"), TY_Int, MN_("datatype"), TY_Array, MN_("count"), 
		_Public|_Static, _F(k_MPI_Get_processor_name), TY_Int, TY_MPI, MN_("MPI_Get_processor_name"), 2, TY_Array, MN_("name"), TY_Array, MN_("resultlen"), 
		_Public|_Static, _F(k_MPI_Get_version), TY_Int, TY_MPI, MN_("MPI_Get_version"), 2, TY_Array, MN_("version"), TY_Array, MN_("subversion"), 
		_Public|_Static, _F(k_MPI_Graph_create), TY_Int, TY_MPI, MN_("MPI_Graph_create"), 6, TY_Int, MN_("comm_old"), TY_Int, MN_("nnodes"), TY_Array, MN_("index"), TY_Array, MN_("edges"), TY_Int, MN_("reorder"), TY_Array, MN_("comm_graph"), 
		_Public|_Static, _F(k_MPI_Graph_get), TY_Int, TY_MPI, MN_("MPI_Graph_get"), 5, TY_Int, MN_("comm"), TY_Int, MN_("maxindex"), TY_Int, MN_("maxedges"), TY_Array, MN_("index"), TY_Array, MN_("edges"), 
		_Public|_Static, _F(k_MPI_Graph_map), TY_Int, TY_MPI, MN_("MPI_Graph_map"), 5, TY_Int, MN_("comm"), TY_Int, MN_("nnodes"), TY_Array, MN_("index"), TY_Array, MN_("edges"), TY_Array, MN_("newrank"), 
		_Public|_Static, _F(k_MPI_Graph_neighbors), TY_Int, TY_MPI, MN_("MPI_Graph_neighbors"), 4, TY_Int, MN_("comm"), TY_Int, MN_("rank"), TY_Int, MN_("maxneighbors"), TY_Array, MN_("neighbors"), 
		_Public|_Static, _F(k_MPI_Graph_neighbors_count), TY_Int, TY_MPI, MN_("MPI_Graph_neighbors_count"), 3, TY_Int, MN_("comm"), TY_Int, MN_("rank"), TY_Array, MN_("nneighbors"), 
		_Public|_Static, _F(k_MPI_Graphdims_get), TY_Int, TY_MPI, MN_("MPI_Graphdims_get"), 3, TY_Int, MN_("comm"), TY_Array, MN_("nnodes"), TY_Array, MN_("nedges"), 
		_Public|_Static, _F(k_MPI_Grequest_complete), TY_Int, TY_MPI, MN_("MPI_Grequest_complete"), 1, TY_Int, MN_("request"), 
		_Public|_Static, _F(k_MPI_Grequest_start), TY_Int, TY_MPI, MN_("MPI_Grequest_start"), 5, TY_Array, MN_("query_fn"), TY_Array, MN_("free_fn"), TY_Array, MN_("cancel_fn"), TY_Array, MN_("extra_state"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Group_compare), TY_Int, TY_MPI, MN_("MPI_Group_compare"), 3, TY_Int, MN_("group1"), TY_Int, MN_("group2"), TY_Array, MN_("result"), 
		_Public|_Static, _F(k_MPI_Group_difference), TY_Int, TY_MPI, MN_("MPI_Group_difference"), 3, TY_Int, MN_("group1"), TY_Int, MN_("group2"), TY_Array, MN_("newgroup"), 
		_Public|_Static, _F(k_MPI_Group_excl), TY_Int, TY_MPI, MN_("MPI_Group_excl"), 4, TY_Int, MN_("group"), TY_Int, MN_("n"), TY_Array, MN_("ranks"), TY_Array, MN_("newgroup"), 
		_Public|_Static, _F(k_MPI_Group_free), TY_Int, TY_MPI, MN_("MPI_Group_free"), 1, TY_Array, MN_("group"), 
		_Public|_Static, _F(k_MPI_Group_incl), TY_Int, TY_MPI, MN_("MPI_Group_incl"), 4, TY_Int, MN_("group"), TY_Int, MN_("n"), TY_Array, MN_("ranks"), TY_Array, MN_("newgroup"), 
		_Public|_Static, _F(k_MPI_Group_intersection), TY_Int, TY_MPI, MN_("MPI_Group_intersection"), 3, TY_Int, MN_("group1"), TY_Int, MN_("group2"), TY_Array, MN_("newgroup"), 
		_Public|_Static, _F(k_MPI_Group_range_excl), TY_Int, TY_MPI, MN_("MPI_Group_range_excl"), 4, TY_Int, MN_("group"), TY_Int, MN_("n"), TY_Array, MN_("ranges"), TY_Array, MN_("newgroup"), 
		_Public|_Static, _F(k_MPI_Group_range_incl), TY_Int, TY_MPI, MN_("MPI_Group_range_incl"), 4, TY_Int, MN_("group"), TY_Int, MN_("n"), TY_Array, MN_("ranges"), TY_Array, MN_("newgroup"), 
		_Public|_Static, _F(k_MPI_Group_rank), TY_Int, TY_MPI, MN_("MPI_Group_rank"), 2, TY_Int, MN_("group"), TY_Array, MN_("rank"), 
		_Public|_Static, _F(k_MPI_Group_size), TY_Int, TY_MPI, MN_("MPI_Group_size"), 2, TY_Int, MN_("group"), TY_Array, MN_("size"), 
		_Public|_Static, _F(k_MPI_Group_translate_ranks), TY_Int, TY_MPI, MN_("MPI_Group_translate_ranks"), 5, TY_Int, MN_("group1"), TY_Int, MN_("n"), TY_Array, MN_("ranks1"), TY_Int, MN_("group2"), TY_Array, MN_("ranks2"), 
		_Public|_Static, _F(k_MPI_Group_union), TY_Int, TY_MPI, MN_("MPI_Group_union"), 3, TY_Int, MN_("group1"), TY_Int, MN_("group2"), TY_Array, MN_("newgroup"), 
		_Public|_Static, _F(k_MPI_Ibsend), TY_Int, TY_MPI, MN_("MPI_Ibsend"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Info_create), TY_Int, TY_MPI, MN_("MPI_Info_create"), 1, TY_Array, MN_("info"), 
		_Public|_Static, _F(k_MPI_Info_delete), TY_Int, TY_MPI, MN_("MPI_Info_delete"), 2, TY_Int, MN_("info"), TY_Array, MN_("key"), 
		_Public|_Static, _F(k_MPI_Info_dup), TY_Int, TY_MPI, MN_("MPI_Info_dup"), 2, TY_Int, MN_("info"), TY_Array, MN_("newinfo"), 
		_Public|_Static, _F(k_MPI_Info_free), TY_Int, TY_MPI, MN_("MPI_Info_free"), 1, TY_Array, MN_("info"), 
		_Public|_Static, _F(k_MPI_Info_get), TY_Int, TY_MPI, MN_("MPI_Info_get"), 5, TY_Int, MN_("info"), TY_Array, MN_("key"), TY_Int, MN_("valuelen"), TY_Array, MN_("value"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Info_get_nkeys), TY_Int, TY_MPI, MN_("MPI_Info_get_nkeys"), 2, TY_Int, MN_("info"), TY_Array, MN_("nkeys"), 
		_Public|_Static, _F(k_MPI_Info_get_nthkey), TY_Int, TY_MPI, MN_("MPI_Info_get_nthkey"), 3, TY_Int, MN_("info"), TY_Int, MN_("n"), TY_Array, MN_("key"), 
		_Public|_Static, _F(k_MPI_Info_get_valuelen), TY_Int, TY_MPI, MN_("MPI_Info_get_valuelen"), 4, TY_Int, MN_("info"), TY_Array, MN_("key"), TY_Array, MN_("valuelen"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Info_set), TY_Int, TY_MPI, MN_("MPI_Info_set"), 3, TY_Int, MN_("info"), TY_Array, MN_("key"), TY_Array, MN_("value"), 
		_Public|_Static, _F(k_MPI_Init), TY_Int, TY_MPI, MN_("MPI_Init"), 2, TY_Array, MN_("argc"), TY_Array, MN_("argv"), 
		_Public|_Static, _F(k_MPI_Init_thread), TY_Int, TY_MPI, MN_("MPI_Init_thread"), 4, TY_Array, MN_("argc"), TY_Array, MN_("argv"), TY_Int, MN_("required"), TY_Array, MN_("provided"), 
		_Public|_Static, _F(k_MPI_Initialized), TY_Int, TY_MPI, MN_("MPI_Initialized"), 1, TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Intercomm_create), TY_Int, TY_MPI, MN_("MPI_Intercomm_create"), 6, TY_Int, MN_("local_comm"), TY_Int, MN_("local_leader"), TY_Int, MN_("peer_comm"), TY_Int, MN_("remote_leader"), TY_Int, MN_("tag"), TY_Array, MN_("newintercomm"), 
		_Public|_Static, _F(k_MPI_Iprobe), TY_Int, TY_MPI, MN_("MPI_Iprobe"), 5, TY_Int, MN_("source"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("flag"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Irecv), TY_Int, TY_MPI, MN_("MPI_Irecv"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("source"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Irsend), TY_Int, TY_MPI, MN_("MPI_Irsend"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Is_thread_main), TY_Int, TY_MPI, MN_("MPI_Is_thread_main"), 1, TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Isend), TY_Int, TY_MPI, MN_("MPI_Isend"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Issend), TY_Int, TY_MPI, MN_("MPI_Issend"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Keyval_create), TY_Int, TY_MPI, MN_("MPI_Keyval_create"), 4, TY_Array, MN_("copy_fn"), TY_Array, MN_("delete_fn"), TY_Array, MN_("keyval"), TY_Array, MN_("extra_state"), 
		_Public|_Static, _F(k_MPI_Keyval_free), TY_Int, TY_MPI, MN_("MPI_Keyval_free"), 1, TY_Array, MN_("keyval"), 
		_Public|_Static, _F(k_MPI_Lookup_name), TY_Int, TY_MPI, MN_("MPI_Lookup_name"), 3, TY_Array, MN_("service_name"), TY_Int, MN_("info"), TY_Array, MN_("port_name"), 
		_Public|_Static, _F(k_MPI_Op_create), TY_Int, TY_MPI, MN_("MPI_Op_create"), 3, TY_Array, MN_("function"), TY_Int, MN_("commute"), TY_Array, MN_("op"), 
		_Public|_Static, _F(k_MPI_Op_free), TY_Int, TY_MPI, MN_("MPI_Op_free"), 1, TY_Array, MN_("op"), 
		_Public|_Static, _F(k_MPI_Open_port), TY_Int, TY_MPI, MN_("MPI_Open_port"), 2, TY_Int, MN_("info"), TY_Array, MN_("port_name"), 
		_Public|_Static, _F(k_MPI_Pack), TY_Int, TY_MPI, MN_("MPI_Pack"), 7, TY_Array, MN_("inbuf"), TY_Int, MN_("incount"), TY_Int, MN_("datatype"), TY_Array, MN_("outbuf"), TY_Int, MN_("outsize"), TY_Array, MN_("position"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Pack_external), TY_Int, TY_MPI, MN_("MPI_Pack_external"), 7, TY_Array, MN_("datarep"), TY_Array, MN_("inbuf"), TY_Int, MN_("incount"), TY_Int, MN_("datatype"), TY_Array, MN_("outbuf"), TY_Int, MN_("outsize"), TY_Array, MN_("position"), 
		_Public|_Static, _F(k_MPI_Pack_external_size), TY_Int, TY_MPI, MN_("MPI_Pack_external_size"), 4, TY_Array, MN_("datarep"), TY_Int, MN_("incount"), TY_Int, MN_("datatype"), TY_Array, MN_("size"), 
		_Public|_Static, _F(k_MPI_Pack_size), TY_Int, TY_MPI, MN_("MPI_Pack_size"), 4, TY_Int, MN_("incount"), TY_Int, MN_("datatype"), TY_Int, MN_("comm"), TY_Array, MN_("size"), 
		_Public|_Static, _F(k_MPI_Probe), TY_Int, TY_MPI, MN_("MPI_Probe"), 4, TY_Int, MN_("source"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Publish_name), TY_Int, TY_MPI, MN_("MPI_Publish_name"), 3, TY_Array, MN_("service_name"), TY_Int, MN_("info"), TY_Array, MN_("port_name"), 
		_Public|_Static, _F(k_MPI_Query_thread), TY_Int, TY_MPI, MN_("MPI_Query_thread"), 1, TY_Array, MN_("provided"), 
		_Public|_Static, _F(k_MPI_Recv), TY_Int, TY_MPI, MN_("MPI_Recv"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("source"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Recv_init), TY_Int, TY_MPI, MN_("MPI_Recv_init"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("source"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Reduce), TY_Int, TY_MPI, MN_("MPI_Reduce"), 7, TY_Array, MN_("sendbuf"), TY_Array, MN_("recvbuf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("op"), TY_Int, MN_("root"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Reduce_local), TY_Int, TY_MPI, MN_("MPI_Reduce_local"), 5, TY_Array, MN_("inbuf"), TY_Array, MN_("inoutbuf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("op"), 
		_Public|_Static, _F(k_MPI_Reduce_scatter), TY_Int, TY_MPI, MN_("MPI_Reduce_scatter"), 6, TY_Array, MN_("sendbuf"), TY_Array, MN_("recvbuf"), TY_Array, MN_("recvcounts"), TY_Int, MN_("datatype"), TY_Int, MN_("op"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Register_datarep), TY_Int, TY_MPI, MN_("MPI_Register_datarep"), 5, TY_Array, MN_("datarep"), TY_Array, MN_("read_conversion_fn"), TY_Array, MN_("write_conversion_fn"), TY_Array, MN_("dtype_file_extent_fn"), TY_Array, MN_("extra_state"), 
		_Public|_Static, _F(k_MPI_Request_free), TY_Int, TY_MPI, MN_("MPI_Request_free"), 1, TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Request_get_status), TY_Int, TY_MPI, MN_("MPI_Request_get_status"), 3, TY_Int, MN_("request"), TY_Array, MN_("flag"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Rsend), TY_Int, TY_MPI, MN_("MPI_Rsend"), 6, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Rsend_init), TY_Int, TY_MPI, MN_("MPI_Rsend_init"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Scan), TY_Int, TY_MPI, MN_("MPI_Scan"), 6, TY_Array, MN_("sendbuf"), TY_Array, MN_("recvbuf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("op"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Scatter), TY_Int, TY_MPI, MN_("MPI_Scatter"), 8, TY_Array, MN_("sendbuf"), TY_Int, MN_("sendcount"), TY_Int, MN_("sendtype"), TY_Array, MN_("recvbuf"), TY_Int, MN_("recvcount"), TY_Int, MN_("recvtype"), TY_Int, MN_("root"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Scatterv), TY_Int, TY_MPI, MN_("MPI_Scatterv"), 9, TY_Array, MN_("sendbuf"), TY_Array, MN_("sendcounts"), TY_Array, MN_("displs"), TY_Int, MN_("sendtype"), TY_Array, MN_("recvbuf"), TY_Int, MN_("recvcount"), TY_Int, MN_("recvtype"), TY_Int, MN_("root"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Send), TY_Int, TY_MPI, MN_("MPI_Send"), 6, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Send_init), TY_Int, TY_MPI, MN_("MPI_Send_init"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Sendrecv), TY_Int, TY_MPI, MN_("MPI_Sendrecv"), 12, TY_Array, MN_("sendbuf"), TY_Int, MN_("sendcount"), TY_Int, MN_("sendtype"), TY_Int, MN_("dest"), TY_Int, MN_("sendtag"), TY_Array, MN_("recvbuf"), TY_Int, MN_("recvcount"), TY_Int, MN_("recvtype"), TY_Int, MN_("source"), TY_Int, MN_("recvtag"), TY_Int, MN_("comm"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Sendrecv_replace), TY_Int, TY_MPI, MN_("MPI_Sendrecv_replace"), 9, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("sendtag"), TY_Int, MN_("source"), TY_Int, MN_("recvtag"), TY_Int, MN_("comm"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Ssend), TY_Int, TY_MPI, MN_("MPI_Ssend"), 6, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Ssend_init), TY_Int, TY_MPI, MN_("MPI_Ssend_init"), 7, TY_Array, MN_("buf"), TY_Int, MN_("count"), TY_Int, MN_("datatype"), TY_Int, MN_("dest"), TY_Int, MN_("tag"), TY_Int, MN_("comm"), TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Start), TY_Int, TY_MPI, MN_("MPI_Start"), 1, TY_Array, MN_("request"), 
		_Public|_Static, _F(k_MPI_Startall), TY_Int, TY_MPI, MN_("MPI_Startall"), 2, TY_Int, MN_("count"), TY_Array, MN_("array_of_requests"), 
		//_Public|_Static, _F(k_MPI_Status_f2c), TY_Int, TY_MPI, MN_("MPI_Status_f2c"), 2, TY_Array, MN_("f_status"), TY_Array, MN_("c_status"), 
		//_Public|_Static, _F(k_MPI_Status_c2f), TY_Int, TY_MPI, MN_("MPI_Status_c2f"), 2, TY_Array, MN_("c_status"), TY_Array, MN_("f_status"), 
		_Public|_Static, _F(k_MPI_Status_set_cancelled), TY_Int, TY_MPI, MN_("MPI_Status_set_cancelled"), 2, TY_Array, MN_("status"), TY_Int, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Status_set_elements), TY_Int, TY_MPI, MN_("MPI_Status_set_elements"), 3, TY_Array, MN_("status"), TY_Int, MN_("datatype"), TY_Int, MN_("count"), 
		_Public|_Static, _F(k_MPI_Test), TY_Int, TY_MPI, MN_("MPI_Test"), 3, TY_Array, MN_("request"), TY_Array, MN_("flag"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Test_cancelled), TY_Int, TY_MPI, MN_("MPI_Test_cancelled"), 2, TY_Array, MN_("status"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Testall), TY_Int, TY_MPI, MN_("MPI_Testall"), 4, TY_Int, MN_("count"), TY_Array, MN_("array_of_requests"), TY_Array, MN_("flag"), TY_Array, MN_("array_of_statuses"), 
		_Public|_Static, _F(k_MPI_Testany), TY_Int, TY_MPI, MN_("MPI_Testany"), 5, TY_Int, MN_("count"), TY_Array, MN_("array_of_requests"), TY_Array, MN_("index"), TY_Array, MN_("flag"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Testsome), TY_Int, TY_MPI, MN_("MPI_Testsome"), 5, TY_Int, MN_("incount"), TY_Array, MN_("array_of_requests"), TY_Array, MN_("outcount"), TY_Array, MN_("array_of_indices"), TY_Array, MN_("array_of_statuses"), 
		_Public|_Static, _F(k_MPI_Topo_test), TY_Int, TY_MPI, MN_("MPI_Topo_test"), 2, TY_Int, MN_("comm"), TY_Array, MN_("top_type"), 
		_Public|_Static, _F(k_MPI_Type_commit), TY_Int, TY_MPI, MN_("MPI_Type_commit"), 1, TY_Array, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_Type_contiguous), TY_Int, TY_MPI, MN_("MPI_Type_contiguous"), 3, TY_Int, MN_("count"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_create_darray), TY_Int, TY_MPI, MN_("MPI_Type_create_darray"), 10, TY_Int, MN_("size"), TY_Int, MN_("rank"), TY_Int, MN_("ndims"), TY_Array, MN_("array_of_gsizes"), TY_Array, MN_("array_of_distribs"), TY_Array, MN_("array_of_dargs"), TY_Array, MN_("array_of_psizes"), TY_Int, MN_("order"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		//_Public|_Static, _F(k_MPI_Type_create_f90_complex), TY_Int, TY_MPI, MN_("MPI_Type_create_f90_complex"), 3, TY_Int, MN_("p"), TY_Int, MN_("r"), TY_Array, MN_("newtype"), 
		//_Public|_Static, _F(k_MPI_Type_create_f90_integer), TY_Int, TY_MPI, MN_("MPI_Type_create_f90_integer"), 2, TY_Int, MN_("r"), TY_Array, MN_("newtype"), 
		//_Public|_Static, _F(k_MPI_Type_create_f90_real), TY_Int, TY_MPI, MN_("MPI_Type_create_f90_real"), 3, TY_Int, MN_("p"), TY_Int, MN_("r"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_create_hindexed), TY_Int, TY_MPI, MN_("MPI_Type_create_hindexed"), 5, TY_Int, MN_("count"), TY_Array, MN_("array_of_blocklengths"), TY_Array, MN_("array_of_displacements"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_create_hvector), TY_Int, TY_MPI, MN_("MPI_Type_create_hvector"), 5, TY_Int, MN_("count"), TY_Int, MN_("blocklength"), TY_Int, MN_("stride"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_create_indexed_block), TY_Int, TY_MPI, MN_("MPI_Type_create_indexed_block"), 5, TY_Int, MN_("count"), TY_Int, MN_("blocklength"), TY_Array, MN_("array_of_displacements"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_create_keyval), TY_Int, TY_MPI, MN_("MPI_Type_create_keyval"), 4, TY_Array, MN_("type_copy_attr_fn"), TY_Array, MN_("type_delete_attr_fn"), TY_Array, MN_("type_keyval"), TY_Array, MN_("extra_state"), 
		_Public|_Static, _F(k_MPI_Type_create_resized), TY_Int, TY_MPI, MN_("MPI_Type_create_resized"), 4, TY_Int, MN_("oldtype"), TY_Int, MN_("lb"), TY_Int, MN_("extent"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_create_struct), TY_Int, TY_MPI, MN_("MPI_Type_create_struct"), 5, TY_Int, MN_("count"), TY_Array, MN_("array_of_blocklengths"), TY_Array, MN_("array_of_displacements"), TY_Array, MN_("array_of_types"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_create_subarray), TY_Int, TY_MPI, MN_("MPI_Type_create_subarray"), 7, TY_Int, MN_("ndims"), TY_Array, MN_("array_of_sizes"), TY_Array, MN_("array_of_subsizes"), TY_Array, MN_("array_of_starts"), TY_Int, MN_("order"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_delete_attr), TY_Int, TY_MPI, MN_("MPI_Type_delete_attr"), 2, TY_Int, MN_("type"), TY_Int, MN_("type_keyval"), 
		_Public|_Static, _F(k_MPI_Type_dup), TY_Int, TY_MPI, MN_("MPI_Type_dup"), 2, TY_Int, MN_("type"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_extent), TY_Int, TY_MPI, MN_("MPI_Type_extent"), 2, TY_Int, MN_("datatype"), TY_Array, MN_("extent"), 
		_Public|_Static, _F(k_MPI_Type_free), TY_Int, TY_MPI, MN_("MPI_Type_free"), 1, TY_Array, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_Type_free_keyval), TY_Int, TY_MPI, MN_("MPI_Type_free_keyval"), 1, TY_Array, MN_("type_keyval"), 
		_Public|_Static, _F(k_MPI_Type_get_attr), TY_Int, TY_MPI, MN_("MPI_Type_get_attr"), 4, TY_Int, MN_("type"), TY_Int, MN_("type_keyval"), TY_Array, MN_("attribute_val"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Type_get_contents), TY_Int, TY_MPI, MN_("MPI_Type_get_contents"), 7, TY_Int, MN_("datatype"), TY_Int, MN_("max_integers"), TY_Int, MN_("max_addresses"), TY_Int, MN_("max_datatypes"), TY_Array, MN_("array_of_integers"), TY_Array, MN_("array_of_addresses"), TY_Array, MN_("array_of_datatypes"), 
		_Public|_Static, _F(k_MPI_Type_get_envelope), TY_Int, TY_MPI, MN_("MPI_Type_get_envelope"), 5, TY_Int, MN_("datatype"), TY_Array, MN_("num_integers"), TY_Array, MN_("num_addresses"), TY_Array, MN_("num_datatypes"), TY_Array, MN_("combiner"), 
		_Public|_Static, _F(k_MPI_Type_get_extent), TY_Int, TY_MPI, MN_("MPI_Type_get_extent"), 3, TY_Int, MN_("datatype"), TY_Array, MN_("lb"), TY_Array, MN_("extent"), 
		_Public|_Static, _F(k_MPI_Type_get_name), TY_Int, TY_MPI, MN_("MPI_Type_get_name"), 3, TY_Int, MN_("type"), TY_Array, MN_("type_name"), TY_Array, MN_("resultlen"), 
		_Public|_Static, _F(k_MPI_Type_get_true_extent), TY_Int, TY_MPI, MN_("MPI_Type_get_true_extent"), 3, TY_Int, MN_("datatype"), TY_Array, MN_("true_lb"), TY_Array, MN_("true_extent"), 
		_Public|_Static, _F(k_MPI_Type_hindexed), TY_Int, TY_MPI, MN_("MPI_Type_hindexed"), 5, TY_Int, MN_("count"), TY_Array, MN_("array_of_blocklengths"), TY_Array, MN_("array_of_displacements"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_hvector), TY_Int, TY_MPI, MN_("MPI_Type_hvector"), 5, TY_Int, MN_("count"), TY_Int, MN_("blocklength"), TY_Int, MN_("stride"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_indexed), TY_Int, TY_MPI, MN_("MPI_Type_indexed"), 5, TY_Int, MN_("count"), TY_Array, MN_("array_of_blocklengths"), TY_Array, MN_("array_of_displacements"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_lb), TY_Int, TY_MPI, MN_("MPI_Type_lb"), 2, TY_Int, MN_("datatype"), TY_Array, MN_("displacement"), 
		_Public|_Static, _F(k_MPI_Type_match_size), TY_Int, TY_MPI, MN_("MPI_Type_match_size"), 3, TY_Int, MN_("typeclass"), TY_Int, MN_("size"), TY_Array, MN_("type"), 
		_Public|_Static, _F(k_MPI_Type_set_attr), TY_Int, TY_MPI, MN_("MPI_Type_set_attr"), 3, TY_Int, MN_("type"), TY_Int, MN_("type_keyval"), TY_Array, MN_("attribute_val"), 
		_Public|_Static, _F(k_MPI_Type_set_name), TY_Int, TY_MPI, MN_("MPI_Type_set_name"), 2, TY_Int, MN_("type"), TY_Array, MN_("type_name"), 
		_Public|_Static, _F(k_MPI_Type_size), TY_Int, TY_MPI, MN_("MPI_Type_size"), 2, TY_Int, MN_("datatype"), TY_Array, MN_("size"), 
		_Public|_Static, _F(k_MPI_Type_struct), TY_Int, TY_MPI, MN_("MPI_Type_struct"), 5, TY_Int, MN_("count"), TY_Array, MN_("array_of_blocklengths"), TY_Array, MN_("array_of_displacements"), TY_Array, MN_("array_of_types"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Type_ub), TY_Int, TY_MPI, MN_("MPI_Type_ub"), 2, TY_Int, MN_("datatype"), TY_Array, MN_("displacement"), 
		_Public|_Static, _F(k_MPI_Type_vector), TY_Int, TY_MPI, MN_("MPI_Type_vector"), 5, TY_Int, MN_("count"), TY_Int, MN_("blocklength"), TY_Int, MN_("stride"), TY_Int, MN_("oldtype"), TY_Array, MN_("newtype"), 
		_Public|_Static, _F(k_MPI_Unpack), TY_Int, TY_MPI, MN_("MPI_Unpack"), 7, TY_Array, MN_("inbuf"), TY_Int, MN_("insize"), TY_Array, MN_("position"), TY_Array, MN_("outbuf"), TY_Int, MN_("outcount"), TY_Int, MN_("datatype"), TY_Int, MN_("comm"), 
		_Public|_Static, _F(k_MPI_Unpack_external), TY_Int, TY_MPI, MN_("MPI_Unpack_external"), 7, TY_Array, MN_("datarep"), TY_Array, MN_("inbuf"), TY_Int, MN_("insize"), TY_Array, MN_("position"), TY_Array, MN_("outbuf"), TY_Int, MN_("outcount"), TY_Int, MN_("datatype"), 
		_Public|_Static, _F(k_MPI_Unpublish_name), TY_Int, TY_MPI, MN_("MPI_Unpublish_name"), 3, TY_Array, MN_("service_name"), TY_Int, MN_("info"), TY_Array, MN_("port_name"), 
		_Public|_Static, _F(k_MPI_Wait), TY_Int, TY_MPI, MN_("MPI_Wait"), 2, TY_Array, MN_("request"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Waitall), TY_Int, TY_MPI, MN_("MPI_Waitall"), 3, TY_Int, MN_("count"), TY_Array, MN_("array_of_requests"), TY_Array, MN_("array_of_statuses"), 
		_Public|_Static, _F(k_MPI_Waitany), TY_Int, TY_MPI, MN_("MPI_Waitany"), 4, TY_Int, MN_("count"), TY_Array, MN_("array_of_requests"), TY_Array, MN_("index"), TY_Array, MN_("status"), 
		_Public|_Static, _F(k_MPI_Waitsome), TY_Int, TY_MPI, MN_("MPI_Waitsome"), 5, TY_Int, MN_("incount"), TY_Array, MN_("array_of_requests"), TY_Array, MN_("outcount"), TY_Array, MN_("array_of_indices"), TY_Array, MN_("array_of_statuses"), 
		_Public|_Static, _F(k_MPI_Win_call_errhandler), TY_Int, TY_MPI, MN_("MPI_Win_call_errhandler"), 2, TY_Int, MN_("win"), TY_Int, MN_("errorcode"), 
		_Public|_Static, _F(k_MPI_Win_create_errhandler), TY_Int, TY_MPI, MN_("MPI_Win_create_errhandler"), 2, TY_Array, MN_("function"), TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Win_create_keyval), TY_Int, TY_MPI, MN_("MPI_Win_create_keyval"), 4, TY_Array, MN_("win_copy_attr_fn"), TY_Array, MN_("win_delete_attr_fn"), TY_Array, MN_("win_keyval"), TY_Array, MN_("extra_state"), 
		_Public|_Static, _F(k_MPI_Win_delete_attr), TY_Int, TY_MPI, MN_("MPI_Win_delete_attr"), 2, TY_Int, MN_("win"), TY_Int, MN_("win_keyval"), 
		_Public|_Static, _F(k_MPI_Win_fence), TY_Int, TY_MPI, MN_("MPI_Win_fence"), 2, TY_Int, MN_("assert"), TY_Int, MN_("win"), 
		_Public|_Static, _F(k_MPI_Win_free), TY_Int, TY_MPI, MN_("MPI_Win_free"), 1, TY_Array, MN_("win"), 
		_Public|_Static, _F(k_MPI_Win_free_keyval), TY_Int, TY_MPI, MN_("MPI_Win_free_keyval"), 1, TY_Array, MN_("win_keyval"), 
		_Public|_Static, _F(k_MPI_Win_get_attr), TY_Int, TY_MPI, MN_("MPI_Win_get_attr"), 4, TY_Int, MN_("win"), TY_Int, MN_("win_keyval"), TY_Array, MN_("attribute_val"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Win_get_errhandler), TY_Int, TY_MPI, MN_("MPI_Win_get_errhandler"), 2, TY_Int, MN_("win"), TY_Array, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Win_get_name), TY_Int, TY_MPI, MN_("MPI_Win_get_name"), 3, TY_Int, MN_("win"), TY_Array, MN_("win_name"), TY_Array, MN_("resultlen"), 
		_Public|_Static, _F(k_MPI_Win_lock), TY_Int, TY_MPI, MN_("MPI_Win_lock"), 4, TY_Int, MN_("lock_type"), TY_Int, MN_("rank"), TY_Int, MN_("assert"), TY_Int, MN_("win"), 
		_Public|_Static, _F(k_MPI_Win_post), TY_Int, TY_MPI, MN_("MPI_Win_post"), 3, TY_Int, MN_("group"), TY_Int, MN_("assert"), TY_Int, MN_("win"), 
		_Public|_Static, _F(k_MPI_Win_set_attr), TY_Int, TY_MPI, MN_("MPI_Win_set_attr"), 3, TY_Int, MN_("win"), TY_Int, MN_("win_keyval"), TY_Array, MN_("attribute_val"), 
		_Public|_Static, _F(k_MPI_Win_set_errhandler), TY_Int, TY_MPI, MN_("MPI_Win_set_errhandler"), 2, TY_Int, MN_("win"), TY_Int, MN_("errhandler"), 
		_Public|_Static, _F(k_MPI_Win_set_name), TY_Int, TY_MPI, MN_("MPI_Win_set_name"), 2, TY_Int, MN_("win"), TY_Array, MN_("win_name"), 
		_Public|_Static, _F(k_MPI_Win_start), TY_Int, TY_MPI, MN_("MPI_Win_start"), 3, TY_Int, MN_("group"), TY_Int, MN_("assert"), TY_Int, MN_("win"), 
		_Public|_Static, _F(k_MPI_Win_test), TY_Int, TY_MPI, MN_("MPI_Win_test"), 2, TY_Int, MN_("win"), TY_Array, MN_("flag"), 
		_Public|_Static, _F(k_MPI_Win_unlock), TY_Int, TY_MPI, MN_("MPI_Win_unlock"), 2, TY_Int, MN_("rank"), TY_Int, MN_("win"), 
		_Public|_Static, _F(k_MPI_Win_wait), TY_Int, TY_MPI, MN_("MPI_Win_wait"), 1, TY_Int, MN_("win"), 

		DEND,
	};
	kloadMethodData(NULL, methoddata);
	return true;
}

static kbool_t mpi_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t mpi_initKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t mpi_setupLingo(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KPACKDEF* mpi_init(void)
{
	static const KPACKDEF d = {
		KPACKNAME("mpi", "1.0"),
		.initPackage  = mpi_initPackage,
		.setupPackage = mpi_setupPackage,
		.initKonohaSpace = mpi_initKonohaSpace,
		.setupPackage = mpi_setupLingo,
	};
	return &d;
}

#ifdef __cplusplus
END_EXTERN_C
#endif

