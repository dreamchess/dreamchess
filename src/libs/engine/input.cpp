
#include "SDL_opengl.h"
#include "SDL.h"
#include "input.h"

keyboard::keyboard()
{
    for (int i=0; i<256; i++)
        wait_for_release[i]=false;
}

bool keyboard::is_pressed( int key, bool one_time )
{
    SDL_PumpEvents();
    Uint8 *keystate = SDL_GetKeyState(NULL);

    bool active=keystate[key];

    // Wait for release is active for the key.
    if ( wait_for_release[key] )
    {        
        if ( !active ) // If the key is not pressed, reset wait for release.
            wait_for_release[key]=false;
        else // If it is pressed, force a 'false' return, even if it's pressed.
            active=false;
    }
    else // Wait for release not set
    {
        if ( one_time && active) // If 'one time' set and the key is active, set wait for release.
            wait_for_release[key]=true;
    }

    // Return the key's active state -- or false, if it's been forced by wait_for_release.
    return active;	
}

mouse::mouse()
{
    for (int i=0; i<256; i++)
        wait_for_release[i]=false;
}

bool mouse::is_waiting( int button )
{
    return wait_for_release[button];
}

bool mouse::is_pressed(int button)
{
    bool active=mouse_buttons[button];

    if (!active)
        wait_for_release[button]=false;

    if (active)
        wait_for_release[button]=true;

    return active;
}

void mouse::update_mouse()
{
    int x, y;
    SDL_PumpEvents();
    SDL_GetMouseState(&x, &y);
    mouse_2d.x=x; mouse_2d.y=y;

    mouse_buttons[1]=SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1);

    if ( !mouse_buttons[1] )
        wait_for_release[1]=false;    
}

vec mouse::position2d()
{
    return mouse_2d;
}

void mouse::update_mouse3d()
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	vec pos=mouse_2d;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)pos.x;
	winY = (float)viewport[3] - (float)pos.y;
	glReadPixels( pos.x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	mouse_3d.x=posX;
	mouse_3d.y=posY;
	mouse_3d.z=posZ;
}

vec mouse::position()
{
    return vec(mouse_3d.x,mouse_3d.y,mouse_3d.z);
}







