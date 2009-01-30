
#include "SDL_opengl.h"
#include "camera.h"

camera::camera(std::string name2)
{
    name=name2;
    type="camera";
    target=NULL;
}

void camera::generate_bbox()
{
	
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
   // printf( "Uh oh.\n" );
}
