
#一个Android.mk文件必须以定义一个LOCAL_PATH变量
LOCAL_PATH := $(call my-dir)/../lua-5.3.0

#清理很多LOCAL_PATH中的LOCAL_XXX变量
include $(CLEAR_VARS)               

#模块名，名字必须是唯一的而且不能有空格
LOCAL_MODULE    := lua

#包含将要被编译和汇编进module的一个C/C++资源文件列表
LOCAL_SRC_FILES := lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lua.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c

 #这里加入必要的编译选项
LOCAL_CFLAGS    := -fexceptions

#生成一个静态库目标 
include $(BUILD_STATIC_LIBRARY)



