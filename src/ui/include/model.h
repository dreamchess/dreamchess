
#ifndef __MODEL_H
#define __MODEL_H

#include <string>
#include "entity.h"
#include "mesh.h"
#include "texture.h"

class model: public entity
{
    public:
        model(std::string msh2, std::string tx2);
        void render();
        void update();
    private:
        mesh *msh;
        texture *tx;
        GLfloat alpha;
        int specular;
};

#endif
