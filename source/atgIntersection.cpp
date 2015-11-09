#include "atgBase.h"
#include "atgIntersection.h"
#include "atgRenderer.h"

atgFrustum::atgFrustum(void)
{
}


atgFrustum::~atgFrustum(void)
{
}

void atgFrustum::BuildFrustumPlanes( const float modl[4][4], const float proj[4][4] )
{
    float  clip[16];                               // This will hold the clipping planes

    // Now that we have our modelview and projection matrix, if we combine these 2 matrices,
    // it will give us our clipping planes.  To combine 2 matrices, we multiply them.

    ConcatTransforms(proj, modl, FPTR_TO_F4x4(clip));

    // Now we actually want to get the sides of the frustum.  To do this we take
    // the clipping planes we received above and extract the sides from them.

    // This will extract the RIGHT side of the frustum
    _frustum[RIGHT].A = clip[12] - clip[0];
    _frustum[RIGHT].B = clip[13] - clip[1];
    _frustum[RIGHT].C = clip[14] - clip[2];
    _frustum[RIGHT].D = clip[15] - clip[3];

    // Now that we have a normal (A,B,C) and a distance (D) to the plane,
    // we want to normalize that normal and distance.

    // Normalize the RIGHT side
    _frustum[RIGHT].NormalizePlane();

    // This will extract the LEFT side of the frustum
    _frustum[LEFT].A = clip[12] + clip[0];
    _frustum[LEFT].B = clip[13] + clip[1];
    _frustum[LEFT].C = clip[14] + clip[2];
    _frustum[LEFT].D = clip[15] + clip[3];

    // Normalize the LEFT side
    _frustum[LEFT].NormalizePlane();

    // This will extract the BOTTOM side of the frustum
    _frustum[BOTTOM].A = clip[12] + clip[4];
    _frustum[BOTTOM].B = clip[13] + clip[5];
    _frustum[BOTTOM].C = clip[14] + clip[6];
    _frustum[BOTTOM].D = clip[15] + clip[7];

    // Normalize the BOTTOM side
    _frustum[BOTTOM].NormalizePlane();

    // This will extract the TOP side of the frustum
    _frustum[TOP].A = clip[12] - clip[4];
    _frustum[TOP].B = clip[13] - clip[5];
    _frustum[TOP].C = clip[14] - clip[6];
    _frustum[TOP].D = clip[15] - clip[7];

    // Normalize the TOP side
    _frustum[TOP].NormalizePlane();

    // This will extract the BACK side of the frustum
    _frustum[BACK].A = clip[12] - clip[8];
    _frustum[BACK].B = clip[13] - clip[9];
    _frustum[BACK].C = clip[14] - clip[10];
    _frustum[BACK].D = clip[15] - clip[11];

    // Normalize the BACK side
    _frustum[BACK].NormalizePlane();

    // This will extract the FRONT side of the frustum
    _frustum[FRONT].A = clip[12] + clip[8];
    _frustum[FRONT].B = clip[13] + clip[9];
    _frustum[FRONT].C = clip[14] + clip[10];
    _frustum[FRONT].D = clip[15] + clip[11];

    // Normalize the FRONT side
    _frustum[FRONT].NormalizePlane();
}

bool atgFrustum::IsPointInFrustum( const float v[3] )
{
    // If you remember the plane equation (A*x + B*y + C*z + D = 0), then the rest
    // of this code should be quite obvious and easy to figure out yourself.
    // In case don't know the plane equation, it might be a good idea to look
    // at our Plane Collision tutorial at www.GameTutorials.com in OpenGL Tutorials.
    // I will briefly go over it here.  (A,B,C) is the (X,Y,Z) of the normal to the plane.
    // They are the same thing... but just called ABC because you don't want to say:
    // (x*x + y*y + z*z + d = 0).  That would be wrong, so they substitute them.
    // the (x, y, z) in the equation is the point that you are testing.  The D is
    // The distance the plane is from the origin.  The equation ends with "= 0" because
    // that is true when the point (x, y, z) is ON the plane.  When the point is NOT on
    // the plane, it is either a negative number (the point is behind the plane) or a
    // positive number (the point is in front of the plane).  We want to check if the point
    // is in front of the plane, so all we have to do is go through each point and make
    // sure the plane equation goes out to a positive number on each side of the frustum.
    // The result (be it positive or negative) is the distance the point is front the plane.

    // Go through all the sides of the frustum
    for(int i = 0; i < 6; i++ )
    {
        // Calculate the plane equation and check if the point is behind a side of the frustum
        if(_frustum[i].Resolve(v) <= 0)
        {
            // The point was behind a side, so it ISN'T in the frustum
            return false;
        }
    }

    // The point was inside of the frustum (In front of ALL the sides of the frustum)
    return true;
}

bool atgFrustum::IsSphereInFrustum( const float center[3], float radius )
{
    // Now this function is almost identical to the PointInFrustum(), except we
    // now have to deal with a radius around the point.  The point is the center of
    // the radius.  So, the point might be outside of the frustum, but it doesn't
    // mean that the rest of the sphere is.  It could be half and half.  So instead of
    // checking if it's less than 0, we need to add on the radius to that.  Say the
    // equation produced -2, which means the center of the sphere is the distance of
    // 2 behind the plane.  Well, what if the radius was 5?  The sphere is still inside,
    // so we would say, if(-2 < -5) then we are outside.  In that case it's false,
    // so we are inside of the frustum, but a distance of 3.  This is reflected below.

    // Go through all the sides of the frustum
    for(int i = 0; i < 6; i++ )
    {
        // If the center of the sphere is farther away from the plane than the radius
        if( _frustum[i].Resolve(center) <= -radius )
        {
            // The distance was greater than the radius so the sphere is outside of the frustum
            return false;
        }
    }

    // The sphere was inside of the frustum!
    return true;
}

bool atgFrustum::IsCubeInFrustum( const float center[3], float size )
{
    // This test is a bit more work, but not too much more complicated.
    // Basically, what is going on is, that we are given the center of the cube,
    // and half the length.  Think of it like a radius.  Then we checking each point
    // in the cube and seeing if it is inside the frustum.  If a point is found in front
    // of a side, then we skip to the next side.  If we get to a plane that does NOT have
    // a point in front of it, then it will return false.

    // *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
    // This happens when all the corners of the bounding box are not behind any one plane.
    // This is rare and shouldn't effect the overall rendering speed.
    float x = center[0];
    float y = center[1];
    float z = center[2];

    for(int i = 0; i < 6; i++ )
    {
        if(_frustum[i].A * (x - size) + _frustum[i].B * (y - size) + _frustum[i].C * (z - size) + _frustum[i].D > 0)
            continue;
        if(_frustum[i].A * (x + size) + _frustum[i].B * (y - size) + _frustum[i].C * (z - size) + _frustum[i].D > 0)
            continue;
        if(_frustum[i].A * (x - size) + _frustum[i].B * (y + size) + _frustum[i].C * (z - size) + _frustum[i].D > 0)
            continue;
        if(_frustum[i].A * (x + size) + _frustum[i].B * (y + size) + _frustum[i].C * (z - size) + _frustum[i].D > 0)
            continue;
        if(_frustum[i].A * (x - size) + _frustum[i].B * (y - size) + _frustum[i].C * (z + size) + _frustum[i].D > 0)
            continue;
        if(_frustum[i].A * (x + size) + _frustum[i].B * (y - size) + _frustum[i].C * (z + size) + _frustum[i].D > 0)
            continue;
        if(_frustum[i].A * (x - size) + _frustum[i].B * (y + size) + _frustum[i].C * (z + size) + _frustum[i].D > 0)
            continue;
        if(_frustum[i].A * (x + size) + _frustum[i].B * (y + size) + _frustum[i].C * (z + size) + _frustum[i].D > 0)
            continue;

        // If we get here, it isn't in the frustum
        return false;
    }

    return true;
}

bool atgFrustum::IsAABBoxInFurstum(const AABBox& bbox)
{
    if (IsPointInFrustum(bbox.vMin.m) && IsPointInFrustum(bbox.vMax.m))
    {
        return true;
    }

    return false;
}

void atgFrustum::DebugRender()
{
    float p1[3];
    float p2[3];
    //后上
    Plane::Intersection(_frustum[BACK].m, _frustum[TOP].m, _frustum[LEFT].m, p1);
    Plane::Intersection(_frustum[BACK].m, _frustum[TOP].m, _frustum[RIGHT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3One.m);
    //后下
    Plane::Intersection(_frustum[BACK].m, _frustum[BOTTOM].m, _frustum[LEFT].m, p1);
    Plane::Intersection(_frustum[BACK].m, _frustum[BOTTOM].m, _frustum[RIGHT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3One.m);
    //后左
    Plane::Intersection(_frustum[BACK].m, _frustum[TOP].m, _frustum[LEFT].m, p1);
    Plane::Intersection(_frustum[BACK].m, _frustum[BOTTOM].m, _frustum[LEFT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3One.m);
    //后右
    Plane::Intersection(_frustum[BACK].m, _frustum[TOP].m, _frustum[RIGHT].m, p1);
    Plane::Intersection(_frustum[BACK].m, _frustum[BOTTOM].m, _frustum[RIGHT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3One.m);

    //前上
    Plane::Intersection(_frustum[FRONT].m, _frustum[TOP].m, _frustum[LEFT].m, p1);
    Plane::Intersection(_frustum[FRONT].m, _frustum[TOP].m, _frustum[RIGHT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3Right.m);
    //前下
    Plane::Intersection(_frustum[FRONT].m, _frustum[BOTTOM].m, _frustum[LEFT].m, p1);
    Plane::Intersection(_frustum[FRONT].m, _frustum[BOTTOM].m, _frustum[RIGHT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3Right.m);
    //前左
    Plane::Intersection(_frustum[FRONT].m, _frustum[TOP].m, _frustum[LEFT].m, p1);
    Plane::Intersection(_frustum[FRONT].m, _frustum[BOTTOM].m, _frustum[LEFT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3Right.m);
    //前右
    Plane::Intersection(_frustum[FRONT].m, _frustum[TOP].m, _frustum[RIGHT].m, p1);
    Plane::Intersection(_frustum[FRONT].m, _frustum[BOTTOM].m, _frustum[RIGHT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3Right.m);


    //左上
    Plane::Intersection(_frustum[BACK].m, _frustum[TOP].m, _frustum[LEFT].m, p1);
    Plane::Intersection(_frustum[FRONT].m, _frustum[TOP].m, _frustum[LEFT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3Up.m);
    //左下
    Plane::Intersection(_frustum[BACK].m, _frustum[BOTTOM].m, _frustum[LEFT].m, p1);
    Plane::Intersection(_frustum[FRONT].m, _frustum[BOTTOM].m, _frustum[LEFT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3Up.m);
    //右上
    Plane::Intersection(_frustum[BACK].m, _frustum[TOP].m, _frustum[RIGHT].m, p1);
    Plane::Intersection(_frustum[FRONT].m, _frustum[TOP].m, _frustum[RIGHT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3Forward.m);
    //右下
    Plane::Intersection(_frustum[BACK].m, _frustum[BOTTOM].m, _frustum[RIGHT].m, p1);
    Plane::Intersection(_frustum[FRONT].m, _frustum[BOTTOM].m, _frustum[RIGHT].m, p2);
    g_Renderer->DrawLine(p1, p2, Vec3Forward.m);
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// WOZZERS!  That seemed like an incredible amount to look at, but if you break it
// down, it's not.  Frustum culling is a VERY useful thing when it comes to 3D.
// If you want a large world, there is no way you are going to send it down the
// 3D pipeline every frame and let OpenGL take care of it for you.  That would
// give you a 0.001 frame rate.  If you hit '+' and bring the sphere count up to
// 1000, then take off culling, you will see the HUGE difference it makes.
// Also, you wouldn't really be rendering 1000 spheres.  You would most likely
// use the sphere code for larger objects.  Let me explain.  Say you have a bunch
// of objects, well... all you need to do is give the objects a radius, and then
// test that radius against the frustum.  If that sphere is in the frustum, then you
// render that object.  Also, you won't be rendering a high poly sphere so it won't
// be so slow.  This goes for bounding box's too (Cubes).  If you don't want to
// do a cube, it is really easy to convert the code for rectangles.  Just pass in
// a width and height, instead of just a length.  Remember, it's HALF the length of
// the cube, not the full length.  So it would be half the width and height for a rect.
//
// This is a perfect starter for an octree tutorial.  Wrap you head around the concepts
// here and then see if you can apply this to making an octree.  Hopefully we will have
// a tutorial up and running for this subject soon.  Once you have frustum culling,
// the next step is getting space partitioning.  Either it being a BSP tree of an Octree.
//
// Let's go over a brief overview of the things we learned here:
//
// 1) First we need to abstract the frustum from OpenGL.  To do that we need the
//    projection and modelview matrix.  To get the projection matrix we use:
//
//          glGetFloatv( GL_PROJECTION_MATRIX, /* An Array of 16 floats */ );
//    Then, to get the modelview matrix we use:
//
//          glGetFloatv( GL_MODELVIEW_MATRIX, /* An Array of 16 floats */ );
//
//    These 2 functions gives us an array of 16 floats (The matrix).
//
// 2) Next, we need to combine these 2 matrices.  We do that by matrix multiplication.
//
// 3) Now that we have the 2 matrixes combined, we can abstract the sides of the frustum.
//    This will give us the normal and the distance from the plane to the origin (ABC and D).
//
// 4) After abstracting a side, we want to normalize the plane data.  (A B C and D).
//
// 5) Now we have our frustum, and we can check points against it using the plane equation.
//    Once again, the plane equation (A*x + B*y + C*z + D = 0) says that if, point (X,Y,Z)
//    times the normal of the plane (A,B,C), plus the distance of the plane from origin,
//    will equal 0 if the point (X, Y, Z) lies on that plane.  If it is behind the plane
//    it will be a negative distance, if it's in front of the plane (the way the normal is facing)
//    it will be a positive number.
//
//
// If you need more help on the plane equation and why this works, download our
// Ray Plane Intersection Tutorial at www.GameTutorials.com.
//
// That's pretty much it with frustums.  There is a lot more we could talk about, but
// I don't want to complicate this tutorial more than I already have.
//
// I want to thank Mark Morley for his tutorial on frustum culling.  Most of everything I got
// here comes from his teaching.  If you want more in-depth, visit his tutorial at:
//
// http://www.markmorley.com/opengl/frustumculling.html
//
// Good luck!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com