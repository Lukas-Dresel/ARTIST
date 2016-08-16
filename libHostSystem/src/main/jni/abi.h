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
// Created by Lukas on 8/18/2015.
//

#ifndef NDKTEST_ABI_INTERFACE_H
#define NDKTEST_ABI_INTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
#include <ucontext.h>

struct InstructionInfo
{
    void*       call_addr;
    void*       mem_addr;
    bool        thumb;
};

/*
 * Returns the actual memory address of the instruction executed when branching to this address.
 * On many architectures this might simply return the entrypoint, but there are exceptions.
 * For example on ARM processors THUMB mode instructions are addressed with the lowest byte set.
 * This has the pointer pointing one byte past the instruction actually being executed.
 */
void *InstructionPointerToCodePointer(const void *instruction_pointer);


/*
 * Returns the length of the instruction at the given InstructionPointer. This must be the
 * InstructionPointer and not the memory address, because e.g. on ARM the instruction size can only
 * be determined with knowledge of whether the processor is in ARM or THUMB mode. This is determined
 * by examining the lowest bit of the InstructionPointer
 */
uint32_t GetInstructionLengthAtInstructionPointer(const void *instruction_pointer);

/*
 * Generally only valid on function entry.
 * Returns the value of the argument at the specified index. This is generalized to a uint64_t and
 * might need to be casted to the appropriate types like uint32_t depending on the target
 * architecture.
 */
uint64_t GetArgument(ucontext_t *c, unsigned int index);

/*
 * Generally only valid on function entry.
 * Sets the value of the argument at the specified index to a specific value.
 * This is generalized to a uint64_t and will internally be cast back to the type appropriate for a
 * specific architecture, e.g on x86 or armeabi to uint32_t.
 */
void SetArgument(ucontext_t *c, unsigned int index, uint64_t val);

/*
 * Ideally it should extract from the current program context the next instruction that will be
 * executed. Because of time-constraints however the current implementation simply returns the
 * next instruction in memory. This means this will result in wrong results whenever the execution
 * branches, like on jumps and calls.
 *
 * CAUTION: Depending on the architecture this might not actually be a pointer to the first
 * instruction of the function in memory. An example of this is the ARM architecture where
 * this returns the address + 1 if the processor is operating in THUMB mode.
 *
 * To get the actual address of the instruction to be executed use the
 * @link{InstructionPointerToCodePointer} function.
 */
void *ExtractNextExecutedInstructionPointer(ucontext_t *ctx);

/*
 * Generally only valid on function entry. Extracts the return address of a function from the
 * calling convention. E.g. on x86 it gets it from the top of the stack,
 * or on ARM from the LR register
 */
void *ExtractReturnAddress(ucontext_t *ctx);



#ifdef __cplusplus
}
#endif

#endif //NDKTEST_ABI_H
