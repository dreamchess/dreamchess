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

#include <SDL.h>
#include <SDL_Image.h>
#include <GL/glew.h>

#include "ResourcePool.h"
#include "Texture.h"
#include "Mesh.h"
#include "System.h"
#include "DreamChess.h"

#include <stdio.h>

ResourcePool::ResourcePool(DreamChess *d) {
	_game = d;
}

Resource *ResourcePool::getResource(std::string name) {
	Resource *r = nullptr;

	auto it = _resources.find(name);

	if (it == _resources.end()) {
		// Resource not found, load it..
		size_t index = name.find_last_of('.');
		std::string extension = name.substr(index + 1);

		_game->getSystem()->chDataDir();

		if (extension == "png") {
			r = static_cast<Resource *>(getTexture(name.c_str()));
		}
		else if (extension == "dcm") {
			r = static_cast<Resource *>(getMesh(name.c_str()));
		}

		r->_filename = name;
		_resources.insert({name, r});

		return r;
	} 
	else {
		return it->second;
	}
}

Texture *ResourcePool::getTexture(const char *filename) {
	Texture *t = new Texture();
	t->load(filename, 0, 1);

	return t;
}

Mesh *ResourcePool::getMesh(const char *filename) {
	Mesh *m = new Mesh();
	m->loadDCM(filename);

	return m;
}