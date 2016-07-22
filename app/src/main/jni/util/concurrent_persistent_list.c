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

#include <assert.h>
#include "concurrent_persistent_list.h"
#include "../logging.h"
#include "memory.h"


static concurrent_persistent_list_entry* new_entry()
{
    void* mem = allocate_memory_chunk(sizeof(concurrent_persistent_list_entry));
    assert(mem != NULL); // if this fails we have a serious problem anyway, so quit
    return (concurrent_persistent_list_entry*)mem;
}

bool concurrent_persistent_list_try_create(concurrent_persistent_list_entry** list_ptr, void* elem)
{
    CHECK(list_ptr != NULL);
    if(*list_ptr != NULL)
    {
        return false;
    }

    concurrent_persistent_list_entry *new_start = new_entry();
    new_start->element = elem;
    new_start->next = new_start;

    if(!__sync_bool_compare_and_swap(list_ptr, NULL, new_start))
    {
        free_memory_chunk(new_start);
        return false;
    }
    return true;
}

concurrent_persistent_list_entry* concurrent_persistent_list_insert_after(
                                        concurrent_persistent_list_entry* entry_after,
                                        void* new_element)
{
    CHECK(entry_after != NULL);

    concurrent_persistent_list_entry* insert_after = entry_after;
    concurrent_persistent_list_entry* new_ent = new_entry();
    new_ent->element = new_element;

    do // wait until non-blocking sync worked out correctly
    {
        new_ent->next = insert_after->next;
    }
    while(__sync_bool_compare_and_swap(&insert_after->next, new_ent->next, new_ent));

    return new_ent;
}

/*
 * This iteration is possible because we made the assumption that entries can never be deleted
 * (see header file). Otherwise we would have to somehow lock the list to avoid getting stuck in
 * removed entries.
 */

void    concurrent_persistent_list_iterate(concurrent_persistent_list_entry *list,
                                           CONCURRENT_PERSISTENT_LIST_ITERATION_CALLBACK callback,
                                           void *args)
{
    CHECK(list != NULL);
    CHECK(callback != NULL);

    int index = 0;
    concurrent_persistent_list_entry* current_entry = list->next;
    while(current_entry != list)
    {
        callback(index++, current_entry, current_entry->element, args);
        current_entry = current_entry->next;
    }
}