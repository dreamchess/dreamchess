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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "dreamer.h"
#include "commands.h"
#include "e_comm.h"
#include "move.h"
#include "repetition.h"
#include "ttable.h"
#include "git_rev.h"
#include "search.h"
#include "san.h"
#include "timer.h"

static int is_coord_move(const char *ms)
{
    int len = strlen(ms);
    /* Check format. */
    if ((len < 4) || (len > 5))
        return 0;
    if ((ms[0] < 'a') || (ms[0] > 'h') ||
            (ms[1] < '1') || (ms[1] > '8') ||
            (ms[2] < 'a') || (ms[2] > 'h') ||
            (ms[3] < '1') || (ms[3] > '8'))
        return 0;

    if (len == 5)
        switch(ms[4])
        {
        case 'q':
        case 'r':
        case 'n':
        case 'b':
            return 1;
        default:
            return 0;
        }

    return 1;
}

static int convert_piece(int san_piece)
{
    switch (san_piece)
    {
    case SAN_KING:
        return KING;
    case SAN_QUEEN:
        return QUEEN;
    case SAN_ROOK:
        return ROOK;
    case SAN_KNIGHT:
        return KNIGHT;
    case SAN_BISHOP:
        return BISHOP;
    case SAN_PAWN:
        return PAWN;
    }

    /* We should never get here */
    assert(0);
}

static int san_piece(int piece)
{
    switch (piece)
    {
    case KING:
        return SAN_KING;
    case QUEEN:
        return SAN_QUEEN;
    case ROOK:
        return SAN_ROOK;
    case KNIGHT:
        return SAN_KNIGHT;
    case BISHOP:
        return SAN_BISHOP;
    case PAWN:
        return SAN_PAWN;
    }

    /* We should never get here */
    assert(0);
}

static Move get_san_move(Board &board, int ply, san_move_t *san)
{
    Move move;
    int piece;
    bitboard_t en_passant = board.en_passant;
    int castle_flags = board.castle_flags;
    int fifty_moves = board.fifty_moves;
    int found = 0;
    Move found_move;

    if (san->type == SAN_QUEENSIDE_CASTLE)
    {
        san->source_file = 4;
        san->source_rank = (board.current_player == SIDE_WHITE ? 0 : 7);
        san->destination = (board.current_player == SIDE_WHITE ? 2 : 58);
        piece = KING + board.current_player;
    }
    else if (san->type == SAN_KINGSIDE_CASTLE)
    {
        san->source_file = 4;
        san->source_rank = (board.current_player == SIDE_WHITE ? 0 : 7);
        san->destination = (board.current_player == SIDE_WHITE ? 6 : 62);
        piece = KING + board.current_player;
    }
    else
        piece = convert_piece(san->piece) + board.current_player;

    g_moveGenerator->computeLegalMoves(board, ply);

    /* Look for move in list. */
    while (!(move = g_moveGenerator->getNextMove(board, ply)).isNone())
    {
        int move_piece;

        if (move.getDest() != san->destination)
            continue;

        if (board.current_player == SIDE_WHITE)
            move_piece = board.findPiece<SIDE_WHITE>(move.getSource());
        else
            move_piece = board.findPiece<SIDE_BLACK>(move.getSource());

        if (move_piece != piece)
            continue;

        if (san->source_file != SAN_NOT_SPECIFIED)
            if (san->source_file != move.getSource() % 8)
                continue;

        if (san->source_rank != SAN_NOT_SPECIFIED)
            if (san->source_rank != move.getSource() / 8)
                continue;

        if (san->type == SAN_CAPTURE)
        {
            /* TODO verify en passant capture? */
            if (!move.doesCapture())
                    continue;
        }

        if (move.doesPromotion()) {
            if (san->promotion_piece != san_piece(move.getPieceKind()))
                continue;
        } else if (san->promotion_piece != SAN_NOT_SPECIFIED) {
            continue;
        }

        /* TODO verify check and checkmate flags? */

        board.makeMove(move);
        board.current_player = OPPONENT(board.current_player);
        if (!g_moveGenerator->isCheck(board, ply + 1))
        {
            found++;
            found_move = move;
        }
 
        board.current_player = OPPONENT(board.current_player);
        board.unmakeMove(move, en_passant, castle_flags, fifty_moves);
    }

    if (found != 1)
        return Move();

    return found_move;
}

static char get_promotion_char(Move move) {
    switch (move.getPieceKind()) {
    case QUEEN:
        return 'q';
    case ROOK:
        return 'r';
    case KNIGHT:
        return 'n';
    case BISHOP:
        return 'b';
    default:
        return 0;
    }
}

static Move get_coord_move(Board &board, int ply, const char *ms)
{
    int source, dest;
    Move move;

    source = (ms[0] - 'a') + 8 * (ms[1] - '1');
    dest = (ms[2] - 'a') + 8 * (ms[3] - '1');

    g_moveGenerator->computeLegalMoves(board, ply);

    /* Look for move in list. */
    while (!(move = g_moveGenerator->getNextMove(board, ply)).isNone())
    {
        if ((move.getSource() == source) && (move.getDest() == dest))
        {
            if (move.doesPromotion())
            {
                if (strlen(ms) != 5 || ms[4] != get_promotion_char(move))
                    continue;
            } 
            else if (strlen(ms) != 4)
            {
                 continue;
            }

            bitboard_t en_passant = board.en_passant;
            int castle_flags = board.castle_flags;
            int fifty_moves = board.fifty_moves;

            /* Move found. */
            board.makeMove(move);
            board.current_player = OPPONENT(board.current_player);
            if (!g_moveGenerator->isCheck(board, ply + 1))
            {
                board.current_player = OPPONENT(board.current_player);
                board.unmakeMove(move, en_passant, castle_flags, fifty_moves);
                break;
            }
            board.current_player = OPPONENT(board.current_player);
            board.unmakeMove(move, en_passant, castle_flags, fifty_moves);
        }
    }
    return move;
}

char *coord_move_str(Move move)
{
    char *ret = (char *)malloc(6);
    ret[0] = 'a' + move.getSource() % 8;
    ret[1] = '1' + move.getSource() / 8;
    ret[2] = 'a' + move.getDest() % 8;
    ret[3] = '1' + move.getDest() / 8;

    if (move.doesPromotion()) {
        ret[4] = get_promotion_char(move);
        ret[5] = 0;
    } else {
        ret[4] = 0;
    }

    return ret;
}

char *san_move_str(Board &board, int ply, Move move)
{
    san_move_t san_move;
    int state;
    int move_piece;
    bitboard_t en_passant = board.en_passant;
    int castle_flags = board.castle_flags;
    int fifty_moves = board.fifty_moves;

    board.makeMove(move);
    state = g_moveGenerator->checkGameState(board, ply);
    board.unmakeMove(move, en_passant, castle_flags, fifty_moves);

    switch (state)
    {
    case STATE_CHECK:
        san_move.state = SAN_STATE_CHECK;
        break;
    case STATE_MATE:
        san_move.state = SAN_STATE_CHECKMATE;
        break;
    default:
        san_move.state = SAN_STATE_NORMAL;
    }

    switch (move.getType())
    {
    case Move::Type::QueensideCastle:
        san_move.type = SAN_QUEENSIDE_CASTLE;
        return san_string(&san_move);
    case Move::Type::KingsideCastle:
        san_move.type = SAN_KINGSIDE_CASTLE;
        return san_string(&san_move);
    case Move::Type::Capture:
    case Move::Type::EnPassant:
    case Move::Type::PromotionCapture:
        san_move.type = SAN_CAPTURE;
        break;
    default:
        san_move.type = SAN_NORMAL;
    }

    if (board.current_player == SIDE_WHITE)
        move_piece = board.findPiece<SIDE_WHITE>(move.getSource()) & PIECE_MASK;
    else
        move_piece = board.findPiece<SIDE_BLACK>(move.getSource()) & PIECE_MASK;

    san_move.piece = san_piece(move_piece);

    if (move.doesPromotion())
        san_move.promotion_piece = san_piece(move.getPieceKind());
    else
        san_move.promotion_piece = SAN_NOT_SPECIFIED;

    san_move.source_file = SAN_NOT_SPECIFIED;
    san_move.source_rank = SAN_NOT_SPECIFIED;
    san_move.destination = move.getDest();

    if (san_move.piece == SAN_PAWN)
    {
        if (move.getSource() % 8 != move.getDest() % 8)
            san_move.source_file = move.getSource() % 8;
    }
    else
    {
        Move u_move;
        u_move = get_san_move(board, ply, &san_move);

        if (u_move.isNone())
        {
            san_move.source_file = move.getSource() % 8;
            u_move = get_san_move(board, ply, &san_move);
            if (u_move.isNone())
            {
                san_move.source_file = SAN_NOT_SPECIFIED;
                san_move.source_rank = move.getSource() / 8;
                u_move = get_san_move(board, ply, &san_move);
                if (u_move.isNone())
                {
                    san_move.source_file = move.getSource() % 8;
                    u_move = get_san_move(board, ply, &san_move);
                    if (u_move.isNone())
                    {
                        char *move_s = coord_move_str(move);

                        e_comm_send("failed to convert move %s to SAN notation", move_s);

                        free(move_s);
                        return NULL;
                    }
                }
            }
        }
    }

    return san_string(&san_move);
}

static void error(const char *type, const char *command)
{
    e_comm_send("Error (%s): %s\n", type, command);
}

#define NOT_NOW(c) error("command not legal now", c)
#define UNKNOWN(c) error("unknown command", c)
#define BADPARAM(c) error("invalid or missing parameter(s)", c)

static int parse_time_control(Dreamer *state, const char *s)
{
    struct time_control t;
    char *end;
    char *semi;

    errno = 0;
    t.mps = strtol(s, &end, 10);

    if (errno || *end != ' ')
        return 1;

    semi = strchr(end + 1, ':');

    if (semi) {
        *semi = ' ';
        t.base = strtol(end + 1, &end, 10) * 60 * 100;
        *semi = ':';

        if (errno || end != semi)
            return 1;

        t.base += strtol(end + 1, &end, 10) * 100;

        if (errno || *end != ' ')
            return 1;
    } else {
        t.base = strtol(end + 1, &end, 10) * 60 * 100;

        if (errno || *end != ' ')
            return 1;
    }

    t.inc = strtol(end + 1, &end, 10) * 100;

    if (errno || *end != 0)
        return 1;

    /* Time control with both mps and inc is invalid */
    if (t.mps != 0 && t.inc != 0)
        return 1;

    state->time = t;
    state->engineTime.set(t.base);
    return 0;
}

static int command_always(Dreamer *state, const char *command)
{
    if (!strcmp(command, "post"))
    {
        state->setOption(OPTION_POST, 1);
        return 1;
    }

    if (!strcmp(command, "nopost"))
    {
        state->setOption(OPTION_POST, 0);
        return 1;
    }

    if (!strncmp(command, "time ", 5))
    {
        int time;
        char *end;
        errno = 0;
        time = strtol(command + 5, &end, 10);
        if (errno || *end != 0)
            BADPARAM(command);
        else
            state->engineTime.set(time);
        return 1;
    }

    if (!strncmp(command, "otim ", 5))
        return 1;

    return 0;
}

int parse_move(Board &board, int ply, const char *command, Move *move)
{
    san_move_t *san;

    san = san_parse(command);

    if (san)
    {
        *move = get_san_move(board, ply, san);
        free(san);
        return 0;
    }
    else if (is_coord_move(command))
    {
        *move = get_coord_move(board, ply, command);
        return 0;
    }

    return 1;
}

int command_usermove(Dreamer *state, const char *command)
{
    Move move;

    if (!parse_move(state->board, 0, command, &move))
    {
        if (move.isNone()) {
            e_comm_send("Illegal move: %s\n", command);
            return 0;
        }

        if (state->isMyTurn())
        {
            NOT_NOW(command);
            return 0;
        }

	if (state->mode == MODE_WHITE || state->mode == MODE_BLACK)
		state->engineTime.start(Timer::Direction::Down);

        state->doMove(move);
        state->checkGameEnd();

        if (state->mode == MODE_IDLE)
            state->mode = (state->board.current_player == SIDE_WHITE?
                           MODE_WHITE : MODE_BLACK);

        if (!state->ponder_my_move.isNone()) {
            /* We already have a possible answer to this move from pondering. */
            if (move == state->ponder_opp_move && state->ponder_my_move.isRegular())
            {
                /* User made the expected move. */
                state->sendMove(state->ponder_my_move);
            }

            state->ponder_my_move = Move();
        }

        return 0;
    }

    return 1;
}

void command_handle(Dreamer *state, const char *command)
{
    if (command_always(state, command))
        return;

    if (!strcmp(command, "xboard"))
    {
        /* xboard mode is default. */
        return;
    }

    if (!strncmp(command, "protover ", 9))
    {
        char *endptr;
        errno = 0;
        strtol(command + 9, &endptr, 10);

        if (errno || (*endptr != 0))
             BADPARAM(command);

        e_comm_send("feature myname=\"Dreamer %s\"\n", g_version);
        e_comm_send("feature setboard=1\n");
        e_comm_send("feature colors=0\n");
        e_comm_send("feature done=1\n");
        return;
    }

    if (!strncmp(command, "level ", 6))
    {
        if (parse_time_control(state, command + 6))
            BADPARAM(command);

        return;
    }

    if (!strncmp(command, "accepted ", 9))
    {
        if (!strcmp(command + 9, "setboard") || !strcmp(command + 9, "done")
            || !strcmp(command + 9, "myname") || !strcmp(command + 9, "colors"))
            return;

        BADPARAM(command);
        return;
    }

    if (!strcmp(command, "new"))
    {
        state->board.setup();
        g_moveGenerator->clearHistory();
        g_transTable->clear();
        pv_clear();
        repetition_init(state->board);
        state->done = 0;
        state->mode = MODE_BLACK;
        state->flags = 0;
        state->depth = MAX_DEPTH;

	if (state->undo_data != NULL)
		free(state->undo_data);
	state->undo_data = NULL;

	state->moves = 0;
	state->engineTime.set(state->time.base * 60 * 100);

        state->hint = Move();
        state->ponder_opp_move = Move();
        state->ponder_my_move = Move();
        state->ponder_actual_move = Move();
        return;
    }

    if (!strcmp(command, "quit"))
    {
        state->mode = MODE_QUIT;
        return;
    }

    if (!strcmp(command, "force"))
    {
        state->mode = MODE_FORCE;
        return;
    }

    if (!strcmp(command, "white"))
    {
        if (state->board.current_player != SIDE_WHITE)
        {
            /* FIXME, we should support this, but right now it would cause
            ** severe problems with the undo system.
            */
            NOT_NOW(command);
            return;
        }
        state->mode = MODE_BLACK;
        return;
    }

    if (!strcmp(command, "black"))
    {
        if (state->board.current_player != SIDE_BLACK)
        {
            /* FIXME, see above. */
            NOT_NOW(command);
            return;
        }
        state->mode = MODE_WHITE;
        return;
    }

    if (!strcmp(command, "playother"))
    {
        if (state->mode != MODE_FORCE)
        {
            NOT_NOW(command);
            return;
        }

        if (state->board.current_player == SIDE_WHITE)
            state->mode = MODE_BLACK;
        else
            state->mode = MODE_WHITE;
        return;
    }

    if (!strncmp(command, "sd", 2))
    {
        const char *number = strchr(command, ' ');
        char *end;
        if (number && (*(number + 1) != '\0'))
        {
            long int val = strtol(number + 1, &end, 10);
            if ((*end == '\0') && (val > 0))
                state->depth = val;
            else
                BADPARAM(command);
        }
        else
            BADPARAM(command);
        return;
    }

    if (!strcmp(command, "go"))
    {
        if (state->board.current_player == SIDE_WHITE)
            state->mode = MODE_WHITE;
        else
            state->mode = MODE_BLACK;
        return;
    }

    if (!strcmp(command, "remove"))
    {
        switch (state->mode)
        {
        case MODE_WHITE:
            if (state->board.current_player == SIDE_WHITE)
            {
                NOT_NOW(command);
                return;
            }
            break;
        case MODE_BLACK:
            if (state->board.current_player == SIDE_BLACK)
            {
                NOT_NOW(command);
                return;
            }
            break;
        case MODE_FORCE:
            break;
        default:
            NOT_NOW(command);
            return;
        }

        if (state->moves < 2)
            NOT_NOW(command);

        state->undoMove();
        state->undoMove();
        state->done = 0;
        return;
    }

    if (!strcmp(command, "?"))
    {
        NOT_NOW(command);
        return;
    }

    if (!strncmp(command, "setboard ", 9))
    {
        Board board;

        if (state->mode != MODE_FORCE)
        {
            NOT_NOW(command);
            return;
        }

        if (board.setupFEN(command + 9))
        {
            BADPARAM(command);
            return;
        }

        state->board = board;
        g_moveGenerator->clearHistory();
        g_transTable->clear();
        repetition_init(state->board);
        state->done = 0;
        return;
    }

    if (!strcmp(command, "noquiesce"))
    {
        state->setOption(OPTION_QUIESCE, 0);
        return;
    }

    if (!strcmp(command, "easy"))
    {
        state->setOption(OPTION_PONDER, 0);
        state->ponder_my_move = Move();
        return;
    }

    if (!strcmp(command, "hard"))
    {
        state->setOption(OPTION_PONDER, 1);
        if (state->moves != 0)
            state->flags |= FLAG_PONDER;
        return;
    }

    if (!strcmp(command, "hint"))
    {
        if (!state->hint.isNone())
        {
            char *str = coord_move_str(state->hint);
            e_comm_send("Hint: %s\n", str);
            free(str);
        }
        return;
    }

    if (!command_usermove(state, command))
        return;

    UNKNOWN(command);
}

int command_check_abort(Dreamer *state, int ply, const char *command)
{
    if (!strcmp(command, "?"))
    {
        if (state->isMyTurn())
            return 1;
    }

    if (command_always(state, command))
        return 0;

    if (!strcmp(command, "new"))
    {
        state->flags = FLAG_NEW_GAME | FLAG_IGNORE_MOVE;
        return 1;
    }

    if (!strcmp(command, "quit") || !strcmp(command, "force"))
    {
        state->flags = FLAG_IGNORE_MOVE;
        command_handle(state, command);
        return 1;
    }

    if (!strcmp(command, "easy"))
    {
        if (state->flags & FLAG_PONDER)
        {
            state->flags = FLAG_IGNORE_MOVE;
            state->ponder_actual_move = Move();
        }
        command_handle(state, command);
        return 1;
    }

    if (!strcmp(command, "hint"))
    {
        if (state->flags & FLAG_PONDER)
        {
            char *str = coord_move_str(state->ponder_opp_move);
            e_comm_send("Hint: %s\n", str);
            free(str);
            return 0;
        }
    }

    if (state->flags & FLAG_PONDER)
    {
        Move move;

        if (!parse_move(state->root_board, ply, command, &move))
        {
            if (move.isNone()) {
                e_comm_send("Illegal move: %s\n", command);
                return 0;
            }

            /* Start our timer */
            state->engineTime.start(Timer::Direction::Down);

            if (move == state->ponder_opp_move)
            {
                /* User made the expected move. */
                state->flags = 0;
                return 0;
            }
            else
            {
                /* User made a different move, abort and restart search. */
                state->flags = FLAG_IGNORE_MOVE;
                state->ponder_actual_move = move;
                return 1;
            }
        }
    }

    NOT_NOW(command);
    return 0;
}
