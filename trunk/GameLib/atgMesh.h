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
        uint32  number;         //>��������
        float*  positions;      //>3d����,��Ȼһ������������3��float���.
        float*  normals;        //>����3��float���
        float*  texureCoords;   //>��������2��float���.
        float*  colors;         //>����ɫ4��float���
    };

    class SubMesh
    {
    public:
        struct Faces
        {
            uint32     primitiveType;    //PrimitveType
            uint32     number;           //>�����
            uint32     numberIndices;    //>���ٸ������������һ����
            uint32*    indices;         //>������
        };

        SubMesh();

        void Draw();

    //private:
        atgMesh*    _parent;
        Faces       _faces;
        uint8       _materialIndex;   //>����id


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
// coordinate system is stored as ��1 in the w-coordinate. 
// The bitangent vector B is then given by B = (N �� T) �� T.w
void CalculateTangentArray(long vertexCount, const float *position, const float *normal,
                           const float *texcoord, long triangleCount, const uint32 *triangle,
                           float *tangent);