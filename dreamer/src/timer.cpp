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

#include "timer.h"

int Timer::getTimePassed() const {
	auto diff = std::chrono::steady_clock::now() - _startTime;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
	return (ms.count() + 5) / 10;
}

int Timer::get() const {
	switch (_state) {
	case State::Stopped:
		return _value;
	case State::CountingDown:
		return _value - getTimePassed();
	case State::CountingUp:
		return _value + getTimePassed();
	}
}

void Timer::set(const int value) {
	stop();
	_value = value;
}

void Timer::start(const Direction dir) {
	_startTime = std::chrono::steady_clock::now();
	_state = (dir == Direction::Up ? State::CountingUp : State::CountingDown);
}

void Timer::stop() {
	_value = get();
	_state = State::Stopped;
}
