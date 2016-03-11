#pragma once

#include "atgMath.h"

class atgMaterial;
class atgIndexBuffer;
class atgVertexBuffer;

class atgMesh
{
public:
    atgMesh(void);
    virtual ~atgMesh(void);

    struct Vertices
    {
        uint32  number;         //>顶点数量
        float*  positions;      //>3d程序,当然一个顶点数据由3个float组成.
        float*  normals;        //>法线3个float组成
        float*  texureCoords;   //>纹理坐标2个float组成.
        float*  colors;         //>顶点色4个float组成
    };

    class SubMesh
    {
    public:
        struct Faces
        {
            uint32     primitiveType;    //PrimitveType
            uint32     number;           //>多个面
            uint32     numberIndices;    //>多少个索引顶点组成一个面
            uint32*    indices;         //>面数据
        };

        SubMesh();

        void Draw();

    //private:
        atgMesh*    _parent;
        Faces       _faces;
        uint8       _materialIndex;   //>材质id


        atgIndexBuffer* _indexBuffer;
    };

    void Render();
    void Render(std::vector<uint8> submeshIndices);

    Vertices                    _vertices;
    std::vector<SubMesh>        _submeshs; 
    std::vector<atgMaterial*>   _materials;

    atgVertexBuffer*      _vertexBuffer;
    
    atgMatrix             _transformation;

};


// The code below generates a four-component tangent T in which the handedness of the local 
// coordinate system is stored as ±1 in the w-coordinate. 
// The bitangent vector B is then given by B = (N × T) · T.w
void CalculateTangentArray(long vertexCount, const float *position, const float *normal,
                           const float *texcoord, long triangleCount, const uint32 *triangle,
                           float *tangent);