LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := NdkTest

LOCAL_LDLIBS := -llog

LOCAL_CFLAGS += "--std=gnu99"
LOCAL_CFLAGS += "-Wno-pointer-to-int-cast"
LOCAL_CFLAGS += "-Wno-int-to-pointer-cast"

include $(BUILD_SHARED_LIBRARY)