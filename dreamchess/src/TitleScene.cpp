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

#include "ResourcePool.h"
#include "TitleScene.h"
#include "Texture.h"
#include "System.h"
#include "Model.h"
#include "Dreamchess.h"
#include "Image.h"

TitleScene::TitleScene(DreamChess *d) : Scene(d) {

}

void TitleScene::init() {
	_backdrop = new Image(this, "menu_title.png");
	
	_backdrop2 = new Image(this, "menu_title.png");
    _backdrop2->setPosition(100, 100, 1);	
	
	_backdrop3D = new Image(this, "menu_title.png"); 
	_backdrop3D->setPosition(1, 1, -600);

	//_chessBoard = static_cast<Model*>(g_ResourcePool->getResource("boards/classic/board.dcm"));
}

void TitleScene::update() {

}

void TitleScene::render() {
	_game->getSystem()->go2D();
    
    _backdrop->render();    
    _backdrop2->render();    

    _game->getSystem()->go3D();
    _backdrop3D->render();
}