#pragma once

#include <cmath>
#include <cassert>
#include <cstring>

//> 定义图形引擎坐标系。
//> 图形引擎使用右手坐标系
//     +y
//     |_ +x
//    /
//   +z
//
//>正交基: right.cross(up) = forward
//>正交基: up.cross(forward) = right
//>正交基: forward.cross(right) = up
//>图形引擎使用的向量以列为主. 所以向量与矩阵相乘的规则为
//>Matrix * Vector 而不是 Vector * Matrix.

#define FPTR_TO_F4x4(ptr)  (*(float(*)[4][4])(ptr))
#define FPTR_TO_F3(ptr) (*(float(*)[3])(ptr))

class atgMath
{
public:

    //> PI常量
    static const float PI;         //> 3.1415926f
    static const float PI_DIV_2;   //> 1.5707963f
    static const float PI_DIV_4;   //> 0.7853981f
    static const float INV_PI;     //> 1/PI 0.3183098f
    static const float PI_DIV_180; //> (PI / 180.f);
    static const float INV_PI_MUL_180; //> (180.f / PI);
    
    //> 误差常量
    static const float EPSILON;    //> 1e-5f

    template<class T> 
    static inline T         Min(T a, T b);

    template<class T> 
    static inline T         Max(T a, T b);

    template <class T>
    static inline void      Swap(T& v1, T& v2);

    template <class T>
    static inline T         Clamp(T v, T min, T max);

    static inline bool      FloatEqual(float a, float b, float deviation=EPSILON);

    static inline bool      DoubleEqual(double a, float b, double deviation=0.0000000001);

    static inline bool      FloatEqualArray(const float* a, const float* b, int size);

    static inline bool      IsFloatZero(float value);

    static inline bool      IsFloatArray(const float* a, int size); 

    static inline float     DegreesToRadians(float degrees);

    static inline float     RadiansToDegrees(float radians);

    static inline float     Abs(float x);

    static inline float     Cot(float x);


    // for example: (0,1) -> (-1, 1) usage: LinearMapping(0.0f, 0.1f, current, -1.0f, 1.0f);
    static inline float     LinearMapping(float from, float to, float current, float mapfrom, float mapto);


    // interpolation
    static inline float     InterpolationLinear(float v1, float v2, float factor);

    static inline void      InterpolationLinear(const float v1[3], const float v2[3], float factor, float dst[3]);

    static inline float     InterpolationHermite(float v1, float v2, float c1, float c2, float factor);

    static inline void      InterpolationHermite(const float v1[3], const float v2[3], const float c1[3], const float c2[3], float factor, float dst[3]);

    static inline float     InterpolationBezier(float v1, float v2, float c1, float c2, float factor);

    static inline void      InterpolationBezier(const float v1[3], const float v2[3], const float c1[3], const float c2[3], float factor, float dst[3]);

    static bool             IsBetween0And1ForClipZ();
};

//////////////////////////////////////////////////////////////////////////
// inline implement

template<class T>
T atgMath::Min( T a, T b )
{
    return a < b ? a : b; 
}

template<class T>
T atgMath::Max( T a, T b )
{
    return a > b ? a : b;
}

template <class T>
void atgMath::Swap( T& v1, T& v2 )
{
    T temp = v1;
    v1 = v2;
    v2 = temp;
}

template <class T>
T atgMath::Clamp( T v, T min, T max )
{
    assert(min < max);
    if (v < min)
    {
        return min;
    }
    else if (v > max)
    {
        return max;
    }
    return v;
}

inline bool atgMath::FloatEqual(float a, float b, float deviation/*=EPSILON*/)
{
    float dif = a >= b ? a - b : b - a;
    if(dif < deviation)
    {
        return true;
    }
    return false;
}

inline bool atgMath::DoubleEqual(double a, float b, double deviation/*=0.000000001*/)
{
    double dif = a >= b ? a - b : b - a;
    if(dif < deviation)
    {
        return true;
    }
    return false;
}

inline bool atgMath::FloatEqualArray(const float* a, const float* b, int size)
{
    const float* pa = a;
    const float* pb = b;
    for (int i = 0; i < size; ++i)
    {
        if (!FloatEqual(*pa, *pb))
        {
            return false;
        }
        ++pa, ++pb;
    }
    return true;
}

inline bool atgMath::IsFloatZero(float value)
{
    if (value > EPSILON || value < -EPSILON)
    {
        return false;
    }
    return true;
}

inline bool atgMath::IsFloatArray(const float* a, int size)
{
    const float* pa = a;
    for (int i = 0; i < size; ++i)
    {
        if (!IsFloatZero(*pa))
        {
            return false;
        }
        ++pa;
    }
}

inline float atgMath::DegreesToRadians(float degrees)
{
    return degrees * PI_DIV_180;
}

inline float atgMath::RadiansToDegrees(float radians)
{
    return radians * INV_PI_MUL_180;
}

inline float atgMath::Abs(float x)
{
    return abs(x);
}

inline float atgMath::Cot(float x)
{
    return 1.0f / tanf(x);
}


inline float atgMath::LinearMapping(float from, float to, float current, float mapfrom, float mapto)
{
    float scale = (mapto - mapfrom) / (to - from);
    return mapfrom + (current - from) * scale;
}


inline float atgMath::InterpolationLinear(float v1, float v2, float factor)
{
    return (1.0f - factor) * v1 + factor * v2;
}

inline void atgMath::InterpolationLinear(const float v1[3], const float v2[3], float factor, float dst[3])
{
    dst[0] = InterpolationLinear(v1[0], v2[0], factor);
    dst[1] = InterpolationLinear(v1[1], v2[1], factor);
    dst[2] = InterpolationLinear(v1[2], v2[2], factor);
}

/*
 c1-->切线
  |
 v1         v2
             |
             c2-->切线
----------------------
 v1起始点 v2结束点
*/
inline float atgMath::InterpolationHermite(float v1, float v2, float c1, float c2, float factor)
{
    float factorPow2 = factor * factor;

    float factor1 = factorPow2 * (2.0f * factor - 3.0f) + 1.0f; // 2*t^3 - 3*t^2 + 1.0f
    float factor2 = factorPow2 * (factor - 2.0f) + factor;      // t^3 - 2*t^2 + t
    float factor3 = factorPow2 * (factor - 1.0f);               // t^3 - t^2
    float factor4 = factorPow2 * (3.0f - 2.0f * factor);        // -2*t^3 + 3*t^2

    // p(t) = (2*t^3 - 3*t^2 + 1)*p0 + (t^3 - 2*t^2 + t)*m0 + (-2*t^3 + 3*t^2)*p1 + (t^3 - t^2)*m1
    return factor1 * v1 + factor2 * c1 + factor3 * c2 + factor4 * v2;
}


inline void atgMath::InterpolationHermite(const float v1[3], const float v2[3], const float c1[3], const float c2[3], float factor, float dst[3])
{
    float factorPow2 = factor * factor;

    float factor1 = factorPow2 * (2.0f * factor - 3.0f) + 1.0f;
    float factor2 = factorPow2 * (factor - 2.0f) + factor;
    float factor3 = factorPow2 * (factor - 1.0f);
    float factor4 = factorPow2 * (3.0f - 2.0f * factor);

    dst[0] = v1[0] * factor1 + c1[0] * factor2 + c2[0] * factor3 + v2[0] * factor4;
    dst[1] = v1[1] * factor1 + c1[1] * factor2 + c2[1] * factor3 + v2[1] * factor4; 
    dst[2] = v1[2] * factor1 + c1[2] * factor2 + c2[2] * factor3 + v2[2] * factor4;
}

/*
     c1-->控制点

 v1         v2

        c2-->控制点
----------------------
v1起始点 v2结束点
*/
inline float atgMath::InterpolationBezier(float v1, float v2, float c1, float c2, float factor)
{
    float factorPow2 = factor * factor;
    float inverseFactor = 1 - factor;
    float inverseFactorPow2 = inverseFactor * inverseFactor;
    float factor1 = inverseFactorPow2 * inverseFactor;  // (1 - t)^3
    float factor2 = 3.0f * factor * inverseFactorPow2;  // 3*(1 - t)^2
    float factor3 = 3.0f * factorPow2 * inverseFactor;  // 3*(1 - t)*t^2
    float factor4 = factorPow2 * factor;                // t^3

    // p(t) = (1 - t)^3*p0 + 3*(1 - t)^2*m0 + 3*(1 - t)*t^2*m1 + (t^3)*p1
    return factor1 * v1 + factor2 * c1 + factor3 * c2 + factor4 *v2;
}

inline void atgMath::InterpolationBezier(const float v1[3], const float v2[3], const float c1[3], const float c2[3], float factor, float dst[3])
{
    float factorPow2 = factor * factor;
    float inverseFactor = 1 - factor;
    float inverseFactorPow2 = inverseFactor * inverseFactor;
    float factor1 = inverseFactorPow2 * inverseFactor;
    float factor2 = 3.0f * factor * inverseFactorPow2;
    float factor3 = 3.0f * factorPow2 * inverseFactor;
    float factor4 = factorPow2 * factor;

    dst[0] = v1[0] * factor1 + c1[0] * factor2 + c2[0] * factor3 + v2[0] * factor4;
    dst[1] = v1[1] * factor1 + c1[1] * factor2 + c2[1] * factor3 + v2[1] * factor4; 
    dst[2] = v1[2] * factor1 + c1[2] * factor2 + c2[2] * factor3 + v2[2] * factor4;
}


struct atgVec2
{
    union
    {
        float m[2];
        struct
        {
            float x;
            float y;
        };
    };

    atgVec2():x(0.0f),y(0.0f) { }
    atgVec2(float x, float y):x(x),y(y) { }
    atgVec2(const float v[2]):x(v[0]),y(v[1]) { }
    atgVec2(const atgVec2& v):x(v.x),y(v.y) { }

    atgVec2& operator =(const atgVec2& v){  x = v.x; y = v.y;  return *this; }
};

struct atgVec3
{
    union
    {
        float m[3];
        struct
        {
            float x;
            float y;
            float z;
        };
    };

    atgVec3():x(0.0f),y(0.0f),z(0.0f) { }
    atgVec3(float x, float y, float z):x(x),y(y),z(z) { }
    atgVec3(const float v[3]):x(v[0]),y(v[1]),z(v[2]) { }
    atgVec3(const atgVec3& v):x(v.x),y(v.y),z(v.z) { }

    void Set(const float p[3]) { x = p[0], y = p[1], z = p[2]; }
    void Set(float x, float y, float z) { m[0] = x; m[1] = y; m[2] = z; }

    atgVec3& operator =(const atgVec3& v){  x = v.x; y = v.y; z = v.z;  return *this; }

    // v0 dot v1 = |v0|*|v1|*cos(a)
    inline float Dot(const atgVec3& v) const
    {
        return this->x*v.x + this->y*v.y + this->z*v.z;
    }

    // v0 cross v1 = |v0|*|v1|*sin(a) * n
    inline atgVec3 Cross(const atgVec3& v) const
    {
        float temp[3];
        temp[0] = this->y * v.z - this->z * v.y;
        temp[1] = this->z * v.x - this->x * v.z;
        temp[2] = this->x * v.y - this->y * v.x;

        return atgVec3(temp);
    }

    inline float Length()
    {
        return sqrtf(Dot(*this));
    }

    inline atgVec3& Normalize()
    {

        float length = Length();
        if (atgMath::FloatEqual(length, 1.0f))
            return *this;
#ifdef _DEBUG
        assert(!atgMath::IsFloatZero(length));
#endif // _DEBUG

        //if (length < atgMath::EPSILON) length = atgMath::EPSILON;
        const float invLen = 1.0f / length;
        x *= invLen;
        y *= invLen;
        z *= invLen;

        return *this;
    }

    // Vproj = length(v0) * cos(a) * normalize(v1) =>
    // => ((v0 dot v1) / length(v1)) * (v1 / length(v1));
    inline atgVec3 Projction(const atgVec3& v0, const atgVec3& v1)
    {
        float p = v0.Dot(v1);
        float LenSqure = v1.Dot(v1);
        float invLenSq = 1.0f / LenSqure;
        this->x = (p * v1.x) * invLenSq;
        this->y = (p * v1.y) * invLenSq;
        this->z = (p * v1.z) * invLenSq;
    }

    // Vproj = ((v0 dot v1) / length(v1)) * (v1 / length(v1)) 
    // if v1 is normalized , so length(v1) euq 1.0 => (v0 dot v1) * v1 
    //=> (v0 dot v1Nomralized) * v1Nomralized
    //inline atgVec3 Projction(const atgVec3 v0, const float v1Nomralized)
    //{
    //    float p = v0.dot(v1Nomralied);
    //    return v1Nomralied * p;
    //}

    atgVec3 operator -() const
    {
        return atgVec3(-x, -y, -z);
    }
};


inline bool operator == (const atgVec3& v1, const atgVec3& v2)
{
    return atgMath::FloatEqualArray(v1.m, v2.m, 3);
}

inline bool operator != (const atgVec3& v1, const atgVec3& v2)
{
    return !atgMath::FloatEqualArray(v1.m, v2.m, 3);
}

inline atgVec3 operator +(const atgVec3& v1, const atgVec3& v2)
{
    return atgVec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline atgVec3 operator -(const atgVec3& v1, const atgVec3& v2)
{
    return atgVec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline atgVec3 operator *(const atgVec3& v1, float s)
{
    return atgVec3(v1.x * s, v1.y * s, v1.z * s);
}

inline atgVec3 operator /(const atgVec3& v1, float s)
{
#ifdef _DEBUG
    assert(!atgMath::IsFloatZero(s));
#endif // _DEBUG

    return atgVec3(v1.x / s, v1.y / s, v1.z / s);
}

inline atgVec3 operator *(const atgVec3& v1, const atgVec3& v2)
{
    return atgVec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline atgVec3 operator /(const atgVec3& v1, const atgVec3& v2)
{

#ifdef _DEBUG
    assert(!atgMath::IsFloatArray(v2.m, 3));
#endif // _DEBUG

    return atgVec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

inline atgVec3& operator +=(atgVec3& v1, const atgVec3& v2)
{
    v1.x += v2.x; v1.y += v2.y; v1.z += v2.z;
    return v1;
}

inline atgVec3& operator -=(atgVec3& v1, const atgVec3& v2)
{
    v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z;
    return v1;
}

inline atgVec3& operator *=(atgVec3& v1, float s)
{
    v1.x *= s; v1.y *= s; v1.z *= s;
    return v1;
}

inline atgVec3& operator /=(atgVec3& v1, float s)
{
#ifdef _DEBUG
    assert(!atgMath::IsFloatZero(s));
#endif // _DEBUG

    v1.x /= s, v1.y /= s, v1.z /= s;
    return v1;
}

extern const atgVec3 Vec3One;      //{ 1.0f,  1.0f,  1.0f }
extern const atgVec3 Vec3Zero;     //{ 0.0f,  0.0f,  0.0f }
extern const atgVec3 Vec3Left;     //{-1.0f,  0.0f,  0.0f }
extern const atgVec3 Vec3Right;    //{ 1.0f,  0.0f,  0.0f }
extern const atgVec3 Vec3Up;       //{ 0.0f,  1.0f,  0.0f }
extern const atgVec3 Vec3Down;     //{ 0.0f, -1.0f,  0.0f }
extern const atgVec3 Vec3Forward;  //{ 0.0f,  0.0f,  1.0f }
extern const atgVec3 Vec3Back;     //{ 0.0f,  0.0f, -1.0f }

struct atgVec4
{
    union
    {
        float m[4];
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
    };

    atgVec4():x(0.0f),y(0.0f),z(0.0f),w(0.0f) { }
    atgVec4(float x, float y, float z, float w):x(x),y(y),z(z),w(w) { }
    atgVec4(const float v[4]):x(v[0]),y(v[1]),z(v[2]),w(v[3]) { }
    atgVec4(const atgVec3& v, float w):x(v.x),y(v.y),z(v.z),w(w) { }

    void Set(const float p[4]) { x = p[0]; y = p[1]; z = p[2]; w = p[3]; }
    void Set(float x, float y, float z, float w) { m[0] = x; m[1] = y; m[2] = z, m[3] = w; }

    atgVec4& operator =(const atgVec4& v) { x = v.x; y = v.y, z = v.z, w = v.w; return *this; }

};


inline bool operator == (const atgVec4& v1, const atgVec4& v2)
{
    return atgMath::FloatEqualArray(v1.m, v2.m, 4);
}

inline bool operator != (const atgVec4& v1, const atgVec4& v2)
{
    return !atgMath::FloatEqualArray(v1.m, v2.m, 4);
}

inline atgVec4 operator +(const atgVec4& v1, const atgVec4& v2)
{
    return atgVec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

inline atgVec4 operator -(const atgVec4& v1, const atgVec4& v2)
{
    return atgVec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

inline atgVec4 operator *(const atgVec4& v1, float s)
{
    return atgVec4(v1.x * s, v1.y * s, v1.z * s, v1.w * s);
}

inline atgVec4 operator /(const atgVec4& v1, float s)
{
#ifdef _DEBUG
    assert(!atgMath::IsFloatZero(s));
#endif // _DEBUG

    return atgVec4(v1.x / s, v1.y / s, v1.z / s, v1.w / s);
}

inline atgVec4 operator *(const atgVec4& v1, const atgVec4& v2)
{
    return atgVec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

inline atgVec4 operator /(const atgVec4& v1, const atgVec4& v2)
{

#ifdef _DEBUG
    assert(!atgMath::IsFloatArray(v2.m, 4));
#endif // _DEBUG

    return atgVec4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

inline atgVec4& operator +=(atgVec4& v1, const atgVec4& v2)
{
    v1.x += v2.x; v1.y += v2.y; v1.z += v2.z; v1.w += v2.w;
    return v1;
}

inline atgVec4 operator -(atgVec4& v1, const atgVec4& v2)
{
    v1.x -= v2.x; v1.y -= v2.y; v1.z -= v2.z; v1.w -= v2.w;
    return v1;
}

inline atgVec4 operator *(atgVec4& v1, float s)
{
    v1.x *= s; v1.y *= s; v1.z *= s; v1.w *= s;
    return v1;
}

inline atgVec4 operator /(atgVec4& v1, float s)
{
#ifdef _DEBUG
    assert(!atgMath::IsFloatZero(s));
#endif // _DEBUG

    v1.x /= s; v1.y /= s; v1.z /= s; v1.w /= s;
    return v1;
}


inline atgVec4 Vec3ToVec4(const atgVec3& vec3)
{
    return atgVec4(vec3.x, vec3.y, vec3.z, 1.0f);
}

inline atgVec3 Vec4ToVec3(const atgVec4& vec4)
{
    if (vec4.w != 0.0f)
    {
        return atgVec3(vec4.x / vec4.w, vec4.y / vec4.w, vec4.z / vec4.w);
    }
    return Vec3Zero;
}


extern const atgVec4 Vec4One;
extern const atgVec4 Vec4Zero;

struct atgQuaternion
{
    union
    {
        float m[4];
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
    };
    atgQuaternion():x(0.0f),y(0.0f),z(0.0f),w(0.0f) { }
    atgQuaternion(float x, float y, float z, float w):x(x),y(y),z(z),w(w) { }
    atgQuaternion(const float q[4]):x(q[0]),y(q[1]),z(q[2]),w(q[3]) { }
    atgQuaternion(float pitch, float yaw, float roll);
    atgQuaternion(const atgVec3& axis, float angle);
    atgQuaternion(const atgVec3& from, const atgVec3& to);

    atgQuaternion& operator =(const atgQuaternion& q)
    {
        x = q.x; y = q.y; z = q.z; w = q.w;
        return *this;
    }

    void Set(const float q[4]) { x = q[0]; y = q[1]; z = q[2]; w = q[3]; }
    void Set(float x, float y, float z, float w) { m[0] = x, m[1] = y, m[2] = z, m[3] = w; }

    atgVec3 GetColumn0() const { return atgVec3(1.0f - 2.0f*(y*y + z*z), 2.0f*(x*y + w*z), 2.0f*(x*z - w*y)); }
    atgVec3 GetColumn1() const { return atgVec3(2.0f*(x*y - w*z), 1.0f - 2.0f*(x*x + z*z), 2.0f*(y*z + w*x)); }
    atgVec3 GetColumn2() const { return atgVec3(2.0f*(x*z + w*y), 2.0f*(y*z - w*x), 1.0f - 2.0f*(x*x + y*y)); }

    atgVec3 GetRow0() const { return atgVec3(1.0f - 2.0f*(y*y + z*z), 2.0f*(x*y - w*z), 2.0f*(w*y + x*z)); }
    atgVec3 GetRow1() const { return atgVec3(2.0f*(x*y + w*z), 1.0f - 2.0f*(x*x + z*z), 2.0f*(y*z - w*x)); }
    atgVec3 GetRow2() const { return atgVec3(2.0f*(x*z - w*y), 2.0f*(y*z + w*x), 1.0f - 2.0f*(x*x + y*y)); }

    atgQuaternion conjugate() const
    {
        return atgQuaternion(-x, -y, -z, w);
    }

    float Length() const;

    atgQuaternion& Normalize();

    atgQuaternion Normalize() const;

    atgQuaternion& Slerp(const atgQuaternion& q1, const atgQuaternion& q2, float f);

    bool GetEulerAngle(atgVec3& euler) const;

};

inline atgQuaternion operator *(const atgQuaternion& q1, const atgQuaternion& q2)
{
    return atgQuaternion(q1.w*q2.x + q1.x*q2.w + q1.y*q2.z + q1.z*q2.y,
                         q1.w*q2.y + q1.x*q2.z + q1.y*q2.w + q1.z*q2.x,
                         q1.w*q2.z + q1.x*q2.y + q1.y*q2.x + q1.z*q2.w,
                         q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z);
}

atgVec3 operator* (const atgQuaternion& q, const atgVec3& v);


atgVec4 operator* (const atgQuaternion& q, const atgVec4& v);

extern const atgQuaternion QuatOne;
extern const atgQuaternion QuatZero;
extern const atgQuaternion QuatIdentity;

struct atgRay
{
    atgVec3 origin;
    atgVec3 dirction;

    atgRay(const atgVec3& o, const atgVec3& dir):origin(o),dirction(dir){ }
    atgRay(const float o[3], const float dir[3]):origin(o[0], o[1], o[2]),dirction(dir[0], dir[1], dir[2]) { }

    atgRay& operator =(const atgRay& r)
    {
        origin = r.origin;
        dirction = r.dirction;
        return *this;
    }

};


struct atgPlane
{
    union
    {
        float m[4];
        struct
        {
            float A; // normal x component
            float B; // normal y component
            float C; // normal z component
            float D; // D = -n * pointOnPlane
        };
        struct
        {
            atgVec3 n; // normal
            float   d;    // D = -n * pointOnPlane
        };
    };

    //Plane_t(Vec3_t& normal, float distance)  // distance to origin with sign(-/+).
    //{
    //    normal.Normalize();
    //    n[0] = normal.x;
    //    n[1] = normal.y;
    //    n[2] = normal.z;
    //    D = -distance;
    //}

    atgPlane(void):A(0),B(0),C(0),D(0){}

    atgPlane(atgVec3& normal, atgVec3& pointOnPlane)
    {
        n = normal;
        n.Normalize();
        D = (-normal).Dot(pointOnPlane);
    }

    atgPlane(const float p[4]):A(p[0]),B(p[1]),C(p[2]),D(p[3]){ }

    atgPlane& operator =(const atgPlane& p)
    {
        n = p.n;
        d = p.d;
        return *this;
    }

    //>到原点的距离
    float GetOriginDistance() const
    {
        return D;
    }

    //>点到平面的距离
    float Resolve(const atgVec3& v) const
    {
        return v.Dot(n) + D;
    }

    //>点是否在平面的正面
    bool IsFront(const atgVec3& v) const
    {
        return Resolve(v) > atgMath::EPSILON;
    }

    //>点是否在平面的背面
    bool IsBack(const atgVec3& v) const
    {
        return Resolve(v) < -atgMath::EPSILON;
    }

    //>点是否在平面上
    bool OnPlane(const atgVec3& v) const
    {
        return atgMath::IsFloatZero(Resolve(v));
    }

    //>归一化参数平面
    void NormalizePlane()
    {
        float magnitude = n.Length();
        if (!atgMath::IsFloatZero(magnitude - 1.0f))
        {
            A /= magnitude;
            B /= magnitude;
            C /= magnitude;
            D /= magnitude;
        }
    }

    atgPlane& FromPoints(const atgVec3& p1, const atgVec3& p2, const atgVec3& p3) //>p1,p2,p3为顺时针围绕的三个点
    {
        n = (p1 - p2).Cross(p3 - p2);
        n.Normalize();
        D = - n.Dot(p2);
        return *this;
    }

    atgPlane& FromVectors(const atgVec3& v1, const atgVec3& v2, const atgVec3& p) //>向量cross容易顺序出错. 建议不要用此函数构造平面
    {
        n = v1.Cross(v2);
        n.Normalize();
        D = - n.Dot(p);
        return *this;
    }
};

// major-row memory. but vector is major-column,
// so matrix is major-column.
//>游戏引擎使用了行为主的和c语言的数组相同的方式,
//>而opengl的矩阵式以列为主的方式.所以传递给opengl
//>使用时需要转置.
//>| m11 m21 m31 m41 |
//>| m12 m22 m32 m41 | ===> opengl存储模式
//>| m13 m23 m33 m41 |
//>| m14 m24 m34 m41 |

struct atgMatrix
{
    union
    {
        float m[4][4];
        struct
        {
            float m11,m12,m13,m14;
            float m21,m22,m23,m24;
            float m31,m32,m33,m34;
            float m41,m42,m43,m44;
        };
    };
    atgMatrix()
    {
        m11 = m12 = m13 = m14 = 0.0f;
        m21 = m22 = m23 = m24 = 0.0f;
        m31 = m32 = m33 = m34 = 0.0f;
        m41 = m42 = m43 = m44 = 0.0f;
    }

    atgMatrix(float m1, float m2, float m3, float m4,
        float m5, float m6, float m7, float m8,
        float m9, float m10, float m11, float m12,
        float m13, float m24, float m15, float m16):
    m11(m1),m12(m2),m13(m3),m14(m4),
        m21(m5),m22(m6),m23(m7),m24(m8),
        m31(m9),m32(m10),m33(m11),m34(m12),
        m41(m13),m42(m14),m43(m15),m44(m16)
    {
    }

    atgMatrix(const atgMatrix& mat)
    {
        *this = mat;
    }

    atgMatrix(const atgQuaternion& q);

    atgMatrix& operator =(const atgMatrix& mat)
    {
        memcpy(m, mat.m, sizeof(atgMatrix));
        return *this;
    }

    void SetRow3(unsigned int index, const atgVec3& v)
    {
        assert(index < 4);
        m[index][0] = v.x;
        m[index][1] = v.y;
        m[index][2] = v.z;
    }

    void SetColumn3(unsigned int index,const atgVec3& v)
    {
        assert(index < 4);
        m[0][index] = v.x;
        m[1][index] = v.y;
        m[2][index] = v.z;
    }

    void GetColumn3(unsigned int index, atgVec3& v)
    {
        assert(index < 4);
        v.x = m[0][index];
        v.y = m[1][index];
        v.z = m[2][index];
    }

    inline atgMatrix& Identity()
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
        return *this;
    }

    inline atgVec3 Transfrom(const atgVec3& v) const
    {
        atgVec4 temp;
        temp = Transfrom(atgVec4(v, 1.0));
        return atgVec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
    }

    atgVec4 Transfrom(const atgVec4& v) const;

    inline atgMatrix& Transpose()
    {
        atgMatrix temp;
        temp.m[0][0] = m[0][0]; temp.m[0][1] = m[1][0]; temp.m[0][2] = m[2][0]; temp.m[0][3] = m[3][0];
        temp.m[1][0] = m[0][1]; temp.m[1][1] = m[1][1]; temp.m[1][2] = m[2][1]; temp.m[1][3] = m[3][1];
        temp.m[2][0] = m[0][2]; temp.m[2][1] = m[1][2]; temp.m[2][2] = m[2][2]; temp.m[2][3] = m[3][2];
        temp.m[3][0] = m[0][3]; temp.m[3][1] = m[1][3]; temp.m[3][2] = m[2][3]; temp.m[3][3] = m[3][3];

        *this = temp;
        return *this;
    }

    inline atgMatrix& Translation(const atgVec3& t)
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = t.x;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = t.y;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = t.z;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

        return *this;
    }

    inline atgMatrix& Scaling(float s)
    {
        m[0][0] = s;    m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = s;    m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = s;    m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

        return *this;
    }

    inline atgMatrix& Scaling(const atgVec3& s)
    {
        m[0][0] = s.x;  m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = s.y;  m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = s.z;  m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
        return *this;
    }

    inline atgMatrix& RotationX(float angle)
    {
        float cos = cosf(atgMath::DegreesToRadians(angle));
        float sin = sinf(atgMath::DegreesToRadians(angle));

        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] =  cos; m[1][2] = -sin; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] =  sin; m[2][2] =  cos; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

        return *this;
    }

    inline atgMatrix& RotationY(float angle)
    {
        float cos = cosf(atgMath::DegreesToRadians(angle));
        float sin = sinf(atgMath::DegreesToRadians(angle));

        m[0][0] =  cos; m[0][1] = 0.0f; m[0][2] =  sin; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = -sin; m[2][1] = 0.0f; m[2][2] =  cos; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

        return *this;
    }

    inline atgMatrix& RotationZ(float angle)
    {
        float cos = cosf(atgMath::DegreesToRadians(angle));
        float sin = sinf(atgMath::DegreesToRadians(angle));

        m[0][0] =  cos; m[0][1] = -sin; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] =  sin; m[1][1] =  cos; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

        return *this;
    }

    bool CanInverse() const;

    atgMatrix& Inverse();

    atgMatrix& AffineInverse();

    atgMatrix& RotationZXY(float angleRoll, float anglePitch, float angleYaw);

    bool Get(atgQuaternion& q);

    float operator [](unsigned char index) const
    {
        return m[index/4][index%4];
    }


    //perspective matrix
    static inline void      LookAt(const atgVec3& eyePos, const atgVec3& lookAt, const atgVec3& up, atgMatrix& result);

    static inline void      LookAtLH(const atgVec3& eyePos, const atgVec3& lookAt, const atgVec3& up, atgMatrix& result);

    static inline void      LookAtRH(const atgVec3& eyePos, const atgVec3& lookAt, const atgVec3& up, atgMatrix& result);

    // width / height = aspect;
    static inline void      Perspective(float fov_y, float aspect, float zNear, float zFar, atgMatrix& result);

    static inline void      PerspectiveLH(float fov_y, float aspect, float zNear, float zFar, atgMatrix& result);

    static inline void      PerspectiveRH(float fov_y, float aspect, float zNear, float zFar, atgMatrix& result);

    static inline void      OrthoProject(float width, float height, float zNear, float zFar, atgMatrix& result);

    static inline void      OrthoProjectLH(float width, float height, float zNear, float zFar, atgMatrix& result);

    static inline void      OrthoProjectRH(float width, float height, float zNear, float zFar, atgMatrix& result);
};

inline void atgMatrix::LookAt(const atgVec3& eyePos, const atgVec3& lookAt, const atgVec3& up, atgMatrix& result)
{
    atgMatrix::LookAtRH(eyePos, lookAt, up, result);
}

inline void atgMatrix::LookAtLH(const atgVec3& eyePos, const atgVec3& lookAt, const atgVec3& up, atgMatrix& result)
{
    atgVec3 zaxis;
    atgVec3 yaxis;
    atgVec3 xaxis;

    zaxis = lookAt - eyePos;
    zaxis.Normalize();
    yaxis = up;
    yaxis.Normalize();
    xaxis = yaxis.Cross(zaxis);
    xaxis.Normalize();
    yaxis = zaxis.Cross(xaxis);
    yaxis.Normalize();

    result.m[0][0] = xaxis.x; result.m[0][1] = xaxis.y; result.m[0][2] = xaxis.z;
    result.m[1][0] = yaxis.x; result.m[1][1] = yaxis.y; result.m[1][2] = yaxis.z;
    result.m[2][0] = zaxis.x; result.m[2][1] = zaxis.y; result.m[2][2] = zaxis.z;

    result.m[0][3] = -xaxis.Dot(eyePos);
    result.m[1][3] = -yaxis.Dot(eyePos);
    result.m[2][3] = -zaxis.Dot(eyePos);

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;
}

inline void atgMatrix::LookAtRH(const atgVec3& eyePos, const atgVec3& lookAt, const atgVec3& up, atgMatrix& result)
{
    atgVec3 zaxis;
    atgVec3 yaxis;
    atgVec3 xaxis;

    zaxis = eyePos - lookAt;
    zaxis.Normalize();
    yaxis = up;
    yaxis.Normalize();
    xaxis = yaxis.Cross(zaxis);
    xaxis.Normalize();
    yaxis = zaxis.Cross(xaxis);
    yaxis.Normalize();

    result.m[0][0] = xaxis.x; result.m[0][1] = xaxis.y; result.m[0][2] = xaxis.z;
    result.m[1][0] = yaxis.x; result.m[1][1] = yaxis.y; result.m[1][2] = yaxis.z;
    result.m[2][0] = zaxis.x; result.m[2][1] = zaxis.y; result.m[2][2] = zaxis.z;

    result.m[0][3] = -xaxis.Dot(eyePos);
    result.m[1][3] = -yaxis.Dot(eyePos);
    result.m[2][3] = -zaxis.Dot(eyePos);

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;
}

// width / height = aspect;
//>顶点变换后,z/w的值在[0.0-1.0]为可见. 且w大于0.
inline void atgMatrix::Perspective(float fov_y, float aspect, float zNear, float zFar, atgMatrix& result)
{
    atgMatrix::PerspectiveRH(fov_y, aspect, zNear, zFar, result);
}

inline void atgMatrix::PerspectiveLH(float fov_y, float aspect, float zNear, float zFar, atgMatrix& result)
{
    assert(!atgMath::FloatEqual(aspect, 0.0f));

    if (zNear > zFar)
        atgMath::Swap(zNear, zFar);

    if (false == atgMath::IsBetween0And1ForClipZ()) // z=> -1 to 1.
    {
        // Right Hand
        float f_n = 1.0f / (zFar - zNear);
        float yScale = atgMath::Cot(atgMath::DegreesToRadians(fov_y) * 0.5f);
        float xScale = yScale / aspect;

        memset(result.m, 0, sizeof(atgMatrix));

        result.m[0][0] = xScale;
        result.m[1][1] = yScale;
        result.m[2][2] = (zFar + zNear) * f_n;
        result.m[3][2] = 1.0f;
        result.m[2][3] = -2.0f * zFar * zNear * f_n;
    }
    else
    {
        // Left Hand
        float f_n = 1.0f / (zFar - zNear);
        float yScale = atgMath::Cot(atgMath::DegreesToRadians(fov_y) * 0.5f);
        float xScale = yScale / aspect;

        memset(result.m, 0, sizeof(atgMatrix));

        result.m[0][0] = xScale;
        result.m[1][1] = yScale;
        result.m[2][2] = zFar * f_n;
        result.m[3][2] = 1.0f;
        result.m[2][3] = -zNear * zFar * f_n;
    }
}

inline void atgMatrix::PerspectiveRH(float fov_y, float aspect, float zNear, float zFar, atgMatrix& result)
{
    assert(!atgMath::FloatEqual(aspect, 0.0f));

    if (zNear > zFar)
        atgMath::Swap(zNear, zFar);

    if (false == atgMath::IsBetween0And1ForClipZ()) // z=> -1 to 1.
    {
        float n_f = 1.0f / (zNear - zFar);
        float yScale = atgMath::Cot(atgMath::DegreesToRadians(fov_y) * 0.5f);
        float xScale = yScale / aspect;

        memset(result.m, 0, sizeof(atgMatrix));

        result.m[0][0] = xScale;
        result.m[1][1] = yScale;
        result.m[2][2] = (zFar + zNear) * n_f;
        result.m[3][2] = -1.0f;
        result.m[2][3] = 2.0f * zFar * zNear * n_f;
    }
    else
    {
        float n_f = 1.0f / (zNear - zFar);
        float yScale = atgMath::Cot(atgMath::DegreesToRadians(fov_y) * 0.5f);
        float xScale = yScale / aspect;

        memset(result.m, 0, sizeof(atgMatrix));

        result.m[0][0] = xScale;
        result.m[1][1] = yScale;
        result.m[2][2] = zFar * n_f;
        result.m[3][2] = -1.0f;
        result.m[2][3] = zNear * zFar * n_f;
    }
}

inline void atgMatrix::OrthoProject(float width, float height, float zNear, float zFar, atgMatrix& result)
{
    atgMatrix::OrthoProjectRH(width, height, zNear, zFar, result);
}

inline void atgMatrix::OrthoProjectLH(float width, float height, float zNear, float zFar, atgMatrix& result)
{
    assert(!atgMath::FloatEqual(zNear, zFar));
    if (zNear > zFar)
        atgMath::Swap(zNear, zFar);

    if (false == atgMath::IsBetween0And1ForClipZ()) // z=> -1 to 1.
    {
        float n_f = 1.0f / (zNear - zFar);

        memset(result.m, 0, sizeof(atgMatrix));

        result.m[0][0] = 2.0f / width;
        result.m[1][1] = 2.0f / height;
        result.m[2][2] = -2.0f * n_f;
        result.m[2][3] = zFar + zNear * n_f;
        result.m[3][3] = 1.0f;
    }
    else
    {
        float f_n = 1.0f / (zFar - zNear);

        memset(result.m, 0, sizeof(atgMatrix));

        result.m[0][0] = 2.0f / width;
        result.m[1][1] = 2.0f / height;
        result.m[2][2] = f_n;
        result.m[2][3] = -zNear * f_n;
        result.m[3][3] = 1.0f;
    }
}

inline void atgMatrix::OrthoProjectRH(float width, float height, float zNear, float zFar, atgMatrix& result)
{
    assert(!atgMath::FloatEqual(zNear, zFar));
    if (zNear > zFar)
        atgMath::Swap(zNear, zFar);

    if (false == atgMath::IsBetween0And1ForClipZ()) // z=> -1 to 1.
    {
        float f_n = 1.0f / (zFar - zNear);

        memset(result.m, 0, sizeof(atgMatrix));

        result.m[0][0] = 2.0f / width;
        result.m[1][1] = 2.0f / height;
        result.m[2][2] = -2.0f * f_n;
        result.m[2][3] = -(zFar + zNear) * f_n;
        result.m[3][3] = 1.0f;
    }
    else
    {
        float n_f = 1.0f / (zNear - zFar);

        memset(result.m, 0, sizeof(atgMatrix));

        result.m[0][0] = 2.0f / width;
        result.m[1][1] = 2.0f / height;
        result.m[2][2] = n_f;
        result.m[2][3] = zNear * n_f;
        result.m[3][3] = 1.0f;
    }
}

inline atgVec4 operator *(const atgMatrix& mat, const atgVec4& vec)
{
    return mat.Transfrom(vec);
}

inline atgMatrix operator *(const atgMatrix& mat1, const atgMatrix& mat2)
{
    atgMatrix temp;
    
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            temp.m[i][j] = mat1.m[i][0] * mat2.m[0][j] 
                         + mat1.m[i][1] * mat2.m[1][j]
                         + mat1.m[i][2] * mat2.m[2][j]
                         + mat1.m[i][3] * mat2.m[3][j];
        }
    }
    
    /*
    temp.m[0][0] = mat1.m[0][0] * mat2.m[0][0] + mat1.m[0][1] * mat2.m[1][0] + mat1.m[0][2] * mat2.m[2][0] + mat1.m[0][3] * mat2.m[3][0];
    temp.m[0][1] = mat1.m[0][0] * mat2.m[0][1] + mat1.m[0][1] * mat2.m[1][1] + mat1.m[0][2] * mat2.m[2][1] + mat1.m[0][3] * mat2.m[3][1];
    temp.m[0][2] = mat1.m[0][0] * mat2.m[0][2] + mat1.m[0][1] * mat2.m[1][2] + mat1.m[0][2] * mat2.m[2][2] + mat1.m[0][3] * mat2.m[3][2];
    temp.m[0][3] = mat1.m[0][0] * mat2.m[0][3] + mat1.m[0][1] * mat2.m[1][3] + mat1.m[0][2] * mat2.m[2][3] + mat1.m[0][3] * mat2.m[3][3];
    temp.m[1][0] = mat1.m[1][0] * mat2.m[0][0] + mat1.m[1][1] * mat2.m[1][0] + mat1.m[1][2] * mat2.m[2][0] + mat1.m[1][3] * mat2.m[3][0];
    temp.m[1][1] = mat1.m[1][0] * mat2.m[0][1] + mat1.m[1][1] * mat2.m[1][1] + mat1.m[1][2] * mat2.m[2][1] + mat1.m[1][3] * mat2.m[3][1];
    temp.m[1][2] = mat1.m[1][0] * mat2.m[0][2] + mat1.m[1][1] * mat2.m[1][2] + mat1.m[1][2] * mat2.m[2][2] + mat1.m[1][3] * mat2.m[3][2];
    temp.m[1][3] = mat1.m[1][0] * mat2.m[0][3] + mat1.m[1][1] * mat2.m[1][3] + mat1.m[1][2] * mat2.m[2][3] + mat1.m[1][3] * mat2.m[3][3];
    temp.m[2][0] = mat1.m[2][0] * mat2.m[0][0] + mat1.m[2][1] * mat2.m[1][0] + mat1.m[2][2] * mat2.m[2][0] + mat1.m[2][3] * mat2.m[3][0];
    temp.m[2][1] = mat1.m[2][0] * mat2.m[0][1] + mat1.m[2][1] * mat2.m[1][1] + mat1.m[2][2] * mat2.m[2][1] + mat1.m[2][3] * mat2.m[3][1];
    temp.m[2][2] = mat1.m[2][0] * mat2.m[0][2] + mat1.m[2][1] * mat2.m[1][2] + mat1.m[2][2] * mat2.m[2][2] + mat1.m[2][3] * mat2.m[3][2];
    temp.m[2][3] = mat1.m[2][0] * mat2.m[0][3] + mat1.m[2][1] * mat2.m[1][3] + mat1.m[2][2] * mat2.m[2][3] + mat1.m[2][3] * mat2.m[3][3];
    temp.m[3][0] = mat1.m[3][0] * mat2.m[0][0] + mat1.m[3][1] * mat2.m[1][0] + mat1.m[3][2] * mat2.m[2][0] + mat1.m[3][3] * mat2.m[3][0];
    temp.m[3][1] = mat1.m[3][0] * mat2.m[0][1] + mat1.m[3][1] * mat2.m[1][1] + mat1.m[3][2] * mat2.m[2][1] + mat1.m[3][3] * mat2.m[3][1];
    temp.m[3][2] = mat1.m[3][0] * mat2.m[0][2] + mat1.m[3][1] * mat2.m[1][2] + mat1.m[3][2] * mat2.m[2][2] + mat1.m[3][3] * mat2.m[3][2];
    temp.m[3][3] = mat1.m[3][0] * mat2.m[0][3] + mat1.m[3][1] * mat2.m[1][3] + mat1.m[3][2] * mat2.m[2][3] + mat1.m[3][3] * mat2.m[3][3];

    */

    return temp;
}

static bool Intersection(const atgPlane& p1, const atgPlane& p2, const atgPlane& p3, atgVec3& result)
{
    atgVec3 v;
    v = p2.n.Cross(p3.n);
    if (atgMath::IsFloatZero(p1.n.Dot(v)))
        return false;

    atgMatrix mat;
    mat.m[0][0] = p1.n.x; mat.m[0][1] = p1.n.y; mat.m[0][2] = p1.n.z; mat.m[0][3] = 0.0f;
    mat.m[1][0] = p2.n.x; mat.m[1][1] = p2.n.y; mat.m[1][2] = p2.n.z; mat.m[1][3] = 0.0f;
    mat.m[2][0] = p3.n.x; mat.m[2][1] = p3.n.y; mat.m[2][2] = p3.n.z; mat.m[2][3] = 0.0f;
    mat.m[3][0] = 0.0f;   mat.m[3][1] = 0.0f;   mat.m[3][2] = 0.0f;   mat.m[3][3] = 1.0f;

    if(mat.CanInverse())
    {
        mat.Inverse();
        atgVec4 v(-p1.D, -p2.D, -p3.D, 1.0f);
        result = Vec4ToVec3( mat * v );
        return true;
    }

    return false;
}

extern const atgMatrix MatrixIdentity;