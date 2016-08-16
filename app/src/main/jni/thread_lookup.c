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


#include "thread_lookup.h"
#include <sys/syscall.h>
#include "art/oat_version_dependent/VERSION045/thread.h"
#include <utility/logging.h>

static bool isCurrentThread(struct Thread* t)
{
    if(t->tlsPtr_.pthread_self != pthread_self())
    {
        return false;
    }
    void* stackpointer = NULL;
    asm("mov %0, sp" : "=r" (stackpointer));
    if(t->tlsPtr_.stack_begin > stackpointer || stackpointer > t->tlsPtr_.stack_end)
    {
        return false;
    }
    if(syscall(SYS_gettid) != t->tls32_.tid)
    {
        return false;
    }
    return true;
}
static struct Thread* ScanMemoryForThreadObjectPointer(JNIEnv* env)
{
    void** start = (void*)((uint32_t)env & ~0xFFF);
    void** end = (void*)start + 0x1000;

    struct Thread* dummy = NULL;
    uint32_t diff = (void*)&dummy->tlsPtr_.jni_env - (void*)dummy;

    for(void** p = start; p < end; p ++)
    {
        if(*p == env)
        {
            struct Thread* potential_thread = (void*)p - diff;
            LOGI("Found JNIEnv reference at:  "PRINT_PTR, p);
            LOGI("Possible Thread* target at: "PRINT_PTR, (uintptr_t)potential_thread);
            if(!isCurrentThread(potential_thread))
            {
                LOGI("Found current Thread object!");
                return  potential_thread;
            }
            LOGI("Turned out to not be the correct Thread object.");
        }
    }
    return NULL;
}
struct Thread* GetCurrentThreadObjectPointer(JNIEnv* env)
{
    struct Thread* t;
    asm("mov %0, r9" : "=r" (t));
    if(t == NULL || t->tlsPtr_.jni_env != env)
    {
        // Didn't find it by cheating, let's try scanning for it.
        return ScanMemoryForThreadObjectPointer(env);;
    }
    return t;
}