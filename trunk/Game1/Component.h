#pragma once

#include "atgCamera.h"
#include "atgRenderer.h"
#include "atgMesh.h"
#include "atgModel.h"
#include "atgLight.h"

#include "Propertie.h"

namespace G1
{

class Entity;
class Component
{
public:
    Component(void);
    virtual ~Component(void);

    virtual bool Load(const Propertie& prop, Entity* ent) = 0;
    virtual bool SetProp(const Propertie& prop, uint8 index) = 0;
    virtual void* GetFunc() = 0;

    virtual void Update(float dt) {}
    virtual void Render() {}


    void SetName(const std::string name) { _name = name; }
    const std::string& GetName() { return _name; }

    virtual void OnWorldMatrixChange() {}

protected:
    void* _funcPtr;
    Entity* _parent;
    std::string _name;
};


class CameraComponent : public Component, public atgCamera
{
public:
    CameraComponent();
    ~CameraComponent();

    virtual bool Load(const Propertie& prop, Entity* ent);
    virtual bool SetProp(const Propertie& prop, uint8 index);
    virtual void* GetFunc();
};

class ModelComponent : public Component
{
public:
    ModelComponent();
    ~ModelComponent();

    virtual bool Load(const Propertie& prop, Entity* ent);
    virtual bool SetProp(const Propertie& prop, uint8 index);
    virtual void* GetFunc();

    virtual void Render();

private:
    atgModel* _model;
    atgPass* _pass;
};


class LightCompoent : public Component
{
public:
    LightCompoent();
    ~LightCompoent();

    virtual bool Load(const Propertie& prop, Entity* ent);
    virtual bool SetProp(const Propertie& prop, uint8 index);
    virtual void* GetFunc();

    virtual void Render();

    virtual void OnWorldMatrixChange();
protected:
    void    UpdateDirectionAndPosition();
private:
    atgLight* _pLight;

    atgVec3 _direction;
};


}