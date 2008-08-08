#ifndef __RESOURCE_LIST_H_
#define __RESOURCE_LIST_H_

#include <vector>
#include <string>
#include <iostream>

#define FALSE 0
#define TRUE -1

class resource
{   
    public:
        resource( std::string name, std::string type, void *data );
        std::string name;
        std::string type;
        void *data;
};

class resource_list
{
    public:
        ~resource_list();
        bool get_resource( std::string name, std::string type );
        void update();
        void add( std::string name, std::string type, void *data );
        void clear();
        void list();
        std::vector<resource*> resources;
};

#endif /* __RESOURCE_LIST_H */
