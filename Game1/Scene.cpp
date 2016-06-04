#include "atgBase.h"
#include "Scene.h"
#include "IniFile.h"
#include "Entity.h"
#include "Component.h"
#include "atgRenderer.h"
#include "atgUtility.h"

using namespace std;

namespace G1
{

Scene::Scene(void)
{
}


Scene::~Scene(void)
{
    {
        std::list<Entity*>::iterator it = _entitys.begin();
        for (std::list<Entity*>::iterator end = _entitys.end(); it != end; ++it)
        {
            SAFE_DELETE(*it);
        }
        _entitys.clear();
    }

    _cameras.clear();
}

bool Scene::Load( const char* fileName )
{
    IniPropertie file;
    if(!file.Load(fileName))
    {
        return false;
    }

    Propertie& prop = *(Propertie*)&file;

    uint32 count = file.GetSectionNumber();
    for (uint32 i = 0; i < count; ++i)
    {
        const string& sectionName =  file.GetSectionName(i);
        file.SetSection(sectionName.c_str());
        string type = prop.GetString("type", "");

        if (type == "")
        {
            LOG("node don't has a type\n");
            continue;
        }

        if (type == "scene_info")
        {
            if(prop.HasKey("render_camera"))
            {
                SetActiveCamera(prop.GetString("render_camera", ""));
            }
            if (prop.HasKey("cull_mode"))
            {
                string mode = prop.GetString("cull_mode", "");
                if (mode == "cw")
                {
                    g_Renderer->SetFaceCull(FCM_CW);
                }
                else if(mode == "ccw")
                {
                    g_Renderer->SetFaceCull(FCM_CCW);
                }
            }
        }
            
        if (type == "node")
        {
            Entity * pEnt = new Entity();
            pEnt->SetName(sectionName);
            if(!pEnt->Load(prop))
            {
                delete pEnt;
                continue;
            }

            std::string parentName = prop.GetString("parent", "");
            if (parentName != "")
            {
                Entity* pParent = FindEntiyByName(parentName.c_str());
                if (pParent)
                {
                    pParent->AddChild(pEnt);
                }
                else
                {
                    AddEntity(pEnt);
                }
            }
            else
            {
                AddEntity(pEnt);
            }
        }
        else if(type == "component")
        {
            Entity* pParent = 0;
            std::string compType = prop.GetString("comp_type", "");
            if (compType == "")
            {
                LOG("component don't has a type\n");
                continue;
            }

            std::string parentName = prop.GetString("parent", "");
            if (parentName == "")
            {
                LOG("component must has a parent\n");
                continue;
            }
            else
            {
                pParent = FindEntiyByName(parentName.c_str());
                if (pParent == NULL)
                {
                    LOG("component can't find a parent[%d]\n", parentName.c_str());
                    continue;
                }
            }

            bool isCamera = false;
            bool isLight = false;
            Component* pComponent = NULL;
            if (compType == "camera")
            {
                pComponent = new CameraComponent();
                isCamera = true;
            }
            else if(compType == "model")
            {
                pComponent = new ModelComponent();
            }
            else if (compType == "light")
            {
                pComponent = new LightCompoent();
                isLight = true;
            }

            if (pComponent)
            {
                pComponent->SetName(sectionName);
                if(!pComponent->Load(prop, pParent))
                {
                    delete pComponent;
                    continue;
                }

                pParent->AddComponent(pComponent);

                if(isCamera)
                {
                    _cameras.push_back(pComponent);
                }

                if (isLight)
                {
                    g_Renderer->AddBindLight((atgLight*)pComponent->GetFunc());
                }
            }
        }
    }

    LOG("=====>load scene success!");
    return true;

}

void Scene::Unload()
{

}

void Scene::AddEntity(Entity* ent)
{
    LOG("=====>AddEntity[%s]\n", ent->GetName().c_str());
    _entitys.push_back(ent);
}

void Scene::RemoveEntity( Entity* ent )
{
    std::list<Entity*>::iterator it = _entitys.begin();
    for (std::list<Entity*>::iterator end = _entitys.end(); it != end; ++it)
    {
        if (*it == ent)
        {
            _entitys.erase(it);
            return;
        }
    }
}

Entity* Scene::FindEntiyByName( const char* name )
{
    std::list<Entity*>::iterator it = _entitys.begin();
    for (std::list<Entity*>::iterator end = _entitys.end(); it != end; ++it)
    {
        Entity* ent = (*it);
        if (ent->GetName() == name)
        {
            return ent;
        }
        else
        {
            Entity* child = ent->FindEntiyByName(name);
            if (child)
                return child;
        }
    }

    return 0;
}

void Scene::Render()
{
    atgCamera* pRenderCamera = GetRenderCamera(_cameraName);
    if (pRenderCamera)
    {
        g_Renderer->SetMatrix(MD_VIEW, pRenderCamera->GetView());
        g_Renderer->SetMatrix(MD_PROJECTION, pRenderCamera->GetProj());

        std::list<Entity*>::iterator it = _entitys.begin();
        for (std::list<Entity*>::iterator end = _entitys.end(); it != end; ++it)
        {
            (*it)->Render();
        }
    }
}

atgCamera* Scene::GetRenderCamera( const std::string& cameraName )
{
    if (cameraName == "" && _cameras.size() > 0)
    {
        return (atgCamera*)((*_cameras.begin())->GetFunc());
    }

    std::list<Component*>::iterator it = _cameras.begin();
    for (std::list<Component*>::iterator end = _cameras.end(); it != end; ++it)
    {
        if ((*it)->GetName() == cameraName)
        {
            return (atgCamera*)((*it)->GetFunc());
        }
    }

    return 0;
}

void Scene::OnKeyScanDown( Key::Scan keyscan )
{
    atgCamera* pCamera = NULL;
    std::list<Component*>::iterator it = _cameras.begin();
    for (std::list<Component*>::iterator end = _cameras.end(); it != end; ++it)
    {
        if ((*it)->GetName() == _cameraName)
        {
            pCamera = (atgCamera*)((*it)->GetFunc());

            atgFlyCamera flyCam;
            if(flyCam.Create(pCamera))
            {
                flyCam.OnKeyScanDown(keyscan);
            }
            return;
        }
    }
}

}