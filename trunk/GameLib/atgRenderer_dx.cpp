#include "atgBase.h"
#include "atgRenderer.h"
#include "atgMisc.h"
#include "atgGame.h"
#include "atgProfile.h"

#ifdef USE_DIRECTX


#define D3D_DEBUG_INFO

//////////////////////////////////////////////////////////////////////////
#include <d3d9.h>
#include <d3dx9.h>

IDirect3D9*             g_pD3D = NULL;              // D3D9 InterFace
IDirect3DDevice9*       g_pd3dDevice = NULL;        // D3D9 Devices InterFace
D3DPRESENT_PARAMETERS*  g_d3dpp = NULL; 

#define SAFE_RELEASE(x) { if(x) { x->Release(); x = NULL; } }


/* >>>>> directx 9.0 的渲染状态初值表

Render States               Default Value
-----------------------------------------
D3DRS_ZENABLE               D3DZB_FALSE 
D3DRS_SPECULARENABLE        FALSE 
D3DFILLMODE                 D3DFILL_SOLID 
D3DSHADEMODE                D3DSHADE_GOURAUD 
D3DRS_ZWRITEENABLE          TRUE 
D3DRS_ALPHATESTENABLE       FALSE 
D3DRS_LASTPIXEL             TRUE 
D3DRS_SRCBLEND              D3DBLEND_ONE 
D3DRS_DESTBLEND             D3DBLEND_ZERO 
D3DRS_ZFUNC                 D3DCMP_LESSEQUAL 
D3DRS_ALPHAREF              0 
D3DRS_ALPHAFUNC             D3DCMP_ALWAYS 
D3DRS_DITHERENABLE          FALSE 
D3DRS_FOGSTART              0 
D3DRS_FOGEND                1 
D3DRS_FOGDENSITY            1 
D3DRS_ALPHABLENDENABLE      FALSE 
D3DRS_DEPTHBIAS             0 
D3DRS_STENCILENABLE         FALSE 
D3DRS_STENCILFAIL           D3DSTENCILOP_KEEP 
D3DRS_STENCILZFAIL          D3DSTENCILOP_KEEP 
D3DRS_STENCILPASS           D3DSTENCILOP_KEEP 
D3DRS_STENCILFUNC           D3DCMP_ALWAYS 
D3DRS_STENCILREF            0 
D3DRS_STENCILMASK           0xffffffff 
D3DRS_STENCILWRITEMASK      0xffffffff 
D3DRS_TEXTUREFACTOR         0xffffffff 
D3DRS_WRAP0                 0 
D3DRS_WRAP1                 0 
D3DRS_WRAP2                 0 
D3DRS_WRAP3                 0 
D3DRS_WRAP4                 0 
D3DRS_WRAP5                 0 
D3DRS_WRAP6                 0 
D3DRS_WRAP7                 0 
D3DRS_WRAP8                 0 
D3DRS_WRAP9                 0 
D3DRS_WRAP10                0 
D3DRS_WRAP11                0 
D3DRS_WRAP12                0 
D3DRS_WRAP13                0 
D3DRS_WRAP14                0 
D3DRS_WRAP15                0 
D3DRS_LOCALVIEWER           TRUE 
D3DRS_EMISSIVEMATERIALSOURCE D3DMCS_MATERIAL 
D3DRS_AMBIENTMATERIALSOURCE D3DMCS_MATERIAL 
D3DRS_DIFFUSEMATERIALSOURCE D3DMCS_COLOR1 
D3DRS_SPECULARMATERIALSOURCE D3DMCS_COLOR2 
D3DRS_COLORWRITEENABLE      0x0000000f 
D3DBLENDOP                  D3DBLENDOP_ADD 
D3DRS_SCISSORTESTENABLE     FALSE 
D3DRS_SLOPESCALEDEPTHBIAS   0 
D3DRS_ANTIALIASEDLINEENABLE FALSE 
D3DRS_TWOSIDEDSTENCILMODE   FALSE 
D3DRS_CCW_STENCILFAIL       D3DSTENCILOP_KEEP 
D3DRS_CCW_STENCILZFAIL      D3DSTENCILOP_KEEP 
D3DRS_CCW_STENCILPASS       D3DSTENCILOP_KEEP 
D3DRS_CCW_STENCILFUNC       D3DCMP_ALWAYS 
D3DRS_COLORWRITEENABLE1     0x0000000f 
D3DRS_COLORWRITEENABLE2     0x0000000f 
D3DRS_COLORWRITEENABLE3     0x0000000f 
D3DRS_BLENDFACTOR           0xffffffff 
D3DRS_SRGBWRITEENABLE       0 
D3DRS_SEPARATEALPHABLENDENABLE FALSE 
D3DRS_SRCBLENDALPHA         D3DBLEND_ONE 
D3DRS_DESTBLENDALPHA        D3DBLEND_ZERO 
D3DRS_BLENDOPALPHA          D3DBLENDOP_ADD

*/

//////////////////////////////////////////////////////////////////////////

// index buffer
class atgIndexBufferImpl
{
public:
    atgIndexBufferImpl():pDXIB(0),accessMode(BAM_Static),locked(0){}
    ~atgIndexBufferImpl() { SAFE_RELEASE(pDXIB); }
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
    Destory();
    g_Renderer->RemoveGpuResource(this);
}

bool atgIndexBuffer::Create( const void *pIndices, uint32 numIndices, IndexFormat format, BufferAccessMode accessMode )
{
    Destory();

    _impl = new atgIndexBufferImpl;
    int indexSize = sizeof(uint16);
    D3DFORMAT d3dFormat = D3DFMT_INDEX16;
    if(format == IFB_Index32)
    {
        indexSize = sizeof(uint32);
        d3dFormat = D3DFMT_INDEX32;
    }
    DWORD d3dUsage = D3DUSAGE_WRITEONLY;
    D3DPOOL d3dPool = D3DPOOL_DEFAULT;
    _impl->accessMode = accessMode;
    if(accessMode == BAM_Dynamic)
    {
        d3dUsage = d3dUsage | D3DUSAGE_DYNAMIC;
    }
    uint32 buffSize = numIndices * indexSize;
    _size = buffSize;
    if( FAILED(g_pd3dDevice->CreateIndexBuffer( buffSize, 
        d3dUsage, d3dFormat, d3dPool, &_impl->pDXIB, NULL)))
    {
        return false;
    }

    atgGpuResource::ReSet();
    g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgIndexBuffer::Destory));

    if (pIndices)
    {
        // upload data
        void* pLockedBuffer = 0;
        pLockedBuffer = Lock(0, buffSize);
        if(pLockedBuffer)
        {
            memcpy(pLockedBuffer, pIndices, buffSize);
            Unlock();
        }
        else
        {
            LOG("lock index buffer data fail.\n");
            return false;
        }
    }

    return true;
}

bool atgIndexBuffer::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);

    return true;
}

void* atgIndexBuffer::Lock( uint32 offset, uint32 size )
{
    AASSERT(_impl);
    void* pIndexBuffer = NULL;
    if (!IsLost() && _impl && size < _size)
    {
        if( FAILED(_impl->pDXIB->Lock(offset,
                                      size,
                                      (void**)&pIndexBuffer,
                                      (_impl->accessMode == BAM_Dynamic ? D3DLOCK_DISCARD : 0))))
        {
            return NULL;
        }

        _impl->locked = true;
    }
    
    return pIndexBuffer;
}

void atgIndexBuffer::Unlock()
{
    AASSERT(_impl);
    if (!IsLost() && _impl && _impl->locked)
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
    ~atgVertexBufferImpl(){ SAFE_RELEASE(pDXVB); SAFE_RELEASE(pDXVD); }
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

atgVertexBuffer::atgVertexBuffer():_impl(0)
{
    _size = 0;
}

atgVertexBuffer::~atgVertexBuffer()
{
    Destory();
    g_Renderer->RemoveGpuResource(this);
}

bool atgVertexBuffer::Create( atgVertexDecl* decl, const void *pData, uint32 size, BufferAccessMode accessMode )
{
    Destory();

    _impl = new atgVertexBufferImpl;
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
        return false;
    }
    SAFE_DELETE_ARRAY(pDXElement);
    
    DWORD d3dusage = D3DUSAGE_WRITEONLY;
    D3DPOOL d3dPool = D3DPOOL_DEFAULT;
    if( accessMode == BAM_Dynamic)
    {
        d3dusage = D3DUSAGE_DYNAMIC;
    }
    
    if( FAILED(g_pd3dDevice->CreateVertexBuffer(size, d3dusage, 0, d3dPool, &_impl->pDXVB, NULL)) )
    {
        SAFE_DELETE(_impl);
        return false;
    }

    atgGpuResource::ReSet();
    g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgVertexBuffer::Destory));

    if (pData)
    {
        void *pLockData = Lock(0, size);
        if(pLockData)
        {
            memcpy(pLockData, pData, size);
            Unlock();
        }
        else
        {
            LOG("lock vertex buffer data fail.\n");
            return false;
        }
    }

    return true;
}

bool atgVertexBuffer::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);

    return true;
}

void* atgVertexBuffer::Lock( uint32 offset, uint32 size )
{
    AASSERT(_impl);
    void *pData =  NULL;
    if(_impl->locked || IsLost())
    {
        return NULL;
    }
    
    if ( FAILED(_impl->pDXVB->Lock(offset, 
                                   size, 
                                   &pData, 
                                   _impl->accessMode == BAM_Dynamic ? D3DLOCK_DISCARD : 0)) )
    {
        return NULL;
    }

    _impl->locked = true;

    return pData;
}

void atgVertexBuffer::Unlock()
{
    if(!_impl->locked || IsLost())
    {
        return;
    }

    DX_ASSERT( _impl->pDXVB->Unlock() );

    _impl->locked = false;
}

bool atgVertexBuffer::IsLocked() const
{
    if(_impl)
        return _impl->locked;

    return false;
}

class atgTextureImpl
{
public:
    atgTextureImpl():pDXTX(NULL),locked(false),accessMode(BAM_Static) {}
    ~atgTextureImpl() { locked = false; SAFE_RELEASE(pDXTX); }
    bool Bind(uint8 index);
    static bool Unbind(uint8 index);
public:
    IDirect3DTexture9* pDXTX;
    bool locked;
    BufferAccessMode accessMode;
};

bool atgTextureImpl::Bind(uint8 index)
{
    if (index >= 8)
        return false;

    DX_ASSERT( g_pd3dDevice->SetTexture(index, pDXTX) );
    return true;
}

bool atgTextureImpl::Unbind(uint8 index)
{
    DX_ASSERT( g_pd3dDevice->SetTexture(index, NULL) );
    return true;
}

atgTexture::atgTexture():_width(0),_height(0),_format(TF_R8G8B8A8),_impl(NULL)
{
    _filter = TFM_FILTER_DEFAULT;
    for (int i = 0; i < MAX_COORDS; ++i)
    {
        _address[i] = MAX_ADDRESSMODES;
    }
}

atgTexture::~atgTexture()
{
    Destory();
    g_Renderer->RemoveGpuResource(this);
}

bool atgTexture::Create( uint32 width, uint32 height, TextureFormat format, const void *pData/*=NULL*/ )
{
    Destory();

    _width = width;
    _height = height;
    _format = format;
    _impl = new atgTextureImpl;

    uint8 pixelSize = 0; 
    bool isDyncmic = false;
    bool isFloatData = false;
    bool isDepthStencil = false;
    D3DFORMAT _inputFormat = D3DFMT_UNKNOWN;
    switch (_format)
    {
    case TF_R8G8B8:
        _inputFormat = D3DFMT_R8G8B8; //directx don't support 24 bit format.
        _inputFormat = D3DFMT_X8R8G8B8;
        pixelSize = 3;
        break;
    case TF_R5G6B5:
        _inputFormat = D3DFMT_R5G6B5;
        pixelSize = 2;
        break;
    case TF_R8G8B8A8:
        _inputFormat = D3DFMT_A8R8G8B8;
        pixelSize = 4;
        break;
    case TF_R5G5B5A1:
        _inputFormat = D3DFMT_A1R5G5B5;
        pixelSize = 2;
        break;
    case TF_R4G4B4A4:
        _inputFormat = D3DFMT_A4R4G4B4;
        pixelSize = 2;
        break;
    case TF_R32F:
        _inputFormat = D3DFMT_R32F;
        isDyncmic = true;
        isFloatData = true;
        pixelSize = 4;
        break;
    case TF_R16F:
        _inputFormat = D3DFMT_R16F;
        isDyncmic = true;
        isFloatData = true;
        pixelSize = 4;
        break;
    case TF_D24S8:
        _inputFormat = D3DFMT_D24S8;
        isDyncmic = true;
        isFloatData = true;
        isDepthStencil = true;
        break;
    case TF_D16:
        _inputFormat = D3DFMT_D16;
        isDyncmic = true;
        isFloatData = true;
        isDepthStencil = true;
        break;
    default:
        break;
    }

    DWORD usage = 0;
    D3DPOOL pool = D3DPOOL_MANAGED;

    //>静态纹理
    //usage = 0;
    //pool = D3DPOOL_MANAGED;

    //>如果是动态纹理 
    //usage |= D3DUSAGE_DYNAMIC;
    //pool = D3DPOOL_DEFAULT;

    //>如果是renderTarget
    //usage |= D3DUSAGE_RENDERTARGET; 或者 usage_ |= D3DUSAGE_DEPTHSTENCIL;
    //pool = D3DPOOL_DEFAULT;

    if (isDyncmic)
    {
        usage |= D3DUSAGE_DYNAMIC;
        _impl->accessMode = BAM_Dynamic;
        pool = D3DPOOL_DEFAULT;
    }
    
    if(!isFloatData)
    {
        usage |= D3DUSAGE_AUTOGENMIPMAP;
    }

    if( FAILED( g_pd3dDevice->CreateTexture(width, height,
                                            0, 
                                            usage,
                                            _inputFormat,
                                            pool,
                                            &_impl->pDXTX,
                                            NULL)) )
    {
        LOG("create texture fail.\n");
        return false;
    }

    atgGpuResource::ReSet();

    if (pool != D3DPOOL_MANAGED)
    {
        g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgTexture::Destory));
    }

    if (!isDepthStencil && pData)
    {
        LockData lockData;

        lockData = Lock();
        if (lockData.pAddr)
        {
            if (_inputFormat == D3DFMT_X8R8G8B8 && pixelSize == 3)
            {
                uint8 *pPtr = (uint8 *)pData;
                for (uint32 i=0; i < _height; ++i)
                {
                    uint8* dest = (uint8*)(lockData.pAddr) + lockData.pitch * i;
                    for (uint32 j = 0; j < _width; ++j)
                    {
                        *((uint32*)dest) = D3DCOLOR_XRGB(*pPtr,*(pPtr+1),*(pPtr+2));
                        pPtr += 3;
                        dest += 4;
                    }
                }
            }
            else
            {
                if (lockData.pitch == width * pixelSize)
                {
                    memcpy( (uint8*)(lockData.pAddr), pData, height * width * pixelSize);
                }
                else
                {
                    char* pSrc = (char*)pData;
                    char* pDst = (char*)lockData.pAddr;
                    int lineSize = width * pixelSize;
                    for (uint32 i = 0; i < height; ++i)
                    {
                        memcpy(pDst, pSrc, lineSize);
                        pDst += lockData.pitch;
                        pSrc += lineSize;
                    }
                }
            }

            Unlock();
        }
        else
        {
            LOG("lock texture data fail.\n");
            return false;
        }
    }

    if (!isFloatData)
    {
        DX_ASSERT( _impl->pDXTX->SetAutoGenFilterType(D3DTEXF_LINEAR) );
        _impl->pDXTX->GenerateMipSubLevels();
    }

    return true;
}

bool atgTexture::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);
    _width = 0;
    _height = 0;
    _format = TF_R8G8B8A8;

    return true;
}

atgTexture::LockData atgTexture::Lock()
{
    AASSERT(_impl);
    LockData lockData;
    lockData.pAddr = NULL;
    lockData.pitch = 0;

    if(_impl->locked || IsLost())
    {
        return lockData;
    }

    D3DLOCKED_RECT rect;
    if( FAILED( _impl->pDXTX->LockRect(0, 
                                       &rect, 
                                       NULL, 
                                       _impl->accessMode == BAM_Dynamic ? D3DLOCK_DISCARD : 0) )  )
    {
        return lockData;
    }

    _impl->locked = true;
    lockData.pAddr = rect.pBits;
    lockData.pitch = rect.Pitch;

    return lockData;
}

void atgTexture::Unlock()
{
    if(!_impl->locked || IsLost())
    {
        return;
    }

    DX_ASSERT( _impl->pDXTX->UnlockRect(0) );

    _impl->locked = false;
}

bool atgTexture::IsLocked() const
{
    return _impl->locked;
}

void atgTexture::SetFilterMode(TextureFilterMode filter)
{
    _filter = filter;
}

void atgTexture::SetAddressMode(TextureCoordinate coordinate, TextureAddressMode address)
{
    _address[coordinate] = address;
}

//////////////////////////////////////////////////////////////////////////
atgResourceShader::atgResourceShader(ResourceShaderType Type):compiled(false)
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
    ~atgVertexShaderImpl() { SAFE_RELEASE(pDXVS); SAFE_RELEASE(pTable); }
public:
    IDirect3DVertexShader9* pDXVS;
    ID3DXConstantTable*     pTable;

    inline void             Bind();

    inline bool             SetInt(const char* var_name, int value);
    inline bool             SetFloat(const char* var_name, float value);
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

inline bool atgVertexShaderImpl::SetInt(const char* var_name, int value)
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetInt(g_pd3dDevice, h, value);
            return true;
        }
    }
    return false;
}

inline bool atgVertexShaderImpl::SetFloat(const char* var_name, float value)
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetFloat(g_pd3dDevice, h, value);
            return true;
        }
    }
    return false;
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
    Destory();
}

bool atgVertexShader::LoadFromFile(const char* SourceFilePath)
{
    if(IsCompiled())
    {
        LOG("vs already loaded.");
        return false;
    }

    if (!SourceFilePath)
    {
        return false;
    }

    atgReadFile reader;
    if(!reader.Open(SourceFilePath))
        return false;

    uint32 fileSize = reader.GetLength();
    if (fileSize <= 0)
        return false;
    char* VSbuffer = new char[fileSize];
    if(fileSize != reader.Read(VSbuffer, 1, fileSize))
    {
        SAFE_DELETE_ARRAY(VSbuffer);
        return false;
    }

    bool rs = LoadFromSource(VSbuffer);
    if (!rs)
    {
        LOG("create vertex shader fail form file[%s]\n", SourceFilePath);
    }

    SAFE_DELETE_ARRAY(VSbuffer);
    return rs;
}

bool atgVertexShader::LoadFromSource( const char* Source )
{
    Destory();

    if(IsCompiled())
    {
        LOG("vs already loaded.");
        return false;
    }

    if(!_impl)
    {
        _impl = new atgVertexShaderImpl;
    }

    if ( FAILED(g_pd3dDevice->CreateVertexShader( (const DWORD*)(Source), &_impl->pDXVS)) )
    {
        LOG("create vertex shader fail form Source\n");
        return false;
    }

    DX_ASSERT( D3DXGetShaderConstantTable((const DWORD*)(Source), &_impl->pTable) );

    bool rs = Compile();

    if (rs)
    {
        atgGpuResource::ReSet();
        //g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgVertexShader::Destory));
    }

    return rs;
}

bool atgVertexShader::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);
        
    return true;
}

bool atgVertexShader::Compile()
{
    compiled = true;
    return compiled;
}

class atgFragmentShaderImpl
{
public:
    atgFragmentShaderImpl():pDXPS(NULL){}
    ~atgFragmentShaderImpl() { SAFE_RELEASE(pDXPS); }
public:
    IDirect3DPixelShader9* pDXPS;
    ID3DXConstantTable*     pTable;

    inline void             Bind();

    inline bool             SetInt(const char* var_name, int value);
    inline bool             SetFloat(const char* var_name, float value);
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

bool atgFragmentShaderImpl::SetInt(const char* var_name, int value)
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetInt(g_pd3dDevice, h, value);
            return true;
        }
    }
    return false;
}

bool atgFragmentShaderImpl::SetFloat(const char* var_name, float value)
{
    if (pTable)
    {
        D3DXHANDLE h = pTable->GetConstantByName(0, var_name);
        if (h)
        {
            pTable->SetFloat(g_pd3dDevice, h, value);
            return true;
        }
    }
    return false;
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
    Destory();
}

bool atgFragmentShader::LoadFromFile(const char* SourceFilePath)
{
    if(IsCompiled())
    {
        LOG("fs already loaded.");
        return false;
    }

    if (!SourceFilePath)
    {
        return false;
    }

    atgReadFile reader;
    if(!reader.Open(SourceFilePath))
        return false;

    uint32 fileSize = reader.GetLength();
    if (fileSize <= 0)
        return false;
    char* PSbuffer = new char[fileSize];
    if(fileSize != reader.Read(PSbuffer, 1, fileSize))
    {
        SAFE_DELETE_ARRAY(PSbuffer);
        return false;
    }

    bool rs = LoadFormSource(PSbuffer);
    if (!rs){
        LOG("create fragment shader fail form file[%s]\n", SourceFilePath);
    }

    SAFE_DELETE_ARRAY(PSbuffer);
    return rs;
}

bool atgFragmentShader::LoadFormSource( const char* Source )
{
    Destory();

    if(!_impl)
    {
        _impl = new atgFragmentShaderImpl;
    }

    if ( FAILED(g_pd3dDevice->CreatePixelShader( (const DWORD*)(Source), &_impl->pDXPS)) )
    {
        LOG("create fragment shader fail form Source \n");
        return false;
    }

    DX_ASSERT( D3DXGetShaderConstantTable((const DWORD*)(Source), &_impl->pTable) );

    bool rs = Compile();

    if (rs)
    {
        atgGpuResource::ReSet();
        //g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgFragmentShader::Destory));
    }

    return rs;
}

bool atgFragmentShader::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);

    return true;
}

bool atgFragmentShader::Compile()
{
    compiled = true;
    return compiled;
}

class atgPassImpl
{
public:
    atgPassImpl():pVS(0),pPS(0){}
    ~atgPassImpl() { SAFE_DELETE(pVS); SAFE_DELETE(pPS); }

    atgVertexShader* pVS;
    atgFragmentShader* pPS;
};

atgPass::atgPass():_impl(0)
{

}

atgPass::~atgPass()
{
    Destory();
    g_Renderer->RemoveGpuResource(this);
}

const char* atgPass::GetName()
{
    return _name.c_str();
}

bool atgPass::Create( const char* VSFilePath, const char* FSFilePath )
{
    Destory();

    _impl = new atgPassImpl;
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

    atgGpuResource::ReSet();
    g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgPass::Destory));
    LOG("create pass(%s,%s) success.\n", VSFilePath, FSFilePath);
    return true;
}

bool  atgPass::CreateFromSource(const char* pVSsrc, const char* pFSsrc)
{
    Destory();

    _impl->pVS = new atgVertexShader();
    if(!_impl->pVS->LoadFromSource(pVSsrc))
    {
        LOG("load vertex shader fail.\n");
        return false;
    }

    _impl->pPS = new atgPixelShader();
    if (!_impl->pPS->LoadFromFile(pFSsrc))
    {
        LOG("load pixel shader fail.\n");
        return false;
    }

    atgGpuResource::ReSet();
    LOG("create pass by source success.\n");
    return true;
}

bool atgPass::Destory()
{
    atgGpuResource::Lost();
    SAFE_DELETE(_impl);
    
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

bool atgPass::Link()
{
    return true;
}

bool atgPass::SetInt(const char* var_name, int value)
{
    bool rt = SetVsInt(var_name, value);
    if (!rt)
    {
        rt = SetPsInt(var_name, value);
    }

    if (!rt)
    {
        LOG("shader SetFloat3 name=[%s] is invliad.\n", var_name);
    }

    return rt;
}

bool atgPass::SetFloat(const char* var_name, float value)
{
    bool rt = SetVsFloat(var_name, value);
    if (!rt)
    {
        rt = SetPsFloat(var_name, value);
    }

    if (!rt)
    {
        LOG("shader SetFloat3 name=[%s] is invliad.\n", var_name);
    }

    return rt;
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

bool atgPass::SetMatrix4x4(const char* var_name, const Matrix& mat)
{
    // 需要矩阵转置
    static float matTemp[4][4]; 
    atgMath::MatTranspose(mat.m, matTemp); 

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

bool atgPass::SetVsInt(const char* var_name, int value)
{
    if (_impl->pVS->_impl)
    {
        return _impl->pVS->_impl->SetInt(var_name, value);
    }

    return false;
}

bool atgPass::SetVsFloat(const char* var_name, float value)
{
    if (_impl->pVS->_impl)
    {
        return _impl->pVS->_impl->SetFloat(var_name, value);
    }

    return false;
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

bool atgPass::SetPsInt(const char* var_name, int value)
{
    if (_impl->pPS->_impl)
    {
        return _impl->pPS->_impl->SetInt(var_name, value);
    }

    return false;
}

bool atgPass::SetPsFloat(const char* var_name, float value)
{
    if (_impl->pPS->_impl)
    {
        return _impl->pPS->_impl->SetFloat(var_name, value);
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
    g_Renderer->GetMatrix(Wrld, MD_WORLD);
    g_Renderer->GetMatrix(View, MD_VIEW);
    g_Renderer->GetMatrix(Proj, MD_PROJECTION);
    View.Concatenate(Wrld, WVP);
    Proj.Concatenate(WVP, WVP);
    SetMatrix4x4(UNF_M_WVP, WVP);
}

void atgPass::EndContext(void* data)
{

}

class atgRenderTargetImpl
{
public:
    atgRenderTargetImpl():pRenderTexture(0),pDepthStencilSurface(0),pOldRT(0),pOldDS(0) {}
    ~atgRenderTargetImpl() { SAFE_RELEASE(pRenderTexture); SAFE_RELEASE(pDepthStencilSurface); SAFE_RELEASE(pOldRT); SAFE_RELEASE(pOldDS); }

    bool Create(uint16 width, uint16 height, D3DFORMAT d3dFormat)
    {
        //> level为1, 渲染到纹理不需要做minmap采样. 
        DX_ASSERT( g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, d3dFormat, D3DPOOL_DEFAULT, &pRenderTexture, NULL) );
        //> 必然需要一块深度缓存来满足绘制图元所需要的基本需求.
        DX_ASSERT( g_pd3dDevice->CreateDepthStencilSurface(width, height, g_d3dpp->AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &pDepthStencilSurface, NULL) );

        return true;
    }

    void Bind(uint8 index_)
    {
        index = index_;
        IDirect3DSurface9* pSurface;
        DX_ASSERT( pRenderTexture->GetSurfaceLevel(0, &pSurface) );

        IDirect3DSurface9* pOldRT = NULL;
        if( SUCCEEDED( g_pd3dDevice->GetRenderTarget(index, &pOldRT)) )
        {
            DX_ASSERT( g_pd3dDevice->SetRenderTarget(index, pSurface) );

            IDirect3DSurface9* pOldDS = NULL;
            if ( SUCCEEDED(g_pd3dDevice->GetDepthStencilSurface(&pOldDS)) )
            {
                DX_ASSERT( g_pd3dDevice->SetDepthStencilSurface(pDepthStencilSurface) );
            }
        }

        SAFE_RELEASE( pSurface );
    }

    void Unbind()
    {
        if (pOldDS != NULL)
        {
            g_pd3dDevice->SetDepthStencilSurface( pOldDS );
            SAFE_RELEASE( pOldDS );
        }

        if (pOldRT != NULL)
        {
            g_pd3dDevice->SetRenderTarget(index, pOldRT);
            SAFE_RELEASE( pOldRT );
        }
    }

    IDirect3DTexture9* pRenderTexture;
    IDirect3DSurface9* pDepthStencilSurface;
    IDirect3DSurface9* pOldRT;
    IDirect3DSurface9* pOldDS;

    uint8  index;
};


atgRenderTarget::atgRenderTarget():_impl(NULL)
{

}

atgRenderTarget::~atgRenderTarget()
{
    Destroy();
}

bool atgRenderTarget::Create(std::vector<atgTexture*>& colorBuffer, atgTexture* depthStencilBuffer)
{
    Destroy();
    
    bool rs = false;
    //_impl = new atgRenderTargetImpl();
    //if (_impl)
    //{
    //    D3DFORMAT d3dFormat = D3DFMT_UNKNOWN;
    //    switch (format)
    //    {
    //    case RBF_A8R8G8B8:
    //        d3dFormat = D3DFMT_A8R8G8B8;
    //        break;
    //    case RBF_R32F:
    //        d3dFormat = D3DFMT_R32F;
    //        break;
    //    default:
    //        break;
    //    }

    //    if (d3dFormat != D3DFMT_UNKNOWN)
    //    {
    //        rs = _impl->Create(width, height, d3dFormat);
    //    }
    //}

    if (rs)
    {
        atgGpuResource::ReSet();
        g_Renderer->InsertGpuResource(this, static_cast<GpuResDestoryFunc>(&atgRenderTarget::Destroy));
    }

    return rs;
}

bool atgRenderTarget::Destroy()
{
    atgGpuResource::Lost();
    SAFE_DELETE( _impl );
   
    return true;
}

bool atgRenderTarget::Active(uint8 index)
{
    if (_impl)
    {
        _impl->Bind(index);
    }
    return false;
}
void atgRenderTarget::Deactive()
{
    if (_impl)
    {
        _impl->Unbind();
    }
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
    ReleaseAllGpuResource();

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

void atgRenderer::SetMatrix(MatrixDefine index, const Matrix& mat)
{
    switch(index)
    {
    case MD_WORLD:
        {
            _matrixs[MD_WORLD] = mat;
            DX_ASSERT( g_pd3dDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)(&_matrixs[MD_WORLD])) );
            break;
        }
    case MD_VIEW:
        {
            _matrixs[MD_VIEW] = mat;
            DX_ASSERT( g_pd3dDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)(&_matrixs[MD_VIEW])) );
            break;
        }
    case MD_PROJECTION:
        {
            _matrixs[MD_PROJECTION] = mat;
            DX_ASSERT( g_pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)(&_matrixs[MD_PROJECTION])) );
            break;
        }
    default: break;
    }
}

void atgRenderer::GetMatrix(Matrix& mat, MatrixDefine index) const
{
    if (MD_WORLD <= index && index < MD_NUMBER)
    {
        mat = _matrixs[index];
    }
}

void atgRenderer::SetLightEnable(bool enable)
{
    if (enable)
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE) );
    }
    else
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE) );
    }
}

void atgRenderer::SetDepthTestEnable(bool enable)
{
    if (enable)
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE) );
    }
    else
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE) );
    }
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
    {
        DX_ASSERT( g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE) );
    }
}

void atgRenderer::SetFaceCull(FaceCullMode mode)
{
    //> d3d9 reset 的时候会自动把cullmode变成D3DCULL_CCW
    //> 所以建议不要改变FaceCull,默认使用FCM_CCW,即逆时针顶点组成的面看不见;
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
    DX_ASSERT( g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,36,36), 1.0f, 0) );
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


static bool isLost(HRESULT _hr)
{
    return D3DERR_DEVICELOST == _hr
        || D3DERR_DRIVERINTERNALERROR == _hr
#if !defined(D3D_DISABLE_9EX)
        || D3DERR_DEVICEHUNG == _hr
        || D3DERR_DEVICEREMOVED == _hr
#endif // !defined(D3D_DISABLE_9EX)
        ;
}

void atgRenderer::Present()
{
    ATG_PROFILE("atgRenderer::Present");

    HRESULT hr = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    if (isLost(hr) )
    {
        LOG("%s device is lost.\n", getName());
        do
        {
            do
            {
                ::Sleep(100);
                hr = g_pd3dDevice->TestCooperativeLevel();
            }
            while (D3DERR_DEVICENOTRESET != hr);

            do
            {
                ReleaseAllGpuResource();

                hr = g_pd3dDevice->Reset(g_d3dpp);
            } while (FAILED(hr) );

            hr = g_pd3dDevice->TestCooperativeLevel();
        }
        while (FAILED(hr) );
    }
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

bool atgRenderer::DrawPrimitive( PrimitveType pt, uint32 primitveCount, uint32 verticesCount, uint32 offset )
{
    D3DPRIMITIVETYPE dx_pt = D3DPT_TRIANGLELIST;
    if(PrimitiveTypeConvertToDX(pt, dx_pt))
    {
        DX_ASSERT( g_pd3dDevice->DrawPrimitive(dx_pt, offset, primitveCount) );
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
            switch (texture->_filter)
            {
            case TFM_FILTER_NEAREST:
                {
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_POINT) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_POINT) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_POINT) );
                }break;
            case TFM_FILTER_BILINEAR:
                {
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_POINT) );
                }break;
            case TFM_FILTER_TRILINEAR:
                {
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_PYRAMIDALQUAD) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_PYRAMIDALQUAD) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR) );
                }break;
            case TFM_FILTER_ANISOTROPIC:
                {
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR) );
                }break;
            case TFM_FILTER_NOT_MIPMAP_ONLY_LINEAR:
                {
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR) );
                    DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR) );
                }break;
            case TFM_FILTER_DEFAULT:
                {
                    //>什么也不做
                    break;
                }
            default:
                break;
            }

            if (texture->_address[TC_COORD_U] != MAX_ADDRESSMODES)
            {
                switch (texture->_address[TC_COORD_U])
                {
                case TAM_ADDRESS_WRAP:
                    {
                        DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP) );
                    }break;
                case TAM_ADDRESS_MIRROR:
                    {
                        DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR) );
                    }break;
                case TAM_ADDRESS_CLAMP:
                    {
                        DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP) );
                    }break;
                case TAM_ADDRESS_BORDER:
                    {
                        DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER) );
                    }break;
                default:
                    break;
                }
            }

            if (texture->_address[TC_COORD_V] != MAX_ADDRESSMODES)
            {
                switch (texture->_address[TC_COORD_V])
                {
                case TAM_ADDRESS_WRAP:
                    {
                        DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP) );
                    }break;
                case TAM_ADDRESS_MIRROR:
                    {
                        DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR) );
                    }break;
                case TAM_ADDRESS_CLAMP:
                    {
                        DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP) );
                    }break;
                case TAM_ADDRESS_BORDER:
                    {
                        DX_ASSERT( g_pd3dDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER) );
                    }break;
                default:
                    break;
                }
            }

        }else
        {
            atgTextureImpl::Unbind(index);
        }
    }
}

void atgRenderer::SetPointSize(float size)
{
    float pointSize = size;
    g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pointSize));
}

#endif
