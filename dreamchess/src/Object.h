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

#ifndef DREAMCHESS_OBJECT_H
#define DREAMCHESS_OBJECT_H

#include "glm/glm.hpp"

class Scene;
class Game;
class Group;

class Object {
public:
    Object() { }
    Object(Game *g, Group *p);
    void setParent(Group *g) {_parent = g;}

    virtual void update() { }
	virtual void render() { }

    glm::vec3 getPosition() {return _position;}
    void setPosition(float x, float y, float z = 0) {_position = glm::vec3(x, y, z);}
    void setPosition(glm::vec3 p) {_position = p;}
    glm::vec3 getRotation() {return _rotation;}
    void setRotation(float x, float y, float z = 0) {_rotation = glm::vec3(x, y, z);}
    void setRotation(glm::vec3 r) {_rotation = r;}    
    glm::vec3 getScale() {return _scale;}
    void setScale(float x, float y, float z = 0) {_scale = glm::vec3(x, y, z);}
    void setScale(glm::vec3 s) {_scale = s;}       
    glm::vec3 getSize() {return _size;}
    void setSize(float x, float y, float z = 0) {_size = glm::vec3(x, y, z);}
    void setSize(glm::vec3 s) {_size = s;}       

protected:
    Game *_game;
    Group *_parent;

    glm::mat4 _matrix;
    glm::vec3 _position, _rotation, _size, _scale;   
};

#endif
