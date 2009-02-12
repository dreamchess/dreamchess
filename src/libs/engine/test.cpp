#include "options.h"
#include "string.h"
#include <stdlib.h>
#include <iostream>

extern "C" {
#include "debug.h"
#include "dir.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
int luaopen_engine(lua_State* L);
int luaopen_lpeg(lua_State* L);
}

static void loadfile(lua_State *L, char *filename)
{
	ch_datadir();
	chdir("scripts");

	if (luaL_loadfile(L, filename) != 0) {
		DBG_ERROR("%s\n", lua_tostring (L, -1));
		exit(1);
	}

	ch_userdir();

	if (lua_pcall(L, 0, 0, 0) != 0) {
		DBG_ERROR("%s\n", lua_tostring (L, -1));
		exit(1);
	}
}

static void setclasspath(lua_State *L)
{
	char path[PATH_MAX + 33 + 7] = "package.path = package.path .. ';";

	ch_datadir();
	chdir("scripts");

	if (!getcwd(path + 33, PATH_MAX)) {
		DBG_ERROR("Error setting lua class path\n");
		exit(1);
	}

	strcat(path, "/?.lua'");

	if (luaL_dostring(L, path) != 0) {
		DBG_ERROR("%s\n", lua_tostring (L, -1));
		exit(1);
	}
}

void testengine()
{
	lua_State *L = lua_open();

	luaL_openlibs(L);
	setclasspath(L);

	
	/* luaopen_engine(L); */
	lua_pushcfunction(L, luaopen_engine);
	lua_pushstring(L, "engine");
	lua_call(L, 1, 0);

	/* luaopen_lpeg(L); */
/*	lua_pushcfunction(L, luaopen_lpeg);
	lua_pushstring(L, "lpeg");
	lua_call(L, 1, 0);*/

	lua_register(L, "luaopen_lpeg", luaopen_lpeg);

/*	if (luaL_dostring(L, "package.preload['lpeg'] = 'luaopen_lpeg'") != 0) {
		DBG_ERROR("%s\n", lua_tostring (L, -1));
		exit(1);
	}*/

	loadfile(L, "main.lua");

	exit(0);
}

