#ifndef _SYSTEM_INFO_H
#define _SYSTEM_INFO_H

#include <jni.h>
#include <unistd.h>

#include "logging.h"

void init_system_info();
void destroy_system_info();

long getSystemPageSize();

#endif





