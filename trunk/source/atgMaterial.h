#ifndef _ATG_MATERIAL_H_
#define _ATG_MATERIAL_H_

#include "atgMath.h"

class atgTexture;
class atgPass;

class atgMaterial
{
public:
    atgMaterial(void);
    virtual ~atgMaterial(void);

    virtual bool                Load(const char* fileName);

    virtual void                Setup();
    virtual void                Desetup();

    inline void                 SetPass(atgPass* pass);
    inline atgPass*             GetPass();

    inline void                 SetDiffuseColor(const Vec3& color);
    inline const Vec3&          GetDiffuseColor() const;

    inline void                 SetSpecularColor(const Vec3& color);
    inline const Vec3&          GetSpecularColor() const;

    inline void                 SetAmbientColor(const Vec3& color);
    inline const Vec3&          GetAmbientColor() const;

    inline void                 SetEmissiveColor(const Vec3& color);
    inline const Vec3&          GetEmissiveColor() const;

    inline void                 SetShininess(float shininess); // if factor < 0 is disable specular.
    inline float                GetShininess() const;

    inline void                 AddTexture(atgTexture* texture);
    inline void                 RemoveTexture(atgTexture* texture);
    inline void                 ClearTexture();

protected:
    typedef std::vector<atgTexture*> TextureVector;
    TextureVector _textures;
    Vec3    _diffuseColor;
    Vec3    _specularColor;
    Vec3    _ambientColor;
    Vec3    _emissiveColor;
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


inline void atgMaterial::SetDiffuseColor( const Vec3& color )
{
    _diffuseColor = color;
}

inline const Vec3& atgMaterial::GetDiffuseColor() const
{
    return _diffuseColor;
}

inline void atgMaterial::SetSpecularColor( const Vec3& color)
{
    _specularColor = color;
}

inline const Vec3& atgMaterial::GetSpecularColor() const
{
    return _specularColor;
}

inline void atgMaterial::SetAmbientColor(const Vec3& color)
{
    _ambientColor = color;
}

inline const Vec3& atgMaterial::GetAmbientColor() const
{
    return _ambientColor;
}

inline void atgMaterial::SetEmissiveColor(const Vec3& color)
{
    _emissiveColor = color;
}

inline const Vec3& atgMaterial::GetEmissiveColor() const
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

inline void atgMaterial::AddTexture(atgTexture* texture)
{
    if (texture)
        _textures.push_back(texture);
}

inline void atgMaterial::RemoveTexture(atgTexture* texture)
{
    TextureVector::iterator itEnd = _textures.end();
    for (TextureVector::iterator it = _textures.begin();
         it != itEnd; ++it)
    {
        if(*it ==  texture)
        {
            _textures.erase(it);
            return;
        }
    }
}

inline void atgMaterial::ClearTexture()
{
    _textures.clear();
}


#endif // _ATG_MATERIAL_H_
