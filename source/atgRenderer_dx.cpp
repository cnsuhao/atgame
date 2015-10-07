#include "atgBase.h"
#include "atgRenderer.h"

#ifdef USE_DIRECTX

//////////////////////////////////////////////////////////////////////////
#include <d3d9.h>
#include <d3dx9.h>

IDirect3D9*             g_pD3D = NULL;              // D3D9 InterFace
IDirect3DDevice9*       g_pd3dDevice = NULL;        // D3D9 Devices InterFace
D3DPRESENT_PARAMETERS*  g_d3dpp = NULL; 

#define SAFE_RELEASE(x) { if(x) { x->Release(); x = NULL; } }

//////////////////////////////////////////////////////////////////////////
// index buffer
class atgIndexBufferImpl
{
public:
    atgIndexBufferImpl():pDXIB(0),accessMode(BAM_Static),locked(0){}
    bool Bind();
    bool Unbind();
public:
    IDirect3DIndexBuffer9* pDXIB;
    BufferAccessMode accessMode;
    bool locked;
};

bool atgIndexBufferImpl::Bind()
{
    if(pDXIB)
    {
        DX_ASSERT( g_pd3dDevice->SetIndices(pDXIB) );
        return true;
    }
    return false;
}

bool atgIndexBufferImpl::Unbind()
{
    DX_ASSERT( g_pd3dDevice->SetIndices(NULL) );
    return true;
}


atgIndexBuffer::atgIndexBuffer():_impl(NULL)
{

}

atgIndexBuffer::~atgIndexBuffer()
{

}

bool atgIndexBuffer::Create( const void *pIndices, uint32 numIndices, IndexFormat format, BufferAccessMode accessMode )
{
    _impl = new atgIndexBufferImpl;
    int indexSize = sizeof(uint16);
    D3DFORMAT d3dFormat = D3DFMT_INDEX16;
    if(format == IFB_Index32)
    {
        indexSize = sizeof(uint32);
        d3dFormat = D3DFMT_INDEX32;
    }
    DWORD d3dUsage = D3DUSAGE_WRITEONLY;
    _impl->accessMode = accessMode;
    if(accessMode == BAM_Dynamic)
        d3dUsage = d3dUsage | D3DUSAGE_DYNAMIC;
    uint32 buffSize = numIndices * indexSize;
    _size = buffSize;
    D3DPOOL d3dPool = D3DPOOL_DEFAULT;
    if( FAILED(g_pd3dDevice->CreateIndexBuffer( buffSize, 
        d3dUsage, d3dFormat, d3dPool, &_impl->pDXIB, NULL)))
    {
            return false;
    }
    // upload data
    void* pLockedBuffer = 0;
    pLockedBuffer = Lock(0, buffSize);
    if(pLockedBuffer)
    {
        memcpy(pLockedBuffer, pIndices, buffSize);
        Unlock();
        return true;
    }
    return false;
}

bool atgIndexBuffer::Destroy()
{
    if(_impl)
    {
        SAFE_RELEASE(_impl->pDXIB);
        SAFE_DELETE(_impl);
    }
    return true;
}

void* atgIndexBuffer::Lock( uint32 offset, uint32 size )
{
    AASSERT(_impl);
    void* pIndexBuffer = NULL;
    if( FAILED(_impl->pDXIB->Lock(offset, 
            size, 
            (void**)&pIndexBuffer,
            (_impl->accessMode == BAM_Dynamic ? D3DLOCK_DISCARD : 0)))){
        return NULL;
    }
    _impl->locked = true;
    return pIndexBuffer;
}

void atgIndexBuffer::Unlock()
{
    AASSERT(_impl);
    if(_impl->locked)
    {
        DX_ASSERT( _impl->pDXIB->Unlock() );
        _impl->locked = false;
    }
}

bool atgIndexBuffer::IsLocked() const
{
    if(_impl)
        return _impl->locked;
    return false;
}



//////////////////////////////////////////////////////////////////////////
//
#define Max_Number_Declaration_Elements 8
class atgVertexDeclImpl
{
public:
    atgVertexDeclImpl():numberElements(0),stride(0) {}
    atgVertexDeclImpl(const atgVertexDeclImpl& other)
    {
        numberElements = other.numberElements;
        stride = other.stride;
        for (uint8 i = 0; i < numberElements; ++i)
        {
            element[i].streamIndex = other.element[i].streamIndex;
            element[i].attribute = other.element[i].attribute;
        }
    }
public:
    uint8 numberElements;
    uint8 stride;
    struct DeclarationElement
    {
        uint8 streamIndex;
        atgVertexDecl::VertexAttribute attribute;
    };
    DeclarationElement element[Max_Number_Declaration_Elements];
};

atgVertexDecl::atgVertexDecl()
{
    _impl = new atgVertexDeclImpl;
}

atgVertexDecl::atgVertexDecl(const atgVertexDecl& other):_impl(NULL)
{
    *this = other;
}

atgVertexDecl::~atgVertexDecl()
{
    SAFE_DELETE(_impl);
}

atgVertexDecl& atgVertexDecl::operator= (const atgVertexDecl& other)
{
    SAFE_DELETE(_impl);
    _impl = new atgVertexDeclImpl();
    *_impl = *other._impl;
    return *this;
}

uint8 atgVertexDecl::GetNumberElement() const
{
    return _impl->numberElements; 
}

uint8 atgVertexDecl::GetElementsStride(void) const
{
    return _impl->stride;
}

bool atgVertexDecl::AppendElement( uint32 streamIndex, atgVertexDecl::VertexAttribute attribute )
{
    int numberElements = _impl->numberElements;
    if ((numberElements + 1) >= Max_Number_Declaration_Elements)
        return false;

    _impl->element[numberElements].streamIndex = streamIndex;
    _impl->element[numberElements].attribute = attribute;

    switch(attribute)
    {
    case atgVertexDecl::VA_Pos3:
    case atgVertexDecl::VA_Normal:
    case atgVertexDecl::VA_Diffuse:
    case atgVertexDecl::VA_Specular:
        {
            _impl->stride += 12;
            break;
        }
    case atgVertexDecl::VA_Pos4:
    case  atgVertexDecl::VA_BlendFactor4:
        {
            _impl->stride += 16;
            break;
        }
    case atgVertexDecl::VA_Texture0:
    case atgVertexDecl::VA_Texture1:
    case atgVertexDecl::VA_Texture2:
    case atgVertexDecl::VA_Texture3:
    case atgVertexDecl::VA_Texture4:
    case atgVertexDecl::VA_Texture5:
    case atgVertexDecl::VA_Texture6:
    case atgVertexDecl::VA_Texture7:
        {
            _impl->stride += 8;
            break;
        }
    case atgVertexDecl::VA_PointSize:
        {
            _impl->stride += 4;
            break;
        }
    default:
        AASSERT(0 && "error type of vertex declaration type. \n");
        return false;
    }

    ++(_impl->numberElements);
    return true;
}

//////////////////////////////////////////////////////////////////////////
// vertex buffer
class atgVertexBufferImpl
{
public:
    atgVertexBufferImpl():pDXVB(NULL),pDXVD(NULL),accessMode(BAM_Static),locked(false){}
    ~atgVertexBufferImpl(){}
    bool Bind();
    bool Unbind();
public:
    atgVertexDecl decl;
    IDirect3DVertexBuffer9* pDXVB;
    IDirect3DVertexDeclaration9* pDXVD;
    BufferAccessMode accessMode;
    bool locked;
};

bool atgVertexBufferImpl::Bind()
{
    DX_ASSERT( g_pd3dDevice->SetStreamSource(0, pDXVB, 0, decl.GetElementsStride()) );
    DX_ASSERT( g_pd3dDevice->SetVertexDeclaration(pDXVD) );

    return true;
}

bool atgVertexBufferImpl::Unbind()
{
    DX_ASSERT( g_pd3dDevice->SetStreamSource(0, NULL, 0, 0) );
    DX_ASSERT( g_pd3dDevice->SetVertexDeclaration(NULL) );
    return true;
}

atgVertexBuffer::atgVertexBuffer()
{
    _impl = new atgVertexBufferImpl;
    _size = 0;
}

atgVertexBuffer::~atgVertexBuffer()
{

}

bool atgVertexBuffer::Create( atgVertexDecl* decl, const void *pData, uint32 size, BufferAccessMode accessMode )
{
    static D3DVERTEXELEMENT9 sElementEnd = {0xFF,0,D3DDECLTYPE_UNUSED,0,0,0};
    _impl->decl = *decl;
    _size = size;
    uint32 numberVDE = decl->GetNumberElement();
    D3DVERTEXELEMENT9* pDXElement = new D3DVERTEXELEMENT9[numberVDE + 1];
    uint32 Offset = 0;
    uint32 elementStride = 0;
    for (uint32 i = 0; i < numberVDE; i++)
    {
        pDXElement[i].Stream = decl->_impl->element[i].streamIndex;
        pDXElement[i].Method = D3DDECLMETHOD_DEFAULT;
        pDXElement[i].Offset = Offset;
        atgVertexDecl::VertexAttribute attribute = decl->_impl->element[i].attribute;
        switch(attribute)
        {
        case atgVertexDecl::VA_Pos3:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_POSITION;
                pDXElement[i].UsageIndex = 0;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Pos4:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT4;
                pDXElement[i].Usage = D3DDECLUSAGE_POSITION;
                pDXElement[i].UsageIndex = 0;
                elementStride = 16;
                break;
            }
        case atgVertexDecl::VA_Normal:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_NORMAL;
                pDXElement[i].UsageIndex = 0;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Diffuse:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_COLOR;
                pDXElement[i].UsageIndex = 0;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Specular:
            {
                pDXElement[i].Type  = D3DDECLTYPE_D3DCOLOR;
                pDXElement[i].Usage = D3DDECLUSAGE_COLOR;
                pDXElement[i].UsageIndex = 1;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Texture0:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT2;
                pDXElement[i].Usage = D3DDECLUSAGE_TEXCOORD;
                pDXElement[i].UsageIndex = 0;
                elementStride = 8;
                break;
            }
        case atgVertexDecl::VA_Texture1:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_TEXCOORD;
                pDXElement[i].UsageIndex = 1;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Texture2:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_TEXCOORD;
                pDXElement[i].UsageIndex = 2;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Texture3:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_TEXCOORD;
                pDXElement[i].UsageIndex = 3;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Texture4:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_TEXCOORD;
                pDXElement[i].UsageIndex = 4;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Texture5:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_TEXCOORD;
                pDXElement[i].UsageIndex = 5;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Texture6:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_TEXCOORD;
                pDXElement[i].UsageIndex = 6;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_Texture7:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT3;
                pDXElement[i].Usage = D3DDECLUSAGE_TEXCOORD;
                pDXElement[i].UsageIndex = 7;
                elementStride = 12;
                break;
            }
        case atgVertexDecl::VA_PointSize:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT1;
                pDXElement[i].Usage = D3DDECLUSAGE_PSIZE;
                pDXElement[i].UsageIndex = 0;
                elementStride = 4;
                break;
            }
        case  atgVertexDecl::VA_BlendFactor4:
            {
                pDXElement[i].Type = D3DDECLTYPE_FLOAT4;
                pDXElement[i].Usage = D3DDECLUSAGE_BLENDWEIGHT;
                pDXElement[i].UsageIndex = 0;
                elementStride = 16;
                break;
            }
        default:
            AASSERT(0 && "error type of vertex declaration type. \n");
            return false;
        }
        Offset += elementStride;
    }
    pDXElement[numberVDE] = sElementEnd;
    if( FAILED(g_pd3dDevice->CreateVertexDeclaration(pDXElement, &_impl->pDXVD)) )
    {
        Destroy();
        return false;
    }
    
    DWORD d3dusage = D3DUSAGE_WRITEONLY;
    if( accessMode == BAM_Dynamic)
    {
        d3dusage = D3DUSAGE_DYNAMIC;
    }
    
    if( FAILED(g_pd3dDevice->CreateVertexBuffer(size, d3dusage, 0, D3DPOOL_DEFAULT, &_impl->pDXVB, NULL)) )
    {
        SAFE_DELETE(_impl);
        return false;
    }

    void *pLockData = Lock(0, size);
    if(pLockData)
    {
        memcpy(pLockData, pData, size);
        Unlock();
        return true;
    }
    return false;
}

bool atgVertexBuffer::Destroy()
{
    if (_impl)
    {
        SAFE_RELEASE(_impl->pDXVB);
        SAFE_RELEASE(_impl->pDXVD);
        SAFE_DELETE(_impl);
    }
    return true;
}

void* atgVertexBuffer::Lock( uint32 offset, uint32 size )
{
    void *pData =  NULL;
    if(_impl->locked)
    {
        return NULL;
    }
    
    if ( FAILED(_impl->pDXVB->Lock(offset, size, &pData, 0)) ) {
        return NULL;
    }
    _impl->locked = true;

    return pData;
}

void atgVertexBuffer::Unlock()
{
    if(!_impl->locked)
    {
        return;
    }

    DX_ASSERT( _impl->pDXVB->Unlock() );

    _impl->locked = false;
}

bool atgVertexBuffer::IsLocked() const
{
    return _impl->locked;
}

class atgTextureImpl
{
public:
    atgTextureImpl():pDXTX(NULL),locked(false) {}
    bool Bind(uint8 index);
    static bool Unbind(uint8 index);
public:
    IDirect3DTexture9* pDXTX;
    bool locked;
};

bool atgTextureImpl::Bind(uint8 index)
{
    if (index >= 8)
        return false;

    DX_ASSERT( g_pd3dDevice->SetTexture(index, pDXTX) );
    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR) );
    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR) );
    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_POINT) );
    return true;
}

bool atgTextureImpl::Unbind(uint8 index)
{
    DX_ASSERT( g_pd3dDevice->SetTexture(index, NULL) );
    return true;
}

atgTexture::atgTexture():_width(0),_height(0),_bbp(0),_impl(NULL)
{
}

atgTexture::~atgTexture()
{
}

atgTexture* atgTexture::Create( uint32 width, uint32 height, uint32 bbp, const void *pData/*=NULL*/ )
{
    atgTexture* pTexture = new atgTexture();
    pTexture->_width = width;
    pTexture->_height = height;
    pTexture->_bbp = bbp / 8;
    char* pBegin = (char*)pData;
    if (pTexture->_impl == NULL)
    {
        pTexture->_impl = new atgTextureImpl;
    }else
    {
        SAFE_RELEASE(pTexture->_impl->pDXTX);
    }
    atgTextureImpl* _impl = pTexture->_impl;

    if( FAILED( g_pd3dDevice->CreateTexture(width, height, 0, D3DUSAGE_AUTOGENMIPMAP, \
            D3DFMT_A8R8G8B8,
            D3DPOOL_MANAGED,
            &_impl->pDXTX,
            NULL)) )
    {
        LOG("create texture fail.\n");
        return NULL;
    }

    if (pData)
    {
        D3DLOCKED_RECT rect;
        if( FAILED( pTexture->_impl->pDXTX->LockRect(0, &rect, NULL, NULL) )  )
        {
            LOG("lock texture data fail.\n");
            return NULL;
        }
        if (pTexture->_bbp == 4)
        {
            //int pitch = width * pTexture->_bbp;
            //for (uint32 i= 0; i < height; ++i)
            //{
            //    memcpy( (uint8*)(rect.pBits) + rect.Pitch * i, pBegin + pitch * i, pitch);
            //}
            memcpy( (uint8*)(rect.pBits), pBegin, height * width * pTexture->_bbp);
        }
        else
        {
            return false;
            //// else 
            //// copy one by one, ca!
            //for (uint32 i=0; i < _height; ++i)
            //{
            //    for (uint32 j=0; j < _width; ++j)
            //    {
            //        uint8* dest = (uint8*)(rect.pBits) + rect.Pitch * i + j * 4;
            //        *((uint32*)dest) = uint8BGR_2_uint32(texture->getBuffer() + texture->getPitch() * i + texture->getBPP() * j);
            //    }
            //}
        }
        DX_ASSERT( _impl->pDXTX->UnlockRect(0) );

        DX_ASSERT( _impl->pDXTX->SetAutoGenFilterType(D3DTEXF_LINEAR) );
        _impl->pDXTX->GenerateMipSubLevels();
    }
    return pTexture;
}

bool atgTexture::Destory()
{
    if (_impl)
    {
        SAFE_RELEASE(_impl->pDXTX);
        SAFE_DELETE(_impl);
        _width = 0;
        _height = 0;
        _bbp = 0;
    }

    return true;
}

void* atgTexture::Lock()
{
    return NULL;
}

void atgTexture::Unlock()
{
}

bool atgTexture::IsLocked() const
{
    return _impl->locked;
}

//////////////////////////////////////////////////////////////////////////
atgResourceShader::atgResourceShader(ResourceShaderType Type)
{
    type = Type;
}

atgResourceShader::~atgResourceShader()
{
}


class atgVertexShaderImpl
{
public:
    atgVertexShaderImpl():pDXVS(NULL),pTable(NULL){}
public:
    IDirect3DVertexShader9* pDXVS;
    ID3DXConstantTable*     pTable;

    inline void             Bind();

    inline bool             SetFloat3(const char* var_name, const float f[3]);
    inline bool             SetFloat4(const char* var_name, const float f[4]);
    inline bool             SetMatrix4x4(const char* var_name, const float mat[4][4]);
};

inline void atgVertexShaderImpl::Bind()
{
    if (pDXVS)
    {
        DX_ASSERT( g_pd3dDevice->SetVertexShader(pDXVS) );
    }
}

inline bool atgVertexShaderImpl::SetFloat3(const char* var_name, const float f[3])
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetFloatArray(g_pd3dDevice, h, f, 3);
            return true;
        }
    }
    return false;
}

inline bool atgVertexShaderImpl::SetFloat4(const char* var_name, const float f[4])
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetFloatArray(g_pd3dDevice, h, f, 4);
            return true;
        }
    }
    return false;
}

inline bool atgVertexShaderImpl::SetMatrix4x4(const char* var_name, const float mat[4][4])
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetMatrix(g_pd3dDevice, h, (CONST D3DXMATRIX*)mat);
            return true;
        }
    }
    return false;
}

atgVertexShader::atgVertexShader():atgResourceShader(RST_Vertex),_impl(NULL)
{
}

atgVertexShader::~atgVertexShader()
{
    if (_impl)
    {
        SAFE_RELEASE(_impl->pDXVS);
        SAFE_RELEASE(_impl->pTable);
        delete _impl;
    }
}

bool atgVertexShader::LoadFromFile(const char* SourceFilePath)
{
    if (!SourceFilePath)
    {
        return false;
    }
    //if (MediaIsExist(Source))
    //    return false;
    atgReadFile reader;
    if(!reader.Open(SourceFilePath))
        return false;

    if(!_impl)
    {
        _impl = new atgVertexShaderImpl;
    }

    uint32 fileSize = reader.GetLength();
    if (fileSize <= 0)
        return false;
    char* VSbuffer = new char[fileSize];
    if(fileSize != reader.Read(VSbuffer, 1, fileSize))
    {
        SAFE_DELETE_ARRAY(VSbuffer);
        return false;
    }
    if ( FAILED(g_pd3dDevice->CreateVertexShader( (const DWORD*)(VSbuffer), &_impl->pDXVS)) )
    {
        LOG("create vertex shader fail form file %s\n", SourceFilePath);
        return false;
    }

    DX_ASSERT( D3DXGetShaderConstantTable((const DWORD*)(VSbuffer), &_impl->pTable) );

    SAFE_DELETE_ARRAY(VSbuffer);
    return Compile();
}

bool atgVertexShader::LoadFromSource( const char* Source )
{
    return false;
}

bool atgVertexShader::Compile()
{
    return true;
}

class atgFragmentShaderImpl
{
public:
    atgFragmentShaderImpl():pDXPS(NULL){}
public:
    IDirect3DPixelShader9* pDXPS;
    ID3DXConstantTable*     pTable;

    inline void             Bind();

    inline bool             SetFloat3(const char* var_name, const float f[3]);
    inline bool             SetFloat4(const char* var_name, const float f[4]);
    inline bool             SetMatrix4x4(const char* var_name, const float mat[4][4]);
};

inline void atgFragmentShaderImpl::Bind()
{
    if (pDXPS)
    {
        DX_ASSERT( g_pd3dDevice->SetPixelShader(pDXPS) );
    }
}

inline bool atgFragmentShaderImpl::SetFloat3(const char* var_name, const float f[3])
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetFloatArray(g_pd3dDevice, h, f, 3);
            return true;
        }
    }
    return false;
}

inline bool atgFragmentShaderImpl::SetFloat4(const char* var_name, const float f[4])
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetFloatArray(g_pd3dDevice, h, f, 4);
            return true;
        }
    }
    return false;
}

inline bool atgFragmentShaderImpl::SetMatrix4x4(const char* var_name, const float mat[4][4])
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetMatrix(g_pd3dDevice, h, (CONST D3DXMATRIX*)mat);
            return true;
        }
    }
    return false;
}

atgFragmentShader::atgFragmentShader():atgResourceShader(RST_Fragment),_impl(NULL)
{
}

atgFragmentShader::~atgFragmentShader()
{
    SAFE_RELEASE(_impl->pDXPS);
    SAFE_RELEASE(_impl->pTable);
    SAFE_DELETE(_impl);
}

bool atgFragmentShader::LoadFromFile(const char* SourceFilePath)
{
    if (!SourceFilePath)
    {
        return false;
    }
    //if (MediaIsExist(Source))
    //    return false;

    atgReadFile reader;
    if(!reader.Open(SourceFilePath))
        return false;
    if(!_impl)
    {
        _impl = new atgFragmentShaderImpl;
    }

    uint32 fileSize = reader.GetLength();
    if (fileSize <= 0)
        return false;
    char* PSbuffer = new char[fileSize];
    if(fileSize != reader.Read(PSbuffer, 1, fileSize))
    {
        SAFE_DELETE_ARRAY(PSbuffer);
        return false;
    }
    if ( FAILED(g_pd3dDevice->CreatePixelShader( (const DWORD*)(PSbuffer), &_impl->pDXPS)) )
    {
        LOG("create vertex shader fail form file %s\n", SourceFilePath);
        return false;
    }

    DX_ASSERT( D3DXGetShaderConstantTable((const DWORD*)(PSbuffer), &_impl->pTable) );

    SAFE_DELETE_ARRAY(PSbuffer);
    return Compile();
}

//
bool atgFragmentShader::LoadFormSource( const char* Source )
{
    return false;
}

bool atgFragmentShader::Compile()
{
    return true;
}

class atgPassImpl
{
public:
    atgPassImpl():pVS(NULL),pPS(NULL){}

    atgVertexShader* pVS;
    atgFragmentShader* pPS;
};

atgPass::atgPass()
{
    _impl = new atgPassImpl;
}

atgPass::~atgPass()
{
    if (_impl)
    {
        SAFE_DELETE(_impl->pVS);
        SAFE_DELETE(_impl->pPS);
        SAFE_DELETE(_impl);
    }
}

const char* atgPass::GetName()
{
    return _name.c_str();
}

bool atgPass::Create( const char* VSFilePath, const char* FSFilePath )
{
    if (!Load(VSFilePath, FSFilePath))
    {
        return false;
    }
    LOG("create pass(%s,%s) success.\n", VSFilePath, FSFilePath);
    return true;
}

void atgPass::Bind()
{
    if (_impl->pVS->_impl)
    {
        _impl->pVS->_impl->Bind();
    }
    if (_impl->pPS->_impl)
    {
        _impl->pPS->_impl->Bind();
    }
}

bool atgPass::Load(const char* VSFilePath, const char* FSFilePath)
{
    SAFE_DELETE(_impl->pVS);
    SAFE_DELETE(_impl->pPS);

    _impl->pVS = new atgVertexShader();
    if(!_impl->pVS->LoadFromFile(VSFilePath))
    {
        LOG("load vertex shader fail.\n");
        return false;
    }

    _impl->pPS = new atgPixelShader();
    if (!_impl->pPS->LoadFromFile(FSFilePath))
    {
        LOG("load pixel shader fail.\n");
        return false;
    }

    return true;
}

bool atgPass::Link()
{
    return true;
}

bool atgPass::SetFloat3(const char* var_name, const float f[3])
{
    bool rt = SetVsFloat3(var_name, f);
    if (!rt)
    {
        rt = SetPsFloat3(var_name, f);
    }

    if (!rt)
    {
        LOG("shader SetFloat3 name=[%s] is invliad.\n", var_name);
    }

    return rt;
}

bool atgPass::SetFloat4(const char* var_name, const float f[4])
{
    bool rt = SetVsFloat4(var_name, f);
    if (!rt)
    {
        rt = SetPsFloat4(var_name, f);
    }
    
    if (!rt)
    {
        LOG("shader SetFloat4 name=[%s] is invliad.\n", var_name);
    }
       
    return rt;
}

bool atgPass::SetMatrix4x4(const char* var_name, const float mat[4][4])
{
    // 需要矩阵转置
    static float matTemp[4][4]; 
    MatTranspose(mat, matTemp); 

    bool rt = SetVsMatrix4x4(var_name, matTemp);
    if (!rt)
    {
        rt = SetPsMatrix4x4(var_name, matTemp);
    }

    if (!rt)
    {
        LOG("shader SetMatrix4x4 name=[%s] is invliad.\n", var_name);
    }

    return rt;
}

bool atgPass::SetVsFloat3(const char* var_name, const float f[3])
{
    if (_impl->pVS->_impl)
    {
        return _impl->pVS->_impl->SetFloat3(var_name, f);
    }

    return false;
}
bool atgPass::SetVsFloat4(const char* var_name, const float f[4])
{
    if (_impl->pVS->_impl)
    {
        return _impl->pVS->_impl->SetFloat4(var_name, f);
    }

    return false;
}

bool atgPass::SetVsMatrix4x4(const char* var_name, const float mat[4][4])
{
    if (_impl->pVS->_impl)
    {
        return _impl->pVS->_impl->SetMatrix4x4(var_name, mat);
    }

    return false;
}

bool atgPass::SetPsFloat3(const char* var_name, const float f[3])
{
    if (_impl->pPS->_impl)
    {
        return _impl->pPS->_impl->SetFloat3(var_name, f);
    }

    return false;
}

bool atgPass::SetPsFloat4(const char* var_name, const float f[4])
{
    if (_impl->pPS->_impl)
    {
        return _impl->pPS->_impl->SetFloat4(var_name, f);
    }

    return false;
}

bool atgPass::SetPsMatrix4x4(const char* var_name, const float mat[4][4])
{
    if (_impl->pPS->_impl)
    {
        return _impl->pPS->_impl->SetMatrix4x4(var_name, mat);
    }

    return false;
}

bool atgPass::SetTexture(const char* var_name, uint8 index)
{
    return true;
}

void atgPass::BeginContext(void* data)
{
    Matrix WVP;
    Matrix Wrld;
    Matrix View;
    Matrix Proj;
    Matrix Temp;
    g_Renderer->GetMatrix(&Wrld.m, MD_WORLD);
    g_Renderer->GetMatrix(&View.m, MD_VIEW);
    g_Renderer->GetMatrix(&Proj.m, MD_PROJECTION);
    ConcatTransforms(View.m, Wrld.m, Temp.m);
    ConcatTransforms(Proj.m, Temp.m, WVP.m);
    SetMatrix4x4(UNF_M_WVP, WVP.m);
}

void atgPass::EndContext(void* data)
{

}

bool atgRenderer::Initialize( uint32 width, uint32 height, uint8 bpp )
{
    // Create DirectX 9 Device Interface.
    if(NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return FALSE;
    // Set Render Parameter
    g_d3dpp = new D3DPRESENT_PARAMETERS();
    ZeroMemory( g_d3dpp, sizeof(D3DPRESENT_PARAMETERS) );
    g_d3dpp->Windowed = TRUE;
    g_d3dpp->BackBufferWidth = width;
    g_d3dpp->BackBufferHeight = height;
    g_d3dpp->BackBufferCount = 1;
    g_d3dpp->hDeviceWindow = g_hWnd;
    g_d3dpp->SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp->EnableAutoDepthStencil = TRUE;
    g_d3dpp->AutoDepthStencilFormat = D3DFMT_D24S8;
    g_d3dpp->BackBufferFormat = D3DFMT_A8R8G8B8;
    g_d3dpp->MultiSampleType = D3DMULTISAMPLE_16_SAMPLES;

    while (g_d3dpp->MultiSampleType)
    {
        DWORD QualityLevels = 0;
        HRESULT Result = g_pD3D->CheckDeviceMultiSampleType(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_d3dpp->BackBufferFormat,
            g_d3dpp->Windowed, g_d3dpp->MultiSampleType, &QualityLevels);

        if (Result == D3D_OK)
        {
            g_d3dpp->MultiSampleQuality = QualityLevels - 1;
            break;
        }
        g_d3dpp->MultiSampleType = D3DMULTISAMPLE_TYPE(g_d3dpp->MultiSampleType - 1);
    }


    if( FAILED( g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        g_d3dpp, &g_pd3dDevice ) ) )
    {
        g_pD3D->Release();
        return false;
    }

    LOG("|||| Graphic Driver ===> Using %s!\n", atgRenderer::getName());
    return true;
}

void atgRenderer::Shutdown()
{
    if(g_pd3dDevice)
    {
        DX_ASSERT( g_pd3dDevice->Release() );
    }
    if(g_pD3D)
    {
        DX_ASSERT( g_pD3D->Release() );
    }
}

bool atgRenderer::Resize( uint32 width, uint32 height )
{
    if (g_d3dpp->BackBufferWidth == width && 
        g_d3dpp->BackBufferHeight == height)
        return false;

    g_d3dpp->BackBufferWidth  = width;
    g_d3dpp->BackBufferHeight = height;

    DX_ASSERT( g_pd3dDevice->Reset(g_d3dpp) );

    return true;
}

void atgRenderer::SetViewport(uint32 offsetX, uint32 offsetY, uint32 width, uint32 height)
{
    if (NULL == g_hWnd)
        return;

    D3DVIEWPORT9 viewport;
    viewport.X = offsetX;
    viewport.Y = offsetY;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinZ = -1.0f;
    viewport.MaxZ =  1.0f;
    DX_ASSERT( g_pd3dDevice->SetViewport(&viewport) );
}

void atgRenderer::GetViewPort( uint32& offsetX, uint32& offsetY, uint32& width, uint32& height ) const
{
    D3DVIEWPORT9 viewport;
    DX_ASSERT( g_pd3dDevice->GetViewport(&viewport) );
    offsetX = viewport.X;
    offsetY = viewport.Y;
    width   = viewport.Width;
    height  = viewport.Height;
}

void atgRenderer::SetMatrix( MatrixDefine index, const float mat[4][4] )
{
    switch(index)
    {
    case MD_WORLD:
        {
            memcpy(&_matrixs[MD_WORLD], mat, sizeof(float[4][4]));
            DX_ASSERT( g_pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)(&_matrixs[MD_WORLD])) );
            break;
        }
    case MD_VIEW:
        {
            memcpy(&_matrixs[MD_VIEW], mat, sizeof(float[4][4]));
            DX_ASSERT( g_pd3dDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)(&_matrixs[MD_VIEW])) );
            break;
        }
    case MD_PROJECTION:
        {
            memcpy(&_matrixs[MD_PROJECTION], mat, sizeof(float[4][4]));
            DX_ASSERT( g_pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)(&_matrixs[MD_PROJECTION])) );
            break;
        }
    default: break;
    }
}

void atgRenderer::GetMatrix(const float (*MtxPtr)[4][4], MatrixDefine index) const
{
    if (MD_WORLD <= index && index < MD_NUMBER)
    {
        memcpy((void*)MtxPtr, _matrixs[index].m, MATRIX44_SIZE);
    }
}

void atgRenderer::SetDepthTestEnable(bool enable)
{
    if (enable)
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE) );
    }
    else
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE) );
}

void atgRenderer::SetAlphaTestEnable(bool enbale, float value)
{
    if (enbale)
    {
        DWORD refValue = ((DWORD)(value * 255.0f)) & 0xFF;
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE) );
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER) );
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, refValue) );
    }
    else
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE) );
}

void atgRenderer::SetFaceCull(FaceCullMode mode)
{
    if (mode == FCM_NONE)
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE) );
    }else if(mode == FCM_CW)
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW) );
    }else
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW) );
    }
}

DWORD BlendFuncToDirectXBlendFunc(BlendFunction BlendFunc)
{
    switch(BlendFunc)
    {
    case BF_ZERO : return D3DBLEND_ZERO;
    case BF_ONE : return D3DBLEND_ONE;
    case BF_SRC_COLOR : return D3DBLEND_SRCCOLOR;
    case BF_ONE_MINUS_SRC_COLOR : return D3DBLEND_INVSRCCOLOR;
    case BF_DST_COLOR : return D3DBLEND_DESTCOLOR;
    case BF_ONE_MINUS_DST_COLOR : return D3DBLEND_INVDESTCOLOR;
    case BF_SRC_ALPHA : return D3DBLEND_SRCALPHA;
    case BF_ONE_MINUS_SRC_ALPHA : return D3DBLEND_INVSRCALPHA;
    case BF_DST_ALPHA : return D3DBLEND_DESTALPHA;
    case BF_ONE_MINUS_DST_ALPHA : return D3DBLEND_INVDESTALPHA;
    default:
        AASSERT(0);
        return 0;
    }
}

BlendFunction DirectXBlendFuncToEngineBlendFunc(DWORD BlendFunc)
{
    switch(BlendFunc)
    {
    case D3DBLEND_ZERO : return BF_ZERO;
    case D3DBLEND_ONE : return BF_ONE;
    case D3DBLEND_SRCCOLOR : return BF_SRC_COLOR;
    case D3DBLEND_INVSRCCOLOR : return BF_ONE_MINUS_SRC_COLOR;
    case D3DBLEND_DESTCOLOR : return BF_DST_COLOR;
    case D3DBLEND_INVDESTCOLOR : return BF_ONE_MINUS_DST_COLOR;
    case D3DBLEND_SRCALPHA : return BF_SRC_ALPHA;
    case D3DBLEND_INVSRCALPHA : return BF_ONE_MINUS_SRC_ALPHA;
    case D3DBLEND_DESTALPHA : return BF_DST_ALPHA;
    case D3DBLEND_INVDESTALPHA : return BF_ONE_MINUS_DST_ALPHA;
    default:
        AASSERT(0);
        return (BlendFunction)0;
    }
}

void atgRenderer::GetBlendFunction(BlendFunction& outSrcBlend, BlendFunction& outDestBlend)
{
    DWORD dxSrc = 0;
    DWORD dxDest = 0;
    DX_ASSERT( g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &dxSrc) );
    DX_ASSERT( g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &dxDest) );
    outSrcBlend = DirectXBlendFuncToEngineBlendFunc(dxSrc);
    outDestBlend = DirectXBlendFuncToEngineBlendFunc(dxDest);
}

void atgRenderer::SetBlendFunction(BlendFunction SrcBlend, BlendFunction DestBlend)
{
    if (SrcBlend == BF_NONE || DestBlend == BF_NONE)
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) );
    }
    else
    {
        DWORD dxSrc = BlendFuncToDirectXBlendFunc(SrcBlend);
        DWORD dxDest = BlendFuncToDirectXBlendFunc(DestBlend);
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) );
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, dxSrc) );
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, dxDest) );
    }
}

void atgRenderer::Clear()
{
    ATG_PROFILE("atgRenderer::Clear");
    DX_ASSERT( g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0) );
}

void atgRenderer::BeginFrame()
{
    // Begin the scene
    DX_ASSERT( g_pd3dDevice->BeginScene() );

    // 设置常量寄存
    //g_pd3dDevice->SetPixelShaderConstantI();
    //g_pd3dDevice->SetPixelShaderConstantB();
    //Matrix mvp;
    //Matrix temp;
    //ConcatTransforms(mvp.m, matView.m, matProjection.m);
    //ConcatTransforms(temp.m, matWorld.m ,mvp.m);

    //ConcatTransforms(mvp.m, matProjection.m, matView.m);
    //ConcatTransforms(temp.m, mvp.m ,matWorld.m);
    // note: direct shader using column-major matrix so we transpose matrix. 
    //MatTransposed(mvp.m, temp.m);

    //float ambient[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    //g_pd3dDevice->SetPixelShaderConstantF(0, (const float*)ambient, 1);
    //static float delta = 0.0f;
    //delta += 0.03f;
    //float intensity[4] = { abs(sin(delta)), 0.0f, 0.0f, 1.0f };
    //g_pd3dDevice->SetPixelShaderConstantF(1, (const float*)intensity, 1);

    //static float i = 0;
    //i += 0.01f;
    //if(i > MATH_PI)
    //    i = -MATH_PI;

    //float light[4] = { sinf(i), 0.0f, cosf(i), 1.0f };
    //DX_ASSERT( g_pd3dDevice->SetPixelShaderConstantF(0, (const float*)light, 1) );
    ////static float delta = 0.0f;
    ////delta += 0.03f;
    //float intensity[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    //DX_ASSERT( g_pd3dDevice->SetPixelShaderConstantF(1, (const float*)intensity, 1) );
}

void atgRenderer::EndFrame()
{
    // End the scene
    DX_ASSERT( g_pd3dDevice->EndScene() );

    for (int i = 0; i < MaxNumberBindTexture; ++i)
        BindTexture(i, NULL);

    BindVertexBuffer(NULL);
    BindIndexBuffer(NULL);
    BindPass(NULL);
    BindMaterial(NULL);
}

void atgRenderer::Present()
{
    ATG_PROFILE("atgRenderer::Present");
    DX_ASSERT( g_pd3dDevice->Present( NULL, NULL, NULL, NULL ) );
}

bool PrimitiveTypeConvertToDX(PrimitveType pt, D3DPRIMITIVETYPE& dx_pt)
{
    switch(pt)
    {
    case PT_POINTS:
        dx_pt = D3DPT_POINTLIST;
        break;
    case PT_LINES:
        dx_pt = D3DPT_LINELIST;
        break;
    case PT_LINE_STRIP:
        dx_pt = D3DPT_LINESTRIP;
        break;
    case PT_TRIANGLES:
        dx_pt = D3DPT_TRIANGLELIST;
        break;
    case PT_TRIANGLE_STRIP:
        dx_pt = D3DPT_TRIANGLESTRIP;
        break;
    case PT_TRIANGLE_FAN:
        dx_pt = D3DPT_TRIANGLEFAN;
        break;
    default:
        return false;
    }
    return true;
}

bool atgRenderer::DrawPrimitive( PrimitveType pt, uint32 primitveCount, uint32 verticesCount )
{
    D3DPRIMITIVETYPE dx_pt = D3DPT_TRIANGLELIST;
    if(PrimitiveTypeConvertToDX(pt, dx_pt))
    {
        DX_ASSERT( g_pd3dDevice->DrawPrimitive(dx_pt, 0, primitveCount) );
        return true;
    }

    return false;
}

bool atgRenderer::DrawIndexedPrimitive( PrimitveType pt, uint32 primitveCount, uint32 indicesCount, uint32 verticesCount )
{
    D3DPRIMITIVETYPE dx_pt = D3DPT_TRIANGLELIST;
    if(PrimitiveTypeConvertToDX(pt, dx_pt))
    {
        DX_ASSERT( g_pd3dDevice->DrawIndexedPrimitive(dx_pt, 0, 0, verticesCount, 0, primitveCount) );
        return true;
    }
    
    return false;
}

void atgRenderer::BindPass( atgPass* pass )
{
    if (_bindPass)
    {
        _bindPass->EndContext(NULL);
    }

    _bindPass = pass;

    if(_bindPass)
    {
        _bindPass->Bind();
        _bindPass->BeginContext(NULL);
    }else
    {
        DX_ASSERT( g_pd3dDevice->SetVertexShader(NULL) );
        DX_ASSERT( g_pd3dDevice->SetPixelShader(NULL) );
    }
}

void atgRenderer::BindIndexBuffer( atgIndexBuffer* indexBuffer )
{
    _bindIndexBuffer = indexBuffer;

    if (_bindIndexBuffer)
    {
        DX_ASSERT( _bindIndexBuffer->_impl->Bind() );
    }
}

void atgRenderer::BindVertexBuffer( atgVertexBuffer* vertexBuffer )
{
    if(!_bindPass)
        return;

    if (vertexBuffer)
    {
        DX_ASSERT( vertexBuffer->_impl->Bind() );
    }

    return;
}

void atgRenderer::BindTexture( uint8 index, atgTexture* texture )
{
    if (0 <= index && index < MaxNumberBindTexture)
    {
        _bindTexture[index] = texture;
        if (texture)
        {
            DX_ASSERT( texture->_impl->Bind(index) );
        }else
        {
            atgTextureImpl::Unbind(index);
        }
    }
}

#endif
