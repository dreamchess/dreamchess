
#include "title.h"

title_camera::title_camera()
{
    direction=1.0;
    speed=0.001;
}

void title_camera::update()
{
    if ( ypos > 8 || ypos < -6)
        direction=-direction;

    ypos+=direction*speed;
}
