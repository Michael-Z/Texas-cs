LOCAL_PATH := $(call my-dir)/../../Classes

include $(CLEAR_VARS)
LOCAL_MODULE            := bspatch
LOCAL_SRC_FILES         := ../UmengAgent/Android/$(TARGET_ARCH_ABI)/libbspatch.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := casdkjni
LOCAL_SRC_FILES         := ../proj.android.mm/mmbilling/Android/$(TARGET_ARCH_ABI)/libcasdkjni.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := identifyapp
LOCAL_SRC_FILES         := ../proj.android.mm/mmbilling/Android/$(TARGET_ARCH_ABI)/libidentifyapp.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := texas_poker_shared

LOCAL_MODULE_FILENAME := libTexasPoker

LOCAL_SRC_FILES := ../proj.android.mm/jni/TexasPoker.cpp \
	$(patsubst $(LOCAL_PATH)/%.cpp,%.cpp,$(wildcard $(LOCAL_PATH)/*.cpp)) \
	$(patsubst $(LOCAL_PATH)/%.cpp,%.cpp,$(wildcard $(LOCAL_PATH)/NetEngine/*.cpp)) \
	$(patsubst $(LOCAL_PATH)/%.cpp,%.cpp,$(wildcard $(LOCAL_PATH)/General/*.cpp)) \
	$(patsubst $(LOCAL_PATH)/%.cpp,%.cpp,$(wildcard $(LOCAL_PATH)/Client/*.cpp))

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(LOCAL_PATH)/NetEngine \
	$(LOCAL_PATH)/General \
	$(LOCAL_PATH)/Client

ifeq ($(Configuration),Debug)
LOCAL_CFLAGS := -O0 -Wall -DVE_DEBUG -DVE_MEM_DEBUG
endif

ifeq ($(Configuration),Release)
LOCAL_CFLAGS := -O2 -Wall -DVE_RELEASE -DVE_MEM_DEBUG
endif

LOCAL_CFLAGS += -DVE_PLATFORM_ANDROID -DVE_STATIC_LIB -DVE_CPU_ARM

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += VePower zziplib curllib
LOCAL_LDLIBS := -llog -landroid -lOpenSLES

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,CocosDenshion/android)
$(call import-module,Source/VePower)
$(call import-module,Externals/zziplib)
$(call import-module,Externals/curllib)
