//
// Created by Lukas on 8/12/2015.
//

#ifndef NDKTEST_DEBUG_UTIL_H
#define NDKTEST_DEBUG_UTIL_H

static inline void waitForDebugger()
{
    #ifdef DEBUG_WAIT
    volatile bool run = false;
    #else
    volatile bool run = true;
    #endif

    int i = 0;
    while(!run)
    {
        i++;
    }
}

#endif //NDKTEST_DEBUG_UTIL_H
