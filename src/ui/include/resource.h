#ifndef __RESOURCE_LIST_H_
#define __RESOURCE_LIST_H_

#include <vector>
#include <string>
#include <iostream>

class resource
{   
    public:
        resource();
        std::string name;
        std::string type;
};

class resource_list
{
    public:
        ~resource_list();
        void *get_resource( std::string name, std::string type );
        bool find_resource( std::string name, std::string type );
        void update();
        void add( resource *res );
        void clear();
        void list();
        std::vector<resource*> resources;
};

#endif /* __RESOURCE_LIST_H */
