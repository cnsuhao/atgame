
#һ��Android.mk�ļ������Զ���һ��LOCAL_PATH����
LOCAL_PATH := $(call my-dir)/../

#����ܶ�LOCAL_PATH�е�LOCAL_XXX����
include $(CLEAR_VARS)

#ģ���������ֱ�����Ψһ�Ķ��Ҳ����пո�
LOCAL_MODULE    := detour

#������Ҫ������ͻ���module��һ��C/C++��Դ�ļ��б�
LIB_DT_SOURCES    := $(wildcard $(LOCAL_PATH)/source/*.cpp)
LIB_DT_SOURCES    := $(LIB_DT_SOURCES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LIB_DT_SOURCES)

 #��������Ҫ�ı���ѡ��
LOCAL_CFLAGS    := -fexceptions -I"include" 

#����һ����̬��Ŀ�� 
include $(BUILD_STATIC_LIBRARY)




