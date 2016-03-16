#include "atgBase.h"
#include "atgMaterial.h"
#include "atgRenderer.h"

const char* sampler_name[TL_MAX] = { 
                                        SAMPLER_DIFFUSE,
                                        SAMPLER_NORMAL,
                                        SAMPLER_BUMPMAP,
                                        SAMPLER_LIGHTMAP,
                                        SAMPLER_SHADOWMAP,
                                        SAMPLER_CUSTOM1,
                                        SAMPLER_CUSTOM2,
                                        SAMPLER_CUSTOM3
};

atgMaterial::atgMaterial(void)
{
    _pass = NULL;
    _emissiveColor = Vec3Zero;
    _diffuseColor = Vec3One;
    _specularColor = atgVec3(0.1f, 0.1f, 0.1f);
    _ambientColor = atgVec3(1.0f, 1.0f, 1.0f);
    _shininess = 64.0f;
}

atgMaterial::~atgMaterial(void)
{
    ClearTexture();
    
    if (_pass)
    {
        //SAFE_DELETE(_pass);
        _pass = NULL;
    }
}

void atgMaterial::Setup()
{
    if (_pass != 0 && !_pass->IsLost())
    {
        _pass->Bind();

        for (int i = 0; i < TL_MAX; ++i)
        {
            if (_textures[i].texture != 0)
            {
                uint8 index = _pass->GetTexture(sampler_name[i]);
                if (index < TL_MAX)
                {
                    _textures[i].samplerIndex = index;
                }
                
                _textures[i].texture->Bind(_textures[i].samplerIndex);
                _pass->SetTexture(sampler_name[i], _textures[i].samplerIndex);
            }
        }
    }
}


void atgMaterial::Desetup()
{
    if (_pass)
    {
        _pass->Unbind();

        for (int i = 0; i < TL_MAX; ++i)
        {
            if (_textures[i].texture != 0)
            {
                _textures[i].texture->Unbind(_textures[i].samplerIndex);
            }
        }
    }
}