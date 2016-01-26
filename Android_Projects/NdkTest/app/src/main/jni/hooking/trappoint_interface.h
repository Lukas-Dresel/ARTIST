//
// Created by Lukas on 8/6/2015.
//

#ifndef NDKTEST_TRAPPOINT_INTERFACE_H
#define NDKTEST_TRAPPOINT_INTERFACE_H

#include <ucontext.h>
#include <stdbool.h>

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

typedef void (*TRAPPOINT_CALLBACK)(void *addr, ucontext_t *ctx, void *additionalArg);

typedef bool (*TRAPPOINT_PREDICATE)(TrapPointInfo *trap, void *args);

void init_trappoints();

void destroy_trappoints();

TrapPointInfo *install_trappoint(void *addr, uint32_t method, TRAPPOINT_CALLBACK handler,
                                 void *additionalArg);

//TODO add ability to enable/disable trappoints so that it is not necessary to delete the trappoint
// in the handler before calling the handler, so that the handler has access to the actual instructions
// that were overwritten to allow e.g single-stepping as the handler needs info on the length of the
// instruction. This is a first attempt.
bool enable_trappoint(TrapPointInfo *trap);

bool disable_trappoint(TrapPointInfo *trap);


void uninstall_trappoint(TrapPointInfo *trap);

TrapPointInfo *find_trappoint_with_predicate(TRAPPOINT_PREDICATE p, void *args);

void dump_installed_trappoints_info();

bool validate_TrapPointInfo_contents(TrapPointInfo *trap);

#ifdef __cplusplus
}
#endif

#endif //NDKTEST_TRAPPOINT_INTERFACE_H
