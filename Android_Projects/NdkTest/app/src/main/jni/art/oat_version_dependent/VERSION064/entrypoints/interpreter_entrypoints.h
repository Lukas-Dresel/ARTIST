//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_ENTRYPOINTS_INTERPRETER_ENTRYPOINTS_H
#define NDKTEST_ART_ENTRYPOINTS_INTERPRETER_ENTRYPOINTS_H

#include "../../../../util/macros.h"
#include "../../../dex_internal.h"
#include "../../../stack.h"

struct Thread;
// Pointers to functions that are called by interpreter trampolines via thread-local storage.
struct PACKED(4) InterpreterEntryPoints {
    void (*pInterpreterToInterpreterBridge)(struct Thread* self, const struct CodeItem* code_item,
                                            struct ShadowFrame* shadow_frame, union JValue* result);
    void (*pInterpreterToCompiledCodeBridge)(struct Thread* self, const struct CodeItem* code_item,
                                             struct ShadowFrame* shadow_frame, union JValue* result);
};

#endif //NDKTEST_ART_ENTRYPOINTS_INTERPRETER_ENTRYPOINTS_H
