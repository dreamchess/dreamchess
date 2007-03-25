/*      
   GOAT Games Menu System
        
   config.c

   Copyright (C)2006 Cryptic Allusion, LLC

   License: KOS/BSD

*/

#include "libmenuint.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

goat_menu_config_t * goat_read_config(const char * fn) {
	FILE * f;
	char buf[2048], *b;
	int l;
	goat_menu_config_t * cfg = NULL, * cur;

	// Open the file
	f = fopen(fn, "rb");
	if (!f)
		return NULL;

	// Read each line...
	while (fgets(buf, 2048, f)) {
		// Skip comments and blank lines
		if (buf[0] == 0 || buf[0] == '#')
			continue;

		// Strip newlines
		l = strlen(buf);
		if (buf[l-1] == '\n') {
			buf[l-1] = 0;
			l--;
		}
		if (buf[l-1] == '\r') {
			buf[l-1] = 0;
			l--;
		}

		// Look for an equals
		b = strchr(buf, '=');
		if (!b)
			continue;

		*b = 0; b++;

		// Make a new config node.
		cur = calloc(1, sizeof(goat_menu_config_t));
		cur->key = strdup(buf);

		// What is its type?
		if (b[0] == '\"') {
			// String. Copy it out.
			b++;
			char * tb = strchr(b, '\"');
			assert( tb );
			*tb = 0;

			cur->type = 1;
			cur->sval = strdup(b);
		} else {
			// Int. Figure out if it's hex or dec.
			cur->type = 0;
			if (b[0] == '0' && b[1] == 'x') {
				b += 2;
				cur->ival = strtoul(b, NULL, 16);
			} else
				cur->ival = strtol(b, NULL, 10);
		}

		// Add it to the list.
		cur->next = cfg;
		cfg = cur;
	}

	fclose(f);

	printf("[goat] loaded config file %s\n", fn);

	return cfg;
}

static const goat_menu_config_t * get_cfg(const goat_menu_config_t * cfg, const char * name) {
	while (cfg && strcmp(cfg->key, name))
		cfg = cfg->next;
	return cfg;
}

int goat_get_int(const goat_menu_config_t * cfg, const char * name) {
	cfg = get_cfg(cfg, name);
	if (!cfg)
		return -1;

	assert( cfg->type == 0 );
	return cfg->ival;
}

const char * goat_get_string(const goat_menu_config_t * cfg, const char * name) {
	cfg = get_cfg(cfg, name);
	if (!cfg)
		return NULL;

	assert( cfg->type == 1 );
	return cfg->sval;
}

void goat_free_config(goat_menu_config_t * cfg) {
	while (cfg) {
		goat_menu_config_t * t = cfg->next;
		free(cfg->key);
		if (cfg->type == 1)
			free(cfg->sval);
		free(cfg);
		cfg = t;
	}
}

const char * goat_get_game_path() {
	goat_menu_config_t * cfg = goat_get_config();
	const char * fn = goat_get_string(cfg, "GamePath");
	if (!fn)
		return "/rd";
	else
		return fn;
}

const char * goat_get_save_name() {
#ifndef GOATMENU_PC
	goat_menu_config_t * cfg = goat_get_config();
	const char * fn = goat_get_string(cfg, "SaveName");
	if (!fn)
		return "GOATGM01";
	else
		return fn;
#else
	return "~/prj/game/goatmenu/save.dat";
#endif
}

uint32 goat_get_game_id() {
	goat_menu_config_t * cfg = goat_get_config();
	int id = goat_get_int(cfg, "GameID");
	if (id < 0)
		return 0;
	else
		return id;
}

