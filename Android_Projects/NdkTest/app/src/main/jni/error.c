//
// Created by Lukas on 8/13/2015.
//

#ifndef NDKTEST_ERROR_H
#define NDKTEST_ERROR_H

#include <stdbool.h>

static const char* no_error = "No Error.";
static const char* last_error = "No Error.";

void set_last_error(const char* c)
{
    last_error = c;
}
bool error_occurred()
{
    return last_error != no_error;
}
const char* get_last_error()
{
    return last_error;
}
void clear_last_error()
{
    last_error = no_error;
}
#endif //NDKTEST_ERROR_H
