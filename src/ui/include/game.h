
#ifndef __GAME_H
#define __GAME_H

#include "scene.h"
#include "camera.h"

class game: public scene
{
    public:
        game();
        void loop();
};

#endif
