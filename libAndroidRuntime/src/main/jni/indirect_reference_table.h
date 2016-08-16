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

#ifndef NDKTEST_ART_INDIRECT_REFERENCE_TABLE_H
#define NDKTEST_ART_INDIRECT_REFERENCE_TABLE_H

#include <stdint.h>

union IRTSegmentState
{
    uint32_t          all;
    struct
    {
        uint32_t      topIndex:16;            /* index of first unused entry */
        uint32_t      numHoles:16;            /* #of holes in entire table */
    } parts;
};
enum IndirectRefKind
{
    kHandleScopeOrInvalid = 0,  // <<stack indirect reference table or invalid reference>>
    kLocal         = 1,  // <<local reference>>
    kGlobal        = 2,  // <<global reference>>
    kWeakGlobal    = 3   // <<weak global reference>>
};

struct IndirectReferenceTable
{
    /* semi-public - read/write by jni down calls */
    union IRTSegmentState segment_state_;

    // Mem map where we store the indirect refs.
    void* table_mem_map_; // Used to be std::unique_ptr<MemMap>
    // bottom of the stack. Do not directly access the object references
    // in this as they are roots. Use Get() that has a read barrier.
    void* table_; // used to be IrtEntry
    /* bit mask, ORed into all irefs */
    const enum IndirectRefKind kind_;
    /* max #of entries allowed */
    const size_t max_entries_;
};

#endif //NDKTEST_ART_INDIRECT_REFERENCE_TABLE_H
