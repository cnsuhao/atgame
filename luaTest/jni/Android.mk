MAIN_PATH := $(call my-dir)/../src

LUA_PATH := $(call my-dir)/../../lua/obj/local/armeabi

# lua
LOCAL_PATH := $(LUA_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := liblua
LOCAL_SRC_FILES := liblua.a
include $(PREBUILT_STATIC_LIBRARY)

#һ��Android.mk�ļ������Զ���һ��LOCAL_PATH����
LOCAL_PATH := $(MAIN_PATH)

#����ܶ�LOCAL_PATH�е�LOCAL_XXX����
include $(CLEAR_VARS)               

#ģ���������ֱ�����Ψһ�Ķ��Ҳ����пո�
LOCAL_MODULE    := android_lua_test

#������Ҫ������ͻ���module��һ��C/C++��Դ�ļ��б�
LOCAL_SRC_FILES := \
    androidMain.cpp
  
#��������Ҫ��
LOCAL_LDLIBS    := -llog -landroid

#��������Ҫ�ı���ѡ�� -fno-exceptions -fno-rtti
LOCAL_CFLAGS    := -fexceptions -finput-charset=utf-8 -D_ANDROID -I"../lua/lua-5.3.0"

#������������ľ�̬ģ��
LOCAL_STATIC_LIBRARIES := android_native_app_glue lua

#����һ����̬��Ŀ�� 
#include $(BUILD_STATIC_LIBRARY)

#����һ����̬��Ŀ��
include $(BUILD_SHARED_LIBRARY)

#����������ģ��·������д��������ģ���Android.mk�ļ�����·��
$(call import-module,android/native_app_glue)