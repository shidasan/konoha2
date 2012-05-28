/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/


#ifndef FD_GLUE_H_
#define FD_GLUE_H_


/* ======================================================================== */
/* FILE low-level*/

// TODO: functions below will return integer which indecates file descriptor

//## @Native int System.lseek(int fd, int offset, int whence)
static KMETHOD System_lseek(CTX, ksfp_t *sfp _RIX)
{
	int fd = sfp[1].ivalue;
	int offset = sfp[2].ivalue;
	int whence = sfp[3].ivalue;
	off_t ret_offset = lseek(fd, offset, whence);
	if (ret_offset == -1) {
		ktrace(_DataFault,
				KEYVALUE_s("@", "lseek"),
				KEYVALUE_u("offset", offset),
				KEYVALUE_u("whence", whence),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNi_((int)ret_offset);
}

//## boolean System.truncate(int fd, int length)
static KMETHOD System_truncate(CTX, ksfp_t *sfp _RIX)
{
	int fd = sfp[1].ivalue;
	int length = sfp[2].ivalue;
	int ret = ftruncate(fd, length);
	if (ret != 0) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "ftruncate"),
				KEYVALUE_u("length", length),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

//## boolean System.chmod(int fd, int length)
static KMETHOD System_chmod(CTX, ksfp_t *sfp _RIX)
{
	int fd = sfp[1].ivalue;
	int mode = sfp[2].ivalue;
	int ret = fchmod(fd, mode);
	if (ret != -1) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "fchmod"),
				KEYVALUE_u("mode", mode),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

//## boolean System.chown(int fd, int owner, int group)
static KMETHOD System_chown(CTX, ksfp_t *sfp _RIX)
{
	int fd = sfp[1].ivalue;
	uid_t owner = (uid_t)sfp[2].ivalue;
	gid_t group = (gid_t)sfp[3].ivalue;

	int ret = fchown(fd, owner, group);
	if (ret != -1) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "fchown"),
				KEYVALUE_u("owner", owner),
				KEYVALUE_u("group", group),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

// TODO: isn't ioctl difficult for script users? should we support this?
//## @Native int File.ioctl(int request, String[] args)
//staic KMETHOD File_ioctl(CTX, ksfp_t *sfp _RIX)
//{
//	kFile *file = (kFile*)sfp[0].o;
//	FILE *fp = file->fp;
//	int request  = Int_to(int, sfp[1]);
//	char *argp = String_to(char*, sfp[2]);
//	if (fp == NULL) RETURNb_(0);
//	int fd = fileno(fp);
//	if (fd == -1) {
//		RETURNb_(0);
//	}
//	int ret = ioctl(fd, request, argp);
//	KNH_NTRACE2(ctx, "ioctl", ret != -1 ? K_OK : K_PERROR, KNH_LDATA(
//				));
//	RETURNb_(ret != -1);
//}

// NOTE: sys_flock can use for a file, only for
//## @Native boolean System.flock(int fd, int opretaion);
static KMETHOD System_flock(CTX, ksfp_t *sfp _RIX)
{
	int fd = sfp[1].ivalue;
	int operation = sfp[2].ivalue;
	int ret = flock(fd, operation);
	if (ret == -1) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "flock"),
				KEYVALUE_u("operation", operation),
				KEYVALUE_s("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

//## @Native boolean System.sync(int fd);
static KMETHOD System_sync(CTX, ksfp_t *sfp _RIX)
{
	int fd = sfp[1].ivalue;
	int ret =  fsync(fd);
	if (ret == -1) {
		ktrace(_SystemFault,
				KEYVALUE_s("@", "fsync"),
				KEYVALUE_p("errstr", strerror(errno))
		);
	}
	RETURNb_(ret == 0);
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _Im kMethod_Immutable
#define _F(F)   (intptr_t)(F)

static kbool_t fd_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	intptr_t MethodData[] = {
		_Public|_Const|_Im, _F(System_lseek), TY_Int, TY_System, MN_("lseek"), 3, TY_Int, FN_("fd"), TY_Int, FN_("offset"), TY_Int, FN_("whence"),
		_Public|_Const|_Im, _F(System_truncate), TY_Boolean, TY_System, MN_("truncate"), 2, TY_Int, FN_("fd"), TY_Int, FN_("length"),
		_Public|_Const|_Im, _F(System_chmod), TY_Boolean, TY_System, MN_("chmod"), 2, TY_Int, FN_("fd"), TY_Int, FN_("length"),
		_Public|_Const|_Im, _F(System_chown), TY_Boolean, TY_System, MN_("chown"), 3, TY_Int, FN_("fd"), TY_Int, FN_("owner"), TY_Int, FN_("group"),
		_Public|_Const|_Im, _F(System_flock), TY_Boolean, TY_System, MN_("flock"), 2, TY_Int, FN_("fd"), TY_Int, FN_("operation"),
		_Public|_Const|_Im, _F(System_sync), TY_Boolean, TY_System, MN_("sync"), 1, TY_Int, FN_("fd"),
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t fd_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t fd_initKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t fd_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}



#endif /* FD_GLUE_H_ */
