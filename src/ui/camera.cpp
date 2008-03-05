
#include <GL/gl.h>
#include <GL/glu.h>
#include "camera.h"

camera::camera()
{
    type="camera";
    target=NULL;
}

void camera::render()
{
    if ( target )
    {
        gluLookAt(xpos,ypos,zpos,
            target->xpos,target->ypos,target->zpos,0,0,1);
    }
    else
    {
        glRotatef( xrot, 1.0f, 0.0f, 0.0f );
        glRotatef( yrot, 0.0f, 1.0f, 0.0f );
        glRotatef( zrot, 0.0f, 0.0f, 1.0f );
        glTranslatef( -xpos, -ypos, -zpos );
    }
}

void camera::update()
{
    printf( "Uh oh.\n" );
}