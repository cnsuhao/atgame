#pragma once

//> �˹��߿��atgMisc��������atgMisc��̫�����������
//> �˹��߿������������ǿ

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

