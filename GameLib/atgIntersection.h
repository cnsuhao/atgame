#pragma once

#include "atgMath.h"


enum IntersectionType
{
    IT_Exclude, //>不相交
    IT_Include, //>不包含
    IT_Cross,   //>交叉
};

struct atgAABBox
{
    atgAABBox() { }
    atgAABBox(float minx, float miny, float minz, float maxx, float maxy, float maxz):vMin(minx,miny,minz),vMax(maxx,maxy,maxz) {}
    atgAABBox(const atgVec3& minPoint, const atgVec3& maxPoint):vMin(minPoint),vMax(minPoint) {}

    atgAABBox& Merge(const atgAABBox& other)
    {
        vMin.x = atgMath::Min(vMin.x, other.vMin.x);
        vMin.y = atgMath::Min(vMin.y, other.vMin.y);
        vMin.z = atgMath::Min(vMin.z, other.vMin.z);
        
        vMax.x = atgMath::Max(vMax.x, other.vMax.x);
        vMax.y = atgMath::Max(vMax.y, other.vMax.y);
        vMax.z = atgMath::Max(vMax.z, other.vMax.z);

        return *this;
    }

    atgAABBox& Merge(const atgVec3& point)
    {
        vMin.x = atgMath::Min(vMin.x, point.x);
        vMin.y = atgMath::Min(vMin.y, point.y);
        vMin.z = atgMath::Min(vMin.z, point.z);

        vMax.x = atgMath::Max(vMax.x, point.x);
        vMax.y = atgMath::Max(vMax.y, point.y);
        vMax.z = atgMath::Max(vMax.z, point.z);

        return *this;
    }

    atgAABBox& Merge(const float point[3])
    {
        vMin.x = atgMath::Min(vMin.x, point[0]);
        vMin.y = atgMath::Min(vMin.y, point[1]);
        vMin.z = atgMath::Min(vMin.z, point[2]);

        vMax.x = atgMath::Max(vMax.x, point[0]);
        vMax.y = atgMath::Max(vMax.y, point[1]);
        vMax.z = atgMath::Max(vMax.z, point[2]);

        return *this;
    }

    atgVec3 GetCenter()
    {
        atgVec3 center = vMin + vMax;
        return center * 0.5f;
    }

    atgVec3 GetSize()
    {
        atgVec3 center = vMax - vMin;
        return center * 0.5f;
    }

    bool IsInner(const atgAABBox& other) const
    {
        if(IsInner(other.vMin) && IsInner(other.vMax))
        {
            return true;
        }

        return false;
    }

    bool IsInner(const atgVec3& point) const
    {
        if (vMin.x < point.x && vMin.y < point.y && vMin.z < point.z && 
            point.x < vMax.x && point.y < vMax.y && point.z < vMax.z)
        {
            return true;
        }
        
        return false;
    }

    IntersectionType Intersect(const atgAABBox& other)
    {
        if (IsInner(other))
        {
            return IT_Include;
        }
        else if(IsInner(other.vMin) || IsInner(other.vMax))
        {
            return IT_Cross;
        }
        else
            return IT_Exclude;
    }

    atgVec3 vMin;
    atgVec3 vMax;
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

    void BuildFrustumPlanes(const atgMatrix& modelview, const atgMatrix projection);

    bool IsPointInFrustum(const float v[3]);
    
    bool IsSphereInFrustum(const float center[3], float radius);

    bool IsCubeInFrustum(const float center[3], float size);

    bool IsAABBoxInFurstum(const atgAABBox& bbox);

    void DebugRender();
protected:

    atgPlane    _frustum[SIDE_NUMBER];
};


class Octree
{
    struct Node 
    {

    };


};