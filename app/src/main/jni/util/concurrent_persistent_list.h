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

#ifndef NDKTEST_CONCURRENT_PERSISTENT_LIST_H
#define NDKTEST_CONCURRENT_PERSISTENT_LIST_H

#include <stdbool.h>

/*
 * Implementation of a circular inked list that is concurrently accessible for the use in
 * trappoints, breakpoints, etc. This list relies on assumptions intrinsic to these, therefore
 * caution is required when attempting to use this implementation in a different context.
 *
 * The important requirements for this list implementation are:
 *
 *  1.  Elements can NEVER be removed. The list can only be destroyed as a whole.
 *  2.  Manipulation without using the provided functions must be synchronized independently
 *  3.  External manipulation with correct values should not break this lists internal
 *      synchronisation
 *  4.  This list implements non-blocking synchronisation
 *  5.  The last element has the next pointer as well as the element pointer set to null
 *  6.  No empty lists exist. Empty lists must instead be represented by NULL pointers.
 */

typedef struct concurrent_persistent_list_entry
{
    struct concurrent_persistent_list_entry* next;
    void*   element;
} concurrent_persistent_list_entry;

#define LIST(x) concurrent_persistent_list_##x

typedef void (*CONCURRENT_PERSISTENT_LIST_ITERATION_CALLBACK)(int index,
                                                   LIST(entry)* entry,
                                                   void* elem, void* args);


bool                    LIST(try_create)(LIST(entry)** list_ptr, void* elem);
struct LIST(entry)*     LIST(insert_after)(LIST(entry)* entry_after, void* new_element);

void                    LIST(iterate)(LIST(entry) *list,
                                      CONCURRENT_PERSISTENT_LIST_ITERATION_CALLBACK callback,
                                      void* args);

#undef LIST

#endif //NDKTEST_CONCURRENT_PERSISTENT_LIST_H
