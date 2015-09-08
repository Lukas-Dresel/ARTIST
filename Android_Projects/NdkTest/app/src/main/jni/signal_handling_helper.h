//
// Created by Lukas on 8/14/2015.
//

#ifndef NDKTEST_SIGNAL_HANDLING_HELPER_H
#define NDKTEST_SIGNAL_HANDLING_HELPER_H

#include <ucontext.h>

uint32_t get_argument(ucontext_t *c, unsigned int index);

void set_argument(ucontext_t *c, unsigned int index, uint32_t val);

void log_siginfo_content(siginfo_t *info);

void log_mcontext_content(mcontext_t *state_info);

#endif //NDKTEST_SIGNAL_HANDLING_HELPER_H
