#ifndef __SCREEN_H_
#define __SCREEN_H_

#include "SDL.h"

#define MAXIMUM_FRAME_RATE 60
#define MINIMUM_FRAME_RATE 15
#define UPDATE_INTERVAL (1000.0 / MAXIMUM_FRAME_RATE)
#define MAX_CYCLES_PER_FRAME (MAXIMUM_FRAME_RATE / MINIMUM_FRAME_RATE)

class vec
{
    public:
    	vec(){x=0,y=0,z=0;}
        vec( float xp, float yp, float zp){x=xp;y=yp;z=zp;}
        float x, y, z;
};

vec get_mouse();

class screen
{
    int width, height;

    public:
        screen(int,int);
        void pre_render();
        void post_render();
        SDL_Surface *surface; 
};

#endif /* __SCREEN_H */
