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
#include "DreamChess.h"
#include "Image.h"

TitleScene::TitleScene(DreamChess *d) : Scene(d) {

}

void TitleScene::init() {
	_backdrop = new Image(this, "menu_title.png");
	_backdrop->setPosition(1280 / 2, 720 / 2);
	_backdrop->setSize(1280, 720);
	addObject(_backdrop);

	_chessBoard = new Model(this, "boards/classic/board.dcm", "boards/classic/board.png");
	_chessBoard->setPosition(-1.5, -0.25, -5);
	_chessBoard->setRotation(-80, 0, 45);
	addObject(_chessBoard);

	_chessBoardSquares = new Image(this, "boards/classic/board.png");
	_chessBoardSquares->setClipRegion(92, 92, 840, 840);

	glm::vec3 pos = _chessBoard->getPosition();
	_chessBoardSquares->setPosition(pos.x, pos.y, pos.z);
	_chessBoardSquares->setRotation(_chessBoard->getRotation());
	_chessBoardSquares->setSize(8, 8, 1);
	addObject(_chessBoardSquares);

	_whiteRook = new Model(this, "pieces/classic/rook.dcm", "pieces/classic/white.png");
	_whiteRook->setPosition(1, -0.25, -5);
	_whiteRook->setRotation(-80, 0, 45);
	addObject(_whiteRook);

	_blackQueen = new Model(this, "pieces/classic/queen.dcm", "pieces/classic/black.png");
	_blackQueen->setPosition(-1.5, -0.25, -5);
	_blackQueen->setRotation(-80, 0, 45);	
	addObject(_blackQueen);
}

void TitleScene::update() {

}
