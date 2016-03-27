#ifndef _ATG_CAMERA_H_
#define _ATG_CAMERA_H_

#include "atgMath.h"

class atgCamera
{
public:
    atgCamera(void);
    ~atgCamera(void);

    void                SetLookAt(const Vec3& Position, const Vec3& Focus, const Vec3& UpAxis);
    void                SetPosition(const Vec3& Position);
    const Vec3&         GetPosition() const { return _position; }
    void                SetForward(const Vec3& Forward);
    const Vec3&         GetForward() const { return _forward; }
    const Vec3&         GetRight() const { static Vec3 tempRight; VecCross(_up.m, _forward.m, tempRight.m); return tempRight; }
    void                SetUp(const Vec3& UpAxis);
    const Vec3&         GetUp() const { return _up; }

    void                SetYaw(float Yaw);
    void                SetPitch(float Pitch);
    void                SetRoll(float Roll);

    void                SetRotation(const Quat& rotation);
    const Quat&         GetRotation() const;

    void                SetProjection(float Fov_y, float Aspect, float ZNear, float ZFar);
    void                SetFov(float Fov_y);
    void                SetAspect(float Aspect);
    float               GetClipNear() const { return _zNear; }
    void                SetClipNear(float ZNear);
    float               GetClipFar() const { return _zFar; }
    void                SetClipFar(float ZFar);

    void                Update();

    const Matrix&       GetView() { if(_needUpdateView) { Update(); } return _viewMat; }
    const Matrix&       GetProj() { if(_needUpdateProj) { Update(); } return _projMat; }

protected:
    void                _UpdateView();
    void                _UpdateProj();

    bool    _needUpdateView;
    bool    _updateViewByAngle;
    bool    _needUpdateProj;

    Vec3    _position;
    Vec3    _forward;
    Vec3    _up;

    Matrix  _viewMat;
    Matrix  _projMat;

    Vec3    _eulerAngle;

    float  _fov_y, _aspect, _zNear, _zFar;
};


#endif // _ATG_CAMERA_H_
