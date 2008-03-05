#ifndef __SCENE_H_
#define __SCENE_H_

#include <vector>
#include "entity.h"
#include "camera.h"

class scene
{
    public:
        ~scene();
        void render();
        void update();
        void add(entity *ent);
        void clear();
        void list();
        int count_type(std::string type);
        int find_type(std::string type, int index);
        camera *active_cam;
        std::vector<entity*> entities; 
};

#endif /* __SCENE_H */
