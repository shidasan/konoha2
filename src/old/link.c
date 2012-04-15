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

#include"commons.h"

/* ************************************************************************ */

#ifdef K_USING_CURL
#include<curl/curl.h>
#endif/*K_USING_CURL*/

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* [LINK] */

///* charset:UTF-8 */
//static kbool_t CHARSET_hasType(CTX, kcid_t cid)
//{
//	return (cid == CLASS_StringEncoder || cid == CLASS_StringDecoder);
//}
//
//static kbool_t CHARSET_exists(CTX, kKonohaSpace *ns, kbytes_t path)
//{
//	kbytes_t t = knh_bytes_next(path, ':');
//	knh_iconv_t ic = ctx->spi->iconv_openSPI(t.text, K_ENCODING);
//	if(ic != (knh_iconv_t)(-1)) {
//		ctx->spi->iconv_closeSPI(ic);
//		return 1;
//	}
//	else if(knh_bytes_strcasecmp(t, STEXT(K_ENCODING)) == 0) {
//		return 1;
//	}
//	return 0;
//}
//
//static kObject* CHARSET_newObjectNULL(CTX, kKonohaSpace *ns, kcid_t cid, kString *s)
//{
//	kbytes_t t = knh_bytes_next(S_tobytes(s), ':');
//	if(cid == CLASS_StringEncoder) {
//		return (kObject*)new_StringEncoderNULL(_ctx, t);
//	}
//	if(cid == CLASS_StringDecoder) {
//		return (kObject*)new_StringDecoderNULL(_ctx, t);
//	}
//	return NULL;
//}

//static const knh_LinkDPI_t LINK_CHARSET = {
//	"charset", "StringConverter|StringDecoder", CHARSET_hasType, CHARSET_exists, CHARSET_newObjectNULL,
//};

//static kbool_t PACKAGE_hasType(CTX, kcid_t cid)
//{
//	return (cid == CLASS_Bytes || cid == CLASS_InputStream);
//}
//
//static kbool_t PACKAGE_exists(CTX, kKonohaSpace *ns, kbytes_t path)
//{
//	CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
//	kbytes_t bpath = knh_bytes_next(path, ':');
//	bpath = CWB_ensure(_ctx, cwb, bpath, K_PATHMAX);
//	kbool_t res = knh_buff_addPackagePath(_ctx, cwb->ba, cwb->pos, bpath);
//	CWB_close(cwb);
//	return res;
//}
//
//static kObject* PACKAGE_newObjectNULL(CTX, kKonohaSpace *ns, kcid_t cid, kString *s)
//{
//	kObject *res = NULL;
//	if(cid == CLASS_Bytes) {
//		kBytes* ba = new_Bytes(_ctx, NULL, 256);
//		if(!knh_buff_addPackagePath(_ctx, ba, 0, S_tobytes(s))) {
//			kObjectoNULL(_ctx, ba);
//		}
//		return UPCAST(ba);
//	}
//	if(cid == CLASS_InputStream) {
//		CWB_t cwbbuf, *cwb = CWB_open(_ctx, &cwbbuf);
//		kbytes_t bpath = knh_bytes_next(S_tobytes(s), ':');
//		knh_buff_addPackagePath(_ctx, cwb->ba, cwb->pos, bpath);
//		res = (kObject*)knh_Bytes_openInputStream(_ctx, cwb->ba, cwb->pos, s);
//		CWB_close(cwb);
//	}
//	return res;
//}

/* ------------------------------------------------------------------------ */
/* K_DSPI_QUERY */


/* ------------------------------------------------------------------------ */

static kbool_t tolowercase(CTX, knh_conv_t *cv, const char *text, size_t len, kBytes *tobuf)
{
	size_t i, s = BA_size(tobuf);
	knh_Bytes_write2(_ctx, tobuf, text, len);
	kbytes_t tt = {{tobuf->bu.text + s}, BA_size(tobuf) - s};
	for(i = 0; i < tt.len; i++) {
		int ch = tt.utext[i];
		if('A' <= ch && ch <= 'Z') {
			tt.ubuf[i] = ch - 'A' + 'a';
		}
	}
	return 1;
}

static kbool_t touppercase(CTX, knh_conv_t *cv, const char *text, size_t len, kBytes *tobuf)
{
	size_t i, s = BA_size(tobuf);
	knh_Bytes_write2(_ctx, tobuf, text, len);
	kbytes_t tt = {{tobuf->bu.text + s}, BA_size(tobuf) - s};
	for(i = 0; i < tt.len; i++) {
		int ch = tt.utext[i];
		if('a' <= ch && ch <= 'z') {
			tt.ubuf[i] = ch - 'a' + 'A';
		}
	}
	return 1;
}

static const knh_ConverterDPI_t TO_lower = {
	K_DSPI_CONVTO, "lower",
	NULL, tolowercase, tolowercase, tolowercase, tolowercase, NULL, NULL,
};

static const knh_ConverterDPI_t TO_upper = {
	K_DSPI_CONVTO, "upper",
	NULL, touppercase, touppercase, touppercase, touppercase, NULL, NULL,
};

void knh_loadSystemDriver(CTX, kKonohaSpace *ns)
{
	const knh_LoaderAPI_t *api = knh_getLoaderAPI();
	knh_KonohaSpace_setLinkClass(_ctx, ns, STEXT("link"), ClassTBL(CLASS_Tdynamic));
	api->addConverterDPI(_ctx, "lower", &TO_lower, NULL);
	api->addConverterDPI(_ctx, "upper", &TO_upper, NULL);
	knh_loadSystemQueryDriver(_ctx, ns);
	knh_loadFFIDriver(_ctx, ns);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
