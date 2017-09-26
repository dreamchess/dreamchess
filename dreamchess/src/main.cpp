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

#include <stdio.h>
#include "SDL.h"

#include "DreamChess.h"
#include "System.h"

DreamChess *g_DreamChess;
System *g_System;

int main(int argc, char *argv[])
{
	LaunchArguments args = LaunchArguments(argc, argv);

    g_System = new System();
    g_System->initVideo();

    g_DreamChess = new DreamChess();
	g_DreamChess->init(&args);
	g_DreamChess->go();

    return true; 
}
