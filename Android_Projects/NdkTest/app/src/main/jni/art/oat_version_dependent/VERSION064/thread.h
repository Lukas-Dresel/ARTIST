//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_THREAD_H
#define NDKTEST_ART_THREAD_H

#include <stddef.h>
#include <jni.h>
#include <stdint.h>
#include <pthread.h>
#include "runtime_stats.h"
#include "jvalue.h"
#include "stack.h"
#include "entrypoints/interpreter_entrypoints.h"
#include "entrypoints/jni_entrypoints.h"
#include "entrypoints/quick_entrypoints.h"
#include "jni_env_ext.h.h"

enum ThreadPriority {
    kMinThreadPriority = 1,
    kNormThreadPriority = 5,
    kMaxThreadPriority = 10,
};

enum ThreadFlag {
    kSuspendRequest   = 1,  // If set implies that suspend_count_ > 0 and the Thread should enter the
    // safepoint handler.
            kCheckpointRequest = 2  // Request that the thread do some checkpoint work and then continue.
};

enum StackedShadowFrameType {
    kShadowFrameUnderConstruction,
    kDeoptimizationShadowFrame
};

// We have no control over the size of 'bool', but want our boolean fields
// to be 4-byte quantities.
typedef uint32_t bool32_t;


// 32 bits of atomically changed state and flags. Keeping as 32 bits allows and atomic CAS to
// change from being Suspended to Runnable without a suspend request occurring.
union PACKED(4) StateAndFlags {
    struct PACKED(4) {
        // Bitfield of flag values. Must be changed atomically so that flag values aren't lost. See
        // ThreadFlags for bit field meanings.
        volatile uint16_t flags;
        // Holds the ThreadState. May be changed non-atomically between Suspended (ie not Runnable)
        // transitions. Changing to Runnable requires that the suspend_request be part of the atomic
        // operation. If a thread is suspended and a suspend_request is present, a thread may not
        // change to Runnable as a GC or other operation is in progress.
        volatile uint16_t state;
    } as_struct;
    int32_t as_atomic_int;
    volatile int32_t as_int;
};

struct PACKED(4) tls_32bit_sized_values
{
    union StateAndFlags state_and_flags;

    // A non-zero value is used to tell the current thread to enter a safe point
    // at the next poll.
    int suspend_count;

    // How much of 'suspend_count_' is by request of the debugger, used to set things right
    // when the debugger detaches. Must be <= suspend_count_.
    int debug_suspend_count;

    // Thin lock thread id. This is a small integer used by the thin lock implementation.
    // This is not to be confused with the native thread's tid, nor is it the value returned
    // by java.lang.Thread.getId --- this is a distinct value, used only for locking. One
    // important difference between this id and the ids visible to managed code is that these
    // ones get reused (to ensure that they fit in the number of bits available).
    uint32_t thin_lock_thread_id;

    // System thread id.
    uint32_t tid;

    // Is the thread a daemon?
    const bool32_t daemon;

    // A boolean telling us whether we're recursively throwing OOME.
    bool32_t throwing_OutOfMemoryError;

    // A positive value implies we're in a region where thread suspension isn't expected.
    uint32_t no_thread_suspension;

    // How many times has our pthread key's destructor been called?
    uint32_t thread_exit_check_count;

    // True if signal is being handled by this thread.
    bool32_t handling_signal_;

    // True if the return value for interpreter after deoptimization is a reference.
    // For gc purpose.
    bool32_t deoptimization_return_value_is_reference;

    // True if the thread is suspended in FullSuspendCheck(). This is
    // used to distinguish runnable threads that are suspended due to
    // a normal suspend check from other threads.
    bool32_t suspended_at_suspend_check;

    // True if the thread has been suspended by a debugger event. This is
    // used to invoke method from the debugger which is only allowed when
    // the thread is suspended by an event.
    bool32_t ready_for_debug_invoke;

    // True if the thread enters a method. This is used to detect method entry
    // event for the debugger.
    bool32_t debug_method_entry_;
};
struct PACKED(8) tls_64bit_sized_values
{
    // The clock base used for tracing.
    uint64_t trace_clock_base;

    // Return value used by deoptimization.
    union JValue deoptimization_return_value;

    struct RuntimeStats stats;
};
struct PACKED(4) tls_ptr_sized_values
{
    // The biased card table, see CardTable for details.
    uint8_t* card_table;

    // The pending exception or null.
    void* exception;

    // The end of this thread's stack. This is the lowest safely-addressable address on the stack.
    // We leave extra space so there's room for the code that throws StackOverflowError.
    uint8_t* stack_end;

    // The top of the managed stack often manipulated directly by compiler generated code.
    struct ManagedStack managed_stack;

    // In certain modes, setting this to 0 will trigger a SEGV and thus a suspend check.  It is
    // normally set to the address of itself.
    uintptr_t* suspend_trigger;

    // Every thread may have an associated JNI environment
    struct JNIEnvExt* jni_env;

    // Temporary storage to transfer a pre-allocated JNIEnvExt from the creating thread to the
    // created thread.
    struct JNIEnvExt* tmp_jni_env;

    // Initialized to "this". On certain architectures (such as x86) reading off of Thread::Current
    // is easy but getting the address of Thread::Current is hard. This field can be read off of
    // Thread::Current to give the address.
    struct Thread* self;

    // Our managed peer (an instance of java.lang.Thread). The jobject version is used during thread
    // start up, until the thread is registered and the local opeer_ is used.
    void* opeer;
    jobject jpeer;

    // The "lowest addressable byte" of the stack.
    uint8_t* stack_begin;

    // Size of the stack.
    size_t stack_size;

    // Pointer to previous stack trace captured by sampling profiler.
    void* stack_trace_sample;

    // The next thread in the wait set this thread is part of or null if not waiting.
    struct Thread* wait_next;

    // If we're blocked in MonitorEnter, this is the object we're trying to lock.
    void* monitor_enter_object;

    // Top of linked list of handle scopes or null for none.
    void* top_handle_scope;

    // Needed to get the right ClassLoader in JNI_OnLoad, but also
    // useful for testing.
    jobject class_loader_override;

    // Thread local, lazily allocated, long jump context. Used to deliver exceptions.
    void* long_jump_context;

    // Additional stack used by method instrumentation to store method and return pc values.
    // Stored as a pointer since std::deque is not PACKED.
    void* instrumentation_stack;

    // JDWP invoke-during-breakpoint support.
    void* debug_invoke_req;

    // JDWP single-stepping support.
    void* single_step_control;

    // For gc purpose, a shadow frame record stack that keeps track of:
    // 1) shadow frames under construction.
    // 2) deoptimization shadow frames.
    void* stacked_shadow_frame_record;

    // Deoptimization return value record stack.
    void* deoptimization_return_value_stack;

    // A cached copy of the java.lang.Thread's name.
    void* name;

    // A cached pthread_t for the pthread underlying this Thread*.
    pthread_t pthread_self;

    // If no_thread_suspension_ is > 0, what is causing that assertion.
    const char* last_no_thread_suspension_cause;

    // Pending checkpoint function or null if non-pending. Installation guarding by
    // Locks::thread_suspend_count_lock_.


    /*

    // Maximum number of checkpoint functions.
    static const uint32_t kMaxCheckpoints = 3;

     */
    void* checkpoint_functions[3];

    // Entrypoint function pointers.
    // TODO: move this to more of a global offset table model to avoid per-thread duplication.
    struct InterpreterEntryPoints interpreter_entrypoints;
    struct JniEntryPoints jni_entrypoints;
    struct QuickEntryPoints quick_entrypoints;

    // Thread-local allocation pointer.
    uint8_t* thread_local_start;
    uint8_t* thread_local_pos;
    uint8_t* thread_local_end;
    size_t thread_local_objects;

    // There are RosAlloc::kNumThreadLocalSizeBrackets thread-local size brackets per thread.

    /*
    static const size_t kNumRosAllocThreadLocalSizeBrackets = 34;
     */
    void* rosalloc_runs[34];

    // Thread-local allocation stack data/routines.
    void* thread_local_alloc_stack_top; // used to be StackReference<mirror::Object>*
    void* thread_local_alloc_stack_end; // used to be StackReference<mirror::Object>*



    //TODO Everything past this doesn't matter for now, if neccessary get it back in later

    /*// Support for Mutex lock hierarchy bug detection.
    void* held_mutexes[kLockLevelCount]; // used to be BaseMutex*

    // Recorded thread state for nested signals.
    jmp_buf* nested_signal_state;

    // The function used for thread flip.
    Closure* flip_function;

    // Current method verifier, used for root marking.
    verifier::MethodVerifier* method_verifier;*/
};




struct Thread {
    /***********************************************************************************************/
    // Thread local storage. Fields are grouped by size to enable 32 <-> 64 searching to account for
    // pointer size differences. To encourage shorter encoding, more frequently used values appear
    // first if possible.
    /***********************************************************************************************/

    struct tls_32bit_sized_values tls32_;
    struct tls_64bit_sized_values tls64_;
    struct tls_ptr_sized_values tlsPtr_;

    //TODO Stuff past this doesn't matter for now, if necessary get it back in later on

    /*
    // Guards the 'interrupted_' and 'wait_monitor_' members.
    Mutex* wait_mutex;

    // Condition variable waited upon during a wait.
    ConditionVariable* wait_cond;
    // Pointer to the monitor lock we're currently waiting on or null if not waiting.
    Monitor* wait_monitor;

    // Thread "interrupted" status; stays raised until queried or thrown.
    bool interrupted;*/

};

#endif //NDKTEST_ART_THREAD_H
