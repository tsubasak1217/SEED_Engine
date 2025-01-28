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

    weight_ = 4.f;
}

void PlayerCorpse::Update(){
    BaseObject::Update();
    EditCollider();

    // 地面ではなく,y==0に落下したら削除
    if(this->GetWorldTranslate().y <= 0.0f){
        isAlive_ = false;
    }
}