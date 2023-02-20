# DreamChess

[![Build status](https://github.com/dreamchess/dreamchess/actions/workflows/ci.yml/badge.svg)](https://github.com/dreamchess/dreamchess/actions/workflows/ci.yml)

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

The following dependencies are required for building DreamChess: [CMake](https://cmake.org/), [bison](https://www.gnu.org/software/bison/), [flex](https://github.com/westes/flex), [gettext](https://www.gnu.org/software/gettext/), [SDL 2.0](https://www.libsdl.org/), [SDL_image 2.0](https://www.libsdl.org/projects/SDL_image/), [SDL_mixer 2.0](https://www.libsdl.org/projects/SDL_mixer/), [Expat](https://libexpat.github.io/), [GLEW](https://glew.sourceforge.net/) and [FreeType](https://freetype.org/).

On Ubuntu this translates to the following package list: gcc cmake bison flex gettext libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libexpat1-dev libglew-dev libfreetype-dev

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

## Locating the saved games and config file

DreamChess uses these folders to store the saved games and config file:

| OS      | Folder                                   |
| ------- | ---------------------------------------- |
| Linux   | ~/.dreamchess                            |
| macOS   | ~/Library/Application Support/DreamChess |
| Windows | %APPDATA%\DreamChess                     |

## Acknowledgments

Special thanks go out to the following people:

- François Dominic Laramée - for his excellent [article](https://www.gamedev.net/articles/programming/artificial-intelligence/chess-programming-part-i-getting-started-r1014/) on chess programming
- Bob Hyatt - for his many useful Usenet posts over the years
- Dan Potter - for making KallistiOS
