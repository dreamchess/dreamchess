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
    input.add( (new keyboard_event("ONE_TIME", SDLK_1, TRUE)) );
    input.add( (new keyboard_event("CONSTANT", SDLK_2, FALSE)) );

    scene scn;
    scn.add( (new model("/usr/local/share/dreamchess/pieces/classic/knight.dcm")) );
    scn.add( (new model("/usr/local/share/dreamchess/pieces/classic/queen.dcm")) );

    while ( !input.get_input("QUIT") )
    {
        input.update();

        if ( input.get_input("ONE_TIME") )
            std::cout << "One time!" << std::endl;
        if ( input.get_input("CONSTANT") )
            std::cout << "Constant!" << std::endl;

        scn.render();
        scn.update();
        scr.update();
    }
}

