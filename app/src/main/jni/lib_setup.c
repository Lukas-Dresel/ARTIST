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
 
#ifdef __cplusplus
extern "C"
{
#endif

#include "lib_setup.h"
#include "../../../../libUtility/src/main/jni/system_info.h"
#include "../../../../libHooking/src/main/jni/trappoint_interface.h"
#include "../../../../libHooking/src/main/jni/self_patching_trappoint.h"

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





