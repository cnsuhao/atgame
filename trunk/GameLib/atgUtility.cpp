#include "atgUtility.h"
#include "atgShaderLibrary.h"
#include "atgMisc.h"
#include "atgCamera.h"
#include "atgIntersection.h"


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
    _pCamera->SetPosition(Vec3(0,150,300));

    return true;
}

void atgSampleDrawFrustum::Render()
{
    atgFrustum f;
    f.BuildFrustumPlanes(_pCamera->GetView().m,_pCamera->GetProj().m);
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

    Matrix oldWorld;
    g_Renderer->GetMatrix(oldWorld, MD_WORLD);
    g_Renderer->SetMatrix(MD_WORLD, MatrixIdentity);

    Matrix oldView;
    g_Renderer->GetMatrix(oldView, MD_VIEW);
    Matrix newView(MatrixIdentity);
    Vec3 p(0.0f, 0.0f, 3.5f);
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
        atgMath::LookAt(p.m, Vec3Zero.m, Vec3Up.m, newView.m);
    }
    g_Renderer->SetMatrix(MD_VIEW, newView);

    Matrix oldProj;
    g_Renderer->GetMatrix(oldProj, MD_PROJECTION);
    Matrix newProj;
    atgMath::Perspective(37.5f,1.f,0.1f,500.0f,newProj.m);
    g_Renderer->SetMatrix(MD_PROJECTION, newProj);

    g_Renderer->BeginLine();
    g_Renderer->AddLine(Vec3Zero.m, Vec3Up.m, Vec3Up.m);
    g_Renderer->AddLine(Vec3Zero.m, Vec3Right.m, Vec3Right.m);
    g_Renderer->AddLine(Vec3Zero.m, Vec3Forward.m, Vec3Forward.m);
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

#define RIPPLE_PASS_IDENTITY "RippleShader"
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
        g_Renderer->BindTexture(0, _primitiveTex);
        SetTexture("textureSampler", 0);

        TextureFormat format = _primitiveTex->GetTextureFormat();
        if (format == TF_R4G4B4A4 || 
            format == TF_R8G8B8A8 ||
            format == TF_R5G5B5A1)
        {//>含有aplha通道,开启混合
            g_Renderer->SetBlendFunction(BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA);
        }
    }

    
    g_Renderer->BindTexture(1, _rippleTex);
    SetTexture("waterHeightSampler", 1);

    Vec4 d(_dx, _dy, 1.0f, 1.0f);
    SetFloat4("u_d", d.m);
}

atgSampleWater::atgSampleWater(void):_pWater(0),_pRippleShader(0),_pDyncTexture(0),_pCamera(0)
{

}

atgSampleWater::~atgSampleWater(void)
{
    SAFE_DELETE(_pWater);
    SAFE_DELETE(_pRippleShader);
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
    _pRippleShader = static_cast<atgRippleShader*>(atgShaderLibFactory::FindOrCreatePass(RIPPLE_PASS_IDENTITY));
    if (NULL == _pRippleShader)
    {
        LOG("create water failture, create ripple shader failture.");
        return false;
    }

    _pRippleShader->SetDxDy(1.0f / float(_pWater->Width - 1), 1.0f / float(_pWater->Height - 1));

    _pDyncTexture = new atgTexture();
    if (!_pDyncTexture->Create(_pWater->Width, _pWater->Height, TF_R32F, NULL))
    {
        LOG("create TF_R32F texture failture.");
        return false;
    }
    _pDyncTexture->SetFilterMode(TFM_FILTER_NOT_MIPMAP_ONLY_LINEAR);

    //pRippleShader->SetTex1(g_Texture);
    _pRippleShader->SetRippleTex(_pDyncTexture);

    _pCamera = new atgCamera();
    _pCamera->SetPosition(Vec3(0,0, 150));
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

    Updata();

    if (_pCamera)
    {
        g_Renderer->SetMatrix(MD_VIEW, _pCamera->GetView());
        g_Renderer->SetMatrix(MD_PROJECTION, _pCamera->GetProj()); 
    }

    if (_pRippleShader->IsLost())
    {
        _pRippleShader = static_cast<atgRippleShader*>(atgShaderLibFactory::FindOrCreatePass(RIPPLE_PASS_IDENTITY));
        if (NULL == _pRippleShader)
        {
            return;
        }
    }

    if (pTexture)
    {
        _pRippleShader->SetPrimitiveTex(pTexture);
    }

    g_Renderer->DrawQuadByPass(&textureQuadData[0], &textureQuadData[5], &textureQuadData[10], &textureQuadData[15], &textureQuadData[3], &textureQuadData[8], &textureQuadData[13], &textureQuadData[18], _pRippleShader);
}

void atgSampleWater::OnKeyScanDown( Key::Scan keyscan )
{
    float moveSpeed = 10.5f;

    switch (keyscan)
    {
    case Key::W:
        {
            Vec3 forward = _pCamera->GetForward();
            Vec3 pos = _pCamera->GetPosition();
            forward.Scale(moveSpeed);
            pos.Add(forward.m);
            _pCamera->SetPosition(pos.m);

        }
        break;
    case Key::S:
        {
            Vec3 forward = _pCamera->GetForward();
            Vec3 pos = _pCamera->GetPosition();
            forward.Scale(-moveSpeed);
            pos.Add(forward.m);
            _pCamera->SetPosition(pos.m);
        }
        break;
    case Key::A:
        {
            Vec3 right = _pCamera->GetRight();
            Vec3 pos = _pCamera->GetPosition();
            right.Scale(moveSpeed);
            pos.Add(right.m);
            _pCamera->SetPosition(pos.m);
        }
        break;
    case Key::D:
        {
            Vec3 right = _pCamera->GetRight();
            Vec3 pos = _pCamera->GetPosition();
            right.Scale(-moveSpeed);
            pos.Add(right.m);
            _pCamera->SetPosition(pos.m);
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
            Vec3 forward = _pCamera->GetForward();
            Vec3 pos = _pCamera->GetPosition();
            atgMath::VecScale(forward.m, moveSpeed, forward.m);
            atgMath::VecAdd(pos.m, forward.m, pos.m);
            _pCamera->SetPosition(pos.m);
        }
        else
        {
            Vec3 forward = _pCamera->GetForward();
            Vec3 pos = _pCamera->GetPosition();
            atgMath::VecScale(forward.m, -moveSpeed, forward.m);
            atgMath::VecAdd(pos.m, forward.m, pos.m);
            _pCamera->SetPosition(pos.m);
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
    float moveSpeed = 10.5f;

    switch (keyscan)
    {
    case Key::W:
        {
            Vec3 forward = _pCamera->GetForward();
            Vec3 pos = _pCamera->GetPosition();
            forward.Scale(moveSpeed);
            pos.Add(forward.m);
            _pCamera->SetPosition(pos.m);

        }
        break;
    case Key::S:
        {
            Vec3 forward = _pCamera->GetForward();
            Vec3 pos = _pCamera->GetPosition();
            forward.Scale(-moveSpeed);
            pos.Add(forward.m);
            _pCamera->SetPosition(pos.m);
        }
        break;
    case Key::A:
        {
            Vec3 right = _pCamera->GetRight();
            Vec3 pos = _pCamera->GetPosition();
            right.Scale(moveSpeed);
            pos.Add(right.m);
            _pCamera->SetPosition(pos.m);
        }
        break;
    case Key::D:
        {
            Vec3 right = _pCamera->GetRight();
            Vec3 pos = _pCamera->GetPosition();
            right.Scale(-moveSpeed);
            pos.Add(right.m);
            _pCamera->SetPosition(pos.m);
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
            Vec3 forward = _pCamera->GetForward();
            Vec3 pos = _pCamera->GetPosition();
            forward.Scale(moveSpeed);
            pos.Add(forward.m);
            _pCamera->SetPosition(pos.m);
        }
        else
        {
            Vec3 forward = _pCamera->GetForward();
            Vec3 pos = _pCamera->GetPosition();
            forward.Scale(-moveSpeed);
            pos.Add(forward.m);
            _pCamera->SetPosition(pos.m);
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
                oYaw -= dx * 0.001f;
                oPitch += dy * 0.001f;
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


atgSimpleShowMapping::atgSimpleShowMapping()
{

}

atgSimpleShowMapping::~atgSimpleShowMapping()
{

}
