
#一个Android.mk文件必须以定义一个LOCAL_PATH变量
LOCAL_PATH := $(call my-dir)/../src

#清理很多LOCAL_PATH中的LOCAL_XXX变量
include $(CLEAR_VARS)               

#模块名，名字必须是唯一的而且不能有空格
LOCAL_MODULE    := jpeg

#包含将要被编译和汇编进module的一个C/C++资源文件列表
LOCAL_SRC_FILES := \
    jcsample.c \
    jcapistd.c \
    jccoefct.c \
    jccolor.c \
    jcdctmgr.c \
    jchuff.c \
    jcinit.c \
    jcmainct.c \
    jcmarker.c \
    jcmaster.c \
    jcomapi.c \
    jcparam.c \
    jcphuff.c \
    jcprepct.c \
    jcsample.c \
    jctrans.c \
    jdapimin.c \
    jdapistd.c \
    jdatadst.c \
    jdatasrc.c \
    jdcoefct.c \
    jdcolor.c \
    jddctmgr.c \
    jdhuff.c \
    jdinput.c \
    jdmainct.c \
    jdmarker.c \
    jdmaster.c \
    jdmerge.c \
    jdphuff.c \
    jdpostct.c \
    jdsample.c \
    jdtrans.c \
    jerror.c \
    jfdctflt.c \
    jfdctfst.c \
    jfdctint.c \
    jidctflt.c \
    jidctfst.c \
    jidctint.c \
    jidctred.c \
    jquant1.c \
    jquant2.c \
    jutils.c \
    jmemmgr.c \
    jmemansi.c \
    jmemname.c \
    jmemnobs.c \
    cjpeg.c \
    djpeg.c \
    jpegtran.c \
    rdcolmap.c \
    rdswitch.c \
    transupp.c \
    rdppm.c \
    wrppm.c \
    rdgif.c \
    wrgif.c \
    rdtarga.c \
    wrtarga.c \
    rdbmp.c \
    wrbmp.c \
    rdrle.c \
    wrrle.c

 #这里加入必要的编译选项
LOCAL_CFLAGS    := -fexceptions

#生成一个静态库目标 
include $(BUILD_STATIC_LIBRARY)



