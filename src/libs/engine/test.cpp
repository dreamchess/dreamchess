#include "options.h"
#include <stdlib.h>
#include <iostream>

extern "C" {
#include "dir.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
int luaopen_engine(lua_State* L);
}

void testengine()
{
    lua_State *L = lua_open();

    luaL_openlibs(L);
    luaopen_engine(L);

    ch_datadir();
    chdir("scripts");
    luaL_loadfile(L, "config_manager.lua");
    ch_userdir();
    lua_pcall(L, 0, 0, 0);
}

