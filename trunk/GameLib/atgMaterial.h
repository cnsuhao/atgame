#pragma once

#include "atgMath.h"

class atgTexture;
class atgPass;

enum TextureLayer
{
    TL_DIFFUSE,
    TL_NORMAL,
    TL_BUMPMAP,
    TL_LIGHTMAP,
    TL_SHADOWMAP,
    TL_CUSTOM1,
    TL_CUSTOM2,
    TL_CUSTOM3,

    TL_MAX = 8,
};


class atgMaterial
{
public:
    atgMaterial(void);
    virtual ~atgMaterial(void);

    virtual void                Setup();
    virtual void                Desetup();

    inline void                 SetPass(atgPass* pass);
    inline atgPass*             GetPass();

    inline void                 SetDiffuseColor(const atgVec3& color);
    inline const atgVec3&       GetDiffuseColor() const;

    inline void                 SetSpecularColor(const atgVec3& color);
    inline const atgVec3&       GetSpecularColor() const;

    inline void                 SetAmbientColor(const atgVec3& color);
    inline const atgVec3&       GetAmbientColor() const;

    inline void                 SetEmissiveColor(const atgVec3& color);
    inline const atgVec3&       GetEmissiveColor() const;

    inline void                 SetShininess(float shininess); // if factor < 0 is disable specular.
    inline float                GetShininess() const;

    inline void                 AddTexture(TextureLayer layer, atgTexture* texture);
    inline void                 RemoveTexture(TextureLayer layer);
    inline void                 ClearTexture();

protected:

    struct TextureInfo
    {
        atgTexture* texture;
        uint8       samplerIndex;
        TextureInfo():texture(0),samplerIndex(0) {}
    };

    TextureInfo _textures[TL_MAX];
    atgVec3    _diffuseColor;
    atgVec3    _specularColor;
    atgVec3    _ambientColor;
    atgVec3    _emissiveColor;
    float   _shininess;
    atgPass*  _pass;
};


inline void atgMaterial::SetPass(atgPass* pass)
{
    _pass = pass;
}

inline atgPass* atgMaterial::GetPass()
{
    return _pass;
}


inline void atgMaterial::SetDiffuseColor( const atgVec3& color )
{
    _diffuseColor = color;
}

inline const atgVec3& atgMaterial::GetDiffuseColor() const
{
    return _diffuseColor;
}

inline void atgMaterial::SetSpecularColor( const atgVec3& color)
{
    _specularColor = color;
}

inline const atgVec3& atgMaterial::GetSpecularColor() const
{
    return _specularColor;
}

inline void atgMaterial::SetAmbientColor(const atgVec3& color)
{
    _ambientColor = color;
}

inline const atgVec3& atgMaterial::GetAmbientColor() const
{
    return _ambientColor;
}

inline void atgMaterial::SetEmissiveColor(const atgVec3& color)
{
    _emissiveColor = color;
}

inline const atgVec3& atgMaterial::GetEmissiveColor() const
{
    return _emissiveColor;
}

inline void atgMaterial::SetShininess(float shininess)
{
    _shininess = shininess;
}
inline float atgMaterial::GetShininess() const
{
    return _shininess;
}

inline void atgMaterial::AddTexture(TextureLayer layer, atgTexture* texture)
{
    if (layer < TL_MAX)
    {
        _textures[layer].texture = texture;
        _textures[layer].samplerIndex = (uint8)layer;
    }
}

inline void atgMaterial::RemoveTexture(TextureLayer layer)
{
    if (layer < TL_MAX)
    {
        _textures[layer].texture = 0;
    }
}

inline void atgMaterial::ClearTexture()
{
    for (int i = TL_DIFFUSE; i < TL_MAX; ++i)
    {
        _textures[i].texture = 0;
    }
}

