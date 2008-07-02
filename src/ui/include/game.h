#ifndef __GAME_H_
#define __GAME_H_

#define MAXIMUM_FRAME_RATE 60
#define MINIMUM_FRAME_RATE 15
#define UPDATE_INTERVAL (1000.0 / MAXIMUM_FRAME_RATE)
#define MAX_CYCLES_PER_FRAME (MAXIMUM_FRAME_RATE / MINIMUM_FRAME_RATE)

#include "SDL/SDL.h"
#include "input.h"
#include "scene.h"
#include "screen.h"

class game
{
    public:
        game();
        ~game();
        void start();
        virtual void loop() = 0; 
        input_layer input;
        screen *scr;
        scene scn;       
        int ups; 
};

#endif /* __GAME_H */
