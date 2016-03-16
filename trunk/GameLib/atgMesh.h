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
        float*  tangent;        //>����4��float���
        Vertices():number(0),positions(0),normals(0),texureCoords(0),colors(0),tangent(0) {}
    };

    class SubMesh
    {
    public:
        struct Faces
        {
            uint32     primitiveType;    //PrimitveType
            uint32     number;           //>�����
            uint32     numberIndices;    //>���ٸ������������һ���� (opengles 2.0֧�� byte or short)
            uint16*    indices;          //>������                   (opengles 2.0֧�� byte or short)
        };

        SubMesh();

        void Draw();

        void DrawTangentSpace();

    //private:
        atgMesh*    _parent;
        Faces       _faces;
        uint8       _materialIndex;   //>����id


        atgIndexBuffer* _indexBuffer;
    };

    void Render();
    void Render(std::vector<uint8> submeshIndices);

    void SetDrawTanget(bool enable) { _drawTangentSpace = enable; }

    Vertices                    _vertices;
    std::vector<SubMesh>        _submeshs; 
    std::vector<atgMaterial*>   _materials;

    atgVertexBuffer*      _vertexBuffer;
    
    atgMatrix             _transformation;

    bool                  _drawTangentSpace;
};


// The code below generates a four-component tangent T in which the handedness of the local 
// coordinate system is stored as ��1 in the w-coordinate. 
// The bitangent vector B is then given by B = (N �� T) �� T.w
void CalculateTangentArray(long vertexCount, const float *position, const float *normal,
                           const float *texcoord, long triangleCount, const uint16 *triangle,
                           float *tangent);

// ogre implement.
void CalculateTangentSpaceVector(long vertexCount, const float *position, const float *normal,
                                 const float *texcoord, long triangleCount, const uint16 *triangle,
                                 float *tangent);