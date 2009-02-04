
#include <iostream>
#include <string>
#include "script.h"

extern "C" int luaopen_luatest(lua_State* L);

script::script( std::string name )
{
    L = lua_open();


    luaL_openlibs(L);
    luaopen_luatest(L);

    luaL_loadfile(L, name.c_str());  
    lua_pcall(L, 0, 0, 0);
}

script::~script()
{
    lua_close(L);
}

void script::run( std::string funcname )
{
    lua_getglobal(L, funcname.c_str());
    if (lua_pcall(L,0,0,0) != 0) {
         std::cout << lua_tostring(L, -1) << std::endl;
         lua_pop(L, 1);
    }
}
