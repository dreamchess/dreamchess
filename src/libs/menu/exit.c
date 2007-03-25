/*      
   GOAT Games Menu System
        
   exit.c

   Copyright (C)2005 Cryptic Allusion, LLC

   License: KOS/BSD

*/

#include "libmenuint.h"
#include <kos/dbglog.h>
#include <arch/exec.h>
#include <arch/arch.h>

/*

Instead of actually providing a real bootstrap to fall back to, we cheese
out here and reload the menu binary from disk using the current host KOS
instance.

*/

void goat_exit() {
	goat_menu_config_t * cfg = goat_get_config();

	if (!goat_menu_present()) {
		goat_free_config(cfg);
		arch_exit();
	}

	// Get the filename of the menu binary to reload.
	const char * fn = goat_get_string(cfg, "MenuBinary");
	void * data;
	ssize_t size = fs_load(fn, &data);
	if (size < 0) {
		dbglog(DBG_ERROR, "goat_exit: can't load menu file '%s'\n", fn);
		arch_exit();
	}
	goat_free_config(cfg);

	// Execute it.
	arch_exec(data, size);
}
