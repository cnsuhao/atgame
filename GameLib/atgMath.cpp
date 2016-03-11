#include "atgMath.h"

const float atgMath::PI             = 3.1415926f;
const float atgMath::PI_DIV_2       = 1.5707963f;
const float atgMath::PI_DIV_4       = 0.7853981f;
const float atgMath::INV_PI         = 0.3183098f;
const float atgMath::PI_DIV_180     = (atgMath::PI / 180.f);
const float atgMath::INV_PI_MUL_180 = (180.f / atgMath::PI);

//> Îó²î³£Á¿
const float atgMath::EPSILON        = 1e-5f;

const atgVec3 Vec3One(1.0f,  1.0f,  1.0f);
const atgVec3 Vec3Zero(0.0f,  0.0f,  0.0f);
const atgVec3 Vec3Left(-1.0f,  0.0f,  0.0f);
const atgVec3 Vec3Right(1.0f,  0.0f,  0.0f);
const atgVec3 Vec3Up(0.0f,  1.0f,  0.0f);
const atgVec3 Vec3Down(0.0f, -1.0f,  0.0f);
const atgVec3 Vec3Forward(0.0f,  0.0f,  1.0f);
const atgVec3 Vec3Back(0.0f,  0.0f, -1.0f);

const atgVec4 Vec4One(1.0f, 1.0f, 1.0f, 1.0f);
const atgVec4 Vec4Zero(0.0f, 0.0f, 0.0f, 0.0f);

const atgQuaternion QuatOne(1.0f, 1.0f, 1.0f, 1.0f);
const atgQuaternion QuatZero(0.0f, 0.0f, 0.0f, 0.0f);
const atgQuaternion QuatIdentity(0.0f, 0.0f, 0.0f, 1.0f);

const atgMatrix MatrixIdentity(1.0f, 0.0f, 0.0f, 0.0f, 
                               0.0f, 1.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);


atgVec4 atgMatrix::Transfrom(const atgVec4& v) const
{
    atgVec4 temp;

    temp.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    temp.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    temp.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    temp.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

    return temp;
}

bool atgMatrix::CanInverse() const
{
    float a0 = m[0][0] * m[1][1] - m[1][0] * m[0][1];
    float a1 = m[0][0] * m[2][1] - m[2][0] * m[0][1];
    float a2 = m[0][0] * m[3][1] - m[3][0] * m[0][1];
    float a3 = m[1][0] * m[2][1] - m[2][0] * m[1][1];
    float a4 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    float a5 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    float b0 = m[0][2] * m[1][3] - m[1][2] * m[0][3];
    float b1 = m[0][2] * m[2][3] - m[2][2] * m[0][3];
    float b2 = m[0][2] * m[3][3] - m[3][2] * m[0][3];
    float b3 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
    float b4 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    float b5 = m[2][2] * m[3][3] - m[3][2] * m[2][3];

    // Calculate the determinant.
    float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

    // Close to zero, can't invert.
    if (fabs(det) <= atgMath::EPSILON)
        return false;

    return true;
}

atgMatrix::atgMatrix(const atgQuaternion& q)
{
    atgVec3 column0 = q.GetColumn0();
    atgVec3 column1 = q.GetColumn1();
    atgVec3 column2 = q.GetColumn2();

    m[0][0] = column0.x;
    m[1][0] = column0.y;
    m[2][0] = column0.z;

    m[0][1] = column1.x;
    m[1][1] = column1.y; 
    m[2][1] = column1.z;

    m[0][2] = column2.x;
    m[1][2] = column2.y;
    m[2][2] = column2.z;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;

    m[3][3] = 1.0f;
}

atgMatrix& atgMatrix::Inverse()
{
    float temp[4][4];

    float a0 = m[0][0] * m[1][1] - m[1][0] * m[0][1];
    float a1 = m[0][0] * m[2][1] - m[2][0] * m[0][1];
    float a2 = m[0][0] * m[3][1] - m[3][0] * m[0][1];
    float a3 = m[1][0] * m[2][1] - m[2][0] * m[1][1];
    float a4 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    float a5 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    float b0 = m[0][2] * m[1][3] - m[1][2] * m[0][3];
    float b1 = m[0][2] * m[2][3] - m[2][2] * m[0][3];
    float b2 = m[0][2] * m[3][3] - m[3][2] * m[0][3];
    float b3 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
    float b4 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    float b5 = m[2][2] * m[3][3] - m[3][2] * m[2][3];

    // Calculate the determinant.
    float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

    // Close to zero, can't invert.
    if (fabs(det) <= atgMath::EPSILON)
        return *this;

    // Support the case where m == dst.
    float inv_det = 1.0f / det;
    temp[0][0] = ( m[1][1] * b5 - m[2][1] * b4 + m[3][1] * b3) * inv_det;
    temp[1][0] = (-m[1][0] * b5 + m[2][0] * b4 - m[3][0] * b3) * inv_det;
    temp[2][0] = ( m[1][3] * a5 - m[2][3] * a4 + m[3][3] * a3) * inv_det;
    temp[3][0] = (-m[1][2] * a5 + m[2][2] * a4 - m[3][2] * a3) * inv_det;

    temp[0][1] = (-m[0][1] * b5 + m[2][1] * b2 - m[3][1] * b1) * inv_det;
    temp[1][1] = ( m[0][0] * b5 - m[2][0] * b2 + m[3][0] * b1) * inv_det;
    temp[2][1] = (-m[0][3] * a5 + m[2][3] * a2 - m[3][3] * a1) * inv_det;
    temp[3][1] = ( m[0][2] * a5 - m[2][2] * a2 + m[3][2] * a1) * inv_det;

    temp[0][2] = ( m[0][1] * b4 - m[1][1] * b2 + m[3][1] * b0) * inv_det;
    temp[1][2] = (-m[0][0] * b4 + m[1][0] * b2 - m[3][0] * b0) * inv_det;
    temp[2][2] = ( m[0][3] * a4 - m[1][3] * a2 + m[3][3] * a0) * inv_det;
    temp[3][2] = (-m[0][2] * a4 + m[1][2] * a2 - m[3][2] * a0) * inv_det;

    temp[0][3] = (-m[0][1] * b3 + m[1][1] * b1 - m[2][1] * b0) * inv_det;
    temp[1][3] = ( m[0][0] * b3 - m[1][0] * b1 + m[2][0] * b0) * inv_det;
    temp[2][3] = (-m[0][3] * a3 + m[1][3] * a1 - m[2][3] * a0) * inv_det;
    temp[3][3] = ( m[0][2] * a3 - m[1][2] * a1 + m[2][2] * a0) * inv_det;

    memcpy(m, temp, sizeof(atgMatrix));

    return *this;
}

atgMatrix& atgMatrix::AffineInverse()
{
    float temp[4][4];
    if (atgMath::IsFloatZero(m[3][0]) && atgMath::IsFloatZero(m[3][1]) && atgMath::IsFloatZero(m[3][2]) && atgMath::IsFloatZero(m[3][3] - 1.0f))
    {
        temp[0][0] = m[0][0]; temp[0][1] = m[1][0]; temp[0][2] = m[2][0];
        temp[1][0] = m[0][1]; temp[1][1] = m[1][1]; temp[1][2] = m[2][1];
        temp[2][0] = m[0][2]; temp[2][1] = m[1][2]; temp[2][2] = m[2][2];
        temp[3][0] = 0.0f;    temp[3][1] = 0.0f;    temp[3][2] = 0.0f;   temp[3][3] = 1.0f;

        float t[3]; t[0] = m[0][3]; t[1] = m[1][3]; t[2] = m[2][3];
        temp[0][3] = -(temp[0][0] * t[0] + temp[0][1] * t[1], temp[0][2] * t[2]);
        temp[1][3] = -(temp[1][0] * t[0] + temp[1][1] * t[1], temp[1][2] * t[2]);
        temp[2][3] = -(temp[2][0] * t[0] + temp[2][1] * t[1], temp[2][2] * t[2]);

        memcpy(m, temp, sizeof(atgMatrix));

    }
    return *this;
}


atgMatrix& atgMatrix::RotationZXY(float angleRoll, float anglePitch, float angleYaw)
{
    atgMatrix rollMat;
    atgMatrix pitchMat;
    atgMatrix yawMat;

    rollMat.RotationZ(angleRoll);
    pitchMat.RotationX(anglePitch);
    yawMat.RotationY(angleYaw);

    *this = yawMat * pitchMat * rollMat;
    return *this;
}

bool atgMatrix::Get(atgQuaternion& q)
{
    float temp[4];
    const float diag = m[0][0] + m[1][1] + m[2][2] + 1;

    if( diag > 0.0f )
    {
        const float scale = sqrtf(diag) * 2.0f; // get scale from diagonal

        // TODO: speed this up
        temp[0] = (m[2][1] - m[1][2]) / scale;
        temp[1] = (m[0][2] - m[2][0]) / scale;
        temp[2] = (m[1][0] - m[0][1]) / scale;
        temp[3] = 0.25f * scale;
    }
    else
    {
        if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
        {
            // 1st element of diag is greatest value
            // find scale according to 1st element, and double it
            const float scale = sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2.0f;

            // TODO: speed this up
            temp[0] = 0.25f * scale;
            temp[1] = (m[0][1] + m[1][0]) / scale;
            temp[2] = (m[2][0] + m[0][2]) / scale;
            temp[3] = (m[2][1] - m[1][2]) / scale;
        }
        else if (m[1][1] > m[2][2])
        {
            // 2nd element of diag is greatest value
            // find scale according to 2nd element, and double it
            const float scale = sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2.0f;

            // TODO: speed this up
            temp[0] = (m[0][1] + m[1][0]) / scale;
            temp[1] = 0.25f * scale;
            temp[2] = (m[1][2] + m[2][1]) / scale;
            temp[3] = (m[0][2] - m[2][0]) / scale;
        }
        else
        {
            // 3rd element of diag is greatest value
            // find scale according to 3rd element, and double it
            const float scale = sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]) * 2.0f;

            // TODO: speed this up
            temp[0] = (m[0][2] + m[2][0]) / scale;
            temp[1] = (m[1][2] + m[2][1]) / scale;
            temp[2] = 0.25f * scale;
            temp[3] = (m[1][0] - m[0][1]) / scale;
        }
    }

    q.Set(temp);
    return true;
}

float atgQuaternion::Length() const
{
    float lenght = m[0] * m[0] + m[1] * m[1] + m[2] * m[2] + m[3] * m[3];
    return sqrtf(lenght);
}

atgQuaternion& atgQuaternion::Normalize()
{
    float length = Length();
    if(atgMath::FloatEqual(length, 1.0f))
    {
        return *this;
    }

#ifdef _DEBUG
    assert(!atgMath::IsFloatZero(length));
#endif // _DEBUG

    float invLength = 1.0f / length;
    m[0] *= invLength;
    m[1] *= invLength;
    m[2] *= invLength;
    m[3] *= invLength;

    return *this;
}

atgQuaternion atgQuaternion::Normalize() const
{
    float length = Length();
    if(atgMath::FloatEqual(length, 1.0f))
    {
        return *this;
    }

#ifdef _DEBUG
    assert(!atgMath::IsFloatZero(length));
#endif // _DEBUG

    float invLength = 1.0f / length;
    
    return atgQuaternion( x * invLength, 
                          y * invLength,
                          z * invLength,
                          w * invLength);
}

atgQuaternion::atgQuaternion(float pitch, float yaw, float roll)
{
    //////////////////////////////////////////////////////////////////////////
    //>copy from lrrlitch.
    //////////////////////////////////////////////////////////////////////////

    float angle;

    angle = pitch * 0.5f;
    const float sx = sin(angle);
    const float cx = cos(angle);

    angle = yaw * 0.5f;
    const float sy = sin(angle);
    const float cy = cos(angle);

    angle = roll * 0.5f;
    const float sz = sin(angle);
    const float cz = cos(angle);

    const float cycz = cy * cz;
    const float sycz = sy * cz;
    const float cysz = cy * sz;
    const float sysz = sy * sz;

    m[0] = (sx * cycz - cx * sysz);
    m[1] = (cx * sycz + sx * cysz);
    m[2] = (cx * cysz - sx * sycz);

    m[3] = (cx * cycz + sx * sysz);
}

atgQuaternion::atgQuaternion(const atgVec3& axis, float angle)
{
    atgVec3 temp(axis);
    temp.Normalize();
    float halfAngle = atgMath::DegreesToRadians(angle) * 0.5f;
    float sinHalfAngle = sinf(halfAngle);
    float cosHalfAngle = cosf(halfAngle);
    m[0] = temp.x * sinHalfAngle;
    m[1] = temp.y * sinHalfAngle;
    m[2] = temp.z * sinHalfAngle;
    m[3] = cosHalfAngle;
}

atgQuaternion::atgQuaternion(const atgVec3& from, const atgVec3& to)
{
    if (atgMath::FloatEqualArray(from.m, to.m, 3))
    {
        x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
        return;
    }

    atgVec3 normStart(from);
    atgVec3 normEnd(to);
    normStart.Normalize();
    normEnd.Normalize();
    float d = normStart.Dot(normEnd);

    if (d > -1.0f + atgMath::EPSILON)
    {
        atgVec3 c = normStart.Cross(normEnd);
        float s = sqrtf((1.0f + d) * 2.0f);
        float invS = 1.0f / s;

        x = c.x * invS;
        y = c.y * invS;
        z = c.z * invS;
        w = 0.5f * s;
    }
    else
    {
        atgVec3 axis = Vec3Right.Cross(normStart);
        if (axis.Length() < atgMath::EPSILON)
            axis = Vec3Up.Cross(normStart);
        
        *this = atgQuaternion(axis, 180.0f);
    }
}

atgQuaternion& atgQuaternion::Slerp(const atgQuaternion& q1, const atgQuaternion& q2, float interpTime)
{
    interpTime = atgMath::Clamp(interpTime, 0.0f, 1.0f);

    if(atgMath::FloatEqual(interpTime, 0.0f))
    {
        x = q1.x; 
        y = q1.y;
        z = q1.z;
        w = q1.w;
        return *this;
    }
    else if(atgMath::FloatEqual(interpTime, 1.0f))
    {
        x = q2.x; 
        y = q2.y;
        z = q2.z;
        w = q2.w;
        return *this;
    }

    if (atgMath::FloatEqualArray(q1.m, q2.m, 4))
    {
        x = q1.x; 
        y = q1.y;
        z = q1.z;
        w = q1.w;
        return *this;
    }

    float cosOmega = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    float temp[4];
    // make sure we use the short rotation
    if(cosOmega < 0.0f)
    {
        temp[0] = -q2.x;
        temp[1] = -q2.y;
        temp[2] = -q2.z;
        temp[3] = -q2.w;
        cosOmega = -cosOmega;
    }else
    {
        temp[0] = q2.x;
        temp[1] = q2.y;
        temp[2] = q2.w;
        temp[3] = q2.w;
    }

    float k0, k1;
    if(cosOmega < 0.985f)
    {
        // spherical interpolation
        //const float theta = acosf(cosOmega);
        //const float invSinTheta = 1.0f /sinf(theta);
        //k0 = sinf(theta * (1.0f - interpTime)) * invSinTheta;
        //k1 = sinf(theta * interpTime) * invSinTheta;
        float sinOmega = sqrtf(1.0f - cosOmega * cosOmega);
        float omega = atan2f(sinOmega, cosOmega);
        float oneOverSinOmega = 1.0f / sinOmega;
        k0 = sinf((1.0f - interpTime) * omega) * oneOverSinOmega;
        k1 = sinf(interpTime * omega) * oneOverSinOmega;
    }
    else
    {
        // linear interploation
        k0 = 1.0f - interpTime;
        k1 = interpTime;
    }

    x = q1.x * k0 + temp[0] * k1;
    y = q1.y * k0 + temp[1] * k1;
    z = q1.z * k0 + temp[2] * k1;
    w = q1.w * k0 + temp[3] * k1;

    // half-life / Valve
    //int i;
    //float omega, cosom, sinom, sclp, sclq;

    //// decide if one of the quaternions is backwards
    //float a = 0;
    //float b = 0;
    //for (i = 0; i < 4; i++) {
    //    a += (quat1[i]-quat2[i])*(quat1[i]-quat2[i]);
    //    b += (quat1[i]+quat2[i])*(quat1[i]+quat2[i]);
    //}
    //float temp[4];
    //if (a > b) {
    //    for (i = 0; i < 4; i++) {
    //        temp[i] = -quat2[i];
    //    }
    //}else
    //{
    //    temp[0] = quat2[0];
    //    temp[1] = quat2[1];
    //    temp[2] = quat2[2];
    //    temp[3] = quat2[3];
    //}

    //cosom = quat1[0]*temp[0] + quat1[1]*temp[1] + quat1[2]*temp[2] + quat1[3]*temp[3];

    //if ((1.0 + cosom) > 0.00000001) {
    //    if ((1.0 - cosom) > 0.00000001) {
    //        omega = acos( cosom );
    //        sinom = sin( omega );
    //        sclp = sin( (1.0 - interpTime)*omega) / sinom;
    //        sclq = sin( interpTime*omega ) / sinom;
    //    }
    //    else {
    //        sclp = 1.0 - interpTime;
    //        sclq = interpTime;
    //    }
    //    for (i = 0; i < 4; i++) {
    //        result[i] = sclp * quat1[i] + sclq * temp[i];
    //    }
    //}
    //else {
    //    result[0] = -quat1[1];
    //    result[1] = quat1[0];
    //    result[2] = -quat1[3];
    //    result[3] = quat1[2];
    //    sclp = sin( (1.0 - interpTime) * 0.5 * MATH_PI);
    //    sclq = sin( interpTime * 0.5 * MATH_PI);
    //    for (i = 0; i < 3; i++) {
    //        result[i] = sclp * quat1[i] + sclq * result[i];
    //    }
    //}

    return *this;
}

bool atgQuaternion::GetEulerAngle(atgVec3& euler) const
{
    float X = x;
    float Y = y;
    float Z = z;
    float W = w;

    const double sqw = W*W;
    const double sqx = X*X;
    const double sqy = Y*Y;
    const double sqz = Z*Z;
    const double test = 2.0 * (Y*W - X*Z);

    if (atgMath::DoubleEqual(test, 1.0, 0.000001))
    {
        // heading = rotation about z-axis
        euler.z = (float) (-2.0*atan2(X, W));
        // bank = rotation about x-axis
        euler.x = 0;
        // attitude = rotation about y-axis
        euler.y = atgMath::PI_DIV_2;
    }
    else if (atgMath::DoubleEqual(test, -1.0, 0.000001))
    {
        // heading = rotation about z-axis
        euler.z = (float) (2.0*atan2(X, W));
        // bank = rotation about x-axis
        euler.x = 0;
        // attitude = rotation about y-axis
        euler.y = -atgMath::PI_DIV_2;
    }
    else
    {
        // heading = rotation about z-axis
        euler.z = (float) atan2(2.0 * (X*Y +Z*W),(sqx - sqy - sqz + sqw));
        // bank = rotation about x-axis
        euler.x = (float) atan2(2.0 * (Y*Z +X*W),(-sqx - sqy + sqz + sqw));
        // attitude = rotation about y-axis
        euler.y = (float) asin( atgMath::Clamp(test, -1.0, 1.0) );
    }

    return true;
}

atgVec3 operator* (const atgQuaternion& q, const atgVec3& v)
{
    if(0)
    {
        // normal method
        atgQuaternion q1(q);
        atgQuaternion q2(v.x, v.y, v.z, 0.0f);
        atgQuaternion q3(q.conjugate());

        atgQuaternion t = (q1 * q2) * q3;
        return atgVec3(t.x, t.y, t.z); 
    }

    // nVidia SDK implementation

    atgVec3 uv;
    atgVec3 uuv;
    atgVec3 qvec(q.x, q.y, q.z);

    uv = qvec.Cross(v);
    uuv = qvec.Cross(uv);

    float halfW = 2.0f * q.w;

    return atgVec3(v.x + uv.x * halfW + uuv.x * 2.0f,
                   v.y + uv.y * halfW + uuv.y * 2.0f,
                   v.z + uv.z * halfW + uuv.z * 2.0f);
}

#include "atgRenderer.h"

bool atgMath::IsBetween0And1ForClipZ()
{
    return !IsOpenGLGraph();
}


