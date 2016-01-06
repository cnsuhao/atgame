#pragma once

#include "atgMisc.h"
#include "atgRenderer.h"

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

#define VERTEXCOLOR_PASS_IDENTITY "VertexColorShader"
EXPOSE_INTERFACE(atgShaderVertexColor, atgPass, VERTEXCOLOR_PASS_IDENTITY);

//////////////////////////////////////////////////////////////////////////
class atgShaderSolidColor : public atgShaderLibPass
{
public:
    atgShaderSolidColor();
    ~atgShaderSolidColor();

    virtual bool            ConfingAndCreate();

    void                    SetSolidColor(const Vec3& SolidColor) { m_SolidColor = SolidColor; }
protected:
    virtual void            BeginContext(void* data);
protected:
    Vec3                    m_SolidColor;
};

#define SOLIDCOLOR_PASS_IDENTITY "SolidColorShader"
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

#define NOT_LIGNTE_TEXTURE_PASS_IDENTITY "NotLightTextureShader"
EXPOSE_INTERFACE(atgShaderNotLighteTexture, atgPass, NOT_LIGNTE_TEXTURE_PASS_IDENTITY);


//////////////////////////////////////////////////////////////////////////
class atgShaderLightTexture : public atgShaderNotLighteTexture
{
public:
    atgShaderLightTexture();
    ~atgShaderLightTexture();

    virtual bool            ConfingAndCreate();
protected:
    virtual void            BeginContext(void* data);
};

#define LIGHT_TEXTURE_PASS_IDENTITY "LightTextureShader"
EXPOSE_INTERFACE(atgShaderLightTexture, atgPass, LIGHT_TEXTURE_PASS_IDENTITY);



//////////////////////////////////////////////////////////////////////////
//>¡Ÿ ±≤‚ ‘ÀÆ≤®shader
class atgRippleShader : public atgShaderLibPass
{
public:
    atgRippleShader();
    ~atgRippleShader();

    virtual bool			ConfingAndCreate();

    void					SetDxDy(float dx, float dy) { _dx = dx; _dy = dy; }

    void                    SetTex1(atgTexture* pTex) { _t1 = pTex; }
    void                    SetTex2(atgTexture* pTex) { _t2 = pTex; }

protected:
    virtual void			BeginContext(void* data);
protected:
    float                   _dx;
    float                   _dy;

    atgTexture*             _t1;
    atgTexture*             _t2;
};

#define RIPPLE_PASS_IDENTITY "RippleShader"
EXPOSE_INTERFACE(atgRippleShader, atgPass, RIPPLE_PASS_IDENTITY);
