#ifndef __SCENE_H_
#define __SCENE_H_

#include "resource.h"
#include "entity.h"
#include "camera.h"
#include "input.h"
#include "screen.h"

#define MAXIMUM_FRAME_RATE 60
#define MINIMUM_FRAME_RATE 15
#define UPDATE_INTERVAL (1000.0 / MAXIMUM_FRAME_RATE)
#define MAX_CYCLES_PER_FRAME (MAXIMUM_FRAME_RATE / MINIMUM_FRAME_RATE)

class scene: public resource_list
{
    public:
        void render();
        void update();
        void start();
        virtual void loop() = 0; 
        int count_type(std::string type);
        int find_type(std::string type, int index);
        camera *active_cam;
        input_layer input;
        screen *scr;       
        int ups; 
};

#endif /* __SCENE_H */
