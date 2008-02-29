
#include <string>
#include "entity.h"

class camera: public entity
{
    public:
        camera();
        void render();
        void update();
};
