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
#include "Model.h"
#include "System.h"
#include "Scene.h"
#include "DreamChess.h"
#include "ResourcePool.h"
#include "Mesh.h"
#include "Texture.h"

#include <stdio.h>
#include <string>

Model::Model(Scene *s, std::string mesh, std::string texture) {
    _scene = s;
    _mesh = static_cast<Mesh*>(_scene->getGame()->getResourcePool()->getResource(mesh));
    _texture = static_cast<Texture*>(_scene->getGame()->getResourcePool()->getResource(texture));    
    _size = glm::vec3(1, 1, 1);
}

void Model::render() {
    float specReflection[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float nospecReflection[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float mcolor[4] = { 1.0f, 1.0f, 1.0f };
    float alpha = 1.0f;
    int specular = 0;

    mcolor[3] = alpha;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mcolor);

    if (specular)
        glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
    else
        glMaterialfv(GL_FRONT, GL_SPECULAR, nospecReflection);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _texture->getID());

    glPushMatrix();
    glTranslatef(_position.x, _position.y, _position.z);
    glRotatef(_rotation.x, 1, 0, 0);
    glRotatef(_rotation.y, 0, 1, 0);
    glRotatef(_rotation.z, 0, 0, 1); 

    glCallList(_mesh->list);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}