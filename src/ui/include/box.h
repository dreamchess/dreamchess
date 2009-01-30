
#ifndef __BOX_H
#define __BOX_H

#include "entity.h"
#include "texture.h"

class box: public entity
{
    public:
        box(std::string name2, float xsize, float ysize, std::string tx, scene *parent );
        void render();
        void generate_bbox();
        void update();
    private:
        std::string tx;    
};

#endif
