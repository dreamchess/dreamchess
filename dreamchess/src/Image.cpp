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
#include <SDL_Image.h>
#include <GL/glew.h>
#include "Texture.h"
#include "Image.h"
#include "Scene.h"
#include "DreamChess.h"

#include <stdio.h>

Image::Image(Scene *s, std::string filename) {
    _scene = s;
    _texture = static_cast<Texture*>(_scene->getGame()->getResourcePool()->getResource(filename));
    _size = glm::vec3(_texture->getSize().x, _texture->getSize().y, 1);
}

void Image::render() { 
    float u1 = 0.0f;
    float v1 = 0.0f;
    float u2 = _texture->getUV().x;
    float v2 = _texture->getUV().y;

    glEnable( GL_TEXTURE_2D );

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, _texture->getID());

    glBegin(GL_QUADS);
    glTexCoord2f(u1, v1);
    glVertex3f( _position.x, _position.y + _size.y, _position.z );
    glTexCoord2f(u2, v1);
    glVertex3f( _position.x + _size.x,  _position.y + _size.y, _position.z );
    glTexCoord2f(u2, v2);
    glVertex3f( _position.x + _size.x,  _position.y, _position.z );
    glTexCoord2f(u1, v2);
    glVertex3f( _position.x, _position.y, _position.z );
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
