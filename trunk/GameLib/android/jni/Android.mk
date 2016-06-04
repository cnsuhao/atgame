MAIN_PATH := $(call my-dir)/../src

JPEG_PATH := $(call my-dir)/../../../jpeg-6b/obj/local/armeabi
FREETYPE_PATH := $(call my-dir)/../../../FreeType/obj/local/armeabi
ZLIB_PATH := $(call my-dir)/../../../zlib/lib/android/arm
PNG_PATN := $(call my-dir)/../../../libpng/lib/android/arm
ASSIMP_PATN := $(call my-dir)/../../android_assimp/obj/local/armeabi

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

# assimp
LOCAL_PATH := $(ASSIMP_PATN)
include $(CLEAR_VARS)
LOCAL_MODULE    := assimp
LOCAL_SRC_FILES := libassimp.a
include $(PREBUILT_STATIC_LIBRARY)


#һ��Android.mk�ļ������Զ���һ��LOCAL_PATH����
LOCAL_PATH := $(MAIN_PATH)

#����ܶ�LOCAL_PATH�е�LOCAL_XXX����
include $(CLEAR_VARS)               

#ģ���������ֱ�����Ψһ�Ķ��Ҳ����пո�
LOCAL_MODULE    := gameLib   

#������Ҫ������ͻ���module��һ��C/C++��Դ�ļ��б�
LOCAL_SRC_FILES := \
    ../../atgMath.cpp \
    ../../atgBase.cpp \
    ../../atgProfile.cpp \
    ../../atgIntersection.cpp \
    ../../atgMisc.cpp \
    ../../atgCamera.cpp \
    ../../atgLight.cpp \
	../../atgGame.cpp \
    ../../atgShaderLibrary.cpp \
    ../../atgRenderer_ogl.cpp \
    ../../atgRenderer_common.cpp \
	../../atgMesh.cpp \
	../../atgMaterial.cpp \
	../../atgUtility.cpp \
    ../../atgModel.cpp \
	../../atgAssimpImport.cpp \
	../../../Game1/Component.cpp \
	../../../Game1/Entity.cpp \
	../../../Game1/IniFile.cpp \
	../../../Game1/Propertie.cpp \
	../../../Game1/Scene.cpp \
	../../../Game1/Game1.cpp \
    androidMain.cpp
  
#��������Ҫ��
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2

#��������Ҫ�ı���ѡ�� -fno-exceptions -fno-rtti
LOCAL_CFLAGS    := -fexceptions -finput-charset=utf-8 -D_ANDROID -DUSE_OPENGL -D_DEBUG -I"../" -I"../../jpeg-6b/src" -I"../../FreeType/include" -I"../../zlib/include" -I"../../libpng/include" -I"../android_assimp/include" -I"../../Game1"

#������������ľ�̬ģ��, ���A����B,��ôA��ǰ��,B�ź���
LOCAL_STATIC_LIBRARIES := android_native_app_glue libjpeg FreeType png zlib assimp

#����һ����̬��Ŀ�� 
#include $(BUILD_STATIC_LIBRARY)

#����һ����̬��Ŀ��
include $(BUILD_SHARED_LIBRARY)

#����������ģ��·������д��������ģ���Android.mk�ļ�����·��
$(call import-module,android/native_app_glue)