#pragma once

//> 此工具库和atgMisc的区别是atgMisc不太依赖于引擎而
//> 此工具库和引擎依赖很强

class atgSampleWater
{
public:
    atgSampleWater(void);
    ~atgSampleWater(void);

    bool                Create(int w, int h);
    void                Updata();
    void                Drop(float xi, float yi);
    void                Render(class atgTexture* pTexture = 0);
private:
    class Water* _pWater;
    class atgRippleShader* _pRippleShader;
    class atgTexture* _pDyncTexture;
};

