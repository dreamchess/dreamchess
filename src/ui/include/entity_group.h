
#include <string>
#include "model.h"
#include "box.h"

class entity_group: public resource_list, public entity
{
    public:
        entity_group();
        void render();
        void update();
        void generate_bbox();
};