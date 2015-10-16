#include "atgBase.h"
#include "atgRenderer.h"
#include "atgShaderLibrary.h"
#include "atgProfile.h"

atgRenderer* g_Renderer = NULL;

const char* StringOpenGLIdentity = "OpenGL Renderer";
const char* StringGLESIdentity = "OpenGL GLES Renderer(2.0)";
const char* StringDirectxIdentity = "Directx Renderer";

bool IsOpenGLGraph()
{
#if defined (USE_OPENGL)
    return true;
#else
    return false;
#endif // USE_OPENGL
}

atgRenderer::atgRenderer(atgGame* game)
{
    _game = game;

    for(int i = 0; i < MD_NUMBER; ++i)
        MatIdentity(_matrixs[i].m);

    _globalAmbientColor = Vec3(0.1f, 0.1f, 0.1f);

    _bindMaterial = NULL;
    _bindPass = NULL;
    _bindIndexBuffer = NULL;
    _bindVertexBuffer = NULL;

    for (int ti = 0; ti < MaxNumberBindTexture; ti++)
        _bindTexture[ti] = NULL;

    _VP_offsetX = 0;
    _VP_offsetY = 0;
    _VP_width = 0;
    _VP_height = 0;

    //LOG("atgRenderer _CommonInitialized!\n");
}

atgRenderer::~atgRenderer(void)
{
}

#if defined (WIN32)
#if defined (USE_OPENGL) 

const char* atgRenderer::getName()
{
    return StringOpenGLIdentity;
}

#elif defined(USE_DIRECTX)

const char* atgRenderer::getName()
{
    return StringDirectxIdentity;
}

#endif

#elif defined(_ANDROID)

const char* atgRenderer::getName()
{
    return StringGLESIdentity;
}

#endif

void atgRenderer::AddBindLight(atgLight* light)
{
#ifdef _DEBUG
    bindLights::iterator it = _bindLights.begin();
    for (bindLights::iterator end = _bindLights.end(); it != end; ++it)
    {
        if (*it == light)
        {
            AASSERT(0);
            return;
        }
    }
#endif

    _bindLights.push_back(light);
}

void atgRenderer::AddBindLights( const bindLights& lights )
{
    std::copy(lights.begin(), lights.end(), _bindLights.begin());
}

void atgRenderer::RemoveBindLight(atgLight* light)
{
    bindLights::iterator it = _bindLights.begin();
    for (bindLights::iterator end; it != end; ++it)
    {
        if (*it == light)
        {
            _bindLights.erase(it);
            break;
        }
    }
}

void atgRenderer::ClearBindLight()
{
    _bindLights.clear();
}

void atgRenderer::BindMaterial( atgMaterial* material )
{
    _bindMaterial = material;
}

bool atgRenderer::DrawPoint( const float center[3], const float color[3] )
{
    static float pointData[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    static const int sizeOfPointData = sizeof(pointData);
    static atgPass* pColorPass = NULL;
    static atgVertexBuffer* pVB = NULL;

    pointData[0] = center[0];
    pointData[1] = center[1];
    pointData[2] = center[2];
    pointData[3] = color[0];
    pointData[4] = color[1];
    pointData[5] = color[2];
    // create pass
    if (!pColorPass)
    {
        pColorPass = atgShaderLibFactory::FindOrCreatePass(SOLIDCOLOR_PASS_IDENTITY);
        if (NULL == pColorPass)
        {
            return false;
        }

        if (pColorPass)
        {
            static_cast<atgShaderSolidColor*>(pColorPass)->SetSolidColor(
                Vec3(color[0], color[1], color[2]));
        }
    }
    // create vertex buffer
    if (!pVB)
    {
        atgVertexDecl decl;
        decl.AppendElement(0, atgVertexDecl::VA_Pos3);
        decl.AppendElement(0, atgVertexDecl::VA_Diffuse);
        pVB = new atgVertexBuffer();
        pVB->Create(&decl, pointData, sizeOfPointData, BAM_Static);
    }else
    {
        // only update vertex buffer
        void *pLockData = pVB->Lock(0, sizeOfPointData);
        if(pLockData)
        {
            memcpy(pLockData, pointData, sizeOfPointData);
            pVB->Unlock();
        }
    }
    g_Renderer->SetDepthTestEnable(false);
    g_Renderer->BindPass(pColorPass);
    g_Renderer->BindVertexBuffer(pVB);

    g_Renderer->DrawPrimitive(PT_POINTS, 1, 1);
    g_Renderer->SetDepthTestEnable(true);
    return true;
}

bool atgRenderer::DrawLine(const float point1[3], const float point2[3], const float color[3])
{
    ATG_PROFILE("atgRenderer::DrawLine");
    static float lineData[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };
    static const int sizeOfLineData = sizeof(lineData);
    static atgPass* pColorPass = NULL;
    static atgVertexBuffer* pVB = NULL;
    lineData[0] = point1[0];
    lineData[1] = point1[1];
    lineData[2] = point1[2];
    lineData[3] = point2[0];
    lineData[4] = point2[1];
    lineData[5] = point2[2];
    // create pass
    if (!pColorPass)
    {
        pColorPass = atgShaderLibFactory::FindOrCreatePass(SOLIDCOLOR_PASS_IDENTITY);
        if (NULL == pColorPass)
            return false;
    }

    if (pColorPass)
    {
        static_cast<atgShaderSolidColor*>(pColorPass)->SetSolidColor(
            Vec3(color[0], color[1], color[2]));
    }

    // create vertex buffer
    if (!pVB)
    {
        atgVertexDecl decl;
        decl.AppendElement(0, atgVertexDecl::VA_Pos3);
        pVB = new atgVertexBuffer();
        pVB->Create(&decl, lineData, sizeOfLineData, BAM_Static);
    }else
    {
        // only update vertex buffer
        void *pLockData = pVB->Lock(0, sizeOfLineData);
        if(pLockData)
        {
            memcpy(pLockData, lineData, sizeOfLineData);
            pVB->Unlock();
        }
    }
    g_Renderer->SetDepthTestEnable(false);
    g_Renderer->BindPass(pColorPass);
    g_Renderer->BindVertexBuffer(pVB);

    g_Renderer->DrawPrimitive(PT_LINES, 1, 2);
    g_Renderer->SetDepthTestEnable(true);
    return true;
}

bool atgRenderer::DrawQuad(const float point1[3], const float point2[3], const float point3[3], const float point4[3], const float color[3])
{
    static float QuadData[] = {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f

    };
    static const int sizeOfQuadData = sizeof(QuadData);
    static atgPass* pColorPass = NULL;
    static atgVertexBuffer* pVB = NULL;
    
    VecCopy(point1, &QuadData[0]);
    VecCopy(point2, &QuadData[3]);
    VecCopy(point3, &QuadData[6]);
    VecCopy(point4, &QuadData[9]);
    // create pass
    if (!pColorPass)
    {
        pColorPass = atgShaderLibFactory::FindOrCreatePass(SOLIDCOLOR_PASS_IDENTITY);
        if (NULL == pColorPass)
            return false;
    }

    if (pColorPass)
    {
        static_cast<atgShaderSolidColor*>(pColorPass)->SetSolidColor(
            Vec3(color[0], color[1], color[2]));
    }

    // create vertex buffer
    if (!pVB)
    {
        atgVertexDecl decl;
        decl.AppendElement(0, atgVertexDecl::VA_Pos3);
        pVB = new atgVertexBuffer();
        pVB->Create(&decl, QuadData, sizeOfQuadData, BAM_Static);
    }else
    {
        // only update vertex buffer
        void *pLockData = pVB->Lock(0, sizeOfQuadData);
        if(pLockData)
        {
            memcpy(pLockData, QuadData, sizeOfQuadData);
            pVB->Unlock();
        }
    }
    g_Renderer->SetDepthTestEnable(false);
    g_Renderer->BindPass(pColorPass);
    g_Renderer->BindVertexBuffer(pVB);

    g_Renderer->DrawPrimitive(PT_TRIANGLE_STRIP, 2, 4);
    g_Renderer->SetDepthTestEnable(true);
    return true;
}

bool atgRenderer::DrawTexureQuad(const float p1[3], const float p2[3], const float p3[3], const float p4[3], const float t1[2], const float t2[2], const float t3[2], const float t4[2], atgTexture* pTexture)
{
    AASSERT(pTexture != NULL);

    static float QuadData[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 1.0f,

    };
    static const int sizeOfQuadData = sizeof(QuadData);
    static atgPass* pTexturePass = NULL;
    static atgVertexBuffer* pVB = NULL;

    VecCopy(p1, &QuadData[0]);
    QuadData[3] = t1[0]; QuadData[4] = t1[1];
    VecCopy(p2, &QuadData[5]);
    QuadData[8] = t2[0]; QuadData[9] = t2[1];
    VecCopy(p3, &QuadData[10]);
    QuadData[13] = t3[0]; QuadData[14] = t3[1];
    VecCopy(p4, &QuadData[15]);
    QuadData[18] = t4[0]; QuadData[19] = t4[1];
    // create pass
    if (!pTexturePass)
    {
        pTexturePass = atgShaderLibFactory::FindOrCreatePass(NOT_LIGNTE_TEXTURE_PASS_IDENTITY);
        if (NULL == pTexturePass)
            return false;
    }

    // create vertex buffer
    if (!pVB)
    {
        atgVertexDecl decl;
        decl.AppendElement(0, atgVertexDecl::VA_Pos3);
        decl.AppendElement(0, atgVertexDecl::VA_Texture0);

        pVB = new atgVertexBuffer();
        pVB->Create(&decl, QuadData, sizeOfQuadData, BAM_Static);
    }else
    {
        // only update vertex buffer
        void *pLockData = pVB->Lock(0, sizeOfQuadData);
        if(pLockData)
        {
            memcpy(pLockData, QuadData, sizeOfQuadData);
            pVB->Unlock();
        }
    }
    g_Renderer->SetDepthTestEnable(false);
    g_Renderer->BindTexture(0, pTexture);
    g_Renderer->BindPass(pTexturePass);
    g_Renderer->BindVertexBuffer(pVB);

    g_Renderer->DrawPrimitive(PT_TRIANGLE_STRIP, 2, 4);
    g_Renderer->SetDepthTestEnable(true);
    return true;
}

bool atgRenderer::DrawTexureQuadPass(const float p1[3], const float p2[3], const float p3[3], const float p4[3], const float t1[2], const float t2[2], const float t3[2], const float t4[2], atgTexture* pTexture, atgPass* pPass)
{
    AASSERT(pTexture != NULL && pPass != NULL);

    static float QuadData[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 1.0f,

    };
    static const int sizeOfQuadData = sizeof(QuadData);
    static atgPass* pTexturePass = NULL;
    static atgVertexBuffer* pVB = NULL;

    VecCopy(p1, &QuadData[0]);
    QuadData[3] = t1[0]; QuadData[4] = t1[1];
    VecCopy(p2, &QuadData[5]);
    QuadData[8] = t2[0]; QuadData[9] = t2[1];
    VecCopy(p3, &QuadData[10]);
    QuadData[13] = t3[0]; QuadData[14] = t3[1];
    VecCopy(p4, &QuadData[15]);
    QuadData[18] = t4[0]; QuadData[19] = t4[1];

    // create vertex buffer
    if (!pVB)
    {
        atgVertexDecl decl;
        decl.AppendElement(0, atgVertexDecl::VA_Pos3);
        decl.AppendElement(0, atgVertexDecl::VA_Texture0);

        pVB = new atgVertexBuffer();
        pVB->Create(&decl, QuadData, sizeOfQuadData, BAM_Static);
    }else
    {
        // only update vertex buffer
        void *pLockData = pVB->Lock(0, sizeOfQuadData);
        if(pLockData)
        {
            memcpy(pLockData, QuadData, sizeOfQuadData);
            pVB->Unlock();
        }
    }
    g_Renderer->SetDepthTestEnable(false);
    g_Renderer->BindPass(pPass);
    g_Renderer->BindVertexBuffer(pVB);

    g_Renderer->DrawPrimitive(PT_TRIANGLE_STRIP, 2, 4);
    g_Renderer->SetDepthTestEnable(true);
    return true;
}

atgPass* atgRenderer::FindCachePass(const char* key)
{
    if (!key)
    {
        return NULL;
    }

    atgPassSet::iterator itr = _cachePass.find(key);
    if (itr != _cachePass.end())
        return itr->second;

    return NULL;
}

atgTexture* atgRenderer::FindCacheTexture(const char* key)
{
    if (!key)
    {
        return NULL;
    }

    atgTextureSet::iterator itr = _cacheTextures.find(key);
    if (itr != _cacheTextures.end())
        return itr->second;

    return NULL;
}

void  atgRenderer::InsertCachePass(const char* Key, atgPass* Pass)
{

#ifdef _DEBUG
    if (FindCachePass(Key))
        return;
#endif // _DEBUG

    _cachePass.insert(atgPassSet::value_type(Key,Pass));

}

void atgRenderer::InsertCacheTexture(const char* Key, atgTexture* TexturePack)
{
#ifdef _DEBUG
    if (FindCacheTexture(Key))
        return;
#endif // _DEBUG

    _cacheTextures.insert(atgTextureSet::value_type(Key, TexturePack));
}