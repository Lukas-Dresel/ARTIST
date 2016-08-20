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
 
//
// Created by Lukas on 8/6/2015.
//


#ifdef __cplusplus
extern "C"
{
#endif

#include "trappoint.h"
#include "../../main/jni/generate_trap_instruction.h"
#include <utility/logging.h>
#include <utility/concurrent_persistent_list.h>
#include <utility/memory.h>
#include <hostsystem/armeabi.h>
#include <utility/error.h>

static struct concurrent_persistent_list_entry* concurrent_persistent_list_installed_trappoints;
static struct sigaction old_sigtrap_action;
static struct sigaction old_sigill_action;

static void sigtrap_handler(int, siginfo_t *, ucontext_t *);
static void sigill_handler(int, siginfo_t *, ucontext_t *);
static void install_signal_handler(int signal, void *handler, struct sigaction *old_action);

static void install_signal_handler(int signal, void *handler, struct sigaction *old_action) {
    struct sigaction action;

    action.sa_sigaction = (void (*)(int, siginfo_t*, void*))handler;
    action.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(signal, &action, old_action) != 0) {
        LOGE("Error installing signal handler for signal #%d: %s", signal, strerror(errno));
    }
}
void init_trappoints() {
    /*stack_t sigstk;
    if ((sigstk.ss_sp = malloc(SIGSTKSZ)) == NULL)
    {
        LOGE("Could not allocate mem for alt stack");
        return;
    }
    sigstk.ss_size = SIGSTKSZ;
    sigstk.ss_flags = 0;
    if (sigaltstack(&sigstk,NULL) < 0)
    {
        perror("sigaltstack");
    }*/

    concurrent_persistent_list_installed_trappoints = NULL;

    install_signal_handler(SIGTRAP, &sigtrap_handler, &old_sigtrap_action);
    install_signal_handler(SIGILL, &sigill_handler, &old_sigill_action);
}

void destroy_trappoints() {
    if (sigaction(SIGILL, &old_sigill_action, NULL) != 0) {
        LOGD("Error uninstalling SIGILLhandler: %s", strerror(errno));
    }
    if (sigaction(SIGTRAP, &old_sigtrap_action, NULL) != 0) {
        LOGD("Error uninstalling SIGTRAP handler: %s", strerror(errno));
    }
}

static bool sigill_handler_trappoint_predicate(TrapPointInfo *trap, void *args) {
    if (trap->hook_info.trapping_method & TRAP_METHOD_SIG_ILL == 0) {
        return false;
    }
    return trap->target_instruction_info.mem_addr == args;
}
static bool sigtrap_handler_trappoint_predicate(TrapPointInfo *trap, void *args) {
    if (trap->hook_info.trapping_method & TRAP_METHOD_SIG_TRAP == 0) {
        return false;
    }
    return trap->target_instruction_info.mem_addr == args;
}

/**
 * @return whether a trappoint for this address was found or not
 */
static bool signal_handler_invoke_trappoint_handler(int signal, siginfo_t *sigInfo,
                                                    ucontext_t *context, TRAPPOINT_PREDICATE p) {
    mcontext_t *state_info = &(context->uc_mcontext);
    void *target = (void *) state_info->arm_pc;

    LOGD("Looking for trappoints for address "
                 PRINT_PTR
                 ".", (uintptr_t) target);

    TrapPointInfo *trap = trappoint_FindWithPredicate(p, target);
    if (trap == NULL) {
        return false;
    }

    /*
     * Every trappoint has to be disabled, as it is necessary to reset the instruction. This is
     * because on return of the signal handler the offending instruction is re-executed.
     *
     * That's why we save the information we still need and then disable the trappoint.
     * This executes the handler in a state where no collision with information that is about
     * to be freed can occur. This allows for example the handler to cause a double-trap by
     * re-installing another trappoint at the same address. Without uninstalling first this
     * might cause warnings for installing trappoints at addresses that already have one set.
     *
     * But this way the handler gets a clean slate to work with.
     */
    HOOKCALLBACK    handler = trap->state.callback;
    void*           handler_arg = trap->state.callback_args;
    void*           trapped_instruction = (void*)trap->target_instruction_info.call_addr;
    // Remove it!
    trappoint_Disable(trap);

    if(handler != NULL)
    {
        void* addr = trapped_instruction;
        LOGD("Executing handler for "PRINT_PTR" with arguments"
        "("PRINT_PTR", "PRINT_PTR", "PRINT_PTR");",
             (uintptr_t)addr, (uintptr_t)addr, (uintptr_t)context, (uintptr_t)handler_arg);

        handler(trapped_instruction, context, handler_arg);
    }
    else
    {
        LOGD("No trappoint handler for "PRINT_PTR" was registered, so it wasn't executed.",
             (uintptr_t) trapped_instruction);
    }
    return true;
}
void sigill_handler(int signal, siginfo_t *sigInfo, ucontext_t *context) {
    LOGD("Inside the SIGILL handler..., signal %d, siginfo_t "PRINT_PTR", context "PRINT_PTR,
         signal, (uintptr_t) sigInfo, (uintptr_t) context);

    if (!signal_handler_invoke_trappoint_handler(signal, sigInfo, context,
                                                 sigill_handler_trappoint_predicate)) {
        // Could not find any trappoints for our desired address
        LOGW("Could not locate any trappoints for this address, this SIGILL should not originate from us.");
        LOGW("Uninstalling own signal handler to pass on the SIGILL.");
        sigaction(SIGILL, &old_sigill_action, NULL);
        return;
    }

    LOGD("Returning from SIGILL-Handler.");
}
static void sigtrap_handler(int signal, siginfo_t *sigInfo, ucontext_t *context) {

    LOGD("Inside the SIGTRAP handler..., signal %d, siginfo_t "PRINT_PTR", context "PRINT_PTR,
         signal, (uintptr_t) sigInfo, (uintptr_t) context);

    if (!signal_handler_invoke_trappoint_handler(signal, sigInfo, context,
                                                 sigtrap_handler_trappoint_predicate)) {
        // Could not find any trappoints for our desired address
        LOGD("Could not locate trappoint, this SIGTRAP should not originate from us.");
        LOGD("Uninstalling own signal handler to pass on the SIGTRAP.");
        sigaction(SIGTRAP, &old_sigtrap_action, NULL);
        return;
    }

    LOGD("Returning from SIGTRAP-Handler.");
}




TrapPointInfo *trappoint_Install(void *addr, uint32_t method, HOOKCALLBACK handler,
                                 void *additionalArgs) {
    if (addr == NULL)
    {
        return NULL;
    }
    if(method == 0)
    {
        #ifdef DEFAULT_TRAPPOINT_METHOD

        // A default trapping method was defined use this.
        method = DEFAULT_TRAPPOINT_METHOD;

        #else

        // No default was specified, pick one randomly to make it harder to counteract
        if(rand() % 2 == 0)
        {
            method = TRAP_METHOD_INSTR_BKPT | TRAP_METHOD_SIG_TRAP;
        }
        else
        {
            method = TRAP_METHOD_INSTR_KNOWN_ILLEGAL | TRAP_METHOD_SIG_ILL;
        }

        #endif
    }

    TrapPointInfo* trap = (TrapPointInfo*)allocate_memory_chunk(sizeof(TrapPointInfo));
    if (trap != NULL)
    {
        trap->state.callback = handler;
        trap->state.callback_args = additionalArgs;

        trap->target_instruction_info.thumb = IsAddressThumbMode(addr);
        trap->target_instruction_info.mem_addr = InstructionPointerToCodePointer(addr);
        trap->target_instruction_info.call_addr = addr;

        trap->hook_info.instr_size = trap->target_instruction_info.thumb ? 2 : 4;
        trap->hook_info.trapping_method = method;

        if (set_memory_protection(trap->target_instruction_info.mem_addr,
                                  trap->hook_info.instr_size, true, true, true))
        {
            if (trap->target_instruction_info.thumb) {
                trap->hook_info.thumb.trap_instruction = make_thumb_trap_instruction(method);

                uint16_t *target = (uint16_t *) trap->target_instruction_info.mem_addr;
                trap->hook_info.thumb.preserved = *target;
                *target = trap->hook_info.thumb.trap_instruction;
            }
            else {
                trap->hook_info.arm.trap_instruction = make_arm_trap_instruction(method);

                uint32_t *target = (uint32_t *) trap->target_instruction_info.mem_addr;
                trap->hook_info.arm.preserved = *target;
                *target = trap->hook_info.arm.trap_instruction;
            }
            __builtin___clear_cache(trap->target_instruction_info.mem_addr,
                                    trap->target_instruction_info.mem_addr +
                                            trap->hook_info.instr_size);

            // Installation succeeded, so insert ourself into the installed trap point list

            if(!concurrent_persistent_list_try_create(&concurrent_persistent_list_installed_trappoints, trap))
            {
                // Not being able to create the list means the list must already exist => insert
                concurrent_persistent_list_insert_after(concurrent_persistent_list_installed_trappoints, trap);
            }
            return trap;
        }
        free_memory_chunk(trap);
    }
    return NULL;
}
void trappoint_Destroy(TrapPointInfo *trap)
{
    // TODO: Check if other stuff must be freed
    free_memory_chunk(trap);
}


// As of now unused, leaving implementation in case we might want to re-introduce it
bool trappoint_Enable(TrapPointInfo *trap)
{
    if (!trappoint_ValidateContents(trap)) {
        return false;
    }
    if (!set_memory_protection(trap->target_instruction_info.mem_addr, trap->hook_info.instr_size, 
                               true, true, true)) 
    {
        set_last_error(
                "Cannot set memory protections to write the trappoint instruction. TRAPPOINT WAS NOT SET.");
        return false;
    }
    else {
        if (trap->target_instruction_info.thumb) {
            *((uint16_t *) trap->target_instruction_info.mem_addr) = trap->hook_info.thumb.trap_instruction;
        }
        else {
            *((uint32_t *) trap->target_instruction_info.mem_addr) = trap->hook_info.arm.trap_instruction;
        }
        __builtin___clear_cache(trap->target_instruction_info.mem_addr, trap->target_instruction_info.mem_addr + trap->hook_info.instr_size);
    }
    return true;
}
// As of now unused, leaving implementation in case we might want to re-introduce it
bool trappoint_Disable(TrapPointInfo *trap) {
    if (!trappoint_ValidateContents(trap)) {
        return false;
    }
    if (!set_memory_protection(trap->target_instruction_info.mem_addr, trap->hook_info.instr_size, true, true, true)) {
        set_last_error(
                "Cannot set memory protections to reset the trappoints target to its original state. NOT RESETTING MEMORY TO ORIGINAL STATE.");
        return false;
    }
    else {
        if (trap->target_instruction_info.thumb) {
            *((uint16_t *) trap->target_instruction_info.mem_addr) = trap->hook_info.thumb.preserved;
        }
        else {
            *((uint32_t *) trap->target_instruction_info.mem_addr) = trap->hook_info.arm.preserved;
        }
        __builtin___clear_cache(trap->target_instruction_info.mem_addr, trap->target_instruction_info.mem_addr + trap->hook_info.instr_size);
    }
    return true;
}

static void dump_trappoint_info(int index, concurrent_persistent_list_entry* ent,
                                void* current_element, void* args)
{
    TrapPointInfo* trap = (TrapPointInfo*)current_element;
    LOGD("FOUND Trappoint for "PRINT_PTR"(%s)", (uintptr_t) trap->target_instruction_info.mem_addr,
         trap->target_instruction_info.thumb ? "THUMB" : "ARM");

    LOGD("->Handler function: "PRINT_PTR, (uintptr_t) trap->state.callback);
    LOGD("->Instruction Size: %d", trap->hook_info.instr_size);

    LOGD("->Code Info:      ");
    if ((trap->hook_info.trapping_method & TRAP_METHOD_SIG_ILL) != 0)
    {
        LOGD("->Trapping generates SIGILL");
    }
    if ((trap->hook_info.trapping_method & TRAP_METHOD_SIG_TRAP) != 0)
    {
        LOGD("->Trapping generates SIGTRAP");
    }
    if ((trap->hook_info.trapping_method & TRAP_METHOD_INSTR_BKPT) != 0)
    {
        LOGD("->->This is achieved using hardware-specific BKPT instructions.");
    }
    if ((trap->hook_info.trapping_method & TRAP_METHOD_INSTR_KNOWN_ILLEGAL) != 0)
    {
        LOGD("->->This is achieved using hardware-specific known illegal instructions.");
    }
    if (trap->target_instruction_info.thumb)
    {
        LOGD("->->Breakpoint:   0x%04x", trap->hook_info.thumb.trap_instruction);
        LOGD("->->Preserved:    0x%04x", trap->hook_info.thumb.preserved);
    }
    else
    {
        LOGD("->->Breakpoint:   0x%08x", trap->hook_info.arm.trap_instruction);
        LOGD("->->Preserved:    0x%08x", trap->hook_info.arm.preserved);
    }
}
void dump_installed_trappoints_info() {
    LOGD("Dumping information on installed trappoints.");
    concurrent_persistent_list_iterate(concurrent_persistent_list_installed_trappoints,
                                       dump_trappoint_info, NULL);
}


bool trappoint_ValidateContents(TrapPointInfo *trap) {
    if (trap == NULL) {
        set_last_error("The trappoint supplied was NULL!");
        return false;
    }
    if (trap->target_instruction_info.call_addr == NULL || trap->target_instruction_info.mem_addr == NULL) {
        set_last_error(
                "A trappoint target address cannot be NULL. Either memory location or address itself was NULL.");
        return false;
    }
    if (trap->target_instruction_info.thumb) {
        if (((uint64_t) trap->target_instruction_info.call_addr & 0x1) == 0) {
            set_last_error(
                    "How can the thumb mode be set if the call address doesn't have the least significant bit set?");
            return false;
        }
        if (trap->hook_info.instr_size != 2) {
            set_last_error("If we are in thumb mode, why is the instruction size not 2?");
            return false;
        }
    }
    else {
        if (((uint64_t) trap->target_instruction_info.call_addr & 0x1) == 0x1) {
            set_last_error(
                    "How can the thumb mode not be set if the call address has the least significant bit set?");
            return false;
        }
        if (trap->hook_info.instr_size != 4) {
            set_last_error("If we are not in thumb mode, why is the instruction size not 4?");
            return false;
        }
    }
    return true;
}

struct TRAPPOINT_LIST_FIND_ARGS
{
    TRAPPOINT_PREDICATE predicate;
    void* predicate_args;
    TrapPointInfo* found_match;
};
static void find_trappoint_in_list_callback(int index, concurrent_persistent_list_entry* ent,
                                            void* current_element, void* args)
{
    TrapPointInfo* trap = (TrapPointInfo*)current_element;
    struct TRAPPOINT_LIST_FIND_ARGS* arguments = (struct TRAPPOINT_LIST_FIND_ARGS*)args;

    if(arguments->predicate(trap, arguments->predicate_args))
    {
        // this should not actually be necessary, since that is a stack structure, but it can't hurt
        __sync_bool_compare_and_swap(&arguments->found_match, NULL, trap);
    }
    return;
}
TrapPointInfo *trappoint_FindWithPredicate(TRAPPOINT_PREDICATE p, void *args)
{
    struct TRAPPOINT_LIST_FIND_ARGS cur_search_args =
    {
        .predicate = p,
        .predicate_args = args,
        .found_match = NULL
    };
    concurrent_persistent_list_iterate(concurrent_persistent_list_installed_trappoints,
                                       find_trappoint_in_list_callback, &cur_search_args);
    return cur_search_args.found_match;
}

#ifdef __cplusplus
}
#endif