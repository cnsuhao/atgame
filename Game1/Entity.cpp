#include "Entity.h"

namespace G1
{

Entity::Entity(void)
{
   _parent = 0;
}


Entity::~Entity(void)
{
    {
        std::list<Entity*>::iterator it = _chlidren.begin();
        for (std::list<Entity*>::iterator end = _chlidren.end(); it != end; ++it)
        {
            SAFE_DELETE(*it);
        }
        _chlidren.clear();
    }

    {
        std::list<Component*>::iterator it = _components.begin();
        for (std::list<Component*>::iterator end = _components.end(); it != end; ++it)
        {
            SAFE_DELETE(*it);
        }
        _components.clear();
    }
}

bool Entity::Load( const Propertie& prop )
{
    if (!prop.HasKey("position_x"))
    {
        return false;
    }

    _position.x = prop.GetFloat("position_x", 0);
    _position.y = prop.GetFloat("position_y", 0);
    _position.z = prop.GetFloat("position_z", 0);

    if (prop.HasKey("rotation_x"))
    {
        _rotation.x = prop.GetFloat("rotation_x", 0);
        _rotation.y = prop.GetFloat("rotation_y", 0);
        _rotation.z = prop.GetFloat("rotation_z", 0);
    }

    _scale.Set(1.0f, 1.0f, 1.0f);
    if (prop.HasKey("scalc_x"))
    {
        _scale.x = prop.GetFloat("scalc_x", 1.f);
        _scale.y = prop.GetFloat("scalc_y", 1.f);
        _scale.z = prop.GetFloat("scalc_z", 1.f);
    }


    return true;
}


void Entity::SetPosition( const atgVec3& position )
{
    _position = position;
    _localChangeMatrix = true;
}

void Entity::SetRotation( const atgVec3& rotation )
{
    _rotation = rotation;
    _localChangeMatrix = true;
}

void Entity::SetScale( const atgVec3& scale )
{
    _scale = scale;
    _localChangeMatrix = true;
}

void Entity::AddChild( Entity* ent )
{
    AASSERT(ent->_parent == 0);

    _chlidren.push_back(ent);
    ent->_parent = this;
}

void Entity::RemoveChild( Entity* ent )
{
    std::list<Entity*>::iterator it = _chlidren.begin();
    for (std::list<Entity*>::iterator end = _chlidren.end(); it != end; ++it)
    {
        if (*it == ent)
        {
            _chlidren.erase(it);
            return;
        }
    }
}

void Entity::AddComponent( Component* comp )
{
    _components.push_back(comp);
}

void Entity::RemoveComponent( Component* comp )
{
    std::list<Component*>::iterator it = _components.begin();
    for (std::list<Component*>::iterator end = _components.end(); it != end; ++it)
    {
        if (*it == comp)
        {
            _components.erase(it);
            return;
        }
    }
}

Entity* Entity::FindEntiyByName(const char* name)
{
    std::list<Entity*>::iterator it = _chlidren.begin();
    for (std::list<Entity*>::iterator end = _chlidren.end(); it != end; ++it)
    {
        if ((*it)->GetName() == name)
        {
            return (*it);
        }
    }

    return 0;
}

const atgMatrix& Entity::GetWorldMatrix()
{
    if (_parent)
    {
        if (_parentChangeMatrix)
        {
            _cacheWorldMatrix = _parent->GetWorldMatrix() * GetLocalMatrix();
            _parentChangeMatrix = false;
            
            {
                std::list<Component*>::iterator it = _components.begin();
                for (std::list<Component*>::iterator end = _components.end(); it != end; ++it)
                {
                    (*it)->OnWorldMatrixChange();
                }
            }
            
        }
        return _cacheWorldMatrix;
    }
    else
    {
        return GetLocalMatrix();
    }
}

const atgMatrix& Entity::GetLocalMatrix()
{
    if (_localChangeMatrix)
    {
        atgMatrix temp;
        _cacheLocalMatrix.RotationZXY(_rotation.z, _rotation.x, _rotation.y);
        _cacheLocalMatrix = _cacheLocalMatrix * temp.Scaling(_scale);
        _cacheLocalMatrix.SetColumn3(3, _position);
        _localChangeMatrix = false;
        
        {
            std::list<Entity*>::iterator it = _chlidren.begin();
            for (std::list<Entity*>::iterator end = _chlidren.end(); it != end; ++it)
            {
                (*it)->_parentChangeMatrix = true;
            }
        }

        {
            std::list<Component*>::iterator it = _components.begin();
            for (std::list<Component*>::iterator end = _components.end(); it != end; ++it)
            {
                (*it)->OnWorldMatrixChange();
            }
        }
    }
    return _cacheLocalMatrix;
}

void Entity::Update( float dt )
{

}

void Entity::Render()
{
    {
        std::list<Entity*>::iterator it = _chlidren.begin();
        for (std::list<Entity*>::iterator end = _chlidren.end(); it != end; ++it)
        {
            (*it)->Render();
        }
    }

    {
        std::list<Component*>::iterator it = _components.begin();
        for (std::list<Component*>::iterator end = _components.end(); it != end; ++it)
        {
            (*it)->Render();
        }
    }

}


}