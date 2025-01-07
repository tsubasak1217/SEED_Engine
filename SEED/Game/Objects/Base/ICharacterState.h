#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../Game/Objects/Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "InputManager.h"
#include "CollisionManaer/CollisionManager.h"
#include "CollisionManaer/ColliderEditor.h"

class ICharacterState{
public:
    ICharacterState() = default;
    ICharacterState(const std::string& stateName, BaseCharacter* character){ stateName; character; }
    virtual ~ICharacterState() = default;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void EndFrame(){
        ManageState();
    };
    virtual void Initialize(const std::string& stateName,BaseCharacter* character){ 
        pCharacter_ = character; 
        stateName_ = stateName;
        colliderEditor_ = std::make_unique<ColliderEditor>(stateName_,pCharacter_->GetWorldMatPtr());
    }

public:// コライダー関連
    void HandOverColliders(){
        for(auto& collider : colliders_){
            CollisionManager::AddCollider(collider.get());
        }
    }

    // ステート名からコライダーを読み込む
    void InitColliders(){
        colliders_.clear();
        ColliderEditor::LoadColliders(stateName_ + ".json", pCharacter_,&colliders_);
    }

    void InitColliders(const std::string& fileName){
        colliders_.clear();
        ColliderEditor::LoadColliders(fileName,pCharacter_,&colliders_);

    }

    void UpdateColliders(){
        for(auto& collider : colliders_){
            collider->Update();
        }
        HandOverColliders();
    }

protected:
    virtual void ManageState() = 0;

protected:
    std::string stateName_ = "";
    BaseCharacter* pCharacter_ = nullptr;
    std::vector<std::unique_ptr<Collider>> colliders_;
    std::unique_ptr<ColliderEditor> colliderEditor_ = nullptr;
    bool isChangeState_ = false;

};