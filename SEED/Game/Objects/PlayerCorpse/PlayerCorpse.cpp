#include "PlayerCorpse.h"

PlayerCorpse::PlayerCorpse(){
    className_ = "PlayerCorpse";
}

PlayerCorpse::~PlayerCorpse(){}

void PlayerCorpse::Initialize(){
    model_ = std::make_unique<Model>("dinosaur_corpse.obj");
    model_->UpdateMatrix();

    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    LoadColliders(ObjectType::PlayerCorpse);
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(ObjectType::Egg);
        collider->AddSkipPushBackType(ObjectType::Editor);
        collider->AddSkipPushBackType(ObjectType::Player);
    }

    SetIsApplyGravity(true);
}

void PlayerCorpse::Update(){
    BaseObject::Update();
    EditCollider();
}