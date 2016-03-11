//
// Created by Lukas on 2/10/2016.
//

#ifndef NDKTEST_MEMORY_MAP_LOOKUP_H
#define NDKTEST_MEMORY_MAP_LOOKUP_H

#include <stdbool.h>
#include <stdint.h>
#include "util/list.h"

// TODO This entire class may still contain memory leaks. Not well checked yet.

struct MemorySegment
{
    struct list_head view_list_segments_entry;
    struct list_head file_list_segments_entry;
    struct list_head region_list_segments_entry;

    void* start;
    void* end;
    bool flag_readable;
    bool flag_writable;
    bool flag_executable;
    bool flag_shared;
    struct FilePath* path;
};
struct FilePath
{
    struct list_head view_list_filepaths_entry;

    unsigned long path_hash;
    char path[0];
};
struct MemoryRegion
{
    struct list_head file_list_regions_entry;
    struct list_head list_segments;
    uint32_t num_segments;
    void* start;
    void* end;
};
struct MemoryMappedFile
{
    struct list_head view_list_files_entry;

    struct list_head list_segments;
    struct list_head list_regions;
    uint32_t         num_segments;
    uint32_t         num_regions;
    struct FilePath* path;
};
struct MemoryMapView
{
    struct list_head list_filepaths;
    struct list_head list_files;
    struct list_head list_segments;

    uint32_t numFoundFiles;
    uint32_t numFoundSegments;
};

typedef bool (*REGION_PREDICATE)(void* start, void* end, void* arg);
typedef bool (*FILE_PREDICATE)(struct MemoryMapView *, struct MemoryMappedFile *, void* arg);
typedef bool (*FILEPATH_PREDICATE)(struct MemoryMapView *, struct FilePath*, void* arg);
typedef bool (*SEGMENT_PREDICATE)(struct MemoryMapView *, struct MemorySegment*, void* arg);


// djb2 by dan bernstein
// Source: http://www.cse.yorku.ca/~oz/hash.html
unsigned long StringHash(const char *string);

bool FilePathByPathStringPredicate(struct MemoryMapView * view, struct FilePath* fp, const char* arg);
bool FileByPathStringPredicate(struct MemoryMapView * view, struct MemoryMappedFile * f, const char* arg);
bool FileByFilePathPredicate(struct MemoryMapView * view, struct MemoryMappedFile * f, const struct FilePath* fp);
bool FileIsElfOatFilePredicate(struct MemoryMapView * view, struct MemoryMappedFile * f, void* unused);
bool MemorySegmentByAddressPredicate(struct MemoryMapView * view, struct MemorySegment* seg, void* addressInQuestion);

struct MemoryMappedFile* findFileByPredicate(struct MemoryMapView * view, FILE_PREDICATE pred, void* arg);
struct MemoryMappedFile* findFileByPath(struct MemoryMapView * view, char* path);
struct MemorySegment* findMemorySegmentByPredicate(struct MemoryMapView * view, SEGMENT_PREDICATE pred, void* arg);
struct MemoryRegion * findRegionByPredicate(struct MemoryMappedFile * f, REGION_PREDICATE pred, void* arg);

bool extractElfOatPointersFromFile(struct MemoryMappedFile *f, void **result_elf_start,
                                   void **result_oat_start, void **result_oat_end);

struct MemoryMapView *  CreateMemoryMapView();
void                    DestroyMemoryMapView(struct MemoryMapView *self);

void logMemorySegmentContents(struct MemorySegment* seg);
void logMemoryRegionContents(struct MemoryRegion *r);
void logFileContents(struct MemoryMapView * view, struct MemoryMappedFile * f);




/*
typedef bool IS_START_SEGMENT(struct MemorySegment* current, char* curr_path, void* args);
typedef bool IS_CONTINUATION_SEGMENT(struct MemorySegment* current, struct MemorySegment* previous, char* curr_path, void* args);

// bool findFileInMemory(void** result_base, void** result_end, const char* path);

bool findSpecialFileInMemory(void** result_base, void** result_end,
                             IS_START_SEGMENT isStartSegment,
                             IS_CONTINUATION_SEGMENT isContinuationSegment,
                             void* startArgs, void* contArgs);

bool findElfFile(struct MemoryMappedFile* result, const char* path);
bool findOatFile(struct MemoryMappedFile* result, const char* path);
/*
 * Fills in the array of MemorySegment struct with information about all the segments in memory
 * containing a file mapping of the file specified by the given path. Fills up to
 * max_result_entries elements in the specified array, and returns the absolute number of elements
 * filled.

uint32_t findFileSegmentsInMemory(struct MemorySegment* result_array, uint32_t max_result_entries, const char* path);

void logFileSegmentsInMemory(const char* path); */

#endif //NDKTEST_MEMORY_MAP_LOOKUP_H
