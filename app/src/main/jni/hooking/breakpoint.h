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
// Created by Lukas on 2/9/2016.
//

#ifndef NDKTEST_BREAKPOINT_H
#define NDKTEST_BREAKPOINT_H

#include <stdint.h>
#include <stdbool.h>
#include "hooking_common.h"

void*   Breakpoint_Install      (void *address, uint32_t trap_method, HOOKCALLBACK handler,
                                 void *arg);
bool    Breakpoint_Enable       (void *p);
bool    Breakpoint_Disable      (void *p);
void    Breakpoint_Uninstall    (void *p);

#endif //NDKTEST_BREAKPOINT_H
