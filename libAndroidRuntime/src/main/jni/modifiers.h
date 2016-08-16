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
// Created by Lukas on 1/26/2016.
//

#ifndef NDKTEST_ART_MODIFIERS_H
#define NDKTEST_ART_MODIFIERS_H


#include <stdint.h>

// class, field, method, ic
#define kAccPublic 0x0001

// field, method, ic
#define kAccPrivate 0x0002

// field, method, ic
#define kAccProtected 0x0004

// field, method, ic
#define kAccStatic 0x0008

// class, field, method, ic
#define kAccFinal 0x0010

// method (only allowed on natives)
#define kAccSynchronized 0x0020

// class (not used in dex)
#define kAccSuper 0x0020

// field
#define kAccVolatile 0x0040

// method (1.5)
#define kAccBridge 0x0040

// field
#define kAccTransient 0x0080

// method (1.5)
#define kAccVarargs 0x0080

// method
#define kAccNative 0x0100

// class, ic
#define kAccInterface 0x0200

// class, method, ic
#define kAccAbstract 0x0400

// method
#define kAccStrict 0x0800

// class, field, method, ic
#define kAccSynthetic 0x1000

// class, ic (1.5)
#define kAccAnnotation 0x2000

// class, field, ic (1.5)
#define kAccEnum 0x4000

// bits set from Java sources (low 16)
#define kAccJavaFlagsMask 0xffff

// method (dex only) <(cl)init>
#define kAccConstructor 0x00010000

// method (dex only)
#define kAccDeclaredSynchronized 0x00020000

// class  (dex only)
#define kAccClassIsProxy 0x00040000

// class (runtime),
#define kAccPreverified 0x00080000

// method (dex only)
#define kAccFastNative 0x00080000

// method (dex only)
#define kAccPortableCompiled 0x00100000

// method (dex only)
#define kAccMiranda 0x00200000

// Special runtime-only flags.
// Note: if only kAccClassIsReference is set, we have a soft reference.

// class/ancestor overrides finalize()
#define kAccClassIsFinalizable 0x80000000

// class is a soft/weak/phantom ref
#define kAccClassIsReference 0x08000000

// class is a weak reference
#define kAccClassIsWeakReference 0x04000000

// class is a finalizer reference
#define kAccClassIsFinalizerReference 0x02000000

// class is a phantom reference
#define kAccClassIsPhantomReference 0x01000000

#define kAccReferenceFlagsMask (kAccClassIsReference | kAccClassIsWeakReference | kAccClassIsFinalizerReference | kAccClassIsPhantomReference)

// Valid (meaningful) bits for a field.
#define kAccValidFieldFlags (kAccPublic | kAccPrivate | kAccProtected | kAccStatic | kAccFinal | kAccVolatile | kAccTransient | kAccSynthetic | kAccEnum)

// Valid (meaningful) bits for a method.
#define kAccValidMethodFlags (kAccPublic | kAccPrivate | kAccProtected | kAccStatic | kAccFinal | kAccSynchronized | kAccBridge | kAccVarargs | kAccNative | kAccAbstract | kAccStrict | kAccSynthetic | kAccMiranda | kAccConstructor | kAccDeclaredSynchronized)

// Valid (meaningful) bits for a class (not interface).
// Note 1. These are positive bits. Other bits may have to be zero.
// Note 2. Inner classes can expose more access flags to Java programs. That is handled by libcore.
#define kAccValidClassFlags (kAccPublic | kAccFinal | kAccSuper | kAccAbstract | kAccSynthetic | kAccEnum)

// Valid (meaningful) bits for an interface.
// Note 1. Annotations are interfaces.
// Note 2. These are positive bits. Other bits may have to be zero.
// Note 3. Inner classes can expose more access flags to Java programs. That is handled by libcore.
#define kAccValidInterfaceFlags (kAccPublic | kAccInterface | kAccAbstract | kAccSynthetic | kAccAnnotation)

#endif //NDKTEST_ART_MODIFIERS_H
