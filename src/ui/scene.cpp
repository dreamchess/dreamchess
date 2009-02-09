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

#include "SDL_opengl.h"
#include "SDL.h"
#include <iostream>

#include "scene.h"

void scene::update()
{
    for ( int i=0; i<resources.size();i++ )
    {
        if ( resources[i]->type == "ENTITY" )
            ((entity*)resources[i])->update();
    }  
}

void scene::start()
{
    static double lastFrameTime = 0.0;
    static double cyclesLeftOver = 0.0;
    double currentTime;
    double updateIterations;

    int updates_per_sec;
    int last_update_tick;

    updates_per_sec=0;
    last_update_tick=SDL_GetTicks();
    ups=0;

    while ( !input.get_input("QUIT") )
    {
        currentTime = SDL_GetTicks();
        updateIterations = ((currentTime - lastFrameTime) + cyclesLeftOver);
  
        if (updateIterations > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL))
            updateIterations = (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL);

        while (updateIterations > UPDATE_INTERVAL) 
        {
            static int last_update_tick;

            updateIterations -= UPDATE_INTERVAL;
            input.update();  

            // Printf update and frame info.
            if ( SDL_GetTicks()-last_update_tick > 1000 )
            {
                //printf( "Updates per Second: %i\n", ups );
                last_update_tick=SDL_GetTicks();
            }
            loop();

            update();
            updates_per_sec++;
        }

        // Count updates per second.
        if ( SDL_GetTicks()-last_update_tick > 1000 )
        {
            ups=updates_per_sec;
            updates_per_sec=0; 
            last_update_tick=SDL_GetTicks();
        }

        cyclesLeftOver = updateIterations;
        lastFrameTime = currentTime;

        //render();
        //scr->update();
    }
}

vec mouse_3d;

vec get_mouse_3d()
{
	return mouse_3d;
}

void _get_mouse_3d()
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	vec pos=get_mouse();

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)pos.x;
	winY = (float)viewport[3] - (float)pos.y;
	glReadPixels( pos.x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    //printf( "%f,%f\n", posX, posY);

	mouse_3d.x=posX;
	mouse_3d.y=posY;
	mouse_3d.z=posZ;

	//return vec(posX, posY, posZ);
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
    for ( int i=0; i<resources.size();i++ )
    {
        if ( resources[i]->type == "ENTITY" && ((entity*)resources[i])->post_mouse_render == false )
            ((entity*)resources[i])->render();            
    }
    
    _get_mouse_3d();
    
    /* Render any post mouses... */
    for ( int i=0; i<resources.size();i++ )
    {
        if ( resources[i]->type == "ENTITY" && ((entity*)resources[i])->post_mouse_render == true )
            ((entity*)resources[i])->render();            
    }
}

int scene::find_type(std::string type, int index)
{
    int count=0;
    int retval=-1;

    if ( count_type(type) < index )
        return -1;

    for ( int i=0; i<resources.size();i++ )
    {
        if ( resources[i]->type == type )
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

    for ( int i=0; i<resources.size();i++ )
    {
        if ( resources[i]->type == type )
            count++;
    }

    return count;
}

