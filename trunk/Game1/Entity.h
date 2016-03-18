#pragma once

#include "atgBase.h"
#include "atgMath.h"
#include "Component.h"
#include "Propertie.h"

namespace G1
{
    class Entity
    {
    public:
        Entity(void);
        ~Entity(void);

        bool Load(const Propertie& prop);

        void SetName(const std::string name) { _name = name; }
        const std::string& GetName() { return _name; }

        void SetPosition(const atgVec3& position);
        const atgVec3& GetPosition() const { return _position; }

        void SetRotation(const atgVec3& rotation);
        const atgVec3& GetRotation() const { return _rotation; }

        void SetScale(const atgVec3& scale);
        const atgVec3& GetScale() const { return _scale; }

        void AddChild(Entity* ent);
        void RemoveChild(Entity* ent);

        void AddComponent(Component* comp);
        void RemoveComponent(Component* comp);
        Entity* FindEntiyByName(const char* name);

        const atgMatrix& GetWorldMatrix();
        const atgMatrix& GetLocalMatrix();

        void Update(float dt);
        void Render();

        std::string _name;

        atgVec3 _position;
        atgVec3 _rotation;
        atgVec3 _scale;

        bool      _parentChangeMatrix;
        bool      _localChangeMatrix;
        atgMatrix _cacheWorldMatrix;
        atgMatrix _cacheLocalMatrix;
        
        Entity* _parent;
        std::list<Entity*> _chlidren;

        std::list<Component*> _components;
    };
}
