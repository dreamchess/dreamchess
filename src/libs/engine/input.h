
#ifndef __INPUT_H_
#define __INPUT_H_
#include "screen.h"

class keyboard
{
	public:
        keyboard();
		bool is_pressed( int key, bool one_time );
        bool wait_for_release[256];
};

class mouse
{
	public:	
        mouse();
		bool is_pressed(int button);

        vec position();
        vec position2d();
        
        void update_mouse();
        void update_mouse3d();

        vec mouse_2d;
        vec mouse_3d;
        bool mouse_buttons[5];
};

#endif /* INPUT_H */
