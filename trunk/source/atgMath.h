#ifndef _ATG_MATH_H_
#define _ATG_MATH_H_

#include <cmath>
#include <cstring>

#ifndef Max
    #define Max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef Min
    #define Min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define MATH_PI     3.1415926f
#define MATH_PI_2   1.5707963f
#define MATH_PI_4   0.7853981f
#define MATH_INV_PI   0.3183098f

#define MATH_PI_DIV_180     (MATH_PI / 180.f);
#define MATH_180_DIV_PI     (180.f / MATH_PI);

#define EPSILON      1e-5f

#define DEG2RAD(d)      DegreesToRadians(d)
#define RAD2DEG(r)      RadiansToDegrees(r)

#define VEC3_SIZE (sizeof(float)*3)
#define VEC4_SIZE (sizeof(float)*4)
#define QUAT_SIZE (sizeof(float)*4)
#define MATRIX44_SIZE (sizeof(float)*16)

#define FPTR_TO_F4x4(ptr)  (*(float(*)[4][4])(ptr))

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

extern const float VectorZero[3];   // { 0.0f,  0.0f,  0.0f };
extern const float VectorOne[3];    // { 1.0f,  1.0f,  1.0f };
extern const float VectorLeft[3];   // {-1.0f,  0.0f,  0.0f };
extern const float VectorRight[3];  // { 1.0f,  0.0f,  0.0f };
extern const float VectorUp[3];     // { 0.0f,  1.0f,  0.0f };
extern const float VectorDown[3];   // { 0.0f, -1.0f,  0.0f };
extern const float VectorForward[3];// { 0.0f,  0.0f,  1.0f };
extern const float VectorBack[3];   // { 0.0f,  0.0f, -1.0f };

inline bool             FloatEqual(float a, float b, float deviation=EPSILON);

inline bool             FloatEqualArray(const float* a, const float* b, int size);

inline bool             IsFloatZero(float value);

inline float            DegreesToRadians(float degrees);

inline float            RadiansToDegrees(float radians);

inline float            Abs(float x);

inline float            Cot(float x);

template <class T>
inline void             Swap(T& v1, T& v2);

template <class T>
inline T                Clamp(T v, T min, T max);

// for example: (0,1) -> (-1, 1) usage: LinearMapping(0.0f, 0.1f, current, -1.0f, 1.0f);
inline float            LinearMapping(float from, float to, float current, float mapfrom, float mapto);

// interpolation
inline float            InterpolationLinear(float v1, float v2, float factor);

inline void             InterpolationLinear(const float v1[3], const float v2[3], float factor, float dst[3]);

inline float            InterpolationHermite(float v1, float v2, float c1, float c2, float factor);

inline void             InterpolationHermite(const float v1[3], const float v2[3], const float c1[3], const float c2[3], float factor, float dst[3]);

inline float            InterpolationBezier(float v1, float v2, float c1, float c2, float factor);

inline void             InterpolationBezier(const float v1[3], const float v2[3], const float c1[3], const float c2[3], float factor, float dst[3]);

// vector {x, y, z}
inline float            VecDot(const float v0[3], const float v1[3]);

inline void             VecCross(const float v0[3], const float v1[3], float result[3]);

inline void             VecProj(const float v0[3], const float v1[3], float result[3]);

inline void             VecAdd(const float v0[3], const float v1[3], float result[3]);

inline void             VecSub(const float v0[3], const float v1[3], float result[3]);

inline void             VecZero(float v[3]);

inline void             VecScale(const float v[3], float scale, float result[3]);

inline void             VecCopy(const float scr[3], float dst[3]);

inline float            VecLength(const float v[3]);

inline void             VecNormalize(float v[3]);

inline void             VecNormalize(const float v[3], float result[3]);

// quaternion or vector4, plane { x , y, z, w }
void                    QuatNormalize(const float quatSrc[4], float quatDst[4]);

void                    QuatFromEulers(const float vec[3], float quat[4]);

void                    QuatFromAxisAngle(const float axis[3], float angle, float quat[4]);

void                    QuatFromTwoVector(const float start[3], const float end[3], float quat[4]);

void                    QuatMultiply(const float quat1[4], const float quat2[4], float result[4]);

void                    QuatToMat(const float quat[4], float matrix[4][4]);

void                    QuatSlerp(const float quat1[4], const float quat2[4], float interpTime, float result[4]);

void                    QuatSquad(const float quat1[4], const float quat2[4],  const float ctrl1[4], const float ctrl2[4], float interpTime, float result[4]);

inline void             QuatCopy(float dst[4], const float scr[4]);

inline float            Vec4Dot(const float v0[4], const float v1[4]);

// Matrix as major column 4x4
inline void             MatIdentity(float dst[4][4]);

inline void             MatTranspose(const float src[4][4], float dst[4][4]);

       bool             MatAffineInverse(const float src[4][4], float dst[4][4]); //>仿射矩阵求逆

       bool             MatInverse(const float src[4][4], float dst[4][4]); //>通用矩阵求逆

inline void             MatTranslation(float tx, float ty, float tz, float dst[4][4]);

inline void             MatScaling(float sx, float sy, float sz, float dst[4][4]);

inline void             MatRotationX(float angle, float dst[4][4]);

inline void             MatRotationY(float angle, float dst[4][4]);

inline void             MatRotationZ(float angle, float dst[4][4]);

void                    MatToQuat(const float matrix[4][4], float quat[4]);

void                    ConcatTransforms(const float mat1[4][4], const float mat2[4][4], float result[4][4]);

// transform function
inline void             VecRotate(const float vec[3], const float mat[4][4], float result[3]);

void                    VecRotate(const float vec[3], const float quat[4], float result[3]);

inline void             VecIRotate(const float vec[3], const float mat[4][4], float result[3]);

inline void             VecTransform(const float vec[3], const float mat[4][4], float result[3]);

inline void             Vec4Transform(const float vec[4], const float mat[4][4], float result[4]);

inline void             VecITransform(const float vec[3], const float mat[4][4], float result[3]);

inline void             LookAt(const float eyePos[3], const float lookAt[3], const float up[3], float result[4][4]);

inline void             LookAtLH(const float eyePos[3], const float lookAt[3], const float up[3], float result[4][4]);

inline void             LookAtRH(const float eyePos[3], const float lookAt[3], const float up[3], float result[4][4]);
// width / height = aspect;
inline void             Perspective(float fov_y, float aspect, float zNear, float zFar, float result[4][4]);

inline void             PerspectiveLH(float fov_y, float aspect, float zNear, float zFar, float result[4][4]);

inline void             PerspectiveRH(float fov_y, float aspect, float zNear, float zFar, float result[4][4]);

inline void             OrthoProject(float width, float height, float zNear, float zFar, float result[4][4]);

inline void             OrthoProjectLH(float width, float height, float zNear, float zFar, float result[4][4]);

inline void             OrthoProjectRH(float width, float height, float zNear, float zFar, float result[4][4]);

//////////////////////////////////////////////////////////////////////////
// inline implement

inline bool FloatEqual(float a, float b, float deviation)
{
    float dif = a >= b ? a - b : b - a;
    if(dif < deviation)
    {
        return true;
    }
    return false;
}

inline bool FloatEqualArray(const float* a, const float* b, int size)
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

inline bool IsFloatZero(float value)
{
    if (value > EPSILON || value < -EPSILON)
    {
        return false;
    }
    return true;
}

inline float DegreesToRadians(float degrees)
{
    return degrees * MATH_PI_DIV_180;
}

inline float RadiansToDegrees(float radians)
{
    return radians * MATH_180_DIV_PI;
}

inline float Abs(float x)
{
    return abs(x);
}

inline float Cot(float x)
{
    return 1.0f / tanf(x);
}

template <class T>
inline void Swap(T& v1, T& v2)
{
    T temp = v1;
    v1 = v2;
    v2 = temp;
}

template <class T>
inline T Clamp(T v, T min, T max)
{
    AASSERT(min < max);
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

inline float LinearMapping(float from, float to, float current, float mapfrom, float mapto)
{
    float scale = (mapto - mapfrom) / (to - from);
    return mapfrom + (current - from) * scale;
}

// v0 * v1 = |v0|*|v1|*cos(a)
inline float VecDot(const float v0[3], const float v1[3])
{
    return v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2];
}

// v0 x v1 = |v0|*|v1|*sin(a) * n
inline void VecCross(const float v0[3], const float v1[3], float result[3])
{
    float temp[3];
    temp[0] = v0[1] * v1[2] - v0[2] * v1[1];
    temp[1] = v0[2] * v1[0] - v0[0] * v1[2];
    temp[2] = v0[0] * v1[1] - v0[1] * v1[0];
    memcpy(result, temp, VEC3_SIZE);
}

// Vproj = length(v0) * cos(a) * normalize(v1) =>
// => ((v0 dot v1) / length(v1)) * (v1 / length(v1));
inline void VecProj(const float v0[3], const float v1[3], float result[3])
{
    float p = VecDot(v0, v1);
    float LenSqure = VecDot(v1, v1);
    float invLenSq = 1.0f / LenSqure;
    result[0] = (p * v1[0]) * invLenSq;
    result[1] = (p * v1[1]) * invLenSq;
    result[2] = (p * v1[2]) * invLenSq;
}

// Vproj = ((v0 dot v1) / length(v1)) * (v1 / length(v1)) => (v0 dot v1Nomralied) * v1Nomralied
//inline void VecProj(const float v0[3], const float v1Nomralied[3], float result[3])
//{
//    float p = VecDot(v0, v1Nomralied);
//    VecScale(v1Nomralied, p, result);
//}

inline void VecAdd(const float v0[3], const float v1[3], float result[3])
{
    result[0] = v0[0] + v1[0];
    result[1] = v0[1] + v1[1];
    result[2] = v0[2] + v1[2];
}

inline void VecSub(const float v0[3], const float v1[3], float result[3])
{
    result[0] = v0[0] - v1[0];
    result[1] = v0[1] - v1[1];
    result[2] = v0[2] - v1[2];
}

inline void VecZero(float v[3])
{
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
}

inline void VecScale(const float v[3], float scale, float result[3])
{
    result[0] = v[0] * scale;
    result[1] = v[1] * scale;
    result[2] = v[2] * scale;
}

inline void VecCopy(const float scr[3], float dst[3])
{
    dst[0] = scr[0];
    dst[1] = scr[1];
    dst[2] = scr[2];
}

inline float VecLength(const float v[3])
{
    return (sqrtf(VecDot(v,v)));
}

inline void VecNormalize(float v[3])
{
    float length = VecLength(v);
    if (FloatEqual(length, 1.0f))
        return;
#ifdef _DEBUG
    AASSERT(!IsFloatZero(length));
#endif // _DEBUG

    if (length < EPSILON) length = EPSILON;
    const float invLen = 1 / length;
    v[0] *= invLen;
    v[1] *= invLen;
    v[2] *= invLen;
}

inline void VecNormalize(const float v[3], float result[3])
{
    float length = VecLength(v);
#ifdef _DEBUG
    AASSERT(!IsFloatZero(length));
#endif // _DEBUG
    if (length < EPSILON) length = EPSILON;
    const float invLen = 1 / length;
    result[0] = v[0] * invLen;
    result[1] = v[1] * invLen;
    result[2] = v[2] * invLen;
}

inline void QuatCopy(const float scr[4], float dst[4])
{
    dst[0] = scr[0];
    dst[1] = scr[1];
    dst[2] = scr[2];
    dst[3] = scr[3];
}

inline float Vec4Dot(const float v0[4], const float v1[4])
{
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2]+ v0[3] * v1[3];
}

inline void MatIdentity(float dst[4][4])
{
    dst[0][0] = 1.0f; dst[0][1] = 0.0f; dst[0][2] = 0.0f; dst[0][3] = 0.0f;
    dst[1][0] = 0.0f; dst[1][1] = 1.0f; dst[1][2] = 0.0f; dst[1][3] = 0.0f;
    dst[2][0] = 0.0f; dst[2][1] = 0.0f; dst[2][2] = 1.0f; dst[2][3] = 0.0f;
    dst[3][0] = 0.0f; dst[3][1] = 0.0f; dst[3][2] = 0.0f; dst[3][3] = 1.0f;
}

inline void MatTranspose(const float src[4][4], float dst[4][4])
{
    float temp[4][4];
    temp[0][0] = src[0][0]; temp[0][1] = src[1][0]; temp[0][2] = src[2][0]; temp[0][3] = src[3][0];
    temp[1][0] = src[0][1]; temp[1][1] = src[1][1]; temp[1][2] = src[2][1]; temp[1][3] = src[3][1];
    temp[2][0] = src[0][2]; temp[2][1] = src[1][2]; temp[2][2] = src[2][2]; temp[2][3] = src[3][2];
    temp[3][0] = src[0][3]; temp[3][1] = src[1][3]; temp[3][2] = src[2][3]; temp[3][3] = src[3][3];
    memcpy(dst, temp, MATRIX44_SIZE);
}

inline void MatTranslation(float tx, float ty, float tz, float dst[4][4])
{
    dst[0][0] = 1.0f; dst[0][1] = 0.0f; dst[0][2] = 0.0f; dst[0][3] = tx;
    dst[1][0] = 0.0f; dst[1][1] = 1.0f; dst[1][2] = 0.0f; dst[1][3] = ty;
    dst[2][0] = 0.0f; dst[2][1] = 0.0f; dst[2][2] = 1.0f; dst[2][3] = tz;
    dst[3][0] = 0.0f; dst[3][1] = 0.0f; dst[3][2] = 0.0f; dst[3][3] = 1.0f;
}

inline void MatScaling(float sx, float sy, float sz, float dst[4][4])
{
    dst[0][0] = sx;   dst[0][1] = 0.0f; dst[0][2] = 0.0f; dst[0][3] = 0.0f;
    dst[1][0] = 0.0f; dst[1][1] = sy;   dst[1][2] = 0.0f; dst[1][3] = 0.0f;
    dst[2][0] = 0.0f; dst[2][1] = 0.0f; dst[2][2] = sz;   dst[2][3] = 0.0f;
    dst[3][0] = 0.0f; dst[3][1] = 0.0f; dst[3][2] = 0.0f; dst[3][3] = 1.0f;
}

inline void MatRotationX(float angle, float dst[4][4])
{
    float cos = cosf(DEG2RAD(angle));
    float sin = sinf(DEG2RAD(angle));
    float temp[4][4];
    temp[0][0] = 1.0f; temp[0][1] = 0.0f; temp[0][2] = 0.0f; temp[0][3] = 0.0f;
    temp[1][0] = 0.0f; temp[1][1] =  cos; temp[1][2] = -sin; temp[1][3] = 0.0f;
    temp[2][0] = 0.0f; temp[2][1] =  sin; temp[2][2] =  cos; temp[2][3] = 0.0f;
    temp[3][0] = 0.0f; temp[3][1] = 0.0f; temp[3][2] = 0.0f; temp[3][3] = 1.0f;
    memcpy(dst, temp, MATRIX44_SIZE);
}

inline void MatRotationY(float angle, float dst[4][4])
{
    float cos = cosf(DEG2RAD(angle));
    float sin = sinf(DEG2RAD(angle));
    float temp[4][4];
    temp[0][0] =  cos; temp[0][1] = 0.0f; temp[0][2] =  sin; temp[0][3] = 0.0f;
    temp[1][0] = 0.0f; temp[1][1] = 1.0f; temp[1][2] = 0.0f; temp[1][3] = 0.0f;
    temp[2][0] = -sin; temp[2][1] = 0.0f; temp[2][2] =  cos; temp[2][3] = 0.0f;
    temp[3][0] = 0.0f; temp[3][1] = 0.0f; temp[3][2] = 0.0f; temp[3][3] = 1.0f;
    memcpy(dst, temp, MATRIX44_SIZE);
}

inline void MatRotationZ(float angle, float dst[4][4])
{
    float cos = cosf(DEG2RAD(angle));
    float sin = sinf(DEG2RAD(angle));
    float temp[4][4];
    temp[0][0] =  cos; temp[0][1] = -sin; temp[0][2] = 0.0f; temp[0][3] = 0.0f;
    temp[1][0] =  sin; temp[1][1] =  cos; temp[1][2] = 0.0f; temp[1][3] = 0.0f;
    temp[2][0] = 0.0f; temp[2][1] = 0.0f; temp[2][2] = 1.0f; temp[2][3] = 0.0f;
    temp[3][0] = 0.0f; temp[3][1] = 0.0f; temp[3][2] = 0.0f; temp[3][3] = 1.0f;
    memcpy(dst, temp, MATRIX44_SIZE);
}

inline void VecRotate(const float vec[3], const float mat[4][4], float result[3])
{
    float temp[3];
    temp[0] = VecDot(mat[0], vec);
    temp[1] = VecDot(mat[1], vec);
    temp[2] = VecDot(mat[2], vec);
    result[0] = temp[0];
    result[1] = temp[1];
    result[2] = temp[2];
}

// rotate by the inverse of the matrix
inline void VecIRotate(const float vec[3], const float mat[4][4], float result[3])
{
    float col1[3] = { mat[0][0], mat[1][0], mat[2][0] };
    float col2[3] = { mat[0][1], mat[1][1], mat[2][1] };
    float col3[3] = { mat[0][2], mat[1][2], mat[2][2] };
    result[0] = VecDot(col1, vec);
    result[1] = VecDot(col2, vec);
    result[2] = VecDot(col3, vec);
}

inline void VecTransform(const float vec[3], const float mat[4][4], float result[3])
{
    float d1 = VecDot(mat[0], vec);
    float d2 = VecDot(mat[1], vec);
    float d3 = VecDot(mat[2], vec);
    result[0] = d1 + mat[0][3];
    result[1] = d2 + mat[1][3];
    result[2] = d3 + mat[2][3];
}

inline void Vec4Transform(const float vec[4], const float mat[4][4], float result[4])
{
    float temp[4];
    temp[0] = Vec4Dot(mat[0], vec);
    temp[1] = Vec4Dot(mat[1], vec);
    temp[2] = Vec4Dot(mat[2], vec);
    temp[3] = Vec4Dot(mat[3], vec);
    memcpy(result, temp, VEC4_SIZE);
}

inline void VecITransform(const float vec[3], const float mat[4][4], float result[3])
{
    float temp[3];
    temp[0] = vec[0] - mat[0][3];
    temp[1] = vec[1] - mat[1][3];
    temp[2] = vec[2] - mat[2][3];
    VecIRotate(temp, mat, result);
}

inline void LookAt(const float eyePos[3], const float lookAt[3], const float up[3], float result[4][4])
{
    LookAtRH(eyePos, lookAt, up, result);
}

inline void LookAtLH(const float eyePos[3], const float lookAt[3], const float up[3], float result[4][4])
{
    float zaxis[3];
    float yaxis[3];
    float xaxis[3];

    VecSub(lookAt, eyePos, zaxis);

    VecNormalize(zaxis);
    VecCopy(up, yaxis);
    VecNormalize(yaxis);
    VecCross(yaxis, zaxis, xaxis);
    VecNormalize(xaxis);
    VecCross(zaxis, xaxis, yaxis);
    VecNormalize(yaxis);

    result[0][0] = xaxis[0]; result[0][1] = xaxis[1]; result[0][2] = xaxis[2];
    result[1][0] = yaxis[0]; result[1][1] = yaxis[1]; result[1][2] = yaxis[2];
    result[2][0] = zaxis[0]; result[2][1] = zaxis[1]; result[2][2] = zaxis[2];

    result[0][3] = -VecDot(xaxis, eyePos);
    result[1][3] = -VecDot(yaxis, eyePos);
    result[2][3] = -VecDot(zaxis, eyePos);

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;
}

inline void LookAtRH(const float eyePos[3], const float lookAt[3], const float up[3], float result[4][4])
{
    float zaxis[3];
    float yaxis[3];
    float xaxis[3];

    VecSub(eyePos, lookAt, zaxis);

    VecNormalize(zaxis);
    VecCopy(up, yaxis);
    VecNormalize(yaxis);
    VecCross(yaxis, zaxis, xaxis);
    VecNormalize(xaxis);
    VecCross(zaxis, xaxis, yaxis);
    VecNormalize(yaxis);

    result[0][0] = xaxis[0]; result[0][1] = xaxis[1]; result[0][2] = xaxis[2];
    result[1][0] = yaxis[0]; result[1][1] = yaxis[1]; result[1][2] = yaxis[2];
    result[2][0] = zaxis[0]; result[2][1] = zaxis[1]; result[2][2] = zaxis[2];

    result[0][3] = -VecDot(xaxis, eyePos);
    result[1][3] = -VecDot(yaxis, eyePos);
    result[2][3] = -VecDot(zaxis, eyePos);

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;
}

inline void Perspective(float fov_y, float aspect, float zNear, float zFar, float result[4][4])
{
    PerspectiveRH(fov_y, aspect, zNear, zFar, result);
}

inline void PerspectiveLH(float fov_y, float aspect, float zNear, float zFar, float result[4][4])
{
    AASSERT(!FloatEqual(aspect, 0.0f));

    if (zNear > zFar)
        Swap(zNear, zFar);

    if (IsOpenGLGraph())
    {
        // Right Hand
        float n_f = 1.0f / (zFar - zNear);
        float yScale = Cot(DEG2RAD(fov_y) * 0.5f);
        float xScale = yScale / aspect;

        memset(result, 0, MATRIX44_SIZE);
        result[0][0] = xScale;
        result[1][1] = yScale;
        result[2][2] = (zFar + zNear) * n_f;
        result[3][2] = 1.0f;
        result[2][3] = -2.0f * zFar * zNear * n_f;
    }
    else
    {
        // Left Hand
        float f_n = 1.0f / (zFar - zNear);
        float yScale = Cot(DEG2RAD(fov_y) * 0.5f);
        float xScale = yScale / aspect;

        memset(result, 0, MATRIX44_SIZE);
        result[0][0] = xScale;
        result[1][1] = yScale;
        result[2][2] = zFar * f_n;
        result[3][2] = 1.0f;
        result[2][3] = -zNear * zFar * f_n;
    }
}

inline void PerspectiveRH(float fov_y, float aspect, float zNear, float zFar, float result[4][4])
{
    AASSERT(!FloatEqual(aspect, 0.0f));

    if (zNear > zFar)
        Swap(zNear, zFar);

    if (IsOpenGLGraph())
    {
        // Right Hand
        float n_f = 1.0f / (zNear - zFar);
        float yScale = Cot(DEG2RAD(fov_y) * 0.5f);
        float xScale = yScale / aspect;

        memset(result, 0, MATRIX44_SIZE);
        result[0][0] = xScale;
        result[1][1] = yScale;
        result[2][2] = (zFar + zNear) * n_f;
        result[3][2] = -1.0f;
        result[2][3] = 2.0f * zFar * zNear * n_f;
    }
    else
    {
        // Left Hand
        float f_n = 1.0f / (zNear - zFar);
        float yScale = Cot(DEG2RAD(fov_y) * 0.5f);
        float xScale = yScale / aspect;

        memset(result, 0, MATRIX44_SIZE);
        result[0][0] = xScale;
        result[1][1] = yScale;
        result[2][2] = zFar * f_n;
        result[3][2] = -1.0f;
        result[2][3] = zNear * zFar * f_n;
    }
}

inline void OrthoProject(float width, float height, float zNear, float zFar, float result[4][4])
{
    OrthoProjectRH(width, height, zNear, zFar, result);
}

inline void OrthoProjectLH(float width, float height, float zNear, float zFar, float result[4][4])
{
    AASSERT(!FloatEqual(zNear, zFar));
    if (zNear > zFar)
        Swap(zNear, zFar);

    if (IsOpenGLGraph())
    {
        AASSERT(0);

        float n_f = 1.0f / (zNear - zFar);

        memset(result, 0, MATRIX44_SIZE);
        result[0][0] = 2.0f / width;
        result[1][1] = 2.0f / height;
        result[2][2] = -2.0f * n_f;
        result[2][3] = zFar + zNear * n_f;
        result[3][3] = 1.0f;
    }
    else
    {
        float f_n = 1.0f / (zFar - zNear);

        memset(result, 0, MATRIX44_SIZE);
        result[0][0] = 2.0f / width;
        result[1][1] = 2.0f / height;
        result[2][2] = f_n;
        result[2][3] = -zNear * f_n;
        result[3][3] = 1.0f;
    }
}

inline void OrthoProjectRH(float width, float height, float zNear, float zFar, float result[4][4])
{
    AASSERT(!FloatEqual(zNear, zFar));
    if (zNear > zFar)
        Swap(zNear, zFar);

    if (IsOpenGLGraph())
    {
        float f_n = 1.0f / (zFar - zNear);

        memset(result, 0, MATRIX44_SIZE);
        result[0][0] = 2.0f / width;
        result[1][1] = 2.0f / height;
        result[2][2] = -2.0f * f_n;
        result[2][3] = -(zFar + zNear) * f_n;
        result[3][3] = 1.0f;
    }
    else
    {
        float n_f = 1.0f / (zNear - zFar);

        memset(result, 0, MATRIX44_SIZE);
        result[0][0] = 2.0f / width;
        result[1][1] = 2.0f / height;
        result[2][2] = n_f;
        result[2][3] = zNear * n_f;
        result[3][3] = 1.0f;
    }
}

inline float InterpolationLinear(float v1, float v2, float factor)
{
    return (1.0f - factor) * v1 + factor * v2;
}

inline void InterpolationLinear(float v1[3], float v2[3], float factor, float dst[3])
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
inline float InterpolationHermite(float v1, float v2, float c1, float c2, float factor)
{
    float factorPow2 = factor * factor;

    float factor1 = factorPow2 * (2.0f * factor - 3.0f) + 1.0f; // 2*t^3 - 3*t^2 + 1.0f
    float factor2 = factorPow2 * (factor - 2.0f) + factor;      // t^3 - 2*t^2 + t
    float factor3 = factorPow2 * (factor - 1.0f);               // t^3 - t^2
    float factor4 = factorPow2 * (3.0f - 2.0f * factor);        // -2*t^3 + 3*t^2

    // p(t) = (2*t^3 - 3*t^2 + 1)*p0 + (t^3 - 2*t^2 + t)*m0 + (-2*t^3 + 3*t^2)*p1 + (t^3 - t^2)*m1
    return factor1 * v1 + factor2 * c1 + factor3 * c2 + factor4 * v2;
}


inline void InterpolationHermite(float v1[3], float v2[3], float c1[3], float c2[3], float factor, float dst[3])
{
    float factorPow2 = factor * factor;

    float factor1 = factorPow2 * (2.0f * factor - 3.0f) + 1.0f;
    float factor2 = factorPow2 * (factor - 2.0f) + factor;
    float factor3 = factorPow2 * (factor - 1.0f);
    float factor4 = factorPow2 * (3.0f - 2.0f * factor);

    float factors[4];
    factors[0] = factor1; factors[1] = factor2; factors[2] = factor3;  factors[3] = factor4;

    float component0[4],component1[4],component2[4];
    component0[0] = v1[0]; component0[1] = c1[0]; component0[2] = c2[0]; component0[3] = v2[0];
    component1[0] = v1[1]; component1[1] = c1[1]; component1[2] = c2[1]; component1[3] = v2[1];
    component2[0] = v1[2]; component2[1] = c1[2]; component2[2] = c2[2]; component2[3] = v2[2];

    dst[0] = Vec4Dot(factors, component0);
    dst[1] = Vec4Dot(factors, component1);
    dst[2] = Vec4Dot(factors, component2);
}

/*
     c1-->控制点

 v1         v2

        c2-->控制点
----------------------
v1起始点 v2结束点
*/
inline float InterpolationBezier(float v1, float v2, float c1, float c2, float factor)
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

inline void InterpolationBezier(float v1[3], float v2[3], float c1[3], float c2[3], float factor, float dst[3])
{
    float factorPow2 = factor * factor;
    float inverseFactor = 1 - factor;
    float inverseFactorPow2 = inverseFactor * inverseFactor;
    float factor1 = inverseFactorPow2 * inverseFactor;
    float factor2 = 3.0f * factor * inverseFactorPow2;
    float factor3 = 3.0f * factorPow2 * inverseFactor;
    float factor4 = factorPow2 * factor;

    float factors[4];
    factors[0] = factor1; factors[1] = factor2; factors[2] = factor3;  factors[3] = factor4;

    float component0[4],component1[4],component2[4];
    component0[0] = v1[0]; component0[1] = c1[0]; component0[2] = c2[0]; component0[3] = v2[0];
    component1[0] = v1[1]; component1[1] = c1[1]; component1[2] = c2[1]; component1[3] = v2[1];
    component2[0] = v1[2]; component2[1] = c1[2]; component2[2] = c2[2]; component2[3] = v2[2];

    dst[0] = Vec4Dot(factors, component0);
    dst[1] = Vec4Dot(factors, component1);
    dst[2] = Vec4Dot(factors, component2);
}

// normal struct define
typedef struct Vec3_t
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
    Vec3_t():x(0.0f),y(0.0f),z(0.0f) { }
    Vec3_t(float x, float y, float z):x(x),y(y),z(z) { }
    Vec3_t(const float v[3]):x(v[0]),y(v[1]),z(v[2]) { }
    void Set(const float p[3]) { x = p[0], y = p[1], z = p[2]; }
    void Set(float x, float y, float z) { m[0] = x; m[1] = y; m[2] = z; }


    inline Vec3_t& Normalize()
    {
        VecNormalize(m);
        return *this;
    }

    inline float Dot(const Vec3_t& v) const
    {
        return VecDot(m, v.m);
    }

    inline Vec3_t Cross(const Vec3_t& v) const
    {
        Vec3_t temp;
        VecCross(m, v.m, temp.m);
        return temp;
    }

    inline Vec3_t& Add(const Vec3_t& v)
    {
        VecAdd(m, v.m, m);
        return *this;
    }

    inline Vec3_t Add(const Vec3_t& v) const
    {
        Vec3_t temp;
        VecAdd(m, v.m, temp.m);
        return temp;
    }

    inline Vec3_t& Sub(const Vec3_t& v)
    {
        VecSub(m, v.m, m);
        return *this;
    }

    inline Vec3_t Sub(const Vec3_t& v) const
    {
        Vec3_t temp;
        VecSub(m, v.m, temp.m);
        return temp;
    }

    inline Vec3_t& Scale(float scale)
    {
        VecScale(m, scale, m);
        return *this;
    }

    inline Vec3_t Scale(float scale) const
    {
        Vec3_t temp;
        VecScale(m, scale, temp.m);
        return temp;
    }

    inline float Length()
    {
        return VecLength(m);
    }

    Vec3_t& operator =(const Vec3_t& v)
    {
        memcpy(m, v.m, VEC3_SIZE);
        return *this;
    }

    Vec3_t operator *(float s) const
    {
        Vec3_t t;
        VecScale(this->m, s, t.m);
        return t;
    }

}Vec3;


inline bool operator == (const Vec3& v1, const Vec3& v2)
{
    return FloatEqualArray(v1.m, v2.m, 3);
}

inline bool operator != (const Vec3& v1, const Vec3& v2)
{
    return !FloatEqualArray(v1.m, v2.m, 3);
}

extern const Vec3 Vec3One;
extern const Vec3 Vec3Zero;
extern const Vec3 Vec3Left;
extern const Vec3 Vec3Right;
extern const Vec3 Vec3Up;
extern const Vec3 Vec3Down;
extern const Vec3 Vec3Forward;
extern const Vec3 Vec3Back;

typedef struct Vec4_t
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
    Vec4_t():x(0.0f),y(0.0f),z(0.0f),w(0.0f) { }
    Vec4_t(float x, float y, float z, float w):x(x),y(y),z(z),w(w) { }
    Vec4_t(const float v[4]):x(v[0]),y(v[1]),z(v[2]),w(v[3]) { }
    Vec4_t(const Vec3& v, float w):x(v.x),y(v.y),z(v.z),w(w) { }
    void Set(const float p[4]) { x = p[0], y = p[1], z = p[2], w = p[3]; }
    void Set(float x, float y, float z, float w) { m[0] = x; m[1] = y; m[2] = z, m[3] = w; }

    Vec4_t& operator =(const Vec4_t& v)
    {
        memcpy(m, v.m, VEC4_SIZE);
        return *this;
    }

    Vec4_t operator *(float s) const
    {
        Vec4_t t;
        t.x = x * s; t.y = y * s; t.z = z * s; t.w = w * s;
        return t;
    }

}Vec4;

inline bool operator == (const Vec4& v1, const Vec4& v2)
{
    return FloatEqualArray(v1.m, v2.m, 4);
}

inline bool operator != (const Vec4& v1, const Vec4& v2)
{
    return !FloatEqualArray(v1.m, v2.m, 4);
}

extern const Vec4 Vec4_One;
extern const Vec4 Vec4_Zero;

typedef struct Quat_t
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
    Quat_t():x(0.0f),y(0.0f),z(0.0f),w(0.0f) { }
    Quat_t(float x, float y, float z, float w):x(x),y(y),z(z),w(w) { }
    Quat_t(const float q[4]):x(q[0]),y(q[1]),z(q[2]),w(q[3]) { }
    void Set(const float q[4]) { x = q[0], y = q[1], z = q[2], w = q[3]; }
    void Set(float x, float y, float z, float w) { m[0] = x, m[1] = y, m[2] = z, m[3] = w; }

    Vec3 GetColumn0() { return Vec3(1.0f - 2.0f*(y*y + z*z), 2.0f*(x*y + w*z), 2.0f*(x*z - w*y)); }
    Vec3 GetColumn1() { return Vec3(2.0f*(x*y - w*z), 1.0f - 2.0f*(x*x + z*z), 2.0f*(y*z + w*x)); }
    Vec3 GetColumn2() { return Vec3(2.0f*(x*z + w*y), 2.0f*(y*z - w*x), 1.0f - 2.0f*(x*x + y*y)); }

    Vec3 GetRow0() { return Vec3(1.0f - 2.0f*(y*y + z*z), 2.0f*(x*y - w*z), 2.0f*(w*y + x*z)); }
    Vec3 GetRow1() { return Vec3(2.0f*(x*y + w*z), 1.0f - 2.0f*(x*x + z*z), 2.0f*(y*z - w*x)); }
    Vec3 GetRow2() { return Vec3(2.0f*(x*z - w*y), 2.0f*(y*z + w*x), 1.0f - 2.0f*(x*x + y*y)); }

    Quat_t& operator =(const Quat_t& q)
    {
        memcpy(m, q.m, QUAT_SIZE);
        return *this;
    }

}Quat;

extern const Quat QuatOne;
extern const Quat QuatZero;
extern const Quat QuatIdentity;

typedef struct Ray_t
{
    Vec3 origin;
    Vec3 dirction;


    Ray_t(const Vec3& o, const Vec3& dir):origin(o),dirction(dir)
    {

    }

    Ray_t(const float o[3], const float dir[3]):origin(o[0], o[1], o[2]),dirction(dir[0], dir[1], dir[2])
    {

    }

    Ray_t& operator =(const Ray_t& r)
    {
        origin = r.origin;
        dirction = r.dirction;
        return *this;
    }

}Ray;


typedef struct Plane_t
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
            float n[3]; // normal
            float d;    // D = -n * pointOnPlane
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
    Plane_t(void):A(0),B(0),C(0),D(0){}

    Plane_t(Vec3_t& normal, Vec3_t& pointOnPlane)
    {
        normal.Normalize();
        n[0] = normal.x;
        n[1] = normal.y;
        n[2] = normal.z;
        D = normal.Scale(-1.0f).Dot(pointOnPlane);
    }

    Plane_t(const float p[4]):A(p[0]),B(p[1]),C(p[2]),D(p[3])
    {

    }

    Plane_t& operator =(const Plane_t& p)
    {
        memcpy(m, p.m, VEC4_SIZE);
        return *this;
    }

    //>到原点的距离
    float GetOriginDistance() const
    {
        return D;
    }

    //>点到平面的距离
    float Resolve(const Vec3_t& v) const
    {
        return VecDot(v.m, n) + D;
    }

    //>点是否在平面的正面
    bool IsFront(const Vec3_t& v) const
    {
        return Resolve(v) > (EPSILON);
    }

    //>点是否在平面的背面
    bool IsBack(const Vec3_t& v) const
    {
        return Resolve(v) < (-EPSILON);
    }

    //>点是否在平面上
    bool OnPlane(const Vec3_t& v) const
    {
        return IsFloatZero(Resolve(v));
    }

    //>归一化参数平面
    bool NormalizePlane()
    {
        float magnitude = VecLength(n);
        if (!IsFloatZero(magnitude - 1.0f))
        {
            A /= magnitude;
            B /= magnitude;
            C /= magnitude;
            D /= magnitude;
        }
    }

    Plane_t& FromPoints(const Vec3_t& p1, const Vec3_t& p2, const Vec3_t& p3) //>p1,p2,p3为顺时针围绕的三个点
    {
        Vec3_t normal = p1.Sub(p2).Cross(p3.Sub(p2));
        VecCopy(normal.Normalize().m, n);
        D = -VecDot(n, p2.m);
        return *this;
    }

    Plane_t& FromVectors(const Vec3_t& v1, const Vec3_t& v2, const Vec3_t& p) //>向量cross容易顺序出错. 建议不要用此函数构造平面
    {
        Vec3_t normal = v1.Cross(v2);
        VecCopy(normal.Normalize().m, n);
        D = -VecDot(n, p.m);
        return *this;
    }

    static bool Intersection(const Plane_t& p1, const Plane_t& p2, const Plane_t& p3, Vec4& result)
    {
        float v[3];
        VecCross(p2.n, p3.n, v);
        if (IsFloatZero(VecDot(p1.n, v)))
            return false;

        float mat[4][4];
        MatIdentity(mat);
        mat[0][0] = p1.n[0]; mat[0][1] = p1.n[1]; mat[0][2] = p1.n[2];
        mat[1][0] = p2.n[0]; mat[1][1] = p2.n[1]; mat[1][2] = p2.n[2];
        mat[2][0] = p3.n[0]; mat[2][1] = p3.n[1]; mat[2][2] = p3.n[2];

        if(MatInverse(mat, mat))
        {
            float v[4] = { -p1.D, -p2.D, -p3.D, 1.0f };
            Vec4Transform(v, mat, result.m);
            return true;
        }

        return false;
    }

    static bool Intersection(const float p1[4] , const float p2[4], const float p3[4], float result[3])
    {
        float v[3];
        VecCross(p2, p3, v);
        if (IsFloatZero(VecDot(p1, v)))
            return false;

        float mat[4][4];
        MatIdentity(mat);
        mat[0][0] = p1[0]; mat[0][1] = p1[1]; mat[0][2] = p1[2];
        mat[1][0] = p2[0]; mat[1][1] = p2[1]; mat[1][2] = p2[2];
        mat[2][0] = p3[0]; mat[2][1] = p3[1]; mat[2][2] = p3[2];

        if(MatInverse(mat, mat))
        {
            float v[4] = { -p1[3], -p2[3], -p3[3], 1.0f };
            float r[4];
            Vec4Transform(v, mat, r);
            VecCopy(r, result);
            return true;
        }

        return false;
    }

}Plane;

// major-row memory. but vector is major-column,
// so matrix is major-column.
//>游戏引擎使用了行为主的和c语言的数组相同的方式,
//>而opengl的矩阵式以列为主的方式.所以传递给opengl
//>使用时需要转置.
//>| m11 m21 m31 m41 |
//>| m12 m22 m32 m41 | ===> opengl存储模式
//>| m13 m23 m33 m41 |
//>| m14 m24 m34 m41 |

typedef struct Matrix_t
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
    Matrix_t()
    {
        m11 = m12 = m13 = m14 = 0.0f;
        m21 = m22 = m23 = m24 = 0.0f;
        m31 = m32 = m33 = m34 = 0.0f;
        m41 = m42 = m43 = m44 = 0.0f;
    }

    Matrix_t(float m1, float m2, float m3, float m4,
             float m5, float m6, float m7, float m8,
             float m9, float m10, float m11, float m12,
             float m13, float m24, float m15, float m16):
            m11(m1),m12(m2),m13(m3),m14(m4),
            m21(m5),m22(m6),m23(m7),m24(m8),
            m31(m9),m32(m10),m33(m11),m34(m12),
            m41(m13),m42(m14),m43(m15),m44(m16)
    {
    }

    Matrix_t(const Matrix_t& mat)
    {
        *this = mat;
    }

    void SetRow3(unsigned int index, const Vec3& v)
    {
        AASSERT(index < 4);
        m[index][0] = v.x;
        m[index][1] = v.y;
        m[index][2] = v.z;
    }

    void SetColumn3(unsigned int index,const Vec3& v)
    {
        AASSERT(index < 4);
        m[0][index] = v.x;
        m[1][index] = v.y;
        m[2][index] = v.z;
    }

    void GetColumn3(unsigned int index, Vec3& v)
    {
        AASSERT(index < 4);
        v.x = m[0][index];
        v.y = m[1][index];
        v.z = m[2][index];
    }

    inline Matrix_t& Identity()
    {
        MatIdentity(m);
        return *this;
    }

    inline Matrix_t& Concatenate(const Matrix_t& mat)
    {
        ConcatTransforms(m, mat.m, m);
        return *this;
    }

    inline Vec3 Transfrom(const Vec3& V)
    {
        Vec3 t;
        VecTransform(V.m, this->m, t.m);
        return t;
        //Vec4 t(V.x, V.y, V.z, 1.0f);
        //Vec4Transform(t.m, this->m, t.m);
        //return Vec3(t.x, t.y, t.z);
    }

    inline Vec4 Transfrom(const Vec4& V)
    {
        Vec4 t;
        Vec4Transform(V.m, this->m, t.m);
        return t;
    }

    inline Matrix_t& Transpose()
    {
        MatTranspose(m, m);
        return *this;
    }

    inline bool CanInverse()
    {
        Matrix_t t;
        return MatInverse(m, t.m);
    }

    inline Matrix_t& AffineInverse()
    {
        MatAffineInverse(m, m);
        return *this;
    }

    inline Matrix_t& Inverse()
    {
        MatInverse(m, m);
        return *this;
    }

    inline Matrix_t& Translation(const Vec3& t)
    {
        MatTranslation(t.x, t.y, t.z, m);
        return *this;
    }

    inline Matrix_t& Scaling(float s)
    {
        MatScaling(s, s, s, m);
        return *this;
    }

    inline Matrix_t& Scaling(const Vec3& s)
    {
        MatScaling(s.x, s.y, s.z, m);
        return *this;
    }

    inline Matrix_t& RotationX(float angle)
    {
        MatRotationX(angle, m);
        return *this;
    }

    inline Matrix_t& RotationY(float angle)
    {
        MatRotationY(angle, m);
        return *this;
    }

    inline Matrix_t& RotationZ(float angle)
    {
        MatRotationZ(angle, m);
        return *this;
    }

    inline Matrix_t& RotationZXY(float angleRoll, float anglePitch, float angleYaw)
    {
        Matrix_t temp;
        MatRotationZ(angleRoll, temp.m);
        MatRotationX(anglePitch, m);
        ConcatTransforms(m, temp.m, m);
        MatRotationY(angleYaw, temp.m);
        ConcatTransforms(temp.m, m, m);
        return *this;
    }

    Matrix_t& operator =(const Matrix_t& mat)
    {
        memcpy(m, mat.m, MATRIX44_SIZE);
        return *this;
    }

    float operator [](uint8 index) const
    {
        return m[index/4][index%4];
    }

}Matrix;


extern const Matrix MatrixIdentity;

#endif // _ATG_MATH_H_
