//
// Created by Lukas on 2/10/2016.
//

#ifndef NDKTEST_MEMORY_MAP_LOOKUP_H
#define NDKTEST_MEMORY_MAP_LOOKUP_H

#include <stdbool.h>

struct MemorySegment
{
    void* start;
    void* end;
    bool flag_readable;
    bool flag_writable;
    bool flag_executable;
    bool flag_shared;
};

// bool findFileInMemory(void** result_base, void** result_end, const char* path);

/*
 * Fills in the array of MemorySegment struct with information about all the segments in memory
 * containing a file mapping of the file specified by the given path. Fills up to
 * max_result_entries elements in the specified array, and returns the absolute number of elements
 * filled.
 */
uint32_t findFileSegmentsInMemory(struct MemorySegment* result_array, uint32_t max_result_entries, const char* path);

void logFileSegmentsInMemory(const char* path);

#endif //NDKTEST_MEMORY_MAP_LOOKUP_H
