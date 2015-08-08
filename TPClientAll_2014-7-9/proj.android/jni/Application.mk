APP_PLATFORM := android-9
APP_ABI := $(CPU)
APP_STL := gnustl_static
ifeq ($(Configuration),Debug)
APP_OPTIM := debug
APP_CPPFLAGS := -frtti -DCOCOS2D_DEBUG=1
endif
ifeq ($(Configuration),Release)
APP_OPTIM := release
APP_CPPFLAGS := -frtti -DCOCOS2D_DEBUG=0
endif
