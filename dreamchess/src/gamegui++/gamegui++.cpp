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

#include "gamegui++/gamegui++.h"

#include <cassert>

namespace GameGUI {

void Widget::attach(WindowManager *windowMan) { _windowMan = windowMan; }

void Widget::setRequestedSize(Size size) { _forcedSize = size; }

Size Widget::getRequestedSize() {
	return {std::max(_forcedSize.width, _minSize.width), std::max(_forcedSize.height, _minSize.height)};
}

void Widget::setSize(Size size) { _allocSize = size; }

GameGUI::Rect Widget::getFocusPos() { return {0, 0, _allocSize.width, _allocSize.height}; }

bool Widget::setFocusPos(const Point &p) { return true; }

Widget *Widget::getUltimateParent() const {
	if (!_parent)
		return nullptr;

	Widget *widget = _parent;

	while (widget->_parent)
		widget = widget->_parent;

	return widget;
}

void Container::add(Widget *widget) {
	_widgets.push_back(widget);
	widget->setParent(this);
	if (widget->isEnabled() /* && widget receives input */)
		_enabled = true;
}

std::size_t Container::size() { return _widgets.size(); }

Widget *Container::getChild(std::vector<Widget *>::size_type idx) { return _widgets[idx]; }

Widget *Bin::getChild() { return Container::getChild(0); }

static Colour colWhite{1.0f, 1.0f, 1.0f, 1.0f};
static Colour colBlack{0.0f, 0.0f, 0.0f, 1.0f};
#if 0
static Colour colBlue{0.0f, 0.5f, 1.0f, 1.0f};
static Colour colBlue2{0.0f, 0.0f, 0.5f, 1.0f};
static Colour colGrey{0.5f, 0.5f, 0.5f, 1.0f};
static Colour colGrey2{0.25f, 0.25f, 0.25f, 1.0f};
#endif

Point Dialog::getScreenPos() {
	Point pos(_pos);
	pos.x -= (int)(_minSize.width * _xAlign);
	pos.y -= (int)(_minSize.height * _yAlign);
	return pos;
}

void Dialog::drawBorder() {
	const Point pos(getScreenPos());

	// Currently, we always draw at the original size of the border images
	const int size(_style->border[0]->getSize().width);
	Rect area{pos.x + size, pos.y + size, _minSize.width - 2 * size, _minSize.height - 2 * size};

	const bool active = true; // FIXME
	int titleHeight(0);

	if (!_title.empty()) {
		Size titleSize(_windowMan->getStringSize(_title));

		titleHeight = (int)(titleSize.height * _style->title.factor + _style->title.sepHeight);

		Rect dest(area);
		dest.y += area.height - titleHeight;
		dest.height = _style->title.sepHeight;

		_windowMan->drawFilledRect(dest, colBlack);

		dest.y += _style->title.sepHeight;
		dest.height = titleHeight - _style->title.sepHeight;

		if (active)
			_windowMan->drawGradientRect(dest, _style->title.bgHiColour1, _style->title.bgHiColour2,
			                             _style->title.bgHiColour1, _style->title.bgHiColour2);
		else
			_windowMan->drawGradientRect(dest, _style->title.bgLoColour1, _style->title.bgLoColour2,
			                             _style->title.bgLoColour1, _style->title.bgLoColour2);

		_windowMan->adjustClipRect(dest);
		dest.y += (int)(titleSize.height * (_style->title.factor - 1.0f) / 2);
		_windowMan->drawString(
		    _title, {dest.x + dest.width / 2, dest.y + (int)(titleSize.height * (_style->title.factor - 1.0f) / 2)},
		    _style->title.textColour, false, 0.5f);
		_windowMan->undoClipRect();
	}

	Rect source{0, 0, size, size};
	Rect dest{pos.x, pos.y, size, size};

	// Draw four corners
	_windowMan->drawImage(*_style->border[6], source, dest, FillMode::Scale, FillMode::Scale, colWhite);
	dest.y += _minSize.height - size;
	_windowMan->drawImage(*_style->border[0], source, dest, FillMode::Scale, FillMode::Scale, colWhite);
	dest.x += _minSize.width - size;
	_windowMan->drawImage(*_style->border[2], source, dest, FillMode::Scale, FillMode::Scale, colWhite);
	dest.y -= _minSize.height - size;
	_windowMan->drawImage(*_style->border[8], source, dest, FillMode::Scale, FillMode::Scale, colWhite);

	// Draw bottom
	dest = {pos.x + size, pos.y, _minSize.width - 2 * size, size};
	_windowMan->drawImage(*_style->border[7], source, dest, FillMode::Tile, FillMode::Scale, colWhite);

	// Draw top
	dest.y += _minSize.height - size;
	_windowMan->drawImage(*_style->border[1], source, dest, FillMode::Tile, FillMode::Scale, colWhite);

	// Draw left
	dest = {pos.x, pos.y + size, size, _minSize.height - 2 * size};
	_windowMan->drawImage(*_style->border[3], source, dest, FillMode::Scale, FillMode::Tile, colWhite);

	// Draw right
	dest.x += _minSize.width - size;
	_windowMan->drawImage(*_style->border[5], source, dest, FillMode::Scale, FillMode::Tile, colWhite);

	/* Draw middle */
	dest = {pos.x + size, pos.y + size, _minSize.width - 2 * size, _minSize.height - 2 * size - titleHeight};
	_windowMan->drawImage(*_style->border[4], source, dest, FillMode::Tile, FillMode::Tile, colWhite);
}

void Dialog::render(const Point &p, FocusMode focus) {
	if (_hidden)
		return;

	// Draw the 'fade'
	_windowMan->drawFilledRect({0, 0, 640, 480}, _style->fadeColour);

	drawBorder();

	if (size() != 0)
		getChild()->render({p.x + _style->horPadding, p.y + _style->vertPadding}, FocusMode::One /* FIXME */);
}

} // namespace GameGUI
