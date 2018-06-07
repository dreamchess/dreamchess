/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the AUTHORS.txt file distributed
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

#ifdef _WIN32
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_thread.h"
#include <SDL2/SDL_syswm.h>

#include <GL/glew.h>

//#define NO_SDL_GLEXT
//#include "SDL_opengl.h"
//#include "SDL_joystick.h"

#include "debug.h"

#include "backend.h"

void Backend::init() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0) {
		DBG_ERROR("SDL initialization failed: %s", SDL_GetError());
		exit(1);
	}

	ch_datadir();	
}

int Backend::ch_dir(std::string name) {
	return chdir(name.c_str());
}