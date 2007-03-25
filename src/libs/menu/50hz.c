/*      
   GOAT Games Menu System
        
   50hz.c

   Copyright (C)2005 Cryptic Allusion, LLC

   License: KOS/BSD

*/

#include "libmenuint.h"

int goat_50hz() {
	return goat_get_info()->config[GOAT_CONFIG_5060HZ] == GOAT_CONFIG_50HZ;
}
