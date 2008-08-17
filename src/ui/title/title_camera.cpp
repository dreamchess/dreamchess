
#include "title.h"

float startrotx=-5,startrotz=-5;

title_camera::title_camera(std::string name2):camera(name2)
{
    name=name2;
    direction=1.0;
    speed=0.05;
    shake=FALSE;
}

void title_camera::update()
{
    if ( shake )
    {
        if ( startrotx == -5 )
            startrotx=xrot;

        if ( startrotz == -5 )
            startrotz=zrot;

        xrot=startrotx;
        xrot+=(float)(rand()%2)-1;

        zrot=startrotz;
        zrot+=(float)(rand()%2)-1;
    }
}
