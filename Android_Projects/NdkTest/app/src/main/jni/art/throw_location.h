/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_RUNTIME_THROW_LOCATION_H_
#define ART_RUNTIME_THROW_LOCATION_H_

#include <stdint.h>
#include "../util/macros.h"

struct PACKED(4) ThrowLocation {
  // The 'this' reference of the throwing method.
  void* this_object_;
  // The throwing method.
  void* method_;
  // The instruction within the throwing method.
  uint32_t dex_pc_;
  // Ensure 8byte alignment on 64bit.
#ifdef __LP64__
  uint32_t pad_;
#endif
};

#endif  // ART_RUNTIME_THROW_LOCATION_H_
