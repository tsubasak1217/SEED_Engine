#include "FieldObject_Soil.h"

/////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
/////////////////////////////////////////////////////////////////////
FieldObject_Soil::FieldObject_Soil(){
    // 名前関連の初期化
    className_ = "FieldObject_Soil";
    name_ = "Soil";
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

FieldObject_Soil::FieldObject_Soil(const std::string& modelName){
    // 名前関連の初期化
    className_ = "FieldObject_Soil";
    name_ = modelName;
    // モデルの初期化
    std::string path = "FieldObject/" + modelName;
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}


/////////////////////////////////////////////////////////////////////
// 初期化関数
/////////////////////////////////////////////////////////////////////
void FieldObject_Soil::Initialize(){}
