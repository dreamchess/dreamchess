/*  DreamChess
**  Copyright (C) 2004-2005  The DreamChess project
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dreamer.h"
#include "e_comm.h"
#include "move.h"
#include "history.h"
#include "repetition.h"
#include "transposition.h"

static int is_check(board_t *board)
{
    move_t moves[28*16];
    board->current_player = OPPONENT(board->current_player);
    if (compute_legal_moves(board, moves) < 0)
    {
        /* We're in check. */
        board->current_player = OPPONENT(board->current_player);
        return 1;
    }
    board->current_player = OPPONENT(board->current_player);
    return 0;
}

static int is_coord_move(char *ms)
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

static move_t *get_coord_move(board_t *board, char *ms)
{
    int source, dest;
    move_t moves[28*16];
    int total_moves;
    int move_nr;
    move_t *move = malloc(sizeof(move_t));

    source = (ms[0] - 'a') + 8 * (ms[1] - '1');
    dest = (ms[2] - 'a') + 8 * (ms[3] - '1');

    total_moves = compute_legal_moves(board, moves);

    /* Look for move in list. */
    move_nr = 0;
    while (move_nr < total_moves)
    {
        if ((moves[move_nr].source == source) && (moves[move_nr].destination
                ==
                dest))
        {
            bitboard_t en_passant = board->en_passant;
            int castle_flags = board->castle_flags;
            int fifty_moves = board->fifty_moves;

            /* Move found. */
            execute_move(board, &moves[move_nr]);
            board->current_player = OPPONENT(board->current_player);
            if (!is_check(board))
            {
                *move = moves[move_nr];
                board->current_player = OPPONENT(board->current_player);
                unmake_move(board, &moves[move_nr], en_passant, castle_flags, fifty_moves);
                break;
            }
            board->current_player = OPPONENT(board->current_player);
            unmake_move(board, &moves[move_nr], en_passant, castle_flags, fifty_moves);
        }
        move_nr++;
    }
    if (move_nr < total_moves)
    {
        if (move->type & MOVE_PROMOTION_MASK)
        {
            /* Set correct promotion piece. */
            move->type &= ~MOVE_PROMOTION_MASK;
            if (strlen(ms) == 5)
                switch(ms[4])
                {
                case 'q':
                    move->type |= PROMOTION_MOVE_QUEEN;
                    break;
                case 'r':
                    move->type |= PROMOTION_MOVE_ROOK;
                    break;
                case 'n':
                    move->type |= PROMOTION_MOVE_KNIGHT;
                    break;
                case 'b':
                    move->type |= PROMOTION_MOVE_BISHOP;
                }
            else
            {
                /* No promotion piece specified. */
                free(move);
                return NULL;
            }
            return move;
        }
        if (strlen(ms) == 4)
            return move;
    }
    free(move);
    return NULL;
}

char *coord_move_str(move_t *move)
{
    char *ret = malloc(6);
    ret[5] = '\0';
    ret[0] = 'a' + move->source % 8;
    ret[1] = '1' + move->source / 8;
    ret[2] = 'a' + move->destination % 8;
    ret[3] = '1' + move->destination / 8;
    switch (move->type & MOVE_PROMOTION_MASK)
    {
    case PROMOTION_MOVE_QUEEN:
        ret[4] = 'q';
        break;
    case PROMOTION_MOVE_BISHOP:
        ret[4] = 'b';
        break;
    case PROMOTION_MOVE_KNIGHT:
        ret[4] = 'n';
        break;
    case PROMOTION_MOVE_ROOK:
        ret[4] = 'r';
        break;
    default:
        ret[4] = '\0';
    }
    return ret;
}

static void error(char *type, char *command)
{
    e_comm_send("Error (%s): %s\n", type, command);
}

#define NOT_NOW(c) error("command not legal now", c)
#define UNKNOWN(c) error("unknown command", c)
#define BADPARAM(c) error("invalid or missing parameter(s)", c)

static int coord_usermove(state_t *state, char *command)
{
    move_t *move;
    if (my_turn(state))
    {
        NOT_NOW(command);
        return 1;
    }
    move = get_coord_move(&state->board, command);
    if (!move)
    {
        e_comm_send("Illegal move: %s\n", command);
        return 1;
    }
    else
    {
        do_move(state, move);
        check_game_end(state);
        return 0;
    }
}

void command_handle(state_t *state, char *command)
{
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

        e_comm_send("feature setboard=1 done=1\n");
        return;
    }

    if (!strncmp(command, "accepted ", 9))
    {
        if (!strcmp(command + 9, "setboard") || !strcmp(command + 9, "done"))
            return;

        BADPARAM(command);
        return;
    }

    if (!strcmp(command, "new"))
    {
        setup_board(&state->board);
        forget_history();
        clear_table();
        repetition_init(&state->board);
        state->depth = 1;
        state->mode = MODE_BLACK;
        state->done = 0;
        set_option(OPTION_QUIESCE, 1);
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
        char *number = strchr(command, ' ');
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

        undo_move(state);
        undo_move(state);
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
        board_t board;

        if (state->mode != MODE_FORCE)
        {
            NOT_NOW(command);
            return;
        }

        if (setup_board_fen(&board, command + 9))
        {
            BADPARAM(command);
            return;
        }

        state->board = board;
        forget_history();
        clear_table();
        repetition_init(&state->board);
        state->done = 0;
        return;
    }

    if (!strncmp(command, "noquiesce", 9))
    {
        set_option(OPTION_QUIESCE, 0);
        return;
    }

    if (is_coord_move(command))
    {
        if (!coord_usermove(state, command))
        {
            if (state->mode == MODE_IDLE)
                state->mode = (state->board.current_player == SIDE_WHITE?
                               MODE_WHITE : MODE_BLACK);
        }
        return;
    }

    UNKNOWN(command);
}

int command_check_abort(state_t *state, char *command)
{
    if (!strcmp(command, "?"))
        return 1;
    else if (!strcmp(command, "new"))
    {
        state->flags = FLAG_NEW_GAME | FLAG_IGNORE_MOVE;
        return 1;
    }
    else if (!strcmp(command, "quit") || !strcmp(command, "force"))
    {
        state->flags = FLAG_IGNORE_MOVE;
        command_handle(state, command);
        return 1;
    }
    else
    {
        NOT_NOW(command);
        return 0;
    }
}
