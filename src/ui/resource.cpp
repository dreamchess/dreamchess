
#include "SDL.h"
#include "resource.h"

resource::resource( std::string n, std::string t, void *d )
{
    name=n;
    type=t;
    data=d;
}

void resource_list::list()
{
    for ( int i=0; i<resources.size();i++ )
    {
        std::cout << resources[i]->name << std::endl;
    }
}

bool resource_list::find_resource( std::string name, std::string type )
{
    bool retval=FALSE;

    for ( int i=0; i<resources.size(); i++ )
    {
        if ( !name.compare(resources[i]->name) ) 
        {
            // Name matches... check type too, unless it's set to 'ANY' ..
            retval=TRUE;
        }
    }    
    return retval;
}

void *resource_list::get_resource( std::string name, std::string type )
{
    void *retval=NULL;

    for ( int i=0; i<resources.size(); i++ )
    {
        if ( !name.compare(resources[i]->name) ) 
        {
            // Name matches... check type too, unless it's set to 'ANY' ..
            retval=resources[i];
        }
    }    
    return retval;
}

void resource_list::add( std::string name, std::string type, void *data )
{
    // set up parent?
    resource *res=new resource(name, type, data);
    resources.push_back(res);
}

resource_list::~resource_list()
{
    clear();
}

void resource_list::clear()
{
    for ( int i=0; i<resources.size();i++ )
    {
        delete resources[i];
    }

    resources.clear();
}


