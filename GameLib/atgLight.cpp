#include "atgBase.h"
#include "atgLight.h"
#include "atgRenderer.h"

atgLight::atgLight()
{
    _color = Vec3One;
    _specular = Vec3Zero;

    _intensity = 1.0f;
}

atgLight::~atgLight(void)
{
}

void atgLight::DebugDraw()
{
    /*
    if (_type == LT_Directional)
    {

    }
    else
    {
        Vec3 direction(_position);
        direction.Scale(-1.0f);
        Vec3 left;
        VecCross(direction.m, VectorUp, left.m);
        Vec3 p;
        Quat q;
        float angle = 0.0f;
        for (float i = 0; i < 10; i += 1.0f)
        {
            QuatFromAxisAngle(direction.m, angle, q.m);
            VecRotate(p.m, left.m, q.m);
            g_Renderer->DrawLine(p.m, _position.m, Vec3One.m);
            angle += 36.0f;
        }

        g_Renderer->DrawLine(Vec3Zero.m, _position.m, Vec3One.m);
    }
    */
}

atgDirectionalLight::atgDirectionalLight()
{
    _type = LT_Directional;
    SetDirection(Vec3Down);
}

atgDirectionalLight::atgDirectionalLight(atgVec3& direction)
{
    _type = LT_Directional;
    SetDirection(direction);
}

void atgDirectionalLight::DebugDraw()
{
    atgVec3 position(_direction * 10.0f);
    g_Renderer->BeginLine();
    g_Renderer->AddLine(Vec3Zero, position, Vec3One);

    atgVec3 a(Vec3Zero);
    atgVec3 b(position);
    a.x -= 10.0f; b.x -= 10.0f;
    g_Renderer->AddLine(a, b, Vec3One);
    a.x += 20.0f; b.x += 20.0f;
    g_Renderer->AddLine(a, b, Vec3One);

    a = Vec3Zero; b = position;
    a.z -= 10.0f; b.z -= 10.0f;
    g_Renderer->AddLine(a, b, Vec3One);
    a.z += 20.0f; b.z += 20.0f;
    g_Renderer->AddLine(a, b, Vec3One);
    g_Renderer->EndLine();
}

atgPointLight::atgPointLight()
{
    _type = LT_Point;
    SetPosition(Vec3Zero);
    SetRange(50.f);
}

atgPointLight::atgPointLight(atgVec3& position, float range)
{
    _type = LT_Point;
    SetPosition(position);
    SetRange(range);
}

void atgPointLight::DebugDraw()
{
    atgMatrix oldMat;
    g_Renderer->GetMatrix(oldMat, MD_WORLD);
    g_Renderer->SetMatrix(MD_WORLD, MatrixIdentity);

    g_Renderer->SetPointSize(10);
    g_Renderer->BeginPoint();
    g_Renderer->AddPoint(_position, Vec3One);
    g_Renderer->EndPoint();

    g_Renderer->SetMatrix(MD_WORLD, oldMat);
}

atgSpotLight::atgSpotLight()
{
    _type = LT_Spot;
    _outerCone = 30.0f;
    _innerCone = _outerCone / 2.0f;
}

atgSpotLight::atgSpotLight(atgVec3& position, atgVec3& direction, float range, float outerCone, float innerCone):atgPointLight(position,range)
{
    _type = LT_Spot;
    _outerCone = outerCone;
    _innerCone = innerCone;
}

void atgSpotLight::DebugDraw()
{
    atgVec3 direction(_position * -1.0f);
    atgVec3 left = direction.Cross(Vec3Up);
    atgVec3 p;
    atgQuaternion q;
    float angle = 0.0f;
    g_Renderer->BeginLine();
    for (float i = 0; i < 10; i += 1.0f)
    {
        atgQuaternion q(direction, angle);
        p = q * p;
        g_Renderer->AddLine(p, _position, Vec3One);
        angle += 36.0f;
    }

    g_Renderer->AddLine(Vec3Zero, _position, Vec3One);
    g_Renderer->EndLine();
}