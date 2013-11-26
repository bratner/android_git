LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

commands_recovery_local_path := $(LOCAL_PATH)

ifneq ($(TARGET_SIMULATOR),true)
ifeq ($(TARGET_ARCH),arm)

LOCAL_SRC_FILES := \
	luksunlock.c \

LOCAL_MODULE := luksunlock

LOCAL_FORCE_STATIC_EXECUTABLE := true

RECOVERY_API_VERSION := 2
LOCAL_CFLAGS += -DRECOVERY_API_VERSION=$(RECOVERY_API_VERSION) -Wall

LOCAL_MODULE_TAGS := eng

LOCAL_STATIC_LIBRARIES := libunz
LOCAL_STATIC_LIBRARIES += libminui libpixelflinger_static libpng libcutils
LOCAL_STATIC_LIBRARIES += libstdc++ libc libz 

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := luksmanage.c input_handler.c passwd_dlg.c vibrator.c bootsync.c bkground.c yesno_dlg.c

LOCAL_MODULE := luksmanage

LOCAL_FORCE_STATIC_EXECUTABLE := true

RECOVERY_API_VERSION := 2
LOCAL_CFLAGS += -DRECOVERY_API_VERSION=$(RECOVERY_API_VERSION) -Wall

LOCAL_MODULE_TAGS := eng

LOCAL_STATIC_LIBRARIES := libunz
LOCAL_STATIC_LIBRARIES += libminui libpixelflinger_static libpng libcutils
LOCAL_STATIC_LIBRARIES += libstdc++ libc libz

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := listprops.c

LOCAL_MODULE := listprops
LOCAL_MODULE_TAGS := eng

#LOCAL_STATIC_LIBRARIES := libcuitls libc 
LOCAL_SHARED_LIBRARIES := libcutils libc
#LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := test_passwd_dlg.c input_handler.c passwd_dlg.c vibrator.c bootsync.c bkground.c yesno_dlg.c

LOCAL_MODULE := test_passwd_dlg

LOCAL_FORCE_STATIC_EXECUTABLE := true

RECOVERY_API_VERSION := 2
LOCAL_CFLAGS += -DRECOVERY_API_VERSION=$(RECOVERY_API_VERSION) -Wall

LOCAL_MODULE_TAGS := eng

LOCAL_STATIC_LIBRARIES := libunz
LOCAL_STATIC_LIBRARIES += libminui libpixelflinger_static libpng libcutils
LOCAL_STATIC_LIBRARIES += libstdc++ libc libz

include $(BUILD_EXECUTABLE)
include $(CLEAR_VARS)

LOCAL_MODULE := csrun
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES := csrun.c
#LOCAL_STATIC_LIBRARIES := libc
#LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

include $(commands_recovery_local_path)/minui/Android.mk
commands_recovery_local_path :=

endif   # TARGET_ARCH == arm
endif	# !TARGET_SIMULATOR

