#ifndef __ENTITY_H_
#define __ENTITY_H_

#include <string>
#include "script.h"
#include "screen.h"

#define FALSE 0
#define TRUE -1

class scene;

class entity
{
    public:
        entity();        
        virtual void render() = 0;
        void update();
        bool collision_at( vec pos );

        std::string name;
        std::string type;
        float xpos,ypos,zpos;
        float xrot,yrot,zrot;
        float xscale,yscale,zscale;
        float xsize,ysize,zsize;

        script *sc;
        scene *parent_scene;
};

#endif /* __ENTITY_H */
