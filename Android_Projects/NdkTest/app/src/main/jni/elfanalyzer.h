#ifndef _ELF_ANALYZER_H
#define _ELF_ANALYZER_H

#include <elf.h>
#include <errno.h>
#include <stdbool.h>

#include "logging.h"
#include "memory.h"

enum SegmentType
{
    Null                    = PT_NULL,
    Load                    = PT_LOAD,
    Dynamic                 = PT_DYNAMIC,
    Interpreter             = PT_INTERP,
    Note                    = PT_NOTE,
    ShLib_Unused            = PT_SHLIB,
    ProgramHeader           = PT_PHDR,
    ThreadLocalStorage      = PT_TLS,
    OsSpecificLow           = PT_LOOS,
    OsSpecificHigh          = PT_HIOS,
    ProcessorSpecificLow    = PT_LOPROC,
    ProcessorSpecificHigh   = PT_HIPROC,
    GCC_EHFrameHeader       = PT_GNU_EH_FRAME,
    GCC_StackExecutability  = PT_GNU_STACK
};

void dumpElfInfo(JNIEnv* env, const void* addr);

#endif