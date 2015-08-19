#ifndef _LOGGING_H
#define _LOGGING_H

#include <android/log.h>
#include <inttypes.h>

#include "config.h"
#include "macros.h"

#define LOG_TAG "jni_mainact_native"

#if LOG_LEVEL <= LOG_LEVEL_VERBOSE
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define LOGV(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#else
#define LOGI(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "["__FILE__":"__LINE_STRING__"]"__VA_ARGS__)
#else
#define LOW(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "["__FILE__":"__LINE_STRING__"]"__VA_ARGS__)
#else
#define LOGE(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, "["__FILE__":"__LINE_STRING__"]"__VA_ARGS__)
#else
#define LOGF(...)
#endif

#define CHECK(x) \
  if (UNLIKELY(!(x))) \
    LOGF( "Check failed: %s", #x );

#define CHECK_EQ(x, y) CHECK((x) == (y))
#define CHECK_NE(x, y) CHECK((x) != (y))
#define CHECK_LE(x, y) CHECK((x) <= (y))
#define CHECK_LT(x, y) CHECK((x) < (y))
#define CHECK_GE(x, y) CHECK((x) >= (y))
#define CHECK_GT(x, y) CHECK((x) > (y))

#define PRINT_PTR "0x%016"PRIxPTR

#endif

