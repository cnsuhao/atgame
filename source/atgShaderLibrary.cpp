#include "atgBase.h"
#include "atgShaderLibrary.h"
#include "atgMaterial.h"
#include "atgLight.h"

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
        g_Renderer->InsertCachePass(Identity, libPass);
        return libPass;
    }
    return pass;
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
    SetFloat4("vec_solid_color", m_SolidColor.m);
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
    //if (m_Texture)
    //{
    //    atgPass::SetTexture("ssss", m_Texture);
    //}
}

void atgShaderNotLighteTexture::SetTexture( atgTexture* texture )
{
    m_Texture = texture;
}

atgShaderLightTexture::atgShaderLightTexture()
{

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
    // 如果到时候没有设置光， new 一个临时光让shader运行正常也不错。

    // set MVMatrix
    Matrix WorldMatrix;
    g_Renderer->GetMatrix(&WorldMatrix.m, MD_WORLD);

    Matrix WorldViewMatrix;
    g_Renderer->GetMatrix(&WorldViewMatrix.m, MD_VIEW);
    WorldViewMatrix.Concatenate(WorldMatrix);
    SetMatrix4x4("mat_world_view", WorldViewMatrix.m);

    Matrix WorldViewMatrixInverseTranspose(WorldViewMatrix);
    WorldViewMatrixInverseTranspose.Inverse();
    WorldViewMatrixInverseTranspose.Transpose();
    SetMatrix4x4("mat_world_view_inverse_transpose", WorldViewMatrixInverseTranspose.m);

    Vec4 lightData0;
    Vec4 lightData1;
    const bindLights& lights = g_Renderer->GetBindLights();
    if (!lights.empty())
    {
        // set light intensity
        atgLight* light = lights[0];
        switch (light->GetType())
        {
        case LT_Directional:
            {
                atgDirectionalLight* directionalLight = static_cast<atgDirectionalLight*>(light);
                // set direction;
                Vec3 lightDirection = WorldViewMatrixInverseTranspose.Transfrom(directionalLight->GetDirection());
                lightDirection.Normalize();
                SetFloat3("u_lightDirection", lightDirection.m);
            }break;
        case LT_Point:
            {
                atgPointLight* pointLight = static_cast<atgPointLight*>(light);
                // set position;
                Vec3 lightPosition = WorldViewMatrix.Transfrom(pointLight->GetPosition());
                SetFloat3("u_lightPosition", lightPosition.m);
                // set range
                lightData1.y = pointLight->GetRange();
            }break;
        case LT_Spot:
            {
                // set position;
                atgSpotLight* spotLight = static_cast<atgSpotLight*>(light);
                Vec3 lightPosition = WorldViewMatrix.Transfrom(spotLight->GetPosition());
                SetFloat3("u_lightPosition", lightPosition.m);
                // set direction;
                Vec3 lightDirection = WorldViewMatrixInverseTranspose.Transfrom(spotLight->GetDirection());
                lightDirection.Normalize();
                SetFloat3("u_lightDirection", lightDirection.m);
                // set range
                lightData1.y = spotLight->GetRange();
                // outer cone angle and inner cone angle
                lightData1.z = DegreesToRadians(Clamp(spotLight->GetOuterCone(),0.0f,90.0f)); 
                lightData1.w = DegreesToRadians(Clamp(spotLight->GetInnerCone(),0.0f,90.0f));

                if (lightData1.w > lightData1.z)
                    Swap(lightData1.z, lightData1.w);

                lightData1.z = cosf(lightData1.z);
                lightData1.w = cosf(lightData1.w);
            }break;
        default:
            break;
        }

        // set light type
        lightData0.x = static_cast<float>(light->GetType());
        // set light intensity
        lightData1.x = light->GetIntensity();

        SetFloat4("u_lightData0", lightData0.m);
        SetFloat4("u_lightData1", lightData1.m);

        // set light diffuse color
        SetFloat3("u_lightDiffuse", light->GetColor().m);
        // set light specular color
        SetFloat3("u_lightSpecular", light->GetSpecular().m);
    }

    atgMaterial* material = g_Renderer->GetBindMaterial();
    if (material)
    {
        SetFloat3("u_materialAmbient", material->GetAmbientColor().m);
        SetFloat3("u_materialDiffuse", material->GetDiffuseColor().m);
        SetFloat3("u_materialSpecular", Vec3One.m);
        Vec4 materialData0(material->GetShininess(), 0.0f, 0.0f, 0.0f);
        SetFloat4("u_materialData0", materialData0.m);
    }

    SetFloat3("u_globalAmbient", g_Renderer->GetGlobalAmbientColor().m);
}
