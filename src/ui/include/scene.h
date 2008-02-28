#ifndef __SCENE_H_
#define __SCENE_H_

#include <vector>
#include "entity.h"

class scene
{
    public:
        ~scene();
        void render();
        void update();
        void add(entity *ent);
        void clear();
        void list();
    private:
        std::vector<entity*> entities; 
};

#endif /* __SCENE_H */
