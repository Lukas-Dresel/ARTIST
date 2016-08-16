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
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_ENTRYPOINTS_QUICK_ENTRYPOINTS_H
#define NDKTEST_ART_ENTRYPOINTS_QUICK_ENTRYPOINTS_H

#include <inttypes.h>
#include <jni.h>

struct Thread;

struct PACKED(4) QuickEntryPoints
{
#define ENTRYPOINT_ENUM(name, rettype, ...) rettype ( * p ## name )( __VA_ARGS__ );

#include "quick_entrypoints_list.h"
  QUICK_ENTRYPOINT_LIST(ENTRYPOINT_ENUM)
#undef QUICK_ENTRYPOINT_LIST
#undef ENTRYPOINT_ENUM
};

#endif //NDKTEST_ART_ENTRYPOINTS_QUICK_ENTRYPOINTS_H
