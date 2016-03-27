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

    if (vertices.tangent != NULL)
    {
        decl.AppendElement(0, atgVertexDecl::VA_Tangent);
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

        if (vertices.tangent)
        {
            uint32 index = i * 4;
            *ptr++ = vertices.tangent[index++];
            *ptr++ = vertices.tangent[index++];
            *ptr++ = vertices.tangent[index++];
            *ptr++ = vertices.tangent[index++];
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
    char* p = (char*)Ptr;
    SAFE_DELETE_ARRAY(p);
}


atgMesh::atgMesh(void)
{
    //memset(&_vertices, 0 , sizeof(_vertices));
    _vertexBuffer = NULL;

    _drawTangentSpace = false;
}

atgMesh::~atgMesh(void)
{
    std::vector<SubMesh>::iterator it = _submeshs.begin();
    for (std::vector<SubMesh>::iterator end = _submeshs.end(); it != end; ++it)
    {
        SAFE_DELETE_ARRAY((*it)._faces.indices);
    }

   SAFE_DELETE_ARRAY(_vertices.positions);
   SAFE_DELETE_ARRAY(_vertices.normals);
   SAFE_DELETE_ARRAY(_vertices.tangent);
   SAFE_DELETE_ARRAY(_vertices.colors);
   SAFE_DELETE_ARRAY(_vertices.texureCoords);
}

void atgMesh::Render(const atgMatrix& worldMatrix)
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

        g_Renderer->SetMatrix(MD_WORLD, worldMatrix * _transformation);

        std::vector<SubMesh>::iterator it = _submeshs.begin();
        for (std::vector<SubMesh>::iterator end = _submeshs.end(); it != end; ++it)
        {
            _materials[it->_materialIndex]->Setup();
            _vertexBuffer->Bind();
            it->Draw();
            _vertexBuffer->Unbind();
            _materials[it->_materialIndex]->Desetup();

            if (_drawTangentSpace)
            {
                it->DrawTangentSpace();
            }
        }
    }
}

void atgMesh::Render(std::vector<uint8> submeshIndices, const atgMatrix& worldMatrix)
{
    if(_vertices.number  > 0 && !_submeshs.empty())
    {
        if (!submeshIndices.empty())
        {
            g_Renderer->SetMatrix(MD_WORLD, worldMatrix * _transformation);

            std::vector<uint8>::iterator it = submeshIndices.begin();
            for (std::vector<uint8>::iterator end = submeshIndices.end(); it != end; ++it)
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
                                                   atgIndexBuffer::IFB_Index16, 
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

void atgMesh::SubMesh::DrawTangentSpace()
{
    if (_parent)
    {
        if (_faces.number > 0)
        {
            atgMesh::Vertices& vertices = _parent->_vertices;
            if (vertices.tangent == NULL || vertices.normals == NULL)
                return;

            atgVec3* pVec3Buf = new atgVec3[vertices.number * 2];
            atgVec3* pVec3 = pVec3Buf;
            float* ptr = (float*)pVec3Buf;
            
            for (uint32 i = 0; i < vertices.number; ++i)
            {
                uint32 index = i * 3;
                *ptr++ = vertices.positions[index];
                *ptr++ = vertices.positions[index+1];
                *ptr++ = vertices.positions[index+2];

                *ptr++ = vertices.positions[index] + vertices.normals[index] * 0.1f;
                *ptr++ = vertices.positions[index+1] + vertices.normals[index+1] * 0.1f;
                *ptr++ = vertices.positions[index+2] + vertices.normals[index+2] * 0.1f;
            }

            g_Renderer->BeginLine();
            for (uint32 i = 0; i < vertices.number; ++i, pVec3 += 2)
            {
                g_Renderer->AddLine(*pVec3, *(pVec3 + 1), Vec3Up);
            }
            g_Renderer->EndLine();
            
            pVec3 = pVec3Buf; 
            ptr = (float*)pVec3Buf;
            for (uint32 i = 0; i < vertices.number; ++i)
            {
                uint32 index = i * 3;
                *ptr++ = vertices.positions[index];
                *ptr++ = vertices.positions[index+1];
                *ptr++ = vertices.positions[index+2];

                uint32 index_t = i * 4;
                *ptr++ = vertices.positions[index] + vertices.tangent[index_t] * 0.1f;
                *ptr++ = vertices.positions[index+1] + vertices.tangent[index_t+1] * 0.1f;
                *ptr++ = vertices.positions[index+2] + vertices.tangent[index_t+2] * 0.1f;
            }

            g_Renderer->BeginLine();
            for (uint32 i = 0; i < vertices.number; ++i, pVec3 += 2)
            {
                g_Renderer->AddLine(*pVec3, *(pVec3 + 1), Vec3Right);
            }
            g_Renderer->EndLine();
            
            
            pVec3 = pVec3Buf; 
            ptr = (float*)pVec3Buf;
            for (uint32 i = 0; i < vertices.number; ++i)
            {
                uint32 index = i * 3;
                *ptr++ = vertices.positions[index];
                *ptr++ = vertices.positions[index+1];
                *ptr++ = vertices.positions[index+2];

                uint32 index_t = i * 4;
                *ptr++ = vertices.positions[index] + (vertices.normals[index+1]*vertices.tangent[index_t+2] - vertices.normals[index+2]*vertices.tangent[index_t+1]) * 0.1f;
                *ptr++ = vertices.positions[index+1] + (vertices.normals[index+2]*vertices.tangent[index_t] - vertices.normals[index]*vertices.tangent[index_t+2])* 0.1f;
                *ptr++ = vertices.positions[index+2] + (vertices.normals[index]*vertices.tangent[index_t+1] - vertices.normals[index+1]*vertices.tangent[index_t])* 0.1f;
            }

            g_Renderer->BeginLine();
            for (uint32 i = 0; i < vertices.number; ++i, pVec3 += 2)
            {
                g_Renderer->AddLine(*pVec3, *(pVec3 + 1), Vec3Forward);
            }
            g_Renderer->EndLine();
            
            SAFE_DELETE_ARRAY(pVec3Buf);
        }
    }
}

void CalculateTangentSpaceVector(long vertexCount, const float *position, const float *normal,
                                 const float *texcoord, long triangleCount, const uint16 *triangle,
                                 float *tangent)
{
    struct Face { uint16 index[3]; };

    Face* pTri = (Face *)triangle;
    atgVec3* pPos = (atgVec3*)position;
    atgVec3* pNor = (atgVec3*)normal;
    atgVec2* pUV = (atgVec2*)texcoord;
    atgVec4* pTan = (atgVec4*)tangent;

    for (long a = 0; a < triangleCount; a++)
    {
        uint16 i1 = pTri->index[0];
        uint16 i2 = pTri->index[1];
        uint16 i3 = pTri->index[2];

        const atgVec3& position1 = pPos[i1];
        const atgVec3& position2 = pPos[i2];
        const atgVec3& position3 = pPos[i3];

        const atgVec2& uv1 = pUV[i1];
        const atgVec2& uv2 = pUV[i2];
        const atgVec2& uv3 = pUV[i3];

        //side0 is the vector along one side of the triangle of vertices passed in, 
        //and side1 is the vector along another side. Taking the cross product of these returns the normal.
        atgVec3 side0 = position1 - position2;
        atgVec3 side1 = position3 - position1;

        //Calculate face normal
        atgVec3 normal = side1.Cross(side0);
        normal.Normalize();

        //Now we use a formula to calculate the tangent. 
        float deltaV0 = uv1.y - uv2.y;
        float deltaV1 = uv3.y - uv1.y;
        atgVec3 tangent = deltaV1 * side0 - deltaV0 * side1;
        tangent.Normalize();

        //Calculate binormal
        float deltaU0 = uv1.x - uv2.x;
        float deltaU1 = uv3.x - uv1.x;
        atgVec3 binormal = deltaU1 * side0 - deltaU0 * side1;
        binormal.Normalize();

        //Now, we take the cross product of the tangents to get a vector which 
        //should point in the same direction as our normal calculated above. 
        //If it points in the opposite direction (the dot product between the normals is less than zero), 
        //then we need to reverse the s and t tangents. 
        //This is because the triangle has been mirrored when going from tangent space to object space.
        //reverse tangents if necessary
        atgVec3 tangentCross = tangent.Cross(binormal);
        if (tangentCross.Dot(normal) < 0.0f)
        {
            tangent = -tangent;
            binormal = -binormal;
        }

        pTan[i1].Set(tangent.x, tangent.y, tangent.z, 1.0);
        pTan[i2].Set(tangent.x, tangent.y, tangent.z, 1.0);
        pTan[i3].Set(tangent.x, tangent.y, tangent.z, 1.0);

        //pTan[i1] +=tangent;
        //pTan[i2] +=tangent;
        //pTan[i3] +=tangent;

        pTri++;
    }


    //for (long a = 0; a < vertexCount; a++)
    //{
    //    // Gram-Schmidt orthogonalize
    //    atgVec3 v3 = Vec4ClipToVec3(pTan[a]);
    //    v3.Normalize();
    //    pTan[a].Set(v3.x, v3.y, v3.z, 1.0f);
    //}
}


// http://www.terathon.com/code/tangent.html
//
void CalculateTangentArray(long vertexCount, const float *position, const float *normal,
                           const float *texcoord, long triangleCount, const uint16 *triangle,
                           float *tangent)
{
    struct Face { uint16 index[3]; };

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
        uint16 i1 = pTri->index[0];
        uint16 i2 = pTri->index[1];
        uint16 i3 = pTri->index[2];

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

        float r = 1.0f / (s1 * t2 - s2 * t1);
        atgVec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,(t2 * z1 - t1 * z2) * r);
        atgVec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,(s1 * z2 - s2 * z1) * r);

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
        pTan[a] = Vec3ToVec4((t - n * n.Dot(t)).Normalize()) * 1.0f;

        // Calculate handedness
        pTan[a].w = (n.Cross(t).Dot(tan2[a]) < 0.0f) ? -1.0f : 1.0f;
    }

    delete[] tan1;
}