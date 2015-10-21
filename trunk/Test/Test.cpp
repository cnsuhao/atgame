// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "atgBase.h"
#include "atgRenderer.h"
#include "atgMisc.h"
#include "atgShaderLibrary.h"
#include "atgGame.h"

class atgTextureImpl
{
public:
	void				Func() {}
	virtual void		VirtualFunc() {}
};

class Water
{
public:
    Water(int w, int h);
    ~Water(void);

    void Updata();
    void Drop(float xi, float yi);
    void CopyTo(void* buffer) { memcpy(buffer, buf2, size); }

    const int Width;
    const int Height;
private:
    static const int r = 5;
    static const int h = -1;
    int size;
    float* buf1;
    float* buf2;

    float GetValue(float* buf, int x, int y)
    {
        x = Clamp(x, 0, Width - 1);
        y = Clamp(y, 0, Height - 1);

        return buf[y * Width + x];
    }

    void SetValue(float* buf, int x, int y, float value)
    {
        x = Clamp(x, 0, Width - 1);
        y = Clamp(y, 0, Height - 1);

        buf[y * Width + x] = value;
    }
};

Water::Water(int w, int h)
    : Width(w), Height(h)
{
    size = w * h * sizeof(float);
    buf1 = (float*)malloc(size);
    buf2 = (float*)malloc(size);

    memset(buf1, 0, size);
    memset(buf2, 0, size);
}

void Water::Updata()
{
    for(int y = 0; y < Height; y++)
    {
        int n = y * Width;
        for (int x = 0; x < Width; x++, n++)
        {
            float s = GetValue(buf1, x, y - 1) + GetValue(buf1, x, y + 1) + GetValue(buf1, x - 1, y) + GetValue(buf1, x + 1, y);
            s = (s / 2 - buf2[n]);
            s -= s / 128;
            if (s > 2) s = 2;
            if (s < -2) s = -2;
            buf2[n] = s;
        }
    }
    auto temp = buf1;
    buf1 = buf2;
    buf2 = temp;
}

void Water::Drop(float xi, float yi)
{
    int px = (int)(xi * (Width - 1));
    int py = (int)(yi * (Height - 1));
    for (int j = py - r; j <= py + r; j++)
    {
        for (int i = px - r; i <= px + r; i++)
        {
            float dx = i - px;
            float dy = j - py;
            float a = (float)(1 - (dx * dx + dy * dy) / (r * r));
            if (a > 0 && a <= 1)
            {
                SetValue(buf1, i, j, a * h);
            }
        }
    }
}

atgTexture* g_Texture = NULL;
GLuint g_WaterTextureID;
float* g_WaterHeightData = NULL;

//////////////////////////////////////////////////////////////////////////
class atgRippleShader : public atgShaderLibPass
{
public:
    atgRippleShader() {}
    ~atgRippleShader() {}

    virtual bool			ConfingAndCreate()
    {
        bool rs = false;
        if (IsOpenGLGraph())
        {
            rs = atgPass::Create("shaders/ripple.glvs", "shaders/ripple.glfs");
        }

        return rs;
    }

    void					SetDxDy(float dx, float dy) { _dx = dx; _dy = dy; }
protected:
    virtual void			BeginContext(void* data)
    {
        atgPass::BeginContext(data);

        GL_ASSERT( glActiveTexture(GL_TEXTURE0) );
        GL_ASSERT( glBindTexture(GL_TEXTURE_2D, 1) );
        SetTexture("textureSampler", 0);

        GL_ASSERT( glActiveTexture(GL_TEXTURE0 + 1) );
        GL_ASSERT( glBindTexture(GL_TEXTURE_2D, g_WaterTextureID) );
        SetTexture("waterHeightSampler", 1);

        Vec4 d(_dx, _dy, 1.0f, 1.0f);
        SetFloat4("u_d", d.m);
    }
protected:
    float                   _dx;
    float                   _dy;
};

#define RIPPLE_PASS_IDENTITY "RippleShader"
EXPOSE_INTERFACE(atgRippleShader, atgPass, RIPPLE_PASS_IDENTITY);


atgRippleShader* g_rippleShader = NULL;

class Test : public atgGame
{
    virtual bool            OnInitialize();

    virtual void            OnFrame();

    virtual void            OnPointerDown(uint8 id, int16 x, int16 y)
    {
        if (id == 0)    // leftbutton down.
        {
            lbd = true;
            pWater->Drop(float(x*1.0f/GetWindowWidth()), float(y*1.0f/GetWindowHeight()));
        }
    }

    virtual void            OnPointerMove(uint8 id, int16 x, int16 y)
    {
        if (lbd)
        {
            pWater->Drop(float(x*1.0f/GetWindowWidth()), float(y*1.0f/GetWindowHeight()));
        }
    }

    virtual void            OnPointerUp(uint8 id, int16 x, int16 y)
    {
        if (id == 0)
        {
            lbd = false;
        }
    }

private:
    bool lbd;
    Water* pWater;
};

const float quadData[] = {
    -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f
};

const float textureQuadData[] = {
    -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
};


bool Test::OnInitialize()
{
    float a[3] = {0.0f, 0.0f, 1.0f};
    Quat q;
    QuatFromAxisAngle(a,180.0f, q.m);
    float p1[3] = {1.0f, 0.0f, 0.0f};
    float p2[3]; 
    VecRotate(p1,q.m,p2);

    for (int i = 0; i < 1000; ++i)
    {
        //Vec3 P(522.0f, 20.0f, 86.0f);
        //Vec3 Q(45.f, 99.0f, 15.0f);

        Vec3 P(Random(100.0f), Random(100.0f), Random(100.0f));
        Vec3 Q(Random(100.0f), Random(100.0f), Random(100.0f));

        // 求P到Q的距离
        Vec3 T;
        VecNormalize(Q.m, T.m);
        float l = VecDot(P.m, T.m);
        VecScale(T.m, l, T.m);  // 求投影
        VecSub(P.m, T.m, T.m);  // 求差向量
        float d1 = VecLength(T.m);  // 求长度

        Vec3 T2;
        VecNormalize(Q.m, T2.m);
        VecCross(P.m, T2.m, T2.m);  // 求P x Q
        float d2 = VecLength(T2.m); // 求|P x Q| = |P|*|Q|*sin(a);  && |Q|为1

        //LOG("[%d], d1=%f,d2=%f\n",i, d1, d2);
        AASSERT(FloatEqual(d1, d2, 0.00002f)); // 只能近似到0.00002的精度.
    }


    float matProj[4][4];
    OrthoProject(2, 2, -1.0f, 100.0f, matProj);
    g_Renderer->SetMatrix(MD_PROJECTION, matProj);

    JPEG_Image image;
    JPEG_Loader::Load(&image, "ui/ui1.jpg", false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);
    g_Texture = atgTexture::Create(image.width, image.height, image.bpp, image.imageData); 
    JPEG_Loader::Release(&image);

    pWater = new Water(200, 160);
    lbd = false;

    g_WaterHeightData = new float[pWater->Width * pWater->Height];
    pWater->CopyTo(g_WaterHeightData);
    GL_ASSERT( glGenTextures(1, &g_WaterTextureID) );
    GL_ASSERT( glBindTexture(GL_TEXTURE_2D, g_WaterTextureID) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
    GL_ASSERT( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
    GL_ASSERT( glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, pWater->Width, pWater->Height, 0, GL_LUMINANCE, GL_FLOAT, g_WaterHeightData) );

    g_rippleShader = static_cast<atgRippleShader*>(atgShaderLibFactory::FindOrCreatePass(RIPPLE_PASS_IDENTITY));
    if (NULL == g_rippleShader)
        return false;

    g_rippleShader->SetDxDy(1.0f / float(pWater->Width - 1), 1.0f / float(pWater->Height - 1));
    atgTextureImpl impl;
	impl.Func();
	impl.VirtualFunc();
    return true;
}

void Test::OnFrame()
{
    if (pWater)
    {
        pWater->Updata();
        pWater->CopyTo(g_WaterHeightData);
        GL_ASSERT( glBindTexture(GL_TEXTURE_2D, g_WaterTextureID) );
        GL_ASSERT( glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pWater->Width, pWater->Height, GL_LUMINANCE, GL_FLOAT, g_WaterHeightData) );
    }

    g_Renderer->Clear();
    g_Renderer->BeginFrame();

    //g_Renderer->DrawTexureQuad(&textureQuadData[0], &textureQuadData[5], &textureQuadData[10], &textureQuadData[15], &textureQuadData[3], &textureQuadData[8], &textureQuadData[13], &textureQuadData[18], g_Texture);
    g_Renderer->DrawTexureQuadPass(&textureQuadData[0], &textureQuadData[5], &textureQuadData[10], &textureQuadData[15], &textureQuadData[3], &textureQuadData[8], &textureQuadData[13], &textureQuadData[18], g_Texture, g_rippleShader);

    g_Renderer->EndFrame();
    g_Renderer->Present();
}

int _tmain(int argc, _TCHAR* argv[])
{
    Test t;
    if(t.Initialize(NULL))
    {
        t.Run();
        t.Shutdown();
    }

	return 0;
}

