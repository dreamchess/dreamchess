
#include "SDL.h"
#include "input.h"

keyboard::keyboard()
{
    for (int i=0; i<256; i++)
        wait_for_release[i]=false;
}

bool keyboard::is_pressed( int key, bool one_time )
{
    SDL_PumpEvents();
    Uint8 *keystate = SDL_GetKeyState(NULL);

    int active=keystate[key];

    // Wait for release is active for the key.
    if ( wait_for_release[key] )
    {        
        if ( !active ) // If the key is not pressed, reset wait for release.
            wait_for_release[key]=false;
        else // If it is pressed, force a 'false' return, even if it's pressed.
            active=false;
    }
    else // Wait for release not set
    {
        if ( one_time && active) // If 'one time' set and the key is active, set wait for release.
            wait_for_release[key]=true;
    }

    // Return the key's active state -- or false, if it's been forced by wait_for_release.
    return active;	
}

bool mouse::is_pressed( int button, bool one_time )
{
    SDL_PumpEvents();
    Uint8 *keystate = SDL_GetKeyState(NULL);

    int active=SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(button);

    // Wait for release is active for the key.
    if ( wait_for_release[button] )
    {        
        if ( !active ) // If the key is not pressed, reset wait for release.
            wait_for_release[button]=false;
        else // If it is pressed, force a 'false' return, even if it's pressed.
            active=false;
    }
    else // Wait for release not set
    {
        if ( one_time && active) // If 'one time' set and the key is active, set wait for release.
            wait_for_release[button]=true;
    }

    // Return the key's active state -- or false, if it's been forced by wait_for_release.
    return active;	
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

