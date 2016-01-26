//
// Created by Lukas on 1/18/2016.
//

#ifndef NDKTEST_ART_ENTRYPOINTS_QUICK_ENTRYPOINTS_H
#define NDKTEST_ART_ENTRYPOINTS_QUICK_ENTRYPOINTS_H

#include <inttypes.h>
#include <jni.h>

struct Thread;

struct PACKED(4) QuickEntryPoints
{
#define ENTRYPOINT_ENUM(name, rettype, ...) rettype ( * p ## name )( __VA_ARGS__ );

#include "quick_entrypoints_list.h"
  QUICK_ENTRYPOINT_LIST(ENTRYPOINT_ENUM)
#undef QUICK_ENTRYPOINT_LIST
#undef ENTRYPOINT_ENUM
};

#endif //NDKTEST_ART_ENTRYPOINTS_QUICK_ENTRYPOINTS_H
