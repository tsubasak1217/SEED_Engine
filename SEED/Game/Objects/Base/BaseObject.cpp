#include "BaseObject.h"

uint32_t BaseObject::nextID_ = 0;

BaseObject::BaseObject(){
    objectID_ = nextID_++;
    className_ = "BaseObject";
    name_ = "BaseObject";
    Initialize();
}

BaseObject::~BaseObject(){}

void BaseObject::Initialize(){
    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/suzanne.obj");
    model_->UpdateMatrix();
}

void BaseObject::Update(){

    // モデルの更新
    model_->Update();

    // コライダーの更新
    for(auto& collider : colliders_){
        collider->Update();
    }

    // 衝突判定のためのコライダーを渡す
    HandOverColliders();
}

void BaseObject::Draw(){
    model_->Draw();
}

void BaseObject::UpdateMatrix(){
    model_->UpdateMatrix();
}

// コライダーの編集
void BaseObject::EditCollider(){
    if(colliderEditor_){
        colliderEditor_->Edit();
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダー関連
//////////////////////////////////////////////////////////////////////////

void BaseObject::AddCollider(Collider* collider){
    colliders_.emplace_back(std::make_unique<Collider>());
    colliders_.back().reset(collider);
}

void BaseObject::ResetCollider(){
    colliders_.clear();
}

void BaseObject::HandOverColliders(){

    // キャラクターの基本コライダーを渡す
    for(auto& collider : colliders_){
        CollisionManager::AddCollider(collider.get());
    }
}
