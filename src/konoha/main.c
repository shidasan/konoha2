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
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* getopt.h */
extern char *optarg;
extern int   optind, opterr;

static const char HELPMSG[] = ""
"usage: konoha [options]  [file | -]\n"
"options and arguments (and corresponding environment variables):\n"
"-c file : compile and verification only (never running)\n"
"-h      : print this help message and exit (also --help)\n"
"-i file : invoke interactive shell after running script\n"
"-l file : logging to file\n"
"-V      : show version and sysinfo and exit (also --version)\n"
"file    : program read from script file\n"
"-       : program read from stdin\n"
"\n"
"\n";

static int isCompileOnly = 0;
static int isInteractiveMode = 0;
static FILE *stdlog = NULL;

static void opt_l(char *filename)
{
	const char *mode = "w";
	if(filename == NULL) {
		filename = "konoha.log";
		mode = "a";
	}
	stdlog = fopen(filename, mode);
	if(stdlog == NULL) {
		fprintf(stderr, "cannot open logfile: %s\n", filename);
		exit(1);
	}
}

static void opt_v(void)
{
	fprintf(stdout, "konoha2-%dbit", ((int)sizeof(void*)*8));
	exit(0);
}

//void knh_closelog(void)
//{
//	if(stdlog != stderr) {
//		fflush(stdlog);
//		fclose(stdlog);
//		stdlog = stderr;
//	}
//}


int parse_opt(int argc, char *argv[])
{
	int c, index = 0;
	static struct option options[] = {
		//{"compile", required_argument, 0, 'l'},
		{"logfile", optional_argument, 0, 'l'},
		{"version", no_argument,       0, 'V'},
		{"help",    no_argument,       0, 'h'},
		{0,         0,                 0,  0 }
	};


	while (1) {
		c = getopt_long(argc, argv, "ci:l:Vh", options, &index);
		if (c == -1)
			break;

		switch (c) {
		case 'c':
			isCompileOnly = 1;
			break;
		case 'i':
			isInteractiveMode = 1;
			break;
		case 'l':
			opt_l(optarg);
			break;
		case 'V':
			opt_v();
			break;
		case 'h':
		default:
			fprintf(stdout, HELPMSG);
			exit(0);
		}
	}
	//fprintf(stderr, "%d\n", optind);
	return optind;
}

// -------------------------------------------------------------------------
// ** main **

int konoha_debug = 0;  // global variables

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
	ret = konoha_main(konoha, argc, argv);
	konoha_close(konoha);
	klib2_check_malloced_size();
	return ret;
}

#ifdef __cplusplus
}
#endif
