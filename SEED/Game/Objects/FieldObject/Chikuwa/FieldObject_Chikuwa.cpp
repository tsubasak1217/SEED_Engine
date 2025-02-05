#include "FieldObject_Chikuwa.h"

FieldObject_Chikuwa::FieldObject_Chikuwa(){
    // 名前関連の初期化
    className_ = "FieldObject_Chikuwa";
    name_ = "Chikuwa";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // 薄い黄色
    model_->meshColor_[0] = { 1.0f,1.0f,0.0f,1.0f };
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}

void FieldObject_Chikuwa::Update(){

    // タッチされて時間がたてば重力を適用
    if(isTouched_){
        dropWaitTime_ -= ClockManager::DeltaTime();
        if(dropWaitTime_ <= 0.0f){
            dropSpeed_ = 0.0f;
            isTouched_ = false;
            SetIsApplyGravity(true);
            // オレンジにする
        }
    }

    // 範囲外に出たら削除
    if(GetWorldTranslate().y < 0.0f){
        removeFlag_ = true;
    }

    FieldObject::Update();
}

// プレイヤーと接触したらタッチされたフラグを立てる
void FieldObject_Chikuwa::OnCollision( BaseObject* other, ObjectType objectType){
    other;
    if(objectType == ObjectType::Player){
        model_->meshColor_[0] = { 1.0f,0.5f,0.0f,1.0f };
        isTouched_ = true;
    }
}
