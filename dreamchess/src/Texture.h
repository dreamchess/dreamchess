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

#ifndef DREAMCHESS_TEXTURE_H
#define DREAMCHESS_TEXTURE_H

struct SDL_Surface;
struct SDL_Rect;

#include "glm/glm.hpp"
#include "ResourcePool.h"

class Texture: public Resource {
public:
	void load(const char *filename, int alpha, int clamp );
	void loadFromSurface(SDL_Surface *surface, SDL_Rect *area, int alpha, int clamp);
	void render(float xpos, float ypos, float zpos, float width, float height);

    unsigned int getID() {return _id;}
    glm::vec2 getSize() {return _size;}
    glm::vec2 getUV() {return _uv;}
    glm::vec4 getUVForRegion(int x, int y, int width, int height);
private:
    unsigned int _id;
    glm::vec2 _nativeSize;
    glm::vec2 _size;
    glm::vec2 _uv;
};

#endif
