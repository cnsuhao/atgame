#pragma once

#include "atgBase.h"
#include "atgMath.h"
#include <stack>

//#define USE_OPENGL
//#define USE_OPENGLES

//#ifndef USE_OPENGL
//    #define USE_DIRECTX
//#endif

#if defined(USE_OPENGL)
    #ifdef _WIN32
        #ifndef USE_OPENGLES
            #include <gl/glew.h>
            #define OPENGL_USE_MAP
        #else
            #include "EGL/egl.h"
            #include "GLES2/gl2.h"
            #include "GLES2/gl2ext.h"
            #define glClearDepth glClearDepthf
        #endif // USE_OPENGLES
    #elif _ANDROID
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
        #define glClearDepth glClearDepthf
    #endif
#endif

#if defined(_DEBUG) && defined(USE_OPENGL)
#define GL_ASSERT(expression) do \
    { \
    expression; \
    GLenum error = glGetError(); \
    if(error != GL_NO_ERROR) \
        { \
        LOG("call opengl's function has error. file(%s),func(%s),line(%d),error(%x)\n", __FILE__, __FUNCTION__, __LINE__, error); \
        AASSERT(0); \
        } \
    }while(0)
// if(error != GL_NO_ERROR) LOG("func(%s),line(%d),error(%x)\n", __FUNCTION__, __LINE__, error);
#else
#define GL_ASSERT(expression) expression
#endif

#if defined(_DEBUG) && defined(USE_DIRECTX)
#define DX_ASSERT(expression) do \
    { \
    HRESULT hr = expression; \
    if(FAILED(hr)) \
        { \
        LOG("call directx's function has error(=0x%x)!\n", hr); \
        AASSERT(0); \
        } \
    }while(0)
#else
#define DX_ASSERT(expression) expression
#endif

class atgRenderer;

class atgGpuResource
{
public:

    atgGpuResource():_isLost(true) { }

    virtual ~atgGpuResource() { }

    bool        IsLost() const { return _isLost; }

    void        Lost();

    void        ReSet();

    virtual const char* GetTypeName() const = 0;
private:
    bool _isLost;
};

typedef bool (atgGpuResource::*GpuResDestoryFunc)(void);

enum BufferAccessMode
{
    BAM_Static,
    BAM_Dynamic
};

class atgIndexBuffer : public atgGpuResource
{
    friend class atgRenderer;
public:
    //>统一使用16bit了.(opengles 2.0支持 byte or short)
    enum IndexFormat
    {
        IFB_Index16,
        IFB_Index32
    };
public:
    atgIndexBuffer();
    ~atgIndexBuffer();

    bool                    Create(uint16* pIndices, uint32 numIndices, 
                                   IndexFormat format, BufferAccessMode accessMode);
    bool                    Destory();

    inline uint32           GetSize() { return _size; }
    void*                   Lock(uint32 offset, uint32 size);
    void                    Unlock();
    bool                    IsLocked() const;

    bool                    Bind();
    void                    Unbind();

    const char*             GetTypeName() const { return "atgIndexBuffer"; }
private:
    uint32 _size;
    class atgIndexBufferImpl* _impl;
};

class atgVertexDecl
{
    friend class atgRenderer;
    friend class atgVertexBuffer;
public:
    enum DataType
    {
        VDT_Int,
        VDT_Int2,
        VDT_Int3,
        VDT_Int4,
        VDT_Float,
        VDT_Float2,
        VDT_Float3,
        VDT_Float4
    };
    enum VertexAttribute
    {
        VA_None = 0,                    ///< Not specified.
        VA_Pos3 = ( 1 << 1 ),           ///< Position in carthesian coordinates.
        VA_Pos4 = ( 1 << 2 ),           ///< Position in homogenious coordinates
        VA_Normal = ( 1 << 3 ),         ///< Normal vector.
        VA_Tangent = ( 1 << 4 ),        ///< Tangent vector. 4 components.
        VA_Diffuse = ( 1 << 5 ),        ///< Diffuse color, 3 components.
        VA_Specular = ( 1 << 6 ),       ///< Specular color, 3 components.
        VA_Texture0 = ( 1 << 7 ),       ///< Texture coordinates for texture unit 0.
        VA_Texture1 = ( 1 << 8 ),       ///< Texture coordinates for texture unit 1.
        VA_Texture2 = ( 1 << 9 ),       ///< Texture coordinates for texture unit 2.
        VA_Texture3 = ( 1 << 10 ),       ///< Texture coordinates for texture unit 3.
        VA_Texture4 = ( 1 << 11 ),      ///< Texture coordinates for texture unit 4.
        VA_Texture5 = ( 1 << 12 ),      ///< Texture coordinates for texture unit 5.
        VA_Texture6 = ( 1 << 13 ),      ///< Texture coordinates for texture unit 6.
        VA_Texture7 = ( 1 << 14 ),      ///< Texture coordinates for texture unit 7.
        VA_PointSize = ( 1 << 15 ),     ///< Point size.
        VA_BlendFactor4 = ( 1 << 16 ),  ///< Blend factors for vertex blending
        VA_Max                          ///< Upper limit.
    };
    static const uint32 MaxNumberElement = 16;

    atgVertexDecl();
    atgVertexDecl(const atgVertexDecl& other);
    ~atgVertexDecl();

    uint8                   GetNumberElement() const;
    uint8                   GetElementsStride() const; 
    bool                    AppendElement(uint32 streamIndex, VertexAttribute attribute);
    VertexAttribute         GetIndexElement(uint32 streamIndex, uint32 index) const;

    atgVertexDecl& operator= (const atgVertexDecl& other);
private:
    class atgVertexDeclImpl* _impl;
};

class atgVertexBuffer : public atgGpuResource
{
    friend class atgRenderer;
public:
    atgVertexBuffer();
    ~atgVertexBuffer();

    bool                    Create(atgVertexDecl* decl, const void* pData, 
                                   uint32 size, BufferAccessMode accessMode);
    bool                    Destory();

    inline uint32           GetSize() { return _size; }
    void*                   Lock( uint32 offset, uint32 size );
    void                    Unlock();
    bool                    IsLocked() const;

    bool                    Bind();
    void                    Unbind();

    const char*             GetTypeName() const { return "atgVertexBuffer"; }
private:

    uint32 _size;
    class atgVertexBufferImpl* _impl;
};


/// Texture filtering mode.
enum TextureFilterMode
{
    TFM_FILTER_NEAREST = 0,
    TFM_FILTER_BILINEAR,
    TFM_FILTER_TRILINEAR,
    TFM_FILTER_ANISOTROPIC,
    TFM_FILTER_NOT_MIPMAP_ONLY_LINEAR,  //>适用于不产生mipmap的float纹理.
    TFM_FILTER_DEFAULT,
    MAX_FILTERMODES
};

/// Texture addressing mode.
enum TextureAddressMode
{
    TAM_ADDRESS_WRAP = 0,
    TAM_ADDRESS_MIRROR,
    TAM_ADDRESS_CLAMP,
    TAM_ADDRESS_BORDER,
    MAX_ADDRESSMODES
};


/// Texture coordinates.
enum TextureCoordinate
{
    TC_COORD_U = 0,
    TC_COORD_V,
    TC_COORD_W,
    MAX_COORDS
};

enum TextureFormat
{
    TF_R8G8B8,          //>color
    TF_R5G6B5,          //>color
    TF_R8G8B8A8,        //>color (虽然这里使用的数据的顺序是RGBA,但是在Directx中必须提供ARGB顺序的数据)
    TF_R5G5B5A1,        //>color (虽然这里使用的数据的顺序是RGBA,但是在Directx中必须提供ARGB顺序的数据)
    TF_R4G4B4A4,        //>color (虽然这里使用的数据的顺序是RGBA,但是在Directx中必须提供ARGB顺序的数据)
    TF_R32F,            //>color
    TF_R16F,            //>color
    TF_D24S8,           //>depth/stencil
    TF_D16,             //>depth/stencil
};

class atgTexture : public atgGpuResource
{
    friend class atgRenderer;
    friend class atgRenderTarget;
public:
    struct LockData
    {
        void* pAddr;
        uint32 pitch; // if value == 0 pitch是无效的.
    };

    atgTexture();
    ~atgTexture();

    bool                    Create(uint32 width, uint32 height, TextureFormat inputFormat, const void *pData=NULL, bool useToRenderTarget = false);
    bool                    Destory();

    LockData                Lock();
    void                    Unlock();
    bool                    IsLocked() const;

    uint32                  GetWidth()  const { return _width; }
    uint32                  GetHeight() const { return _height; }
    TextureFormat           GetTextureFormat() const { return _format; }

    void                    SetFilterMode(TextureFilterMode filter);
    TextureFilterMode       GetFilterMode() const { return _filter; }
    void                    SetAddressMode(TextureCoordinate coordinate, TextureAddressMode address);
    TextureAddressMode      GetAddressMode(TextureCoordinate coordinate) const { return _address[coordinate]; }

    bool                    Bind(uint8 index);
    void                    Unbind(uint8 index);

    const char*             GetTypeName() const { return "atgTexture"; }
private:
    uint32 _width;
    uint32 _height;
    TextureFormat _format;
    TextureFilterMode _filter;
    TextureAddressMode _address[MAX_COORDS];

    class atgTextureImpl* _impl;
};

enum ResourceShaderType
{
    RST_Vertex,
    RST_Fragment
};

class atgResourceShader : public atgGpuResource
{
public:
    atgResourceShader(ResourceShaderType Type);
    virtual ~atgResourceShader();

    inline ResourceShaderType GetType() { return type; }
    bool                    IsCompiled() { return compiled; }

    const char*             GetTypeName() const { return "atgResourceShader"; }
private:
    ResourceShaderType type;
protected:
    bool compiled;
};

class atgVertexShader : public atgResourceShader
{
    friend class atgPass;
public:
    atgVertexShader();
    ~atgVertexShader();

    bool                    LoadFromFile(const char* SourceFilePath);
    bool                    LoadFromSource(const char* Source);
    bool                    Destory();

    const char*             GetTypeName() const { return "atgVertexShader"; }
protected:
    bool                    Compile();
private:
    class atgVertexShaderImpl* _impl;
};

class atgFragmentShader : public atgResourceShader
{
    friend class atgPass;
public:
    atgFragmentShader();
    ~atgFragmentShader();

    bool                    LoadFromFile(const char* SourceFilePath);
    bool                    LoadFormSource(const char* Source);
    bool                    Destory();

    const char*             GetTypeName() const { return "atgFragmentShader"; }
protected:
    bool                    Compile();
private:
    class atgFragmentShaderImpl* _impl;
};
typedef atgFragmentShader atgPixelShader;

class atgPass : public atgGpuResource
{
    friend class atgRenderer;
    friend class atgMaterial;
public:
    // uniform name define
#define UNF_M_W              "mat_world"
#define UNF_M_WV             "mat_world_view"
#define UNF_M_WVP            "mat_world_view_projection"

#define UNF_M_WI             "mat_world_inverse"
#define UNF_M_WVI            "mat_world_view_inverse"
#define UNF_M_WVPI           "mat_world_view_projection_inverse"

#define UNF_M_WIT            "mat_world_inverse_transpose"
#define UNF_M_WVIT           "mat_world_view_inverse_transpose"
#define UNF_M_WVPIT          "mat_world_view_projection_inverse_transpose"

#define UNF_M_V              "mat_view"
#define UNF_M_VP             "mat_view_projection"

#define UNF_M_VI             "mat_view_inverse"
#define UNF_M_VPI            "mat_view_projection_inverse"

#define UNF_M_VIT            "mat_view_inverse_transpose"
#define UNF_M_VPIT           "mat_view_projection_inverse_transpose"

#define UNF_M_P              "mat_projction"
#define UNF_M_PI             "mat_projection_inverse"
#define UNF_M_PIT            "mat_projection_inverse_transpose"

#define UNF_V_VP             "vec_viewer_position"
#define UNF_V_VD             "vec_viewer_direction"
#define UNF_V_VU             "vec_viewer_up"

#define UNF_V_GAC            "vec_global_ambient_color"

#define OGL_V_VP             "vs_vertexPosition"
#define OGL_V_VN             "vs_vertexNormal"
#define OGL_V_VT             "vs_vertexTangent"
#define OGL_V_VDC            "vs_vertexDiffuse"
#define OGL_V_VSC            "vs_vertexSpecular"
#define OGL_V_VT0            "vs_textureCoord"
#define OGL_V_VT1            "vs_textureCoord1"

#define SAMPLER_DIFFUSE      "sampler_diffuse"
#define SAMPLER_NORMAL       "sampler_normal"
#define SAMPLER_BUMPMAP      "sampler_bumpmap"
#define SAMPLER_LIGHTMAP     "sampler_lightmap"
#define SAMPLER_SHADOWMAP    "sampler_shadowmap"
#define SAMPLER_CUSTOM1      "sampler_custom1"
#define SAMPLER_CUSTOM2      "sampler_custom2"
#define SAMPLER_CUSTOM3      "sampler_custom3"

public:
    atgPass();
    virtual ~atgPass();
    // need has vertex shader and fragment shader
    bool                    Create(const char* pVSFilePath, const char* pFSFilePath);
    bool                    CreateFromSource(const char* pVSsrc, const char* pFSsrc);
    bool                    Destory();

    const char*             GetName();
    void                    SetName(const char* name) { _name = name; }

    bool                    SetInt(const char* var_name, int value);
    bool                    SetFloat(const char* var_name, float value);
    bool                    SetFloat2(const char* var_name, const atgVec2& f);
    bool                    SetFloat3(const char* var_name, const atgVec3& f);
    bool                    SetFloat4(const char* var_name, const atgVec4& f);
    bool                    SetMatrix4x4(const char* var_name, const atgMatrix& mat);

    bool                    SetTexture(const char* var_name, uint8 index);
    uint8                   GetTexture(const char* var_name);
#ifdef USE_DIRECTX

    bool                    SetVsInt(const char* var_name, int value);
    bool                    SetVsFloat(const char* var_name, float value);
    bool                    SetVsFloat2(const char* var_name, const atgVec2& f);
    bool                    SetVsFloat3(const char* var_name, const atgVec3& f);
    bool                    SetVsFloat4(const char* var_name, const atgVec4& f);
    bool                    SetVsMatrix4x4(const char* var_name, const atgMatrix& mat);

    bool                    SetPsInt(const char* var_name, int value);
    bool                    SetPsFloat(const char* var_name, float value);
    bool                    SetPsFloat2(const char* var_name, const atgVec2& f);
    bool                    SetPsFloat3(const char* var_name, const atgVec3& f);
    bool                    SetPsFloat4(const char* var_name, const atgVec4& f);
    bool                    SetPsMatrix4x4(const char* var_name, const atgMatrix& mat);
    bool                    SetPsTexture(const char* var_name, uint8 index);
    uint8                   GetPsTexture(const char* var_name);
#endif // USE_DIRECTX

    bool                    Bind();
    void                    Unbind();

    const char*             GetTypeName() const { return "atgPass"; }
protected:

    virtual void            BeginContext(void* data);
    virtual void            EndContext(void* data);
    bool                    Link();
private:
    std::string _name;
    class atgPassImpl* _impl;
};
/*
class atgTechnique
{
    atgTechnique();
    ~atgTechnique();

    const char*             GetName();

    atgPass*                GetPassByName(const char* pass_name);
    uint8                   GetNumOfPass();
private:
    std::string     _name;
    class atgTechniqueImpl* _impl;
};

class atgEffect
{
    atgEffect();
    ~atgEffect();

    const char*             GetName();
    bool                    LoadFromFile(const char* file_name);   // Direct / OpenGL

    atgTechnique*           GetTechniqueByName(const char* pass_name);
    uint8                   GetNumOfTechnique();

    // set uniform
    bool                    SetFloat3(const char* var_name, float f[3]);
    bool                    SetFloat4(const char* var_name, float f[4]);
    bool                    SetMat4x4(const char* var_name, float mat[4][4]);

    // set render states

    // set texture sampler states


    // set render targets

};
*/

//enum RenderTargetType
//{
//    RTT_Color,
//    RTT_Depth,
//    RTT_Stencil
//};

class atgRenderTarget : public atgGpuResource
{
public:
    atgRenderTarget();
    ~atgRenderTarget();

    bool                    Create(std::vector<atgTexture*>& colorBuffer, atgTexture* depthStencilBuffer);
    bool                    Destory();


    bool                    Bind(uint8 index);
    void                    Unbind();

    const std::vector<atgTexture*>& GetColorBuffer() const { return _colorBuffer; }
    atgTexture*             GetDepthStencilBuffer() const { return _depthStencilBuffer; }

    const char*             GetTypeName() const { return "atgRenderTarget"; }
private:
    class atgRenderTargetImpl* _impl;

    std::vector<atgTexture*> _colorBuffer;
    atgTexture*              _depthStencilBuffer;
};


enum MatrixDefine
{
    MD_WORLD,
    MD_VIEW,
    MD_PROJECTION,
    MD_NUMBER
};

enum PrimitveType
{
    PT_POINTS,
    PT_LINES,
    PT_LINE_STRIP,
    PT_TRIANGLES,
    PT_TRIANGLE_STRIP,
    PT_TRIANGLE_FAN,
};

enum FaceCullMode
{
    FCM_NONE,
    FCM_CCW,
    FCM_CW,
};

enum BlendFunction
{
    BF_NONE,
    BF_ZERO,
    BF_ONE,
    BF_SRC_COLOR,
    BF_ONE_MINUS_SRC_COLOR,
    BF_DST_COLOR,
    BF_ONE_MINUS_DST_COLOR,
    BF_SRC_ALPHA,
    BF_ONE_MINUS_SRC_ALPHA,
    BF_DST_ALPHA,
    BF_ONE_MINUS_DST_ALPHA,
};

enum ClearTarget
{
    CT_COLOR = 1,
    CT_DEPTH = 2,
    CT_STENCIL = 4,
    CT_COLOR_AND_DEPTH = CT_COLOR | CT_DEPTH,
    CT_COLOR_AND_DEPTHSTENCIL = CT_COLOR_AND_DEPTH | CT_STENCIL,
};

extern const char* StringOpenGLIdentity;
extern const char* StringGLESIdentity;
extern const char* StringDirectxIdentity;

class atgGame;
class atgMaterial;
class atgLight;
class atgCamera;

typedef std::vector<atgLight*> bindLights;

class atgRenderer
{
public:
    static const uint8 MaxNumberBindTexture = 8;

    atgRenderer(atgGame* game = NULL);
    ~atgRenderer(void);

    const char*             getName();
    bool                    Initialize(uint32 width, uint32 height, uint8 bpp);
    void                    Shutdown();
    void                    SetVSyncState(bool sync);
    bool                    Resize(uint32 width, uint32 height);

    void                    SetViewPort(uint32 offsetX, uint32 offsetY, uint32 width, uint32 height);
    void                    GetViewPort(uint32& offsetX, uint32& offsetY, uint32& width, uint32& height) const;

    void                    SetMatrix(MatrixDefine index, const atgMatrix& mat);
    void                    GetMatrix(atgMatrix& mat, MatrixDefine index) const;

    void                    SetLightEnable(bool enable);
    void                    SetDepthTestEnable(bool enable);
    void                    SetAlphaTestEnable(bool enbale, float value = 1.0f);
    void                    SetFaceCull(FaceCullMode mode);
    void                    SetScissorEnable(bool enable, int x = 0, int y = 0, int w = 0, int h = 0);
    void                    GetBlendFunction(BlendFunction& outSrcBlend, BlendFunction& outDestBlend);
    void                    SetBlendFunction(BlendFunction SrcBlend, BlendFunction DestBlend);

    void                    Clear(ClearTarget target = CT_COLOR_AND_DEPTH);

    void                    BeginFrame();
    void                    EndFrame();
    void                    Present();

    void                    PushRenderTarget(uint8 index, atgRenderTarget* pRenderTarget);
    void                    PopRenderTarget(uint8 index);

    void                    AddBindLight(atgLight* light);
    void                    AddBindLights(const bindLights& lights);
    void                    RemoveBindLight(atgLight* light);
    void                    ClearBindLight();
    inline const bindLights& GetBindLights() const;

    inline void             SetGlobalAmbientColor(const atgVec3& color);
    inline const atgVec3&   GetGlobalAmbientColor() const;

    void                    SetPointSize(float size);

    // 渲染调用
    bool                    DrawPrimitive(PrimitveType pt, uint32 primitveCount, uint32 verticesCount, uint32 offset = 0);
    bool                    DrawIndexedPrimitive(PrimitveType pt, uint32 primitveCount, uint32 indicesCount, uint32 verticesCount);

    //> draw point mobile phone 不是支持得很好.所以没啥用
    bool                    BeginPoint();
    void                    AddPoint(const atgVec3& center, const atgVec3& color);
    void                    EndPoint();

    bool                    BeginLine();
    void                    AddLine(const atgVec3& point1, const atgVec3& point2, const atgVec3& color);
    void                    EndLine();
    
    //                        1  2
    //                        3  4
    bool                    BeginQuad();
    void                    AddQuad(const atgVec3& p1, const atgVec3& p2, const atgVec3& p3, const atgVec3& p4, const atgVec3& color);
    void                    EndQuad();

    //> 绘制实四边形
    bool                    BeginFullQuad();
    void                    AddFullQuad(const atgVec3& p1, const atgVec3& p2, const atgVec3& p3, const atgVec3& p4, const atgVec3& color);
    void                    EndFullQuad(const char* pPassIdentity = NULL);

    //> 绘制纹理多边形
    bool                    DrawTexureQuad(const atgVec3& p1, const atgVec3& p2, const atgVec3& p3, const atgVec3& p4, const atgVec2& t1, const atgVec2& t2, const atgVec2& t3, const atgVec2& t4, atgTexture* pTexture);
    bool                    DrawFullScreenQuad(atgTexture* pTexture, bool uvConvert = false);
    bool                    DrawQuadByPass(const atgVec3& p1, const atgVec3& p2, const atgVec3& p3, const atgVec3& p4, const atgVec2& t1, const atgVec2& t2, const atgVec2& t3, const atgVec2& t4, const char* pPassIdentity);
    
    //> 轴对其盒子
    bool                    BeginAABBoxLine();
    void                    AddAABBoxLine(const atgVec3& vMin, const atgVec3& vMax, const atgVec3& color);
    void                    EndAABBoxLine();

    //>圆

private:
    void                    _CommonInitialize();

protected:
    atgGame*                _game;
    atgMatrix               _matrixs[MD_NUMBER];

    atgVec3                 _globalAmbientColor;

    bindLights              _bindLights;

    // Viewport
    uint32 _VP_offsetX;
    uint32 _VP_offsetY;
    uint32 _VP_width;
    uint32 _VP_height;

    FaceCullMode            _cullMode;

public:
    atgPass*                FindCachePass(const char* Key);
    atgTexture*             FindCacheTexture(const char* Key);

    void                    InsertCachePass(const char* Key, atgPass* Pass);
    void                    InsertCacheTexture(const char* Key, atgTexture* Texture);

    void                    FreeCachePass();
    void                    FreeCacheTextures();

    void                    InsertGpuResource(atgGpuResource* pRes, GpuResDestoryFunc);
    void                    RemoveGpuResource(atgGpuResource* pRes);
    void                    ReleaseAllGpuResource();

private:
    typedef std::map<std::string, atgPass* > atgPassSet;
    atgPassSet  _cachePass;

    typedef std::map<std::string, atgTexture* > atgTextureSet;
    atgTextureSet _cacheTextures;

    typedef std::stack<atgRenderTarget* > atgRenderTargetStack;
    atgRenderTargetStack _renderTargetStack;

    typedef std::map<atgGpuResource*, GpuResDestoryFunc> atgGpuResourceMap;
    atgGpuResourceMap _gpuResources;


    typedef std::vector<float> atgDrawPoints;
    atgDrawPoints _drawPoints;

    typedef std::vector<float> atgDrawLines;
    atgDrawLines _drawLines;

    typedef std::vector<float> atgDrawQuads;
    atgDrawQuads _drawQuads;

    typedef std::vector<float> atgDrawFullQuads;
    atgDrawFullQuads _drawFullQuads;

    typedef std::vector<float> atgDrawAABBoxs;
    atgDrawAABBoxs _drawAABBoxs;
};

//////////////////////////////////////////////////////////////////////////
// inline implement

inline void atgRenderer::SetGlobalAmbientColor(const atgVec3& color)
{
    _globalAmbientColor = color;
}

inline const atgVec3& atgRenderer::GetGlobalAmbientColor() const
{
    return _globalAmbientColor;
}

inline const bindLights& atgRenderer::GetBindLights() const
{
    return _bindLights;
}

extern atgRenderer* g_Renderer;


