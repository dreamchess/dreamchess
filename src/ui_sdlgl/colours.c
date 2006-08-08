
#include "ui_sdlgl.h"

static gg_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_red =
    {
        1.0f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_white =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

static gg_colour_t col_yellow =
    {
        1.0f, 1.0f, 0.0f, 1.0f
    };

gg_colour_t *get_col( int colour )
{
    switch ( colour )
    {
        case COL_BLACK:
            return &col_black;
            break;
        case COL_RED:
            return &col_red;
            break;
        case COL_YELLOW:
            return &col_yellow;
            break;
        case COL_WHITE:
        default:
            return &col_white;
            break;
    }
}
