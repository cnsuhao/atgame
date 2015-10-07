#pragma once

#include "atgMath.h"

class atgJoint
{
    friend class atgJointAnimData;
public:
    atgJoint(void);
    virtual ~atgJoint(void);

    virtual void                SetParent(atgJoint* parent);
    virtual atgJoint*           GetParent();
    virtual void                AddChild(atgJoint* joint);
    virtual uint32              GetNumberChild() const;
    virtual atgJoint*           GetChild(uint32 index);

    const Matrix&               GetMatrix() const { return _transform; }

    void                        UpdateJointTransform();

    static void                 DrawJoint(atgJoint* rootJoint, int dep = 0);

    int8    _id;    //>debug ¡Ÿ ±ÃÌº”
protected:
typedef std::vector<atgJoint*> jointContainer;
    atgJoint* _parent;
    jointContainer _childs;
    float (*_pivot)[3];
    float (*_translation)[3];
    float (*_rotation)[4];
    float (*_scaling)[3];
    Matrix _transform; // joint world transform
};


class atgJointAnimData
{
public:
    enum  InterpolationType
    {
        JADIT_NONE = 0,
        JADIT_LINEAR,
        JADIT_HERMITE,
        JADIT_BEZIER,
    };

    static void                 UpdateAnim(atgJoint* joint, atgJointAnimData* data, uint32 time, uint32 starTime, uint32 endTime)
    {
        UpdateAnimTranslation(joint, data, time, starTime, endTime);
        UpdateAnimRotation(joint, data, time, starTime, endTime);
        UpdateAnimScaling(joint, data, time, starTime, endTime);
    }
                                
    virtual uint32              GetNumberTranslationKey() { return 0; }
    virtual uint32              GetTimeTranslationKey(int keyIndex) { return 0; }

    virtual uint32              GetNumberRotationKey() { return 0; }
    virtual uint32              GetTimeRotationKey(int keyIndex) { return 0; }

    virtual uint32              GetNumberScalingKey() { return 0; }
    virtual uint32              GetTimeScalingKey(int keyIndex) { return 0; }       

    virtual uint32              GetKeyTranslationInterpolationType(int keyIndex) { return 0; }
    virtual uint32              GetKeyRotationInterpolationType(int keyIndex) { return 0; }
    virtual uint32              GetKeyScalingInterpolationType(int keyIndex) { return 0; }

    virtual void                GetKeyTranslation(float (*translation)[3], float (*control1)[3], float (*control2)[3],int keyIndex) {}
    virtual void                GetKeyRotation(float (*rotation)[4], float (*control1)[4], float (*control2)[4],int keyIndex) {}
    virtual void                GetKeyScaling(float (*scaling)[3],float (*control1)[3], float (*control2)[3], int keyIndex) {}
protected:
    static void                 UpdateAnimTranslation(atgJoint* joint, atgJointAnimData* data, uint32 time, uint32 starTime, uint32 endTime);
    static void                 UpdateAnimRotation(atgJoint* joint, atgJointAnimData* data, uint32 time, uint32 starTime, uint32 endTime);
    static void                 UpdateAnimScaling(atgJoint* joint, atgJointAnimData* data, uint32 time, uint32 starTime, uint32 endTime);

    float _translation[3];
    float _rotation[4];
    float _scaling[3];
};


class atgSkinAnimation
{
public:
    atgSkinAnimation(void);
    virtual ~atgSkinAnimation(void);
    virtual void                Play(uint8 index);
    virtual void                Play(const char* name);
    virtual void                Update(float elapsedTime);
    virtual float               GetAimationTime();

    virtual uint8               GetNumberAnimation() const;
    virtual const char*         GetName(uint8 index) const;
    virtual float               GetStartTime(uint8 index) const;
    virtual float               GetEndTime(uint8 index) const;
protected:
    void                        Reset();
protected:
    int _curAnimationIndex;
    float _elapsedTime;
};

