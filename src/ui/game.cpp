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

#include "input.h"
#include "game.h"

game::game()
{

}

game::~game()
{

}

void game::start()
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

    while ( !input.get_input("QUIT") )
    {
        currentTime = SDL_GetTicks();
        updateIterations = ((currentTime - lastFrameTime) + cyclesLeftOver);
  
        if (updateIterations > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL))
            updateIterations = (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL);

        while (updateIterations > UPDATE_INTERVAL) 
        {
            updateIterations -= UPDATE_INTERVAL;

            loop();

            scn.update();
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

        scn.render();
        scr->update();
    }
}

