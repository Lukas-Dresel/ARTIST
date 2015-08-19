#include "debug_util.h"

#include <stdbool.h>

void waitForDebugger()
{
    #ifdef DEBUG_WAIT
    volatile bool run = true;
    #else
    volatile bool run = true;
    #endif

    int i = 0;
    while(!run)
    {
        i++;
    }
}