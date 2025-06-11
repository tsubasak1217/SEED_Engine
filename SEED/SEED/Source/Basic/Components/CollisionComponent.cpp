#include "CollisionComponent.h"


CollisionComponent::CollisionComponent(GameObject* pOwner, const std::string& tagName) 
    : IComponent(pOwner,tagName){
}

void CollisionComponent::Initialize(const std::string& fileName, ObjectType objectType){
    // コライダーの初期化
    InitColliders(fileName, objectType);

#ifdef _DEBUG
    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(owner_->GetName(), owner_);
#endif // _DEBUG
}

void CollisionComponent::BeginFrame(){
    // コライダーの開始処理
    for(auto& collider : colliders_){
        collider->BeginFrame();
    }

    if(isHandOverColliders_){
        HandOverColliders();
    }
}

void CollisionComponent::Update(){
    // コライダーの更新
    for(auto& collider : colliders_){
        collider->Update();
    }
}

void CollisionComponent::Draw(){
    // コライダーの描画
    if(isColliderVisible_){
        for(auto& collider : colliders_){
            collider->Draw();
        }
    }
}

void CollisionComponent::EndFrame(){
    // コライダーの更新
    EraseCheckColliders();
    // コライダーをCollisionManagerに渡す
    HandOverColliders();
}

void CollisionComponent::Finalize(){
}

//////////////////////////////////////////////////////////////////////////
// コライダーの追加
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::AddCollider(Collider* collider){
    colliders_.emplace_back(std::make_unique<Collider>());
    colliders_.back().reset(collider);
}


//////////////////////////////////////////////////////////////////////////
// コライダーの読み込み
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::LoadColliders(ObjectType objectType){
    // コライダーの読み込み
    ColliderEditor::LoadColliders(owner_->GetIdolName() + ".json", owner_, &colliders_);

    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }
}

void CollisionComponent::LoadColliders(const std::string& fileName, ObjectType objectType){
    // コライダーの読み込み
    ColliderEditor::LoadColliders(fileName, owner_, &colliders_);
    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }
}



//////////////////////////////////////////////////////////////////////////
// コライダーの初期化
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::InitColliders(ObjectType objectType){
    colliders_.clear();
    LoadColliders(objectType);
}

void CollisionComponent::InitColliders(const std::string& fileName, ObjectType objectType){
    colliders_.clear();
    LoadColliders(fileName, objectType);
}



//////////////////////////////////////////////////////////////////////////
// コライダーの削除チェック
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::EraseCheckColliders(){

    // 終了した要素の削除
    for(int i = 0; i < colliders_.size(); ++i){
        if(colliders_[i]->IsEndAnimation()){
            colliders_.erase(colliders_.begin() + i);
            --i;
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// コライダーのリセット
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::ResetCollider(){
    colliders_.clear();
}


//////////////////////////////////////////////////////////////////////////
// コライダーをCollisionManagerに渡す
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::HandOverColliders(){

    // 衝突情報の保存・初期化
    preIsCollide_ = isCollide_;
    isCollide_ = false;

    if(!isHandOverColliders_){
        return;
    }
    // キャラクターの基本コライダーを渡す
    for(auto& collider : colliders_){
        CollisionManager::AddCollider(collider.get());
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダーの判定スキップリストの追加
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::AddSkipPushBackType(ObjectType skipType){
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(skipType);
    }
}