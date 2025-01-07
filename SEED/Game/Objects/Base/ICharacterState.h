#pragma once
#include <vector>
#include "../Game/Objects/Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "InputManager.h"
#include "CollisionManaer/CollisionManager.h"

class ICharacterState{
public:
    ICharacterState() = default;
    ICharacterState(BaseCharacter* character){ character; }
    virtual ~ICharacterState() = default;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Initialize(BaseCharacter* character){ pCharacter_ = character; }

public:// コライダー追加
    void HandOverColliders(){
        for(auto& collider : colliders_){
            CollisionManager::AddCollider(collider.get());
        }
    }

protected:
    virtual void ManageState() = 0;

protected:
    BaseCharacter* pCharacter_ = nullptr;
    std::vector<std::unique_ptr<Collider>> colliders_;

};