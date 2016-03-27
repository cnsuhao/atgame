#include "atgBase.h"
#include "atgCamera.h"

atgCamera::atgCamera(void)
{
    _needUpdateView = true;
    _updateViewByAngle = false;
    _needUpdateProj = true;

    _position.Set(0.0f, 100.0f, 100.0f);

    _forward = _position * -1.0f;
    _forward.Normalize();

    _up.Set(0.0f, 1.0f, 0.0f);
    SetForward(_forward.m);
    
    atgMatrix mat(MatrixIdentity);
    mat.SetColumn3(0, GetRight().m);
    mat.SetColumn3(1, _up.m);
    mat.SetColumn3(2, _forward.m);

    atgQuaternion q;
    mat.Get(q);
    q.GetEulerAngle(_eulerAngle);

    _fov_y = 90.0f;
    _aspect = 4.0f / 3.0f;
    _zNear = 0.1f;
    _zFar = 1000.0f;

    Update();
}

atgCamera::~atgCamera(void)
{
}

void atgCamera::SetLookAt( const atgVec3& Position, const atgVec3& Focus, const atgVec3& UpAxis )
{
    _position = Position;

    _forward = Focus - _position;
    _forward.Normalize();

    _up = UpAxis;
    _up.Normalize();

    atgVec3 right;
    right = _up.Cross(_forward);
    right.Normalize();

    _up = _forward.Cross(right.m);
    _up.Normalize();

    _needUpdateView = true;
    _updateViewByAngle = false;
}

void atgCamera::SetPosition( const atgVec3& Position )
{
    _position = Position;

    _needUpdateView = true;
}

void atgCamera::SetForward( const atgVec3& Forward )
{
    _forward = Forward;

    atgVec3 right = _up.Cross(_forward);
    right.Normalize();

    _up = _forward.Cross(right);
    _up.Normalize();

    _needUpdateView = true;
    _updateViewByAngle = false;
}

void atgCamera::SetUp( const atgVec3& UpAxis )
{
    _up = UpAxis;
    _up.Normalize();

    atgVec3 right = _up.Cross(_forward);
    right.Normalize();

    _forward = right.Cross(_up);
    _forward.Normalize();

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

void atgCamera::SetRotation( const atgQuaternion & rotation )
{
    atgMatrix mat(rotation);

    mat.GetColumn3(1, _up);
    mat.GetColumn3(2, _forward);

    _needUpdateView = true;
}

const atgQuaternion& atgCamera::GetRotation() const
{
    atgMatrix mat(MatrixIdentity);
    atgVec3 right = _up.Cross(_forward);
    right.Normalize();

    mat.SetColumn3(0, right);
    mat.SetColumn3(1, _up);
    mat.SetColumn3(2, _forward);

    static atgQuaternion q;
    mat.Get(q);

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
        atgQuaternion q(_eulerAngle.x, _eulerAngle.y, _eulerAngle.z);

        _up      = q.GetColumn1();
        _forward = q.GetColumn2();
        _updateViewByAngle = false;
    }
    else
    {
        atgMatrix mat(MatrixIdentity);
        mat.SetColumn3(0, GetRight().m);
        mat.SetColumn3(1, _up.m);
        mat.SetColumn3(2, _forward.m);

        atgQuaternion q;
        mat.Get(q);
        q.GetEulerAngle(_eulerAngle);
    }
    atgVec3 at = _position + _forward;
    atgMatrix::LookAt(_position, at, _up, _viewMat);
}

void atgCamera::_UpdateProj()
{
    atgMatrix::Perspective(_fov_y, _aspect, _zNear, _zFar, _projMat);
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




