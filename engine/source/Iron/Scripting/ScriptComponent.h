#pragma once

#include "../EntityComponentSystem/Component.h"
#include "../Physics/Collision.h"

struct Collision;

using ScriptPointer = intptr_t;

class ScriptComponent : public Component
{
public:
    explicit ScriptComponent(EntityID ownerEntityID) : Component(ownerEntityID)
    {
        _scriptPointer = 0;
    }

    void Init(const char* fullName, ScriptPointer scriptPointer);
    ScriptPointer GetScriptPointer() const;

    void OnUpdate();
    void OnCreate();
    void OnDestroy();
    void OnFixedUpdate();
    void OnLateUpdate();
    void OnEnabled();
    void OnDisabled();

    void OnCollisionEnter(Collision collision);
    void OnCollisionExit(Collision collision);

    void OnMouseOver();
    void OnMouseEnter();
    void OnMouseExit();
    void OnMousePressed();
    void OnMouseJustPressed();
    void OnMouseJustReleased();

private:
    ScriptPointer _scriptPointer;
    const char* _fullName;

    bool hasCollision = false;
    Collision collisionStay;
};
