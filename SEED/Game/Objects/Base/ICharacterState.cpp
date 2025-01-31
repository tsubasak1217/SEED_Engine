#include "ICharacterState.h"


//////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
//////////////////////////////////////////////////////////////////////////
void ICharacterState::EndFrame(){
    EraseCheckColliders();
    ManageState();
};

//////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////
void ICharacterState::Initialize(const std::string& stateName, BaseCharacter* character){
    pCharacter_ = character;
    stateName_ = stateName;
    colliderEditor_ = std::make_unique<ColliderEditor>(stateName_, pCharacter_);
}

//////////////////////////////////////////////////////////////////////////
// コライダーを渡す関数
//////////////////////////////////////////////////////////////////////////
void ICharacterState::HandOverColliders(){
    for(auto& collider : colliders_){
        CollisionManager::AddCollider(collider.get());
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダーの初期化
//////////////////////////////////////////////////////////////////////////
void ICharacterState::InitColliders(ObjectType objectType){
    colliders_.clear();
    ColliderEditor::LoadColliders(stateName_ + ".json", pCharacter_, &colliders_);

    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }
}

void ICharacterState::InitColliders(const std::string& fileName, ObjectType objectType){
    colliders_.clear();
    ColliderEditor::LoadColliders(fileName, pCharacter_, &colliders_);

    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }

}


//////////////////////////////////////////////////////////////////////////
// コライダーの更新
//////////////////////////////////////////////////////////////////////////
void ICharacterState::EraseCheckColliders(){

    // 終了した要素の削除
    for(int i = 0; i < colliders_.size(); ++i){
        if(colliders_[i]->IsEndAnimation()){
            colliders_.erase(colliders_.begin() + i);
            --i;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 前フレームのコライダーを今のフレームの状態と同期
//////////////////////////////////////////////////////////////////////////
void ICharacterState::DiscardPreCollider(){
    for(auto& collider : colliders_){
        collider->DiscardPreCollider();
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダーの判定スキップリストの追加
//////////////////////////////////////////////////////////////////////////
void ICharacterState::AddSkipPushBackType(ObjectType skipType){
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(skipType);
    }
}
