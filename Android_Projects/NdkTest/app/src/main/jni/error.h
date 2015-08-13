//
// Created by Lukas on 8/13/2015.
//

#ifndef NDKTEST_ERROR_H
#define NDKTEST_ERROR_H

static const char* no_error = "No Error.";
static const char* last_error = "No Error.";

static inline void set_last_error(const char* c)
{
    last_error = c;
}
static inline bool error_occurred()
{
    return last_error != no_error;
}
static inline const char* get_last_error()
{
    return last_error;
}
static inline void clear_last_error()
{
    last_error = no_error;
}
#endif //NDKTEST_ERROR_H
