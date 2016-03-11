#include "atgBase.h"
#include "atgMesh.h"
#include "atgRenderer.h"
#include "atgMaterial.h"

atgVertexDecl GetVertexDeclaration(atgMesh::Vertices& vertices)
{
    atgVertexDecl decl;
    if (vertices.positions != NULL)
    {
        decl.AppendElement(0, atgVertexDecl::VA_Pos3);
    }

    if (vertices.normals != NULL)
    {
        decl.AppendElement(0, atgVertexDecl::VA_Normal);
    }

    if (vertices.texureCoords != NULL)
    {
        decl.AppendElement(0, atgVertexDecl::VA_Texture0);
    }

    if (vertices.colors != NULL)
    {
        decl.AppendElement(0, atgVertexDecl::VA_Diffuse);
    }
    return decl;
}

void* AllocVertices(atgMesh::Vertices& vertices, atgVertexDecl& decl)
{
    float* pVertices = (float*)new char[decl.GetElementsStride() *  vertices.number];
    float* ptr = pVertices;
    for (uint32 i = 0; i < vertices.number; ++i)
    {
        if (vertices.positions)
        {
            uint32 index = i * 3;
            *ptr++ = vertices.positions[index++];
            *ptr++ = vertices.positions[index++];
            *ptr++ = vertices.positions[index++];
        }

        if (vertices.normals)
        {
            uint32 index = i * 3;
            *ptr++ = vertices.normals[index++];
            *ptr++ = vertices.normals[index++];
            *ptr++ = vertices.normals[index++];
        }

        if (vertices.texureCoords)
        {
            uint32 index = i * 2;
            *ptr++ = vertices.texureCoords[index++];
            *ptr++ = vertices.texureCoords[index++];
        }

        if (vertices.colors)
        {
            uint32 index = i * 4;
            *ptr++ = vertices.colors[index++];
            *ptr++ = vertices.colors[index++];
            *ptr++ = vertices.colors[index++];
            *ptr++ = vertices.colors[index++];
        }
    }

    return pVertices;
}

inline void FreeVertices(void* Ptr)
{
    SAFE_DELETE_ARRAY(Ptr);
}


atgMesh::atgMesh(void)
{
    memset(&_vertices, 0 , sizeof(_vertices));
    _vertexBuffer = NULL;
}

atgMesh::~atgMesh(void)
{
   
}

void atgMesh::Render()
{
    if(_vertices.number  > 0 && !_submeshs.empty())
    {
        if (!_vertexBuffer || _vertexBuffer->IsLost())
        {
            SAFE_DELETE(_vertexBuffer);
            atgVertexDecl vertexDecl = GetVertexDeclaration(_vertices);
            if (0 == vertexDecl.GetNumberElement())
            {
                return; 
            }

            _vertexBuffer = new atgVertexBuffer();
            if (_vertexBuffer)
            {
                void* verticesBuf = AllocVertices(_vertices, vertexDecl);

                bool rs = _vertexBuffer->Create(&vertexDecl,
                                                verticesBuf, 
                                                _vertices.number * vertexDecl.GetElementsStride(), 
                                                BAM_Static);
                
                FreeVertices(verticesBuf);

                if (!rs)
                {
                    SAFE_DELETE(_vertexBuffer);
                    return;
                }
            }
        }

        g_Renderer->SetMatrix(MD_WORLD, _transformation);

        auto it = _submeshs.begin();
        for (auto end = _submeshs.end(); it != end; ++it)
        {
            _materials[it->_materialIndex]->Setup();
            _vertexBuffer->Bind();
            it->Draw();
            _vertexBuffer->Unbind();
            //float* tangent = (float*) new char[_vertices.number * sizeof(atgVec4)];
            //CalculateTangentArray(_vertices.number, _vertices.positions, _vertices.normals, _vertices.texureCoords,
            //                      it->_faces.number, it->_faces.indices, tangent);

            _materials[it->_materialIndex]->Desetup();
        }
    }
}

void atgMesh::Render( std::vector<uint8> submeshIndices )
{
    if(_vertices.number  > 0 && !_submeshs.empty())
    {
        if (!submeshIndices.empty())
        {
            auto it = submeshIndices.begin();
            for (auto end = submeshIndices.end(); it != end; ++it)
            {
                if (*it < _submeshs.size())
                {
                    _submeshs[*it].Draw();
                }
            }
        }
    }
}


atgMesh::SubMesh::SubMesh()
{
    _parent = NULL;
    _indexBuffer = NULL;

    memset(&_faces, 0 , sizeof(_faces));
}

void atgMesh::SubMesh::Draw()
{
    if (_parent)
    {
        if (_faces.number > 0)
        {
            if (!_indexBuffer || _indexBuffer->IsLost())
            {
                SAFE_DELETE(_indexBuffer);
                _indexBuffer = new atgIndexBuffer();
                if (_indexBuffer)
                {
                    bool rs = _indexBuffer->Create(_faces.indices, 
                                                   _faces.number * _faces.numberIndices, 
                                                   atgIndexBuffer::IFB_Index32, 
                                                   BAM_Static);
                    if (!rs)
                    {
                        SAFE_DELETE(_indexBuffer);
                        return;
                    }
                }
            }

            _indexBuffer->Bind();
            g_Renderer->DrawIndexedPrimitive((PrimitveType)_faces.primitiveType, _faces.number, _faces.number * _faces.numberIndices, _parent->_vertices.number);
            _indexBuffer->Unbind();
        }
    }
}


void CalculateTangentArray(long vertexCount, const float *position, const float *normal,
                           const float *texcoord, long triangleCount, const uint32 *triangle,
                           float *tangent)
{
    struct Face { uint32 index[3]; };

    struct Position { float x; float y; float z; };

    struct Texcoord { float x; float y; };


    atgVec3 *tan1 = (atgVec3 *)new char[vertexCount * 2 * sizeof(atgVec3)];
    atgVec3 *tan2 = tan1 + vertexCount;
    memset(tan1, 0, vertexCount * 2 * sizeof(atgVec3));

    Face* pTri = (Face *)triangle;
    Position* pPos = (Position*)position;
    atgVec3* pNor = (atgVec3*)normal;
    Texcoord* pUV = (Texcoord*)texcoord;
    atgVec4* pTan = (atgVec4*)tangent;

    for (long a = 0; a < triangleCount; a++)
    {
        long i1 = pTri->index[0];
        long i2 = pTri->index[1];
        long i3 = pTri->index[2];

        const Position& v1 = pPos[i1];
        const Position& v2 = pPos[i2];
        const Position& v3 = pPos[i3];

        const Texcoord& w1 = pUV[i1];
        const Texcoord& w2 = pUV[i2];
        const Texcoord& w3 = pUV[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        atgVec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
            (t2 * z1 - t1 * z2) * r);
        atgVec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
            (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;

        pTri++;
    }

    for (long a = 0; a < vertexCount; a++)
    {
        const atgVec3& n = pNor[a];
        const atgVec3& t = tan1[a];

        // Gram-Schmidt orthogonalize
        pTan[a] = Vec3ToVec4((t - n * n.Dot(t)).Normalize());

        // Calculate handedness
        pTan[a].w = (n.Cross(t).Dot(tan2[a]) < 0.0F) ? -1.0F : 1.0F;
    }

    delete[] tan1;
}