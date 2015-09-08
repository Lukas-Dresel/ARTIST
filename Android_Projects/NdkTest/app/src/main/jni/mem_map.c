//
// Created by Lukas on 8/18/2015.
//
#include "mem_map.h"

#include <asm-generic/mman-common.h>
#include <sys/mman.h>

#include "system_info.h"
#include "util.h"

static bool mem_map_IsInUse(MemMap *self)
{
    return ((self->name_ != NULL) ||
            (self->begin_ != NULL) ||
            (self->size_ != 0) ||
            (self->base_begin_ != NULL) ||
            (self->base_size_ != 0) ||
            (self->prot_ != 0) ||
            (self->reuse_));
}

static bool mem_map_SeemsValid(MemMap *self)
{
    long pageSize = getSystemPageSize();
    byte *expected_base_begin = get_page_base(self->begin_);
    int page_offset = (int) (self->begin_ - expected_base_begin);
    size_t expected_base_size = (size_t) round_up(self->size_ + page_offset, (uint64_t) pageSize);

    if (expected_base_begin != self->base_begin_)
    {
        return false;
    }
    if (expected_base_size != self->base_size_)
    {
        return false;
    }
    return true;
}

bool mem_map_ForceClear(MemMap *self)
{
    self->name_ = NULL;
    self->begin_ = NULL;
    self->size_ = 0;
    self->base_begin_ = NULL;
    self->base_size_ = 0;
    self->prot_ = 0;
    self->reuse_ = false;
}

bool mem_map_Initialize(MemMap *self)
{
    CHECK(!mem_map_SeemsValid(self));

    return mem_map_ForceClear(self);
}

static bool CheckMapRequest(byte *expected_ptr, void *actual_ptr, size_t byte_count)
{
    // Handled first by caller for more specific error messages.
    CHECK(actual_ptr != MAP_FAILED);
    if (expected_ptr == NULL)
    {
        return true;
    }
    uintptr_t actual = (uintptr_t) (actual_ptr);
    uintptr_t expected = (uintptr_t) (expected_ptr);
    uintptr_t limit = expected + byte_count;
    if (expected_ptr == actual_ptr)
    {
        return true;
    }
    // We asked for an address but didn't get what we wanted, all paths below here should fail.
    int result = munmap(actual_ptr, byte_count);
    if (result == -1)
    {
        LOGW("munmap(%p, %zd) failed", actual_ptr, byte_count);
    }
    // We call this here so that we can try and generate a full error
    // message with the overlapping mapping. There's no guarantee that
    // that there will be an overlap though, since
    // - The kernel is not *required* to honour expected_ptr unless MAP_FIXED is
    //   true, even if there is no overlap
    // - There might have been an overlap at the point of mmap, but the
    //   overlapping region has since been unmapped.
    LOGF("Failed to mmap at expected address, mapped at "
                 "0x%08"
                 PRIxPTR
                 " instead of 0x%08"
                 PRIxPTR,
         actual, expected);
    LOGW("Check the following /proc/self/maps contents for overlaps. But keep in mind that there are other reasons for this to fail.");
    dump_process_memory_map();

    return false;
}

bool mem_map_MapFileAtAddress(MemMap *self, byte *expected_ptr, size_t byte_count,
                              int prot, int flags, int fd, off_t start, bool reuse,
                              const char *filename)
{
    if (UNLIKELY(mem_map_IsInUse(self)))
    {
        if (mem_map_SeemsValid(self))
        {
            LOGF("Called MapFileAtAddress with a mapping that seems like it was used and not destroyed. This will cause memory leaks and is not allowed.");
        }
        else
        {
            LOGF("Called MapFileAtAddress with a mapping that seems like it wasn't initialized. Make sure to call initialize before using new mappings.");
        }
        dump_process_memory_map();
        return false;
    }

    CHECK_NE(0, prot);
    CHECK_NE(0, flags & (MAP_SHARED | MAP_PRIVATE));
    // Note that we do not allow MAP_FIXED unless reuse == true, i.e we
    // expect his mapping to be contained within an existing map.
    if (reuse)
    {
        // reuse means it is okay that it overlaps an existing page mapping.
        // Only use this if you actually made the page reservation yourself.
        CHECK(expected_ptr != NULL);
        flags |= MAP_FIXED;
    }
    else
    {
        CHECK_EQ(0, flags & MAP_FIXED);
        // Don't bother checking for an overlapping region here. We'll
        // check this if required after the fact inside CheckMapRequest.
    }
    if (byte_count == 0)
    {
        self->name_ = filename;
        self->begin_ = NULL;
        self->size_ = 0;
        self->base_begin_ = NULL;
        self->base_size_ = 0;
        self->prot_ = prot;
        self->reuse_ = false;
        return true;
    }
    long pageSize = getSystemPageSize();
    // Adjust 'offset' to be page-aligned as required by mmap.
    int page_offset = start % pageSize;
    off_t page_aligned_offset = start - page_offset;
    // Adjust 'byte_count' to be page-aligned as we will map this anyway.
    size_t page_aligned_byte_count = (size_t) round_up(byte_count + page_offset,
                                                       (uint64_t) pageSize);
    // The 'expected_ptr' is modified (if specified, ie non-null) to be page aligned to the file but
    // not necessarily to virtual memory. mmap will page align 'expected' for us.
    byte *page_aligned_expected = (expected_ptr == NULL) ? NULL : (expected_ptr - page_offset);

    byte *actual = (byte *) (mmap(page_aligned_expected,
                                  page_aligned_byte_count,
                                  prot,
                                  flags,
                                  fd,
                                  page_aligned_offset));
    if (actual == MAP_FAILED)
    {
        auto saved_errno = errno;
        LOGF("mmap(%p, %zd, 0x%x, 0x%x, %d, %"
                     PRId64
                     ") of file '%s' failed: %s",
             (void *) page_aligned_expected, page_aligned_byte_count, prot, flags, fd,
             (int64_t) (page_aligned_offset), filename,
             strerror(saved_errno));
        dump_process_memory_map();
        return NULL;
    }
    if (!CheckMapRequest(expected_ptr, actual, page_aligned_byte_count))
    {
        return NULL;
    }

    // Everything worked, fill MemMap entry with info.
    self->name_ = filename;
    self->begin_ = actual + page_offset;
    self->size_ = byte_count;
    self->base_begin_ = actual;
    self->base_size_ = page_aligned_byte_count;
    self->prot_ = prot;
    self->reuse_ = reuse;
    return true;
}

bool mem_map_Unmap(MemMap *self)
{
    if (UNLIKELY(!mem_map_IsInUse(self) || !mem_map_SeemsValid(self)))
    {
        LOGF("Calling %s with a invalid MemMap struct. Either you supplied a inconsistent MemMap, "
                     "or a just initialized one.", __func__);
        return false;
    }
    if (self->base_begin_ == NULL && self->base_size_ == 0)
    {
        return true;
    }
    if (!self->reuse_)
    {
        int result = munmap(self->base_begin_, self->base_size_);
        if (result == -1)
        {
            LOGF("munmap failed");
        }
    }
}