#include "FieldObject_Goal.h"

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
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}

FieldObject_Goal::FieldObject_Goal(const std::string& modelName)
    : FieldObject(modelName){
    // クラス名の初期化
    className_ = "FieldObject_Goal";
    name_ = "goal";
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
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
