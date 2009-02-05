#ifndef __SCREEN_H_
#define __SCREEN_H_

#include "SDL.h"

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
        void update();
    private:
        SDL_Surface *surface; 
};

#endif /* __SCREEN_H */
