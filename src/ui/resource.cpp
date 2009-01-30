
#include "SDL.h"
#include "resource.h"

resource::resource()
{

}

void resource_list::list()
{
    for ( int i=0; i<resources.size();i++ )
    {
        std::cout << "Name: " << resources[i]->name << " ,Type: " << resources[i]->type << std::endl;
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

void resource_list::add( resource *res )
{
    resources.push_back(res);
}

resource_list::~resource_list()
{
    clear();
}

void resource_list::clear()
{
	resources.clear();
}


