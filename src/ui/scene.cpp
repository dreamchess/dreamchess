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

#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL/SDL.h"
#include <iostream>

#include "scene.h"

void scene::update()
{
    for ( int i=0; i<entities.size();i++ )
    {
        entities[i]->update();
    }             
}

void scene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();

    glTranslatef( -cam.xpos, -cam.ypos, -cam.zpos );

    /* Step through the list and render them alls! */
    for ( int i=0; i<entities.size();i++ )
    {
        entities[i]->render();
    }          
}

void scene::list()
{
    for ( int i=0; i<entities.size();i++ )
    {
        std::cout << entities[i]->name << std::endl;
    }
}

void scene::add( entity *ent )
{
    entities.push_back(ent);
}

scene::~scene()
{
    clear();
}

void scene::clear()
{
    for ( int i=0; i<entities.size();i++ )
    {
        delete entities[i];
    }

    entities.clear();
}
