//
// Created by Lukas on 8/15/2015.
//

#ifndef NDKTEST_TRAPPOINT_SIGNAL_HANDLING_H
#define NDKTEST_TRAPPOINT_SIGNAL_HANDLING_H

#include <ucontext.h>

void sigill_handler(int signal, siginfo_t* sigInfo, ucontext_t* context);
void sigtrap_handler(int signal, siginfo_t* sigInfo, ucontext_t* context);

#endif //NDKTEST_TRAPPOINT_SIGNAL_HANDLING_H
