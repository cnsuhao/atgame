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

atgDirectionalLight::atgDirectionalLight(Vec3& direction)
{
    _type = LT_Directional;
    SetDirection(direction);
}

void atgDirectionalLight::DebugDraw()
{
    Vec3 position(_direction * 10.0f);
    g_Renderer->DrawLine(Vec3Zero.m, position.m, Vec3One.m);

    Vec3 a(Vec3Zero);
    Vec3 b(position);
    a.x -= 10.0f; b.x -= 10.0f;
    g_Renderer->DrawLine(a.m, b.m, Vec3One.m);
    a.x += 20.0f; b.x += 20.0f;
    g_Renderer->DrawLine(a.m, b.m, Vec3One.m);

    a = Vec3Zero; b = position;
    a.z -= 10.0f; b.z -= 10.0f;
    g_Renderer->DrawLine(a.m, b.m, Vec3One.m);
    a.z += 20.0f; b.z += 20.0f;
    g_Renderer->DrawLine(a.m, b.m, Vec3One.m);
}

atgPointLight::atgPointLight()
{
    _type = LT_Point;
    SetPosition(Vec3Zero);
    SetRange(50.f);
}

atgPointLight::atgPointLight(Vec3& position, float range)
{
    _type = LT_Point;
    SetPosition(position);
    SetRange(range);
}

void atgPointLight::DebugDraw()
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

atgSpotLight::atgSpotLight()
{
    _type = LT_Spot;
    _outerCone = 30.0f;
    _innerCone = _outerCone / 2.0f;
}

atgSpotLight::atgSpotLight(Vec3& position, Vec3& direction, float range, float outerCone, float innerCone):atgPointLight(position,range)
{
    _type = LT_Spot;
    _outerCone = outerCone;
    _innerCone = innerCone;
}

void atgSpotLight::DebugDraw()
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