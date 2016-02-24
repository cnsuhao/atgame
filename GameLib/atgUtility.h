#pragma once

//> �˹��߿��atgMisc��������atgMisc��̫�����������
//> �˹��߿������������ǿ

#include "atgBase.h"
#include "atgMath.h"


//////////////////////////////////////////////////////////////////////////
//> ������������
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

//> ��׵����ʾ
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

//> view port ����
class atgSamlpeViewPortDrawAxis
{
public:
    atgSamlpeViewPortDrawAxis();
    ~atgSamlpeViewPortDrawAxis();

    void                Render(class atgCamera* sceneCamera);
};

//> ˮ��
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

//> ��Ⱦ������
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

//> Ӱ��
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
    class atgTexture* pPixelDepthTex;     //>��Ϊ��Ⱦ����ʹ��
    class atgTexture* pNormalDepthTex;    //>Ĭ�ϵ���Ȼ���

    Vec3  lightPos;
    Vec3  lightDir;
    Matrix lightViewMatrix;
    float  bias;
    float d_far;
    float d_near;
};