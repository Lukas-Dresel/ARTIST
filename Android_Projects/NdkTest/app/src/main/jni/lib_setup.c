#include "lib_setup.h"
#include "system_info.h"
#include "trappoint_interface.h"
#include "breakpoint.h"

void init()
{
    init_system_info();
    init_trappoints();
    init_breakpoints();
}

void destroy()
{
    destroy_breakpoints();
    destroy_trappoints();
    destroy_system_info();
}






