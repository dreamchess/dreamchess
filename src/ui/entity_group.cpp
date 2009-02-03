
#include "entity_group.h"

entity_group::entity_group()
{
	type="ENTITY";
}

void entity_group::generate_bbox()
{
	
}

void entity_group::render()
{	
	for ( int i=0; i<resources.size(); i++ )
    {
    	((entity*)resources[i])->render();
    } 
}

void entity_group::update()
{
	for ( int i=0; i<resources.size(); i++ )
    {
    	((entity*)resources[i])->update();
    } 
}
