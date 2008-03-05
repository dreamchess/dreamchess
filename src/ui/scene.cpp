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

    // Lights...
    int light_count=count_type("light");

    // FIX MEEEeee...
    if ( light_count >= 0 )
    {
        float mcolor[] = { 1.0f, 1.0f, 1.0f };
        float position[] = { 5.919f, -1.160f, 1.299f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, position );  

	    /* Create light components */
	    GLfloat ambientLight[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	    GLfloat diffuseLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      	 
	    /* Assign created components to GL_LIGHT0 */
	    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

        /* White specular highlights */
        glLightModelf(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mcolor);

        glEnable(GL_LIGHT0);  
    }

    // Camera! 
    active_cam->render();

    /* Step through the list and render them alls! */
    for ( int i=0; i<entities.size();i++ )
        entities[i]->render();            
}

void scene::list()
{
    for ( int i=0; i<entities.size();i++ )
    {
        std::cout << entities[i]->name << std::endl;
    }
}

int scene::find_type(std::string type, int index)
{
    int count=0;
    int retval=-1;

    if ( count_type(type) < index )
        return -1;

    for ( int i=0; i<entities.size();i++ )
    {
        if ( entities[i]->type == type )
        {
            if ( count == index )
                return i;
            else
                count++;
        }
    }

    return retval;
}

int scene::count_type(std::string type)
{
    int count=0;

    for ( int i=0; i<entities.size();i++ )
    {
        if ( entities[i]->type == type )
            count++;
    }

    return count;
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
