#include "lib_setup.h"

#include "system_info.h"
#include "trappoint_interface.h"

void init()
{
    init_system_info();
    init_trappoints();
}

void destroy()
{
    destroy_trappoints();
    destroy_system_info();
}






