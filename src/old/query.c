/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
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

#ifdef __cplusplus
extern "C" {
#endif

#include<sqlite3.h>

static sqlite3 *SQLITE3_qopen(CTX, kPath *path)
{
	sqlite3 *qconn = NULL;
	int r = sqlite3_open(path->ospath, &qconn);
	if (r != SQLITE_OK) {
		return NULL;
	}
	return qconn;
}

//static void SQLITE3_qclose(CTX, kPath *path, sqlite3 *qconn)
//{
//	sqlite3_close(qconn);
//}

static void SQLITE3_qfree(void *qcur)
{
	knitr_t *m = (knitr_t*)qcur;
	sqlite3_finalize((sqlite3_stmt*)m->qstmt);
	sqlite3_close((sqlite3*)m->qconn);
	m->nfree = NULL;
	m->nptr = NULL;
}

typedef ITRNEXT (*fsqlite3_next)(CTX, ksfp_t *, sqlite3_stmt* _RIX);

static ITRNEXT nextData(CTX, ksfp_t *sfp, sqlite3_stmt *stmt _RIX)
{
	size_t i, column_size = (size_t)sqlite3_column_count(stmt);
	kMap *dmap = new_DataMap(_ctx);
	for(i = 0; i < column_size; i++) {
		const char *name = (const char*)sqlite3_column_name(stmt, i);
		int type = sqlite3_column_type(stmt, i);
		switch(type) {
			case SQLITE_INTEGER: {
				knh_DataMap_setInt(_ctx, dmap, name, (kint_t)sqlite3_column_int64(stmt, i));
				break;
			}
			case SQLITE_FLOAT: {
				knh_DataMap_setFloat(_ctx, dmap, name, (kfloat_t)sqlite3_column_double(stmt, i));
				break;
			}
			case SQLITE_TEXT: {
				knh_DataMap_setString(_ctx, dmap, name, (const char*)sqlite3_column_text(stmt,i));
				break;
			}
			case SQLITE_BLOB: {
				knh_DataMap_setBlob(_ctx, dmap, name, (const char*)sqlite3_column_blob(stmt,i), sqlite3_column_bytes(stmt, i));
				break;
			}
//				case SQLITE_NULL:
//				default: {
//					ResultSet_setNULL(_ctx, rs, i);
//				}
		}
	}
	ITRNEXT_(dmap);
}

static ITRNEXT SQLITE3_next(CTX, ksfp_t *sfp, fsqlite3_next fnext _RIX)
{
	kIterator *itr = ITR(sfp);
	sqlite3_stmt *stmt = DP(itr)->m.qstmt;
	if(stmt == NULL) {
		kView *view = (kView*)DP(itr)->source;
		sqlite3 *qconn = SQLITE3_qopen(_ctx, view->path);
		kString *query = knh_View_getQuery(_ctx, view);
		if(qconn == NULL) {
			ITREND_();
		}
		sqlite3_prepare(qconn, S_totext(query), S_size(query), &stmt, NULL);
		DP(itr)->m.nptr = &(DP(itr)->m);
		DP(itr)->m.nfree = SQLITE3_qfree;
		DP(itr)->m.qconn = qconn;
		DP(itr)->m.qstmt = stmt;
	}
	int r = sqlite3_step(stmt);
	if(SQLITE_ROW == r) {
		return fnext(_ctx, sfp, stmt, K_RIX);
	}
	ITREND_();
}

static ITRNEXT SQLITE3_nextData(CTX, ksfp_t *sfp _RIX)
{
	return SQLITE3_next(_ctx, sfp, nextData, K_RIX);
}

//static const knh_PathDPI_t STREAM_SQLITE3 = {
//	K_STREAM_NET, "NOFILE", K_PAGESIZE,
//	NOFILE_exists, NOFILE_ospath, NOFILE_openNULL,
////	NOFILE_open, NOFILE_read, NOFILE_write, NOFILE_close,
////	NOFILE_info, NOFILE_getc, NOFILE_readline, NOFILE_feof, NOFILE_flush,
//	SQLITE3_nextData,
//};

void knh_loadSystemQueryDriver(CTX, kLingo *ns)
{
//	const knh_LoaderAPI_t *api = knh_getLoaderAPI();
//	api->addStreamDPI(_ctx, "sqlite3", &STREAM_SQLITE3);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

