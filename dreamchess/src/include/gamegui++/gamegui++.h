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

/*! \file */

#ifndef GAMEGUIPP_GAMEGUIPP_H
#define GAMEGUIPP_GAMEGUIPP_H

#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace GameGUI {

//! Dimensions
struct Size {
	int width;  //!< Width in pixels
	int height; //!< Height in pixels
};

//! Point
struct Point {
	int x; //!< X-coordinate in pixels
	int y; //!< Y-coordinate in pixels
};

//! Rectangle
struct Rect {
	int x;      //!< Leftmost x coordinate of the rectangle
	int y;      //!< Lowermost y coordinate of the rectangle
	int width;  //!< Width of the rectangle in pixels
	int height; //!< Height of the rectangle in pixels

	//! Determines whether or not the rectangle is valid
	bool isValid() { return width >= 0 && height >= 0; }

	//! Determines whether or not the rectangle is empty
	bool isEmpty() { return width == 0 && height == 0; }

	//! Clips the rectangle against rectangle \p other
	void clip(const Rect &other) {
		Rect clipped;
		clipped.x = std::max(x, other.x);
		clipped.y = std::max(y, other.y);
		clipped.width = std::min(x + width, other.x + other.width) - clipped.x;
		clipped.height = std::min(y + height, other.y + other.height) - clipped.y;

		if (clipped.width < 0)
			clipped.width = 0;
		if (clipped.height < 0)
			clipped.height = 0;

		*this = clipped;
	}
};

class Image {
public:
	virtual Size getSize() const = 0;
};

enum class EventType { None, Key, Char, Mouse };

enum class MouseEventType { ButtonDown, ButtonUp, Move };

struct MouseEvent {
	MouseEventType type;
	unsigned int button;
	Point pos;
};

enum class Key {
	Backspace = 0x08,
	Escape = 0x1b,
	Space = 0x20,
	Delete = 0x7f,
	Action = 0x100,
	Extra1,
	Extra2,
	Extra3,
	Up,
	Right,
	Down,
	Left,
	Home,
	End,
	Enter
};

struct Event {
	EventType type;
	Key key;
	MouseEvent mouse;
};

//! Focus propagation modes
enum class FocusMode {
	None,    //!< No children should get focus
	One,     //!< At most one child should get focus
	All,     //!< All children should get focus
	Disabled //!< All children disabled
};

//! Fill modes
enum class FillMode {
	Tile, //!< Tile image to fill target area
	Scale //!< Scale image to fill target area
};

//! RGBA colour
struct Colour {
	float r; //!< Red; ranges from 0.0f to 1.0f
	float g; //!< Green; ranges from 0.0f to 1.0f
	float b; //!< Blue; ranges from 0.0f to 1.0f
	float a; //!< Alpha; ranges from 0.0f (transparent) to 1.0f (opaque)
};

//! Text bounce parameters
struct TextBounce {
	float amp;        //!< Amplitude in pixels
	unsigned int len; //!< Wave length in characters
	float speed;      //!< Speed in bounces per second
};

class Driver;
class WindowManager;

class Widget {
public:
	virtual ~Widget() {}
	virtual void attach(WindowManager *windowMan);
	virtual void render(const Point &p, FocusMode focus) = 0;
	bool input(const Event &event);
	void setSize(Size size);
	Size getRequestedSize();
	void setRequestedSize(Size size);
	Rect getFocusPos();
	bool setFocusPos(const Point &p);
	Widget *getUltimateParent() const;
	bool isEnabled() const { return _enabled; }
	Widget *getParent() const { return _parent; }
	void setParent(Widget *parent) { _parent = parent; }

protected:
	Widget *_parent = nullptr;
	WindowManager *_windowMan = nullptr;
	bool _enabled = false;
	Size _minSize{};
	Size _forcedSize{-1, -1};
	Size _allocSize{};
};

class Container : public Widget {
public:
	void add(Widget *widget);
	std::size_t size();
	Widget *getChild(std::vector<Widget *>::size_type idx);

protected:
	std::vector<Widget *> _widgets;
};

class Bin : public Container {
public:
	Widget *getChild();
};

struct DialogStyle {
	bool applyTexture; //!< Set to true to apply a texture to the dialog
	Image *border[9];  //!< Images to use for texturing the dialog
	Colour fadeColour; //!< Colour of the quad that will be drawn the size of the whole screen.
	int horPadding;    //!< Horizontal padding (between the border and the widget) in pixels
	int vertPadding;   //!< Horizontal padding (between the border and the widget) in pixels
	struct {
		float factor;                    //!< Titlebar size factor relative to text height
		unsigned int sepHeight;          //!< Titlebar seperator height in pixels
		Colour textColour;               //!< Title text colour
		Colour bgHiColour1, bgHiColour2; //!< Titlebar gradient colours when dialog has focus
		Colour bgLoColour1, bgLoColour2; //!< Titlebar gradient colours when dialog does not have focus
	} title;
};

class Dialog : public Bin {
public:
	Dialog(const DialogStyle *style) : _style(style) {}
	void render(const Point &p, FocusMode focus) override;

protected:
	Point getScreenPos();
	void drawBorder();

	Dialog *_parentDialog = nullptr;
	bool _hidden = false;
	bool _modal = false;
	bool _autoHideParent = false;
	bool _moving = false;
	bool _leftButton = false;
	Point _pos{};
	Point _orgPos{};
	float _xAlign = 0.0f, _yAlign = 0.0f;
	const DialogStyle *_style = nullptr;
	std::string _title;
};

class Driver {
public:
	virtual void drawFilledRect(const GameGUI::Rect &rect, const GameGUI::Colour &colour) const = 0;
	virtual void drawGradientRect(const GameGUI::Rect &rect, const GameGUI::Colour &colTopLeft,
	                              const GameGUI::Colour &colTopRight, const GameGUI::Colour &colBottomLeft,
	                              const GameGUI::Colour &colBottomRight) const = 0;
	virtual void drawImage(const GameGUI::Image &image, const GameGUI::Rect &rectSource, const GameGUI::Rect &rectDest,
	                       GameGUI::FillMode fillH, GameGUI::FillMode fillV, const GameGUI::Colour &colour) const = 0;
	virtual const Image &getCharImage(char c) const = 0;
	virtual GameGUI::Size getCharSize(char c) const = 0;
	virtual void drawChar(char c, const GameGUI::Point &p, const GameGUI::Colour &colour) const = 0;
};

class WindowManager {
public:
	WindowManager(std::unique_ptr<Driver> driver) : _driver(std::move(driver)) {}

	template <class T>
	void open(const DialogStyle *style) {
		_dialogs.push_back(std::unique_ptr<Dialog>(new T(style)));
		_dialogs.back()->attach(this);
	}

	void render() {
		for (auto &it: _dialogs) {
			it->render({}, FocusMode::One);
		}
	}

	//! Gets the text bounce parameters.
	TextBounce getTextBounce() const { return _textBounce; }

	//! Sets the text bounce paramaters to \p textBounce.
	void setTextBounce(TextBounce textBounce) { _textBounce = textBounce; }

	//! Adjusts the clipping rectangle to \p rect, clipped to the previous clipping rectangle (if any).
	void adjustClipRect(Rect clip) {
		if (!_clipRects.empty())
			clip.clip(_clipRects.back());
		_clipRects.push_back(clip);
	}

	//! Undoes the last clipping adjustment.
	void undoClipRect() {
		if (!_clipRects.empty())
			_clipRects.pop_back();
	}

	//! Draws rectangle \p rect outline in colour \p colour.
	void drawRect(Rect rect, const Colour &colour) const {
		Rect r(rect);
		r.height = 1;
		drawFilledRect(r, colour);
		r.y += rect.height - 1;
		drawFilledRect(r, colour);
		r = {r.x, rect.y + 1, 1, rect.height - 2};
		drawFilledRect(r, colour);
		r.x += rect.width - 1;
		drawFilledRect(r, colour);
	}

	//! Draws rectangle \p rect filled with colour \p colour.
	void drawFilledRect(Rect rect, const Colour &colour) const {
		if (!_clipRects.empty())
			rect.clip(_clipRects.back());
		_driver->drawFilledRect(rect, colour);
	}

	//! Draws gradient rectangle \p rect using a colour for each corner.
	void drawGradientRect(Rect rect, const Colour &colTopLeft, const Colour &colTopRight, const Colour &colBottomRight,
	                      const Colour &colBottomLeft) const {
		if (!_clipRects.empty())
			rect.clip(_clipRects.back());
		_driver->drawGradientRect(rect, colTopLeft, colTopRight, colBottomRight, colBottomLeft);
	}

	//! Draws an image.
	/*! @param image The image to draw
	    @param rectS The rectangular area (in pixels) of \p image to use for drawing
	    @param rectD The rectangular screen area (in pixels) to fill with \p image
	    @param fillH Horizontal fill mode
	    @param fillV Vertical fill mode
	    @param colour The colour to draw the image with
	*/
	void drawImage(const Image &image, const Rect &rectS, const Rect &rectD, FillMode fillH, FillMode fillV,
	               const Colour &colour) const {
		Rect clipRectS(rectS);
		Rect clipRectD(rectD);

		if (!_clipRects.empty()) {
			int x_offset;
			int y_offset;

			clipRectD.clip(_clipRects.back());
			x_offset = clipRectD.x - rectD.x;
			y_offset = rectD.y + rectD.height - clipRectD.y - clipRectD.height;

			switch (fillH) {
			case FillMode::Scale: {
				// Magnification factor
				float x_mag = rectD.width / (float)rectS.width;

				clipRectS.x = (int)(rectS.x + x_offset / x_mag);
				clipRectS.width = (int)(clipRectD.width / x_mag);
			} break;
			case FillMode::Tile:
				clipRectS.x = rectS.x + x_offset % rectS.width;
				// Width is undefined for FillMode::Tile
			}

			switch (fillV) {
			case FillMode::Scale: {
				// Magnification factor
				float y_mag = rectD.height / (float)rectS.height;

				clipRectS.y = (int)(rectS.y + y_offset / y_mag);
				clipRectS.height = (int)(clipRectD.height / y_mag);
			} break;
			case FillMode::Tile:
				clipRectS.y = rectS.y + y_offset % rectS.height;
				// Height is undefined for FillMode::Tile
			}
		}

		_driver->drawImage(image, clipRectS, clipRectD, fillH, fillV, colour);
	}

	//! Gets the size of the image representing char \p c.
	Size getCharSize(char c) const { return _driver->getCharSize(c); }

	//! Gets the size of the image representing string \p str.
	Size getStringSize(const std::string &str) const {
		Size strSize{};

		for (size_t i = 0; i < str.size(); ++i) {
			Size charSize(getCharSize(str[i]));

			strSize.width += charSize.width;
			strSize.height = std::max(strSize.height, charSize.height);
		}

		return strSize;
	}

	//! Draws a string.
	/*! @param str The string to draw
	    @param p Screen position in pixels of where to draw
	    @param colour Text colour
	    @param bounce Whether or not to apply the bouncy text effect
	    @param align Horizontal alignment (0.0f = left, 1.0f = right)
	*/
	void drawString(const std::string &str, const Point &p, const Colour &colour, bool bounce, float align) const {
		auto timer = std::chrono::steady_clock::now().time_since_epoch();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timer);
		Rect rectDest{p.x};

		if (align != 0.0f) {
			Size size(getStringSize(str));
			rectDest.x -= (int)(size.width * align);
		}

		for (size_t i = 0; i < str.size(); ++i) {
			float phaseOffset(0.0f);
			int yOffset(0);
			const Image &image(_driver->getCharImage(str[i]));
			Colour black{0.0f, 0.0f, 0.0f, 1.0f};

			if (bounce) {
				float phase =
				    ((ms.count() % (int)(1000 / getTextBounce().speed)) / (1000 / getTextBounce().speed)) + phaseOffset;

				if (phase < 0.5)
					yOffset = (int)(phase * 2 * (getTextBounce().amp + 1));
				else
					yOffset = (int)(((1.0f - phase) * 2) * (getTextBounce().amp + 1));
			}

			Size charSize(image.getSize());
			Rect rectSource{0, 0, charSize.width, charSize.height};
			rectDest.width = rectSource.width;
			rectDest.height = rectSource.height;
			rectDest.y = p.y + yOffset;

			/* FIXME */
			rectDest.x += 2;
			rectDest.y -= 2;

			/* FIXME  Magic alpha value to turn off shadow */
			if (colour.a != 2.0f)
				drawImage(image, rectSource, rectDest, FillMode::Scale, FillMode::Scale, black);
			rectDest.x -= 2;
			rectDest.y += 2;

			drawImage(image, rectSource, rectDest, FillMode::Scale, FillMode::Scale, colour);

			rectDest.x += rectSource.width;

			phaseOffset += 1.0f / getTextBounce().speed / getTextBounce().len;
		}
	}

	Driver *getDriver() { return _driver.get(); }

private:
	std::unique_ptr<Driver> _driver;

	std::vector<std::unique_ptr<Dialog>> _dialogs;

	TextBounce _textBounce{2.0f, 10, 3.0f};
	std::vector<Rect> _clipRects;
};

} // namespace GameGUI

#endif
