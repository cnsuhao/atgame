#include "Component.h"
#include "Entity.h"
#include "atgAssimpImport.h"
#include "atgMaterial.h"
#include "atgShaderLibrary.h"

namespace G1
{


Component::Component(void):_funcPtr(0),_parent(0)
{
     
}


Component::~Component(void)
{
}

CameraComponent::CameraComponent()
{

}

CameraComponent::~CameraComponent()
{

}

bool CameraComponent::Load(const Propertie& prop, Entity* ent)
{
    _parent = ent;

    if (!prop.HasKey("forward_x"))
    {
        return false;
    }

    atgVec3 forward;
    forward.x = prop.GetFloat("forward_x", 0.0f);
    forward.y = prop.GetFloat("forward_y", 0.0f);
    forward.z = prop.GetFloat("forward_z", -1.0f);


    atgVec3 upAxis = Vec3Up;
    if (prop.HasKey("up_x"))
    {
        upAxis.x = prop.GetFloat("up_x", 0);
        upAxis.y = prop.GetFloat("up_y", 1.0f);
        upAxis.z = prop.GetFloat("up_z", 0);
    }

    
    SetLookAt(_parent->GetPosition(), _parent->GetPosition() + forward, upAxis);

    if (prop.HasKey("fov_y"))
    {
         SetFov(prop.GetFloat("fov_y", 60.f));
    }

    if (prop.HasKey("aspect"))
    {
        SetAspect(prop.GetFloat("aspect", 1.0f));
    }

    if (prop.HasKey("clip_near"))
    {
        SetClipNear(prop.GetFloat("clip_near", 1.0f));
    }

    if (prop.HasKey("clip_far"))
    {
        SetClipFar(prop.GetFloat("clip_far", 1000.0f));
    }

    return true;
}

bool CameraComponent::SetProp(const Propertie& prop, uint8 index)
{
    return false;
}

void* CameraComponent::GetFunc()
{
    return static_cast<atgCamera*>(this);
}


ModelComponent::ModelComponent():_model(0)
{

}

ModelComponent::~ModelComponent()
{
    SAFE_DELETE(_model);
}

bool ModelComponent::Load( const Propertie& prop, Entity* ent )
{
    if (_model != 0)
    {
        return true;
    }

    _parent = ent;

    if (!prop.HasKey("model_path"))
    {
        return false;
    }

    _model = new atgModel();
    if(!atgAssimpImport::loadModel(prop.GetString("model_path", "").c_str(), _model))
    {
        SAFE_DELETE(_model);
        return false;
    }

    if (prop.HasKey("shader"))
    {
        atgPass* pass = atgShaderLibFactory::FindOrCreatePass(prop.GetString("shader", "").c_str());
        if (pass)
        {
            for (uint32 i = 0; i < _model->_meshs.size(); ++i)
            {
                for (uint32 j = 0; j < _model->_meshs[i]->_materials.size(); j++)
                {
                    _model->_meshs[i]->_materials[j]->SetPass(pass);
                }
            }
        }
    }


    return true;
}

bool ModelComponent::SetProp( const Propertie& prop, uint8 index )
{
    return false;
}

void* ModelComponent::GetFunc()
{
    return 0;
}

void ModelComponent::Render()
{
    if (_model)
    {
        _model->render(_parent->GetWorldMatrix());
    }
}


LightCompoent::LightCompoent():_pLight(0)
{

}

LightCompoent::~LightCompoent()
{
    SAFE_DELETE(_pLight);
}

bool LightCompoent::Load( const Propertie& prop, Entity* ent )
{
    if (_pLight != 0)
    {
        return false;
    }

    _parent = ent;

    if (!prop.HasKey("light_type"))
    {
        return false;
    }

    std::string lightType = prop.GetString("light_type", "");
    if (lightType == "directional")
    {
        atgDirectionalLight* pDirLight = new atgDirectionalLight();
        _pLight = pDirLight;

        _direction.x = prop.GetFloat("direction_x", 0);
        _direction.y = prop.GetFloat("direction_y", 0);
        _direction.z = prop.GetFloat("direction_z", -1.0f);
    }
    else if(lightType == "point")
    {
        atgPointLight* _pPointLight = new atgPointLight();
        _pLight = _pPointLight;

        _pPointLight->SetRange(prop.GetFloat("range", 500.f));
    }
    else if (lightType == "Spot")
    {
        atgSpotLight* _pSpotLight = new atgSpotLight();
         _pLight = _pSpotLight;

         _direction.x = prop.GetFloat("direction_x", 0);
         _direction.y = prop.GetFloat("direction_y", 0);
         _direction.z = prop.GetFloat("direction_z", -1.0f);

         _pSpotLight->SetOuterCone(prop.GetFloat("direction_x", 30.0f));
         _pSpotLight->SetInnerCone(prop.GetFloat("direction_x", 20.0f));
    }
    else
    {
        return false;
    }

    atgVec3 color;
    color.x = prop.GetFloat("color_r", 1.0f);
    color.y = prop.GetFloat("color_g", 1.0f);
    color.z = prop.GetFloat("color_b", 1.0f);
    _pLight->SetColor(color);

    atgVec3 specular;
    specular.x = prop.GetFloat("specular_r", 0.0f);
    specular.y = prop.GetFloat("specular_g", 1.0f);
    specular.z = prop.GetFloat("specular_b", 0.0f);
    _pLight->SetSpecular(specular);

    _pLight->SetIntensity(prop.GetFloat("intensity", 1.0f));
    _pLight->SetLambertFactor(prop.GetFloat("lambert_factor", 0.0f));

    UpdateDirectionAndPosition();

    return true;
}

bool LightCompoent::SetProp( const Propertie& prop, uint8 index )
{
    return false;
}

void* LightCompoent::GetFunc()
{
    return _pLight;
}

void LightCompoent::Render()
{
    if (_pLight)
    {
        g_Renderer->SetMatrix(MD_WORLD, MatrixIdentity);

        _pLight->DebugDraw();
    }
}

void LightCompoent::OnWorldMatrixChange()
{
    UpdateDirectionAndPosition();
}

void LightCompoent::UpdateDirectionAndPosition()
{
    LightType type = _pLight->GetType();
    if (type == LT_Directional)
    {
        atgDirectionalLight* pDirLight = (atgDirectionalLight*)_pLight;
        atgVec3 newDir = _parent->GetWorldMatrix().TransfromNormal(_direction);
        pDirLight->SetDirection(newDir);
    }
    else if(type == LT_Point)
    {
        atgPointLight* pPointLight = (atgPointLight*)_pLight;
        atgVec3 newPos;
        _parent->GetWorldMatrix().GetColumn3(3 , newPos);
        pPointLight->SetPosition(newPos);
    }
    else if (type == LT_Spot)
    {
        atgSpotLight* pSpotLight = (atgSpotLight*)_pLight;
        atgVec3 newDir = _parent->GetWorldMatrix().TransfromNormal(_direction);
        pSpotLight->SetDirection(newDir);
        atgVec3 newPos;
        _parent->GetWorldMatrix().GetColumn3(3 , newPos);
        pSpotLight->SetPosition(newPos);
    }
}

}