#include "FieldObject_PointLight.h"

FieldObject_PointLight::FieldObject_PointLight(){
    className_ = "FieldObject_PointLight";
    name_ = "PointLight";
    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/Sphere.obj");
    model_->isRotateWithQuaternion_ = false;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // ポイントライトの初期化
    pointLight_ = std::make_unique<PointLight>();
    // 全般の初期化
    FieldObject::Initialize();
}

void FieldObject_PointLight::Initialize(){
}

void FieldObject_PointLight::Update(){
    FieldObject::Update();
    pointLight_->position = GetWorldTranslate();
}

void FieldObject_PointLight::Draw(){

    // ライトの情報を送る
    pointLight_->SendData();

#ifdef _DEBUG
    FieldObject::Draw();
#endif // _DEBUG
}
