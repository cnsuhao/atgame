#pragma once

#include "atgMath.h"

class atgCamera
{
public:
    atgCamera(void);
    ~atgCamera(void);

    void                SetLookAt(const atgVec3& Position, const atgVec3& Focus, const atgVec3& UpAxis);
    void                SetPosition(const atgVec3& Position);
    const atgVec3&      GetPosition() const { return _position; }
    void                SetForward(const atgVec3& Forward);
    const atgVec3&      GetForward() const { return _forward; }
    const atgVec3&      GetRight() const { static atgVec3 tempRight; tempRight = _up.Cross(_forward); return tempRight; }
    void                SetUp(const atgVec3& UpAxis);
    const atgVec3&      GetUp() const { return _up; }

    void                SetYaw(float Yaw);
    float               GetYaw() const { return _eulerAngle.y; }
    void                SetPitch(float Pitch);
    float               GetPitch() const { return _eulerAngle.x; }
    void                SetRoll(float Roll);
    float               GetRoll() const { return _eulerAngle.z; }


    void                SetRotation(const atgQuaternion& rotation);
    const atgQuaternion& GetRotation() const;

    void                SetProjection(float Fov_y, float Aspect, float ZNear, float ZFar);
    void                SetFov(float Fov_y);
    void                SetAspect(float Aspect);
    float               GetClipNear() const { return _zNear; }
    void                SetClipNear(float ZNear);
    float               GetClipFar() const { return _zFar; }
    void                SetClipFar(float ZFar);

    void                Update();

    const atgMatrix&    GetView() { if(_needUpdateView) { Update(); } return _viewMat; }
    const atgMatrix&    GetProj() { if(_needUpdateProj) { Update(); } return _projMat; }

protected:
    void                _UpdateView();
    void                _UpdateProj();

    bool    _needUpdateView;
    bool    _updateViewByAngle;
    bool    _needUpdateProj;

    atgVec3    _position;
    atgVec3    _forward;
    atgVec3    _up;

    atgMatrix  _viewMat;
    atgMatrix  _projMat;

    atgVec3    _eulerAngle;

    float  _fov_y, _aspect, _zNear, _zFar;
};

