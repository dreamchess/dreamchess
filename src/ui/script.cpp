
#include <iostream>
#include <string>
#include "script.h"

script::script( std::string name )
{
  /*  L = lua_open();

    luaopen_io(L);
    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    //luaopen_loadlib(L); 

    luaL_loadfile(L, name.c_str());  
    lua_pcall(L, 0, 0, 0); */   
}

script::~script()
{
  //  lua_close(L);
}

void script::run( std::string funcname )
{
   // lua_getglobal(L, funcname.c_str());
   // lua_call(L,0,0);
}

