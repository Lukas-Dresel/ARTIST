//
// Created by Lukas on 8/19/2015.
//

#include "list.h"
#include "trappoint_interface.h"
#include "memory.h"
#include "macros.h"
#include "logging.h"
#include "abi_interface.h"
#include "../../armeabi-v7a/jni/abi.h"

static struct list_head installed_breakpoints;

typedef TRAPPOINT_CALLBACK BREAKPOINT_CALLBACK;

struct BreakpointInfo
{
    struct list_head    installed_entry;

    BREAKPOINT_CALLBACK handler;
    void*               additional_args;

    TrapPointInfo*      primary_trappoint;

};

void init_breakpoints()
{
    INIT_LIST_HEAD(&installed_breakpoints);
}
void destroy_breakpoints()
{

}

/*static void breakpoint_handler(void *trap_addr, ucontext_t *context, void *additionalArg)
{
    InstructionInfo next_instr = extract_next_executed_instruction(context);
    LOGD("SingleStep-Handler: Next instruction assumed to be: "PRINT_PTR, (uintptr_t)next_instr.call_addr);

    LOGD("Attempting to install next trappoint in single step chain. ");
    install_trappoint(next_instr.call_addr, TRAP_METHOD_SIG_ILL | TRAP_METHOD_INSTR_KNOWN_ILLEGAL, handler_step_function, arg);
}
struct BreakpointInfo* breakpoint_Create(void* address, uint32_t trap_method, BREAKPOINT_CALLBACK handler, void* additional_args)
{
    CHECK_NE(address, NULL);
    CHECK_NE(handler, NULL);

    struct BreakpointInfo* self = allocate_memory_chunk(sizeof(struct BreakpointInfo));
    if(UNLIKELY(self == NULL))
    {
        return NULL;
    }
    self->handler = handler;
    self->additional_args = additional_args;
    TrapPointInfo* actual_trap_point = install_trappoint(address, trap_method, breakpoint_handler, self )
    install_trappoint(address, trap_method, handler, additional_args);
}*/
