#ifndef __ENTITY_H_
#define __ENTITY_H_

//#include "dreamchess_ui.h"
#include <string>

class scene;

class entity
{
    public:
        entity();
        virtual void update() = 0;
        virtual void render() = 0;

        float xpos,ypos,zpos;
        float xrot,yrot,zrot;
        float xscale,yscale,zscale;
        std::string name;
        std::string type;
        scene *parent_scene;
};

#endif /* __ENTITY_H */
