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
 
/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_RUNTIME_BASE_MUTEX_H_
#define ART_RUNTIME_BASE_MUTEX_H_

#if defined(__APPLE__)
#define ART_USE_FUTEXES 0
#else
#define ART_USE_FUTEXES 1
#endif

// Currently Darwin doesn't support locks with timeouts.
#if !defined(__APPLE__)
#define HAVE_TIMED_RWLOCK 1
#else
#define HAVE_TIMED_RWLOCK 0
#endif

// LockLevel is used to impose a lock hierarchy [1] where acquisition of a Mutex at a higher or
// equal level to a lock a thread holds is invalid. The lock hierarchy achieves a cycle free
// partial ordering and thereby cause deadlock situations to fail checks.
//
// [1] http://www.drdobbs.com/parallel/use-lock-hierarchies-to-avoid-deadlock/204801163
enum LockLevel {
  kLoggingLock = 0,
  kMemMapsLock,
  kSwapMutexesLock,
  kUnexpectedSignalLock,
  kThreadSuspendCountLock,
  kAbortLock,
  kJdwpSocketLock,
  kReferenceQueueSoftReferencesLock,
  kReferenceQueuePhantomReferencesLock,
  kReferenceQueueFinalizerReferencesLock,
  kReferenceQueueWeakReferencesLock,
  kReferenceQueueClearedReferencesLock,
  kReferenceProcessorLock,
  kRosAllocGlobalLock,
  kRosAllocBracketLock,
  kRosAllocBulkFreeLock,
  kAllocSpaceLock,
  kDexFileMethodInlinerLock,
  kDexFileToMethodInlinerMapLock,
  kMarkSweepMarkStackLock,
  kTransactionLogLock,
  kInternTableLock,
  kOatFileSecondaryLookupLock,
  kDefaultMutexLevel,
  kMarkSweepLargeObjectLock,
  kPinTableLock,
  kLoadLibraryLock,
  kJdwpObjectRegistryLock,
  kModifyLdtLock,
  kAllocatedThreadIdsLock,
  kMonitorPoolLock,
  kClassLinkerClassesLock,
  kBreakpointLock,
  kMonitorLock,
  kMonitorListLock,
  kThreadListLock,
  kBreakpointInvokeLock,
  kAllocTrackerLock,
  kDeoptimizationLock,
  kProfilerLock,
  kJdwpEventListLock,
  kJdwpAttachLock,
  kJdwpStartLock,
  kRuntimeShutdownLock,
  kTraceLock,
  kHeapBitmapLock,
  kMutatorLock,
  kInstrumentEntrypointsLock,
  kThreadListSuspendThreadLock,
  kZygoteCreationLock,

  kLockLevelCount  // Must come last.
};

#endif  // ART_RUNTIME_BASE_MUTEX_H_
