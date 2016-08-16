//
// Created by Lukas on 8/12/2016.
//

#include "init.h"
#include "system_info.h"

void init_libUtility()
{
    init_system_info();
}

void destroy_libUtility()
{
    destroy_system_info();
}
