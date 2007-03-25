/*      
   GOAT Games Menu System
        
   libmenu.c

   Copyright (C)2005 Cryptic Allusion, LLC

   License: KOS/BSD

*/

#include "libmenuint.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#ifndef GOATMENU_PC
#	include <kos/fs.h>
#endif

static int menu_present = -1;
static goat_menu_info_t info_dbg;
static goat_menu_info_t * info
#ifdef GOATMENU_PC
	= &info_dbg;
#define fs_chdir(x)
#else
	= (goat_menu_info_t *)0x8c00fb00;
#endif
static goat_menu_config_t * cfg = NULL;

goat_menu_config_t * goat_get_config() {
	return cfg;
}

goat_menu_info_t * goat_get_info() {
	return info;
}

int goat_menu_present() {
	return menu_present;
}

int goat_init() {
	// If we've already been initted then don't try again.
	if (menu_present >= 0)
		return goat_menu_present() ? 0 : -1;

	// Find the info block.
	if (memcmp(info->magic, GOAT_MENU_MAGIC, sizeof(info->magic))) {
		printf("[goat] menu not present\n");
		// Try to provide some sane defaults (for debugging mainly).
		memset(&info_dbg, 0, sizeof(info_dbg));
		info = &info_dbg;
		strcpy(info->magic, GOAT_MENU_MAGIC);
		strcpy(info->config_path, "/rd");

		menu_present = 0;
	} else {
		printf("[goat] menu present\n");
		menu_present = 1;
	}

	// Try to load up our config if we have one.
	cfg = goat_read_config(info->config_path);

	// Change into the game's proper path, if it has one.
	if (cfg) {
		const char * p = goat_get_game_path();
		printf("[goat] config loaded, our path is %s\n", p);
		fs_chdir(p);
	} else {
		printf("[goat] config not loaded, using defaults\n");
		fs_chdir("/rd");
	}
	
	return menu_present - 1;
}

// This basically just manually puts the info block in place before
// calling the normal init.
int goat_menu_init() {
	memset(info, 0, sizeof(info_dbg));
	strcpy(info->magic, GOAT_MENU_MAGIC);

	return goat_init();
}
