//#include<konoha2/konoha2.h>
//#include<konoha2/sugar.h>
//
//
//// --------------------------------------------------------------------------
//
//
//// **************************************************************************
//// LIST OF CONTRIBUTERS
////  shinpei - Shinpei Nakata, Yokohama National University, Japan
////  kimio - Kimio Kuramitsu, Yokohama National University, Japan
////  goccy - Masaaki Goshima, Yokohama National University, Japan
//// **************************************************************************
//
//#include <unistd.h>
//#include <arpa/inet.h>
//#include <sys/socket.h>
//#include <errno.h>
//#include <signal.h>
////#include <sys/ioctl.h>
///* ======================================================================== */
//// [csock module]
//
//#define MOD_SOCKET 21
//
//#define ksocketmod 				((ksocketmod_t *)_ctx->mod[MOD_SOCKET])
//#define ksocketshare				((ksocketshare_t *)_ctx->modshare[MOD_SOCKET])
//#define CT_CSock				ksocketshare->cCSock
//#define TY_CSock				ksocketshare->cCSock->cid
//
//#define IS_socket(O)				((O)->h.ct == CT_socket)
//
//typedef struct {
//	kmodshare_t h;
//	const kclass_t *cCSock;
//} ksocketshare_t;
//
//typedef struct {
//	kmod_t	h;
//}ksocketmod_t;
//
//typedef	struct ksocket_t	ksocket_t;
//struct ksocket_t {
//	const char *DBG_NAME;  // unnecessary for free
//};
//
//#undef new_T
//#define new_T(t)            new_kString(t, strlen(t), SPOL_TEXT|SPOL_ASCII|SPOL_POOL)
//
///* ======================================================================== */
//// [private functions]
//
///*static knh_IntData_t CSockConstInt[] = {
//	// Protocol families.
//	{"PF_LOCAL"        , PF_LOCAL         },
//	{"PF_UNIX"         , PF_UNIX          },
//	{"PF_INET"         , PF_INET          },
//	{"PF_INET6"        , PF_INET6         },
//	{"PF_APPLETALK"    , PF_APPLETALK     },
//#ifndef __APPLE__
//	{"PF_PACKET"       , PF_PACKET        },
//#endif
//	// Address families.
//	{"AF_LOCAL"        , AF_LOCAL         },
//	{"AF_UNIX"         , AF_UNIX          },
//	{"AF_INET"         , AF_INET          },
//	{"AF_INET6"        , AF_INET6         },
//	{"AF_APPLETALK"    , AF_APPLETALK     },
//#ifndef __APPLE__
//	{"AF_PACKET"       , AF_PACKET        },
//#endif
//	// Types of sockets.
//	{"SOCK_STREAM"     , SOCK_STREAM      },
//	{"SOCK_DGRAM"      , SOCK_DGRAM       },
//	{"SOCK_RAW"        , SOCK_RAW         },
//	{"SOCK_RDM"        , SOCK_RDM         },
//	// send & recv Flags.
//	{"MSG_OOB"         , MSG_OOB          },
//	{"MSG_PEEK"        , MSG_PEEK         },
//	{"MSG_DONTROUTE"   , MSG_DONTROUTE    },
//	{"MSG_TRUNC"       , MSG_TRUNC        },
//	{"MSG_DONTWAIT"    , MSG_DONTWAIT     },
//	{"MSG_EOR"         , MSG_EOR          },
//	{"MSG_WAITALL"     , MSG_WAITALL      },
//#ifndef __APPLE__
//	{"MSG_CONFIRM"     , MSG_CONFIRM      },
//	{"MSG_ERRQUEUE"    , MSG_ERRQUEUE     },
//	{"MSG_NOSIGNAL"    , MSG_NOSIGNAL     },
//	{"MSG_MORE"        , MSG_MORE         },
//#endif
//	// Socket options.
//	{"SO_REUSEADDR"    , SO_REUSEADDR     },
//	{"SO_TYPE"         , SO_TYPE          },
//	{"SO_ERROR"        , SO_ERROR         },
//	{"SO_DONTROUTE"    , SO_DONTROUTE     },
//	{"SO_BROADCAST"    , SO_BROADCAST     },
//	{"SO_SNDBUF"       , SO_SNDBUF        },
//	{"SO_RCVBUF"       , SO_RCVBUF        },
//	{"SO_KEEPALIVE"    , SO_KEEPALIVE     },
//	{"SO_OOBINLINE"    , SO_OOBINLINE     },
//#ifndef __APPLE__
//	{"SO_NO_CHECK"     , SO_NO_CHECK      },
//	{"SO_PRIORITY"     , SO_PRIORITY      },
//#endif
//	// for shutdown
//	{"SHUT_RD"         , SHUT_RD          },
//	{"SHUT_WR"         , SHUT_WR          },
//	{"SHUT_RDWR"       , SHUT_RDWR        },
//	{NULL, 0}, //necessary for checking rnd of definition
//};
//*/
//
//
//static void CSock_init(CTX, kObject *po, void *conf)
//{
//	errno = 0;
//}
//
//static void CSock_free(CTX, kObject *po)
//{
//	errno = 0;
//}
//
//// <String, int, int> => sockaddr_in*
//void toSockaddr(struct sockaddr_in *addr, char *ip, int port, int family)
//{
//	memset(addr, 0, sizeof(*addr));
//	addr->sin_addr.s_addr = (*ip==0) ? 0 : inet_addr(ip);
//	addr->sin_port        = htons(port);
//	addr->sin_family      = family;
//}
//
//// sockaddr_in* => <String, int, int>
//char* fromSockaddr(struct sockaddr_in addr)
//{
//	static char info[64];
//	memset(info, 0, sizeof(info));
//
//	snprintf( info, sizeof(info), "addr:%s port:%d family:0x%08x",
//			   inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), addr.sin_family);
//	return info;
//}
//
//// for accept & recvfrom
//void char2string(CTX , char* c, kString *s)
//{
//	kString *str = new_(String, s);
//	s->str.ubuf = str->str.ubuf;
//	s->str.len  = str->str.len;
//}
//
//// for select :: kArray* => fd_set*
//static fd_set* toFd( fd_set* s, kArray *a )
//{
//	if( s == NULL || a->size <= 0) {
//		return NULL;
//	}
//
//	FD_ZERO(s);
//	int indx;
//	for( indx=0 ; indx<a->size ; indx++ ) {
//		if ( (a->ilist[indx] >= 0) && (a->ilist[indx] < FD_SETSIZE) ) {
//			FD_SET(a->ilist[indx], s);
//		}
//	}
//
//	return s;
//}
//
//// for select :: fd_set* => kArray*
//static void fromFd( CTX , fd_set* s, kArray *a )
//{
//	if( s != NULL && a->size > 0 ) {
//		int indx;
//		for( indx=0 ; indx<a->size ; indx++ ) {
//			if( !FD_ISSET(a->ilist[indx], s) ) {
////				kArray_remove_(ctx, a, indx);
//			}
//		}
//	}
//}
//
//// for select
//static int getArrayMax(kArray *a)
//{
//	int ret = -1;
//
//	if( a->size > 0 )	{
//		int cnt;
//		for( cnt=0 ; cnt<a->size ; cnt++ ) {
//			if( ret < a->ilist[cnt] ) {
//				ret = a->ilist[cnt];
//			}
//		}
//	}
//
//	return ret;
//}
//
//// for select
//static int getNfd( kArray *a1, kArray *a2, kArray *a3 )
//{
//	int ret = -1;
//	int tmp;
//
//	if( (tmp=getArrayMax(a1)) > ret ) {
//		ret = tmp;
//	}
//	if( (tmp=getArrayMax(a2)) > ret ) {
//		ret = tmp;
//	}
//	if( (tmp=getArrayMax(a3)) > ret ) {
//		ret = tmp;
//	}
//
//	return ret;
//}
///* ------------------------------------------------------------------------ */
//
////static const kclass_t *addClassDef(CTX, kString *name, KDEFINE_CLASS *cdef, kline_t pline)
//////DEFAPI(void) defCSock(CTX, kclass_t cid, kclassdef_t *cdef)
////{
////	kclass_t *ct = new_CT(_ctx, cdef, pline);
////	if (name == NULL) {
////		const char *n = cdef->structname;
////		assert(n != NULL); // structname must be set;
////		name = new_kString(n, strlen(n), SPOL_ASCII|SPOL_POOL|SPOL_TEXT);
////	}
////	ct->packid = cdef->packid;
////	ct->packdom = cdef->packdom;
////	CT_setName(_ctx, ct, name, pline);
////	return (const kclass_t*)ct;
/////*	cdef->name = "CSock";
////	cdef->init = CSock_init;
////	cdef->free = CSock_free;
////	*/
////}
//
////DEFAPI(void) constCSock(CTX, kclass_t cid, const knh_LoaderAPI_t *kapi)
////{
////	kapi->loadClassIntConst(ctx, cid, CSockConstInt);
////}
//
//typedef struct kCSock	kCSock;
//struct kCSock {
//	kObjectHeader h;
//};
//static KDEFINE_CLASS CSockDef = {
//		STRUCTNAME(CSock),
//		.cflag = 0,
//		.init = CSock_init,
//		.free = CSock_free
//};
//
//static void ksocketshare_setup(CTX, struct kmodshare_t *def)
//{
//
//}
//
//static void ksocketshare_reftrace(CTX, struct kmodshare_t *baseh)
//{
//
//}
//
//static void ksocketshare_free(CTX, struct kmodshare_t *baseh)
//{
//	KNH_FREE(baseh, sizeof(ksocketshare_t));
//}
///* ======================================================================== */
//// [KMETHODS]
//
////## int CSock.accept(int socket, String remoteInfo);
//static KMETHOD CSock_accept(CTX, ksfp_t* sfp _RIX)
//{
//	int ret = -1;
//	struct sockaddr_in addr;
//	int addrLen = sizeof(addr);
//
//	memset(&addr, 0, addrLen);
//	if( (ret = accept((int)sfp[1].ivalue,
//						 (struct sockaddr*)&addr,
//						 (socklen_t*)&addrLen) ) >= 0 ) {
//		char2string(_ctx, fromSockaddr(addr), sfp[2].s );
//	}
//	RETURNi_( ret );
//}
//
////## int CSock.bind(int socket, String srcIP, int srcPort, int family);
//KMETHOD CSock_bind(CTX, ksfp_t* sfp _RIX)
//{
//	struct sockaddr_in addr;
//	toSockaddr( &addr,
//					(char*)sfp[2].s,
////				  String_to(char*, sfp[2]),
//				  (int)sfp[3].ivalue,
//				  (int)sfp[4].ivalue );
//
//	RETURNi_( bind((int)sfp[1].ivalue,
//					 (struct sockaddr*)&addr,
//					 sizeof(addr)) );
//}
//
////## int CSock.close(int fd);
//KMETHOD CSock_close(CTX, ksfp_t* sfp _RIX)
//{
//	RETURNi_( close((int)sfp[1].ivalue) );
//}
//
////## int CSock.connect(int socket, String dstIP, int dstPort, int family);
//KMETHOD CSock_connect(CTX, ksfp_t* sfp _RIX)
//{
//	struct sockaddr_in addr;
//	toSockaddr( &addr,
//			(char*)sfp[2].s,
//			//				  String_to(char*, sfp[2]),
//				  (int)sfp[3].ivalue,
//				  (int)sfp[4].ivalue );
//
//	RETURNi_( connect((int)sfp[1].ivalue,
//						 (struct sockaddr*)&addr,
//						 sizeof(addr)) );
//}
//
////## int CSock.listen(int socket, int backlog);
//KMETHOD CSock_listen(CTX, ksfp_t* sfp _RIX)
//{
//	RETURNi_( listen((int)sfp[1].ivalue, (int)sfp[2].ivalue) );
//}
//
////## String CSock.name(int socket);
//KMETHOD CSock_name(CTX, ksfp_t* sfp _RIX)
//{
//	kString *ret_s = (kString*) K_NULL;/* TODO KNH_TNULL(String)*/
////	kString *ret_s = KNH_TNULL(kString);
//	struct sockaddr_in addr;
//	int addrLen = sizeof(addr);
//
//	memset(&addr, 0, addrLen);
//	if( getsockname( (int)sfp[1].ivalue,
//					   (struct sockaddr*)&addr,
//					   (socklen_t*)&addrLen ) == 0 ) {
////		ret_s = new_kString(_ctx, fromSockaddr(addr));
//		ret_s = new_T(fromSockaddr(addr));
//	}
//	RETURN_( ret_s );
//}
//
////## int CSock.optget(int socket, int option);
//KMETHOD CSock_optget(CTX, ksfp_t* sfp _RIX)
//{
//	int val;
//	int valLen = sizeof(val);
//
//	int ret = getsockopt((int)sfp[1].ivalue,
//							SOL_SOCKET,
//							(int)sfp[2].ivalue,
//							&val,
//							(socklen_t*)&valLen);
//	if( ret != -1 ) {
//		ret = val;
//	}
//
//	RETURNi_(ret);
//}
//
////## int CSock.optset(int socket, int option, int value);
//KMETHOD CSock_optset(CTX, ksfp_t* sfp _RIX)
//{
//	RETURNi_( setsockopt((int)sfp[1].ivalue,
//							SOL_SOCKET,
//							(int)sfp[2].ivalue,
//							&sfp[3].ivalue,
//							sizeof(sfp[3].ivalue)) );
//}
//
////## String CSock.peername(int socket);
//KMETHOD CSock_peername(CTX, ksfp_t* sfp _RIX)
//{
//	kString *ret_s = (kString*) K_NULL;//KNH_TNULL(String);
//	struct sockaddr_in addr;
//	int addrLen = sizeof(addr);
//
//	memset(&addr, 0, addrLen);
//	if( getpeername( (int)sfp[1].ivalue,
//					   (struct sockaddr*)&addr,
//					   (socklen_t*)&addrLen ) == 0 ) {
////		ret_s = new_String(_ctx, fromSockaddr(addr));
//		ret_s = new_T(fromSockaddr(addr));
//	}
//
//	RETURN_( ret_s );
//}
//
////## String CSock.perror();
//KMETHOD CSock_perror(CTX, ksfp_t* sfp _RIX)
//{
//	RETURN_(new_T(strerror(errno)));
////	RETURN_( new_String(_ctx, strerror(errno)) );
//}
//
////## int CSock.recv(int socket, byte[] buffer, int flags);
///*
//KMETHOD CSock_recv(CTX, ksfp_t* sfp _RIX)
//{
//	kBytes *ba  = sfp[2].ba;
//
//	RETURNi_( recv((int)sfp[1].ivalue,
//					 ba->bu.buf,
//					 ba->bu.len,
//					 (int)sfp[3].ivalue) );
//}
//*/
//
////## int CSock.recvfrom(int socket, byte[] buffer, int flags, String remoteInfo);
//
///*
//KMETHOD CSock_recvfrom(CTX, ksfp_t* sfp _RIX)
//{
//	int ret = -1;
//	kBytes *ba  = sfp[2].ba;
//	struct sockaddr_in addr;
//	toSockaddr( &addr,
////				  String_to(char*, sfp[4]),
//				  (char*)sfp[4].s,
//				  (int)sfp[5].ivalue,
//				  (int)sfp[6].ivalue );
//	int addrLen = sizeof(addr);
//
//	if( (ret=recvfrom( (int)sfp[1].ivalue,
//						  ba->bu.buf,
//						  ba->bu.len,
//						  (int)sfp[3].ivalue,
//						  (struct sockaddr *)&addr,
//						  (socklen_t*)&addrLen)) >= 0 ) {
//		char2string(_ctx, fromSockaddr(addr), sfp[4].s );
//	}
//
//	RETURNi_( ret );
//}
//*/
//
////## int CSock.select(int[] readsock, int[] writesock, int[] exceptsock, long timeoutSec, long timeoutUSec);
//KMETHOD CSock_select(CTX, ksfp_t* sfp _RIX)
//{
//	int ret;
//	kArray *a1 = sfp[1].a;
//	kArray *a2 = sfp[2].a;
//	kArray *a3 = sfp[3].a;
//	int nfd = getNfd( a1, a2, a3 );
//
//	fd_set rfds, wfds, efds;
//	fd_set *rfd = toFd( &rfds, a1 );
//	fd_set *wfd = toFd( &wfds, a2 );
//	fd_set *efd = toFd( &efds, a3 );
//
//	struct timeval tv;
//    tv.tv_sec  = (long)sfp[4].ivalue;
//    tv.tv_usec = (long)sfp[5].ivalue;
//
//    ret = select(nfd+1, rfd, wfd, efd, &tv);
//    if ( ret > 0 ) {
//    	fromFd(_ctx, rfd, a1 );
//    	fromFd(_ctx, wfd, a2 );
//    	fromFd(_ctx, efd, a3 );
//    } else {
//    	// TODO::error or timeout is socket list all clear [pending]
//    	kArray_clear(a1, 0);
//    	kArray_clear(a2, 0);
//    	kArray_clear(a3, 0);
//     }
//
//    RETURNi_( ret );
//}
//
////## int CSock.send(int socket, byte[] message, int flags);
///*
//KMETHOD CSock_send(CTX, ksfp_t* sfp _RIX)
//{
//	kBytes *ba = sfp[2].ba;
//
//	// Broken Pipe Signal Mask
//#ifndef __APPLE__
//	__sighandler_t oldset = signal(SIGPIPE, SIG_IGN);
//#else
//	sig_t oldset = signal(SIGPIPE, SIG_IGN);
//#endif
//	int ret = send( (int)sfp[1].ivalue,
//					  ba->bu.buf,
//					  ba->bu.len,
//					  (int)sfp[3].ivalue );
//
//	if ( oldset != SIG_ERR )
//		signal(SIGPIPE, oldset);
//
//	RETURNi_( ret );
//}
//*/
//
////## int CSock.sendto(int socket, byte[] message, int flags, String dstIP, int dstPort, int family);
///*
//KMETHOD CSock_sendto(CTX, ksfp_t* sfp _RIX)
//{
//	kBytes *ba = sfp[2].ba;
//	struct sockaddr_in addr;
//	toSockaddr( &addr,
////				  String_to(char*, sfp[4]),
//				  (char*)sfp[4].s,
//				  (int)sfp[5].ivalue,
//				  (int)sfp[6].ivalue );
//
//	// Broken Pipe Signal Mask
//#ifndef __APPLE__
//	__sighandler_t oldset = signal(SIGPIPE, SIG_IGN);
//#else
//	sig_t oldset = signal(SIGPIPE, SIG_IGN);
//#endif
//	int ret = sendto(	(int)sfp[1].ivalue,
//						ba->bu.buf,
//						ba->bu.len,
//						(int)sfp[3].ivalue,
//						(struct sockaddr *)&addr,
//						sizeof(struct sockaddr)	);
//	if ( oldset != SIG_ERR )
//		signal(SIGPIPE, oldset);
//
//	RETURNi_( ret );
//}
//*/
//
////## int CSock.shutdown(int socket, int how);
//KMETHOD CSock_shutdown(CTX, ksfp_t* sfp _RIX)
//{
//	RETURNi_( shutdown((int)sfp[1].ivalue, (int)sfp[2].ivalue) );
//}
//
////## int CSock.sockatmark(int socket);
//KMETHOD CSock_sockatmark(CTX, ksfp_t* sfp _RIX)
//{
//	RETURNi_( sockatmark((int)sfp[1].ivalue) );
//}
//
////## int CSock.socket(int family, int type, int protocol);
//KMETHOD CSock_socket(CTX, ksfp_t* sfp _RIX)
//{
//	RETURNi_( socket((int)sfp[1].ivalue, (int)sfp[2].ivalue, (int)sfp[3].ivalue) );
//}
//
////## int CSock.socketpair(int family, int type, int protocol, int[] pairCSock);
//KMETHOD CSock_socketpair(CTX, ksfp_t* sfp _RIX)
//{
//	int ret = -2;
//	kArray *a = sfp[4].a;
//	if( a->size >= 2 ) {
//		int pairFd[2];
//		if( (ret = socketpair( (int)sfp[1].ivalue,
//								  (int)sfp[2].ivalue,
//								  (int)sfp[3].ivalue,
//								  pairFd ) ) == 0 ) {
//			a->ilist[0] = pairFd[0];
//			a->ilist[1] = pairFd[1];
//		}
//	}
//	RETURNi_( ret );
//}
//
//// --------------------------------------------------------------------------
//#define _Public   kMethod_Public
//#define _Static	  kMethod_Static
//#define _Const    kMethod_Const
//#define _Coercion kMethod_Coercion
//#define _F(F)   (intptr_t)(F)
//
//static	kbool_t socket_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
//{
//	ksocketshare_t *base = (ksocketshare_t *)KNH_ZMALLOC(sizeof(ksocketshare_t));
//	base->h.name		= "socket";
//	base->h.setup		= ksocketshare_setup;
//	base->h.reftrace	= ksocketshare_reftrace;
//	base->h.free		= ksocketshare_free;
//	ksetModule(MOD_SOCKET, &base->h, pline);
//	base->cCSock		= kaddClassDef(NULL, &CSockDef, pline);
//
//	int FN_x = FN_("x");
//	intptr_t methoddata[] = {
//		_Public, _F(CSock_accept), TY_CSock, TY_Int, MN_("accept"), 1, TY_String, FN_x,
//		DEND,
//	};
//	kloadMethodData(ks, methoddata);
//	KDEFINE_INT_CONST IntData[] = {
//			{"PF_LOCAL",TY_Int, PF_LOCAL},
//			{"PF_UNIX", TY_Int, PF_UNIX},
//			{"PF_INET", TY_Int, PF_INET},
//			{"PF_INET6", TY_Int, PF_INET6},
//			{"PF_APPLETALK", TY_Int, PF_APPLETALK},
//#ifndef __APPLE__
//			{"PF_PACKET", TY_Int, PF_PACKET},
//#endif
//			{"AF_LOCAL", TY_Int, AF_LOCAL},
//			{"AF_UNIX", TY_Int, AF_UNIX},
//			{"AF_INET", TY_Int, AF_INET},
//			{"AF_INET6", TY_Int, AF_INET6},
//			{"AF_APPLETALK", TY_Int, AF_APPLETALK},
//#ifndef __APPLE__
//			{"AF_PACKET", TY_Int, AF_PACKET},
//#endif
//			// Types of sockets
//			{"SOCK_STREAM", TY_Int, SOCK_STREAM},
//			{"SOCK_DGRAM", TY_Int, SOCK_DGRAM},
//			{"SOCK_RAW", TY_Int, SOCK_RAW},
//			{"SOCK_RDM", TY_Int, SOCK_RDM},
//			// send & recv flags
//			{"MSG_OOB", TY_Int, MSG_OOB},
//			{"MSG_PEEK", TY_Int, MSG_PEEK},
//			{"MSG_DONTROUTE", TY_Int, MSG_DONTROUTE},
//			{"MSG_OOB", TY_Int, MSG_OOB},
//			{"MSG_TRUNC", TY_Int, MSG_TRUNC},
//			{"MSG_DONTWAIT", TY_Int, MSG_DONTWAIT},
//			{"MSG_EOR", TY_Int, MSG_EOR},
//			{"MSG_WAITALL", TY_Int, MSG_WAITALL},
//#ifndef	__APPLE__
//			{"MSG_CONFIRM", TY_Int, MSG_CONFIRM},
//			{"MSG_ERRQUEUE", TY_Int, MSG_ERRQUEUE},
//			{"MSG_NOSIGNAL", TY_Int, MSG_NOSIGNAL},
//			{"MSG_MORE", TY_Int, MSG_MORE},
//#endif
//			// socket options
//			{"SO_REUSEADDR", TY_Int, SO_REUSEADDR},
//			{"SO_TYPE", TY_Int, SO_TYPE},
//			{"SO_ERROR", TY_Int, SO_ERROR},
//			{"SO_DONTROUTE", TY_Int, SO_DONTROUTE},
//			{"SO_BROADCAST", TY_Int, SO_BROADCAST},
//			{"SO_SNDBUF", TY_Int, SO_SNDBUF},
//			{"SO_RCVBUF", TY_Int, SO_RCVBUF},
//			{"SO_KEEPALIVE", TY_Int, SO_KEEPALIVE},
//			{"SO_OOBINLINE", TY_Int, SO_OOBINLINE},
//#ifndef __APPLE__
//			{"SO_NO_CHECK", TY_Int, SO_NO_CHECK},
//			{"SO_PRIORITY", TY_Int, SO_PRIORITY},
//#endif
//			{"SHUT_RD", TY_Int, SHUT_RD},
//			{"SHUT_WR", TY_Int, SHUT_WR},
//			{"SHUT_RDWR", TY_Int, SHUT_RDWR},
//			{}
//	};
//	kloadConstData(ks, IntData, pline);
//	return true;
//}
//
//static kbool_t socket_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
//{
//	return true;
//}
//
//static kbool_t socket_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
//{
//	return true;
//}
//
//static kbool_t socket_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
//{
//	return true;
//}
//
//KPACKDEF* socket_init(void)
//{
//	static KPACKDEF d = {
//		KPACKNAME("socket", "1.0"),
//		.initPackage = socket_initPackage,
//		.setupPackage = socket_setupPackage,
//		.initKonohaSpace = socket_initKonohaSpace,
//		.setupKonohaSpace = socket_setupKonohaSpace,
//	};
//	return &d;
//}
//
//#undef new_T
