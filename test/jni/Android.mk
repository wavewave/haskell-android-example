LOCAL_PATH := $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE := libtest11
#LOCAL_SRC_FILES := ../test/libtest11.a
#include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := wrapper
LOCAL_SRC_FILES := ../wrapper.c
#LOCAL_STATIC_LIBRARIES := libtest11
include $(BUILD_SHARED_LIBRARY)

