MAIN_PATH := $(call my-dir)/../src

LUA_PATH := $(call my-dir)/../../lua/obj/local/armeabi

# lua
LOCAL_PATH := $(LUA_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := liblua
LOCAL_SRC_FILES := liblua.a
include $(PREBUILT_STATIC_LIBRARY)

#一个Android.mk文件必须以定义一个LOCAL_PATH变量
LOCAL_PATH := $(MAIN_PATH)

#清理很多LOCAL_PATH中的LOCAL_XXX变量
include $(CLEAR_VARS)               

#模块名，名字必须是唯一的而且不能有空格
LOCAL_MODULE    := android_lua_test

#包含将要被编译和汇编进module的一个C/C++资源文件列表
LOCAL_SRC_FILES := \
    androidMain.cpp
  
#这里加入必要的
LOCAL_LDLIBS    := -llog -landroid

#这里加入必要的编译选项 -fno-exceptions -fno-rtti
LOCAL_CFLAGS    := -fexceptions -finput-charset=utf-8 -D_ANDROID -I"../lua/lua-5.3.0"

#这里加入依赖的静态模块
LOCAL_STATIC_LIBRARIES := android_native_app_glue lua

#生成一个静态库目标 
#include $(BUILD_STATIC_LIBRARY)

#生成一个动态库目标
include $(BUILD_SHARED_LIBRARY)

#导入依赖的模块路径，填写的是依赖模块的Android.mk文件所在路径
$(call import-module,android/native_app_glue)