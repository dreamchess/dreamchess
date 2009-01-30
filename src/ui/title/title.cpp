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
#include <math.h>

#include "model.h"
#include "mesh.h"
#include "box.h"
#include "light.h"
#include "chess_board.h"
#include "screen.h"

#include "title.h"

/* constant used for converting to radians */
const float piover180 = 0.0174532925f;

title_screen::title_screen()
{
    scr = new screen(640,480);

    input.add( (new keyboard_event("QUIT", "INPUT_EVENT", SDLK_ESCAPE, TRUE)) );

    input.add( (new keyboard_event("UP", "INPUT_EVENT", SDLK_UP, FALSE)) );
    input.add( (new keyboard_event("DOWN", "INPUT_EVENT", SDLK_DOWN, FALSE)) );
    input.add( (new keyboard_event("LEFT", "INPUT_EVENT", SDLK_LEFT, FALSE)) );
    input.add( (new keyboard_event("RIGHT", "INPUT_EVENT", SDLK_RIGHT, FALSE)) );
    input.add( (new keyboard_event("ZOOMIN", "INPUT_EVENT", SDLK_o, FALSE)) );
    input.add( (new keyboard_event("ZOOMOUT", "INPUT_EVENT", SDLK_p, FALSE)) );

    input.add( (new keyboard_event("ROTX", "INPUT_EVENT", SDLK_q, FALSE)) );
    input.add( (new keyboard_event("ROTY", "INPUT_EVENT", SDLK_a, FALSE)) );
    input.add( (new keyboard_event("ROTZ", "INPUT_EVENT", SDLK_z, FALSE)) );
    input.add( (new keyboard_event("ROTXN", "INPUT_EVENT", SDLK_w, FALSE)) );
    input.add( (new keyboard_event("ROTYN", "INPUT_EVENT", SDLK_s, FALSE)) );
    input.add( (new keyboard_event("ROTZN", "INPUT_EVENT", SDLK_x, FALSE)) );

    input.add( (new keyboard_event("CAMSHAKE", "INPUT_EVENT", SDLK_k, TRUE)) );
    input.add( (new keyboard_event("INFO", "INPUT_EVENT", SDLK_i, TRUE)) );

    // mouse.
    input.add( (new mouse_event("LEFTMOUSE", "INPUT_EVENT", 1, TRUE)) );
    input.add( (new mouse_event("MIDDLEMOUSE", "INPUT_EVENT", 2, TRUE)) );
    input.add( (new mouse_event("RIGHTMOUSE", "INPUT_EVENT", 3, TRUE)) );
    
    input.list();

    // Textures.
    add( (new texture("BOARD_TEX", "TEXTURE", "/usr/local/share/dreamchess/boards/classic/board.png")) );
    add( (new texture("WHITE_TEX", "TEXTURE", "/usr/local/share/dreamchess/pieces/classic/white.png")) );
    add( (new texture("BLACK_TEX", "TEXTURE", "/usr/local/share/dreamchess/pieces/classic/black.png")) );

    // Meshes.
    add( (new mesh("BOARD_MESH", "MESH", "/usr/local/share/dreamchess/boards/classic/board.dcm")) );
    add( (new mesh("QUEEN_MESH", "MESH", "/usr/local/share/dreamchess/pieces/classic/queen.dcm")) );
    add( (new mesh("BISHOP_MESH", "MESH", "/usr/local/share/dreamchess/pieces/classic/bishop.dcm")) );
    add( (new mesh("ROOK_MESH", "MESH", "/usr/local/share/dreamchess/pieces/classic/rook.dcm")) );
    add( (new mesh("KING_MESH", "MESH", "/usr/local/share/dreamchess/pieces/classic/king.dcm")) );

    // Entities.
    entity *e;
    add( (new chess_board("BOARD","ENTITY", "BOARD_MESH","BOARD_TEX",this)) );

    e = new model("WHITE_QUEEN","QUEEN_MESH","WHITE_TEX",this);
    e->type="ENTITY"; e->xpos=-0.5f; e->ypos=2.5f;
    add(e); // White queen

    e = new model("WHITE_BISHOP","BISHOP_MESH", "WHITE_TEX",this);
    e->type="ENTITY"; e->xpos=-0.5f; e->ypos=1.5f; e->zrot=90.0f;
    add(e); // White bishop

    e = new model("WHITE_ROOK","ROOK_MESH", "WHITE_TEX",this);
    e->type="ENTITY"; e->xpos=3.5f; e->ypos=-0.5f;
    add(e); // White rook

    e = new model("BLACK_KING","KING_MESH","BLACK_TEX",this);
    e->type="ENTITY"; 
    e->xpos=2.5f; e->ypos=3.5f; e->zpos=0.35;
    e->xrot=96.0f; e->yrot=20.0f; e->zrot=23.0f;
    add(e); // Black king

    //Position Camera...
    title_camera *c = new title_camera("CAMERA");
	c->type="CAMERA";
    //c->xpos=5.518997f; c->ypos=-0.860000f; c->zpos=1.099000f;
    //c->xrot=-93.0f; c->yrot=-1.0f; c->zrot=-59.285999f;
    //c->target=scn.entities[2];
    c->xpos=0.0f; c->ypos=0.0f; c->zpos=10.0f;
    c->xrot=0.0f; c->yrot=0.0f; c->zrot=0.0f;
    add(c); // Camera

    active_cam=c; // Set the camera.

    /*e = new light("LIGHT");
    e->xpos=5.919f; e->ypos=-1.160f; e->zpos=1.299f;
    e->xrot=-90.0f; e->yrot=0.0f; e->zrot=-52.286f;
    add("LIGHT","ENTITY",e); // Light*/

    list();
}

void title_screen::loop()
{
    if ( input.get_input("INFO") )
        printf( "Camera: pos(%f,%f,%f), rot(%f,%f,%f), frametime:%f\n", 
            active_cam->xpos, active_cam->ypos, active_cam->zpos, 
            active_cam->xrot, active_cam->yrot, active_cam->zrot );
    
    if ( input.get_input("LEFTMOUSE") )
    {
        for ( int i=0; i<resources.size(); i++ )
        {
            if ( resources[i]->type == "ENTITY" )
            if ( ((entity*)resources[i])->collision_at(get_mouse_3d()) == TRUE )            
                {
                    printf( "Hmm!\n" );
                    ((entity*)resources[i])->sc->run("entity_clicked");
                }
        }        
        //vec v=GetOGLPos(get_mouse());
        //printf( "Vector: %f, %f, %f\n", v.x, v.y, v.z );
    }    

  /*  if ( input.get_input("LEFTMOUSE") )
    {
        vec v=GetOGLPos(get_mouse());
        //printf( "Vector: %f, %f, %f\n", v.x, v.y, v.z );
    }  

    if ( input.get_input("LEFTMOUSE") )
    {
        vec v=GetOGLPos(get_mouse());
        //printf( "Vector: %f, %f, %f\n", v.x, v.y, v.z );
    }  */

	//((entity*)resources[11])->xpos=get_mouse_3d().x;
	//((entity*)resources[11])->ypos=get_mouse_3d().y;
	//((entity*)resources[11])->zpos=0;

    if ( input.get_input("CAMSHAKE") )
    {
        title_camera *cam=(title_camera*)active_cam;

        if ( cam->shake == TRUE )
            cam->shake=FALSE;
        else
            cam->shake=TRUE;
    }

    if ( input.get_input("UP") )
    {
    	active_cam->xpos += (float)sin(active_cam->zrot*piover180) * 0.1f;
    	active_cam->ypos += (float)cos(active_cam->zrot*piover180) * 0.1f;
    }
    if ( input.get_input("DOWN") )
    {
    	active_cam->xpos -= (float)sin(active_cam->zrot*piover180) * 0.1f;
    	active_cam->ypos -= (float)cos(active_cam->zrot*piover180) * 0.1f;
    }

    if ( input.get_input("LEFT") )
        active_cam->zrot-=1.0;
    if ( input.get_input("RIGHT") )
        active_cam->zrot+=1.0;


    if ( input.get_input("ZOOMIN") )
        active_cam->zpos-=0.1;
    if ( input.get_input("ZOOMOUT") )
        active_cam->zpos+=0.1;

    if ( input.get_input("ROTX") )
        active_cam->xrot+=1.0;
    if ( input.get_input("ROTXN") )
        active_cam->xrot-=1.0;

    if ( input.get_input("ROTY") )
        active_cam->yrot+=1.0;
    if ( input.get_input("ROTYN") )
        active_cam->yrot-=1.0;

    if ( input.get_input("ROTZ") )
        active_cam->zrot+=1.0;
    if ( input.get_input("ROTZN") )
        active_cam->zrot-=1.0;

}

