
#ifndef __LIGHT_H
#define __LIGHT_H

#include "entity.h"

class light: public entity
{
    public:
        light(std::string name2);
        void render();
        void update();    
        void generate_bbox();    
};

#endif
