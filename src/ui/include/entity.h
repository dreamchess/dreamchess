#ifndef __ENTITY_H_
#define __ENTITY_H_

#include <string>
#include "script.h"
#include "screen.h"
#include "resource.h"

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

		float bbox_x1, bbox_x2;
		float bbox_y1, bbox_y2;
		float bbox_z1, bbox_z2;
		
		bool post_mouse_render;

        script *sc;
        scene *parent_scene;
};

#endif /* __ENTITY_H */
