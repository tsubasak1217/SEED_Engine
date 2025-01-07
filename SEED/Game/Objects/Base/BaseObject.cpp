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
    // コライダーの初期化
    InitColliders(ObjectType::None);
}

void BaseObject::Update(){

    // モデルの更新
    model_->Update();

    // コライダーの更新
    UpdateColliders();
}

void BaseObject::Draw(){
    model_->Draw();
}

void BaseObject::EndFrame(){}

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

// 衝突処理
void BaseObject::OnCollision(const BaseObject* other, ObjectType objectType){
    other;
    objectType;
}


// コライダーの読み込み
void BaseObject::LoadColliders(ObjectType objectType){
    // コライダーの読み込み
    ColliderEditor::LoadColliders(className_ + ".json",this,&colliders_);

    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }
}

// コライダーの初期化
void BaseObject::InitColliders(ObjectType objectType){
    colliders_.clear();
    LoadColliders(objectType);
}

// コライダーの更新
void BaseObject::UpdateColliders(){

    // コライダーの更新
    for(auto& collider : colliders_){
        collider->Update();
    }

    // 終了した要素の削除
    for(int i = 0; i < colliders_.size(); ++i){
        if(colliders_[i]->IsEndAnimation()){
            colliders_.erase(colliders_.begin() + i);
            --i;
        }
    }

    // コライダーを渡す
    HandOverColliders();
}
