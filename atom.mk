
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libvideo-defs
LOCAL_CATEGORY_PATH := libs
LOCAL_DESCRIPTION := Video definitions library
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_CFLAGS := -DVDEF_API_EXPORTS -fvisibility=hidden -std=gnu11 -D_GNU_SOURCE
LOCAL_SRC_FILES := \
	src/vdefs.c \
	src/vdefs_formats.c \
	src/vdefs_json.c \
	src/vdefs_params.c

# Public API headers - top level headers first
# This header list is currently used to generate a python binding
LOCAL_EXPORT_CUSTOM_VARIABLES := LIBVIDEODEFS_HEADERS=$\
	$(LOCAL_PATH)/include/video-defs/vdefs.h;

LOCAL_PUBLIC_LIBRARIES := \
	libh264 \
	libh265

LOCAL_PRIVATE_LIBRARIES := \
	json \
	libulog

include $(BUILD_LIBRARY)


ifdef TARGET_TEST

include $(CLEAR_VARS)

LOCAL_MODULE := tst-libvideo-defs
LOCAL_LIBRARIES := \
	libcunit\
	libfutils \
	libulog \
	libvideo-defs
LOCAL_CFLAGS := -std=gnu11
LOCAL_SRC_FILES := \
	tests/vdefs_test_calc.c \
	tests/vdefs_test_csv.c \
	tests/vdefs_test_frac.c \
	tests/vdefs_test_framerate.c \
	tests/vdefs_test_resolution.c \
	tests/vdefs_test.c

include $(BUILD_EXECUTABLE)

endif
