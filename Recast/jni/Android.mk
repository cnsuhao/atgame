
#一个Android.mk文件必须以定义一个LOCAL_PATH变量
LOCAL_PATH := $(call my-dir)/../

#清理很多LOCAL_PATH中的LOCAL_XXX变量
include $(CLEAR_VARS)

#模块名，名字必须是唯一的而且不能有空格
LOCAL_MODULE    := recast

#包含将要被编译和汇编进module的一个C/C++资源文件列表
LIB_RECASR_SOURCES    := $(wildcard $(LOCAL_PATH)/source/*.cpp)
LIB_RECASR_SOURCES    := $(LIB_RECASR_SOURCES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(LIB_RECASR_SOURCES)

 #这里加入必要的编译选项
LOCAL_CFLAGS    := -fexceptions -I"include" 

#生成一个静态库目标 
include $(BUILD_STATIC_LIBRARY)




