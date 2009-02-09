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

void screen::pre_render()
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
    	
    currentTime = SDL_GetTicks();
    updateIterations = ((currentTime - lastFrameTime) + cyclesLeftOver);
  
    if (updateIterations > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL))
        updateIterations = (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL);

    while (updateIterations > UPDATE_INTERVAL) 
    {
        static int last_update_tick;

        updateIterations -= UPDATE_INTERVAL;

        // Printf update and frame info.
        if ( SDL_GetTicks()-last_update_tick > 1000 )
        {
            //printf( "Updates per Second: %i\n", ups );
            last_update_tick=SDL_GetTicks();
        }
        //loop();

        //update();
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
}

void screen::post_render()
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
