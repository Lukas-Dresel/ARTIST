//
// Created by Lukas on 8/16/2015.
//


#include <string.h>
#include <sys/types.h>
#include "oat.h"
#include "logging.h"
#include "typedefs.h"
#include "dex.h"
#include "oat_dex_file_storage.h"
#include "oat_info.h"


#define CASE_ENUM_REPR(x) case (x): \
                              return #x

char *repr_OatClassType(uint16_t t)
{
    switch (t)
    {
        CASE_ENUM_REPR(kOatClassAllCompiled);
        CASE_ENUM_REPR(kOatClassSomeCompiled);
        CASE_ENUM_REPR(kOatClassNoneCompiled);
        CASE_ENUM_REPR(kOatClassMax);
        default:
        {
            LOGF("Unknown OatClassType encountered: %hu", t);
            return "UNKNOWN OatClassType";
        }
    }
}

char *repr_mirror_Class_Status(int16_t status)
{
    switch (status)
    {
        CASE_ENUM_REPR(kStatusRetired);
        CASE_ENUM_REPR(kStatusError);
        CASE_ENUM_REPR(kStatusNotReady);
        CASE_ENUM_REPR(kStatusIdx);
        CASE_ENUM_REPR(kStatusLoaded);
        CASE_ENUM_REPR(kStatusResolving);
        CASE_ENUM_REPR(kStatusResolved);
        CASE_ENUM_REPR(kStatusVerifying);
        CASE_ENUM_REPR(kStatusRetryVerificationAtRuntime);
        CASE_ENUM_REPR(kStatusVerifyingAtRuntime);
        CASE_ENUM_REPR(kStatusVerified);
        CASE_ENUM_REPR(kStatusInitializing);
        CASE_ENUM_REPR(kStatusInitialized);
        CASE_ENUM_REPR(kStatusMax);
        default:
        {
            LOGF("Unknown mirror::Class::Status encountered: %hd", status);
            return "UNKNOWN mirror::Class::Status";
        }
    }
}

char *repr_InstructionSet(InstructionSet set)
{
    switch (set)
    {
        CASE_ENUM_REPR(kNone);
        CASE_ENUM_REPR(kArm);
        CASE_ENUM_REPR(kArm64);
        CASE_ENUM_REPR(kThumb2);
        CASE_ENUM_REPR(kX86);
        CASE_ENUM_REPR(kX86_64);
        CASE_ENUM_REPR(kMips);
        CASE_ENUM_REPR(kMips64);
        default:
        {
            LOGF("Unknown InstructionSet encountered: %d", set);
            return "UNKNOWN InstructionSet";
        }
    }
}

char *repr_InstructionSetFeatures(uint32_t f)
{
    switch (f)
    {
        case 0:
            return "None";
        case kHwDiv:
            // Supports hardware divide.
            return "Hardware Division Support";
        case kHwLpae:
            // Supports Large Physical Address Extension.
            return "Large PhysicalAddress Extension";
        case kHwDiv | kHwLpae:
            return "Hardware Division Support | Large Physical Address Extension";
        default:
        {
            LOGF("Unknown InstructionSetFeatures value %x", f);
            return "Illegal InstructionSetFeatures value";
        }
    }
}

#undef CASE_ENUM_REPR

static const char *oat_file_ParseString(const char *start, const char *end)
{
    while (start < end && *start != 0)
    {
        start++;
    }
    return start;
}

const char *oat_GetStoreValueByKey(OatHeader *this, const char *key)
{
    const char *ptr = (const char *) (&this->key_value_store_);
    const char *end = ptr + this->key_value_store_size_;
    while (ptr < end)
    {
        // Scan for a closing zero.
        const char *str_end = oat_file_ParseString(ptr, end);
        if (str_end < end)
        {
            if (strcmp(key, ptr) == 0)
            {
                // Same as key. Check if value is OK.
                if (oat_file_ParseString(str_end + 1, end) < end)
                {
                    return str_end + 1;
                }
            }
            else
            {
                // Different from key. Advance over the value.
                ptr = oat_file_ParseString(str_end + 1, end) + 1;
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

bool oat_GetStoreKeyValuePairByIndex(OatHeader *this, size_t index, const char **key,
                                     const char **value)
{
    const char *ptr = (const char *) (&this->key_value_store_);
    const char *end = ptr + this->key_value_store_size_;
    ssize_t counter = (ssize_t) (index);
    while (ptr < end && counter >= 0)
    {
        // Scan for a closing zero.
        const char *str_end = oat_file_ParseString(ptr, end);
        if (str_end < end)
        {
            const char *maybe_key = ptr;
            ptr = oat_file_ParseString(str_end + 1, end) + 1;
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

