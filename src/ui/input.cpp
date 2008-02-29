
#include "SDL/SDL.h"
#include "input.h"

void input_layer::list()
{
    for ( int i=0; i<inputs.size();i++ )
    {
        std::cout << inputs[i]->tag << std::endl;
    }
}

void input_layer::update()
{
    for ( int i=0; i<inputs.size();i++ )
    {
        inputs[i]->update();
    }
}

bool input_layer::get_input( std::string name )
{
    bool retval=FALSE;

    for ( int i=0; i<inputs.size(); i++ )
    {
        if ( !name.compare(inputs[i]->tag) && inputs[i]->active )
            retval=TRUE;
    }    
    return retval;
}

void input_layer::add( input_event *event )
{
    inputs.push_back(event);
}

input_layer::~input_layer()
{
    clear();
}

void input_layer::clear()
{
    for ( int i=0; i<inputs.size();i++ )
    {
        delete inputs[i];
    }

    inputs.clear();
}

keyboard_event::keyboard_event( std::string t, int c, bool ot ): input_event(t) 
{
    key=c;
    one_time=ot;
    wait_for_release=FALSE;
}

void keyboard_event::update()
{
    SDL_PumpEvents();
    Uint8 *keystate = SDL_GetKeyState(NULL);

    active=keystate[key];

    if ( one_time ) // Keypress should only register once.
    {
        if ( active )
        {
            if ( wait_for_release )
                active=FALSE;
            else
                wait_for_release=TRUE;
        }
        else
            wait_for_release=FALSE;
    }

}

