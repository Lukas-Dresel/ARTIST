#ifdef __cplusplus
extern "C"
{
#endif

#include "lib_setup.h"
#include "system_info.h"
#include "hooking/trappoint_interface.h"
#include "hooking/self_patching_trappoint.h"

void init()
{
    init_system_info();
    init_trappoints();
    init_self_patching_trappoints();
}

void destroy()
{
    destroy_self_patching_trappoints();
    destroy_trappoints();
    destroy_system_info();
}

#ifdef __cplusplus
}
#endif





