
#include "SDL.h"
#include "input.h"

bool keyboard::is_pressed( int key )
{
    SDL_PumpEvents();
    Uint8 *keystate = SDL_GetKeyState(NULL);

    return keystate[key];	
}

bool mouse::is_pressed( int button )
{
    SDL_PumpEvents();

    return SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(button);	
}

/* Old stuff! 

void input_layer::update()
{
    for ( int i=0; i<resources.size();i++ )
    {
        ((input_event*)resources[i])->update();
    }
}

bool input_layer::get_input( std::string name )
{
    bool retval=false;

    for ( int i=0; i<resources.size(); i++ )
    {
        if ( !name.compare(resources[i]->name) && ((input_event*)resources[i])->active )
        {
            retval=true;
        }
    }    
    return retval;
}

mouse_event::mouse_event( std::string name2, std::string type2, int b, bool ot )//: input_event() 
{
    button=b;
    one_time=ot;
    wait_for_release=false;
    name=name2;
    type=type2;
}

void mouse_event::update()
{
    SDL_PumpEvents();

    active=SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(button);

    if ( one_time ) // Mouse button should only register once.
    {
        if ( active )
        {
            if ( wait_for_release )
                active=false;
            else
                wait_for_release=true;
        }
        else
            wait_for_release=false;
    }

}

keyboard_event::keyboard_event( std::string name2, std::string type2, int c, bool ot ): input_event() 
{
    key=c;
    one_time=ot;
    wait_for_release=false;
    name=name2;
    type=type2;
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
                active=false;
            else
                wait_for_release=true;
        }
        else
            wait_for_release=false;
    }
}*/

