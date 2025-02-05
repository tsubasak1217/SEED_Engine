#include "FieldObject_Star.h"

#include "AudioManager.h"

///////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////////////
FieldObject_Star::FieldObject_Star(){
    // 名前関連の初期化
    className_ = "FieldObject_Star";
    name_ = "Star";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    model_->color_ = { 1.0f,1.0f,0.0f,1.0f };
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // ライトの初期化
    light_ = std::make_unique<PointLight>();
    light_->color_ = { 1.0f,1.0f,0.5f,1.0f };
    // 全般の初期化
    FieldObject::Initialize();
}

FieldObject_Star::FieldObject_Star(const std::string& modelName){
    // 名前関連の初期化
    className_ = "FieldObject_Star";
    name_ = modelName;
    // モデルの初期化
    std::string path = "FieldObject/" + modelName;
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    model_->color_ = { 1.0f,1.0f,0.0f,1.0f };
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::Field);
    // ライトの初期化
    light_ = std::make_unique<PointLight>();
    light_->color_ = { 1.0f,1.0f,0.5f,1.0f };
    // 全般の初期化
    FieldObject::Initialize();
}

///////////////////////////////////////////////////////////////////
// 初期化関数
///////////////////////////////////////////////////////////////////
void FieldObject_Star::Initialize(){}


///////////////////////////////////////////////////////////////////
// 更新関数
///////////////////////////////////////////////////////////////////
void FieldObject_Star::Update(){
    model_->rotate_.y += (3.14f) * ClockManager::DeltaTime();
    light_->position = model_->translate_;
    light_->intensity = 0.5f + std::sin(3.14f * ClockManager::TotalTime()) * 0.4f;
    FieldObject::Update();
}

///////////////////////////////////////////////////////////////////
// 描画関数
///////////////////////////////////////////////////////////////////
void FieldObject_Star::Draw(){
    model_->color_ = { 1.0f,1.0f,0.0f,1.0f };
    FieldObject::Draw();
    light_->SendData();
}


///////////////////////////////////////////////////////////////////
// 衝突処理
///////////////////////////////////////////////////////////////////
void FieldObject_Star::OnCollision( BaseObject* other, ObjectType objectType){
    other;
    if(objectType == ObjectType::Player){
        // 取得されたので削除依頼を行う
        removeFlag_ = true;
        AudioManager::PlayAudio("SE/star.wav",false,0.8f);
    }
}
