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

#ifndef DREAMER_TIMER_H
#define DREAMER_TIMER_H

#include <chrono>

/** Timer with unit centiseconds. */

class Timer {
public:
	/** Timer direction. */
	enum class Direction {
		Up,  ///< Increase timer value
		Down ///< Decrease timer value
	};

	Timer() : _value(0), _state(State::Stopped) { }

	/**
	 * Starts the timer.
	 *
	 * @param dir Indicates whether the timer value should increase or decrease
	 */
	void start(const Direction dir);

	/**
	 * Stops the timer.
	 */
	void stop();

	/**
	 * Gets the current timer value.
	 *
	 * @return Current timer value in centiseconds
	 */
	int get() const;

	/**
	 * Sets the current timer value and stops the timer
	 *
	 * @param value The value in centiseconds to set the timer to
	 */
	void set(const int value);

private:
	enum class State {
		Stopped,
		CountingDown,
		CountingUp
	};

	int getTimePassed() const;

	int _value;
	State _state;
	std::chrono::time_point<std::chrono::steady_clock> _startTime;
};

#endif // DREAMER_TIMER_H
