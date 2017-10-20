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

#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include "Texture.h"
#include "Image.h"
#include "Scene.h"
#include "DreamChess.h"
#include "Mesh.h"

#include <stdio.h>

Image::Image(Scene *s, std::string filename) {
    _scene = s;
    _texture = static_cast<Texture*>(_scene->getGame()->getResourcePool()->getResource(filename));
    _size = glm::vec3(_texture->getSize().x, _texture->getSize().y, 1);
    setClipRegion(0, 0, _size.x, _size.y);
}

void Image::setClipRegion(int x, int y, int width, int height) {
    _clipRegion = _texture->getUVForRegion(x, y, width, height);
}

void Image::render() { 	
    glEnable( GL_TEXTURE_2D );

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, _texture->getID());

    glPushMatrix();
    glTranslatef(_position.x, _position.y, _position.z);
    glRotatef(_rotation.x, 1, 0, 0);
    glRotatef(_rotation.y, 0, 1, 0);
    glRotatef(_rotation.z, 0, 0, 1); 

    glBegin(GL_QUADS);
    glTexCoord2f(_clipRegion[0], _clipRegion[1]);
    glVertex3f(-(_size.x / 2), (_size.y / 2), 0);
    glTexCoord2f(_clipRegion[2], _clipRegion[1]);
    glVertex3f((_size.x / 2), (_size.y / 2), 0);
    glTexCoord2f(_clipRegion[2], _clipRegion[3]);
    glVertex3f((_size.x / 2), -(_size.y / 2), 0);
    glTexCoord2f(_clipRegion[0], _clipRegion[3]);
    glVertex3f(-(_size.x / 2), -(_size.y / 2), 0);
    glEnd();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}
