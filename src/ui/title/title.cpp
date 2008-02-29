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

    input_layer input;
    input.add( (new keyboard_event("QUIT", SDLK_ESCAPE, TRUE)) );
    input.add( (new keyboard_event("UP", SDLK_UP, FALSE)) );
    input.add( (new keyboard_event("DOWN", SDLK_DOWN, FALSE)) );
    input.add( (new keyboard_event("LEFT", SDLK_LEFT, FALSE)) );
    input.add( (new keyboard_event("RIGHT", SDLK_RIGHT, FALSE)) );

    scene scn;
    scn.add( (new model("/usr/local/share/dreamchess/pieces/classic/knight.dcm",
        "/usr/local/share/dreamchess/pieces/classic/white.png" )) );
    scn.add( (new model("/usr/local/share/dreamchess/pieces/classic/queen.dcm",
        "/usr/local/share/dreamchess/pieces/classic/white.png")) );

    while ( !input.get_input("QUIT") )
    {
        input.update();

        if ( input.get_input("UP") )
            scn.cam.ypos+=0.01;
        if ( input.get_input("DOWN") )
            scn.cam.ypos-=0.01;
        if ( input.get_input("LEFT") )
            scn.cam.zpos+=0.01;
        if ( input.get_input("RIGHT") )
            scn.cam.zpos-=0.01;

        scn.render();
        scn.update();
        scr.update();
    }
}

