#ifndef __SCRIPT_H_
#define __SCRIPT_H_

#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class script
{
    public:
        script(std::string name);
        ~script();
        void run(std::string funcname);
        lua_State *L;
};

#endif /* __SCRIPT_H */
