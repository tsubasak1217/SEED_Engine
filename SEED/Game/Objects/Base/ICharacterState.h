#pragma once
#include <memory>
#include <string>
#include <vector>
#include <Physics.h>
#include "../Game/Objects/Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "InputManager.h"
#include "CollisionManaer/CollisionManager.h"
#include "CollisionManaer/ColliderEditor.h"

class ICharacterState
{
public:
    ICharacterState() = default;
    ICharacterState(const std::string &stateName, BaseCharacter *character)
    {
        stateName;
        character;
    }
    virtual ~ICharacterState() = default;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void EndFrame();
    virtual void Initialize(const std::string &stateName, BaseCharacter *character);

public: // コライダー関連
    virtual void HandOverColliders();
    void InitColliders(ObjectType objectType);
    void InitColliders(const std::string &fileName, ObjectType objectType);
    void EraseCheckColliders();
    void DiscardPreCollider();
    void AddSkipPushBackType(ObjectType skipType);

protected:
    virtual void ManageState() = 0;

protected:
    std::string stateName_ = "";
    BaseCharacter *pCharacter_ = nullptr;
    std::vector<std::unique_ptr<Collider>> colliders_;
    std::unique_ptr<ColliderEditor> colliderEditor_ = nullptr;
    bool isChangeState_ = false;
};