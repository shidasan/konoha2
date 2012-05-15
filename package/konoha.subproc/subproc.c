/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c)  2010-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 *
 * You may choose one of the following two licenses when you use konoha.
 * See www.konohaware.org/license.html for further information.
 *
 * (1) GNU Lesser General Public License 3.0 (with KONOHA_UNDER_LGPL3)
 * (2) Konoha Software Foundation License 1.0
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

// **************************************************************************
// LIST OF CONTRIBUTERS
//  kimio - Kimio Kuramitsu, Yokohama National University, Japan
//  goccy - Masaaki Goshima, Yokohama National University, Japan
// **************************************************************************

#include <konoha1.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <sys/time.h>

#ifndef K_USING_MACOSX_
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27)
#define __USE_LOCAL_PIPE2__
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined (K_USING_MACOSX_) && !defined(__USE_LOCAL_PIPE2__)
extern int pipe2 (int __pipedes[2], int __flags);
#endif
extern int sigignore (int __sig);

// child process IO mode
#define M_DEFAULT          -2              // initialization state
#define M_NREDIRECT        -1              // parent process succession
#define M_PIPE             0               // pipe
#define M_STDOUT           1               // standard OUT
#define M_FILE             2               // file

// child process status code
#define S_RUNNING          -300            // running
#define S_PREEXECUTION     -400            // preexecution
#define S_TIMEOUT          -500            // tiomeout
#define S_EXIT             0               // terminate

// subproc macro
#define MAXARGS            128             // the number maximum of parameters for spSplit
#define BUFSIZE            64 * 1024       // the reading buffer size maximum for pipe
#define DELAY              1000            // the adjustment value at the time of signal transmission
#define DEF_TIMEOUT        60 * 1000       // default timeout value
#define ONEXEC(p)          ( (p != NULL) && (p->cpid > 0) ) ? 1 : 0
#define PREEXEC(p)         ( (p != NULL) && (p->cpid == -1) ) ? 1 : 0
#define WORD2INT(val)      (sizeof(val)==8) ? (val&0x7FFFFFFF)|((val>>32)&0x80000000) : val

/* ======================================================================== */
// [private functions]

typedef struct {
    int mode;                              // the kind of identifier
    FILE* fp;                              // file stream pointer
} pfd_t;                                   // pipe fd structure

typedef struct {
    kbool_t shell;                         // shell mode [true/false]
    kbool_t closefds;                      // closefds   [true/false]
    kbool_t bg;                            // bg mode    [true/false]
    kArray *env;                           // child process environment
    kString *command;                      // child process command
    kString *cwd;                          // child process current working directory
    pfd_t r;                               // child process output identifier 
    pfd_t w;                               // child process input identifier 
    pfd_t e;                               // child process error identifier 
    int cpid;                              // child process ID
    int bufferSize;                        // buffer size (unused)
    int timeout;                           // child process timeout value
    int status;                            // waitpid status
    int timeoutKill;                       // child process Timeout ending flag [true/false]
} subprocData_t;                           // subproc data structure

static knh_IntData_t SubprocConstInt[] = {
    {"S_RUNNING"       , S_RUNNING        },
    {"S_TIMEOUT"       , S_TIMEOUT        },
    {"S_PREEXECUTION"  , S_PREEXECUTION   },
    {"S_EXIT"          , S_EXIT           },
    {NULL, 0}, //necessary for checking rnd of definition
};

static jmp_buf env;
static void alarmHandler(int sig) { siglongjmp(env, 1); }
static int fgPid;
static void keyIntHandler(int sig) { kill(fgPid, SIGINT); }

#if defined (K_USING_MACOSX_) || defined(__USE_LOCAL_PIPE2__)
// for fg & bg & exec & restart
static int pipe2( int *fd, int flags ) {
    int val;
    int p[2];
    if ( pipe(p) < 0 ) {
        return -1;
    }
    if ( (val=fcntl(p[0], F_GETFL, 0)) < 0 ) {
        goto L_ERR;
    }
    if ( fcntl(p[0], F_SETFL, val | flags) < 0 ) {
        goto L_ERR;
    }
    if ( (val=fcntl(p[1], F_GETFL, 0)) < 0 ) {
        goto L_ERR;
    }
    if ( fcntl(p[1], F_SETFL, val | flags) < 0 ) {
        goto L_ERR;
    }
    fd[0] = p[0];
    fd[1] = p[1];
    return 0;
L_ERR:;
    {
        close(p[0]);
        close(p[1]);
        return -1;
    }
}
#endif

// for argSplit & fg & bg & exec & restart
// ===========================================================================
// <example>
// char str[12], *args[2];
// strcpy(str, "Hello world");
// int param = spSplit( str, args );
// if ( param > 0 ) {
//    printf("param:%d args[0]:%s args[1]:%s\n", param, args[0], args[1]);
// } else {
//     printf("spSplit error[%d]\n", param);
// }
// ---------------------------------------------------------------------------
// output:
//     param:2 args[0]:Hello args[1]:world
// ===========================================================================
/**
 * @return number of parameter (zero or more)
 *         -1 is Internal Error
 *         -2 is the maximum error of the number of parameters
 */
static int spSplit(char* str, char* args[]) {

    if ( (str == NULL) || (args == NULL) ) {
        return -1;
    }
    int indx;
    char *cp = str;
    for (indx=0 ; indx<=MAXARGS ; indx++) {
        if ( indx == MAXARGS ) {
            return -2;
        } else if ( (args[indx] = strtok(cp, " ")) == NULL ) {
            break;
        } else {
            cp = NULL;
        }
    }
    // number of parameter
    return indx;
}

// for getIN & getOUT & getERR
/**
 * @return "konoha.posix.File" class id
 */
static kclass_t khn_getFileClass( CTX ctx ) {
    char *name = "konoha.posix.File";
    kbytes_t lname;
    lname.ubuf = (kchar_t*)name;
    lname.len = knh_strlen(lname.text);
    return knh_getcid(ctx, lname);
}

// for fg & bg & exec & restart
/**
 * @return child process id
 *         -1 is Internal Error
 */
static int knh_popen( CTX ctx, kString* command, subprocData_t *dtp, int defaultMode ) {

    if ( command == (kString*)KNH_NULVAL(CLASS_String) ) {
        return -1;
    }
    int pid  = -1;
    int rmode = (dtp->r.mode==M_DEFAULT) ? defaultMode : dtp->r.mode ;
    int wmode = (dtp->w.mode==M_DEFAULT) ? defaultMode : dtp->w.mode ;
    int emode = (dtp->e.mode==M_DEFAULT) ? defaultMode : dtp->e.mode ;
    int c2p[2];
    int p2c[2];
    int err[2];

    if ( rmode == M_PIPE ) {
        if ( pipe2(c2p, O_NONBLOCK) != 0 ) {
            return -1;
        }
    }
    if ( wmode == M_PIPE ) {
        if ( pipe(p2c) != 0 ) {
            close(c2p[0]); close(c2p[1]);
            return -1;
        }
    }
    if ( emode == M_PIPE ) {
        if ( pipe2(err, O_NONBLOCK) != 0 ) {
            close(c2p[0]); close(c2p[1]);
            close(p2c[0]); close(p2c[1]);
            return -1;
        }
    }
    if ( (pid=fork()) < 0) {
        // parent process illegal route
        if ( rmode == M_PIPE ) {
            close(c2p[0]); close(c2p[1]);
        }
        if ( wmode == M_PIPE ) {
            close(p2c[0]); close(p2c[1]);
        }
        if ( emode == M_PIPE ) {
            close(err[0]); close(err[1]);
        }
    } else if ( pid == 0 ) {
        // child process normal route
        if ( wmode == M_PIPE ) {
            close(0);
            dup2(p2c[0], 0);
            close(p2c[0]); close(p2c[1]);
        } else if ( wmode == M_FILE ) {
            close(0);
            dup2(fileno(dtp->w.fp), 0);
        }
        if ( rmode == M_PIPE ) {
            close(1);
            dup2(c2p[1], 1);
            close(c2p[0]); close(c2p[1]);
        } else if ( rmode == M_FILE ) {
            close(1);
            dup2(fileno(dtp->r.fp), 1);
        }
        if ( emode == M_PIPE ) {
            close(2);
            dup2(err[1], 2);
            close(err[0]); close(err[1]);
        } else if ( emode == M_FILE ) {
            close(2);
            dup2(fileno(dtp->e.fp), 2);
        } else if ( emode == M_STDOUT ) {
            close(2);
            dup2(1, 2);
        }
        if (dtp->closefds == 1) {
            // close other than 0, 1, and 2
            int cfd = 3;
            int maxFd = sysconf(_SC_OPEN_MAX);
            do {
                close(cfd);
            } while ( ++cfd < maxFd );
        }
//      setsid(); // separation from tty
        if ( dtp->cwd != (kString*)KNH_NULVAL(CLASS_String) ) {
            if ( chdir(dtp->cwd->str.text) != 0 ) {
                perror("knh_popen::chdir :");
                _exit(1);
            }
        }
        char *args[MAXARGS];
        if ( dtp->shell == 0 ) {
            // division of a commnad parameter
            if ( spSplit(command->str.buf, args) < 0 ) {
                args[0] = NULL;
            }
        }
        if ( dtp->env != (kArray*)KNH_NULVAL(CLASS_Array) ) {
            // division of a environment parameter
            kArray *a = dtp->env;
            int num = a->size;
            char *envs[num+1];
            int i;
            for(i=0 ; i<num ; i++) {
                envs[i] = (char*)S_totext(a->strings[i]);
            }
            envs[num] = NULL;
            if ( dtp->shell == 0 ) {
                execve(args[0], args, envs);
            } else {
                execle("/bin/sh", "sh", "-c", command->str.text, NULL, envs);
            }
        } else {
            if ( dtp->shell == 0 ) {
                execvp(args[0], args);
            } else {
                execlp("sh", "sh", "-c", command->str.text, NULL);
            }
        }
        perror("knh_popen :");
        _exit(1);
    } else {
        // parent process normal route
        if ( rmode == M_PIPE ) {
            dtp->r.fp = fdopen( c2p[0], "r" );
            close(c2p[1]);
        }
        if ( wmode == M_PIPE ) {
            dtp->w.fp = fdopen( p2c[1], "w" );
            close(p2c[0]);
        }
        if ( emode == M_PIPE ) {
            dtp->e.fp = fdopen( err[0], "r" );
            close(err[1]);
        }
    }
    return pid;
}

// for wait & fg & exec & communicate
/**
 * @return termination status of a child process 
 */
static int knh_wait( int pid, int bg, int timeout, int *status ) {

#ifndef __APPLE__
    __sighandler_t alarm_oldset  = SIG_ERR;
    __sighandler_t keyInt_oldset = SIG_ERR;
#else
    sig_t alarm_oldset  = SIG_ERR;
    sig_t keyInt_oldset = SIG_ERR;
#endif
    if ( timeout > 0 ) {
        if( sigsetjmp(env, 1) ) {
            // wait timeout return route
            setitimer(ITIMER_REAL, NULL, NULL);
            if ( alarm_oldset != SIG_ERR ) {
                signal(SIGALRM, alarm_oldset);
            } else {
                signal(SIGALRM, SIG_DFL);
            }
            return S_TIMEOUT;
        }
    }
    if ( bg != 1 ) {
        // SIGINT registration
        fgPid = pid;
        keyInt_oldset = signal(SIGINT, keyIntHandler);
    }
    if ( timeout > 0 ) {
        // SIGALRM registration
        struct itimerval its;
        its.it_value.tv_sec  = timeout / 1000;
        its.it_value.tv_usec = (timeout % 1000) * 1000;
        its.it_interval.tv_sec  = 0;
        its.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &its, NULL);
        alarm_oldset = signal(SIGALRM, alarmHandler);
    }
    int stat;
    waitpid( pid, &stat, WUNTRACED );
    if ( timeout > 0 ) {
        // SIGALRM release 
        setitimer(ITIMER_REAL, NULL, NULL);
        if ( alarm_oldset != SIG_ERR ) {
            signal(SIGALRM, alarm_oldset);
        } else {
            signal(SIGALRM, SIG_DFL);
        }
    }
    if ( bg != 1 ) {
        // SIGINT release 
        if ( keyInt_oldset != SIG_ERR ) {
            signal(SIGINT, keyInt_oldset);
        } else {
            signal(SIGINT, SIG_DFL);
        }
    }
    if ( status != NULL ) {
        *status = stat;
    }
    // return value creation
    if ( WIFSIGNALED(stat) ) {
        return WTERMSIG(stat) * -1;
    } else if ( WIFSTOPPED(stat) ) {
        return WSTOPSIG(stat) * -1;
    } else {
        return S_EXIT;
    }
}

// for fg & bg & restart
/**
 * @return in the case of foreground, it is start status of a child process
 *         in the case of background, it is termination status of a child process
 */
static int proc_start( CTX ctx, subprocData_t *sp ) {
    int ret = S_PREEXECUTION;
    int pid = knh_popen( ctx, sp->command, sp, M_NREDIRECT );
    if ( pid > 0 ) {
        sp->cpid  = pid;

        if (sp->bg != 1) {
            ret = knh_wait( sp->cpid, sp->bg, sp->timeout, &sp->status );
        } else {
            // nomal end status for bg
            ret = 0;
        }
    }
    return ret;
}

// for new & enablePipemodeXXX(false) & enableStandardXXX(false) & enableERR2StdOUT(false)
inline void initFd(pfd_t *p) {
    p->mode = M_DEFAULT;
    p->fp   = NULL;
}

// for setFileXXX & PipemodeXXX(true) & enableStandardXXX(true) & enableERR2StdOUT(true)
static void setFd(CTX ctx, pfd_t *p, int changeMode, FILE* ptr) {
    if ( ((p->mode == M_PIPE) || (p->mode == M_FILE)) && !(p->mode == changeMode) ) {
        // warning of the pipe or file mode overwrite
        char *msg = (p->mode==M_PIPE) ? "pipe has already set, but we overwrite it." :
                                        "file has already set, but we overwrite it." ;
        WARN_PackageMessage( ctx, msg );
    }
    p->mode = changeMode;
    p->fp = ptr;
}

// for exec & restart
static void clearFd(pfd_t *p) {
    if ( ( (p->mode == M_PIPE) || (p->mode == M_DEFAULT) ) && (p->fp != NULL) ) {
        // a file identification child does not close
        fclose(p->fp);
        p->fp = NULL;
    }
}

// for new
static void initData ( CTX ctx, subprocData_t* p ) {
    p->command     = (kString*)KNH_NULVAL(CLASS_String);
    p->cwd         = (kString*)KNH_NULVAL(CLASS_String);
    p->env         = (kArray*)KNH_NULVAL(CLASS_Array);
    p->cpid        = -1;
    p->closefds    = 0;
    p->bg          = 0;
    p->shell       = 0;
    p->timeout     = DEF_TIMEOUT;
    p->bufferSize  = 0;
    p->timeoutKill = 0;
    initFd(&p->r);
    initFd(&p->w);
    initFd(&p->e);
}

// for poll
static int getPidStatus( int pid, int *status ) {
    return waitpid(pid, status, WNOHANG | WUNTRACED | WCONTINUED );
}

// for Subproc_free & fg & exec & communicate & restart
static void killWait( int pid ) {
    int status;
    kill(pid, SIGKILL);
    usleep(DELAY);
    getPidStatus(pid, &status); // this wait is in order not to leave a zombie process. 
}

static void Subproc_init(CTX ctx, kRawPtr *po)
{
    po->rawptr = NULL;
}

static void Subproc_free(CTX ctx, kRawPtr *po)
{
    if (po->rawptr != NULL) {
        subprocData_t *p = (subprocData_t*)po->rawptr;
        if ( ONEXEC(p) ) {
            killWait(p->cpid);
        }
        KNH_FREE(ctx, po->rawptr, sizeof(subprocData_t));
        po->rawptr = NULL;
    }
}

/* ------------------------------------------------------------------------ */

DEFAPI(void) defSubproc(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
    cdef->name = "Subproc";
    cdef->init = Subproc_init;
    cdef->free = Subproc_free;
}

DEFAPI(void) constSubproc(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
    kapi->loadClassIntConst(ctx, cid, SubprocConstInt);
}

/* ======================================================================== */
// [KMETHODS]

//## Subproc Subproc.new(String cmd, boolean closefds);
KMETHOD Subproc_new(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)KNH_MALLOC( ctx, sizeof(subprocData_t) );
    KNH_NTRACE2(ctx, "subproc.new", (p != NULL) ? K_OK : K_PERROR, KNH_LDATA0 );
    if ( p != NULL ) {
        initData( ctx, p );
        p->command = (sfp[1].s->str.len > 0) ? new_String(ctx, sfp[1].s->str.text) :
                                               (kString*)KNH_NULVAL(CLASS_String);
        p->closefds = sfp[2].bvalue;
    }
    kRawPtr *po = new_ReturnCppObject(ctx, sfp, p, NULL/*ignored*/);
    RETURN_(po);
}

//## boolean Subproc.bg();
KMETHOD Subproc_bg(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = -1;
    if ( PREEXEC(p) ) {
        p->timeoutKill = 0;
        p->bg = 1;
        if ( (ret = proc_start(ctx, p)) != 0 ) {
            KNH_NTRACE2(ctx, "package.subproc.bg ", K_PERROR, KNH_LDATA0);
        }
    }
    RETURNb_( (ret == 0) );
}

//## @Throwable int Subproc.fg();
KMETHOD Subproc_fg(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = S_PREEXECUTION;
    if ( PREEXEC(p) ) {
        p->timeoutKill = 0;
        p->bg = 0;
        if ( (ret = proc_start(ctx, p)) == S_TIMEOUT ) {
            p->timeoutKill = 1;
            killWait(p->cpid);
            KNH_NTHROW2(ctx, sfp, "Script!!", "subproc.fg :: timeout", K_FAILED, KNH_LDATA0);
        }
    }
    RETURNi_( ret );
}

//## @Throwable String Subproc.exec(String data);
KMETHOD Subproc_exec(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    kString *ret_s = KNH_TNULL(String);
    if ( PREEXEC(p) ) {
        p->timeoutKill = 0;
        kString *command = (sfp[1].s->str.len > 0) ? new_String(ctx, sfp[1].s->str.text) :
                                                     (kString*)KNH_NULVAL(CLASS_String);
        int pid = knh_popen( ctx, command, p, M_PIPE );
        if ( pid > 0 ) {
            if ( knh_wait( pid, 0, p->timeout, NULL ) == S_TIMEOUT ) {
                p->timeoutKill = 1;
                killWait(pid);
                clearFd(&p->r);
                clearFd(&p->w);
                clearFd(&p->e);
                KNH_NTHROW2(ctx, sfp, "Script!!", "subproc.exec :: timeout", K_FAILED, KNH_LDATA0);
            } else if ( (p->r.mode == M_PIPE) || (p->r.mode == M_DEFAULT) ) {
                char buf[BUFSIZE];
                memset(buf, 0x00, sizeof(buf));
                if( fread(buf, sizeof(char), sizeof(buf)-1, p->r.fp) > 0 ) {
                    ret_s = new_String(ctx, buf);
                } else {
                    KNH_NTRACE2(ctx, "package.subprocess.exec.fread ", K_PERROR, KNH_LDATA0);
                }
                clearFd(&p->r);
                clearFd(&p->w);
                clearFd(&p->e);
            } else if ( p->r.mode == M_FILE ) {
                char *msg = " will be ignored.";
                char *cmd = sfp[1].s->str.buf;
                char mbuf[strlen(msg)+strlen(cmd)+1];
                snprintf(mbuf, sizeof(mbuf), "'%s'%s", cmd, msg);
                WARN_PackageMessage( ctx, mbuf );
            }
        } else {
            KNH_NTRACE2(ctx, "package.subproc.exec ", K_PERROR, KNH_LDATA0);
        }
    }
    RETURN_( ret_s );
}

//## @Throwable String[] Subproc.communicate(String input);
KMETHOD Subproc_communicate(CTX ctx, ksfp_t *sfp _RIX)
{
    kArray *ret_a =  KNH_TNULL(Array);
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;

    if ( ONEXEC(p) ) {
        if ( (p->w.mode == M_PIPE) && (sfp[1].ba->bu.len > 0) ) {
            kBytes *ba = sfp[1].ba;
            // The measure against panic, 
            // if "Broken Pipe" is detected at the time of writing.
#ifndef K_USING_MACOSX_
            __sighandler_t oldset = signal(SIGPIPE, SIG_IGN);
#else
            sig_t oldset = signal(SIGPIPE, SIG_IGN);
#endif
            if ( fwrite(ba->bu.buf, sizeof(char), ba->bu.len, p->w.fp) > 0 ) {
                fputc('\n', p->w.fp);
                fflush(p->w.fp);
                fsync(fileno(p->w.fp));
            } else {
                KNH_NTRACE2(ctx, "package.subproc.communicate ", K_PERROR, KNH_LDATA0);
            }
            if ( oldset != SIG_ERR ) {
                signal(SIGPIPE, oldset);
            }
        }
        if ( knh_wait( p->cpid, p->bg, p->timeout, &p->status ) == S_TIMEOUT ) {
            p->timeoutKill = 1;
            killWait(p->cpid);
            KNH_NTHROW2(ctx, sfp, "Script!!", "subproc.communicate :: timeout", K_FAILED, KNH_LDATA0);
        } else {
            ret_a = new_Array(ctx, CLASS_String, 0);
            if ( p->r.mode == M_PIPE ) {
                char buf[BUFSIZE];
                memset(buf, 0x00, sizeof(buf));
                if( fread(buf, sizeof(char), sizeof(buf)-1, p->r.fp) > 0 ) {
                    knh_Array_add(ctx, ret_a, new_String(ctx, buf));
                } else {
                    knh_Array_add(ctx, ret_a, KNH_NULL);
                    KNH_NTRACE2(ctx, "package.subprocess.communicate.fread ", K_PERROR, KNH_LDATA0);
                }
            } else {
                knh_Array_add(ctx, ret_a, KNH_NULL);
            }
            if ( p->e.mode == M_PIPE ) {
                char buf[BUFSIZE];
                memset(buf, 0x00, sizeof(buf));
                if( fread(buf, sizeof(char), sizeof(buf)-1, p->e.fp) > 0 ) {
                    knh_Array_add(ctx, ret_a, new_String(ctx, buf));
                } else {
                    knh_Array_add(ctx, ret_a, KNH_NULL);
                    KNH_NTRACE2(ctx, "package.subprocess.communicate.fread ", K_PERROR, KNH_LDATA0);
                }
            } else {
                knh_Array_add(ctx, ret_a, KNH_NULL);
            }
        }
    }
    RETURN_( ret_a );
}

//## int Subproc.poll();
KMETHOD Subproc_poll(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = S_PREEXECUTION;

    if ( ONEXEC(p) ) {
        if ( p->timeoutKill == 1 ) {
            ret = S_TIMEOUT;
        } else if ( getPidStatus( p->cpid, &p->status ) == 0 ) {
            ret = S_RUNNING;
        } else {
            if ( WIFSIGNALED(p->status) ) {
                ret = WTERMSIG(p->status) * -1;
            } else if ( WIFSTOPPED(p->status) ) {
                ret = WSTOPSIG(p->status) * -1;
            } else if (WIFCONTINUED(p->status) ) {
                ret = S_RUNNING;
            } else {
                ret = S_EXIT;
            }
        }
    } else {
        if ( (p != NULL) && (p->timeoutKill == 1) ) {
            ret = S_TIMEOUT;
        }
    }
    RETURNi_( ret );
}

//## int Subproc.wait();
KMETHOD Subproc_wait(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = S_PREEXECUTION;
    if ( ONEXEC(p) ) {
        if ( p->timeoutKill != 1 ) {
            ret = knh_wait( p->cpid, p->bg, p->timeout, &p->status );
        } else {
            ret = S_TIMEOUT;
        }
    }
    RETURNi_( ret );
}

//## boolean Subproc.sendSignal(int signal);
KMETHOD Subproc_sendSignal(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = -1;
    if ( ONEXEC(p) ) {
        if ( (ret=kill( p->cpid, sfp[1].ivalue )) != 0 ) {
            KNH_NTRACE2(ctx, "package.subproc.sendSignal ", K_PERROR, KNH_LDATA0);
        } else {
            usleep(DELAY);
        }
    }
    RETURNb_( ret == 0 );
}

//## boolean Subproc.terminate();
KMETHOD Subproc_terminate(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = -1;
    if ( ONEXEC(p) ) {
        if ( (ret=kill( p->cpid, SIGTERM )) != 0 ) {
            KNH_NTRACE2(ctx, "package.subproc.terminate ", K_PERROR, KNH_LDATA0);
        } else {
            usleep(DELAY);
        }
    }
    RETURNb_( ret == 0 );
}

//## boolean Subproc.kill(void);
KMETHOD Subproc_kill(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = -1;
    if ( ONEXEC(p) ) {
        if ( (ret=kill(p->cpid, SIGKILL)) != 0  ) {
            KNH_NTRACE2(ctx, "package.subproc.kill ", K_PERROR, KNH_LDATA0);
        } else {
            usleep(DELAY);
        }
    }
    RETURNb_( ret == 0 );
}

//## boolean Subproc.stop(void);
KMETHOD Subproc_stop(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = -1;
    if ( ONEXEC(p) ) {
        if ( (ret=kill(p->cpid, SIGSTOP)) != 0  ) {
            KNH_NTRACE2(ctx, "package.subproc.stop ", K_PERROR, KNH_LDATA0);
        } else {
            usleep(DELAY);
        }
    }
    RETURNb_( ret == 0 );
}

//## boolean Subproc.start();
KMETHOD Subproc_start(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = -1;
    if ( ONEXEC(p) ) {
        if ( (ret=kill(p->cpid, SIGCONT)) != 0  ) {
            KNH_NTRACE2(ctx, "package.subproc.start ", K_PERROR, KNH_LDATA0);
        } else {
            usleep(DELAY);
        }
    }
    RETURNb_( ret == 0 );
}

//## boolean Subproc.restart();
KMETHOD Subproc_restart(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = -1;
    if ( ONEXEC(p) ) {
        killWait(p->cpid);
        p->cpid = -1;
        p->timeoutKill = 0;
        clearFd(&p->r);
        clearFd(&p->w);
        clearFd(&p->e);
        if ( (ret = proc_start(ctx, p)) != 0 ) {
            if ( (p->bg != 1) && ( ret == S_TIMEOUT ) ) {
                p->timeoutKill = 1;
                killWait(p->cpid);
                KNH_NTHROW2(ctx, sfp, "Script!!", "subproc.restart :: timeout", K_FAILED, KNH_LDATA0);
            } else {
                KNH_NTRACE2(ctx, "package.subproc.restart ", K_PERROR, KNH_LDATA0);
            }
        }
    }
    RETURNb_( ret == 0 );
}

//## String[] Subproc.argSplit(String args);
KMETHOD Subproc_argSplit(CTX ctx, ksfp_t *sfp _RIX)
{
    kArray *ret_a =  KNH_TNULL(Array);
    kString *str = new_String(ctx, sfp[1].s->str.text);
    if ( str != NULL ) {
        char* args[MAXARGS];
        int paramNum = spSplit( str->str.buf, args );
        if ( paramNum > 0 ) {
            ret_a = new_Array(ctx, CLASS_String, 0);
            int indx;
            for (indx=0 ; indx<paramNum ; indx++) {
                knh_Array_add(ctx, ret_a, new_String(ctx, args[indx]));
            }
        }
    }
    RETURN_(ret_a);
}

//## @Restricted boolean Subproc.enableShellmode(boolean isShellmode);
KMETHOD Subproc_enableShellmode(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        p->shell = sfp[1].bvalue;
    }
    RETURNb_( ret );
}

//## boolean Subproc.setEnv(Map env);
KMETHOD Subproc_setEnv(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        kDictMap *env = (kDictMap *)sfp[1].o;
        int i;
        size_t msize = env->spi->size(ctx, env->mapptr);
        if ( p->env != (kArray*)KNH_NULVAL(CLASS_Array) ) {
            knh_Array_clear( ctx, p->env, 0 );
        }
        p->env = new_Array(ctx, CLASS_String, msize);
        for (i = 0; i < msize; i++) {
            kString *key = (kString *)knh_DictMap_keyAt(env, i);
            kString *val = (kString *)knh_DictMap_valueAt(env, i);
            char buf[key->str.len + val->str.len + 2];
            snprintf(buf, sizeof(buf), "%s=%s", key->str.buf, val->str.buf);
            knh_Array_add( ctx, p->env, new_String(ctx, buf) );
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.setCwd(String cwd);
KMETHOD Subproc_setCwd(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        p->cwd = new_String(ctx, sfp[1].s->str.text);
    }
    RETURNb_( ret );
}

//## boolean Subproc.setBufsize(int size);
KMETHOD Subproc_setBufsize(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        p->bufferSize = WORD2INT(sfp[1].ivalue);
    }
    RETURNb_( ret );
}

//## boolean Subproc.setFileIN(File in);
KMETHOD Subproc_setFileIN(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        ret = (sfp[1].p->rawptr != NULL);
        if ( ret ) {
            setFd(ctx, &p->w, M_FILE, (FILE*)sfp[1].p->rawptr);
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.setFileOUT(File out);
KMETHOD Subproc_setFileOUT(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        ret = (sfp[1].p->rawptr != NULL);
        if ( ret ) {
            setFd(ctx, &p->r, M_FILE, (FILE*)sfp[1].p->rawptr);
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.setFileERR(File err);
KMETHOD Subproc_setFileERR(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        ret = (sfp[1].p->rawptr != NULL);
        if ( ret ) {
            setFd(ctx, &p->e, M_FILE, (FILE*)sfp[1].p->rawptr);
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.setTimeout(int milisec);
KMETHOD Subproc_setTimeout(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        int time = WORD2INT(sfp[1].ivalue);
        p->timeout = ( time > 0 ) ? time : 0;
    }
    RETURNb_( ret );
}

//## File Subproc.getIN();
KMETHOD Subproc_getIn(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    kRawPtr *po = (kRawPtr*)KNH_NULVAL(CLASS_Tvoid);
    if ( ONEXEC(p) ) {
        if( p->w.mode == M_PIPE ) {
            po = new_RawPtr(ctx, ClassTBL(khn_getFileClass(ctx)), p->w.fp);
        }
    }
    RETURN_( po );
}

//## File Subproc.getOUT();
KMETHOD Subproc_getOut(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    kRawPtr *po = (kRawPtr*)KNH_NULVAL(CLASS_Tvoid);
    if ( ONEXEC(p) ) {
        if( p->r.mode == M_PIPE ) {
            po = new_RawPtr(ctx, ClassTBL(khn_getFileClass(ctx)), p->r.fp);
        }
    }
    RETURN_( po );
}

//## File Subproc.getERR();
KMETHOD Subproc_getErr(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    kRawPtr *po = (kRawPtr*)KNH_NULVAL(CLASS_Tvoid);
    if ( ONEXEC(p) ) {
        if( p->e.mode == M_PIPE ) {
            po = new_RawPtr(ctx, ClassTBL(khn_getFileClass(ctx)), p->e.fp);
        }
    }
    RETURN_( po );
}

//## int Subproc.getPid();
KMETHOD Subproc_getPid(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNi_( (p!=NULL) ? p->cpid : -1 );
}

//## int Subproc.getTimeout();
KMETHOD Subproc_getTimeout(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNi_( (p!=NULL) ? p->timeout : -1 );
}

//## int Subproc.getReturncode();
KMETHOD Subproc_getReturncode(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNi_( (p!=NULL) ? p->status : -1 );
}

//## boolean Subproc.enablePipemodeIN(Boolean isPipemode);
KMETHOD Subproc_enablePipemodeIN(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        if ( sfp[1].bvalue == 1 ) {
            setFd(ctx, &p->w, M_PIPE, NULL);
        } else {
            if ( p->w.mode == M_PIPE ) {
                initFd(&p->w);
            }
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.enablePipemodeOUT(Boolean isPipemode);
KMETHOD Subproc_enablePipemodeOUT(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        if ( sfp[1].bvalue == 1 ) {
            setFd(ctx, &p->r, M_PIPE, NULL);
        } else {
            if ( p->r.mode == M_PIPE ) {
                initFd(&p->r);
            }
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.enablePipemodeERR(Boolean isPipemode);
KMETHOD Subproc_enablePipemodeERR(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        if ( sfp[1].bvalue == 1 ) {
            setFd(ctx, &p->e, M_PIPE, NULL);
        } else {
            if ( p->e.mode == M_PIPE ) {
                initFd(&p->e);
            }
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.enableStandardIN(Boolean isStandard);
KMETHOD Subproc_enableStandardIN(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        if ( sfp[1].bvalue == 1 ) {
            setFd(ctx, &p->w, M_NREDIRECT, NULL);
        } else {
            if ( p->w.mode == M_NREDIRECT ) {
                initFd(&p->w);
            }
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.enableStandardOUT(Boolean isStandard);
KMETHOD Subproc_enableStandardOUT(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        if ( sfp[1].bvalue == 1 ) {
            setFd(ctx, &p->r, M_NREDIRECT, NULL);
        } else {
            if ( p->r.mode == M_NREDIRECT ) {
                initFd(&p->r);
            }
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.enableStandardERR(Boolean isStandard);
KMETHOD Subproc_enableStandardERR(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        if ( sfp[1].bvalue == 1 ) {
            setFd(ctx, &p->e, M_NREDIRECT, NULL);
        } else {
            if ( p->e.mode == M_NREDIRECT ) {
                initFd(&p->e);
            }
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.enableERR2StdOUT(Boolean isStdout);
KMETHOD Subproc_enableERR2StdOUT(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    int ret = PREEXEC(p);
    if ( ret ) {
        if ( sfp[1].bvalue == 1 ) {
            setFd(ctx, &p->e, M_STDOUT, NULL);
        } else {
            if ( p->e.mode == M_STDOUT ) {
                initFd(&p->e);
            }
        }
    }
    RETURNb_( ret );
}

//## boolean Subproc.isShellmode();
KMETHOD Subproc_isShellmode(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNb_( (p!=NULL) ? (p->shell==1) : 0 );
}

//## boolean Subproc.isPipemodeIN();
KMETHOD Subproc_isPipemodeIN(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNb_( (p!=NULL) ? (p->w.mode==M_PIPE) : 0 );
}

//## boolean Subproc.isPipemodeOUT();
KMETHOD Subproc_isPipemodeOUT(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNb_( (p!=NULL) ? (p->r.mode==M_PIPE) : 0 );
}

//## boolean Subproc.isPipemodeERR();
KMETHOD Subproc_isPipemodeERR(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNb_( (p!=NULL) ? (p->e.mode==M_PIPE) : 0 );
}

//## boolean Subproc.isStandardIN();
KMETHOD Subproc_isStandardIN(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNb_( (p!=NULL) ? (p->w.mode==M_NREDIRECT) : 0 );
}

//## boolean Subproc.isStandardOUT();
KMETHOD Subproc_isStandardOUT(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNb_( (p!=NULL) ? (p->r.mode==M_NREDIRECT) : 0 );
}

//## boolean Subproc.isStandardERR();
KMETHOD Subproc_isStandardERR(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNb_( (p!=NULL) ? (p->e.mode==M_NREDIRECT) : 0 );
}

//## boolean Subproc.isERR2StdOUT();
KMETHOD Subproc_isERR2StdOUT(CTX ctx, ksfp_t *sfp _RIX)
{
    subprocData_t *p = (subprocData_t*)sfp[0].p->rawptr;
    RETURNb_( (p!=NULL) ? (p->e.mode==M_STDOUT) : 0 );
}

#ifdef _SETUP

DEFAPI(const knh_PackageDef_t*) init(CTX ctx, const knh_LoaderAPI_t *kapi)
{
    kapi->setPackageProperty(ctx, "name", "subproc");
    kapi->setPackageProperty(ctx, "version", "1.0");
    RETURN_PKGINFO("konoha.subproc");
}

#endif

#ifdef __cplusplus
}
#endif
