//
// Created by Lukas on 8/16/2015.
//


#include <string.h>
#include <sys/types.h>
#include "oat.h"

static const char* oat_header_ParseString(const char* start, const char* end)
{
    while (start < end && *start != 0)
    {
        start++;
    }
    return start;
}

const char* oat_header_GetStoreValueByKey(OatHeader* this, const char* key)
{
    const char* ptr = (const char*)(&this->key_value_store_);
    const char* end = ptr + this->key_value_store_size_;
    while (ptr < end)
    {
        // Scan for a closing zero.
        const char* str_end = oat_header_ParseString(ptr, end);
        if (str_end < end)
        {
            if (strcmp(key, ptr) == 0)
            {
                // Same as key. Check if value is OK.
                if (oat_header_ParseString(str_end + 1, end) < end)
                {
                    return str_end + 1;
                }
            }
            else
            {
                // Different from key. Advance over the value.
                ptr = oat_header_ParseString(str_end + 1, end) + 1;
            }
        }
        else
        {
            break;
        }
    }
    // Not found.
    return NULL;
}
bool oat_header_GetStoreKeyValuePairByIndex(OatHeader* this, size_t index, const char** key, const char** value)
{
    const char* ptr = (const char*)(&this->key_value_store_);
    const char* end = ptr + this->key_value_store_size_;
    ssize_t counter = (ssize_t)(index);
    while (ptr < end && counter >= 0)
    {
        // Scan for a closing zero.
        const char* str_end = oat_header_ParseString(ptr, end);
        if (str_end < end)
        {
            const char* maybe_key = ptr;
            ptr = oat_header_ParseString(str_end + 1, end) + 1;
            if (ptr <= end)
            {
                if (counter == 0)
                {
                    *key = maybe_key;
                    *value = str_end + 1;
                    return true;
                }
                else
                {
                    counter--;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            break;
        }
    }
    // Not found.
    return false;
}
size_t oat_header_GetHeaderSize(OatHeader* this)
{
    return sizeof(OatHeader) + this->key_value_store_size_;
}