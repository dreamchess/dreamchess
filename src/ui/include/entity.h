#ifndef __ENTITY_H_
#define __ENTITY_H_

#include <string>
#include "script.h"
#include "screen.h"
#include "resource.h"

#define FALSE 0
#define TRUE -1

class scene;

class entity: public resource
{
    public:
        entity();        
        virtual void render() = 0;
        virtual void generate_bbox();
        void update();
        bool collision_at( vec pos );

        float xpos,ypos,zpos;
        float xrot,yrot,zrot;
        float xscale,yscale,zscale;
        float xsize,ysize,zsize;

        script *sc;
        scene *parent_scene;
};

#endif /* __ENTITY_H */
