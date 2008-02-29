
#include <string>
#include "entity.h"
#include "mesh.h"

class model: public entity
{
    public:
        model(char *name);
        void render();
        void update();
    private:
        mesh *msh;
        //texture *tx;
        GLfloat alpha;
        int specular;
};

