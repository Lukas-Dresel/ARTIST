#ifndef _CONFIG_H
#define _CONFIG_H

#include "config_constants.h"

#define LOG_LEVEL                           LOG_LEVEL_ALL

#define OAT_VERSION OAT_VERSION_064

//#define TARGET_ARCHITECTURE                 ARCHITECTURE_ARM
//#define DEBUG
//#define DEBUG_WAIT

#define PROC_PID_MAPS_MAX_LINE_LENGTH 3200

#define DEFAULT_TRAPPOINT_METHOD (TRAP_METHOD_INSTR_KNOWN_ILLEGAL | TRAP_METHOD_SIG_ILL)

#endif





