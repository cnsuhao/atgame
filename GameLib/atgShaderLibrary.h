#pragma once

#include "atgMisc.h"
#include "atgRenderer.h"
#include "atgLight.h"

class atgMaterial;

class atgShaderLibPass : public atgPass
{
public:
    virtual bool            ConfingAndCreate() = 0;
};

class atgShaderLibFactory
{
public:
    static atgPass*         FindOrCreatePass(const char* Identity, int *pReturnCode = NULL);
};
// atgShaderLibFactory::FindOrCreatePass pReturnCode Value 
extern const int IPASS_CONFIG_FAILD;  // -2

//////////////////////////////////////////////////////////////////////////
class atgShaderVertexColor : public atgShaderLibPass
{
public:
    atgShaderVertexColor();
    ~atgShaderVertexColor();

    virtual bool            ConfingAndCreate();
};

#define VERTEXCOLOR_PASS_IDENTITY "atgVertexColorShader"
EXPOSE_INTERFACE(atgShaderVertexColor, atgPass, VERTEXCOLOR_PASS_IDENTITY);

//////////////////////////////////////////////////////////////////////////
class atgShaderSolidColor : public atgShaderLibPass
{
public:
    atgShaderSolidColor();
    ~atgShaderSolidColor();

    virtual bool            ConfingAndCreate();

    void                    SetSolidColor(const atgVec3& SolidColor) { m_SolidColor = SolidColor; }
protected:
    virtual void            BeginContext(void* data);
protected:
    atgVec3                 m_SolidColor;
};

#define SOLIDCOLOR_PASS_IDENTITY "atgSolidColorShader"
EXPOSE_INTERFACE(atgShaderSolidColor, atgPass, SOLIDCOLOR_PASS_IDENTITY);

//////////////////////////////////////////////////////////////////////////
class atgShaderNotLighteTexture : public atgShaderLibPass
{
public:
    atgShaderNotLighteTexture();
    ~atgShaderNotLighteTexture();

    virtual bool            ConfingAndCreate();

    void                    SetTexture(atgTexture* texture);
protected:
    virtual void            BeginContext(void* data);

protected:
    atgTexture*             m_Texture;
};

#define NOT_LIGNTE_TEXTURE_PASS_IDENTITY "atgNotLightTextureShader"
EXPOSE_INTERFACE(atgShaderNotLighteTexture, atgPass, NOT_LIGNTE_TEXTURE_PASS_IDENTITY);


//////////////////////////////////////////////////////////////////////////
class atgShaderLightTexture : public atgShaderNotLighteTexture
{
public:
    atgShaderLightTexture();
    ~atgShaderLightTexture();

    virtual bool            ConfingAndCreate();

    void                    SetMaterial(atgMaterial* pMaterial) { _material = pMaterial; }
protected:
    virtual void            BeginContext(void* data);

    virtual void            EndContext(void* data);

    atgPointLight           _tempLight;
    bool                    _usingTempLight;
    atgMaterial*            _material;
};

#define LIGHT_TEXTURE_PASS_IDENTITY "atgLightTextureShader"
EXPOSE_INTERFACE(atgShaderLightTexture, atgPass, LIGHT_TEXTURE_PASS_IDENTITY);

//class atgShaderLightNormalTexture : public  atgShaderNotLighteTexture