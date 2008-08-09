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

#include <iostream>

#include "model.h"
#include "box.h"
#include "light.h"
#include "chess_board.h"

#include "title.h"

ingame::ingame()
{
    scr = new screen(640,480);

    input.add( "QUIT", "INPUT_EVENT", (new keyboard_event(SDLK_ESCAPE, TRUE)) );

    input.add( "UP", "INPUT_EVENT", (new keyboard_event(SDLK_UP, FALSE)) );
    input.add( "DOWN", "INPUT_EVENT", (new keyboard_event(SDLK_DOWN, FALSE)) );
    input.add( "LEFT", "INPUT_EVENT", (new keyboard_event(SDLK_LEFT, FALSE)) );
    input.add( "RIGHT", "INPUT_EVENT", (new keyboard_event(SDLK_RIGHT, FALSE)) );
    input.add( "ZOOMIN", "INPUT_EVENT", (new keyboard_event(SDLK_o, FALSE)) );
    input.add( "ZOOMOUT", "INPUT_EVENT", (new keyboard_event(SDLK_p, FALSE)) );

    input.add( "ROTX", "INPUT_EVENT", (new keyboard_event(SDLK_q, FALSE)) );
    input.add( "ROTY", "INPUT_EVENT", (new keyboard_event(SDLK_a, FALSE)) );
    input.add( "ROTZ", "INPUT_EVENT", (new keyboard_event(SDLK_z, FALSE)) );
    input.add( "ROTXN", "INPUT_EVENT", (new keyboard_event(SDLK_w, FALSE)) );
    input.add( "ROTYN", "INPUT_EVENT", (new keyboard_event(SDLK_s, FALSE)) );
    input.add( "ROTZN", "INPUT_EVENT", (new keyboard_event(SDLK_x, FALSE)) );

    input.add( "CAMSHAKE", "INPUT_EVENT", (new keyboard_event(SDLK_k, TRUE)) );
    input.add( "INFO", "INPUT_EVENT", (new keyboard_event(SDLK_i, TRUE)) );

    // Board... 
    scn.add( "BOARD","BOARD",(new chess_board("/usr/local/share/dreamchess/boards/classic/board.dcm",
        "/usr/local/share/dreamchess/boards/classic/board.png" )) );

    /*entity *e;
    // Various pieces...
    e = new model("/usr/local/share/dreamchess/pieces/classic/queen.dcm",
        "/usr/local/share/dreamchess/pieces/classic/white.png" );
    e->xpos=-0.5f; e->ypos=2.5f;
    scn.add("WHITE_QUEEN","PIECE",e); // White queen

    e = new model("/usr/local/share/dreamchess/pieces/classic/bishop.dcm",
        "/usr/local/share/dreamchess/pieces/classic/white.png" );
    e->xpos=-0.5f; e->ypos=1.5f; e->zrot=90.0f;
    scn.add("WHITE_BISHOP","PIECE",e); // White bishop

    e = new model("/usr/local/share/dreamchess/pieces/classic/rook.dcm",
        "/usr/local/share/dreamchess/pieces/classic/white.png" );
    e->xpos=3.5f; e->ypos=-0.5f;
    scn.add("WHITE_ROOK","PIECE",e); // White rook

    e = new model("/usr/local/share/dreamchess/pieces/classic/king.dcm",
        "/usr/local/share/dreamchess/pieces/classic/black.png" );
    e->xpos=2.5f; e->ypos=3.5f; e->zpos=0.35;
    e->xrot=96.0f; e->yrot=20.0f; e->zrot=23.0f;
    scn.add("BLACK_KING","PIECE",e); // Black king*/

    //Position Camera...
    title_camera *c = new title_camera();

    c->xpos=5.518997f; c->ypos=-0.860000f; c->zpos=1.099000f;
    c->xrot=-93.0f; c->yrot=-1.0f; c->zrot=-59.285999f;
    //c->target=scn.entities[2];
    scn.add("CAMERA","CAMERA",c); // Camera

    scn.active_cam=c; // Set the camera.

    e = new light();
    e->xpos=5.919f; e->ypos=-1.160f; e->zpos=1.299f;
    e->xrot=-90.0f; e->yrot=0.0f; e->zrot=-52.286f;
    scn.add("LIGHT","LIGHT",e); // Light
}

title_screen::~title_screen()
{
    printf( "title_screen::~title_screen:game\n");
}

void title_screen::loop()
{
    static int last_update_tick;

    input.update();

    // Printf update and frame info.
    if ( SDL_GetTicks()-last_update_tick > 1000 )
    {
        printf( "Updates per Second: %i\n", ups );
        last_update_tick=SDL_GetTicks();
    }

    if ( input.get_input("INFO") )
        printf( "Camera: pos(%f,%f,%f), rot(%f,%f,%f), frametime:%f\n", 
            scn.active_cam->xpos, scn.active_cam->ypos, scn.active_cam->zpos, 
            scn.active_cam->xrot, scn.active_cam->yrot, scn.active_cam->zrot );
    if ( input.get_input("CAMSHAKE") )
    {
        title_camera *cam=(title_camera*)scn.active_cam;

        if ( cam->shake == TRUE )
            cam->shake=FALSE;
        else
            cam->shake=TRUE;
    }

    if ( input.get_input("UP") )
        scn.active_cam->ypos+=0.1;
    if ( input.get_input("DOWN") )
        scn.active_cam->ypos-=0.1;
    if ( input.get_input("LEFT") )
        scn.active_cam->xpos+=0.1;
    if ( input.get_input("RIGHT") )
        scn.active_cam->xpos-=0.1;
    if ( input.get_input("ZOOMIN") )
        scn.active_cam->zpos-=0.1;
    if ( input.get_input("ZOOMOUT") )
        scn.active_cam->zpos+=0.1;

    if ( input.get_input("ROTX") )
        scn.active_cam->xrot+=1.0;
    if ( input.get_input("ROTXN") )
        scn.active_cam->xrot-=1.0;

    if ( input.get_input("ROTY") )
        scn.active_cam->yrot+=1.0;
    if ( input.get_input("ROTYN") )
        scn.active_cam->yrot-=1.0;

    if ( input.get_input("ROTZ") )
        scn.active_cam->zrot+=1.0;
    if ( input.get_input("ROTZN") )
        scn.active_cam->zrot-=1.0;
}

