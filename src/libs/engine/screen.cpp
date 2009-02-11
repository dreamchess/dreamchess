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

#include "screen.h"

int ticks_omg=0;
int frames_yay=0;

vec get_mouse()
{
    int x, y;
    SDL_PumpEvents();
    SDL_GetMouseState(&x, &y);

    return vec(x,y,0);
}

void screen::push()
{
	glPushMatrix();
}

void screen::pop()
{
	glPopMatrix();
}

void screen::move( float x, float y, float z )
{
	glTranslatef( x, y, z );
}

void screen::rotate( float x, float y, float z )
{
	glRotatef(x, 1, 0, 0);
    glRotatef(y, 0, 1, 0);
    glRotatef(z, 0, 0, 1);
}

void screen::scale( float x, float y, float z )
{
	glScalef(x, y, z);
}

void screen::start_frame()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();

    // Lights...
    //int light_count=count_type("light");

    // FIX MEEEeee...
    /*if ( light_count >= 0 )
    {
        float mcolor[] = { 1.0f, 1.0f, 1.0f };
        float position[] = { 5.919f, -1.160f, 1.299f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, position );  

	    // Create light components 
	    GLfloat ambientLight[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	    GLfloat diffuseLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      	 
	    // Assign created components to GL_LIGHT0 
	    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

        // White specular highlights 
        glLightModelf(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mcolor);

        glEnable(GL_LIGHT0);  
    }	*/
}

void screen::end_frame()
{
    if ( SDL_GetTicks()-ticks_omg > 1000 )
    {
        //printf( "FPS: %i\n", frames_yay );
        frames_yay=0;
        ticks_omg=SDL_GetTicks();
    }

    frames_yay++;
    SDL_GL_SwapBuffers();
}

screen::screen(int w, int h) 
{
    int video_flags;
    const SDL_VideoInfo *video_info;
    GLfloat ratio;

    width = w; height = h;

   if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
	{
	    fprintf( stderr, "Video initialization failed: %s\n",
		     SDL_GetError( ) );
	    exit(1);
	} 

    // Fetch the video info 
    video_info = SDL_GetVideoInfo( );

    if ( !video_info )
	{
	    fprintf( stderr, "Video query failed: %s\n", SDL_GetError( ) );
	    exit(1);
	}

    // the flags to pass to SDL_SetVideoMode 
    video_flags  = SDL_OPENGL;          
    video_flags |= SDL_GL_DOUBLEBUFFER; 
    video_flags |= SDL_HWPALETTE;       
    video_flags |= SDL_RESIZABLE;       

    // This checks to see if surfaces can be stored in memory 
    if ( video_info->hw_available )
	    video_flags |= SDL_HWSURFACE;
    else
	    video_flags |= SDL_SWSURFACE;

    // This checks if hardware blits can be done 
    if ( video_info->blit_hw )
	video_flags |= SDL_HWACCEL;

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    surface = SDL_SetVideoMode( width, height, 24, video_flags );

    // Verify there is a surface
    if ( !surface )
	{
	    fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
	    exit(1);
	}

    // initialize OpenGL
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    // resize the initial window 
    // Protect against a divide by zero 
    if ( height == 0 )
    	height = 1;
    ratio = ( GLfloat )width / ( GLfloat )height;

    // Setup our viewport.
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    // change to the projection matrix and set our viewing volume. 
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    // Set our perspective 
    gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

    // Make sure we're chaning the model view and not the projection
    glMatrixMode( GL_MODELVIEW );

    // Reset The View
    glLoadIdentity( );
}
