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

#include "entity.h"

entity::entity()
{
    xpos=ypos=zpos=0.0f;
    xrot=yrot=zrot=0.0f;
    xscale=yscale=zscale=1.0f;
    parent_scene=NULL;

    sc=new script("/usr/local/share/dreamchess/scripts/chess_piece.lua");
    //sc->run("entity_spawn");
}

void entity::generate_bbox()
{
	
}

bool entity::collision_at( vec pos )
{
    bool retval=FALSE;

    if ( pos.x > (xpos-(xsize/2)) && pos.x < (xpos+(xsize/2)) )
    if ( pos.y > (ypos-(ysize/2)) && pos.y < (ypos+(ysize/2)) )
    //if ( pos.z > (zpos-(zsize/2)) && pos.z < (zpos+(zsize/2)) )
    {
        printf( "Collision: %s\n", name.c_str() );
        if ( type == "ENTITY" );
        retval=TRUE;
    }

    if ( name=="LIGHT" )
    {
        printf( "X:%f,%f,%f\n", (xpos-(xsize/2)), pos.x, (xpos+(xsize/2)) );
        printf( "Y:%f,%f,%f\n", (ypos-(ysize/2)), pos.y, (ypos+(ysize/2)) );
    }

    return retval;
}

void entity::update()
{
    //script_test();
}




















