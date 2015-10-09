#pragma once

#include "atgMath.h"

struct AABBox
{
    AABBox(float minx, float miny, float minz, float maxx, float maxy, float maxz):vMin(minx,miny,minz),vMax(maxx,maxy,maxz) {}
    AABBox(const Vec3& minPoint, const Vec3& maxPoint):vMin(minPoint),vMax(minPoint) {}

    AABBox& Merge(const AABBox& other)
    {
        vMin.x = Min(vMin.x, other.vMin.x);
        vMin.y = Min(vMin.y, other.vMin.y);
        vMin.z = Min(vMin.z, other.vMin.z);
        
        vMax.x = Max(vMax.x, other.vMax.x);
        vMax.y = Max(vMax.y, other.vMax.y);
        vMax.z = Max(vMax.z, other.vMax.z);

        return *this;
    }

    AABBox& Merge(const Vec3& point)
    {
        vMin.x = Min(vMin.x, point.x);
        vMin.y = Min(vMin.y, point.y);
        vMin.z = Min(vMin.z, point.z);

        vMax.x = Max(vMax.x, point.x);
        vMax.y = Max(vMax.y, point.y);
        vMax.z = Max(vMax.z, point.z);

        return *this;
    }

    Vec3 GetCenter()
    {
        Vec3 center = vMin.Add(vMax);
        center.Scale(0.5f);
        return center;
    }

    float GetSize()
    {
        vMax.Sub(vMin);
    }

    bool IsInner(const AABBox& other)
    {
        if(IsInner(other.vMin) && IsInner(other.vMax))
        {
            return true;
        }

        return false;
    }

    bool IsInner(const Vec3& point)
    {
        if (vMin.x > point.x && vMin.y > point.y && vMin.z > point.z && 
            point.x < vMax.x && point.y < vMax.y && point.z < vMax.z)
        {
            return true;
        }
        
        return false;
    }

    Vec3 vMin;
    Vec3 vMax;
};

// We create an enum of the sides so we don't have to call each side 0 or 1.   
// This way it makes it more understandable and readable when dealing with frustum sides.   
enum FrustumSide   
{   
    RIGHT   = 0,        // The RIGHT side of the frustum   
    LEFT    = 1,        // The LEFT  side of the frustum   
    BOTTOM  = 2,        // The BOTTOM side of the frustum   
    TOP     = 3,        // The TOP side of the frustum   
    BACK    = 4,        // The BACK side of the frustum   
    FRONT   = 5,         // The FRONT side of the frustum
    SIDE_NUMBER,
}; 

class atgFrustum
{
public:
    atgFrustum();
    ~atgFrustum();

    void BuildFrustumPlanes(const float modelview[4][4], const float projection[4][4]);

    bool IsPointInFrustum(const float v[3]);
    
    bool IsSphereInFrustum(const float center[3], float radius);

    bool IsCubeInFrustum(const float center[3], float size);

    void DebugRender();
protected:

    Plane _frustum[SIDE_NUMBER];
};

