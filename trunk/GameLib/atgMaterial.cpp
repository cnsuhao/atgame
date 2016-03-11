#include "atgBase.h"
#include "atgMaterial.h"
#include "atgRenderer.h"

atgMaterial::atgMaterial(void)
{
    _pass = NULL;
    _emissiveColor = Vec3Zero;
    _diffuseColor = Vec3One;
    _specularColor = atgVec3(1.0f, 0.0f, 0.0f);
    _ambientColor = atgVec3(1.0f, 1.0f, 1.0f);
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

void atgMaterial::Setup()
{
    if (!_textures.empty())
    {
        for (size_t i = 0; i < _textures.size() && i < 8; i++)
        {
            _textures[i]->Bind(i);
        }
    }

    if (_pass != 0 && !_pass->IsLost())
    {
        _pass->Bind();
    }
}


void atgMaterial::Desetup()
{
    if (_pass)
    {
        _pass->Unbind();
    }

    if (!_textures.empty())
    {
        for (size_t i = 0; i < _textures.size() && i < 8; i++)
        {
            _textures[i]->Unbind(i);
        }
    }
}