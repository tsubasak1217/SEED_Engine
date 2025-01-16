#include "FieldObject_Sphere.h"

uint32_t FieldObject_Sphere::nextFieldObjectID_ = 1;

///////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////////////
FieldObject_Sphere::FieldObject_Sphere(){
    // 名前関連の初期化
    className_ = "FieldObject_Sphere";
    name_ = "sphere";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();

    fieldObjectID_ = nextFieldObjectID_++;
}

FieldObject_Sphere::FieldObject_Sphere(const std::string& modelName){
    // 名前関連の初期化
    className_ = "FieldObject_Sphere";
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

    fieldObjectID_ = nextFieldObjectID_++;
}

///////////////////////////////////////////////////////////////////
// 初期化関数
///////////////////////////////////////////////////////////////////
void FieldObject_Sphere::Initialize(){}