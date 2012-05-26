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

#include<konoha2/konoha2.h>
#include<konoha2/sugar.h>
#include <unistd.h>
#include <signal.h>

//## @Static @Public Int System.getpid();

static KMETHOD System_getPid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getpid());
}

static KMETHOD System_getPpid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getppid());
}

static KMETHOD System_getUid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getuid());
}

static KMETHOD System_geteUid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(geteuid());
}

static KMETHOD System_getGid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getgid());
}

static KMETHOD System_geteGid(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getegid());
}

static KMETHOD System_getPgid(CTX, ksfp_t *sfp _RIX)
{
	int pid = sfp[1].ivalue;
	int gid = getpgid(pid);
	RETURNi_(gid);
}

static KMETHOD System_chdir(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	const char *dir = S_text(s);
	int ret = chdir(dir);
	RETURNi_(ret);
}

static KMETHOD System_fchdir(CTX, ksfp_t *sfp _RIX)
{
	int ch = fchdir(sfp[1].ivalue);
	RETURNi_(ch);
}

static KMETHOD System_chroot(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	const char *root = S_text(s);
	int ret = chroot(root);
	RETURNi_(ret);
}

static KMETHOD System_getPriority(CTX, ksfp_t *sfp _RIX)
{
	int arg1 = sfp[1].ivalue;
	int arg2 = sfp[2].ivalue;
	int ret = getpriority(arg1, arg2);
	RETURNi_(ret);
}

static KMETHOD System_setPriority(CTX, ksfp_t *sfp _RIX)
{
	int arg1 = sfp[1].ivalue;
	int arg2 = sfp[2].ivalue;
	int arg3 = sfp[3].ivalue;
	int ret = setpriority(arg1, arg2, arg3);
	RETURNi_(ret);
}

static KMETHOD System_getGroups(CTX, ksfp_t *sfp _RIX)
{
	int size = kArray_size(sfp[2].a);
	kArray *list = sfp[2].a;
	int ret = getgroups(size, list);
	RETURNi_(ret);
}

static KMETHOD System_setGroups(CTX, ksfp_t *sfp _RIX)
{
	int size = kArray_size(sfp[2].a);
	kArray *list = sfp[2].a;
	int ret = setgroups(size, list);
	RETURNi_(ret);
}

static KMETHOD System_Link(CTX, ksfp_t *sfp _RIX)
{
	kString *s1 = sfp[1].s;
	kString *s2 = sfp[2].s;
	const char *oldpath = S_text(s1);
	const char *newpath = S_text(s2);
	int ret = link(oldpath, newpath);
	RETURNi_(ret);
}

static KMETHOD System_Rename(CTX, ksfp_t *sfp _RIX)
{
	kString *s1 = sfp[1].s;
	kString *s2 = sfp[2].s;
	const char *oldpath = S_text(s1);
	const char *newpath = S_text(s2);
	int ret = rename(oldpath, newpath);
	RETURNi_(ret);
}

static KMETHOD System_Rmdir(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	const char *pathname = S_text(s);
	int ret = rmdir(pathname);
	RETURNi_(ret);
}

static KMETHOD System_symLink(CTX, ksfp_t *sfp _RIX)
{
	kString *s1 = sfp[1].s;
	kString *s2 = sfp[2].s;
	const char *oldpath = S_text(s1);
	const char *newpath = S_text(s2);
	int ret = symlink(oldpath, newpath);
	RETURNi_(ret);
}

static KMETHOD System_readLink(CTX, ksfp_t *sfp _RIX)
{
	kString *s1 = sfp[1].s;
	kString *s2 = sfp[2].s;
	const char *pathname = S_text(s1);
	const char *buf = S_text(s2);
	size_t bufsize = strlen(buf);
	ssize_t ret = readlink(pathname, buf, bufsize);
	RETURNi_(ret);
}

static KMETHOD System_Chown(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	const char *pathname = S_text(s);
	int owner = sfp[2].ivalue;
	int group = sfp[3].ivalue;
	int ret = chown(pathname, owner, group);
	RETURNi_(ret);
}

static KMETHOD System_lChown(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	const char *pathname = S_text(s);
	int owner = sfp[2].ivalue;
	int group = sfp[3].ivalue;
	int ret = lchown(pathname, owner, group);
	RETURNi_(ret);
}

static KMETHOD System_fChown(CTX, ksfp_t *sfp _RIX)
{
	int fd = sfp[1].ivalue;
	int owner = sfp[2].ivalue;
	int group = sfp[3].ivalue;
	int ret = fchown(fd, owner, group);
	RETURNi_(ret);
}

static KMETHOD System_Access(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	const char *dir = S_text(s);
	int mode = sfp[2].ivalue;
	int ret = access(dir, mode);
	RETURNi_(ret);
}

static KMETHOD System_Flock(CTX, ksfp_t *sfp _RIX)
{
	int arg1 = sfp[1].ivalue;
	int arg2 = sfp[2].ivalue;
	int ret = flock(arg1, arg2);
	RETURNi_(ret);
}

static KMETHOD System_Sync(CTX, ksfp_t *sfp _RIX)
{
	sync();
	RETURNvoid_();
}

static KMETHOD System_Fsync(CTX, ksfp_t *sfp _RIX)
{
	int fd = sfp[1].ivalue;
	int ret = fsync(fd);
	RETURNi_(ret);
}

static KMETHOD System_getPagesize(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(getpagesize());
}

static KMETHOD System_Kill(CTX, ksfp_t *sfp _RIX)
{
	int pid = sfp[1].ivalue;
	int sig = sfp[2].ivalue;
	int ret = kill(pid, sig);
	RETURNi_(ret);
}

static KMETHOD System_getHostname(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	char *name = S_text(s);
	size_t len = strlen(name);
	int ret = gethostname(name, len);
	RETURNi_(ret);
}

static KMETHOD System_setHostname(CTX, ksfp_t *sfp _RIX)
{
	kString *s = sfp[1].s;
	const char *name = S_text(s);
	size_t len = strlen(name);
	int ret = sethostname(name, len);
	RETURNi_(ret);
}

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Static   kMethod_Static
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)

#define _KVi(T) #T, TY_Int, T

static	kbool_t process_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	intptr_t MethodData[] = {
		_Public|_Static, _F(System_getPid), TY_Int, TY_System, MN_("getPid"), 0,
		_Public|_Static, _F(System_getPpid), TY_Int, TY_System, MN_("getPpid"), 0,
		_Public|_Static, _F(System_getUid), TY_Int, TY_System, MN_("getUid"), 0,
		_Public|_Static, _F(System_geteUid), TY_Int, TY_System, MN_("geteUid"), 0,
		_Public|_Static, _F(System_getGid), TY_Int, TY_System, MN_("getGid"), 0,
		_Public|_Static, _F(System_geteGid), TY_Int, TY_System, MN_("geteGid"), 0,
		_Public|_Static, _F(System_getPgid), TY_Int, TY_System, MN_("getPgid"), 1, TY_Int, FN_("pid"),
		_Public|_Static, _F(System_chdir), TY_Int, TY_System, MN_("chdir"), 1, TY_String, FN_("chdir"),
		_Public|_Static, _F(System_fchdir), TY_Int, TY_System, MN_("fchdir"), 1, TY_Int, FN_("fchdir"),
		_Public|_Static, _F(System_chroot), TY_Int, TY_System, MN_("chroot"), 1, TY_String, FN_("chroot"),
		_Public|_Static, _F(System_getPriority), TY_Int, TY_System, MN_("getPriority"), 2, TY_Int, FN_("Priorityarg1"), TY_Int, FN_("Priorityarg2"),
		_Public|_Static, _F(System_setPriority), TY_Int, TY_System, MN_("setPriority"), 3, TY_Int, FN_("Priorityarg1"), TY_Int, FN_("Priorityarg2"), TY_Int, FN_("Priorityarg3"),
		_Public|_Static, _F(System_getGroups), TY_Int, TY_System, MN_("getGroups"), 2, TY_Int, FN_("getgroupsarg1"), TY_Array, FN_("getgroupsarg2"),
		_Public|_Static, _F(System_setGroups), TY_Int, TY_System, MN_("setGroups"), 2, TY_Int, FN_("setgroupsarg1"), TY_Array, FN_("setgroupsarg2"),
		_Public|_Static, _F(System_Link), TY_Int, TY_System, MN_("Link"), 2, TY_String, FN_("linkarg1"), TY_String, FN_("linkarg2"),
		_Public|_Static, _F(System_Rename), TY_Int, TY_System, MN_("Rename"), 2, TY_String, FN_("renamearg1"), TY_String, FN_("renamearg2"),
		_Public|_Static, _F(System_Rmdir), TY_Int, TY_System, MN_("Rmdir"), 1, TY_String, FN_("rmdirarg"),
		_Public|_Static, _F(System_symLink), TY_Int, TY_System, MN_("symLink"), 2, TY_String, FN_("symlinkarg1"), TY_String, FN_("symlikarg2"),
		_Public|_Static, _F(System_readLink), TY_Int, TY_System, MN_("readLink"), 3, TY_String, FN_("readlinkarg1"), TY_String, FN_("readlikarg2"), TY_Int, FN_("readlinkarg3"),
		_Public|_Static, _F(System_Chown), TY_Int, TY_System, MN_("Chown"), 3, TY_String, FN_("chownarg1"), TY_Int, FN_("chownarg2"), TY_Int, FN_("chownarg3"),
		_Public|_Static, _F(System_lChown), TY_Int, TY_System, MN_("lChown"), 3, TY_String, FN_("lchownarg1"), TY_Int, FN_("lchownarg2"), TY_Int, FN_("lchownarg3"),
		_Public|_Static, _F(System_fChown), TY_Int, TY_System, MN_("fChown"), 3, TY_Int, FN_("fchownarg1"), TY_Int, FN_("fchownarg2"), TY_Int, FN_("fchownarg3"),
		_Public|_Static, _F(System_Access), TY_Int, TY_System, MN_("Access"), 2, TY_String, FN_("accessarg1"), TY_Int, FN_("accessarg2"),
		_Public|_Static, _F(System_Flock), TY_Int, TY_System, MN_("Flock"), 2, TY_Int, FN_("flockarg1"), TY_Int, FN_("flockarg2"),
		_Public|_Static, _F(System_Sync), TY_void, TY_System, MN_("Sync"), 0,
		_Public|_Static, _F(System_Fsync), TY_Int, TY_System, MN_("Fsync"), 1, TY_Int, FN_("fsynarg1"),
		_Public|_Static, _F(System_getPagesize), TY_Int, TY_System, MN_("getPagesize"), 0,
		_Public|_Static, _F(System_Kill), TY_Int, TY_System, MN_("Kill"), 2, TY_Int, FN_("killarg1"), TY_Int, FN_("killarg2"),
		_Public|_Static, _F(System_getHostname), TY_Int, TY_System, MN_("getHostname"), 2, TY_String, FN_("gethostnamearg1"), TY_Int, FN_("gethostnamearg2"),
		_Public|_Static, _F(System_setHostname), TY_Int, TY_System, MN_("setHostname"), 2, TY_String, FN_("sethostnamearg1"), TY_Int, FN_("sethostnamearg2"),
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	KDEFINE_INT_CONST IntData[] = {
		{_KVi(SIGHUP)},
		{_KVi(SIGINT)},
		{_KVi(SIGABRT)},
		{_KVi(SIGKILL)},
		{}
	};
	kKonohaSpace_loadConstData(kmodsugar->rootks, IntData, 0);
	return true;
}

static kbool_t process_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t process_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t process_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

KDEFINE_PACKAGE* process_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("package.posix", "1.0"),
		KPACKLIB("POSIX.1", "1.0"),
		.initPackage = process_initPackage,
		.setupPackage = process_setupPackage,
		.initKonohaSpace = process_initKonohaSpace,
		.setupKonohaSpace = process_setupKonohaSpace,
	};
	return &d;
}
