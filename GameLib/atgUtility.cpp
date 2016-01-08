#include "atgUtility.h"
#include "atgShaderLibrary.h"
#include "atgMisc.h"


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

atgSampleWater::atgSampleWater(void):_pWater(0),_pRippleShader(0),_pDyncTexture(0)
{

}

atgSampleWater::~atgSampleWater(void)
{
    SAFE_DELETE(_pWater);
    SAFE_DELETE(_pDyncTexture);
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
