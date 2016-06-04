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


#一个Android.mk文件必须以定义一个LOCAL_PATH变量
LOCAL_PATH := $(MAIN_PATH)

#清理很多LOCAL_PATH中的LOCAL_XXX变量
include $(CLEAR_VARS)               

#模块名，名字必须是唯一的而且不能有空格
LOCAL_MODULE    := gameLib   

#包含将要被编译和汇编进module的一个C/C++资源文件列表
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
  
#这里加入必要的
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2

#这里加入必要的编译选项 -fno-exceptions -fno-rtti
LOCAL_CFLAGS    := -fexceptions -finput-charset=utf-8 -D_ANDROID -DUSE_OPENGL -D_DEBUG -I"../" -I"../../jpeg-6b/src" -I"../../FreeType/include" -I"../../zlib/include" -I"../../libpng/include" -I"../android_assimp/include" -I"../../Game1"

#这里加入依赖的静态模块, 如果A依赖B,那么A放前面,B放后面
LOCAL_STATIC_LIBRARIES := android_native_app_glue libjpeg FreeType png zlib assimp

#生成一个静态库目标 
#include $(BUILD_STATIC_LIBRARY)

#生成一个动态库目标
include $(BUILD_SHARED_LIBRARY)

#导入依赖的模块路径，填写的是依赖模块的Android.mk文件所在路径
$(call import-module,android/native_app_glue)