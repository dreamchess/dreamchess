
#include <iostream>
#include <string>
#include "script.h"

script::script( std::string name )
{
    L = lua_open();


	luaL_openlibs(L);

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
    lua_call(L,0,0);
}

