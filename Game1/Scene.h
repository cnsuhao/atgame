#pragma once

#include <list>


class atgCamera;

namespace G1
{

    class Entity;
    class Component;

    class Scene
    {
    public:
        Scene(void);
        ~Scene(void);
    
        bool Load(const char* fileName);
        void Unload();

        void AddEntity(Entity* ent);
        void RemoveEntity(Entity* ent);
        Entity* FindEntiyByName(const char* name);

        void Render();

        void SetActiveCamera(const std::string name) { _cameraName = name; }
        const std::string& GetActiveName() { return _cameraName; }

        void OnKeyScanDown(Key::Scan keyscan);

    protected:
        atgCamera* GetRenderCamera(const std::string& cameraName);

    private:
        std::list<Entity*> _entitys;
        std::list<Component*> _cameras;
        std::string _cameraName;
    };
}



