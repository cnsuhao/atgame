
#һ��Android.mk�ļ������Զ���һ��LOCAL_PATH����
LOCAL_PATH := $(call my-dir)/../src

#����ܶ�LOCAL_PATH�е�LOCAL_XXX����
include $(CLEAR_VARS)               

#ģ���������ֱ�����Ψһ�Ķ��Ҳ����пո�
LOCAL_MODULE    := jpeg

#������Ҫ������ͻ���module��һ��C/C++��Դ�ļ��б�
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

 #��������Ҫ�ı���ѡ��
LOCAL_CFLAGS    := -fexceptions

#����һ����̬��Ŀ�� 
include $(BUILD_STATIC_LIBRARY)



