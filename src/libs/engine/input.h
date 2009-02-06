
#ifndef __INPUT_H_
#define __INPUT_H_

//#include "resource.h"

class keyboard
{
	public:
		bool is_pressed( int key );
};

class mouse
{
	public:	
		bool is_pressed( int button );
};

/* Old stuffs o-o 

class input_event: public resource
{
    friend class keyboard_event;
    friend class mouse_event;
    friend class input_layer;
    public:
        input_event() { active=false; event_type=false; }
        bool is_active() { return active; }
        virtual void update() = 0;
        bool active;
        int event_type;
};

class mouse_event: public input_event
{
    public:
        mouse_event(std::string name2, std::string type2, int b, bool ot );
        void update();
    private:
        int button;
        bool one_time;
        bool wait_for_release;
};

class keyboard_event: public input_event
{
    public:
        keyboard_event(std::string name2, std::string type2, int c, bool ot );
        void update();
    private:
        int key;
        bool one_time;
        bool wait_for_release;
};

class input_layer: public resource_list
{
    public:
        bool get_input( std::string name );
        void update();
};*/

#endif /* INPUT_H */
