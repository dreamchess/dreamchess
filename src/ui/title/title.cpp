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
#include "screen.h"
#include "scene.h"
#include "input.h"
#include "model.h"
#include "box.h"
#include "camera.h"
#include "light.h"
#include "chess_board.h"

#include "title.h"

title_screen::title_screen()
{
    std::cout << "Starting title screen." << std::endl;
}

title_screen::~title_screen()
{
    std::cout << "Cleaning up title screen." << std::endl;
}

void title_screen::loop()
{
    screen scr(640,480);
    entity *e;

    input_layer input;
    input.add( (new keyboard_event("QUIT", SDLK_ESCAPE, TRUE)) );

    input.add( (new keyboard_event("UP", SDLK_UP, FALSE)) );
    input.add( (new keyboard_event("DOWN", SDLK_DOWN, FALSE)) );
    input.add( (new keyboard_event("LEFT", SDLK_LEFT, FALSE)) );
    input.add( (new keyboard_event("RIGHT", SDLK_RIGHT, FALSE)) );
    input.add( (new keyboard_event("ZOOMIN", SDLK_o, FALSE)) );
    input.add( (new keyboard_event("ZOOMOUT", SDLK_p, FALSE)) );

    input.add( (new keyboard_event("ROTX", SDLK_q, FALSE)) );
    input.add( (new keyboard_event("ROTY", SDLK_a, FALSE)) );
    input.add( (new keyboard_event("ROTZ", SDLK_z, FALSE)) );
    input.add( (new keyboard_event("ROTXN", SDLK_w, FALSE)) );
    input.add( (new keyboard_event("ROTYN", SDLK_s, FALSE)) );
    input.add( (new keyboard_event("ROTZN", SDLK_x, FALSE)) );

    input.add( (new keyboard_event("INFO", SDLK_i, TRUE)) );

    scene scn;

    // Board... 
    scn.add( (new chess_board("/usr/local/share/dreamchess/boards/classic/board.dcm",
        "/usr/local/share/dreamchess/boards/classic/board.png" )) );

    // Various pieces...
    e = new model("/usr/local/share/dreamchess/pieces/classic/queen.dcm",
        "/usr/local/share/dreamchess/pieces/classic/white.png" );
    e->xpos=-0.5f; e->ypos=2.5f;
    scn.add(e); // White queen

    e = new model("/usr/local/share/dreamchess/pieces/classic/bishop.dcm",
        "/usr/local/share/dreamchess/pieces/classic/white.png" );
    e->xpos=-0.5f; e->ypos=1.5f; e->zrot=90.0f;
    scn.add(e); // White bishop

    e = new model("/usr/local/share/dreamchess/pieces/classic/rook.dcm",
        "/usr/local/share/dreamchess/pieces/classic/white.png" );
    e->xpos=3.5f; e->ypos=-0.5f;
    scn.add(e); // White rook

    e = new model("/usr/local/share/dreamchess/pieces/classic/king.dcm",
        "/usr/local/share/dreamchess/pieces/classic/black.png" );
    e->xpos=2.5f; e->ypos=3.5f; e->zpos=0.35;
    e->xrot=96.0f; e->yrot=20.0f; e->zrot=23.0f;
    scn.add(e); // Black king

    //Position Camera...
    camera *c = new title_camera();
    c->xpos=1.919f; c->ypos=-1.160f; c->zpos=1.299f;
    c->xrot=-90.0f; c->yrot=0.0f; c->zrot=-52.286f;
    c->target=scn.entities[2];
    scn.add(c); // Camera

    scn.active_cam=c; // Set the camera.

    e = new light();
    e->xpos=5.919f; e->ypos=-1.160f; e->zpos=1.299f;
    e->xrot=-90.0f; e->yrot=0.0f; e->zrot=-52.286f;
    scn.add(e); // Light

    while ( !input.get_input("QUIT") )
    {
        input.update();

        if ( input.get_input("INFO") )
            printf( "Camera: pos(%f,%f,%f), rot(%f,%f,%f)\n", 
                scn.active_cam->xpos, scn.active_cam->ypos, scn.active_cam->zpos, 
                scn.active_cam->xrot, scn.active_cam->yrot, scn.active_cam->zrot );

        if ( input.get_input("UP") )
            scn.active_cam->ypos+=0.01;
        if ( input.get_input("DOWN") )
            scn.active_cam->ypos-=0.01;
        if ( input.get_input("LEFT") )
            scn.active_cam->xpos+=0.01;
        if ( input.get_input("RIGHT") )
            scn.active_cam->xpos-=0.01;
        if ( input.get_input("ZOOMIN") )
            scn.active_cam->zpos-=0.01;
        if ( input.get_input("ZOOMOUT") )
            scn.active_cam->zpos+=0.01;

        if ( input.get_input("ROTX") )
            scn.active_cam->xrot+=0.01;
        if ( input.get_input("ROTXN") )
            scn.active_cam->xrot-=0.01;

        if ( input.get_input("ROTY") )
            scn.active_cam->yrot+=0.01;
        if ( input.get_input("ROTYN") )
            scn.active_cam->yrot-=0.01;

        if ( input.get_input("ROTZ") )
            scn.active_cam->zrot+=0.01;
        if ( input.get_input("ROTZN") )
            scn.active_cam->zrot-=0.01;

        scn.render();
        scn.update();
        scr.update();
    }
}

