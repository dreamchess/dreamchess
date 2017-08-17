# DreamChess

[![Build status](https://travis-ci.org/dreamchess/dreamchess.svg?branch=master)](https://travis-ci.org/dreamchess/dreamchess)
[![Build status](https://ci.appveyor.com/api/projects/status/9c30h9u3p9bywpf0?svg=true)](https://ci.appveyor.com/project/waltervn/dreamchess)

## About

DreamChess is an open source chess game. It comes with its own
engine called Dreamer.

Both DreamChess and Dreamer are compatible with the
xboard/Winboard chess engine communication protocol. This means that
DreamChess can be used with other xboard-compatible chess engines such as
[crafty](http://www.craftychess.com/) and [GNU Chess](https://www.gnu.org/software/chess/).
Similarly, the Dreamer chess engine can
be used with other xboard-compatible user interfaces such as [XBoard/WinBoard](https://www.gnu.org/software/xboard/).

DreamChess currently supports Windows, macOS and Linux. It is licensed under the [GPLv3](https://www.gnu.org/licenses/gpl.html).

## Building

The following dependencies are required for building DreamChess: [CMake](https://cmake.org/), [bison](https://www.gnu.org/software/bison/), [flex](https://github.com/westes/flex), [SDL 1.2](https://www.libsdl.org/), [SDL_image 1.2](https://www.libsdl.org/projects/SDL_image/), [SDL_mixer 1.2](https://www.libsdl.org/projects/SDL_mixer/), [Mini-XML](http://michaelrsweet.github.io/mxml/) and [GLEW](http://glew.sourceforge.net/).

On Ubuntu this translates to the following package list: gcc, cmake, bison, flex, libsdl1.2-dev, libsdl-image1.2-dev, libsdl-mixer1.2-dev, libmxml-dev and libglew-dev.

## Controls

DreamChess can be controlled with the mouse and/or the keyboard. You can
select menu items or chess pieces either by pointing the mouse and left-clicking, or by using the arrow keys and pressing Enter.

The board view can be rotated freely by holding down the middle mouse button and moving the mouse. When using the keyboard, hold down the left Ctrl key
and press the arrow keys.

The in-game menu can be opened by right-clicking the mouse, or by pressing
the Esc key.

Additionally, there are several keyboard shortcuts you may like to use:

| Button          | Action                                 |
| --------------- | -------------------------------------- |
| 'P' Key         | View previous move in game history     |
| 'N' Key         | View next move in game history         |
| 'R' Key         | Retract move (when possible)           |
| Alt+Enter / F11 | Toggle fullscreen mode                 |
| Ctrl+F          | Toggle FPS counter (title screen only) |

## Acknowledgments

Special thanks go out to the following people:

- François Dominic Laramée - for his excellent [article](https://www.gamedev.net/articles/programming/artificial-intelligence/chess-programming-part-i-getting-started-r1014/) on chess programming
- Bob Hyatt - for his many useful Usenet posts over the years
- Dan Potter - for making KallistiOS
