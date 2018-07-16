LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := injector
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.c)
LOCAL_C_INCLUDES += src/include
LOCAL_LDFLAGS += -pie

include $(BUILD_EXECUTABLE)
