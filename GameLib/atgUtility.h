#pragma once

//> 此工具库和atgMisc的区别是atgMisc不太依赖于引擎而
//> 此工具库和引擎依赖很强

#include "atgBase.h"
#include "atgMath.h"


//////////////////////////////////////////////////////////////////////////
//> 方便操作的相机
class atgFlyCamera
{
public:
    atgFlyCamera();
    ~atgFlyCamera();

    bool                Create(class atgCamera* pCamera);

    void                OnKeyScanDown(Key::Scan keyscan);
    void                OnPointerDown(uint8 id, int16 x, int16 y);
    void                OnPointerMove(uint8 id, int16 x, int16 y);
    void                OnPointerUp(uint8 id, int16 x, int16 y);
    
    class atgCamera*    GetCamera() { return _pCamera; }
private:
    class atgCamera* _pCamera;
    bool  button[2];
};

//> 视椎体显示
class atgSampleDrawFrustum
{
public:
    atgSampleDrawFrustum();
    ~atgSampleDrawFrustum();

    bool                Create();
    void                Render();
private:
    class atgCamera* _pCamera;
};

//> view port 绘制
class atgSamlpeViewPortDrawAxis
{
public:
    atgSamlpeViewPortDrawAxis();
    ~atgSamlpeViewPortDrawAxis();

    void                Render(class atgCamera* sceneCamera);
};

//> 水波
class atgSampleWater
{
public:
    atgSampleWater(void);
    ~atgSampleWater(void);

    bool                Create(int w, int h);
    void                Updata();
    void                Drop(float xi, float yi);
    void                Render(class atgTexture* pTexture = 0);


    void                OnKeyScanDown(Key::Scan keyscan);
    void                OnPointerDown(uint8 id, int16 x, int16 y);
    void                OnPointerMove(uint8 id, int16 x, int16 y);
    void                OnPointerUp(uint8 id, int16 x, int16 y);

private:
    class Water* _pWater;
    class atgTexture* _pDyncTexture;
    class atgCamera* _pCamera;
    bool  btn[2];
};

//> 渲染到纹理
class atgSampleRenderToTextrue
{
public:
    atgSampleRenderToTextrue();
    ~atgSampleRenderToTextrue();

    bool                Create();

    void                OnDrawBegin();
    void                OnDrawEnd();
private:
    class atgTexture* pDepthTex;
    class atgTexture* pColorTex;
    class atgRenderTarget* pRT;
};

//> 影子
class atgSimpleShadowMapping
{
public:
    atgSimpleShadowMapping();
    ~atgSimpleShadowMapping();

    bool                Create();

    void                Render(class atgCamera* sceneCamera);

    void                OnKeyScanDown(Key::Scan keyscan);

    void                OnPointerMove(uint8 id, int16 x, int16 y);
protected:
    void                DrawDepthTex(class atgCamera* sceneCamera);
    void                DrawSceen(class atgCamera* sceneCamera);

    void                DrawBox(const char* pPassIdentity = NULL);
    void                DrawPlane(const char* pPassIdentity = NULL);
private:
    class atgRenderTarget* pRT;
    class atgTexture* pPixelDepthTex;     //>作为渲染纹理使用
    class atgTexture* pNormalDepthTex;    //>默认的深度缓存

    Vec3  lightPos;
    Vec3  lightDir;
    Matrix lightViewMatrix;
    float  bias;
    float d_far;
    float d_near;
};