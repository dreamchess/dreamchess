
#ifndef __MODEL_H
#define __MODEL_H

#include <string>
#include "entity.h"
#include "mesh.h"
#include "texture.h"

class model: public entity
{
    public:
        model(char *msh2, char *tx2);
        void render();
        void update();
    private:
        mesh *msh;
        texture *tx;
        GLfloat alpha;
        int specular;
};

#endif
