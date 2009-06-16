
#ifndef __MD3_H
#define __MD3_H

#include "SDL_opengl.h"
#include "SDL.h"
#include <string>

#include <md3/structure.h>
#include <md3/loader.h>
#include <md3/convert.h>
#include <md3/mesh.h>

#include "texture.h"


class md3
{
    public:
        md3(std::string filename);
        void render( texture *tx );
        
        libmd3_file *modelFile;        
};

#endif
