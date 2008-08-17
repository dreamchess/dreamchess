
#include "SDL/SDL.h"
#include "input.h"

void input_layer::update()
{
    for ( int i=0; i<resources.size();i++ )
    {
        ((input_event*)resources[i]->data)->update();
    }
}

bool input_layer::get_input( std::string name )
{
    bool retval=FALSE;

    for ( int i=0; i<resources.size(); i++ )
    {
        if ( !name.compare(resources[i]->name) && ((input_event*)resources[i]->data)->active )
        {
            retval=TRUE;
        }
    }    
    return retval;
}

mouse_event::mouse_event( int b, bool ot ): input_event() 
{
    button=b;
    one_time=ot;
    wait_for_release=FALSE;
}

void mouse_event::update()
{
    SDL_PumpEvents();

    active=SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(button);

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

keyboard_event::keyboard_event( int c, bool ot ): input_event() 
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

