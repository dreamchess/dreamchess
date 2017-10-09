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

#ifndef DREAMCHESS_RESOURCEPOOL_H
#define DREAMCHESS_RESOURCEPOOL_H

#include <unordered_map>

class Texture;
class Model;
class DreamChess;

class Resource {
public:
	std::string _filename;
};

class ResourcePool {
public:
	ResourcePool(DreamChess *d);
	~ResourcePool();

	Resource *getResource(std::string name);
	Texture *getTexture(const char *filename);
	Model *getModel(const char *filename);
private:
    std::unordered_map<std::string, Resource *> _resources;
    DreamChess *_game;
};

#endif
