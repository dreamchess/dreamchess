
#ifndef __CAMERA_H
#define __CAMERA_H

#include <string>
#include "entity.h"

class camera: public entity
{
    public:
        camera();
        void render();
        void update();
};

#endif
