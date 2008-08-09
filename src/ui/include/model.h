
#ifndef __MODEL_H
#define __MODEL_H

#include <string>
#include "entity.h"

class model: public entity
{
    public:
        model(std::string msh2, std::string tx2, void *parent);
        void render();
        void update();
    private:
        std::string msh;
        std::string tx;
        float alpha;
        int specular;
};

#endif
