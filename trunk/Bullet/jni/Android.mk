
#一个Android.mk文件必须以定义一个LOCAL_PATH变量
LOCAL_PATH := $(call my-dir)/../src

#清理很多LOCAL_PATH中的LOCAL_XXX变量
include $(CLEAR_VARS)

#模块名，名字必须是唯一的而且不能有空格
LOCAL_MODULE    := bullet

#包含将要被编译和汇编进module的一个C/C++资源文件列表

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

 #这里加入必要的编译选项
LOCAL_CFLAGS    := -fexceptions -I"src"

#这里加入必要的
LOCAL_LDLIBS    := -llog -landroid   

#生成一个静态库目标 
#include $(BUILD_STATIC_LIBRARY)

#生成一个动态库目标
include $(BUILD_SHARED_LIBRARY)