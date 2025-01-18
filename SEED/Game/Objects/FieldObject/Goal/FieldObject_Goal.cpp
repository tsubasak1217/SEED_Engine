#include "FieldObject_Goal.h"
#include "StageManager.h"

uint32_t FieldObject_Goal::nextFieldObjectID_ = 1;

////////////////////////////////////////////////////////////////////////////////////////
//  コンストラクタ
////////////////////////////////////////////////////////////////////////////////////////
FieldObject_Goal::FieldObject_Goal(){
    className_ = "FieldObject_Goal";
    name_ = "goal";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();

    fieldObjectID_ = nextFieldObjectID_++;
}

FieldObject_Goal::FieldObject_Goal(const std::string& modelName)
    : FieldObject(modelName){
    // クラス名の初期化
    className_ = "FieldObject_Goal";
    name_ = "goal";
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();

    fieldObjectID_ = nextFieldObjectID_++;
}

////////////////////////////////////////////////////////////////////////////////////////
//  初期化関数
////////////////////////////////////////////////////////////////////////////////////////
void FieldObject_Goal::Initialize(){}

////////////////////////////////////////////////////////////////////////////////////////
//  更新関数
////////////////////////////////////////////////////////////////////////////////////////
void FieldObject_Goal::Update(){
    FieldObject::Update();
}


////////////////////////////////////////////////////////////////////////////////////////
//  衝突処理
////////////////////////////////////////////////////////////////////////////////////////
void FieldObject_Goal::OnCollision(const BaseObject* other, ObjectType objectType){
    other;
    if(objectType == ObjectType::Player){
        // ゴールしたことを通知
        StageManager::StepStage(1);
    }
}
