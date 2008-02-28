#ifndef __SCREEN_H_
#define __SCREEN_H_

#include "SDL/SDL.h"

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
