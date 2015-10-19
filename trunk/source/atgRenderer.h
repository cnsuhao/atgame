#ifndef _ATG_RENDERER_H_
#define _ATG_RENDERER_H_

#include "atgMath.h"

#define USE_OPENGL

#ifndef USE_OPENGL
    #define USE_DIRECTX
#endif

#if defined(USE_OPENGL)
    #ifdef _WIN32
        #include <gl/glew.h>
        #define OPENGL_USE_MAP
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

enum BufferAccessMode
{
    BAM_Static,
    BAM_Dynamic
};

class atgIndexBuffer
{
friend class atgRenderer;
public:
    enum IndexFormat
    {
        IFB_Index16,
        IFB_Index32
    };
public:
    atgIndexBuffer();
    ~atgIndexBuffer();

    bool                    Create(const void* pIndices, uint32 numIndices, 
                                   IndexFormat format, BufferAccessMode accessMode);
    bool                    Destroy();

    inline uint32           GetSize() { return _size; }
    void*                   Lock(uint32 offset, uint32 size);
    void                    Unlock();
    bool                    IsLocked() const;
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
        VA_Diffuse = ( 1 << 4 ),        ///< Diffuse color, 4 components.
        VA_Specular = ( 1 << 5 ),       ///< Specular color, 4 components.
        VA_Texture0 = ( 1 << 6 ),       ///< Texture coordinates for texture unit 0.
        VA_Texture1 = ( 1 << 7 ),       ///< Texture coordinates for texture unit 1.
        VA_Texture2 = ( 1 << 8 ),       ///< Texture coordinates for texture unit 2.
        VA_Texture3 = ( 1 << 9 ),       ///< Texture coordinates for texture unit 3.
        VA_Texture4 = ( 1 << 10 ),      ///< Texture coordinates for texture unit 4.
        VA_Texture5 = ( 1 << 11 ),      ///< Texture coordinates for texture unit 5.
        VA_Texture6 = ( 1 << 12 ),      ///< Texture coordinates for texture unit 6.
        VA_Texture7 = ( 1 << 13 ),      ///< Texture coordinates for texture unit 7.
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

class atgVertexBuffer
{
friend class atgRenderer;
public:
    atgVertexBuffer();
    ~atgVertexBuffer();

    bool                    Create(atgVertexDecl* decl, const void* pData, 
                                   uint32 size, BufferAccessMode accessMode);
    bool                    Destroy();

    inline uint32           GetSize() { return _size; }
    void*                   Lock( uint32 offset, uint32 size );
    void                    Unlock();
    bool                    IsLocked() const;
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

class atgTexture
{
friend class atgRenderer;
public:
    ~atgTexture();

    static atgTexture*      Create(uint32 width, uint32 height, uint32 bbp, const void *pData=NULL);
    bool                    Destory();

    void                    *Lock();
    void                    Unlock();
    bool                    IsLocked() const;

    uint32                  GetWidth()  const { return _width; }
    uint32                  GetHeight() const { return _height; }
    uint32                  GetBBP()    const { return _bbp; }

    void                    SetFilterMode(TextureFilterMode filter);
    void                    SetAddressMode(TextureCoordinate coordinate, TextureAddressMode address);
private:
    atgTexture();
    uint32 _width;
    uint32 _height;
    uint32 _bbp;
    class atgTextureImpl* _impl;
};

enum ResourceShaderType
{
    RST_Vertex,
    RST_Fragment
};

class atgResourceShader
{
public:
    atgResourceShader(ResourceShaderType Type);
    virtual ~atgResourceShader();

    inline ResourceShaderType GetType() { return type; }
    bool                    IsCompiled() { return compiled; }
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
protected:
    bool                    Compile();
private:
    class atgFragmentShaderImpl* _impl;
};
typedef atgFragmentShader atgPixelShader;

class atgPass
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
public:
    atgPass();
    virtual ~atgPass();
    // need has vertex shader and fragment shader
    bool                    Create(const char* VSFilePath, const char* FSFilePath);
    void                    Bind();

    const char*             GetName();

    bool                    SetInt(const char* var_name, int value);
    bool                    SetFloat(const char* var_name, float value);
    bool                    SetFloat3(const char* var_name, const float f[3]);
    bool                    SetFloat4(const char* var_name, const float f[4]);
    bool                    SetMatrix4x4(const char* var_name, const float mat[4][4]);

    bool                    SetTexture(const char* var_name, uint8 index);
#ifdef USE_DIRECTX

    bool                    SetVsInt(const char* var_name, int value);
    bool                    SetVsFloat(const char* var_name, float value);
    bool                    SetVsFloat3(const char* var_name, const float f[3]);
    bool                    SetVsFloat4(const char* var_name, const float f[4]);
    bool                    SetVsMatrix4x4(const char* var_name, const float mat[4][4]);

    bool                    SetPsInt(const char* var_name, int value);
    bool                    SetPsFloat(const char* var_name, float value);
    bool                    SetPsFloat3(const char* var_name, const float f[3]);
    bool                    SetPsFloat4(const char* var_name, const float f[4]);
    bool                    SetPsMatrix4x4(const char* var_name, const float mat[4][4]);
#endif // USE_DIRECTX

protected:
    
    virtual void            BeginContext(void* data);
    virtual void            EndContext(void* data);
    bool                    Load(const char* VSFilePath, const char* FSFilePath);
    bool                    Link();
private:
    std::string _name;
    class atgPassImpl* _impl;
};

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


enum RenderTargetType
{
    RTT_Color,
    RTT_Depth,
    RTT_Stencil
};

class atgRenderTarget
{
public:
    atgRenderTarget();
    ~atgRenderTarget();
    bool                    Create(RenderTargetType type);
    bool                    Destroy();
private:
    class atgRenderTargetImpl* _impl;
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
    bool                    Resize(uint32 width, uint32 height);

    void                    SetViewport(uint32 offsetX, uint32 offsetY, uint32 width, uint32 height);
    void                    GetViewPort(uint32& offsetX, uint32& offsetY, uint32& width, uint32& height) const;

    void                    SetMatrix(MatrixDefine index, const float mat[4][4]);
    void                    GetMatrix(const float (*MtxPtr)[4][4], MatrixDefine index) const;
        
    void                    SetDepthTestEnable(bool enable);
    void                    SetAlphaTestEnable(bool enbale, float value = 1.0f);
    void                    SetFaceCull(FaceCullMode mode);
    void                    GetBlendFunction(BlendFunction& outSrcBlend, BlendFunction& outDestBlend);
    void                    SetBlendFunction(BlendFunction SrcBlend, BlendFunction DestBlend);

    void                    Clear();

    void                    BeginFrame();
    void                    EndFrame();
    void                    Present();

    void                    AddBindLight(atgLight* light);
    void                    AddBindLights(const bindLights& lights);
    void                    RemoveBindLight(atgLight* light);
    void                    ClearBindLight();
    inline const bindLights& GetBindLights() const;

    inline void             SetGlobalAmbientColor(const Vec3& color);
    inline const Vec3&      GetGlobalAmbientColor() const;

    void                    BindMaterial(atgMaterial* material);
    inline atgMaterial*     GetBindMaterial() const;

    void                    BindPass(atgPass* pass);
    inline atgPass*         GetBindPass() const;

    void                    BindIndexBuffer(atgIndexBuffer* indexBuffer);
    inline atgIndexBuffer*  GetBindIndexBuffer() const;

    void                    BindVertexBuffer(atgVertexBuffer* vertexBuffer);
    inline atgVertexBuffer* GetBindVertexBuffer() const;

    void                    BindTexture(uint8 index, atgTexture* texture);
    inline atgTexture*      GetBindTexture(uint8 index) const;

    inline void             SetCamera(atgCamera* pCamera) { _camera = pCamera; }
    inline atgCamera*       GetCamera() const { return _camera; }

    // ‰÷»æµ˜”√
    bool                    DrawPrimitive(PrimitveType pt, uint32 primitveCount, uint32 verticesCount);
    bool                    DrawIndexedPrimitive(PrimitveType pt, uint32 primitveCount, uint32 indicesCount, uint32 verticesCount);

    bool                    DrawPoint(const float center[3], const float color[3]);
    bool                    DrawLine(const float point1[3], const float point2[3], const float color[3]);
    //                        1  3
    //                        2  4
    bool                    DrawQuad(const float p1[3], const float p2[3], const float p3[3], const float p4[3], const float color[3]);
    bool                    DrawTexureQuad(const float p1[3], const float p2[3], const float p3[3], const float p4[3], const float t1[2], const float t2[2], const float t3[2], const float t4[2], atgTexture* pTexture);
    bool                    DrawTexureQuadPass(const float p1[3], const float p2[3], const float p3[3], const float p4[3], const float t1[2], const float t2[2], const float t3[2], const float t4[2], atgTexture* pTexture, atgPass* pPass);

private:
    void                    _CommonInitialize();

protected:
    atgGame*                 _game;
    Matrix                   _matrixs[MD_NUMBER];

    Vec3                    _globalAmbientColor;

    bindLights              _bindLights;
    atgMaterial*            _bindMaterial;
    atgPass*                _bindPass;
    atgIndexBuffer*         _bindIndexBuffer;
    atgVertexBuffer*        _bindVertexBuffer;
    atgTexture*             _bindTexture[MaxNumberBindTexture];
    atgCamera*              _camera;

    // Viewport
    uint32 _VP_offsetX;
    uint32 _VP_offsetY;
    uint32 _VP_width;
    uint32 _VP_height;

public:
    atgPass*                FindCachePass(const char* Key);
    atgTexture*             FindCacheTexture(const char* Key);

    void                    InsertCachePass(const char* Key, atgPass* Pass);
    void                    InsertCacheTexture(const char* Key, atgTexture* Texture);

    void                    FreeCachePass();
    void                    FreeCacheTextures();
private:
    typedef std::map<std::string, atgPass* > atgPassSet;
    atgPassSet  _cachePass;

    typedef std::map<std::string, atgTexture* > atgTextureSet;
    atgTextureSet _cacheTextures;
};

//////////////////////////////////////////////////////////////////////////
// inline implement

inline void atgRenderer::SetGlobalAmbientColor(const Vec3& color)
{
    _globalAmbientColor = color;
}

inline const Vec3& atgRenderer::GetGlobalAmbientColor() const
{
    return _globalAmbientColor;
}

inline const bindLights& atgRenderer::GetBindLights() const
{
    return _bindLights;
}

inline atgMaterial* atgRenderer::GetBindMaterial() const
{
    return _bindMaterial;
}

inline atgPass* atgRenderer::GetBindPass() const
{ 
    return _bindPass; 
}

inline atgIndexBuffer* atgRenderer::GetBindIndexBuffer() const
{
    return _bindIndexBuffer;
}

inline atgVertexBuffer* atgRenderer::GetBindVertexBuffer() const
{
    return _bindVertexBuffer;
}

inline atgTexture* atgRenderer::GetBindTexture( uint8 index ) const
{
    if (0 <= index && index < MaxNumberBindTexture)
    {
        return _bindTexture[index];
    }
    return NULL;
}

extern atgRenderer* g_Renderer;

#endif // _ATG_RENDERER_H_

