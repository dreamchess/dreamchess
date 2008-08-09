
#ifndef __TITLE_H
#define __TITLE_H

#include "scene.h"
#include "screen.h"
#include "camera.h"

class title_screen: public scene
{
    public:
        title_screen();
        void loop();
};

class title_camera: public camera
{
    public:
        int shake;
        title_camera();
        void update();
        float speed;
        float direction;
};

#endif
