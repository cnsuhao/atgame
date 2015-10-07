#include "atgBase.h"
#include "atgSkinAnimation.h"
#include "atgRenderer.h"

atgJoint::atgJoint( void )
{
    _parent = NULL;
    _pivot = NULL;
    _translation = NULL;
    _rotation = NULL;
    _scaling = NULL;
    _childs.clear();
    MatIdentity(_transform.m);
}

atgJoint::~atgJoint( void )
{

}

void atgJoint::SetParent( atgJoint* parent )
{
    AASSERT(_parent == NULL);
    _parent = parent;
}

atgJoint* atgJoint::GetParent()
{
    return _parent;
}
void atgJoint::AddChild( atgJoint* joint )
{
    if (joint)
    {
        _childs.push_back(joint);
    }
}

uint32 atgJoint::GetNumberChild() const
{
    return _childs.size();
}

atgJoint* atgJoint::GetChild(uint32 index)
{
    AASSERT(index < _childs.size());
    return _childs[index];
}

void atgJoint::UpdateJointTransform()
{
    static Matrix pivotMat;
    static Matrix negativePivotMat;
    static Matrix transMat;
    static Matrix rotaMat;
    static Matrix scalMat;

    MatIdentity(_transform.m);
    float pivot[3] = { (*_pivot)[0], (*_pivot)[1], (*_pivot)[2] };
    MatTranslation(pivot[0], pivot[1], pivot[2], pivotMat.m);
    MatTranslation(-pivot[0], -pivot[1], -pivot[2], negativePivotMat.m);

    MatScaling((*_scaling)[0], (*_scaling)[1], (*_scaling)[2], scalMat.m);  // 缩放
    QuatToMat(*_rotation, rotaMat.m);   // 旋转
    MatTranslation((*_translation)[0], (*_translation)[1], (*_translation)[2], transMat.m); // 平移

    ConcatTransforms(scalMat.m, negativePivotMat.m, scalMat.m); 
    ConcatTransforms(pivotMat.m, scalMat.m, scalMat.m); // 先平移到原点缩放后再平移回来
    ConcatTransforms(rotaMat.m, negativePivotMat.m, rotaMat.m);
    ConcatTransforms(pivotMat.m, rotaMat.m, rotaMat.m); // 先平移到原点旋转后再平移回来

    ConcatTransforms(rotaMat.m, scalMat.m, _transform.m);   // 缩放->旋转
    ConcatTransforms(transMat.m, _transform.m, _transform.m);   // ->平移

    if (_parent)
    {
        ConcatTransforms(_parent->_transform.m, _transform.m, _transform.m);
    }

    //char buf[256];
    //sprintf(buf, "ObjectId=%d\n,(%f,%f,%f,%f)\n(%f,%f,%f,%f)\n(%f,%f,%f,%f)\n(%f,%f,%f,%f)\n", _id, 
    //  _transform.m11,_transform.m21,_transform.m31,_transform.m41,
    //  _transform.m12,_transform.m22,_transform.m32,_transform.m42,
    //  _transform.m13,_transform.m23,_transform.m33,_transform.m43,
    //  _transform.m14,_transform.m24,_transform.m34,_transform.m44);

    //OutputDebugString(buf);

    uint32 numberChild = GetNumberChild();
    for (uint32 i = 0; i < numberChild; i++)
    {
        atgJoint* child = GetChild(i);
        if (child)
        {
            child->UpdateJointTransform();
        }
    }
}

void atgJoint::DrawJoint( atgJoint* joint, int dep )
{
    static float lineColor[9][3] = { {1.0f, 0.0f, 0.0f },
                                     {0.0f, 1.0f, 0.0f },
                                     {0.0f, 0.0f, 1.0f },
                                     {1.0f, 1.0f, 0.0f },
                                     {1.0f, 0.0f, 1.0f },
                                     {1.0f, 0.0f, 0.0f },
                                     {1.0f, 1.0f, 1.0f },
                                     {0.0f, 0.0f, 0.0f },
                                     {0.5f, 1.0f, 0.0f } };
    ++dep;
    int numberChild = joint->GetNumberChild();
    for (int i = 0; i < numberChild; i++)
    {
        atgJoint* child = joint->GetChild(i);
        if (child)
        {
            atgJoint::DrawJoint(child, dep);
        }
    }
    float pivot[3];
    VecTransform(*(joint->_pivot), joint->_transform.m, pivot);
    if (joint->GetParent())
    {
        float parentPivot[3];
        atgJoint* parent = joint->GetParent();
        VecTransform(*(parent->_pivot), parent->_transform.m, parentPivot);
        g_Renderer->DrawLine(parentPivot, pivot, lineColor[dep]);
    }else
    {
        g_Renderer->DrawPoint(pivot, lineColor[dep]);
    }

}

void atgJointAnimData::UpdateAnimTranslation( atgJoint* joint, atgJointAnimData* data, uint32 time, uint32 starTime, uint32 endTime )
{
    static float key1Data[3];
    static float key2Data[3];
    static float control1Data[3];
    static float control2Data[3];
    if (!joint || !data)
        return;
    joint->_translation = &data->_translation;
    int numberKey = data->GetNumberTranslationKey();
    if (numberKey < 2)
    {
        data->_translation[0] = 0.0f;
        data->_translation[1] = 0.0f;
        data->_translation[2] = 0.0f;
        return;
    }
    else
    {
        int keyIndex = -1;
        uint32 key1Time = data->GetTimeTranslationKey(0);
        uint32 key2Time = key1Time;
        int i = 1;
        for (; i < numberKey; i++)
        {
            key2Time = data->GetTimeTranslationKey(i);
            if (starTime <= key1Time && key1Time < endTime && time < key2Time)
            {
                keyIndex = i - 1;
                break;
            }else
            {
                key1Time = key2Time;
            }
        }

        if (-1 != keyIndex && key2Time > endTime)
        {
            data->GetKeyTranslation(&key1Data, NULL, NULL, keyIndex);
            data->_translation[0] = key1Data[0];
            data->_translation[1] = key1Data[1];
            data->_translation[2] = key1Data[2];
            return;
        }

        if (-1 == keyIndex)
        {
            if (starTime <= key1Time &&  key1Time <= endTime && i == numberKey)
            {
                data->GetKeyTranslation(&key1Data, NULL, NULL, data->GetNumberTranslationKey() - 1);
                data->_translation[0] = key1Data[0];
                data->_translation[1] = key1Data[1];
                data->_translation[2] = key1Data[2];
                return;
            }

            data->_translation[0] = 0.0f;
            data->_translation[1] = 0.0f;
            data->_translation[2] = 0.0f;
            return;
        }

        InterpolationType type = (InterpolationType)data->GetKeyTranslationInterpolationType(keyIndex);
        data->GetKeyTranslation(&key1Data, NULL, &control1Data, keyIndex);
        float factor = 0.0f;
        if (key1Time == key2Time)
        {
            type = JADIT_NONE;
        }else
        {
            data->GetKeyTranslation(&key2Data, &control2Data, NULL, keyIndex + 1);
            factor = (time - key1Time)  / ((key2Time - key1Time) * 1.0f);
        }

        switch (type)
        {
        case JADIT_NONE:
            data->_translation[0] = key1Data[0];
            data->_translation[1] = key1Data[1];
            data->_translation[2] = key1Data[2];
            break;
        case JADIT_LINEAR:
            InterpolationLinear(key1Data, key2Data, factor, data->_translation);
            break;
        case JADIT_HERMITE:
            InterpolationHermite(key1Data, key2Data, control1Data, control2Data, factor, data->_translation);
            break;
        case JADIT_BEZIER:
            InterpolationBezier(key1Data, key2Data, control1Data, control2Data, factor, data->_translation);
            break;
        default:
            AASSERT(0);
            break;
        }
        //char buf[256];
        //sprintf(buf, "ObjectId=%d, time=%d, type=%d, TranslationVector(%f,%f,%f)\n",joint->_id, time, type, data->_translation[0], data->_translation[1], data->_translation[2]);
        //OutputDebugString(buf);
    }
}

void atgJointAnimData::UpdateAnimRotation( atgJoint* joint, atgJointAnimData* data, uint32 time, uint32 starTime, uint32 endTime )
{
    static float key1Data[4];
    static float key2Data[4];
    static float control1Data[4];
    static float control2Data[4];
    if (!joint || !data)
        return;
    joint->_rotation = &data->_rotation;
    int numberKey = data->GetNumberRotationKey();
    if (numberKey < 2)
    {
        data->_rotation[0] = 0.0f;
        data->_rotation[1] = 0.0f;
        data->_rotation[2] = 0.0f;
        data->_rotation[3] = 1.0f;
        return;
    }
    else
    {
        int keyIndex = -1;
        uint32 key1Time = data->GetTimeRotationKey(0);
        uint32 key2Time = key1Time;
        int i = 1;
        for (; i < numberKey; i++)
        {
            key2Time = data->GetTimeRotationKey(i);
            if (starTime <= key1Time && key1Time < endTime && time < key2Time)
            {
                keyIndex = i - 1;
                break;
            }else
            {
                key1Time = key2Time;
            }
        }

        if (-1 != keyIndex && key2Time > endTime)
        {
            data->GetKeyRotation(&key1Data, NULL, NULL, keyIndex);
            data->_rotation[0] = key1Data[0];
            data->_rotation[1] = key1Data[1];
            data->_rotation[2] = key1Data[2];
            data->_rotation[3] = key1Data[3];
            return;
        }

        if (-1 == keyIndex)
        {
            if (starTime <= key1Time &&  key1Time <= endTime && i == numberKey)
            {
                data->GetKeyRotation(&key1Data, NULL, NULL, data->GetNumberRotationKey() - 1);
                data->_rotation[0] = key1Data[0];
                data->_rotation[1] = key1Data[1];
                data->_rotation[2] = key1Data[2];
                data->_rotation[3] = key1Data[3];
                return;
            }

            data->_rotation[0] = 0.0f;
            data->_rotation[1] = 0.0f;
            data->_rotation[2] = 0.0f;
            data->_rotation[3] = 1.0f;
            return;
        }

        InterpolationType type = (InterpolationType)data->GetKeyRotationInterpolationType(keyIndex);
        float factor = 0.0f;
        data->GetKeyRotation(&key1Data, &control1Data, &control2Data, keyIndex);
        if (key1Time == key2Time)
        {
            type = JADIT_NONE;
        }else
        {
            data->GetKeyRotation(&key2Data, &control1Data, &control2Data, keyIndex + 1);
            factor = (time - key1Time) / ((key2Time - key1Time) * 1.0f);
        }

        switch (type)
        {
        case JADIT_NONE:
            data->_rotation[0] = key1Data[0];
            data->_rotation[1] = key1Data[1];
            data->_rotation[2] = key1Data[2];
            data->_rotation[3] = key1Data[3];
            break;
        case JADIT_LINEAR:
            QuatSlerp(key1Data, key2Data, factor, data->_rotation);
            break;
        case JADIT_HERMITE:
            QuatSlerp(key1Data, key2Data, factor, data->_rotation);
            //QuatSquad(data->_rotation, key1Data, key2Data, control1Data, control2Data, factor);
            //D3DXQuaternionSquad((D3DXQUATERNION*)data->_rotation, (D3DXQUATERNION*)key1Data, (D3DXQUATERNION*)control1Data, (D3DXQUATERNION*)control2Data, (D3DXQUATERNION*)key2Data, factor);
            break;
        case JADIT_BEZIER:
            QuatSlerp(key1Data, key2Data, factor, data->_rotation);
            //QuatSquad(data->_rotation, key1Data, key2Data, control1Data, control2Data, factor);
            break;
        default:
            AASSERT(0);
            break;
        }
        //char buf[256];
        //sprintf(buf, "ObjectId=%d, time=%d, type=%d, RotationVector(%f,%f,%f,%f)\n",joint->_id, time, type, data->_rotation[0],data->_rotation[1], data->_rotation[2], data->_rotation[3]);
        //OutputDebugString(buf);
    }
}

void atgJointAnimData::UpdateAnimScaling( atgJoint* joint, atgJointAnimData* data, uint32 time, uint32 starTime, uint32 endTime )
{
    static float key1Data[3];
    static float key2Data[3];
    static float control1Data[3];
    static float control2Data[3];
    if (!joint || !data)
        return;
    joint->_scaling = &data->_scaling;
    uint32 numberKey = data->GetNumberScalingKey();
    if (numberKey < 2)
    {
        data->_scaling[0] = 1.0f;
        data->_scaling[1] = 1.0f;
        data->_scaling[2] = 1.0f;
        return;
    }
    else
    {
        int keyIndex = -1;
        uint32 key1Time = data->GetTimeScalingKey(0);
        uint32 key2Time = key1Time;
        uint32 i = 1;
        for (; i < numberKey - 1; i++)
        {
            key2Time = data->GetTimeScalingKey(i);
            if (starTime <= key1Time && key1Time < endTime && time < key2Time)
            {
                keyIndex = i - 1;
                break;
            }else
            {
                key1Time = key2Time;
            }
        }

        if (-1 != keyIndex && key2Time > endTime)
        {
            data->GetKeyScaling(&key1Data, NULL, NULL, keyIndex);
            data->_scaling[0] = key1Data[0];
            data->_scaling[1] = key1Data[1];
            data->_scaling[2] = key1Data[2];
            return;
        }

        if (-1 == keyIndex)
        {
            if (starTime <= key1Time &&  key1Time <= endTime && i == numberKey)
            {
                data->GetKeyScaling(&key1Data, NULL, NULL, data->GetNumberScalingKey() - 1);
                data->_scaling[0] = key1Data[0];
                data->_scaling[1] = key1Data[1];
                data->_scaling[2] = key1Data[2];
                return;
            }
            data->_scaling[0] = 1.0f;
            data->_scaling[1] = 1.0f;
            data->_scaling[2] = 1.0f;
            return;
        }

        InterpolationType type = (InterpolationType)data->GetKeyScalingInterpolationType(keyIndex);
        float factor = 0.0f;
        data->GetKeyScaling(&key1Data, NULL, &control1Data, keyIndex);
        if (key1Time == key2Time)
        {
            type = JADIT_NONE;       
        }else
        {
            data->GetKeyScaling(&key2Data, &control2Data, NULL, keyIndex + 1);
            factor = (time - key1Time) / ((key2Time - key1Time) * 1.0f);
        }

        switch (type)
        {
        case JADIT_NONE:
            data->_scaling[0] = key1Data[0];
            data->_scaling[1] = key1Data[1];
            data->_scaling[2] = key1Data[2];
            break;
        case JADIT_LINEAR:
            InterpolationLinear(key1Data, key2Data, factor, data->_scaling);
            break;
        case JADIT_HERMITE:
            InterpolationHermite(key1Data, key2Data, control1Data, control2Data, factor, data->_scaling);
            break;
        case JADIT_BEZIER:
            InterpolationBezier(key1Data, key2Data, control1Data, control2Data, factor, data->_scaling);
            break;
        default:
            AASSERT(0);
            break;
        }
    }
}


atgSkinAnimation::atgSkinAnimation(void):_curAnimationIndex(0),_elapsedTime(0)
{
}


atgSkinAnimation::~atgSkinAnimation(void)
{
}

void atgSkinAnimation::Play(uint8 index)
{
    int numberAnimation = GetNumberAnimation();
    if (index >= numberAnimation)
    {
        return;
    }
    _curAnimationIndex = index; 
    Reset();
}

void atgSkinAnimation::Play(const char* name)
{
    int numberAnimation = GetNumberAnimation();
    for (int i = 0 ; i < numberAnimation; i++)
    {
        if (0 != strcmp( GetName(i), name))
        {
            continue;
        }
        Play(i);
        break;
    }
}

void atgSkinAnimation::Update( float elapsedTime )
{
    _elapsedTime += elapsedTime * 1000.f;
    float oldElapsedTime = _elapsedTime;
    float endTime = GetEndTime(_curAnimationIndex);
    if (_elapsedTime > endTime)
    {
        Reset();
        //LOG("loop replay =%d\n", _curAnimationIndex);
        _elapsedTime += (oldElapsedTime - endTime);
    }
}

float atgSkinAnimation::GetAimationTime()
{
    return _elapsedTime;
}

uint8 atgSkinAnimation::GetNumberAnimation() const
{
    return 0;
}

const char* atgSkinAnimation::GetName( uint8 index ) const
{
    return "";
}

float atgSkinAnimation::GetStartTime( uint8 index ) const
{
    return 0;
}

float atgSkinAnimation::GetEndTime( uint8 index ) const
{
    return 0;
}

void atgSkinAnimation::Reset()
{
    _elapsedTime = GetStartTime(_curAnimationIndex);
}



