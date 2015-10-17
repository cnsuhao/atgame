#include "atgBase.h"
#include "atgMaterial.h"
#include "atgRenderer.h"

atgMaterial::atgMaterial(void)
{
    _pass = NULL;
    _emissiveColor = Vec3Zero;
    _diffuseColor = Vec3One;
    _specularColor = Vec3(1.0f, 0.0f, 0.0f);
    _ambientColor = Vec3(1.0f, 1.0f, 1.0f);
    _shininess = 1.0f;
}

atgMaterial::~atgMaterial(void)
{
    if (!_textures.empty())
    {
        TextureVector::iterator it = _textures.begin();
        TextureVector::iterator itEnd = _textures.end();
        while(it != itEnd)
        {
            //SAFE_DELETE(*it);
            ++it;
        }
    }
    _textures.clear();
    
    if (_pass)
    {
        //SAFE_DELETE(_pass);
        _pass = NULL;
    }
}

bool atgMaterial::Load( const char* fileName )
{
    return true;
}

void atgMaterial::Setup()
{
    g_Renderer->BindMaterial(this);

    if (!_textures.empty())
    {
        for (size_t i = 0; i < _textures.size() && i < 8; i++)
        {
            g_Renderer->BindTexture((uint8)i, _textures[i]);
        }
    }

    if (_pass)
    {
        g_Renderer->BindPass(_pass);
    }
}


void atgMaterial::Desetup()
{
    if (_pass)
    {
        g_Renderer->BindPass(NULL);
    }

    if (!_textures.empty())
    {
        for (size_t i = 0; i < _textures.size() && i < 8; i++)
        {
            g_Renderer->BindTexture((uint8)i, NULL);
        }
    }

    g_Renderer->BindMaterial(NULL);
}