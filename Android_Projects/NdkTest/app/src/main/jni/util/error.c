//
// Created by Lukas on 8/13/2015.
//

#ifndef NDKTEST_ERROR_H
#define NDKTEST_ERROR_H

#include <stdbool.h>
#include "../logging.h"

static const char * const no_error = "No Error.";
static const char *last_error = "No Error.";

void set_last_error(const char * c)
{
    LOGE("Setting the last error to %s", c);
    last_error = c;
}

bool error_occurred()
{
    return last_error != no_error;
}

const char * get_last_error()
{
    return last_error;
}

void clear_last_error()
{
    LOGE("Clearing the last set error.");
    last_error = no_error;
}

#endif //NDKTEST_ERROR_H
