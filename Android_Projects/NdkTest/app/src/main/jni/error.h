//
// Created by Lukas on 8/13/2015.
//

#ifndef NDKTEST_ERROR_H
#define NDKTEST_ERROR_H

#include <stdbool.h>

void set_last_error(const char *c);

bool error_occurred();

const char *get_last_error();

void clear_last_error();

#endif //NDKTEST_ERROR_H
