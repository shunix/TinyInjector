LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := injector
LOCAL_SRC_FILES := src/utils.c src/ptrace.c src/injector.c src/main.c
LOCAL_LDFLAGS += -pie

include $(BUILD_EXECUTABLE)
