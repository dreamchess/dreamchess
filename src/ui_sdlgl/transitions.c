
#include "ui_sdlgl.h"

float fade_start;

void set_fade_start( float start )
{
    fade_start=start;
}

int draw_fade( int inout )
{
    float amount=(gg_system_get_ticks()-fade_start)/(FADE_SPEED*1000);

    gg_colour_t col={0.0f, 0.0f, 0.0f, 1.0f-amount };

    if ( inout == FADE_OUT )
        col.a=amount;

    if (col.a > 1.0f)
        col.a = 1.0f;

    if (col.a >= 0.0f)
        gg_system_draw_filled_rect(0, 0, 640, 480, &col );

    if ( amount >= 1.0f )
        return FALSE;

    return TRUE;
}
