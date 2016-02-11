//
// Created by Lukas on 2/10/2016.
//

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "memory_map_lookup.h"
#include "logging.h"
#include "util/error.h"
#include "elf.h"


/*
bool findFileInMemory(void** result_base, void** result_end, const char* path)
{
    CHECK_RETURNFALSE(result_base != NULL);
    CHECK_RETURNFALSE(result_end != NULL);

    char buff[MAX_LINE_LENGTH];

    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
    void* file_base = 0;
    void* file_end = 0;
    char previous_name[MAX_LINE_LENGTH];
    previous_name[0] = 0;
    while (fgets(buff, sizeof(buff), fp) != NULL)
    {
        uint32_t start, end;
        char perms[10];
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
        if(strcmp(previous_name, filename) != 0 || file_end != (void*)start)
        {
            // New segment started

            if(strcmp(previous_name, path) == 0)
            {
                *result_base = file_base;
                *result_end = file_end;
                return true;
            }

            file_base = (void*)start;
            file_end = (void*)end;
            strncpy(previous_name, filename, MAX_LINE_LENGTH);
            int i = strlen(previous_name);
            while(i >= 0)
            {
                if(previous_name[i] == '\n' || previous_name[i] == '\0' ||
                   previous_name[i] == '\t' || previous_name[i] == ' ')
                {
                    previous_name[i--] = 0;
                    continue;
                }
                break;
            }
            continue;
        }
        else
        {
            // Still the same file, simply expand it
            file_end = (void*)end;
            continue;
        }
    }

    CHECK(ferror(fp) || feof(fp));
    if (ferror(fp))
    {
        set_last_error(strerror(errno));
    }
    fclose(fp);
    return false;
}*/

/*
 * Fills in the array of MemorySegment struct with information about all the segments in memory
 * containing a file mapping of the file specified by the given path. Fills up to
 * max_result_entries elements in the specified array, and returns the absolute number of elements
 * filled.
 */
uint32_t findFileSegmentsInMemory(struct MemorySegment* result_array, uint32_t max_result_entries, const char* path)
{
    CHECK_RETURN(result_array != NULL, 0);

    char buff[PROC_PID_MAPS_MAX_LINE_LENGTH];

    uint32_t path_length = strlen(path);

    uint32_t written_entries = 0;

    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
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
        if(written_entries >= max_result_entries)
        {
            // No more, quit this right now
            break;
        }
        // We can write one more.
        struct MemorySegment* towrite = &result_array[written_entries];
        towrite->start = (void*)start;
        towrite->end = (void*)end;
        towrite->flag_readable   = (perms[0] == 'r') ? true : false;
        towrite->flag_writable   = (perms[1] == 'w') ? true : false;
        towrite->flag_executable = (perms[2] == 'x') ? true : false;
        towrite->flag_shared     = (perms[3] == 's') ? true : false;
        written_entries++;
    }
    if (ferror(fp))
    {
        set_last_error(strerror(errno));
    }
    fclose(fp);
    return written_entries;
}


uint32_t findElfFileSegmentsInMemory(struct MemorySegment* result, uint32_t max_result_entries, const char* path)
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

        // We found the upper bound.

        bool foundElfHeader = false;
        for(uint32_t currseg = 0; currseg < found; currseg++)
        {
            if(!foundElfHeader && IsValidElfFileHeader(segs[currseg].start))
            if(IsValidElfFileHeader(segs[currseg].start))
            {
                for(uint32_t i = 1; i < max_result_entries; i++)
                {

                }
            }
        }
        break;
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
}