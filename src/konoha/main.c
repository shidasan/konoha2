/****************************************************************************
 * KONOHA2.0 COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2006-2012, Kimio Kuramitsu <kimio at ynu.ac.jp>
 *           (c) 2008-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 *
 * You may choose one of the following two licenses when you use konoha.
 * If you want to use the latter license, please contact us.
 *
 * (1) GNU General Public License 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure License 1.0
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

#include <konoha2/konoha2.h>
#include "../gc/gc_api.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#ifdef __cplusplus
extern "C" {
#endif

///* getopt.h */
//extern char *optarg;
//extern int   optind, opterr;
//
//static const char HELPMSG[] = ""
//"usage: konoha [options]  [file | -]\n"
//"options and arguments (and corresponding environment variables):\n"
//"-c file : compile and verification only (never running)\n"
//"-h      : print this help message and exit (also --help)\n"
//"-i file : invoke interactive shell after running script\n"
//"-l file : logging to file\n"
//"-V      : show version and sysinfo and exit (also --version)\n"
//"file    : program read from script file\n"
//"-       : program read from stdin\n"
//"\n"
//"\n";
//
//static int isCompileOnly = 0;
//static int isInteractiveMode = 0;
//static FILE *stdlog = NULL;
//
//static void opt_l(char *filename)
//{
//	const char *mode = "w";
//	if(filename == NULL) {
//		filename = "konoha.log";
//		mode = "a";
//	}
//	stdlog = fopen(filename, mode);
//	if(stdlog == NULL) {
//		fprintf(stderr, "cannot open logfile: %s\n", filename);
//		exit(1);
//	}
//}
//
//static void opt_v(void)
//{
//	fprintf(stdout, "konoha2-%dbit", ((int)sizeof(void*)*8));
//	exit(0);
//}
//
////void knh_closelog(void)
////{
////	if(stdlog != stderr) {
////		fflush(stdlog);
////		fclose(stdlog);
////		stdlog = stderr;
////	}
////}
//
//
//int parse_opt(int argc, char *argv[])
//{
//	int c, index = 0;
//	static struct option options[] = {
//		//{"compile", required_argument, 0, 'l'},
//		{"logfile", optional_argument, 0, 'l'},
//		{"version", no_argument,       0, 'V'},
//		{"help",    no_argument,       0, 'h'},
//		{0,         0,                 0,  0 }
//	};
//
//
//	while (1) {
//		c = getopt_long(argc, argv, "ci:l:Vh", options, &index);
//		if (c == -1)
//			break;
//
//		switch (c) {
//		case 'c':
//			isCompileOnly = 1;
//			break;
//		case 'i':
//			isInteractiveMode = 1;
//			break;
//		case 'l':
//			opt_l(optarg);
//			break;
//		case 'V':
//			opt_v();
//			break;
//		case 'h':
//		default:
//			fprintf(stdout, HELPMSG);
//			exit(0);
//		}
//	}
//	//fprintf(stderr, "%d\n", optind);
//	return optind;
//}

// -------------------------------------------------------------------------
// keval

kstatus_t MODEVAL_eval(CTX, const char *script, size_t len, kline_t uline);
kstatus_t MODEVAL_loadscript(CTX, const char *path, size_t len, kline_t pline);

/* ------------------------------------------------------------------------ */

static char *(*kreadline)(const char *);
static int  (*kadd_history)(const char *);

static char* readline(const char* prompt)
{
	static int checkCTL = 0;
	int ch, pos = 0;
	static char linebuf[1024]; // THREAD-UNSAFE
	fputs(prompt, stdout);
	while((ch = fgetc(stdin)) != EOF) {
		if(ch == '\r') continue;
		if(ch == 27) {
			/* ^[[A */;
			fgetc(stdin); fgetc(stdin);
			if(checkCTL == 0) {
				fprintf(stdout, " - use readline, it provides better shell experience.\n");
				checkCTL = 1;
			}
			continue;
		}
		if(ch == '\n' || pos == sizeof(linebuf) - 1) {
			linebuf[pos] = 0;
			break;
		}
		linebuf[pos] = ch;
		pos++;
	}
	if(ch == EOF) return NULL;
	char *p = (char*)malloc(pos+1);
	memcpy(p, linebuf, pos+1);
	return p;
}

static int add_history(const char* line)
{
	return 0;
}

static int checkstmt(const char *t, size_t len)
{
	size_t i = 0;
	int ch, quote = 0, nest = 0;
	int hasDOC = 0;
	L_NORMAL:
	for(; i < len; i++) {
		ch = t[i];
		if(ch == '{' || ch == '[' || ch == '(') nest++;
		if(ch == '}' || ch == ']' || ch == ')') nest--;
		if(ch == '\'' || ch == '"' || ch == '`') {
			if(t[i+1] == ch && t[i+2] == ch) {
				quote = ch; i+=2;
				goto L_TQUOTE;
			}
		}
		if(ch == '\n') hasDOC = 0;
		if(ch == '#') {
			hasDOC = 1;
		}
	}
	return (hasDOC == 1) ? 1 : nest;
	L_TQUOTE:
	DBG_ASSERT(i > 0);
	for(; i < len; i++) {
		ch = t[i];
		if(t[i-1] != '\\' && ch == quote) {
			if(t[i+1] == ch && t[i+2] == ch) {
				i+=2;
				goto L_NORMAL;
			}
		}
	}
	return 1;
}

static kstatus_t readstmt(CTX, kwb_t *wb, kushort_t *uline)
{
	int line = 1;
	kstatus_t status = K_CONTINUE;
//	CWB_clear(cwb, 0);
//	fputs(TERM_BBOLD(_ctx), stdout);
	while(1) {
		int check;
		char *ln = kreadline(line == 1 ? ">>> " : "    ");
		if(ln == NULL) {
			kwb_free(wb);
			status = K_BREAK;
			break;
		}
		if(line > 1) kwb_putc(wb, '\n');
		kwb_write(wb, ln, strlen(ln));
		free(ln);
		if((check = checkstmt(kwb_top(wb, 0), kwb_size(wb))) > 0) {
			uline[1]++;
			line++;
			continue;
		}
		if(check < 0) {
			fputs("(Cancelled)...\n", stdout);
			kwb_free(wb);
		}
		break;
	}
	if(kwb_size(wb) > 0) {
		kadd_history(kwb_top(wb, 1));
	}
//	fputs(TERM_EBOLD(_ctx), stdout);
	fflush(stdout);
	return status;
}

void MODEVAL_dumpEval(CTX, kwb_t *wb);
static void shell(CTX)
{
	kwb_t wb;
	kwb_init(&(_ctx->stack->cwb), &wb);
	kline_t uline = KURI("(shell)") | 1;
	void *handler = dlopen("libreadline" K_OSDLLEXT, RTLD_LAZY);
	void *f = (handler != NULL) ? dlsym(handler, "readline") : NULL;
	kreadline = (f != NULL) ? (char* (*)(const char*))f : readline;
	f = (handler != NULL) ? dlsym(handler, "add_history") : NULL;
	kadd_history = (f != NULL) ? (int (*)(const char*))f : add_history;
	fprintf(stdout, "Konoha 2.0-alpha (Miyajima) (%d,%s)\n", K_REVISION, __DATE__);
	fprintf(stdout, "[gcc %s]\n", __VERSION__);
	while(1) {
		kstatus_t status = readstmt(_ctx, &wb, (kushort_t*)&uline);
		if(status == K_CONTINUE && kwb_size(&wb) > 0) {
			status = MODEVAL_eval(_ctx, kwb_top(&wb, 1), kwb_size(&wb), uline);
			if(status != K_FAILED) {
				kwb_free(&wb);
				MODEVAL_dumpEval(_ctx, &wb);
			}
		}
		kwb_free(&wb);
		if(status == K_BREAK) {
			break;
		}
	}
	kwb_free(&wb);
	fprintf(stdout, "\n");
	return;
}

int konoha_main(konoha_t konoha, int argc, const char **argv)
{
	int ret = 0;
	//int i, ret = 0, n = knh_parseopt(_ctx, argc, argv);
	//argc = argc - n;
	//argv = argv + n;
	if(argc == 1) {
		shell((CTX_t)konoha);
	}
	else {
		if(MODEVAL_loadscript((CTX_t)konoha, argv[1], strlen(argv[1]), 0) == K_CONTINUE/* && !knh_isCompileOnly(_ctx)*/) {
			//ret = knh_runMain(_ctx, argc, argv);
			//if(isInteractiveMode) {
			//	konoha_shell(_ctx, NULL);
			//}
		}
	}
	return ret;
}

// -------------------------------------------------------------------------
// ** main **

int main(int argc, const char *argv[])
{
	int ret = 0;
	if(getenv("KONOHA_DEBUG") != NULL) {
		konoha_debug = 1;
	}
	konoha_ginit(argc, argv);
	konoha_t konoha = konoha_open();
	BEGIN_CONTEXT(konoha);
	ret = konoha_main(konoha, argc, argv);
	END_CONTEXT(konoha);
	konoha_close(konoha);
	MODGC_check_malloced_size();
	return ret;
}

#ifdef __cplusplus
}
#endif
