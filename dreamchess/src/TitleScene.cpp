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
#include <GL/glew.h>
#include "TitleScene.h"
#include "Texture.h"
#include "System.h"

void TitleScene::init() {
    _backdrop = new Texture();
    _backdrop->load("menu_title.png", 0, 1);
}

void TitleScene::update() {

}

void TitleScene::render() {
	g_System->go2D();
    _backdrop->render(0, 0, 1, 1280, 720);
}