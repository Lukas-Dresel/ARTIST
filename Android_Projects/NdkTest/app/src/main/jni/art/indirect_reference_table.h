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
