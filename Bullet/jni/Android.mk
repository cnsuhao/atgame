
#һ��Android.mk�ļ������Զ���һ��LOCAL_PATH����
LOCAL_PATH := $(call my-dir)/../src

#����ܶ�LOCAL_PATH�е�LOCAL_XXX����
include $(CLEAR_VARS)

#ģ���������ֱ�����Ψһ�Ķ��Ҳ����пո�
LOCAL_MODULE    := bullet

#������Ҫ������ͻ���module��һ��C/C++��Դ�ļ��б�

COLLiSION_PATH    := $(wildcard $(LOCAL_PATH)/BulletCollision/BroadphaseCollision/*.cpp)
COLLiSION_PATH    += $(wildcard $(LOCAL_PATH)/BulletCollision/CollisionDispatch/*.cpp)
COLLiSION_PATH    += $(wildcard $(LOCAL_PATH)/BulletCollision/CollisionShapes/*.cpp)
COLLiSION_PATH    += $(wildcard $(LOCAL_PATH)/BulletCollision/Gimpact/*.cpp)
COLLiSION_PATH    += $(wildcard $(LOCAL_PATH)/BulletCollision/NarrowPhaseCollision/*.cpp)

DYNAMISC_PATH     := $(wildcard $(LOCAL_PATH)/BulletDynamics/Character/*.cpp)
DYNAMISC_PATH     += $(wildcard $(LOCAL_PATH)/BulletDynamics/ConstraintSolver/*.cpp)
DYNAMISC_PATH     += $(wildcard $(LOCAL_PATH)/BulletDynamics/Dynamics/*.cpp)
DYNAMISC_PATH     += $(wildcard $(LOCAL_PATH)/BulletDynamics/Vehicle/*.cpp)

SOFTBODY_PATH     := $(wildcard $(LOCAL_PATH)/BulletSoftBody/*.cpp)
LINEAR_MATH_PATH  := $(wildcard $(LOCAL_PATH)/LinearMath/*.cpp)

COLLiSION_PATH    := $(COLLiSION_PATH:$(LOCAL_PATH)/%=%)
DYNAMISC_PATH     := $(DYNAMISC_PATH:$(LOCAL_PATH)/%=%)
SOFTBODY_PATH     := $(SOFTBODY_PATH:$(LOCAL_PATH)/%=%)
LINEAR_MATH_PATH  := $(LINEAR_MATH_PATH:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES += $(COLLiSION_PATH)
LOCAL_SRC_FILES += $(DYNAMISC_PATH)
LOCAL_SRC_FILES += $(SOFTBODY_PATH)
LOCAL_SRC_FILES += $(LINEAR_MATH_PATH)

 #��������Ҫ�ı���ѡ��
LOCAL_CFLAGS    := -fexceptions -I"src"

#��������Ҫ��
LOCAL_LDLIBS    := -llog -landroid   

#����һ����̬��Ŀ�� 
#include $(BUILD_STATIC_LIBRARY)

#����һ����̬��Ŀ��
include $(BUILD_SHARED_LIBRARY)