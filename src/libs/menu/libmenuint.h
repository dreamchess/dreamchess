/*      
   GOAT Games Menu System
        
   libmenuint.h

   Copyright (C)2005 Cryptic Allusion, LLC

   License: KOS/BSD

*/

#ifndef __LIBMENUINT_H
#define __LIBMENUINT_H

#ifndef GOATMENU_PC
#	include <dc/maple.h>
#endif

// Internal libmenu stuff -- only accessed by libmenu itself and the GOAT
// Games menu system.

#include "libmenu.h"

__BEGIN_DECLS

// This is the structure of the info block the menu will leave resident
// at 0x8c00fe00 when the program is started.
typedef struct goat_menu_info {
	char	magic[4];		// Should be "GGM\0"
	char	config_path[56];	// Path to this game's config file
	uint8	config[68];		// Configuration (see below)
} goat_menu_info_t;

#define GOAT_MENU_MAGIC "GGM"

// Offsets within 'config' various config flags.
#define GOAT_CONFIG_5060HZ	0	// 50/60Hz
#define GOAT_CONFIG_H_OFFSET	2	// Horizontal picture offset
#define GOAT_CONFIG_V_OFFSET	4	// Vertical picture offset
#define GOAT_CONFIG_H_SIZE	6	// Horizontal picture size delta
#define GOAT_CONFIG_V_SIZE	8	// Vertical picture size delta

// Choices for 50/60Hz selection.
#define GOAT_CONFIG_60HZ	0
#define GOAT_CONFIG_50HZ	1

// Holds the loaded configuration from the config file passed down
// from the menu (or by hand for testing).
typedef struct goat_menu_config {
	char	* key;
	int	type;	// 0 = int, 1 = string
	union {
		int	ival;
		char	* sval;
	};
	struct goat_menu_config * next;
} goat_menu_config_t;

/*

The loaded config will be stored in a directory called goatmenu.cfg
inside each game's directory. The menu will scan the root directory for
these files and add each to the menu, ordered by game ID.

Allowable keys:

GameName[string] - Game's human-readable name (short)
GameAuthor[string] - Game's author
GameDescription[string] - Game's long description
GameID[int] - Game's ID for score table and code generation
GamePath[string] - Full path to the game's files
GameBinary[string] - Filename within the game's path for its executable file
GameThumb[string] - Filename within the game's path for a thumbnail graphic
GameImage[string] - Filename within the game's path for a full-sized graphic
MenuBinary[string] - Full path to the menu's executable file
SaveName[string] - Filename on the VMU for the save file

*/

// Read a textual config file from disk.
goat_menu_config_t * goat_read_config(const char * fn);

// Retrieves an integer config item from the struct by name.
int goat_get_int(const goat_menu_config_t * cfg, const char * name);

// Retrieves a string config item from the struct by name.
const char * goat_get_string(const goat_menu_config_t * cfg, const char * name);

// Free a loaded config stuct.
void goat_free_config(goat_menu_config_t * conf);

// Load up our config if needed, and return a pointer to it.
// (for use within games only)
goat_menu_config_t * goat_get_config();

// Find and return the info block.
goat_menu_info_t * goat_get_info();

// Tries to obtain the path to the game's files.
const char * goat_get_game_path();

// Tries to obtain the filename of the save file on the VMU.
const char * goat_get_save_name();

// Tries to obtain the game ID.
uint32 goat_get_game_id();

// Do libmenu init, from the menu itself.
int goat_menu_init();

__END_DECLS

#ifdef __cplusplus

namespace Goatmenu {

// An auto-free pointer class. This is to avoid the error return nastiness.
template<class T>
class AutoFree {
public:
	AutoFree(T * iptr = NULL) { ptr = iptr; }
	~AutoFree() { delete ptr; }
	T * release() { T * o = ptr; ptr = NULL; return o; }

	operator T*() const { return ptr; }
	T* operator->() const { return ptr; }
	bool operator!() const { return ptr == NULL; }
	bool operator==(T * other) const { return ptr == other; }
	T * operator=(T * other) { ptr = other; return other; }

	T	* ptr;
};

// This internal class will be used to keep track of a whole set of
// score tables. This represents a full GOAT Games save.
class GoatSave {
public:
	GoatSave();
	~GoatSave();

	// Warning: Don't try to *decrease* the table size!
	bool setSize(int cnt);

	int		m_tblcnt;
	score_table_t	** m_tables;
#ifndef GOATMENU_PC
	maple_device_t	* m_vmu;
#endif
};

GoatSave * loadSave();
bool saveSave(GoatSave * gs);

}

#else	// __cplusplus
#ifndef GOATMENU_PC

// Replacements for the C++ stuff

typedef int bool;
#ifndef true
#define true (1)
#endif
#ifndef false
#define false (0)
#endif

struct _ptr8 {
	uint8 *ptr;
};
typedef struct _ptr8 Ptr8;

struct _goatSave {
	int m_tblcnt;
	score_table_t **m_tables;
	maple_device_t *m_vmu;
};
typedef struct _goatSave GoatSave;

GoatSave *GoatSave_Construct();
void GoatSave_Destruct(GoatSave *pDat);
bool GoatSave_setSize(int cnt, GoatSave *pDat);

GoatSave *GoatSave_loadSave();
bool GoatSave_saveSave(GoatSave *pDat);

#endif	// GOATMENU_PC

#endif	// __cplusplus

#endif	// __LIBMENUINT_H


