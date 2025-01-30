#include "Egg.h"

// State
#include "Egg/State/EggState_Follow.h"

//Engine
#include "ClockManager.h"

#include "MyMath.h"

// Object
#include "Model.h"
#include "Player/Player.h"

// Manager
#include "../adapter/json/JsonCoordinator.h"
#include "Manager/EggManager.h"

Egg::Egg(BaseObject* _player)
    :BaseCharacter(),
    player_(_player){
    className_ = "Egg";
    isApplyGravity_ = false;
}

Egg::~Egg(){}

void Egg::Initialize(){
    // model 読み込み
    model_ = std::make_unique<Model>("egg.obj");

    currentState_ = std::make_unique<EggState_Follow>(this,this->player_);

    JsonCoordinator::RegisterItem("Egg","weight",weight_);

    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    LoadColliders(ObjectType::Egg);
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(ObjectType::Egg);
        collider->AddSkipPushBackType(ObjectType::Editor);
        collider->AddSkipPushBackType(ObjectType::Enemy);
        collider->AddSkipPushBackType(ObjectType::Player);
    }
    isApplyGravity_ = false;
}

void Egg::Update(){
    BaseCharacter::Update();
    EditCollider();
}

void Egg::OnCollision([[maybe_unused]] const BaseObject* other,ObjectType objectType){
    BaseObject::OnCollision(other, objectType);

    if(objectType == ObjectType::Field){

        if(!isThrown_){
            return;
        }

        // 衝突した場所の真下に落ちる
        // XZ を固定
        SetTranslateX(prePos_.x);
        SetTranslateZ(prePos_.z);

        velocity_.y = -MyMath::Length(this->GetWorldTranslate() - prePos_);
    }
}

void Egg::Break(){
    //Player を spawn させる
    Player* player = dynamic_cast<Player*>(player_);
    if(player_){
        Vector3 spawnPos = this->GetWorldTranslate();
        spawnPos.y += 1.0f;
        player->Spawn(spawnPos);
    }

    isBreak_ = true;
}
