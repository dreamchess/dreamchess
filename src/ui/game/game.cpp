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
//#include "dir.h"

#include "game.h"

extern "C" int ch_datadir();

/* constant used for converting to radians */
const float piover180 = 0.0174532925f;

game::game()
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

	//ch_datadir();

    // Textures.
    add( (new texture("BOARD_TEX", "TEXTURE", "boards/classic/board.png")) );
    add( (new texture("WHITE_TEX", "TEXTURE", "pieces/classic/white.png")) );
    add( (new texture("BLACK_TEX", "TEXTURE", "pieces/classic/black.png")) );

    // Meshes.
    add( (new mesh("BOARD_MESH", "MESH", "boards/classic/board.dcm")) );
    add( (new mesh("KING_MESH", "MESH", "pieces/classic/king.dcm")) );
    add( (new mesh("QUEEN_MESH", "MESH", "pieces/classic/queen.dcm")) );
    add( (new mesh("BISHOP_MESH", "MESH", "pieces/classic/bishop.dcm")) );
    add( (new mesh("ROOK_MESH", "MESH", "pieces/classic/rook.dcm")) );
    add( (new mesh("KNIGHT_MESH", "MESH", "pieces/classic/knight.dcm")) );
    add( (new mesh("PAWN_MESH", "MESH", "pieces/classic/pawn.dcm")) );

    // Entities.
    entity *e;
    add( (new chess_board("BOARD","ENTITY","BOARD_MESH","BOARD_TEX",this)) );

    /* White pieces... */
   /* e = new model("KING_MESH","WHITE_TEX",this );
    e->xpos=0.5f; e->ypos=-3.5f;
    add("WHITE_KING","ENTITY",e); // White king

    e = new model("QUEEN_MESH","WHITE_TEX",this );
    e->xpos=-0.5f; e->ypos=-3.5f;
    add("WHITE_QUEEN","ENTITY",e); // White queen

    e = new model("BISHOP_MESH","WHITE_TEX",this );
    e->xpos=-1.5f; e->ypos=-3.5f;
    add("WHITE_BISHOP","ENTITY",e); // White bishop

    e = new model("BISHOP_MESH","WHITE_TEX",this );
    e->xpos=1.5f; e->ypos=-3.5f;
    add("WHITE_BISHOP","ENTITY",e); // White bishop

    e = new model("KNIGHT_MESH","WHITE_TEX",this );
    e->xpos=-2.5f; e->ypos=-3.5f;
    add("WHITE_KNIGHT","ENTITY",e); // White knight

    e = new model("KNIGHT_MESH","WHITE_TEX",this );
    e->xpos=2.5f; e->ypos=-3.5f;
    add("WHITE_KNIGHT","ENTITY",e); // White knight

    e = new model("ROOK_MESH","WHITE_TEX",this );
    e->xpos=-3.5f; e->ypos=-3.5f;
    add("WHITE_ROOK","ENTITY",e); // White rook

    e = new model("ROOK_MESH","WHITE_TEX",this );
    e->xpos=3.5f; e->ypos=-3.5f;
    add("WHITE_ROOK","ENTITY",e); // White rook

    for (float f=-3.5; f<4; f+=1.0f )
    {
        e = new model("PAWN_MESH","WHITE_TEX",this );
        e->xpos=f; e->ypos=-2.5f;
        add("WHITE_PAWN","ENTITY",e); // White pawn
    }*/


    /* Black pieces... */
   /* e = new model("BLACK_KING","KING_MESH","BLACK_TEX",this );
    e->xpos=0.5f; e->ypos=3.5f;
    add("BLACK_KING","ENTITY",e); // White king

    e = new model("QUEEN_MESH","BLACK_TEX",this );
    e->xpos=-0.5f; e->ypos=3.5f;
    add("BLACK_QUEEN","ENTITY",e); // White queen

    e = new model("BISHOP_MESH","BLACK_TEX",this );
    e->xpos=-1.5f; e->ypos=3.5f;
    add("BLACK_BISHOP","ENTITY",e); // White bishop

    e = new model("BISHOP_MESH","BLACK_TEX",this );
    e->xpos=1.5f; e->ypos=3.5f;
    add("BLACK_BISHOP","ENTITY",e); // White bishop

    e = new model("KNIGHT_MESH","BLACK_TEX",this );
    e->xpos=-2.5f; e->ypos=3.5f;
    add("BLACK_KNIGHT","ENTITY",e); // White knight

    e = new model("KNIGHT_MESH","BLACK_TEX",this );
    e->xpos=2.5f; e->ypos=3.5f;
    add("BLACK_KNIGHT","ENTITY",e); // White knight

    e = new model("ROOK_MESH","BLACK_TEX",this );
    e->xpos=-3.5f; e->ypos=3.5f;
    add("BLACK_ROOK","ENTITY",e); // White rook

    e = new model("ROOK_MESH","BLACK_TEX",this );
    e->xpos=3.5f; e->ypos=3.5f;
    add("BLACK_ROOK","ENTITY",e); // White rook

    for (float f=-3.5; f<4; f+=1.0f )
    {
        e = new model("PAWN_MESH","BLACK_TEX",this );
        e->xpos=f; e->ypos=2.5f;
        add("BLACK_PAWN","ENTITY",e); // White pawn
    }*/

    //Position Camera...
    camera *c = new camera("camera");
	c->type="CAMERA";
    c->xpos=0.0f; c->ypos=-7.0f; c->zpos=10.0f;
    c->xrot=-32.0f; c->yrot=0.0f; c->zrot=-0.0f;
    //c->target=scn.entities[2];
    add(c); // Camera

    active_cam=c; // Set the camera.

    e = new light("light");
    e->type="LIGHT";
    e->xpos=5.919f; e->ypos=-1.160f; e->zpos=1.299f;
    e->xrot=-90.0f; e->yrot=0.0f; e->zrot=-52.286f;
    add(e); // Light
}

void game::loop()
{
    if ( input.get_input("INFO") )
        printf( "Camera: pos(%f,%f,%f), rot(%f,%f,%f), frametime:%f\n", 
            active_cam->xpos, active_cam->ypos, active_cam->zpos, 
            active_cam->xrot, active_cam->yrot, active_cam->zrot );

    /*if ( input.get_input("CAMSHAKE") )
    {
        camera *cam=(camera*)active_cam;

        if ( cam->shake == TRUE )
            cam->shake=FALSE;
        else
            cam->shake=TRUE;
    }*/

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
        active_cam->zrot-=1.0;//
    if ( input.get_input("RIGHT") )
        active_cam->zrot+=1.0;//


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

}

