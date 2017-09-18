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

#include <thread>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>

#include "dreamer.h"
#include "board.h"
#include "move.h"
#include "search.h"
#include "hashing.h"
#include "e_comm.h"
#include "commands.h"
#include "repetition.h"
#include "ttable.h"
#include "config.h"

static int start_time;

Dreamer::Dreamer() :
        undo_data(nullptr) { }

bool Dreamer::isMyTurn()
{
    return (((mode == MODE_WHITE) && (board.current_player == SIDE_WHITE))
            || ((mode == MODE_BLACK) && (board.current_player == SIDE_BLACK)));
}

bool Dreamer::isCheck(Board &board, int ply)
{
    /* FIXME */
    board.current_player = OPPONENT(board.current_player);
    if (g_moveGenerator->computeLegalMoves(board, ply) < 0)
    {
        /* We're in check. */
        board.current_player = OPPONENT(board.current_player);
        return true;
    }
    board.current_player = OPPONENT(board.current_player);
    return false;
}

int Dreamer::checkGameState(Board &board, int ply)
{
    Move move;
    int mate = STATE_MATE;
    g_moveGenerator->computeLegalMoves(board, ply);

    while (!(move = g_moveGenerator->getNextMove(board, ply)).isNone())
    {
        bitboard_t en_passant = board.en_passant;
        int castle_flags = board.castle_flags;
        int fifty_moves = board.fifty_moves;

        board.makeMove(move);
        board.current_player = OPPONENT(board.current_player);
        if (!isCheck(board, ply + 1))
        {
            mate = STATE_NORMAL;
            board.current_player = OPPONENT(board.current_player);
            board.unmakeMove(move, en_passant, castle_flags, fifty_moves);
            break;
        }
        board.current_player = OPPONENT(board.current_player);
        board.unmakeMove(move, en_passant, castle_flags, fifty_moves);
    }
    /* We're either stalemated or checkmated. */
    if (!isCheck(board, ply) && (mate == STATE_MATE))
        mate = STATE_STALEMATE;
    if (isCheck(board, ply) && (mate == STATE_NORMAL))
        mate = STATE_CHECK;
    return mate;
}

int Dreamer::getOption(int option)
{
    return options & (1 << option);
}

void Dreamer::setOption(int option, int value)
{
    options &= ~(1 << option);
    options |= (value << option);
}

void Dreamer::checkGameEnd()
{
    int res = checkGameState(board, 0);

    switch (res)
    {
    case STATE_MATE:
        done = 1;
        if (board.current_player == SIDE_WHITE)
            e_comm_send("0-1 {Black mates}\n");
        else
            e_comm_send("1-0 {White mates}\n");
        return;
    case STATE_STALEMATE:
        done = 1;
        e_comm_send("1/2-1/2 {Stalemate}\n");
        return;
    case STATE_NORMAL:
        switch (is_draw(board))
        {
        case 1:
            done = 1;
            e_comm_send("1/2-1/2 {Drawn by 3-fold repetition}\n");
            return;
        case 2:
            done = 1;
            e_comm_send("1/2-1/2 {Drawn by 50 move rule}\n");
            return;
        }
    }

}

int Dreamer::checkAbort(int ply)
{
    char *s;

    if (!(flags & FLAG_PONDER) && (moveTime.get() <= 0))
        return 1;

    s = e_comm_poll();
    if (!s)
        return 0;
    return command_check_abort(this, ply, s);
}

void Dreamer::doMove(Move move)
{
    moves++;
    undo_data = (undo_data_t *)realloc(undo_data, sizeof(undo_data_t) * moves);
    undo_data[moves - 1].en_passant =
        board.en_passant;
    undo_data[moves - 1].castle_flags =
        board.castle_flags;
    undo_data[moves - 1].fifty_moves =
        board.fifty_moves;
    undo_data[moves - 1].move =
        move;
    board.makeMove(move);
    repetition_add(board, move);
}

void Dreamer::undoMove()
{
    if (moves == 0)
        return;

    moves--;

    board.unmakeMove(undo_data[moves].move,
                undo_data[moves].en_passant,
                undo_data[moves].castle_flags,
                undo_data[moves].fifty_moves);

    repetition_remove();
}

void Dreamer::setStartTime()
{
    searchTime.set(0);
    searchTime.start(Timer::Direction::Up);
}

void Dreamer::setMoveTime()
{
    int safe_time = engineTime.get() - 1000;

    if (safe_time > 0)
    {
        if (time.mps == 0)
            moveTime.set(safe_time / 30 + time.inc);
        else {
            int moves_left = time.mps - (moves / 2) % time.mps;
            moveTime.set(safe_time / moves_left + time.inc);
        }
    }
    else
        moveTime.set(0);
}

int Dreamer::getTime()
{
    return searchTime.get();
}

void Dreamer::updateClock()
{
    int val;

    if (time.mps == 0)
        return;

    val = engineTime.get();

    if ((((moves  + 1) / 2) % time.mps) == 0)
        val += time.base;

    val += time.inc;

    engineTime.set(val);
}

void Dreamer::sendMove(Move move)
{
    char *str = coord_move_str(move);
    doMove(move);
    e_comm_send("move %s\n", str);
    moveTime.stop();
    updateClock();
    free(str);
    checkGameEnd();
}

void Dreamer::run()
{
    e_comm_init();
    setStartTime();

    time.mps = 40;
    time.base = 5;
    time.inc = 0;
    setOption(OPTION_QUIESCE, 1);
    setOption(OPTION_PONDER, 0);
    setOption(OPTION_POST, 0);

    command_handle(this, "new");

    while (mode != MODE_QUIT)
    {
        char *s;
        Move move;

        s = e_comm_poll();

        if (!s)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        else
        {
            command_handle(this, s);
            free(s);
        }

        if (mode != MODE_IDLE && mode != MODE_FORCE && !done)
        {
            if (isMyTurn())
            {
                flags = 0;
                setMoveTime();

                engineTime.start(Timer::Direction::Down);
                move = find_best_move(this);

                if (flags & FLAG_NEW_GAME)
                    command_handle(this, "new");
                else if (move.isRegular())
                {
                    sendMove(move);
                    engineTime.stop();
                    if (getOption(OPTION_PONDER))
                        flags |= FLAG_PONDER;
                }
            }
            else if (flags & FLAG_PONDER)
            {
                move = ponder(this);

                if (flags & FLAG_NEW_GAME)
                    command_handle(this, "new");
                else if (!isMyTurn())
                {
                    if (!move.isNone()) {
                        /* We are done pondering, but opponent hasn't moved yet. */
                        ponder_my_move = move;
                        flags = 0;
                    }
                    else
                    {
                        /* Opponent made an illegal move, continue pondering. */
                        if (getOption(OPTION_PONDER))
                            flags |= FLAG_PONDER;
                    }
                }
                else if (move.isRegular())
                {
                    /* Opponent made the expected move. */
                    sendMove(move);
                    if (getOption(OPTION_PONDER))
                        flags |= FLAG_PONDER;
                }
            }
        }
    }
}
