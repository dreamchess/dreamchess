/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the COPYRIGHT file distributed
**  with this source distribution.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DREAMCHESS_MODEL_H
#define DREAMCHESS_MODEL_H

class Texture;
class Mesh;

#include "glm/glm.hpp"
#include "ResourcePool.h"

typedef enum primitive_type
{
    PRIM_TRIANGLES,
    PRIM_STRIP
} primitive_type_t;

typedef struct group
{
    primitive_type_t type;
    int len;
    unsigned int *data;
} group_t;

typedef struct bone
{
    char *name;
    float offset[3];
    int children;
    int *child;
} bone_t;

class Mesh {
public:
    int has_bones;
    int vertices;
    float *vertex;
    float *normal;
    float *tex_coord;
    int *bone_w;
    int groups;
    group_t *group;
    bone_t *bone;
    unsigned int list;
};

class Model: public Resource {
public:
    int load(const char *mesh); //, const char *texture);
	Mesh *loadDCM(const char *filename);
    void render();
	//void loadFromSurface(SDL_Surface *surface, SDL_Rect *area, int alpha, int clamp);
	//void render(float xpos, float ypos, float zpos, float width, float height);
private:
    glm::vec3 _position, _rotation;
    Texture *_texture;
    Mesh *_mesh;
};

#endif
