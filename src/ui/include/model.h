
#ifndef __MODEL_H
#define __MODEL_H

#include <string>
#include "entity.h"

class model: public entity
{
    public:
        model(std::string name, std::string msh2, std::string tx2, scene *parent);
        void render();
        //void update();
        void generate_bbox();
    private:
        std::string msh;
        std::string tx;
        float alpha;
        int specular;
};

#endif
