
#һ��Android.mk�ļ������Զ���һ��LOCAL_PATH����
LOCAL_PATH := $(call my-dir)/../lua-5.3.0

#����ܶ�LOCAL_PATH�е�LOCAL_XXX����
include $(CLEAR_VARS)               

#ģ���������ֱ�����Ψһ�Ķ��Ҳ����пո�
LOCAL_MODULE    := lua

#������Ҫ������ͻ���module��һ��C/C++��Դ�ļ��б�
LOCAL_SRC_FILES := lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lua.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c

 #��������Ҫ�ı���ѡ��
LOCAL_CFLAGS    := -fexceptions

#����һ����̬��Ŀ�� 
include $(BUILD_STATIC_LIBRARY)



