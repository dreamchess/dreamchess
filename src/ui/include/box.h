
#ifndef __BOX_H
#define __BOX_H

#include "entity.h"
#include "texture.h"

class box: public entity
{
    public:
        box( float xsize, float ysize, std::string file );
        void render();
        void update();
    private:
        texture *tx;    
};

#endif
