#include "atgBase.h"
#include "atgCamera.h"

atgCamera::atgCamera(void)
{
    _needUpdateView = true;
    _updateViewByAngle = false;
    _needUpdateProj = true;

    _position.Set(0.0f, 100, 100.0f);

    _forward = _position;
    VecScale(_forward.m, -1.0f, _forward.m);
    VecNormalize(_forward.m);

    _up.Set(0.0f, 1.0f, 0.0f);
    SetForward(_forward.m);

    _eulerAngle.Set(0.0f, 0.0, 0.0f);

    _fov_y = 90.0f;
    _aspect = 4.0f / 3.0f;
    _zNear = 0.1f;
    _zFar = 1000.0f;

    Update();
}

atgCamera::~atgCamera(void)
{
}

void atgCamera::SetLookAt( const Vec3& Position, const Vec3& Focus, const Vec3& UpAxis )
{
    _position = Position;

    VecSub(Focus.m, _position.m, _forward.m);
    VecNormalize(_forward.m);

    _up = UpAxis;
    VecNormalize(_up.m);

    Vec3 right;
    VecCross(_up.m, _forward.m, right.m);
    VecNormalize(right.m);

    VecCross(_forward.m, right.m, _up.m);
    VecNormalize(_up.m);

    _needUpdateView = true;
    _updateViewByAngle = false;
}

void atgCamera::SetPosition( const Vec3& Position )
{
    _position = Position;

    _needUpdateView = true;
}

void atgCamera::SetForward( const Vec3& Forward )
{
    _forward = Forward;

    Vec3 right;
    VecCross(_up.m, _forward.m, right.m);
    VecNormalize(right.m);

    VecCross(_forward.m, right.m, _up.m);
    VecNormalize(_up.m);

    _needUpdateView = true;
    _updateViewByAngle = false;
}

void atgCamera::SetUp( const Vec3& UpAxis )
{
    _up = UpAxis;
    VecNormalize(_up.m);

    Vec3 right;
    VecCross(_up.m, _forward.m, right.m);
    VecNormalize(right.m);

    VecCross(right.m, _up.m, _forward.m);
    VecNormalize(_forward.m);

    _needUpdateView = true;
    _updateViewByAngle = false;
}

void atgCamera::SetYaw( float Yaw )
{
    _eulerAngle.y = Yaw;

    _needUpdateView = true;
    _updateViewByAngle = true;
}

void atgCamera::SetPitch( float Pitch )
{
    _eulerAngle.x = Pitch;

    _needUpdateView = true;
    _updateViewByAngle = true;
}

void atgCamera::SetRoll( float Roll )
{
    _eulerAngle.z = Roll;

    _needUpdateView = true;
    _updateViewByAngle = true;
}

void atgCamera::SetRotation( const Quat& rotation )
{
    Matrix mat;
    QuatToMat(rotation.m, mat.m);

    mat.GetColumn3(1, _up);
    mat.GetColumn3(2, _forward);

    _needUpdateView = true;
}

const Quat& atgCamera::GetRotation() const
{
    Matrix mat(MatrixIdentity);
    Vec3 right;
    VecCross(_up.m, _forward.m, right.m);
    VecNormalize(right.m);

    mat.SetColumn3(0, right);
    mat.SetColumn3(1, _up);
    mat.SetColumn3(2, _forward);

    static Quat q;
    MatToQuat(mat.m, q.m);

    return q;
}


void atgCamera::Update()
{
    if (_needUpdateView)
    {
        _UpdateView();
        _needUpdateView = false;
    }

    if (_needUpdateProj)
    {
        _UpdateProj();
        _needUpdateProj = false;
    }
}

void atgCamera::_UpdateView()
{
    if (_updateViewByAngle)
    {
        Quat q;
        QuatFromEulers(_eulerAngle.m, q.m);

        _up      = q.GetColumn1();
        _forward = q.GetColumn2();
        _updateViewByAngle = false;
    }

    Vec3 at;
    VecAdd(_position.m, _forward.m, at.m);
    ::LookAt(_position.m, at.m, _up.m, _viewMat.m);
}

void atgCamera::_UpdateProj()
{
    Perspective(_fov_y, _aspect, _zNear, _zFar, _projMat.m);
}

void atgCamera::SetProjection( float Fov_y, float Aspect, float ZNear, float ZFar )
{
    _fov_y = Fov_y;
    _aspect = Aspect;
    _zNear = ZNear;
    _zFar = ZFar;

    _UpdateProj();
}

void atgCamera::SetFov( float Fov_y )
{
    _fov_y = Fov_y;

    _needUpdateProj = true;
}

void atgCamera::SetAspect( float Aspect )
{
    _aspect = Aspect;

    _needUpdateProj = true;
}

void atgCamera::SetClipNear( float ZNear )
{
    _zNear = ZNear;

    _needUpdateProj = true;
}

void atgCamera::SetClipFar( float ZFar )
{
    _zFar = ZFar;

    _needUpdateProj = true;
}




