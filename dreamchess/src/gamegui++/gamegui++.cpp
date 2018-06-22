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

void Widget::init(WindowManager *windowMan) { _windowMan = windowMan; }

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

} // namespace GameGUI
