ifeq ($(TARGET_ARCH),arm)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	src/b2g-audio-daemon.cpp 

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	liblog \
	libril \
	libmedia

LOCAL_MODULE:= b2g-audio-daemon

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

endif
