#ifndef __ENTITY_H_
#define __ENTITY_H_

//#include "dreamchess_ui.h"
#include <string>

class entity
{
    public:
        entity();
        entity( float x, float y, float z);
        entity( float x, float y, float z, 
            float xr, float yr, float zr );
        entity( float x, float y, float z, 
            float xr, float yr, float zr,
            float xs, float ys, float zs );
        virtual void update() = 0;
        virtual void render() = 0;

        float xpos,ypos,zpos;
        float xrot,yrot,zrot;
        float xscale,yscale,zscale;
        std::string name;
};

#endif /* __ENTITY_H */
