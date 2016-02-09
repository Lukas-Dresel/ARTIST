//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_TRAPPOINT_INTERFACE_H
#define NDKTEST_TRAPPOINT_INTERFACE_H

#include <ucontext.h>
#include <stdbool.h>
#include "hooking_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define TRAP_METHOD_INSTR_BKPT 1
#define TRAP_METHOD_INSTR_KNOWN_ILLEGAL 2
#define TRAP_METHOD_SIG_TRAP 0x10000
#define TRAP_METHOD_SIG_ILL  0x20000

struct TrapPointInfo;
typedef struct TrapPointInfo TrapPointInfo;

typedef bool (*TRAPPOINT_PREDICATE)(TrapPointInfo *trap, void *args);

void init_trappoints();
void destroy_trappoints();
void dump_installed_trappoints_info();

TrapPointInfo * trappoint_Install           (void *addr, uint32_t method, HOOKCALLBACK handler,
                                             void *additionalArg);
void            trappoint_Uninstall         (TrapPointInfo *trap);
TrapPointInfo * trappoint_FindWithPredicate (TRAPPOINT_PREDICATE p, void *args);
bool            trappoint_ValidateContents  (TrapPointInfo *trap);

#ifdef __cplusplus
}
#endif

#endif //NDKTEST_TRAPPOINT_INTERFACE_H
