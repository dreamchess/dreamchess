
#include "title.h"

title_camera::title_camera()
{
    mod=0.03;
}

void title_camera::update()
{
    if ( ypos > 8 || ypos < -6)
        mod=-mod;

    ypos+=mod;
}
