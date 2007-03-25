/*      
   GOAT Games Menu System

   Ported to C by M.Brent, adapted from:

   score_table.cpp

   Copyright (C)2005 Cryptic Allusion, LLC

   License: KOS/BSD

*/

// Note: this code is not thread-safe

#include <kos.h>
#include <bzlib/bzlib.h>
#include <assert.h>
#include "libmenuint.h"

// This must be used instead of MAPLE_FOREACH because for some reason
// VMU devices never get __dev->status_valid set (!?)
#define VMU_FOREACH_BEGIN() \
do { \
	maple_device_t  * __dev; \
	int     __i = 0; \
	while ( (__dev = maple_enum_type(__i, MAPLE_FUNC_MEMCARD)) ) {

#define VMU_FOREACH_END() \
		__i++; \
	} \
} while(0);

GoatSave *GoatSave_Construct() {
	GoatSave *pDat;

	pDat=(GoatSave *)malloc(sizeof(GoatSave));
	if (NULL != pDat) {
		pDat->m_tblcnt = 0;
		pDat->m_tables = NULL;
		pDat->m_vmu = NULL;
	}
	return pDat;
}
void GoatSave_Destruct(GoatSave *pDat) {
	int i;

	if (NULL != pDat) {
		if (pDat->m_tblcnt) {
			for (i=0; i<pDat->m_tblcnt; i++) {
				if (pDat->m_tables[i]) {
					free(pDat->m_tables[i]);
				}
			}
			free(pDat->m_tables);
		}
		free(pDat);
	}
}

// Warning: Don't try to *decrease* the table size!
bool GoatSave_setSize(int cnt, GoatSave *pDat) {
	int i;

	assert( cnt >= pDat->m_tblcnt );
	if (cnt == pDat->m_tblcnt)
		return true;

	score_table_t ** newtbls = malloc(cnt * sizeof(score_table_t*));
	if (!newtbls)
		return false;
	for (i=0; i<cnt; i++) {
		newtbls[i] = NULL;
	}
	if (pDat->m_tables) {
		// Move over old table references, where applicable.
		for (i=0; i<pDat->m_tblcnt; i++) {
			newtbls[i] = pDat->m_tables[i];
		}
		free(pDat->m_tables);
	}

	pDat->m_tblcnt = cnt;
	pDat->m_tables = newtbls;

	return true;
}

// Internal function that loads the entire save game and parses it out.
GoatSave * GoatSave_loadSave() {
	Ptr8 buf = {NULL};
	int	bufsize = -1;
	maple_device_t * src = NULL;
	const char * fn;
	int i;

	// Get our save name.
	fn = goat_get_save_name();

	// Search all available VMUs for a goat games save.
	VMU_FOREACH_BEGIN()
		if (vmufs_read(__dev, fn, (void **)&buf.ptr, &bufsize) >= 0) {
			// Yay, we found one!
			src = __dev;
			break;
		}
	VMU_FOREACH_END()

	if (!src) {
		// There was no save.
		errno = ENOENT;
		//
		if (NULL != buf.ptr) free(buf.ptr);
		return NULL;
	}

	assert( buf.ptr );

	// Parse the save file into useful chunks.
	vmu_pkg_t pkg;
	if (vmu_pkg_parse((uint8 *)buf.ptr, &pkg) < 0) {
		// Something was screwed in the save file...
		errno = EIO;
		return NULL;
	}

	// The payload will be the original size plus the score tables, bz2'd.
	unsigned int uncmpsize = *((int *)pkg.data);
	if (uncmpsize > 120*1024) {
		// Draw a line somewhere at what a reasonable size is, in
		// case it was corrupted...
		errno = EIO;
		return NULL;
	}

	// We'll alloc a bit extra just in case...
	Ptr8 tablebuf;
	tablebuf.ptr=(uint8*)malloc(uncmpsize+64);

	//if (!tablebuf) {
	if (NULL == tablebuf.ptr) {
		if (NULL != buf.ptr) free(buf.ptr);
		errno = ENOMEM;
		return NULL;
	}
	uncmpsize += 64;

	// Decompress it.
	int bzrv = BZ2_bzBuffToBuffDecompress((char *)tablebuf.ptr, &uncmpsize,
		(char *)(pkg.data + 4), pkg.data_len - 4, 1, 0);
	if (bzrv != BZ_OK) {
		// Looks like the data was corrupted (or there was some
		// other error during decompress).
		dbglog(DBG_ERROR, "score_table load: BZ error %d\n", bzrv);
		if (NULL != buf.ptr) free(buf.ptr);
		if (NULL != tablebuf.ptr) free(tablebuf.ptr);

		errno = EIO;
		return NULL;
	}

	// Find the first actual score table in the data.
	score_table_t * table = (score_table_t *)(tablebuf.ptr + 4);
	int tablecnt = *((uint32 *)tablebuf.ptr);

	// Allocate a save struct with enough slots in it.
	GoatSave *save=GoatSave_Construct();

	if (NULL == save) {
		if (NULL != buf.ptr) free(buf.ptr);
		if (NULL != tablebuf.ptr) free(tablebuf.ptr);
		errno = ENOMEM;
		return NULL;
	}

	if (!GoatSave_setSize(tablecnt, save)) {
		GoatSave_Destruct(save);
		if (NULL != buf.ptr) free(buf.ptr);
		if (NULL != tablebuf.ptr) free(tablebuf.ptr);
		errno = ENOMEM;
		return NULL;
	}
	save->m_vmu = src;

	// Traverse the save and break it up into nice pieces.
	for (i=0; i<tablecnt; i++) {
		// Copy one over.
		save->m_tables[i] = (score_table_t *)malloc(SCORE_TABLE_SIZE(table));
		if (NULL == save->m_tables[i]) {
			GoatSave_Destruct(save);
			errno = ENOMEM;
			return NULL;
		}
		memcpy(save->m_tables[i], table, SCORE_TABLE_SIZE(table));

		// Move to the next.
		table = (score_table_t *)(((uint8 *)table) + SCORE_TABLE_SIZE(table));
	}

	if (NULL != buf.ptr) free(buf.ptr);
	if (NULL != tablebuf.ptr) free(tablebuf.ptr);

	return save;
}

bool GoatSave_saveSave(GoatSave * save) {
	const char * fn = goat_get_save_name();
	int i;

	// Erase the save game first so space tests are accurate
	goat_save_erase();

	// Figure out the size of all the saves together.
	int totsize = 4;
	for (i=0; i<save->m_tblcnt; i++)
		totsize += SCORE_TABLE_SIZE(save->m_tables[i]);

	// Combine all the saves into a big save table block.
	Ptr8 tblblock;
	tblblock.ptr = (uint8*)malloc(totsize * sizeof(uint8));
	if (NULL == tblblock.ptr) {
		errno = ENOMEM;
		return false;
	}

	*((uint32 *)tblblock.ptr) = save->m_tblcnt;
	uint8 * outp = tblblock.ptr + 4;
	for (i=0; i<save->m_tblcnt; i++) {
		memcpy(outp, save->m_tables[i], SCORE_TABLE_SIZE(save->m_tables[i]));
		outp += SCORE_TABLE_SIZE(save->m_tables[i]);
	}
	assert( outp == (tblblock.ptr + totsize) );

	// Compress it. Leave some extra space in case we end up
	// *expanding* the data... ^_^;
	Ptr8 cmptbl;
	cmptbl.ptr = (uint8*)malloc((totsize*2+4) * sizeof(uint8));
	if (NULL == cmptbl.ptr) {
		if (NULL != tblblock.ptr) free(tblblock.ptr);
		errno = ENOMEM;
		return false;
	}

	unsigned int cmpsize = totsize*2+4;
	int bzrv = BZ2_bzBuffToBuffCompress((char *)cmptbl.ptr + 4, &cmpsize,
		(char *)tblblock.ptr, totsize, 1, 0, 30);
	if (bzrv != BZ_OK) {
		// WTF?
		dbglog(DBG_ERROR, "score_table save: BZ compress error %d\n", bzrv);
		if (NULL != cmptbl.ptr) free(cmptbl.ptr);
		if (NULL != tblblock.ptr) free(tblblock.ptr);
		errno = EIO;
		return false;
	}

	// Store the uncompressed size at the front.
	*((uint32 *)cmptbl.ptr) = totsize;

	// Bundle that into a VMU package.
	vmu_pkg_t pkg;
	strcpy(pkg.desc_short, "GOAT Games V1");
	strcpy(pkg.desc_long, "GOAT Games Volume 1");
	strcpy(pkg.app_id, fn);
	pkg.icon_cnt = 0;
	pkg.eyecatch_type = VMUPKG_EC_NONE;
	pkg.data_len = cmpsize + 4;
	pkg.data = cmptbl.ptr;
	
	Ptr8 pkgd;
	pkgd.ptr=NULL;

	int pkgsize;
	if (vmu_pkg_build(&pkg, &pkgd.ptr, &pkgsize) < 0) {
		// Again.. WTF?
		if (NULL != pkgd.ptr) free(pkgd.ptr);
		if (NULL != cmptbl.ptr) free(cmptbl.ptr);
		if (NULL != tblblock.ptr) free(tblblock.ptr);
		errno = EIO;
		return false;
	}

	int pkgblocks = (pkgsize / 512) + ((pkgsize % 512) ? 1 : 0);

	// Pad the data up if needed.
	if (pkgsize % 512) {
		//printf("score_table save: padding from %d to %d bytes\n",
		//	pkgsize, pkgblocks*512);
		pkgd.ptr = (uint8*)realloc(pkgd.ptr, pkgblocks*512);
		memset(pkgd.ptr + pkgsize, 0, pkgblocks*512 - pkgsize);
		pkgsize = pkgblocks * 512;
	}

	// Do we have a device already?
	if (save->m_vmu) {
		// Check to make sure we have enough free space.
		if (vmufs_free_blocks(save->m_vmu) < pkgblocks) {
			if (NULL != pkgd.ptr) free(pkgd.ptr);
			if (NULL != cmptbl.ptr) free(cmptbl.ptr);
			if (NULL != tblblock.ptr) free(tblblock.ptr);
			errno = ENOSPC;
			return false;
		}
	} else {
		// Search all available VMUs for sufficient space
		VMU_FOREACH_BEGIN()
			if (vmufs_free_blocks(__dev) >= pkgblocks) {
				// Yay, we found one!
				save->m_vmu = __dev;
				break;
			}
		VMU_FOREACH_END()

		if (!save->m_vmu) {
			if (NULL != pkgd.ptr) free(pkgd.ptr);
			if (NULL != cmptbl.ptr) free(cmptbl.ptr);
			if (NULL != tblblock.ptr) free(tblblock.ptr);
			errno = ENOSPC;
			return false;
		}
	}

	// Write it.
	if (vmufs_write(save->m_vmu, fn, pkgd.ptr, pkgsize, VMUFS_OVERWRITE) < 0) {
		if (NULL != pkgd.ptr) free(pkgd.ptr);
		if (NULL != cmptbl.ptr) free(cmptbl.ptr);
		if (NULL != tblblock.ptr) free(tblblock.ptr);
		errno = EIO;
		return false;
	}

	if (NULL != pkgd.ptr) free(pkgd.ptr);
	if (NULL != cmptbl.ptr) free(cmptbl.ptr);
	if (NULL != tblblock.ptr) free(tblblock.ptr);
	return true;
}

score_table_t * goat_load_score_table(uint32 game_id) {
	GoatSave *save;
	save=GoatSave_loadSave();

	if (!save)
		return NULL;

	if (game_id == 0)
		game_id = goat_get_game_id();

	// Search through the tables and try to find this app's table.
	int i;
	for (i=0; i<save->m_tblcnt; i++) {
		if (save->m_tables[i]->game_id == game_id) break;
	}

	// Did we find one?
	if (i >= save->m_tblcnt) {
		GoatSave_Destruct(save);
		errno = ENOMSG;
		return NULL;
	}

	// Yes... pluck it out and let autofree kill the rest.
	score_table_t * rv = save->m_tables[i];
	save->m_tables[i] = NULL;

	// So much for autofree ;)
	GoatSave_Destruct(save);

	return rv;
}

int goat_save_score_table(uint32 game_id, score_table_t * table) {
	bool bRet;
	// Try to load up the old score table first.
	GoatSave *save;
	save=GoatSave_loadSave();

	if (game_id == 0)
		game_id = goat_get_game_id();

	int idx = -1;
	if (!save) {
		// Not having a save already is OK. Everything else indicates
		// a fairly serious problem.
		if (errno != ENOENT) {
			GoatSave_Destruct(save);
			return -1;
		}

		// Make a new one with a single entry.
		save = GoatSave_Construct();
		GoatSave_setSize(1, save);
		idx = 0;
		dbglog(DBG_INFO, "score_table save: making new save file for entry %08lx\n", game_id);
	} else {
		// Search through the tables and try to find this app's table.
		int i;
		for (i=0; i<save->m_tblcnt; i++) {
			if (save->m_tables[i]->game_id == game_id) break;
		}

		// Did we find one?
		if (i < save->m_tblcnt) {
			idx = i;
			dbglog(DBG_INFO, "score_table save: replacing old entry for %08lx\n", game_id);
		} else {
			idx = save->m_tblcnt;
			GoatSave_setSize(idx + 1, save);
			dbglog(DBG_INFO, "score_table save: making new entry for %08lx\n", game_id);
		}
	}

	if (!save->m_tables[idx]) {
		save->m_tables[idx] = (score_table_t *)malloc(SCORE_TABLE_SIZE(table));
		if (!save->m_tables[idx]) {
			GoatSave_Destruct(save);
			errno = ENOMEM;
			return -1;
		}
	}
	memcpy(save->m_tables[idx], table, SCORE_TABLE_SIZE(table));

	save->m_tables[idx]->game_id = game_id;

	bRet=GoatSave_saveSave(save);
	GoatSave_Destruct(save);

	return bRet ? 0 : -1;
}

void goat_save_erase() {
	const char * fn = goat_get_save_name();

	// Search all available VMUs for a goat games save. If we find one,
	// delete it and stop searching.
	VMU_FOREACH_BEGIN()
		if (vmufs_delete(__dev, fn) >= 0)
			return;
	VMU_FOREACH_END()
}

