#include "lib_setup.h"

#include "system_info.h"
#include "trappoint_interface.h"

void init()
{
    init_system_info();
    init_trap_points();
}
void destroy()
{
    destroy_trap_points();
    destroy_system_info();
}






