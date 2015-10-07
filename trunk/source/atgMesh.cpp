#include "atgBase.h"
#include "atgMesh.h"
#include "atgRenderer.h"
#include "atgMaterial.h"
#include "atgProfile.h"

atgMesh::atgMesh(void)
{
    _vertexBuffer = NULL;
    _indexBuffer = NULL;
    _material = NULL;

    _vertexsDataDirty = false;
    _indicesDataDirty = false;
}

atgMesh::~atgMesh(void)
{
}

uint32 atgMesh::GetNumberOfVertexs() const
{
    return 0;
}

void* atgMesh::GetVertexs()
{
    return NULL;
}

atgVertexDecl* atgMesh::GetVertexDeclaration() const
{
    return NULL;
}

bool atgMesh::IsVertexsDataDirty() const
{
    return _vertexsDataDirty;
}


uint32 atgMesh::GetNumberOfIndices() const
{
    return 0;
}

void* atgMesh::GetIndices()
{
    return NULL;
}

uint32 atgMesh::GetIndexFormat() const
{
    return 0;
}

bool atgMesh::IsIndicesDataDirty() const
{
    return _indicesDataDirty;
}

void atgMesh::SetMaterial( atgMaterial* material )
{
    _material = material;
}

atgMaterial* atgMesh::GetMaterial()
{
    return _material;
}

void atgMesh::Render()
{
    ATG_PROFILE("atgMesh::Render");
    //AASSERT(g_Renderer);
    atgVertexDecl* pVertexDecl = NULL; 
    if (_vertexBuffer && IsVertexsDataDirty())
    {
        ATG_PROFILE("atgMesh::UpdateVertexBuffer");
#if defined OPENGL_USE_MAP
        pVertexDecl = GetVertexDeclaration();
        AASSERT(pVertexDecl);
        uint32 newDataSize = GetNumberOfVertexs() * pVertexDecl->GetElementsStride();
        if (_vertexBuffer->GetSize() < newDataSize)
        {
            SAFE_DELETE( _vertexBuffer );
        }else
        {
            void* pData = _vertexBuffer->Lock(0, newDataSize);
            if (pData)
            {
                memcpy(pData, GetVertexs(), newDataSize);
                _vertexBuffer->Unlock();
            }
        }
#else
        SAFE_DELETE( _vertexBuffer );
#endif
    }

    if (!_vertexBuffer)
    {
        ATG_PROFILE("atgMesh::UpdateVertexBuffer");
        pVertexDecl = GetVertexDeclaration();
        AASSERT(pVertexDecl);
        _vertexBuffer = new atgVertexBuffer();
        if (_vertexBuffer)
        {
            bool rs = _vertexBuffer->Create(pVertexDecl, 
                                            GetVertexs(), 
                                            GetNumberOfVertexs() * pVertexDecl->GetElementsStride(), 
                                            BAM_Static);
            if (!rs)
            {
                SAFE_DELETE(_vertexBuffer);
                return;
            }
        
        }
    }

    if (_indexBuffer && IsIndicesDataDirty())
    {
        uint32 newDataSize = GetNumberOfIndices() * sizeof(uint16);
        if (_indexBuffer->GetSize() < newDataSize)
        {
            SAFE_DELETE( _indexBuffer );
        }else
        {
            void* pData = _indexBuffer->Lock(0, newDataSize);
            if (pData)
            {
                memcpy(pData, GetVertexs(), newDataSize);
                _indexBuffer->Unlock();
            }
        }
    }

    if (!_indexBuffer)
    {
        _indexBuffer = new atgIndexBuffer();
        if (_indexBuffer)
        {
            bool rs = _indexBuffer->Create(GetIndices(), 
                                           GetNumberOfIndices(), 
                                           (atgIndexBuffer::IndexFormat)GetIndexFormat(), 
                                           BAM_Static);
            if (!rs)
            {
                SAFE_DELETE(_indexBuffer);
                return;
            }
        }

    }

    if (_material)
    {
        _material->Setup();
        g_Renderer->BindVertexBuffer(_vertexBuffer);
        g_Renderer->BindIndexBuffer(_indexBuffer);
        g_Renderer->DrawIndexedPrimitive(PT_TRIANGLES, GetNumberOfIndices() / 3, GetNumberOfIndices(), GetNumberOfVertexs());
        g_Renderer->BindVertexBuffer(NULL);
        g_Renderer->BindIndexBuffer(NULL);
        _material->Desetup();
    }
}



