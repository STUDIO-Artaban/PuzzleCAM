LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LS_CPP = $(subst $(1)/,,$(wildcard $(1)/$(2)/*.cpp))

LOCAL_MODULE           := PuzzleCAM
LOCAL_C_INCLUDES       := $(LOCAL_PATH) $(LOCAL_PATH)/Sources
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/Sources
LOCAL_SRC_FILES  := JNI.cpp                                       \
                       $(call LS_CPP,$(LOCAL_PATH),Sources)       \
                       $(call LS_CPP,$(LOCAL_PATH),Sources/Title) \
                       $(call LS_CPP,$(LOCAL_PATH),Sources/Game)
LOCAL_CPPFLAGS         := -D__ANDROID__ -fPIC -fexceptions -Wmultichar -ffunction-sections -fdata-sections -std=c++98 -std=gnu++98 -fno-rtti
LOCAL_STATIC_LIBRARIES := boost_system boost_thread boost_math_c99f boost_regex
LOCAL_SHARED_LIBRARIES := libogg libvorbis openal libeng
LOCAL_LDLIBS           := -llog -landroid -lEGL -lGLESv2
LOCAL_LDFLAGS          := -Wl -gc-sections

include $(BUILD_SHARED_LIBRARY)

$(call import-module, boost_1_53_0)
$(call import-module, libogg-vorbis)
$(call import-module, openal-1_15_1)

$(call import-add-path, /home/pascal/workspace)
$(call import-module, libeng)
