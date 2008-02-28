
#ifndef __INPUT_H_
#define __INPUT_H_

#include <vector>
#include <string>
#include <iostream>

#define FALSE 0
#define TRUE -1

class input_event
{
    friend class keyboard_event;
    friend class input_layer;
    public:
        input_event() { tag="Empty."; active=FALSE; event_type=FALSE; }
        input_event( std::string t ) { tag=t; active=FALSE;} 
        bool is_active() { return active; }
        virtual void update() = 0;
    private:
        std::string tag;
        bool active;
        int event_type;
};

class keyboard_event: public input_event
{
    public:
        keyboard_event( std::string t, char c, bool ot );
        void update();
    private:
        char key;
        bool one_time;
        bool wait_for_release;
};

class input_layer
{
    public:
        ~input_layer();
        bool get_input( std::string name );
        void update();
        void add( input_event *event );
        void clear();
        void list();
    private:
        std::vector<input_event*> inputs;
};

#endif /* __B_BINPUT_H */
