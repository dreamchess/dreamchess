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

#ifndef DREAMCHESS_SYSTEM_H
#define DREAMCHESS_SYSTEM_H

#include <string>
#include <fstream>

struct SDL_Window;
#include "glm/glm.hpp"

enum {SHADER_VERTEX, SHADER_FRAGMENT};
enum {VERTLOCATION, NORMLOCATION, TEXLOCATION, COLLOCATION, COLMODLOCATION};

class System;

class Shader {
public:
	Shader(System *s, std::string vert, std::string frag);
	~Shader();
	
	void activate();
	int load(std::string filename, int type);
private:
	System *_system;
	unsigned int _programID;
	unsigned int _vertexShaderID;
	unsigned int _fragmentShaderID;
};

class System {
public:
	int pollEvents();
	
	int initVideo();
	void initGL();
	void finishFrame();

	void go2D();
	void go3D();

	int chDataDir();
	int chUserDir();
private:
	glm::mat4 _currentModelMatrix;
	Shader *_currentShader;

	SDL_Window *_window;
	int _width, _height, _ms;
	bool _fullscreen;
};

#endif
