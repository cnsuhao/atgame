#pragma once

#include "atgMath.h"

enum LightType
{
    LT_Directional, // 方向光
    LT_Point,       // 点光
    LT_Spot,        // 聚光灯
};

class atgLight
{
protected:
    atgLight();
public:
    virtual ~atgLight(void);

    inline LightType        GetType() const;

    inline void             SetColor(const Vec3& color);
    inline const Vec3&      GetColor() const;

    inline void             SetSpecular(const Vec3& color);
    inline const Vec3&      GetSpecular() const;

    inline void             SetIntensity(float scale);
    inline float            GetIntensity() const;

    virtual void            DebugDraw();
protected:

    LightType _type;

    Vec3    _color;
    Vec3    _specular;

    float   _intensity;
};


class atgDirectionalLight : public atgLight
{
public:
    atgDirectionalLight();
    atgDirectionalLight(Vec3& direction);

    inline void             SetDirection(const Vec3& direction);
    inline const Vec3&      GetDirection() const;

    virtual void            DebugDraw();
protected:
    Vec3    _direction;
};


class atgPointLight : public atgLight
{
public:
    atgPointLight();
    atgPointLight(Vec3& position, float range);

    inline void             SetPosition(const Vec3& position);
    inline const Vec3&      GetPosition() const;

    inline void             SetRange(float range);
    inline float            GetRange() const;

    virtual void            DebugDraw();
protected:
    Vec3    _position;
    float   _range;
};

//////////////////////////
//
//     o
//    /|\-> outer cone
//   / |\\
//  /  |a\\b
//   inner cone 
//  b线与光源方向的夹角为outer cone
//  a线与光源方向的夹角为inner cone

class atgSpotLight : public atgPointLight
{
public:
    atgSpotLight();
    atgSpotLight(Vec3& position, Vec3& direction, float range, float outerCone, float innerCone);

    inline void             SetDirection(const Vec3& direction);
    inline const Vec3&      GetDirection() const;

    // outer cone 的角度必须小于等于90°
    inline void             SetOuterCone(float degress);
    inline float            GetOuterCone() const;

    // inner cone 的角度必须小于 outer cone 的角度
    inline void             SetInnerCone(float degress);
    inline float            GetInnerCone() const;

    virtual void            DebugDraw();
protected:
    Vec3    _direction;
    float   _innerCone;
    float   _outerCone;
};


inline LightType atgLight::GetType() const
{
    return _type;
}

inline void atgLight::SetColor( const Vec3& color )
{
    _color = color;
}

inline const Vec3& atgLight::GetColor() const
{
    return _color;
}

inline void  atgLight::SetSpecular(const Vec3& color)
{
    _specular = color;
}

inline const Vec3& atgLight::GetSpecular() const
{
    return _specular;
}

inline void atgLight::SetIntensity( float scale )
{
    _intensity = Max(0.0f, scale);
}

inline float atgLight::GetIntensity() const
{
    return _intensity;
}


inline void atgDirectionalLight::SetDirection(const Vec3& direction)
{
    _direction = direction;
}

inline const Vec3& atgDirectionalLight::GetDirection() const
{
    return _direction;
}

inline void atgPointLight::SetPosition(const Vec3& position)
{
    _position = position;
}

inline const Vec3& atgPointLight::GetPosition() const
{
    return _position;
}

inline void atgPointLight::SetRange(float range)
{
    _range = Abs(range);
}

inline float atgPointLight::GetRange() const
{
    return _range;
}

inline void atgSpotLight::SetDirection(const Vec3& direction)
{
    _direction = direction;
}

inline const Vec3& atgSpotLight::GetDirection() const
{
    return _direction;
}

inline void   atgSpotLight::SetOuterCone(float degress)
{
    _outerCone = degress;
}

inline float atgSpotLight::GetOuterCone() const
{
    return _outerCone;
}

inline void atgSpotLight::SetInnerCone(float degress)
{
    _innerCone = degress;
}

inline float atgSpotLight::GetInnerCone() const
{
    return _innerCone;
}