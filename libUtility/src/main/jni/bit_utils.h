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
 
#ifndef NDKTEST_BIT_UTILS_H
#define NDKTEST_BIT_UTILS_H

#define COUNT_32BIT_SET_BITS(x) __builtin_popcount(x)
#define COUNT_64BIT_SET_BITS(x) __builtin_popcountll(x)

#define COUNT_32BIT_LEADING_ZEROS(x) __builtin_clz(x)
#define COUNT_64BIT_LEADING_ZEROS(x) __builtin_clzll(x)

#define COUNT_32BIT_TAILING_ZEROS(x) __builtin_ctz(x)
#define COUNT_64BIT_TAILING_ZEROS(x) __builtin_ctzll(x)

#endif //NDKTEST_BIT_UTILS_H
