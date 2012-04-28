/****************************************************************************
 * KONOHA2 COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
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

/* ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

#include"commons.h"

//#ifdef HAVE_LIBCURL
//#define K_USING_CURL 1
//#endif

/* ************************************************************************ */

void knh_buff_addpath(CTX, kBytes *ba, size_t pos, int needsSEP, kbytes_t t)
{
	size_t i;
	if(needsSEP) {
		kbytes_t b = {{ba->bu.text}, pos};
		if(!(b.len > 0 && b.buf[b.len-1] == '/')) {
			kwb_putc(ba, '/');
		}
	}
	for(i = 0; i < t.len; i++) {
		int ch = t.ubuf[i];
		kwb_putc(ba, ch);
	}
}

void knh_buff_addospath(CTX, kBytes *ba, size_t pos, int needsSEP, kbytes_t t)
{
	size_t i;
	if(needsSEP) {
		kbytes_t b = {{ba->bu.text}, pos};
		if(!(b.len > 0 && b.buf[b.len-1] == K_SEP)) {
			kwb_putc(ba, K_SEP);
		}
	}
	for(i = 0; i < t.len; i++) {
		int ch = t.ubuf[i];
		if(ch == '\\' || ch == '/') ch = K_SEP;
		if(ch < 128) {
			kwb_putc(ba, ch);
			continue;
		}
		kwb_putc(ba, ch);
	}
}

void knh_buff_trim(CTX, kBytes *ba, size_t pos, int ch)
{
	kchar_t *ubuf = ba->bu.ubuf + pos;
	long i, len = BA_size(ba) - pos;
	if(ch == '/' && ch != K_SEP) ch = K_SEP;
	for(i = len - 1; i >= 0 ; i--) {
		if(ubuf[i] == ch) {
			knh_Bytes_clear(ba, pos + i);
			return;
		}
	}
}

static const char *new_cwbtext(CTX, CWB_t *cwb, size_t *lenref)
{
	const char *p = CWB_totext(_ctx, cwb);
	size_t len = knh_strlen(p) + 1;
	char *newtext = (char*) KCALLOC(len);
	knh_memcpy(newtext, p, len);
	lenref[0] = len;
	return (const char*)newtext;
}

KNHAPI2(kPath*) new_Path(CTX, kString *path)
{
	kPath *pth = new_(Path);
	KSETv(pth->urn, path);
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, S_tobytes(path));
	if(knh_strcmp(S_text(path), CWB_totext(_ctx, cwb)) == 0) {
		pth->ospath = S_text(path);
		pth->asize = 0;
	}
	else {
		pth->ospath = new_cwbtext(_ctx, cwb, &(pth->asize));
	}
	CWB_close(_ctx, cwb);
	return pth;
}

static void knh_buff_addScriptPath(CTX, kBytes *ba, size_t pos, kKonohaSpace *ns, kbytes_t path)
{
	kbytes_t bpath = knh_bytes_next(path, ':');
	knh_buff_addpath(_ctx, ba, pos, 0, B(ns->path->ospath));
	if(!knh_buff_isdir(_ctx, ba, pos)) {
		knh_buff_trim(_ctx, ba, pos, '/');
	}
	knh_buff_addospath(_ctx, ba, pos, 1, bpath);
}

kPath *new_ScriptPath(CTX, kString *urn, kKonohaSpace *ns)
{
	kPath *pth = new_(Path);
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	KSETv(pth->urn, urn);
	kbytes_t bpath = knh_bytes_next(S_tobytes(urn), ':');
	knh_buff_addScriptPath(_ctx, cwb->ba, cwb->pos, ns, bpath);
	pth->ospath = new_cwbtext(_ctx, cwb, &(pth->asize));
	CWB_close(_ctx, cwb);
	return pth;
}

#ifdef K_USING_POSIX_
#include <unistd.h>
#include <dirent.h>
#endif

kArray *knh_PathDir_toArray(CTX, kPath *path)
{
	kArray *a = new_ArrayG(_ctx, CLASS_StringARRAY, 0);
#ifdef K_USING_POSIX_
	DIR *dirptr = opendir(path->ospath);
	if(dirptr != NULL) {
		struct dirent *dp;
		while((dp = readdir(dirptr))!= NULL) {
			const char *d = dp->d_name;
			if(d[0] == '.' && (d[1] == 0 || (d[2] == 0 && d[1] == '.'))) continue;
			kArray_add(a, new_String(_ctx, d));
		}
		closedir(dirptr);
	}
#endif
	return a;
}

/* ------------------------------------------------------------------------ */
/* K_DPI_STREAM */

static size_t io2_writeNOP(CTX, kio_t *io2, const char *buf, size_t bufsiz)
{
	return 0;
}
static kbool_t io2_readNOP(CTX, kio_t *io2)
{
	return 0;
}
static void io2_closeNOP(CTX, kio_t *io2)
{
}

static kbool_t io2_readFILE(CTX, kio_t *io2)
{
	if(io2->bufsiz == 0) {
		io2->bufsiz = K_PAGESIZE;
		io2->buffer = KCALLOC(io2->bufsiz);
	}
	size_t size = fread(io2->buffer, 1, io2->bufsiz, io2->fp);
	if(size == 0) {
		int tf = 1;
		if(ferror(io2->fp)) {
			KNH_NTRACE2(_ctx, "fread", K_PERROR, KNH_LDATA(LOG_p("fp", io2->fp), LOG_s("path", io2->DBG_NAME)));
			tf = 0;
		}
		io2->_close(_ctx, io2);
		return 0;
	}
	else {
		io2->top = 0;
		io2->tail = size;
		return 1;
	}
}

static size_t io2_writeFILE(CTX, kio_t *io2, const char *buf, size_t bufsiz)
{
	size_t size;

	fflush(io2->fp);
	size = fwrite(buf, 1, bufsiz, io2->fp);
	if(size == 0) {
		if(ferror(io2->fp)) {
			KNH_NTRACE2(_ctx, "fwrite", K_PERROR, KNH_LDATA(LOG_i("fp", io2->fp), LOG_s("path", io2->DBG_NAME)));
			io2->_close(_ctx, io2);
		}
		return 0;
	}
	fflush(io2->fp);
	return size;
}

static void io2_closeFILE(CTX, kio_t *io2)
{
	DBG_ASSERT(io2->isRunning == 1);
	fclose(io2->fp);
	io2->isRunning = 0;
}

kio_t* new_FILE(CTX, FILE *fp, size_t bufsiz)
{
	kio_t *io2 = KCALLOC(sizeof(kio_t));
	io2->fp  = fp;
	io2->isRunning = 1;
	io2->bufsiz = bufsiz;
	if(bufsiz > 0) {
		io2->buffer = KCALLOC(bufsiz);
	}
	io2->top  = 0;
	io2->tail = 0;
	io2->_close         = io2_closeFILE;
	io2->_blockread     = io2_readFILE;
	io2->_unblockread   = io2_readFILE;
	io2->_read          = io2_readFILE;
	io2->_blockwrite    = io2_writeFILE;
	io2->_unblockwrite  = io2_writeFILE;
	io2->_write         = io2_writeFILE;
	return io2;
}

static kbool_t io2_blockread(CTX, kio_t *io2)
{
	if(io2->bufsiz == 0) {
		io2->bufsiz = K_PAGESIZE;
		io2->buffer = KCALLOC(io2->bufsiz);
	}
	int fd = io2->fd;
	ssize_t size = read(fd, io2->buffer, io2->bufsiz);
	if(size == -1) {
		KNH_NTRACE2(_ctx, "read", K_PERROR, KNH_LDATA(LOG_i("fd", fd), LOG_s("path", io2->DBG_NAME)));
		io2->_close(_ctx, io2);
		return 0;
	}
	else {
		io2->top = 0;
		io2->tail = size;
		if(size == 0) {
			io2->_close(_ctx, io2);
		}
		return 1;
	}
}

static kbool_t io2_unblockread(CTX, kio_t *io2)
{
	int fd = io2->fd;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	int rc = select(fd + 1, &fds, NULL, NULL, NULL);
	if(rc != -1) {
		if(FD_ISSET((int)fd,&fds)) {
			return io2->_blockread(_ctx, io2);
		}
		knh_bzero(io2->buffer, io2->bufsiz);
		io2->top  = 0;
		io2->tail = 0;
		return 1;
	}
	{
		KNH_NTRACE2(_ctx, "select", K_PERROR, KNH_LDATA(LOG_i("fd", fd), LOG_s("path", io2->DBG_NAME)));
	}
	return 0;
}

static size_t io2_blockwrite(CTX, kio_t *io2, const char *buf, size_t bufsiz)
{
	int fd = io2->fd;
	ssize_t size = write(fd, buf, bufsiz);
	if(size == -1) {
		KNH_NTRACE2(_ctx, "write", K_PERROR, KNH_LDATA(LOG_i("fd", fd), LOG_s("path", io2->DBG_NAME)));
		io2->_close(_ctx, io2);
	}
	return 0;
}

static size_t io2_unblockwrite(CTX, kio_t *io2, const char *buf, size_t size)
{
	int fd = io2->fd;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	int rc = select(fd + 1, NULL, &fds, NULL, NULL);
	if(rc != -1) {
		if(FD_ISSET((int)fd,&fds)) {
			return io2->_blockwrite(_ctx, io2, buf, size);
		}
		return 0;
	}
	{
		KNH_NTRACE2(_ctx, "select", K_PERROR, KNH_LDATA(LOG_i("fd", fd), LOG_s("path", io2->DBG_NAME)));
	}
	return 0;
}

static void io2_closeFD(CTX, kio_t *io2)
{
	DBG_ASSERT(io2->isRunning == 1);
	close(io2->fd);
	io2->isRunning = 0;
}

static void io2_closeFD_stdio(CTX, kio_t *io2)
{
	DBG_ASSERT(io2->isRunning == 1);
	io2->isRunning = 0;
}

static kio_t* new_io2_(CTX, int fd, size_t bufsiz, void (*_close)(CTX, struct kio_t *))
{
	kio_t *io2 = KCALLOC(sizeof(kio_t));
	io2->handler  = NULL;
	io2->handler2 = NULL;
	io2->fd = fd;
	io2->isRunning = 1;
	io2->bufsiz = bufsiz;
	if(bufsiz > 0) {
		io2->buffer = KCALLOC(bufsiz);
	}
	io2->top  = 0;
	io2->tail = 0;
	io2->_close         = _close;
	io2->_blockread     = io2_blockread;
	io2->_unblockread   = io2_unblockread;
	io2->_read          = io2_blockread;
	io2->_blockwrite    = io2_blockwrite;
	io2->_unblockwrite  = io2_unblockwrite;
	io2->_write         = io2_blockwrite;
	return io2;
}

kio_t* new_io2(CTX, int fd, size_t bufsiz)
{
	return new_io2_(_ctx, fd, bufsiz, io2_closeFD);
}

kio_t* new_io2_stdio(CTX, int fd, size_t bufsiz)
{
	return new_io2_(_ctx, fd, bufsiz, io2_closeFD_stdio);
}

kio_t* new_io2ReadBuffer(CTX, const char *buf, size_t bufsiz)
{
	kio_t *io2 = KCALLOC(sizeof(kio_t));
	io2->handler  = NULL;
	io2->handler2 = NULL;
	io2->fd = -1;
	io2->isRunning = 0;
	io2->buffer = KCALLOC(bufsiz);
	knh_memcpy(io2->buffer, buf, bufsiz);
	io2->bufsiz = bufsiz;
	io2->top  = 0;
	io2->tail = bufsiz;
	io2->_close         = io2_close;
	io2->_blockread     = io2_readNOP;
	io2->_unblockread   = io2_readNOP;
	io2->_read          = io2_readNOP;
	io2->_blockwrite    = io2_writeNOP;
	io2->_unblockwrite  = io2_writeNOP;
	io2->_write         = io2_writeNOP;
	return io2;
}

static size_t io2_writeBytes(CTX, kio_t *io2, const char *buf, size_t bufsiz)
{
	knh_Bytes_write2(_ctx, io2->baNC, buf, bufsiz);
	return bufsiz;
}

static void io2_closeBytes(CTX, kio_t *io2)
{
	io2->_blockwrite    = io2_writeNOP;
	io2->_unblockwrite  = io2_writeNOP;
	io2->_write         = io2_writeNOP;
	io2->isRunning = 0;
}

kio_t* new_io2WriteBuffer(CTX, kBytes *ba)
{
	kio_t *io2 = KCALLOC(sizeof(kio_t));
	io2->baNC  = ba;
	io2->isRunning = 1;
	io2->buffer = NULL;
	io2->bufsiz = 0;
	io2->top  = 0;
	io2->tail = 0;
	io2->_close         = io2_closeBytes;
	io2->_blockread     = io2_readNOP;
	io2->_unblockread   = io2_readNOP;
	io2->_read          = io2_readNOP;
	io2->_blockwrite    = io2_writeBytes;
	io2->_unblockwrite  = io2_writeBytes;
	io2->_write         = io2_writeBytes;
	return io2;
}

kio_t *io2_null(void)
{
	static kio_t io2_dummy = {
			{0}, NULL
	};
	io2_dummy._close = io2_closeNOP;
	io2_dummy._read  = io2_readNOP;
	io2_dummy._write = io2_writeNOP;
	return &io2_dummy;
}

void io2_free(CTX, kio_t *io2)
{
	if(io2->isRunning == 1) {
		io2->_close(_ctx, io2);
	}
	if(io2->bufsiz > 0) {
		KFREE(_ctx, io2->buffer, io2->bufsiz);
		io2->bufsiz = 0;
		io2->buffer = NULL;
		io2->top = 0;
		io2->tail = 0;
	}
	if(io2 != io2_null()) {
		KFREE(_ctx, io2, sizeof(kio_t));
	}
}

void io2_close(CTX, kio_t *io2)
{
	if(io2->isRunning == 1) {
		io2_flush(_ctx, io2);
		io2->_close(_ctx, io2);
		io2->top  = 0;
		io2->tail = 0;
	}
}

kbool_t io2_isClosed(CTX, kio_t *io2)
{
	return (io2->isRunning == 0 && io2->top >= io2->tail);
}

int io2_getc(CTX, kio_t *io2)
{
	int ch = EOF;
	if(io2->top < io2->tail) {
		ch = (kchar_t)io2->buffer[io2->top];
		io2->top += 1;
	}
	else if(io2->isRunning) {
		io2->_read(_ctx, io2);
		if(io2->top < io2->tail) {
			ch = io2->buffer[io2->top];
			io2->top += 1;
		}
	}
	return ch;
}

size_t io2_read(CTX, kio_t *io2, char *buf, size_t bufsiz)
{
	size_t rsize = 0;
	while(bufsiz > 0) {
		long remsiz = io2->tail - io2->top;
		if(remsiz > 0) {
			if(remsiz <= bufsiz) {
				knh_memcpy(buf, io2->buffer + io2->top, bufsiz);
				io2->top += bufsiz;
				rsize += bufsiz;
				return rsize;
			}
			else {
				knh_memcpy(buf, io2->buffer + io2->top, remsiz);
				buf    += remsiz;
				rsize  += remsiz;
				bufsiz -= remsiz;
			}
		}
		if(!io2->isRunning) break;
		io2->_read(_ctx, io2);
	}
	return rsize;
}

static kString *CWB_newLine(CTX, CWB_t *cwb, kStringDecoder *dec)
{
	if(CWB_size(cwb) > 0) {
		if(cwb->ba->bu.buf[BA_size(cwb->ba) - 1] == '\r') {
			cwb->ba->bu.buf[BA_size(cwb->ba) - 1] = 0;
			BA_size(cwb->ba) -= 1;
			if(CWB_size(cwb) == 0) return TS_EMPTY;
		}
		if(dec == NULL) {
			return CWB_newString(_ctx, cwb, SPOL_POOLNEVER);
		}
		else {
			return CWB_newStringDECODE(_ctx, cwb, dec);
		}
	}
	return TS_EMPTY;
}

kString* io2_readLine(CTX, kio_t *io2, kStringDecoder *dec)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	while(io2->isRunning) {
		size_t i, start, hasUTF8 = 0;
		if(!(io2->top < io2->tail)) {
			io2->_read(_ctx, io2);
		}
		start = io2->top;
		for(i = io2->top; i < io2->tail; i++) {
			int ch = ((unsigned char*)io2->buffer)[i];
			if(ch == '\n') {
				knh_Bytes_write2(_ctx, cwb->ba, (const char*)io2->buffer + start, i - start);
				io2->top = i + 1;
				return CWB_newLine(_ctx, cwb, hasUTF8 ? dec : NULL);
			}
			if(ch > 127) hasUTF8 = 1;
		}
		knh_Bytes_write2(_ctx, cwb->ba, (const char*)io2->buffer + start, io2->tail - start);
		io2->top = i;
	}
	if(io2->top < io2->tail) {
		knh_Bytes_write2(_ctx, cwb->ba, (const char*)io2->buffer + io2->top, io2->tail - io2->top);
		io2->top  = 0;
		io2->tail = 0;
		return CWB_newLine(_ctx, cwb, dec);
	}
	return KNH_TNULL(String);
}

void io2_readAll(CTX, kio_t *io2, kBytes *ba)
{
	while(io2->isRunning == 1) {
		if(!(io2->top < io2->tail)) {
			io2->_read(_ctx, io2);
		}
		if(io2->tail > io2->top) {
			knh_Bytes_write2(_ctx, ba, (const char*)io2->buffer + io2->top, io2->tail - io2->top);
			io2->top  = 0;
			io2->tail = 0;
		}
	}
}

void io2_flush(CTX, kio_t *io2)
{
	if(io2->bufsiz > 0 && io2->tail > 0) {
		io2->_write(_ctx, io2, io2->buffer, io2->tail);
		io2->tail = 0;
	}
}

size_t io2_write(CTX, kio_t *io2, const char *buf, size_t bufsiz)
{
	if(io2->bufsiz > 0) {
		if(io2->tail + bufsiz < io2->bufsiz) {
			knh_memcpy(io2->buffer + io2->tail, buf, bufsiz);
			io2->tail += bufsiz;
			return bufsiz;
		}
		io2->_write(_ctx, io2, io2->buffer, io2->tail);
		io2->tail = 0;
		if(bufsiz < io2->bufsiz) {
			knh_memcpy(io2->buffer, buf, bufsiz);
			io2->tail += bufsiz;
			return bufsiz;
		}
	}
	return io2->_write(_ctx, io2, buf, bufsiz);
}

size_t io2_writeMultiByteChar(CTX, kio_t *io2, const char *buf, size_t bufsiz, kStringEncoder *enc)
{
	KNH_TODO("enc");
	return io2->_write(_ctx, io2, buf, bufsiz);
}

kbool_t NOFILE_exists(CTX, kPath *path)
{
	return 0;
}
void NOFILE_ospath(CTX, kPath *path, kKonohaSpace *ns)
{
	path->ospath ="";
	path->asize = 0;
}
kio_t* NOFILE_openNULL(CTX, kPath *path, const char *mode, kDictMap *conf)
{
	return NULL;
}

static const knh_PathDPI_t STREAM_NOFILE = {
	K_STREAM_NULL, "NOFILE", K_PAGESIZE,
	NOFILE_exists, NOFILE_ospath, NOFILE_openNULL,
};

static kbool_t FILE_exists(CTX, kPath *path)
{
	return knh_exists(_ctx, path->ospath);
}
static void FILE_ospath(CTX, kPath *path, kKonohaSpace *ns)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	kbytes_t urn = S_tobytes(path->urn);
	if(isalpha(urn.text[0]) && urn.text[1] == ':') { // C:\Windows
		knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, urn);
	}
	else {
		knh_buff_addospath(_ctx, cwb->ba, cwb->pos, 0, knh_bytes_next(urn, ':'));
	}
	if(knh_strcmp(S_text(path->urn), CWB_totext(_ctx, cwb)) == 0) {
		path->ospath = S_text(path->urn);
		path->asize = 0;
	}
	else {
		path->ospath = new_cwbtext(_ctx, cwb, &(path->asize));
	}
	CWB_close(_ctx, cwb);
}
static kio_t* FILE_openNULL(CTX, kPath *path, const char *mode, kDictMap *conf)
{
	FILE *fp = fopen(path->ospath, mode);
	if(fp != NULL) return new_FILE(_ctx, fp, 4096);
	return NULL;
}

static const knh_PathDPI_t STREAM_FILE = {
	K_STREAM_FILE, "file", K_OUTBUF_MAXSIZ,
	FILE_exists, FILE_ospath, FILE_openNULL,
};

static void SCRIPT_ospath(CTX, kPath *path, kKonohaSpace *ns)
{
	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
	kbytes_t bpath = knh_bytes_next(S_tobytes(path->urn), ':');
	knh_buff_addScriptPath(_ctx, cwb->ba, cwb->pos, ns, bpath);
	path->ospath = new_cwbtext(_ctx, cwb, &(path->asize));
	CWB_close(_ctx, cwb);
}

static const knh_PathDPI_t STREAM_SCRIPT = {
	K_STREAM_FILE, "script", K_OUTBUF_MAXSIZ,
	FILE_exists, SCRIPT_ospath, FILE_openNULL,
};

/* ------------------------------------------------------------------------ */

#ifdef K_USING_CURL

#include<curl/curl.h>

static kbool_t CURL_exists(CTX, kPath *path)
{
	kbool_t res = 0;
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, S_text(path->urn));
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	CURLcode code = curl_easy_perform(curl);
	res = (code == CURLE_OK);
	curl_easy_cleanup(curl);
	return res;
}

static void CURL_ospath(CTX, kPath *path, kKonohaSpace *ns)
{
	path->ospath = NULL;
	path->asize = 0;
}

typedef struct {
	CURL *curl;
	CURLM *multi_handle;
	char *buffer;               /* buffer to store cached data*/
	long pos;
	long buffer_len;             /* currently allocated buffers length */
	long buffer_pos;             /* end of data in buffer*/
	int still_running;          /* Is background url fetch still in progress */
} curl_t;

static size_t write_callback(char *buffer, size_t size, size_t nitems, void *userp)
{
	curl_t *cp = (curl_t*)userp;
	char *newbuff;
	size *= nitems;
	long rembuff = cp->buffer_len - cp->buffer_pos; /* remaining space in buffer */
	if(size > rembuff) {
		newbuff = realloc(cp->buffer, cp->buffer_len + (size - rembuff));
		if(newbuff == NULL) {
			size = rembuff;
		}
		else {
			cp->buffer_len += size - rembuff;
			cp->buffer = newbuff;
		}
	}
	memcpy(&cp->buffer[cp->buffer_pos], buffer, size);
	cp->buffer_pos += size;
	return size;
}

static int CURL_open(CTX, kPath *path, const char *mode, kDictMap *conf)
{
	curl_t *cp = knh_malloc(_ctx, sizeof(curl_t));
	memset(cp, 0, sizeof(curl_t));
	cp->curl = curl_easy_init();
	curl_easy_setopt(cp->curl, CURLOPT_URL, S_text(path->urn));
	curl_easy_setopt(cp->curl, CURLOPT_WRITEDATA, cp);
	curl_easy_setopt(cp->curl, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(cp->curl, CURLOPT_WRITEFUNCTION, write_callback);
	cp->multi_handle = curl_multi_init();
	curl_multi_add_handle(cp->multi_handle, cp->curl);

	/* lets start the fetch */
	while(curl_multi_perform(cp->multi_handle, &cp->still_running) == CURLM_CALL_MULTI_PERFORM );

	if((cp->buffer_pos == 0) && (!cp->still_running)) {
		/* if still_running is 0 now, we should return NULL */
		/* make sure the easy handle is not in the multi handle anymore */
		curl_multi_remove_handle(cp->multi_handle, cp->curl);
		/* cleanup */
		curl_easy_cleanup(cp->curl);
		knh_free(_ctx, cp, sizeof(curl_t));
		cp = NULL;
	}
	return (int)cp;
}

static kbool_t CURL_info(CTX, int fd, kObject *o)
{
	//	if(cp->contenttype == NULL){
	//		char *type = NULL;
	//		curl_easy_getinfo(cp->curl, CURLINFO_CONTENT_TYPE, &type);
	//		if(type != NULL){
	//			char *charset = NULL;
	//			charset = strrchr(type, '=');
	//			if(charset != NULL){
	//				charset++;
	//				type = strtok(type, ";");
	//			}
	//			cp->charset = (const char*)charset;
	//			cp->contenttype = (const char*)type;
	//		}
	//	}
	return 0;
}

/* only attempt to fill buffer if transactions still running and buffer
 * doesnt exceed required size already
 */

static int fill_buffer(curl_t *cp, int want, int waittime2)
{
	if((!cp->still_running) || (cp->buffer_pos > want)) {
		return 0;
	}
	/* attempt to fill buffer */
	do {
		int maxfd = -1;
		fd_set fdread;
		fd_set fdwrite;
		fd_set fdexcep;
		struct timeval timeout;
		int rc;

		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);

		/* set a suitable timeout to fail on */
		timeout.tv_sec = 60; /* 1 minute */
		timeout.tv_usec = 0;

		/* get file descriptors from the transfers */
		curl_multi_fdset(cp->multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

		/* In a real-world program you OF COURSE check the return code of the
		 * function calls.  On success, the value of maxfd is guaranteed to be
		 * greater or equal than -1.  We call select(maxfd + 1, ...), specially
		 * in case of (maxfd == -1), we call select(0, ...), which is basically
		 * equal to sleep. */

		KNH_ASSERT(maxfd != -1);
		rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
		switch(rc) {
		case -1: /* select error */
		case 0: break;
		default:
			/* timeout or readable/writable sockets */
			/* note we *could* be more efficient and not wait for
			 * CURLM_CALL_MULTI_PERFORM to clear here and check it on re-entry
			 * but that gets messy */
			while(curl_multi_perform(cp->multi_handle, &cp->still_running) == CURLM_CALL_MULTI_PERFORM);
			break;
		}
	} while(cp->still_running && (cp->buffer_pos < want));
	return 1;
}

/* use to remove want bytes from the front of a files buffer */
static int use_buffer(curl_t *cp, int want)
{
	/* sort out buffer */
	if((cp->buffer_pos - want) <= 0) {
		/* ditch buffer - write will recreate */
		if(cp->buffer != NULL) free(cp->buffer);
		cp->buffer = NULL;
		cp->buffer_pos = 0;
		cp->buffer_len = 0;
	} else { /* move rest down make it available for later */
		memmove(cp->buffer, &cp->buffer[want], (cp->buffer_pos - want));
		cp->buffer_pos -= want;
	}
	return 0;
}

//static intptr_t CURL_read(CTX, int fd, char *buf, size_t bufsiz)
//{
//	curl_t *cp = (curl_t*)fd;
//	fill_buffer(cp, bufsiz, 1);
//	if(!cp->buffer_pos) return 0;
//	/* ensure only available data is copackdomered */
//	if(cp->buffer_pos < bufsiz) bufsiz = cp->buffer_pos;
//	/* xfer data to caller */
//	memcpy(buf, cp->buffer, bufsiz);
//	use_buffer(cp, bufsiz);
//	return bufsiz;
//}
//
//static kbool_t CURL_readline(CTX, int fd, kBytes *ba)
//{
//	curl_t *cp = (curl_t*)fd;
//	int ret = 0;
//	while(1) {
//		int i, prev = 0;
//		fill_buffer(cp, K_PAGESIZE, 1);
//		if(!cp->buffer_pos) return ret;
//		for(i = 0; i < cp->buffer_pos; i++) {
//			int ch = cp->buffer[i];
//			if(ch == '\n') {
//				if(prev == '\r') {
//					knh_Bytes_write2(_ctx, ba, cp->buffer, i-1); /* Windows */
//				}
//				else {
//					knh_Bytes_write2(_ctx, ba, cp->buffer, i);   /* UNIX */
//				}
//				use_buffer(cp, i);
//				return 1;
//			}
//			if(prev == '\r' /*&& ch != '\n'*/) { /* OLD MAC */
//				knh_Bytes_write2(_ctx, ba, cp->buffer, i-1);
//				use_buffer(cp, i);
//				return 1;
//			}
//			prev = ch;
//		}
//		ret = 1;
//		knh_Bytes_write2(_ctx, ba, cp->buffer, cp->buffer_pos);
//		use_buffer(cp, cp->buffer_pos);
//	}
//}
//
//static void CURL_close(CTX, int fd)
//{
//	curl_t *cp = (curl_t*)fd;
//	curl_multi_remove_handle(cp->multi_handle, cp->curl);
//	curl_easy_cleanup(cp->curl);
//	if(cp->buffer) free(cp->buffer);
//	knh_free(_ctx, cp, sizeof(curl_t));
//}
//
//static int CURL_feof(CTX, int fd)
//{
//	curl_t *cp = (curl_t*)fd;
//	int ret = 0;
//	if((cp->buffer_pos == 0) && (!cp->still_running)) {
//		ret = 1;
//	}
//	return ret;
//}
//
//static int CURL_getc(CTX, int fd)
//{
//	curl_t *file = (curl_t*)fd;
//	if(!file->buffer_pos || file->buffer_pos < file->buffer_len)
//		fill_buffer(file, K_PAGESIZE, 1);
//	return file->buffer[file->buffer_pos++];
//}
//
//static intptr_t CURL_write(CTX, int fd, const char *buf, size_t bufsiz)
//{
//	return 0;
//}
//
//static void CURL_flush(CTX, int fd)
//{
//
//}

const knh_PathDPI_t STREAM_CURL = {
	K_STREAM_NET, "curl", K_PAGESIZE,
	CURL_exists,
	CURL_ospath,
	CURL_open,
//	CURL_read,
//	CURL_write,
//	CURL_close,
//	CURL_info,
//	CURL_getc,
//	CURL_readline,
//	CURL_feof,
//	CURL_flush,
};

#endif/*K_USING_CURL*/

const knh_PathDPI_t *knh_getDefaultPathStreamDPI(void)
{
	return &STREAM_FILE;
}

/* ------------------------------------------------------------------------ */

KNHAPI2(kInputStream*) new_InputStream(CTX, kio_t *io2, kPath *path)
{
	kInputStream* in = new_(InputStream);
	in->io2 = io2;
	if(path != NULL) {
		KSETv(in->path, path);
		io2->DBG_NAME = S_text(path->urn);
		if(io2 == NULL) {
			io2 = path->dpi->io2openNULL(_ctx, path, "r", NULL);
			if(io2 == NULL) {
				io2 = io2_null();
				kObjectoNULL(_ctx, in);
			}
			in->io2 = io2;
		}
	}
	return in;
}

kInputStream* new_BytesInputStream(CTX, const char *buf, size_t bufsiz)
{
	return new_InputStream(_ctx, new_io2ReadBuffer(_ctx, buf, bufsiz), NULL);
}

KNHAPI2(kOutputStream*) new_OutputStream(CTX,  kio_t *io2, kPath *path)
{
	kOutputStream* w = new_(OutputStream);
	w->io2 = io2;
	if(path != NULL) {
		KSETv(w->path, path);
		io2->DBG_NAME = S_text(path->urn);
		if(io2 == NULL) {
			io2 = path->dpi->io2openNULL(_ctx, path, "a", NULL);
			if(io2 == NULL) {
				io2 = io2_null();
				kObjectoNULL(_ctx, w);
			}
			w->io2 = io2;
		}
	}
	return w;
}

void knh_InputStream_setCharset(CTX, kInputStream *in, kStringDecoder *c)
{
	if(in->decNULL == NULL) {
		if(c != NULL) {
			KINITv(in->decNULL, c);
		}
	}
	else {
		if(c != NULL) {
			KSETv(in->decNULL, c);
		}
		else {
			KNH_FINALv(_ctx, in->decNULL);
			in->decNULL = c;
		}
	}
}

void knh_OutputStream_setCharset(CTX, kOutputStream *w, kStringEncoder *c)
{
	if(w->encNULL == NULL) {
		KINITv(w->encNULL, c);
	}
	else {
		KSETv(w->encNULL, c);
	}
}

KNHAPI2(kOutputStream*) new_BytesOutputStream(CTX, kBytes *ba)
{
	kOutputStream *w = new_OutputStream(_ctx, new_io2WriteBuffer(_ctx, ba), NULL);
	KINITv(w->bufferNULL, ba);
	return w;
}

KNHAPI2(void) knh_OutputStream_flush(CTX, kOutputStream *w)
{
	io2_flush(_ctx, w->io2);
}

KNHAPI2(void) knh_OutputStream_putc(CTX, kOutputStream *w, int ch)
{
	char buf[8] = {ch};
	io2_write(_ctx, w->io2, buf, 1);
}

KNHAPI2(void) knh_OutputStream_write(CTX, kOutputStream *w, kbytes_t buf)
{
	io2_write(_ctx, w->io2, buf.text, buf.len);
}

KNHAPI2(void) knh_OutputStream_p(CTX, kOutputStream *w, kbytes_t buf)
{
	if(w->encNULL != NULL) {
		size_t i;
		for(i = 0; i < buf.len; i++) {
			int ch = buf.ubuf[i];
			if(ch > 127) {
				io2_writeMultiByteChar(_ctx, w->io2, buf.text, buf.len, w->encNULL);
				return;
			}
		}
	}
	io2_write(_ctx, w->io2, buf.text, buf.len);
}

/* ------------------------------------------------------------------------ */
/* [knh_write] */

KNHAPI2(void) knh_write_EOL(CTX, kOutputStream *w)
{
	io2_write(_ctx, w->io2, K_OSLINEFEED, sizeof(K_OSLINEFEED) - 1);
	if(OutputStream_isAutoFlush(w)) {
		io2_flush(_ctx, w->io2);
	}
	//OutputStream_setBOL(w, 1);
}

KNHAPI2(void) knh_write_TAB(CTX, kOutputStream *w)
{
	io2_write(_ctx, w->io2, "\t", 1);
}

/* ------------------------------------------------------------------------ */
/* [datatype] */

void knh_write_bool(CTX, kOutputStream *w, int b)
{
	kbytes_t t = (b) ? STEXT("true") : STEXT("false");
	io2_write(_ctx, w->io2, t.text, t.len);
}

void knh_write_ptr(CTX, kOutputStream *w, void *ptr)
{
	char buf[KINT_FMTSIZ];
	knh_snprintf(buf, sizeof(buf), "%p", ptr);
	io2_write(_ctx, w->io2, (const char*)buf, knh_strlen(buf));
}

void knh_write_dfmt(CTX, kOutputStream *w, const char *fmt, intptr_t n)
{
	char buf[KINT_FMTSIZ];
	knh_snprintf(buf, sizeof(buf), fmt, n);
	io2_write(_ctx, w->io2, (const char*)buf, knh_strlen(buf));
}

void knh_write_ifmt(CTX, kOutputStream *w, const char *fmt, kint_t n)
{
	char buf[KINT_FMTSIZ];
	knh_snprintf(buf, sizeof(buf), fmt, n);
	io2_write(_ctx, w->io2, (const char*)buf, knh_strlen(buf));
}

void knh_write_ffmt(CTX, kOutputStream *w, const char *fmt, kfloat_t n)
{
	char buf[KFLOAT_FMTSIZ];
	knh_snprintf(buf, sizeof(buf), fmt, n);
	io2_write(_ctx, w->io2, (const char*)buf, knh_strlen(buf));
}

void knh_write_flag(CTX, kOutputStream *w, kflag_t flag)
{
	kchar_t ubuf[8];
	kbytes_t t = {{(const char*)ubuf}, 1};
	intptr_t i;
	kflag_t f = 1 << 15;
	for(i = 0; i < 16; i++) {
		if(i > 0 && i % 8 == 0) {
			ubuf[0] = ' ';
			io2_write(_ctx, w->io2, t.text, 1);
		}
		ubuf[0] = ((f & flag) == f) ? '1' : '0';
		io2_write(_ctx, w->io2, t.text, 1);
		f = f >> 1;
	}
}

KNHAPI2(void) knh_write_ascii(CTX, kOutputStream *w, const char *text)
{
	io2_write(_ctx, w->io2, text, knh_strlen(text));
}

KNHAPI2(void) knh_write_utf8(CTX, kOutputStream *w, kbytes_t t, int hasUTF8)
{
	if(hasUTF8 && w->encNULL != NULL) {
		io2_writeMultiByteChar(_ctx, w->io2, t.text, t.len, w->encNULL);
	}
	else {
		io2_write(_ctx, w->io2, t.text, t.len);
	}
}

/* ------------------------------------------------------------------------ */

void knh_write_quote(CTX, kOutputStream *w, int quote, kbytes_t t, int hasUTF8)
{
	kbytes_t sub = t;
	size_t i, s = 0;
	kwb_putc(wb, quote);
	for(i = 0; i < t.len; i++) {
		int ch = t.utext[i];
		if(ch == '\t' || ch == '\n' || ch == '\r' || ch == '\\' || ch == quote) {
			sub.utext = t.utext + s;
			sub.len = i - s;
			knh_write_utf8(_ctx, w, sub, hasUTF8);
			s = i + 1;
			kwb_putc(wb, '\\');
			if(ch == '\t') {
				kwb_putc(wb, 't');
			}
			else if(ch == '\n') {
				kwb_putc(wb, 'n');
			}
			else if(ch == '\r') {
				kwb_putc(wb, 'r');
			}
			else {
				kwb_putc(wb, ch);
			}
		}
	}
	if (s < t.len) {
		sub.utext = t.utext + s;
		sub.len = t.len - s;
		knh_write_utf8(_ctx, w, sub, hasUTF8);
	}
	kwb_putc(wb, quote);
}

void knh_write_cap(CTX, kOutputStream *w, kbytes_t t, int hasUTF8)
{
	if(islower(t.utext[0])) {
		kwb_putc(wb, toupper(t.utext[0]));
		t.utext = t.utext+1; t.len = t.len -1;
	}
	knh_write_utf8(_ctx, w, t, hasUTF8);
}

void knh_write_Object(CTX, kOutputStream *w, Object *o, int level)
{
	if(level % 2 == 0) { // TYPED
		knh_write_ascii(_ctx, w, CLASS__(O_cid(o)));
		kwb_putc(wb, ':');
	}
	if(Object_isNullObject(o)) {
		knh_write(_ctx, w, STEXT("null"));
	}
	else {
		O_ct(o)->cdef->p(_ctx, w, RAWPTR(o), level);
	}
	if(IS_FMTdump(level)) {
		knh_write_EOL(_ctx, w);
	}
}

void knh_write_InObject(CTX, kOutputStream *w, Object *o, int level)
{
	if(level % 2 == 0) { // TYPED
		knh_write_ascii(_ctx, w, CLASS__(O_cid(o)));
		kwb_putc(wb, ':');
	}
	if(Object_isNullObject(o)) {
		knh_write_ascii(_ctx, w, "null");
	}
	else {
//		if(checkRecursiveCalls(_ctx, sfp)) {
//			knh_write_dots(_ctx, w);
//			goto L_CLOSE;
//		}
		O_ct(o)->cdef->p(_ctx, w, RAWPTR(o), level);
	}
}

/* ------------------------------------------------------------------------ */
/* [printf] */

static const char* knh_vprintf_parseindex(const char *p, int *index)
{
	const char *ptr = p+1;
	if(ptr[0] == '{' && isdigit(ptr[1]) && ptr[2] == '}') {
		*index = ptr[1] - '0';
		ptr += 3;
		return ptr;
	}
	return p;
}

#define VA_NOP                0
#define VA_DIGIT              1 /* intptr_t */
#define VA_LONG               2 /* kint_t */
#define VA_FLOAT              3
#define VA_CHAR               4
#define VA_POINTER            5
#define VA_OBJECT             6
#define VA_FIELDN             7
#define VA_CLASS              8
#define VA_TYPE               9
#define VA_METHODN            10
#define VA_BYTES              11

typedef struct {
	int atype;
	union {
		intptr_t  dvalue;
		uintptr_t uvalue;
		kint_t     ivalue;
		kfloat_t   fvalue;
		kfloat_t   evalue;
		void         *pvalue;
		char         *svalue;
		Object       *ovalue;
		kbytes_t   bvalue;
		kcid_t     cid;
		ktype_t      type;
		ksymbol_t    fn;
		kmethodn_t   mn;
	};
} knh_valist_t;

/* ------------------------------------------------------------------------ */

void knh_vprintf(CTX, kOutputStream *w, const char *fmt, va_list ap)
{
	knh_valist_t args[10];
	const char *c = fmt;
	int i, ch, bindex = 0, bindex_max = 10;
	for(i = 0; i < bindex_max; i++) args[i].atype = 0;
	while((ch = *c) != '\0') {
		c++;
		if(ch == '%') {
			int index;
			ch = *c;
			if(ch == '%') {
				c++;
				continue;
			}
			index = bindex++;
			c = knh_vprintf_parseindex(c, &index);
			//DBG_P("bindex=%d, index=%d", bindex, index);
			switch(ch) {
				case 'd': case 'u':
				args[index].atype = VA_DIGIT; break;
				case 'l': case 'i':
					args[index].atype = VA_LONG; break;
				case 'f': case 'e':
					args[index].atype = VA_FLOAT; break;
				case 's':
					args[index].atype = VA_CHAR; break;
				case 'p':
					args[index].atype = VA_POINTER; break;
				case 'L':
				case 'K': case 'k':
				case 'O': case 'o':
					args[index].atype = VA_OBJECT; break;
				case 'N': case 'F':
					args[index].atype = VA_FIELDN; break;
				case 'M':
					args[index].atype = VA_METHODN; break;
				case 'C':
					args[index].atype = VA_CLASS; break;
				case 'T':
					args[index].atype = VA_TYPE; break;
				case 'B':
					args[index].atype = VA_BYTES; break;
				// TODO
				// we should care if "fmt" has "%%".
				// sometimes, next args is NULL.
				case '%':
					index--;
					c++;
				default:
					bindex--;
			}
			if(bindex == 10) {
				DBG_ASSERT(bindex < 10);
				break;
			}
		}
	}

	for(i = 0; i < 10; i++) {
		switch(args[i].atype) {
		case VA_DIGIT:
			args[i].dvalue = (intptr_t)va_arg(ap, intptr_t); break;
		case VA_LONG:
			args[i].ivalue = (kint_t)va_arg(ap, kint_t); break;
		case VA_FLOAT:
#if defined(K_USING_NOFLOAT)
			args[i].fvalue = (kfloat_t)va_arg(ap, kfloat_t);
#else
			args[i].fvalue = (kfloat_t)va_arg(ap, double);
#endif
			break;
		case VA_CHAR:
			args[i].svalue = (char*)va_arg(ap, char*); break;
		case VA_POINTER:
			args[i].pvalue = (void*)va_arg(ap, void*); break;
		case VA_OBJECT:
			args[i].ovalue = (Object*)va_arg(ap, Object*); break;
		case VA_FIELDN:
			args[i].fn = (ksymbol_t)va_arg(ap, int/*ksymbol_t*/); break;
		case VA_METHODN:
			args[i].mn = (kmethodn_t)va_arg(ap, int/*kmethodn_t*/); break;
		case VA_CLASS:
			args[i].cid = (kcid_t)va_arg(ap, int/*kcid_t*/); break;
		case VA_TYPE:
			args[i].type = (ktype_t)va_arg(ap, int/*ktype_t*/); break;
		case VA_BYTES:
			args[i].bvalue = (kbytes_t)va_arg(ap, kbytes_t); break;
		default:
			bindex_max = i;
			goto L_FORMAT;
		}
	}

	L_FORMAT: {
		kbytes_t b;
		c = fmt;
		bindex = 0;
		b.text = c;
		b.len = 0;
		while((ch = *c) != '\0') {
			c++;
			if(ch == '\\') {
				if(b.len > 0) {
					knh_write_utf8(_ctx, w, b, 1);
				}
				ch = *c;
				switch(ch) {
					case '\0' : return ;
					case 'n': knh_write_EOL(_ctx, w); break;
					case 't': knh_write_TAB(_ctx, w); break;
					default:
						kwb_putc(wb, '\\');
						kwb_putc(wb, ch);
				}
				b.text = c;
				b.len = 0;
			}
			else if(ch == '%') {
				if(b.len > 0) {
				  knh_write_utf8(_ctx, w, b, 1);
				}
				ch = *c;
				if(ch == '%') {
					kwb_putc(wb, '%');
					c++;
					b.text = c;
					b.len = 0;
					continue;
				}
				int index = bindex++;
				c = knh_vprintf_parseindex(++c, &index);

				switch(ch) {
					case '\0' : return ;
					case 'd':
						DBG_ASSERT(args[index].atype == VA_DIGIT);
						knh_write_dfmt(_ctx, w, K_INTPTR_FMT, args[index].dvalue);
						break;
					case 'u':
						DBG_ASSERT(args[index].atype == VA_DIGIT);
						knh_write_dfmt(_ctx, w, K_INTPTR_UFMT, args[index].uvalue);
						break;
					case 'l': case 'i' :
						DBG_ASSERT(args[index].atype == VA_LONG);
						knh_write_ifmt(_ctx, w, KINT_FMT, args[index].ivalue);
						break;
					case 'f':
						DBG_ASSERT(args[index].atype == VA_FLOAT);
						knh_write_ffmt(_ctx, w, KFLOAT_FMT, args[index].fvalue);
						break;
					case 'e':
						DBG_ASSERT(args[index].atype == VA_FLOAT);
						knh_write_ffmt(_ctx, w, KFLOAT_FMTE, args[index].fvalue);
						break;
					case 's':
						DBG_ASSERT(args[index].atype == VA_CHAR);
						knh_write(_ctx, w, B(args[index].svalue));
						break;
					case 'p':
						DBG_ASSERT(args[index].atype == VA_POINTER);
						knh_write_ptr(_ctx, w, args[index].pvalue);
						break;
					case 'L':
					case 'O': case 'o':
						DBG_ASSERT(args[index].atype == VA_OBJECT);
						knh_write_Object(_ctx, w, args[index].ovalue, FMT_s);
						break;
					case 'K': case 'k':
						DBG_ASSERT(args[index].atype == VA_OBJECT);
						knh_write_Object(_ctx, w, args[index].ovalue, FMT_line);
						break;
					case 'N': case 'F':
						DBG_ASSERT(args[index].atype == VA_FIELDN);
						knh_write_ascii(_ctx, w, FN__(args[index].fn));
						break;
					case 'M':
						DBG_ASSERT(args[index].atype == VA_METHODN);
						knh_write_mn(_ctx, w, args[index].mn);
						break;
					case 'C':
						DBG_ASSERT(args[index].atype == VA_CLASS);
						knh_write_cid(_ctx, w, args[index].cid);
						break;
					case 'T':
						DBG_ASSERT(args[index].atype == VA_TYPE);
						knh_write_type(_ctx, w, args[index].type);
						break;
					case 'B':
						DBG_ASSERT(args[index].atype == VA_BYTES);
						knh_write(_ctx,w, args[index].bvalue);
						break;
					case '%':
						index--;
						bindex--;
					default:
						//kwb_putc(wb, '%');
						kwb_putc(wb, ch);
				}
				b.text = c;
				b.len = 0;
				if(!(bindex <= bindex_max)) {
					DBG_ASSERT(bindex <= bindex_max);
					break;
				}
			}
			else {
				b.len = b.len+1;
			}
		}
		if(b.len > 0) {
		  knh_write_utf8(_ctx, w, b, 1);
		}
	}
}

KNHAPI2(void) knh_printf(CTX, kOutputStream *w, const char *fmt, ...)
{
	va_list ap;
	va_start(ap , fmt);
	knh_vprintf(_ctx, w, fmt, ap);
	va_end(ap);
}

/* ------------------------------------------------------------------------ */

static KMETHOD InputStream_getByte(CTX, ksfp_t *sfp _RIX)
{
	RETURNi_(io2_getc(_ctx, (sfp[0].in)->io2));
}

// Bytes InputStream.read(Bytes buf, size_t size);

static KMETHOD InputStream_read(CTX, ksfp_t *sfp _RIX)
{
	kBytes *ba = sfp[1].ba;
	if(IS_NULL(ba)) {
		ba = new_Bytes(_ctx, NULL, 4096);
		KSETv(sfp[1].ba, ba);
		io2_readAll(_ctx, (sfp[0].in)->io2, ba);
	}
	else {
		size_t len = Int_to2(size_t, sfp[3], ba->dim->capacity);
		if(!(len < ba->dim->capacity)) {
			THROW_OutOfRange(_ctx, sfp, len, ba->dim->capacity);
		}
		len = io2_read(_ctx, (sfp[0].in)->io2, ba->bu.buf, len);
		ba->bu.len = len;
	}
	RETURN_(ba);
}

static KMETHOD InputStream_eachLine(CTX, ksfp_t *sfp _RIX)
{
	ksfp_t *thissfp = sfp + 2 + K_CALLDELTA;
	//kMethod *mtd = knh_Func_preset(_ctx, sfp[1].fo, thissfp);
	while(1) {
		kString *line = io2_readLine(_ctx, (sfp[0].in)->io2, (sfp[0].in)->decNULL);
		if(IS_NULL(line)) break;
		KSETv(thissfp[1].o, line);
		knh_Func_invoke(_ctx, sfp[1].fo, sfp+2, 1);
//		((kcontext_t*)ctx)->esp = thissfp + 2;
//		(mtd)->fcall_1(_ctx, thissfp, K_CALLDELTA);
	}
	RETURNvoid_();
}

static KMETHOD InputStream_isClosed(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(io2_isClosed(_ctx, (sfp[0].in)->io2));
}

static KMETHOD InputStream_setCharset(CTX, ksfp_t *sfp _RIX)
{
	knh_InputStream_setCharset(_ctx, sfp[0].in, (kStringDecoder*)sfp[1].o);
	RETURN_(sfp[1].o);
}

static KMETHOD OutputStream_putByte(CTX, ksfp_t *sfp _RIX)
{
	kOutputStream *w = sfp[0].w;
	kwb_putc(wb, (int)(sfp[1].ivalue));
	RETURNvoid_();
}

static KMETHOD OutputStream_isClosed(CTX, ksfp_t *sfp _RIX)
{
	RETURNb_(io2_isClosed(_ctx, (sfp[0].in)->io2));
}

static KMETHOD OutputStream_setCharset(CTX, ksfp_t *sfp _RIX)
{
	knh_OutputStream_setCharset(_ctx, sfp[0].w, (kStringEncoder*)sfp[1].o);
	RETURN_(sfp[1].o);
}

static KMETHOD System_addHistory(CTX, ksfp_t *sfp _RIX)
{
	ctx->spi->add_history(S_text(sfp[1].s));
	RETURNvoid_();
}

#define FuncData(X) {#X , X}

static knh_FuncData_t FuncData[] = {
	FuncData(InputStream_getByte),
	FuncData(InputStream_read),
	FuncData(InputStream_eachLine),
	FuncData(InputStream_isClosed),
	FuncData(InputStream_setCharset),
	FuncData(OutputStream_putByte),
	FuncData(OutputStream_isClosed),
	FuncData(OutputStream_setCharset),
	FuncData(System_addHistory),
	{NULL, NULL},
};

void knh_initStreamFuncData(CTX, const knh_LoaderAPI_t *kapi)
{
	kapi->addStreamDPI(_ctx, "file", &STREAM_FILE);
	knh_KonohaSpace_setLinkClass(_ctx, ctx->share->rootks, STEXT("file"), ClassTBL(CLASS_Path));
	kapi->addStreamDPI(_ctx, "script", &STREAM_SCRIPT);
	knh_KonohaSpace_setLinkClass(_ctx, ctx->share->rootks, STEXT("script"), ClassTBL(CLASS_Path));
#ifdef K_USING_CURL
	kapi->addStreamDPI(_ctx, "http", &STREAM_CURL);
	knh_KonohaSpace_setLinkClass(_ctx, ctx->share->rootks, STEXT("http"), ClassTBL(CLASS_Path));
#endif
	kapi->loadFuncData(_ctx, FuncData);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
