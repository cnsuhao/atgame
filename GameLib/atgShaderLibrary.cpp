#include "atgBase.h"
#include "atgShaderLibrary.h"
#include "atgMaterial.h"

const int IPASS_CONFIG_FAILD = -2;

atgPass* atgShaderLibFactory::FindOrCreatePass(const char* Identity, int *pReturnCode)
{
    atgPass* pass = g_Renderer->FindCachePass(Identity);
    if (!pass)
    {
        atgShaderLibPass* libPass = reinterpret_cast<atgShaderLibPass*>(CreateInterface(Identity, pReturnCode));
        if (!libPass)
        {
            return NULL;
        }

        if (!libPass->ConfingAndCreate())
        {
            if (pReturnCode)
                *pReturnCode = IPASS_CONFIG_FAILD;

            SAFE_DELETE(pass);
            return NULL;
        }
        libPass->SetName(Identity);
        g_Renderer->InsertCachePass(Identity, libPass);
        return libPass;
    }
    
    if (pass->IsLost())
    {
        atgShaderLibPass* libPass = reinterpret_cast<atgShaderLibPass*>(pass);
        if (!libPass->ConfingAndCreate())
        {
            return NULL;
        }
    }

    return pass;
}

atgShaderVertexColor::atgShaderVertexColor()
{

}

atgShaderVertexColor::~atgShaderVertexColor()
{

}

bool atgShaderVertexColor::ConfingAndCreate()
{
    bool rs = false;

    if (IsOpenGLGraph())
    {
        rs = atgPass::Create("shaders/vertex_color.glvs", "shaders/vertex_color.glfs");
    }
    else
    {
        rs = atgPass::Create("shaders/vertex_color.dxvs", "shaders/vertex_color.dxps");
    }

    return rs;
}


atgShaderSolidColor::atgShaderSolidColor()
{

}

atgShaderSolidColor::~atgShaderSolidColor()
{

}

bool atgShaderSolidColor::ConfingAndCreate()
{
    bool rs = false;
    
    if (IsOpenGLGraph())
    {
        rs = atgPass::Create("shaders/solid_color.glvs", "shaders/solid_color.glfs");
    }
    else
    {
        rs = atgPass::Create("shaders/solid_color.dxvs", "shaders/solid_color.dxps");
    }

    SetSolidColor(Vec3One);

    return rs;
}

void atgShaderSolidColor::BeginContext( void* data )
{
    atgPass::BeginContext(data);
    // set solid color.
    SetFloat4("vec_solid_color", Vec3ToVec4(m_SolidColor));
}


atgShaderNotLighteTexture::atgShaderNotLighteTexture():m_Texture(NULL)
{

}

atgShaderNotLighteTexture::~atgShaderNotLighteTexture()
{

}

bool atgShaderNotLighteTexture::ConfingAndCreate()
{
    bool rs = false;
    if (IsOpenGLGraph())
    {
        rs = atgPass::Create("shaders/not_light_texture.glvs", "shaders/not_light_texture.glfs");
    }
    else
    {
        rs = atgPass::Create("shaders/not_light_texture.dxvs", "shaders/not_light_texture.dxps");
    }

    return rs;
}

void atgShaderNotLighteTexture::BeginContext( void* data )
{
    atgPass::BeginContext(data);
    // set texture Uniform 
    if (m_Texture)
    {
        TextureFormat format = m_Texture->GetTextureFormat();
        if (format == TF_R4G4B4A4 || 
            format == TF_R8G8B8A8 ||
            format == TF_R5G5B5A1)
        {//>含有aplha通道,开启混合
            g_Renderer->SetBlendFunction(BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA);
        }

        //> 以后修改为
        //> SamplerIndex idx = atgPass::GetTextureSamplerIndex("textureSampler");
        //> g_Renderer->BindTexture(idx, pTexture);


    }
}

void atgShaderNotLighteTexture::SetTexture( atgTexture* texture )
{
    m_Texture = texture;
}

atgShaderLightTexture::atgShaderLightTexture()
{
    _usingTempLight = false;
    _material = NULL;

    _tempLight.SetRange(500.0f);
    _tempLight.SetPosition(atgVec3(0.0f, 50.0f, 50.0f));
    _tempLight.SetColor(atgVec3(0.8f, 0.8f, 0.8f));
    _tempLight.SetSpecular(Vec3One);
}

atgShaderLightTexture::~atgShaderLightTexture()
{

}


bool atgShaderLightTexture::ConfingAndCreate()
{
    bool rs = false;
    if (IsOpenGLGraph())
    {
        rs = atgPass::Create("shaders/light_texture.glvs", "shaders/light_texture.glfs");
    }
    else
    {
        rs = atgPass::Create("shaders/light_texture.dxvs", "shaders/light_texture.dxps");
    }

    return rs;
}

void atgShaderLightTexture::BeginContext( void* data )
{
    
    atgShaderNotLighteTexture::BeginContext(data);

    // set MVMatrix
    atgMatrix WorldMatrix;
    g_Renderer->GetMatrix(WorldMatrix, MD_WORLD);
    SetMatrix4x4(UNF_M_W, WorldMatrix);

    atgMatrix ViewMatrix;
    g_Renderer->GetMatrix(ViewMatrix, MD_VIEW);
    SetMatrix4x4(UNF_M_V, ViewMatrix);
    atgVec3 cameraPosition;
    ViewMatrix.GetColumn3(3,cameraPosition);
    atgVec4 temp(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z, 0);
    temp = ViewMatrix.Transpose() * temp;
    cameraPosition = Vec4ClipToVec3(temp);

    atgMatrix WorldMatrixInverseTranspose(WorldMatrix);
    WorldMatrixInverseTranspose.Inverse();
    WorldMatrixInverseTranspose.Transpose();
    SetMatrix4x4(UNF_M_WIT, WorldMatrixInverseTranspose);

    atgVec4 lightData0;
    atgVec4 lightData1;
    // 如果到时候没有设置光， new 一个临时光让shader运行正常也不错。
    if (g_Renderer->GetBindLights().empty())
    {
        g_Renderer->AddBindLight(&_tempLight);
        _usingTempLight = true;
    }

    const bindLights& lights = g_Renderer->GetBindLights();
    if (!lights.empty())
    {
        char uniformNameBuff[64];
        int numberLight = atgMath::Min((int)lights.size(), 8);
        SetInt("u_numberOfLights", numberLight);
        for (int i = 0; i < numberLight; ++i)
        {
            // set light intensity
            atgLight* light = lights[i];
            switch (light->GetType())
            {
            case LT_Directional:
                {
                    atgDirectionalLight* directionalLight = static_cast<atgDirectionalLight*>(light);
                    // set direction;
                    atgVec3 lightDirection = directionalLight->GetDirection();
                    lightDirection.Normalize();
                    sprintf(uniformNameBuff, "%s[%d]", "u_lightDirection", i);
                    SetFloat3(uniformNameBuff, lightDirection);
                }break;
            case LT_Point:
                {
                    atgPointLight* pointLight = static_cast<atgPointLight*>(light);
                    // set position;
                    atgVec3 lightPosition = pointLight->GetPosition();
                    sprintf(uniformNameBuff, "%s[%d]", "u_lightPosition", i);
                    SetFloat3(uniformNameBuff, lightPosition);
                    // set range
                    lightData1.y = pointLight->GetRange();
                }break;
            case LT_Spot:
                {
                    // set position;
                    atgSpotLight* spotLight = static_cast<atgSpotLight*>(light);
                    atgVec3 lightPosition = spotLight->GetPosition();
                    sprintf(uniformNameBuff, "%s[%d]", "u_lightPosition", i);
                    SetFloat3(uniformNameBuff, lightPosition);
                    // set direction;
                    atgVec3 lightDirection = spotLight->GetDirection();
                    lightDirection.Normalize();
                    sprintf(uniformNameBuff, "%s[%d]", "u_lightDirection", i);
                    SetFloat3(uniformNameBuff, lightDirection);
                    // set range
                    lightData1.y = spotLight->GetRange();
                    // outer cone angle and inner cone angle
                    lightData1.z = atgMath::DegreesToRadians(atgMath::Clamp(spotLight->GetOuterCone(),0.0f,90.0f)); 
                    lightData1.w = atgMath::DegreesToRadians(atgMath::Clamp(spotLight->GetInnerCone(),0.0f,90.0f));

                    if (lightData1.w > lightData1.z)
                        atgMath::Swap(lightData1.z, lightData1.w);

                    lightData1.z = cosf(lightData1.z);
                    lightData1.w = cosf(lightData1.w);
                }break;
            default:
                break;
            }

            // set light type
            lightData0.x = static_cast<float>(light->GetType());
            lightData0.y = light->GetLambertFactor();
            // set light intensity
            lightData1.x = light->GetIntensity();

            sprintf(uniformNameBuff, "%s[%d]", "u_lightData0", i);
            SetFloat4(uniformNameBuff, lightData0);
            sprintf(uniformNameBuff, "%s[%d]", "u_lightData1", i);
            SetFloat4(uniformNameBuff, lightData1);

            // set light diffuse color
            sprintf(uniformNameBuff, "%s[%d]", "u_lightDiffuse", i);
            SetFloat3(uniformNameBuff, light->GetColor());
            // set light specular color
            sprintf(uniformNameBuff, "%s[%d]", "u_lightSpecular", i);
            SetFloat3(uniformNameBuff, light->GetSpecular());
        }
    }

    if (_material)
    {
        SetFloat3("u_materialAmbient", _material->GetAmbientColor());
        SetFloat3("u_materialDiffuse", _material->GetDiffuseColor());
        SetFloat3("u_materialSpecular", _material->GetSpecularColor());
        atgVec4 materialData0(_material->GetShininess(), 0.0f, 0.0f, 0.0f);
        SetFloat4("u_materialData0", materialData0);
    }

    SetFloat3("u_globalAmbient", g_Renderer->GetGlobalAmbientColor());

    SetFloat3("u_eyePosition", cameraPosition);
}

void atgShaderLightTexture::EndContext( void* data )
{
    atgShaderNotLighteTexture::EndContext(data);

    if (_usingTempLight)
    {
        g_Renderer->RemoveBindLight(&_tempLight);
    }

    _usingTempLight = false;
}

atgShaderBumpMap::atgShaderBumpMap()
{

}

atgShaderBumpMap::~atgShaderBumpMap()
{

}

bool atgShaderBumpMap::ConfingAndCreate()
{
    bool rs = false;
    if (IsOpenGLGraph())
    {
        rs = atgPass::Create("shaders/bumpmap.glvs", "shaders/bumpmap.glfs");
    }
    else
    {
        rs = atgPass::Create("shaders/bumpmap.dxvs", "shaders/bumpmap.dxps");
    }

    return rs;
}
