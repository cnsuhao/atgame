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


#һ��Android.mk�ļ������Զ���һ��LOCAL_PATH����
LOCAL_PATH := $(MAIN_PATH)

#����ܶ�LOCAL_PATH�е�LOCAL_XXX����
include $(CLEAR_VARS)               

#ģ���������ֱ�����Ψһ�Ķ��Ҳ����пո�
LOCAL_MODULE    := android_main     

#������Ҫ������ͻ���module��һ��C/C++��Դ�ļ��б�
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
  
#��������Ҫ��
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2   

#��������Ҫ�ı���ѡ�� -fno-exceptions -fno-rtti
LOCAL_CFLAGS    := -fexceptions -finput-charset=utf-8 -D_ANDROID -D_DEBUG -I"../source" -I"../jpeg-6b/src" -I"../FreeType/include" -I"../zlib/include" -I"../libpng/include"  -I"../AtWin"

#������������ľ�̬ģ��, ���A����B,��ôA��ǰ��,B�ź���
LOCAL_STATIC_LIBRARIES := android_native_app_glue libjpeg FreeType png zlib

#����һ����̬��Ŀ�� 
#include $(BUILD_STATIC_LIBRARY)

#����һ����̬��Ŀ��
include $(BUILD_SHARED_LIBRARY)

#����������ģ��·������д��������ģ���Android.mk�ļ�����·��
$(call import-module,android/native_app_glue)