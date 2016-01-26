//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_RUNTIME_STATS_H
#define NDKTEST_ART_RUNTIME_STATS_H

#include <stdint.h>
#include "../util/macros.h"

// These must match the values in dalvik.system.VMDebug.
enum StatKinds {
    KIND_ALLOCATED_OBJECTS      = 1<<0,
    KIND_ALLOCATED_BYTES        = 1<<1,
    KIND_FREED_OBJECTS          = 1<<2,
    KIND_FREED_BYTES            = 1<<3,
    KIND_GC_INVOCATIONS         = 1<<4,
    KIND_CLASS_INIT_COUNT       = 1<<5,
    KIND_CLASS_INIT_TIME        = 1<<6,

    // These values exist for backward compatibility.
    KIND_EXT_ALLOCATED_OBJECTS = 1<<12,
    KIND_EXT_ALLOCATED_BYTES   = 1<<13,
    KIND_EXT_FREED_OBJECTS     = 1<<14,
    KIND_EXT_FREED_BYTES       = 1<<15,

    KIND_GLOBAL_ALLOCATED_OBJECTS   = KIND_ALLOCATED_OBJECTS,
    KIND_GLOBAL_ALLOCATED_BYTES     = KIND_ALLOCATED_BYTES,
    KIND_GLOBAL_FREED_OBJECTS       = KIND_FREED_OBJECTS,
    KIND_GLOBAL_FREED_BYTES         = KIND_FREED_BYTES,
    KIND_GLOBAL_GC_INVOCATIONS      = KIND_GC_INVOCATIONS,
    KIND_GLOBAL_CLASS_INIT_COUNT    = KIND_CLASS_INIT_COUNT,
    KIND_GLOBAL_CLASS_INIT_TIME     = KIND_CLASS_INIT_TIME,

    KIND_THREAD_ALLOCATED_OBJECTS   = KIND_ALLOCATED_OBJECTS << 16,
    KIND_THREAD_ALLOCATED_BYTES     = KIND_ALLOCATED_BYTES << 16,
    KIND_THREAD_FREED_OBJECTS       = KIND_FREED_OBJECTS << 16,
    KIND_THREAD_FREED_BYTES         = KIND_FREED_BYTES << 16,

    KIND_THREAD_GC_INVOCATIONS      = KIND_GC_INVOCATIONS << 16,

    // TODO: failedAllocCount, failedAllocSize
};

/*
 * Memory allocation profiler state.  This is used both globally and
 * per-thread.
 */
struct PACKED(4) RuntimeStats {

// Number of objects allocated.
uint64_t allocated_objects;
// Cumulative size of all objects allocated.
uint64_t allocated_bytes;

// Number of objects freed.
uint64_t freed_objects;
// Cumulative size of all freed objects.
uint64_t freed_bytes;

// Number of times an allocation triggered a GC.
uint64_t gc_for_alloc_count;

// Number of initialized classes.
uint64_t class_init_count;
// Cumulative time spent in class initialization.
uint64_t class_init_time_ns;
};

#endif //NDKTEST_ART_RUNTIME_STATS_H
