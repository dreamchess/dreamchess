
#include "camera.h"

class title_screen
{
    public:
        title_screen();
        ~title_screen();
        void loop();
};

class title_camera: public camera
{
    public:
        title_camera();
        void update();
        float mod;
};
