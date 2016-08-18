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
 

#include "memory_map_lookup.h"

#include <stdio.h>
#include <errno.h>
#include <utility/error.h>
#include <utility/logging.h>
#include <utility/memory.h>
#include <utility/city.h>
#include <art/oat_internal.h>

#include "elf.h"

static size_t whitespaceFreeStringLength(const char* str)
{
    CHECK(str != NULL);

    size_t len = strlen(str);
    while (len > 0)
    {
        switch(str[len - 1])
        {
            case '\n':
            case ' ':
            case '\t':
            case '\0':
                len --;
                break;
            default:
                return len;
        }
    }
    return len;
}
static size_t newLineFreeStringLength(const char* str)
{
    CHECK(str != NULL);
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n'))
    {
        len -= 1;
    }
    return len;
}

bool FilePathByPathStringPredicate(struct MemoryMapView * view, struct FilePath* fp, const char* arg)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(fp != NULL);
    CHECK_RETURNFALSE(arg != NULL);

    if(fp->path_hash != CityHash64(arg, strlen(arg)))
    {
        return false;
    }
    return strcmp(arg, fp->path) == 0;
}
bool FileByPathStringPredicate(struct MemoryMapView * view, struct MemoryMappedFile * f, const char* arg)
{
    CHECK_RETURNFALSE(f != NULL);

    return FilePathByPathStringPredicate(view, f->path, arg);
}
bool FileByFilePathPredicate(struct MemoryMapView * view, struct MemoryMappedFile * f, const struct FilePath* fp)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(f != NULL);
    CHECK_RETURNFALSE(fp != NULL);


    return f->path == fp;
}
bool FileIsElfOatFilePredicate(struct MemoryMapView * view, struct MemoryMappedFile * f, void* unused)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(f != NULL);

    void* ignored;
    return extractElfOatPointersFromFile(f, &ignored, &ignored, &ignored);
}
bool MemorySegmentByAddressPredicate(struct MemoryMapView * view, struct MemorySegment* seg, void* addressInQuestion)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(seg != NULL);

    return (addressInQuestion >= seg->start && addressInQuestion < seg->end);
}

struct MemoryMappedFile * findFileByPredicate(struct MemoryMapView * view, FILE_PREDICATE pred, void* arg)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(pred != NULL);

    struct MemoryMappedFile * currentFile;
    list_for_each_entry(currentFile, &view->list_files, view_list_files_entry)
    {
        if(pred(view, currentFile, arg))
        {
            return currentFile;
        }
    }
    return NULL;
}
struct MemoryMappedFile * findFileByPath(struct MemoryMapView * view, char* path)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(path != NULL);

    return findFileByPredicate(view, FileByPathStringPredicate, path);
}
struct MemorySegment* findMemorySegmentByPredicate(struct MemoryMapView * view, SEGMENT_PREDICATE pred, void* arg)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(pred != NULL);

    struct MemorySegment* currentSegment;
    list_for_each_entry(currentSegment, &view->list_segments, view_list_segments_entry)
    {
        if(pred(view, currentSegment, arg))
        {
            return currentSegment;
        }
    }
    return NULL;
}
struct MemoryRegion * findRegionByPredicate(struct MemoryMappedFile * f, REGION_PREDICATE pred, void* arg)
{
    CHECK_RETURNFALSE(f != NULL);
    CHECK_RETURNFALSE(pred != NULL);

    struct MemoryRegion * region;
    list_for_each_entry(region, &f->list_regions, file_list_regions_entry)
    {
        if(pred(region->start, region->end, arg))
        {
            return region;
        }
    }
    return NULL;
}
bool extractElfOatPointersFromFile(struct MemoryMappedFile *f, void **result_elf_start,
                                   void **result_oat_start, void **result_oat_end)
{
    CHECK_RETURNFALSE(result_elf_start != NULL);
    CHECK_RETURNFALSE(result_oat_start != NULL);
    CHECK_RETURNFALSE(result_oat_end != NULL);

    struct MemoryRegion * region;
    list_for_each_entry(region, &f->list_regions, file_list_regions_entry)
    {
        if(!IsValidElfFileHeader(region->start))
        {
            continue;
        }
        void* expected_oat = region->start + 0x1000;
        if(region->end - expected_oat < sizeof(struct OatHeader))
        {
            continue;
        }
        if(!IsValidOatHeader(expected_oat))
        {
            continue;
        }
        *result_elf_start = region->start;
        *result_oat_start = expected_oat;
        *result_oat_end = region->end;
        return true;
    }
    return false;
}

static struct FilePath* newFilePath(const char* path)
{
    unsigned int path_len = strlen(path);
    uint64_t path_hash = CityHash64(path, path_len);

    // the struct + the following string + null byte
    struct FilePath* self = allocate_memory_chunk(sizeof(struct FilePath) + path_len + 1);
    if(self == NULL)
    {
        LOGE("Could not allocate memory for FilePath struct: %s", strerror(errno));
        set_last_error("Error allocating memory for FilePath struct.");
        return NULL;
    }
    self->path_hash = path_hash;
    // Copy over the path together with the null byte
    strncpy(&self->path[0], path, path_len + 1);
    INIT_LIST_HEAD(&self->view_list_filepaths_entry);
    return self;
}

static struct FilePath* addOrFindFilePath(struct MemoryMapView * view, const char* path)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(path != NULL);

    uint64_t path_hash = CityHash64(path, strlen(path));

    // find hash in path list
    struct FilePath* current_path;
    list_for_each_entry(current_path, &view->list_filepaths, view_list_filepaths_entry)
    {
        if(current_path->path_hash == path_hash && strcmp(current_path->path, path) == 0)
        {
            return current_path;
        }
    }
    struct FilePath* new_path = newFilePath(path);
    if(new_path == NULL)
    {
        return NULL;
    }
    // link it into the view
    list_add(&new_path->view_list_filepaths_entry, &view->list_filepaths);
    return new_path;
}

static struct MemoryMappedFile* newFile(struct FilePath* fp)
{
    struct MemoryMappedFile * self = allocate_memory_chunk(sizeof(struct MemoryMappedFile));
    if(self == NULL)
    {
        LOGE("Could not allocate memory for MemoryMappedFile struct: %s", strerror(errno));
        set_last_error("Error allocating memory for MemoryMappedFile struct.");
        return NULL;
    }
    INIT_LIST_HEAD(&self->list_segments);
    INIT_LIST_HEAD(&self->list_regions);
    INIT_LIST_HEAD(&self->view_list_files_entry);
    self->path = fp;
    self->num_regions = 0;
    self->num_segments = 0;
    return self;
}
static struct MemoryMappedFile * addOrFindFile(struct MemoryMapView * view, struct FilePath* path)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(path != NULL);

    struct MemoryMappedFile * found_file = findFileByPredicate(view, FileByFilePathPredicate, path);
    if(found_file != NULL)
    {
        return found_file;
    }
    struct MemoryMappedFile* new_file = newFile(path);
    if(new_file == NULL)
    {
        return NULL;
    }
    list_add(&new_file->view_list_files_entry, &view->list_files);
    view->numFoundFiles++;
    return new_file;
}

static struct MemoryRegion* newRegion(struct MemorySegment* seg)
{
    struct MemoryRegion * new_self = allocate_memory_chunk(sizeof(struct MemoryRegion));
    if(new_self == NULL)
    {
        LOGE("Could not allocate memory for MemoryRegion struct: %s", strerror(errno));
        set_last_error("Error allocating memory for MemoryRegion struct.");
        return NULL;
    }
    new_self->start = seg->start;
    new_self->end = seg->end;
    new_self->num_segments = 0;
    INIT_LIST_HEAD(&new_self->list_segments);
    INIT_LIST_HEAD(&new_self->file_list_regions_entry);
    return new_self;
}

static struct MemoryRegion * addOrExpandRegion(struct MemoryMappedFile * f, struct MemorySegment* seg)
{
    CHECK_RETURNFALSE(f != NULL);
    CHECK_RETURNFALSE(seg != NULL);

    if(!seg->flag_readable)
    {
        // unreadable segments aren't considered for file mappings as they cannot be executed
        // and checks on these addresses would segfault
        return NULL;
    }

    if(f->num_regions > 0)
    {
        struct MemoryRegion * most_recent = list_entry(f->list_regions.next, struct MemoryRegion, file_list_regions_entry);
        if(most_recent->end == seg->start)
        {
            most_recent->end = seg->end;
            list_add_tail(&seg->region_list_segments_entry, &most_recent->list_segments);
            return most_recent;
        }
    }
    struct MemoryRegion* new_self = newRegion(seg);
    if(new_self == NULL)
    {
        return NULL;
    }
    list_add_tail(&seg->region_list_segments_entry, &new_self->list_segments);
    list_add(&new_self->file_list_regions_entry, &f->list_regions);
    f->num_regions++;
    return new_self;
}
static struct MemorySegment* newSegment(uint32_t start, uint32_t end, char* perms)
{
    struct MemorySegment* self = allocate_memory_chunk(sizeof(struct MemorySegment));
    if(self == NULL)
    {
        set_last_error("Allocating memory for MemorySegment structure failed.");
        LOGE("Failed to allocate memory for MemorySegment structure: %s", strerror(errno));
        return NULL;
    }
    self->start = (void *) start;
    self->end = (void *) end;
    self->flag_readable = (perms[0] == 'r') ? true : false;
    self->flag_writable = (perms[1] == 'w') ? true : false;
    self->flag_executable = (perms[2] == 'x') ? true : false;
    self->flag_shared = (perms[3] == 's') ? true : false;
    self->path = NULL;
    INIT_LIST_HEAD(&self->file_list_segments_entry);
    INIT_LIST_HEAD(&self->region_list_segments_entry);
    INIT_LIST_HEAD(&self->view_list_segments_entry);
    return self;
}
static bool insertNewSegment(struct MemoryMapView *view, const char *line)
{
    CHECK_RETURNFALSE(view != NULL);
    CHECK_RETURNFALSE(line != NULL);

    uint32_t start, end;
    char perms[4];
    uint32_t page_offset;
    uint32_t dev_major, dev_minor;
    long int inode_number;

    int path_start;

    // Sadly the behavior of the return value of sscanf seems to be inconsistent, so the only
    // assumption we can reasonably make is the length being >= than the parameters before it.
    if (sscanf(line, "%x-%x %4c %x %x:%x %lu %n", &start, &end, &perms[0], &page_offset,
               &dev_major, &dev_minor, &inode_number, &path_start) < 7)
    {
        LOGE("Could not completely parse the line of /proc/self/maps: %s [Error: %s]", line, strerror(errno));
        return false;
    }
    const char *path = &line[path_start];
    struct FilePath *file_path_element = addOrFindFilePath(view, path);
    if (file_path_element == NULL)
    {
        return false;
    }

    struct MemoryMappedFile *file_element = addOrFindFile(view, file_path_element);
    if (file_element == NULL)
    {
        return false;
    }

    struct MemorySegment* self = newSegment(start, end, perms);
    if(self == NULL)
    {
        return NULL;
    }
    struct MemoryRegion* region = addOrExpandRegion(file_element, self);
    if(region != NULL) // the region can be null, if the segment isn't mapped readable
    {
        list_add_tail(&self->region_list_segments_entry, &region->list_segments);
        region->num_segments++;
    }

    list_add_tail(&self->file_list_segments_entry, &file_element->list_segments);
    file_element->num_segments++;

    list_add(&self->view_list_segments_entry, &view->list_segments);
    view->numFoundSegments++;

    return true;
}

static bool populateMemoryMappedFileView(struct MemoryMapView * self)
{
    CHECK_RETURNFALSE(self != NULL);

    char buff[PROC_PID_MAPS_MAX_LINE_LENGTH];

    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
    if(fp == NULL)
    {
        set_last_error("Could not open /proc/self/maps, what the hell?");
        return false;
    }

    while (fgets(buff, sizeof(buff), fp) != NULL)
    {
        uint32_t actual_len = newLineFreeStringLength(buff);
        buff[actual_len] = 0; // clear trailing newlines
        if(!insertNewSegment(self, buff))
        {
            return false;
        }
    }
    CHECK(ferror(fp) || feof(fp));
    if (ferror(fp))
    {
        set_last_error(strerror(errno));
    }
    fclose(fp);
    return true;
}


void DestroyMemoryMapView(struct MemoryMapView *self)
{
    if(self == NULL)
    {
        return;
    }
    while(!list_empty(&self->list_filepaths))
    {
        struct FilePath* fp = list_entry(self->list_filepaths.next, struct FilePath, view_list_filepaths_entry);
        // Remove from the filepaths list in the view
        list_del(&fp->view_list_filepaths_entry);
        free_memory_chunk(fp);
    }
    while(!list_empty(&self->list_segments))
    {
        struct MemorySegment* seg = list_entry(self->list_segments.next, struct MemorySegment, view_list_segments_entry);
        // Remove from the segments list in the view
        list_del(&seg->view_list_segments_entry);
        // And the segments list in files
        list_del(&seg->file_list_segments_entry);
        // Don't need to free the FilePath pointer content, it was cleared in the previous loop
        free_memory_chunk(seg);
        self->numFoundSegments--;
    }
    while(!list_empty(&self->list_files))
    {
        struct MemoryMappedFile * f = list_entry(self->list_files.next, struct MemoryMappedFile, view_list_files_entry);
        // Don't need to clear the list_files, should be empty from the previous loop
        CHECK(list_empty(&f->list_segments));
        list_del(&f->view_list_files_entry);
        // Not clearing list_segments here because we don't need to. It's all gonna be freed anyway
        free_memory_chunk(f);
        self->numFoundFiles--;
    }

    CHECK(list_empty(&self->list_filepaths));
    CHECK(list_empty(&self->list_segments));
    CHECK(list_empty(&self->list_files));
    CHECK(self->numFoundSegments == 0);
    CHECK(self->numFoundFiles == 0);
    free_memory_chunk(self);
    return;
}

struct MemoryMapView *CreateMemoryMapView()
{
    struct MemoryMapView * self = allocate_memory_chunk(sizeof(struct MemoryMapView));
    if(self == NULL)
    {
        set_last_error("Error allocating memory for MemoryMapView structure.");
        LOGE("Could not allocate MemoryMapView: %s", strerror(errno));
        return NULL;
    }
    INIT_LIST_HEAD(&self->list_filepaths);
    INIT_LIST_HEAD(&self->list_files);
    INIT_LIST_HEAD(&self->list_segments);
    self->numFoundFiles = 0;
    self->numFoundSegments = 0;

    if(populateMemoryMappedFileView(self))
    {
        return self;
    }
    DestroyMemoryMapView(self);
    return NULL;
}

void logMemorySegmentContents(struct MemorySegment* seg)
{
    if(seg == NULL)
    {
        return;
    }
    LOGD(PRINT_PTR"-"PRINT_PTR", %c%c%c%c",
         (uintptr_t)seg->start, (uintptr_t)seg->end,
         seg->flag_readable ? 'r' : '-',
         seg->flag_writable ? 'w' : '-',
         seg->flag_executable ? 'x' : '-',
         seg->flag_shared ? 's' : 'p');
}
void logMemoryRegionContents(struct MemoryRegion *r)
{
    if(r == NULL)
    {
        return;
    }
    LOGD(PRINT_PTR"-"PRINT_PTR, (uintptr_t)r->start, (uintptr_t)r->end);
}
void logFileContents(struct MemoryMappedFile * f)
{
    if(f == NULL)
    {
        return;
    }
    CHECK_RETURNVOID(f != NULL);

    LOGD("Memory mapped file \"%s\":", f->path->path);

    LOGD("[%d Regions]", f->num_regions);
    struct MemoryRegion * region;
    list_for_each_entry(region, &f->list_regions, file_list_regions_entry)
    {
        logMemoryRegionContents(region);
    }

    LOGD("[%d Segments]", f->num_segments);
    struct MemorySegment* seg;
    list_for_each_entry(seg, &f->list_segments, file_list_segments_entry)
    {
        logMemorySegmentContents(seg);
    }
}

/*

static bool parseProcPIDMapsLine(struct MemorySegment* result, char ** result_path, char* line)
{
    uint32_t start, end;
    char perms[4];
    uint32_t page_offset;
    uint32_t dev_major, dev_minor;
    long int inode_number;

    int path_start;

    // Sadly the behavior of the return value of sscanf seems to be inconsistent, so the only
    // assumption we can reasonably make is the length being >= than the parameters before it.
    if(sscanf(line, "%x-%x %4c %x %x:%x %lu %n", &start, &end, &perms[0], &page_offset, &dev_major, &dev_minor, &inode_number, &path_start) < 7)
    {
        LOGE("Could not completely parse the line of /proc/self/maps: %s [Error: %s]", line, strerror(errno));
        return false;
    }
    *result_path = &line[path_start];

    result->start = (void*)start;
    result->end = (void*)end;
    result->flag_readable   = (perms[0] == 'r') ? true : false;
    result->flag_writable   = (perms[1] == 'w') ? true : false;
    result->flag_executable = (perms[2] == 'x') ? true : false;
    result->flag_shared     = (perms[3] == 's') ? true : false;
    return true;
}
bool findSpecialFileInMemory(void** result_base, void** result_end,
                             IS_START_SEGMENT isStartSegment,
                             IS_CONTINUATION_SEGMENT isContinuationSegment,
                             void* startArgs, void* contArgs)
{
    CHECK_RETURNFALSE(result_base != NULL);
    CHECK_RETURNFALSE(result_end != NULL);

    char buff[PROC_PID_MAPS_MAX_LINE_LENGTH];

    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
    if(fp == NULL)
    {
        set_last_error("Could not open /proc/self/maps, what the hell?");
        return false;
    }

    void* file_base = NULL;
    void* file_end = NULL;
    bool found = false;

    struct MemorySegment a;
    struct MemorySegment b;

    struct MemorySegment* current = &b;
    struct MemorySegment* previous = &a;
    while (fgets(buff, sizeof(buff), fp) != NULL)
    {
        char* filepath;
        // Sadly the behavior of the return value of sscanf seems to be inconsistent, so the only
        // assumption we can reasonably make is the length being >= than the parameters before it.
        if(!parseProcPIDMapsLine(current, &filepath, buff))
        {
            continue;
        }
        filepath[whitespaceFreeStringLength(filepath)] = 0; // clear trailing stuff

        if(found)
        {
            // The first loop should not be a problem here where previous isn't valid because
            // in the first iteration the variable found can not be set
            if(isContinuationSegment(current, previous, filepath, contArgs))
            {
                file_end = current->end;
            }
            else
            {
                // Found the last segment, return
                *result_base = file_base;
                *result_end = file_end;
                return true;
            }
        }
        else
        {
            if(isStartSegment(current, filepath, startArgs))
            {
                found = true;
                file_base = current->start;
                file_end = current->end;
            }
        }
        // Done with this line, swap active segments
        void* temp = current;
        current = previous;
        previous = temp;
    }
    CHECK(ferror(fp) || feof(fp));
    if (ferror(fp))
    {
        set_last_error(strerror(errno));
    }
    fclose(fp);
    return false;
}

static bool isNamedElfStart(struct MemorySegment* current, char* filename, char* target_path)
{
    if(!current->flag_readable)
    {
        // elf file segments must be readable to verify the elf header
        return false;
    }
    size_t f_len = whitespaceFreeStringLength(filename);
    size_t t_len = whitespaceFreeStringLength(target_path);
    if(f_len != t_len)
    {
        return false;
    }
    if(strncmp(filename, target_path, f_len) != 0)
    {
        return false;
    }

    if(!IsValidElfFileHeader(current->start))
    {
        return false;
    }
    return true;
}
static bool isNamedOatStart(struct MemorySegment* current, char* filename, char* target_path)
{
    if(!current->flag_readable)
    {
        // oat file segments must be readable to verify the oat header
        return false;
    }

    size_t f_len = whitespaceFreeStringLength(filename);
    size_t t_len = whitespaceFreeStringLength(target_path);
    if(f_len != t_len)
    {
        return false;
    }
    if(strncmp(filename, target_path, f_len) != 0)
    {
        return false;
    }
    if(!IsValidOatHeader(current->start))
    {
        return false;
    }
    return true;
}
static bool isNamedContinuation(struct MemorySegment* current, struct MemorySegment* previous, char* filename, char* target_path)
{
    if(current->start != previous->end)
    {
        return false;
    }
    if(strcmp(filename, target_path) != 0)
    {
        return false;
    }
    return true;
}

bool findElfFile(struct MemoryMappedFile* result, const char* path)
{
    void* arg = (void*)path;
    return findSpecialFileInMemory(&result->start, &result->end,
                                   isNamedElfStart, isNamedContinuation,
                                   arg, arg);
}
bool findOatFile(struct MemoryMappedFile* result, const char* path)
{
    void* arg = (void*)path;
    return findSpecialFileInMemory(&result->start, &result->end,
                                   isNamedOatStart, isNamedContinuation,
                                   arg, arg);
}
/*
 * Fills in the array of MemorySegment struct with information about all the segments in memory
 * containing a file mapping of the file specified by the given path. Fills up to
 * max_result_entries elements in the specified array, and returns the absolute number of elements
 * filled.

uint32_t findFileSegmentsInMemory(struct MemorySegment* result_array, uint32_t max_result_entries, const char* path)
{
    CHECK_RETURN(result_array != NULL, 0);

    char buff[PROC_PID_MAPS_MAX_LINE_LENGTH];

    uint32_t path_length = whitespaceFreeStringLength(path);

    uint32_t written_entries = 0;

    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
    struct MemorySegment current;
    while (written_entries < max_result_entries && fgets(buff, sizeof(buff), fp) != NULL)
    {
        const char* filename;
        // Sadly the behavior of the return value of sscanf seems to be inconsistent, so the only
        // assumption we can reasonably make is the length being >= than the parameters before it.
        if(!parseProcPIDMapsLine(&current, &filename, buff))
        {
            continue;
        }

        size_t comparison_length = whitespaceFreeStringLength(path);
        if(comparison_length != path_length || strncmp(filename, path, comparison_length) != 0)
        {
            continue;
        }
        // We can write one more.
        memcpy(&result_array[written_entries], &current, sizeof(struct MemorySegment));
        written_entries++;
    }
    if (ferror(fp))
    {
        set_last_error(strerror(errno));
    }
    fclose(fp);
    return written_entries;
}


/*int32_t findElfFileSegmentsInMemory(struct MemorySegment* result, uint32_t max_result_entries, const char* path)
{
    uint32_t num_segments = 50;
    while(true)
    {
        struct MemorySegment segs[num_segments];
        uint32_t found = findFileSegmentsInMemory(segs, num_segments, path);

        if(found >= num_segments)
        {
            // try more
            num_segments *= 2;
            continue;
        }


        int32_t elf_start_index = -1;
        int32_t elf_last_index = -1;

        int32_t index = 0;

        // We found the right amount of entries
        while( index < found && !IsValidElfFileHeader(segs[index].start))
        {
            index++;
        }
        if(index == found)
        {
            // Could not find an Elf Header
            return 0;
        }
        elf_start_index = index++;

        // Found the elf header -> Start chaining together adjacent segments
        while(index < found && (segs[index - 1].end == segs[index].start))
        {
            index++;
        }
        elf_last_index = index - 1;

        for(int i = elf_start_index; i <= elf_last_index; i++)
        {
            memcpy(&result[i - elf_start_index], &segs[i], sizeof(struct MemorySegment));
        }
        return (elf_last_index + 1) - elf_start_index;
    }
}

bool findElfFileInMemory(void** result_ptr_start, void** result_ptr_end, const char* path)
{
    uint32_t num_segments = 50;
    while(true)
    {
        struct MemorySegment segs[num_segments];
        uint32_t found = findFileSegmentsInMemory(segs, num_segments, path);

        if(found >= num_segments)
        {
            // try more
            num_segments *= 2;
            continue;
        }


        int32_t elf_start_index = -1;
        int32_t elf_last_index = -1;

        int32_t index = 0;

        // We found the right amount of entries
        while( index < found && !IsValidElfFileHeader(segs[index].start))
        {
            index++;
        }
        if(index == found)
        {
            // Could not find an Elf Header
            return 0;
        }
        elf_start_index = index++;

        // Found the elf header -> Start chaining together adjacent segments
        while(index < found && (segs[index - 1].end == segs[index].start))
        {
            index++;
        }
        elf_last_index = index - 1;

        for(int i = elf_start_index; i <= elf_last_index; i++)
        {
            memcpy(&result[i - elf_start_index], &segs[i], sizeof(struct MemorySegment));
        }
        return (elf_last_index + 1) - elf_start_index;
    }
}

void logFileSegmentsInMemory(const char* path)
{
    char buff[PROC_PID_MAPS_MAX_LINE_LENGTH];

    uint32_t path_length = strlen(path);

    uint32_t written_entries = 0;

    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
    LOGD("Segments for file \"%s\":", path);
    while (fgets(buff, sizeof(buff), fp) != NULL)
    {
        uint32_t start, end;
        char perms[4];
        uint32_t page_offset;
        uint32_t dev_major, dev_minor;
        long int inode_number;

        int path_start;

        // Sadly the behavior of the return value of sscanf seems to be inconsistent, so the only
        // assumption we can reasonably make is the length being >= than the parameters before it.
        if(sscanf(buff, "%x-%x %4c %x %x:%x %lu %n", &start, &end, &perms[0], &page_offset, &dev_major, &dev_minor, &inode_number, &path_start) < 7)
        {
            LOGE("Could not completely parse the line of /proc/self/maps: %s [Error: %s]", buff, strerror(errno));
            continue;
        }

        char* filename = &buff[path_start];
        if(strncmp(filename, path, path_length) != 0 )
        {
            continue;
        }
        // We found a segment that starts with the correct path, check if the only thing
        // following is a newline. This would mean a perfect match.
        if(filename[path_length] != '\n' || filename[path_length + 1] != 0)
        {
            // IMPOSTER!
            continue;
        }
        LOGD("%s", buff);
    }
    if (ferror(fp))
    {
        set_last_error(strerror(errno));
    }
    fclose(fp);
    return written_entries;
}*/