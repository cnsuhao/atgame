MAIN_PATH := $(call my-dir)/../src

JPEG_PATH := $(call my-dir)/../../jpeg-6b/obj/local/armeabi
FREETYPE_PATH := $(call my-dir)/../../FreeType/obj/local/armeabi
ZLIB_PATH := $(call my-dir)/../../zlib/lib/android/arm
PNG_PATN := $(call my-dir)/../../libpng/lib/android/arm

# jpeg
LOCAL_PATH := $(JPEG_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libjpeg
LOCAL_SRC_FILES := libjpeg.a
include $(PREBUILT_STATIC_LIBRARY)

# FreeType
LOCAL_PATH := $(FREETYPE_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := FreeType
LOCAL_SRC_FILES := libFreeType.a
include $(PREBUILT_STATIC_LIBRARY)

# png
LOCAL_PATH := $(PNG_PATN)
include $(CLEAR_VARS)
LOCAL_MODULE    := png
LOCAL_SRC_FILES := libpng.a
include $(PREBUILT_STATIC_LIBRARY)

# zlib
LOCAL_PATH := $(ZLIB_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := zlib
LOCAL_SRC_FILES := libzlib.a
include $(PREBUILT_STATIC_LIBRARY)


#一个Android.mk文件必须以定义一个LOCAL_PATH变量
LOCAL_PATH := $(MAIN_PATH)

#清理很多LOCAL_PATH中的LOCAL_XXX变量
include $(CLEAR_VARS)               

#模块名，名字必须是唯一的而且不能有空格
LOCAL_MODULE    := android_main     

#包含将要被编译和汇编进module的一个C/C++资源文件列表
LOCAL_SRC_FILES := \
    ../../source/atgMath.cpp \
    ../../source/atgBase.cpp \
    ../../source/atgProfile.cpp \
    ../../source/atgIntersection.cpp \
    ../../source/atgMisc.cpp \
    ../../source/atgCamera.cpp \
    ../../source/atgMesh.cpp \
    ../../source/atgMaterial.cpp \
    ../../source/atgSkinAnimation.cpp \
    ../../source/atgGame.cpp \
    ../../source/atgShaderLibrary.cpp \
    ../../source/atgRenderer_ogl.cpp \
    ../../source/atgRenderer_common.cpp \
    ../../source/atgUIBase.cpp\
    ../../source/atgUI.cpp \
    ../../source/atgUIElements.cpp \
    ../../source/atgThread.cpp \
    ../../AtWin/Game.cpp \
    androidMain.cpp
  
#这里加入必要的
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2   

#这里加入必要的编译选项 -fno-exceptions -fno-rtti
LOCAL_CFLAGS    := -fexceptions -finput-charset=utf-8 -D_ANDROID -D_DEBUG -I"../source" -I"../jpeg-6b/src" -I"../FreeType/include" -I"../zlib/include" -I"../libpng/include"  -I"../AtWin"

#这里加入依赖的静态模块, 如果A依赖B,那么A放前面,B放后面
LOCAL_STATIC_LIBRARIES := android_native_app_glue libjpeg FreeType png zlib

#生成一个静态库目标 
#include $(BUILD_STATIC_LIBRARY)

#生成一个动态库目标
include $(BUILD_SHARED_LIBRARY)

#导入依赖的模块路径，填写的是依赖模块的Android.mk文件所在路径
$(call import-module,android/native_app_glue)