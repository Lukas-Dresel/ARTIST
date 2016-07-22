/*
 * Copyright 2016 Lukas Dresel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
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
