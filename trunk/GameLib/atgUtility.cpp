#include "atgUtility.h"
#include "atgShaderLibrary.h"
#include "atgMisc.h"
#include "atgCamera.h"
#include "atgIntersection.h"

inline atgVec3 Convert(const MiscVec3& v)
{
    return atgVec3(v.x, v.y, v.z);
}


atgFlyCamera::atgFlyCamera():_pCamera(0)
{
    button[0] = button[1] = false;
}

atgFlyCamera::~atgFlyCamera()
{

}

bool atgFlyCamera::Create( class atgCamera* pCamera )
{
    _pCamera = pCamera;

    return true;
}

void atgFlyCamera::OnKeyScanDown( Key::Scan keyscan )
{
    float moveSpeed = 1.5f;

    switch (keyscan)
    {
    case Key::W:
        {
            atgVec3 forward = _pCamera->GetForward() * moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += forward;
            _pCamera->SetPosition(pos);

        }
        break;
    case Key::S:
        {
            atgVec3 forward = _pCamera->GetForward() * -moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += forward;
            _pCamera->SetPosition(pos);
        }
        break;
    case Key::A:
        {
            atgVec3 right = _pCamera->GetRight() * moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += right;
            _pCamera->SetPosition(pos);
        }
        break;
    case Key::D:
        {
            atgVec3 right = _pCamera->GetRight() * -moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += right;
            _pCamera->SetPosition(pos);
        }
        break;
    default:
        break;
    }
}

void atgFlyCamera::OnPointerDown( uint8 id, int16 x, int16 y )
{
    if(id == 0)
    {
        button[0] = true;
    }

    if(id == 1)
    {
        button[1] = true;
    }
}

void atgFlyCamera::OnPointerMove( uint8 id, int16 x, int16 y )
{
    static int16 last_x = x;
    static int16 last_y = y;

    if (id == MBID_MIDDLE && _pCamera)
    {
        float moveSpeed = 10.5f;
        if (x > 0)
        {
            atgVec3 forward = _pCamera->GetForward() * moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += forward;
            _pCamera->SetPosition(pos);
        }
        else
        {
            atgVec3 forward = _pCamera->GetForward() * -moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += forward;
            _pCamera->SetPosition(pos);
        }
    }
    else
    {
        if (button[1])
        {
            if (_pCamera)
            {
                float oYaw = _pCamera->GetYaw();
                float oPitch = _pCamera->GetPitch();
                float dx = static_cast<float>(x - last_x);
                float dy = static_cast<float>(y - last_y);
                oYaw += dx * 0.001f;
                oPitch += dy * 0.001f;
                oPitch = atgMath::Clamp(oPitch, atgMath::DegreesToRadians(-270.0f), atgMath::DegreesToRadians(-90.0f));
                _pCamera->SetYaw(oYaw);
                _pCamera->SetPitch(oPitch);
            }
        }

        last_x = x;
        last_y = y;
    }
}

void atgFlyCamera::OnPointerUp( uint8 id, int16 x, int16 y )
{
    if(id == 0)
    {
        button[0] = false;
    }

    if(id == 1)
    {
        button[1] = false;
    }
}

atgSampleDrawFrustum::atgSampleDrawFrustum()
{

}

atgSampleDrawFrustum::~atgSampleDrawFrustum()
{

}

bool atgSampleDrawFrustum::Create()
{
    _pCamera = new atgCamera();
    _pCamera->SetClipNear(10);
    _pCamera->SetClipFar(400);
    _pCamera->SetPosition(atgVec3(0,150,300));

    return true;
}

void atgSampleDrawFrustum::Render()
{
    atgFrustum f;
    f.BuildFrustumPlanes(_pCamera->GetView(), _pCamera->GetProj());
    f.DebugRender();
}

atgSamlpeViewPortDrawAxis::atgSamlpeViewPortDrawAxis()
{

}

atgSamlpeViewPortDrawAxis::~atgSamlpeViewPortDrawAxis()
{

}

void atgSamlpeViewPortDrawAxis::Render( class atgCamera* sceneCamera )
{
    uint32 oldVP[4];
    g_Renderer->GetViewPort(oldVP[0], oldVP[1], oldVP[2], oldVP[3]);

    uint32 newVP[4] = {0, IsOpenGLGraph() ? 0 : oldVP[3] - 100, 100, 100 };
    g_Renderer->SetViewPort(newVP[0], newVP[1], newVP[2], newVP[3]);

    atgMatrix oldWorld;
    g_Renderer->GetMatrix(oldWorld, MD_WORLD);
    g_Renderer->SetMatrix(MD_WORLD, MatrixIdentity);

    atgMatrix oldView;
    g_Renderer->GetMatrix(oldView, MD_VIEW);
    atgMatrix newView(MatrixIdentity);
    atgVec3 p(0.0f, 0.0f, 3.5f);
    if (sceneCamera)
    {
        newView.SetColumn3(0, sceneCamera->GetRight());
        newView.SetColumn3(1, sceneCamera->GetUp());
        newView.SetColumn3(2, sceneCamera->GetForward());
        newView.Transpose();
        newView.SetColumn3(3, p*-1.0f);
    }
    else
    {
        atgMatrix::LookAt(p, Vec3Zero, Vec3Up, newView);
    }
    g_Renderer->SetMatrix(MD_VIEW, newView);

    atgMatrix oldProj;
    g_Renderer->GetMatrix(oldProj, MD_PROJECTION);
    atgMatrix newProj;
    atgMatrix::Perspective(37.5f,1.f,0.1f,500.0f,newProj);
    g_Renderer->SetMatrix(MD_PROJECTION, newProj);

    g_Renderer->BeginLine();
    g_Renderer->AddLine(Vec3Zero, Vec3Up, Vec3Up);
    g_Renderer->AddLine(Vec3Zero, Vec3Right, Vec3Right);
    g_Renderer->AddLine(Vec3Zero, Vec3Forward, Vec3Forward);
    g_Renderer->EndLine();

    g_Renderer->SetMatrix(MD_WORLD, oldWorld);
    g_Renderer->SetMatrix(MD_VIEW, oldView);
    g_Renderer->SetMatrix(MD_PROJECTION, oldProj);
    g_Renderer->SetViewPort(oldVP[0], oldVP[1], oldVP[2], oldVP[3]);
}

//////////////////////////////////////////////////////////////////////////
//>水波shader
class atgRippleShader : public atgShaderLibPass
{
public:
    atgRippleShader();
    ~atgRippleShader();

    virtual bool			ConfingAndCreate();

    void					SetDxDy(float dx, float dy) { _dx = dx; _dy = dy; }

    void                    SetPrimitiveTex(atgTexture* pTex) { _primitiveTex = pTex; }
    void                    SetRippleTex(atgTexture* pTex) { _rippleTex = pTex; }

protected:
    virtual void			BeginContext(void* data);
protected:
    float                   _dx;    //>纹理宽的倒数(1/w)
    float                   _dy;    //>纹理搞的倒数(1/h)

    atgTexture*             _primitiveTex;
    atgTexture*             _rippleTex;
};

#define RIPPLE_PASS_IDENTITY "atgRippleShader"
EXPOSE_INTERFACE(atgRippleShader, atgPass, RIPPLE_PASS_IDENTITY);

atgRippleShader::atgRippleShader()
{
    _primitiveTex = NULL;
    _rippleTex = NULL;
}

atgRippleShader::~atgRippleShader()
{
}

bool atgRippleShader::ConfingAndCreate()
{
    bool rs = false;
    if (IsOpenGLGraph())
    {
        rs = atgPass::Create("shaders/ripple.glvs", "shaders/ripple.glfs");
    }
    else
    {
        rs = atgPass::Create("shaders/ripple.dxvs", "shaders/ripple.dxps");
    }

    return rs;
}

void atgRippleShader::BeginContext(void* data)
{
    atgPass::BeginContext(data);
    
    if (_primitiveTex)
    {
        _primitiveTex->Bind(0);
        SetTexture("textureSampler", 0);

        TextureFormat format = _primitiveTex->GetTextureFormat();
        if (format == TF_R4G4B4A4 || 
            format == TF_R8G8B8A8 ||
            format == TF_R5G5B5A1)
        {//>含有aplha通道,开启混合
            g_Renderer->SetBlendFunction(BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA);
        }
    }

    
    _rippleTex->Bind(1);
    SetTexture("waterHeightSampler", 1);

    atgVec4 d(_dx, _dy, 1.0f, 1.0f);
    SetFloat4("u_d", d);
}

atgSampleWater::atgSampleWater(void):_pWater(0),_pDyncTexture(0),_pCamera(0)
{

}

atgSampleWater::~atgSampleWater(void)
{
    SAFE_DELETE(_pWater);
    SAFE_DELETE(_pDyncTexture);
    SAFE_DELETE(_pCamera);
}

bool atgSampleWater::Create( int w, int h )
{
    if (_pWater != NULL)
    {
        LOG("create water failture, duplicate create.");
        return false;
    }

    _pWater = new Water(200, 160);

    _pDyncTexture = new atgTexture();
    if (!_pDyncTexture->Create(_pWater->Width, _pWater->Height, TF_R32F, NULL))
    {
        LOG("create TF_R32F texture failture.");
        return false;
    }
    _pDyncTexture->SetFilterMode(TFM_FILTER_NOT_MIPMAP_ONLY_LINEAR);

    _pCamera = new atgCamera();
    _pCamera->SetPosition(atgVec3(0,0, 150));
    _pCamera->SetForward(Vec3Back);

    btn[0] = btn [1] = 0;

    return true;
}

void atgSampleWater::Updata()
{
    if (_pWater)
    {
        _pWater->Updata();

        if (_pDyncTexture->IsLost())
        {
           if(!_pDyncTexture->Create(_pWater->Width, _pWater->Height, TF_R32F, NULL))
           {
               return;
           }
        }

        atgTexture::LockData lockData = _pDyncTexture->Lock();
        if (lockData.pAddr)
        {
            if (lockData.pitch > 0)
            {
                char* pSrc = (char*)_pWater->GetBuff();
                char* pDst = (char*)lockData.pAddr;
                int lineSize = _pWater->Width * sizeof(float);
                for (int i = 0; i < _pWater->Height; ++i)
                {
                    memcpy(pDst, pSrc, lineSize);
                    pDst += lockData.pitch;
                    pSrc += lineSize;
                }
            }
            else
            {
                _pWater->CopyTo(lockData.pAddr);
            }

            _pDyncTexture->Unlock();
        }
    }
}

void atgSampleWater::Drop( float xi, float yi )
{
    if (_pWater)
    {
        _pWater->Drop(xi, yi);
    }
}

void atgSampleWater::Render(atgTexture* pTexture /* = 0 */)
{
    const float textureQuadData[] = {
        -100.0f,  100.0f, 20.0f, 0.0f, 0.0f,
         100.0f,  100.0f, 20.0f, 1.0f, 0.0f,
        -100.0f, -100.0f, 20.0f, 0.0f, 1.0f,
         100.0f, -100.0f, 20.0f, 1.0f, 1.0f,
    };

    atgRippleShader* pRippleShader = static_cast<atgRippleShader*>(atgShaderLibFactory::FindOrCreatePass(RIPPLE_PASS_IDENTITY));
    if (NULL == pRippleShader)
    {
        LOG("can't find pass[%s]\n", RIPPLE_PASS_IDENTITY);
        return;
    }

    pRippleShader->SetDxDy(1.0f / float(_pWater->Width - 1), 1.0f / float(_pWater->Height - 1));
    pRippleShader->SetRippleTex(_pDyncTexture);

    Updata();

    if (_pCamera)
    {
        g_Renderer->SetMatrix(MD_VIEW, _pCamera->GetView());
        g_Renderer->SetMatrix(MD_PROJECTION, _pCamera->GetProj()); 
    }

    if (pTexture)
    {
        pRippleShader->SetPrimitiveTex(pTexture);
    }

    g_Renderer->DrawQuadByPass(&textureQuadData[0], &textureQuadData[5], &textureQuadData[10], &textureQuadData[15], &textureQuadData[3], &textureQuadData[8], &textureQuadData[13], &textureQuadData[18], RIPPLE_PASS_IDENTITY);
}

void atgSampleWater::OnKeyScanDown( Key::Scan keyscan )
{
    float moveSpeed = 10.5f;

    switch (keyscan)
    {
    case Key::W:
        {
            atgVec3 forward = _pCamera->GetForward() * moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += forward;
            _pCamera->SetPosition(pos);

        }
        break;
    case Key::S:
        {
            atgVec3 forward = _pCamera->GetForward() * -moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += forward;
            _pCamera->SetPosition(pos);
        }
        break;
    case Key::A:
        {
            atgVec3 right = _pCamera->GetRight() * moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += right;
            _pCamera->SetPosition(pos);
        }
        break;
    case Key::D:
        {
            atgVec3 right = _pCamera->GetRight() * -moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += right;
            _pCamera->SetPosition(pos);
        }
        break;
    default:
        break;
    }
}

void atgSampleWater::OnPointerDown( uint8 id, int16 x, int16 y )
{
    if(id == 1)
    {
        // right button down
        btn[1] = true;
    }

    if (id == 0)
    {
        btn[0] = true;
        
        uint32 vp[4];
        g_Renderer->GetViewPort(vp[0],vp[1],vp[2],vp[3]);
        Drop(float(x*1.0f/vp[2]), float(y*1.0f/vp[3]));
    }
}

void atgSampleWater::OnPointerMove( uint8 id, int16 x, int16 y )
{
    if(btn[0])
    {
        uint32 vp[4];
        g_Renderer->GetViewPort(vp[0],vp[1],vp[2],vp[3]);
        Drop(float(x*1.0f/vp[2]), float(y*1.0f/vp[3]));
    }

    if (btn[1])
    {

    }

    if (id == MBID_MIDDLE && _pCamera)
    {
        float moveSpeed = 10.5f;
        if (x > 0)
        {
            atgVec3 forward = _pCamera->GetForward() * moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += forward;
            _pCamera->SetPosition(pos);
        }
        else
        {
            atgVec3 forward = _pCamera->GetForward() * -moveSpeed;
            atgVec3 pos = _pCamera->GetPosition();
            pos += forward;
            _pCamera->SetPosition(pos);
        }
    }
}

void atgSampleWater::OnPointerUp( uint8 id, int16 x, int16 y )
{
    if(id == 1)
    {
        // right button down
        btn[1] = false;
    }

    if (id == 0)
    {
        btn[0] = false;
    }
}

atgSampleRenderToTextrue::atgSampleRenderToTextrue()
{
    pDepthTex = 0;
    pColorTex = 0;
    pRT = 0;
}

atgSampleRenderToTextrue::~atgSampleRenderToTextrue()
{
    SAFE_DELETE(pDepthTex);
    SAFE_DELETE(pColorTex);
    SAFE_DELETE(pRT);
}

bool atgSampleRenderToTextrue::Create()
{
    //>创建深度纹理
    pDepthTex = new atgTexture();
    if (!pDepthTex->Create(512,512, TF_D24S8, NULL, true))
    {
        return false;
    }

    //>创建颜色纹理
    pColorTex = new atgTexture();
    if (!pColorTex->Create(512,512, TF_R8G8B8A8, NULL, true))
    {
        return false;
    }

    //>设置颜色纹理过滤模式
    pColorTex->SetFilterMode(TFM_FILTER_NOT_MIPMAP_ONLY_LINEAR);
    pColorTex->SetAddressMode(TC_COORD_U, TAM_ADDRESS_CLAMP);
    pColorTex->SetAddressMode(TC_COORD_V, TAM_ADDRESS_CLAMP);

    pRT = new atgRenderTarget();
    std::vector<atgTexture*> colorBuffer; colorBuffer.push_back(pColorTex);
    if (!pRT->Create(colorBuffer, pDepthTex))
    {
        return false;
    }

    return true;
}

void atgSampleRenderToTextrue::OnDrawBegin()
{
    if (pColorTex->IsLost())
    {
        pColorTex->Create(512,512, TF_R8G8B8A8, NULL, true);
    }

    if (pDepthTex->IsLost())
    {
        pDepthTex->Create(512,512, TF_D24S8, NULL, true);
    }

    g_Renderer->PushRenderTarget(0, pRT);

    g_Renderer->Clear();
}

void atgSampleRenderToTextrue::OnDrawEnd()
{
    g_Renderer->PopRenderTarget(0);

    g_Renderer->DrawFullScreenQuad(pColorTex, IsOpenGLGraph());
}

//////////////////////////////////////////////////////////////////////////
//>场景深度
class atgShaderRTSceenDepthColor : public atgShaderLibPass
{
public:
    atgShaderRTSceenDepthColor();
    ~atgShaderRTSceenDepthColor();

    virtual bool			ConfingAndCreate();

    void                    SetMatirxOfLightViewPojection(const atgMatrix& mat) { _ligthViewProj = mat; }
    void                    SetOpenglDepthTextureOES(float f) { _opengl_depth_texture = f; }

protected:
    virtual void			BeginContext(void* data);
    
    atgMatrix               _ligthViewProj;
    float                    _opengl_depth_texture;
};

#define RT_DEPTH_COLOR_PASS_IDENTITY "atgSceenDepthColorShader"
EXPOSE_INTERFACE(atgShaderRTSceenDepthColor, atgPass, RT_DEPTH_COLOR_PASS_IDENTITY);

atgShaderRTSceenDepthColor::atgShaderRTSceenDepthColor()
{
    _opengl_depth_texture = 0;
}

atgShaderRTSceenDepthColor::~atgShaderRTSceenDepthColor()
{

}

bool atgShaderRTSceenDepthColor::ConfingAndCreate()
{
    bool rs = false;

    if (IsOpenGLGraph())
    {
        rs = atgPass::Create("shaders/rt_depth_color.glvs", "shaders/rt_depth_color.glfs");
    }
    else
    {
        rs = atgPass::Create("shaders/rt_depth_color.dxvs", "shaders/rt_depth_color.dxps");
    }

    return rs;
}

void atgShaderRTSceenDepthColor::BeginContext(void* data)
{
    atgPass::BeginContext(data);

    // set matrix.
    SetMatrix4x4("mat_light_view_Projection", _ligthViewProj);

    // set use OES_depth_texture
    if (IsOpenGLGraph())
    {
        SetFloat("use_depth_texture_OES", _opengl_depth_texture);
    }
}


//////////////////////////////////////////////////////////////////////////
//>场景深度
class atgShaderShadowMapping : public atgShaderLibPass
{
public:
    atgShaderShadowMapping();
    ~atgShaderShadowMapping();

    virtual bool			ConfingAndCreate();


    void                    SetMatirxOfLightViewPojection(const atgMatrix& mat) { _ligthViewProj = mat; }
    void                    SetColorDepthTex(atgTexture* pTex) { _pColorDepthTex = pTex; }

    void                    SetLight(const atgVec3& ligPos, const atgVec3& ligDir) { _ligPos = ligPos; _ligDir = ligDir; }
    void                    SetSpotParam(float outerCone, float innerCone) { _spot_outer_cone = outerCone;  _spot_inner_cone = innerCone; }
    void                    SetAmbientColor(const atgVec4& color) { _ambientColor = color; }
    void                    SetBias(float bias) { _bias = bias; }
    void                    SetOpenglDepthTextureOES(float f) { _opengl_depth_texture = f; }

protected:
    virtual void			BeginContext(void* data);

    atgMatrix                  _ligthViewProj;
    atgTexture*             _pColorDepthTex;

    atgVec3                 _ligPos;
    atgVec3                 _ligDir;
    float                   _spot_outer_cone;
    float                   _spot_inner_cone;
    atgVec4                 _ambientColor;
    float                   _bias;
    float                   _opengl_depth_texture;
};

#define SHADOW_MAPPING_PASS_IDENTITY "atgShadowMappingShader"
EXPOSE_INTERFACE(atgShaderShadowMapping, atgPass, SHADOW_MAPPING_PASS_IDENTITY);


atgShaderShadowMapping::atgShaderShadowMapping():_pColorDepthTex(0)
{

}

atgShaderShadowMapping::~atgShaderShadowMapping()
{

}

bool atgShaderShadowMapping::ConfingAndCreate()
{
    bool rs = false;

    if (IsOpenGLGraph())
    {
        rs = atgPass::Create("shaders/rt_shadow_mapping.glvs", "shaders/rt_shadow_mapping.glfs");
    }
    else
    {
        rs = atgPass::Create("shaders/rt_shadow_mapping.dxvs", "shaders/rt_shadow_mapping.dxps");
    }

    return rs;
}

void atgShaderShadowMapping::BeginContext(void* data)
{
    atgPass::BeginContext(data);

    // set matrix.
    SetMatrix4x4("mat_light_view_Projection", _ligthViewProj);

    // set use OES_depth_texture
    if (IsOpenGLGraph())
    {
        SetFloat("use_depth_texture_OES", _opengl_depth_texture);
    }

    // set texture
    _pColorDepthTex->Bind(0);
    SetTexture("rtDepthSampler", 0);

    // set light
    SetFloat3("LightPosition", _ligPos);
    SetFloat3("LightDirection", _ligDir);
    SetFloat("spot_outer_cone", _spot_outer_cone);
    SetFloat("spot_inner_cone", _spot_inner_cone);

    // set bias
    SetFloat("bias", _bias);

    // set ambient
    SetFloat4("ambient", _ambientColor);

    // set port size
    uint32 viewPort[4];
    g_Renderer->GetViewPort(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);
    float viewPortF[2];
    viewPortF[0] = (float)viewPort[2];
    viewPortF[1] = (float)viewPort[3];
    SetFloat2("fViewportDimensions", viewPortF);
}

//////////////////////////////////////////////////////////////////////////
//>影子贴图
atgSimpleShadowMapping::atgSimpleShadowMapping():pRT(0),pColorTex(0),pDepthTex(0)
{
    bias = 0.008f;
    d_far = 1000.f;
    d_near = 1.0f;
}

atgSimpleShadowMapping::~atgSimpleShadowMapping()
{

}


void atgSimpleShadowMapping::Destory()
{
    SAFE_DELETE(pColorTex);
    SAFE_DELETE(pDepthTex);
    SAFE_DELETE(pRT);
}


bool atgSimpleShadowMapping::Create()
{
    const int textureSize = 1024;
    pColorTex = new atgTexture();
    
    _opengl_depth_texture = 0;
#if 1 && defined(_ANDROID) //> oes depth texture 效果不好(精度值太低了), 所以禁用了
    std::string extensions = (char*)glGetString(GL_EXTENSIONS);
    if (extensions.find("OES_depth_texture") != std::string::npos)
    {
        if (false == pColorTex->Create(textureSize, textureSize, TF_R32F, NULL, true))
        {
            return false;
        }
        _opengl_depth_texture = 1.0f;
    }
    else
    {
        if (false == pColorTex->Create(textureSize, textureSize, IsOpenGLGraph() ? TF_R8G8B8A8 : TF_R32F, NULL, true))
        {
            return false;
        }
    }
#else
    if (false == pColorTex->Create(textureSize, textureSize, IsOpenGLGraph() ? TF_R8G8B8A8 : TF_R32F, NULL, true))
    {
        return false;
    }
#endif

    pColorTex->SetFilterMode(TFM_FILTER_NOT_MIPMAP_ONLY_LINEAR);

    pDepthTex = new atgTexture();
    if (false == pDepthTex->Create(textureSize, textureSize, TF_D16, NULL, true))
    {
        return false;
    }

    std::vector<atgTexture*> colorBuffer;
    colorBuffer.push_back(pColorTex);
    pRT = new atgRenderTarget();
    if (false == pRT->Create(colorBuffer, pDepthTex))
    {
        return false;
    }

    lightPos.Set(150.f*1.2f,180.f*1.2f,-110.f*1.2f);
    lightDir.Set(-0.65f,-0.73f,0.172f);

    //lightPos.Set(1149.f*0.5f,2113.f*0.5f,-324.f*0.5f);
    //lightDir.Set(-0.326f,-0.93f,0.166f);

    lightDir.Normalize();
    atgMatrix::LookAt(lightPos, (lightPos + lightDir), Vec3Up, lightViewMatrix);

    return true;
}

void atgSimpleShadowMapping::Render(class atgCamera* sceneCamera)
{
    DrawDepthTex(sceneCamera);
    DrawSceen(sceneCamera);
}

void atgSimpleShadowMapping::OnKeyScanDown( Key::Scan keyscan )
{
    switch (keyscan)
    {
    case Key::Semicolon:
        {
            bias += 0.0001f;
            LOG("new bias[%f]\n", bias);
            break;
        }
    case Key::Apostrophe:
        {
            bias -= 0.0001f;
            LOG("new bias[%f]\n", bias);
            break;
        }
    case Key::Comma: //<
        {
            d_far += 10.f;
            LOG("new d_far[%f]\n", d_far);
        }break;
    case Key::Period: //>
        {
            d_far -= 10.f;
            LOG("new d_far[%f]\n", d_far);

        }break;
    case Key::LeftBracket: //[
        {
            //bias += 0.000001f;
            //LOG("new bias[%f]\n", bias);
            d_near += 10.f;
            LOG("new d_near[%f]\n", d_near);
        }break;
    case Key::RightBracket: //]
        {
            d_near -= 10.f;
            LOG("new d_near[%f]\n", d_near);
            //bias -= 0.000001f;
            //LOG("new bias[%f]\n", bias);
        }break;
    default:
        break;
    }
}

void atgSimpleShadowMapping::OnPointerMove( uint8 id, int16 x, int16 y )
{
#ifndef _WIN32
    //if (id == 0)
    //{
    //    bias -= 0.0001f;
    //}else
    //{
    //    bias += 0.0001f;
    //}
    //LOG("----new bias[%f]\n", bias);

    if (id == 0)
    {
        d_near += 1.f;
    }
    else
    {
        d_near -= 1.f;
    }
    LOG("new d_near[%f]\n", d_near);
#endif // !_WIN32
}

void atgSimpleShadowMapping::DrawDepthTex(class atgCamera* sceneCamera)
{
    if (pRT)
    {
        g_Renderer->PushRenderTarget(0, pRT);
        
        g_Renderer->Clear();

        atgPass* pPass = atgShaderLibFactory::FindOrCreatePass(RT_DEPTH_COLOR_PASS_IDENTITY);
        if (pPass == NULL)
        {
            LOG("can't find pass[%s]\n", RT_DEPTH_COLOR_PASS_IDENTITY);
            return;
        }

        //>fov在60度左右影子效果比较好.
        atgMatrix projMat;
        atgMatrix::Perspective(60.0f, 1.0f, d_near, d_far, projMat);
        atgShaderRTSceenDepthColor* pDepthPass = (atgShaderRTSceenDepthColor*)pPass;
        pDepthPass->SetMatirxOfLightViewPojection(projMat * lightViewMatrix);
        pDepthPass->SetOpenglDepthTextureOES(_opengl_depth_texture);
        
        DrawBox(RT_DEPTH_COLOR_PASS_IDENTITY);
        DrawPlane(RT_DEPTH_COLOR_PASS_IDENTITY);
        
        g_Renderer->PopRenderTarget(0);
    }
}

void atgSimpleShadowMapping::DrawSceen(class atgCamera* sceneCamera)
{
    g_Renderer->SetMatrix(MD_VIEW, sceneCamera->GetView());
    g_Renderer->SetMatrix(MD_PROJECTION, sceneCamera->GetProj());

    atgPass* pPass = atgShaderLibFactory::FindOrCreatePass(SHADOW_MAPPING_PASS_IDENTITY);
    if (pPass == NULL)
    {
        LOG("can't find pass[%s]\n", SHADOW_MAPPING_PASS_IDENTITY);
        return;
    }

    atgShaderShadowMapping* pShadowPass = (atgShaderShadowMapping*)pPass;
    
    //>fov在60度左右影子效果比较好.
    atgMatrix projMat;
    atgMatrix::Perspective(60.0f, 1.0f, d_near, d_far, projMat);
    pShadowPass->SetMatirxOfLightViewPojection(projMat * lightViewMatrix);    
    pShadowPass->SetColorDepthTex(pColorTex);
    //pShadowPass->SetAmbientColor(GetVec4Color(YD_COLOR_NAVAJO_WHITE));
    pShadowPass->SetAmbientColor(atgVec4(0.3f, 0.3f, 0.3f, 1.0f));
    pShadowPass->SetLight(lightPos, lightDir);
    pShadowPass->SetSpotParam(30.0f, 15.0f);
    pShadowPass->SetBias(bias);
    pShadowPass->SetOpenglDepthTextureOES(_opengl_depth_texture);
    
    DrawBox(SHADOW_MAPPING_PASS_IDENTITY);
    DrawPlane(SHADOW_MAPPING_PASS_IDENTITY);

    uint32 oldVP[4];
    g_Renderer->GetViewPort(oldVP[0], oldVP[1], oldVP[2], oldVP[3]);
    uint32 newVP[4] = {0, IsOpenGLGraph() ? 0 : oldVP[3] - 200, 200, 200 };
    g_Renderer->SetViewPort(newVP[0], newVP[1], newVP[2], newVP[3]);

    g_Renderer->DrawFullScreenQuad(pColorTex, IsOpenGLGraph());

    g_Renderer->SetViewPort(oldVP[0], oldVP[1], oldVP[2], oldVP[3]);
    
}

void atgSimpleShadowMapping::DrawBox(const char* pPassIdentity /* = NULL */)
{
    g_Renderer->BeginFullQuad();

    atgVec3 startPos;
    float size = 100.0f;

    atgVec3 color = Convert(GetVec3Color(YD_COLOR_DARK_TURQUOISE));

    //>前
    g_Renderer->AddFullQuad( startPos,                            // 1
                            (startPos + atgVec3(size, 0, 0)),        // 2
                            (startPos + atgVec3(0, -size, 0)),       // 3
                            (startPos + atgVec3(size, -size, 0)),    // 4
                             color); 

    //>后
    g_Renderer->AddFullQuad((startPos + atgVec3(0, 0, -size)),       // 5
                            (startPos + atgVec3(0, -size, -size)),   // 6
                            (startPos + atgVec3(size, 0, -size)),    // 7
                            (startPos + atgVec3(size, -size, -size)),// 8
                             color);  

    //>左
    g_Renderer->AddFullQuad((startPos + atgVec3(0, 0, -size)),       // 5
                             startPos,                            // 1
                            (startPos + atgVec3(0, -size, -size)),   // 6
                            (startPos + atgVec3(0, -size, 0)),       // 3
                             color);

    //>右
    g_Renderer->AddFullQuad((startPos + atgVec3(size, 0, 0)),        // 2
                            (startPos + atgVec3(size, 0, -size)),    // 7
                            (startPos + atgVec3(size, -size, 0)),    // 4
                            (startPos + atgVec3(size, -size, -size)),// 8
                             color);

    //>顶
    g_Renderer->AddFullQuad((startPos + atgVec3(0, 0, -size)),       // 5
                            (startPos + atgVec3(size, 0, -size)),    // 7
                             startPos,                            // 1
                            (startPos + atgVec3(size, 0, 0)),        // 2
                            color);


    //>低
    g_Renderer->AddFullQuad((startPos + atgVec3(0, -size, 0)),       // 3
                            (startPos + atgVec3(size, -size, 0)),    // 4
                            (startPos + atgVec3(0, -size, -size)),   // 6
                            (startPos + atgVec3(size, -size, -size)),// 8
                             color);



    g_Renderer->EndFullQuad(pPassIdentity);
}

void atgSimpleShadowMapping::DrawPlane(const char* pPassIdentity /* = NULL */)
{
    g_Renderer->BeginFullQuad();

    atgVec3 startPos(-250.f, -100.0f, 250.0f);
    float size = 500.0f;

    atgVec3 color = Convert(GetVec3Color(YD_COLOR_PERU));

    g_Renderer->AddFullQuad( startPos,                            // 1
                            (startPos + atgVec3(0, 0, -size)),       // 2
                            (startPos + atgVec3(size, 0, 0)),        // 3
                            (startPos + atgVec3(size, 0, -size)),    // 4
                             color);

    g_Renderer->EndFullQuad(pPassIdentity);
}

#include "atgAssimpImport.h"
#include "atgMesh.h"
#include "atgMaterial.h"

MeshTest::MeshTest()
{

}

MeshTest::~MeshTest()
{
}

bool MeshTest::Init()
{
    _light.SetRange(300.0f);
    _light.SetPosition(atgVec3(3.0f, 37.0f, 8.0f));
    _light.SetColor(atgVec3(0.8f, 0.8f, 0.8f));
    _light.SetSpecular(Vec3One);
    _light.SetIntensity(1.0f);
    g_Renderer->AddBindLight(&_light);
    return atgAssimpImport::loadMesh("model/powergirl hero156.fbx", _meshs);
    //return atgBlenderImport::loadMesh("model/modelBuilding_House1.fbx", _meshs);
    //return atgBlenderImport::loadMesh("model/boxA.fbx", _meshs);
}

void MeshTest::Render( class atgCamera* sceneCamera )
{
    g_Renderer->SetMatrix(MD_VIEW, sceneCamera->GetView());
    g_Renderer->SetMatrix(MD_PROJECTION, sceneCamera->GetProj());
    g_Renderer->SetFaceCull(FCM_CW);

    _light.DebugDraw();

    for (std::vector<class atgMesh*>::iterator it = _meshs.begin(); it != _meshs.end(); ++it)
    {
        (*it)->Render(MatrixIdentity);
    }
}

void MeshTest::OnKeyScanDown( Key::Scan keyscan )
{
    atgVec3 position = _light.GetPosition();

    switch (keyscan)
    {
    case Key::Semicolon:
        {
            position.x += 1.0f;
            LOG("new x[%f]\n", position.x);
            break;
        }
    case Key::Apostrophe:
        {
            position.x -= 1.0f;
            LOG("new x[%f]\n", position.x);
            break;
        }
    case Key::Comma: //<
        {
            position.y += 1.0f;
            LOG("new y[%f]\n", position.y);
        }break;
    case Key::Period: //>
        {
            position.y -= 1.f;
            LOG("new y[%f]\n", position.y);

        }break;
    case Key::LeftBracket: //[
        {
            //bias += 0.000001f;
            //LOG("new bias[%f]\n", bias);
            position.z += 1.f;
            LOG("new z[%f]\n", position.z);
        }break;
    case Key::RightBracket: //]
        {
            position.z -= 1.f;
            LOG("new z[%f]\n", position.z);
            //bias -= 0.000001f;
            //LOG("new bias[%f]\n", bias);
        }break;
    
    case Key::N:
        {
            float f = _light.GetLambertFactor();
            f += 0.01f;
            _light.SetLambertFactor(f);
        }break;
    case Key::M:
        {
            float f = _light.GetLambertFactor();
            f -= 0.01f;
            _light.SetLambertFactor(f);
        }break;
    case Key::T:
        {
            static bool drawTangentSpace = false;
            drawTangentSpace = !drawTangentSpace;
            for (uint32 i = 0; i < _meshs.size(); ++i)
            {
                _meshs[i]->SetDrawTanget(drawTangentSpace);
            }
        }break;
    case Key::One:
        {
            for (uint32 i = 0; i < _meshs.size(); ++i)
            {
                for (uint32 j = 0; j < _meshs[i]->_materials.size(); j++)
                {
                    _meshs[i]->_materials[j]->SetPass(atgShaderLibFactory::FindOrCreatePass(BUMP_MAP_PASS_IDENTITY));
                }
            }
        }break;
    case Key::Two:
        {
            for (uint32 i = 0; i < _meshs.size(); ++i)
            {
                for (uint32 j = 0; j < _meshs[i]->_materials.size(); j++)
                {
                    _meshs[i]->_materials[j]->SetPass(atgShaderLibFactory::FindOrCreatePass(LIGHT_TEXTURE_PASS_IDENTITY));
                }
            }
        }break;
    case Key::Three:
        {
            for (uint32 i = 0; i < _meshs.size(); ++i)
            {
                for (uint32 j = 0; j < _meshs[i]->_materials.size(); j++)
                {
                    _meshs[i]->_materials[j]->SetPass(atgShaderLibFactory::FindOrCreatePass(NOT_LIGNTE_TEXTURE_PASS_IDENTITY));
                }
            }
        }break;
    default:
        break;
    }

    _light.SetPosition(position);
}


