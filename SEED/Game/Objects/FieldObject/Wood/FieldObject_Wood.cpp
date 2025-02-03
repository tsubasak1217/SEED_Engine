#include "FieldObject_Wood.h"

FieldObject_Wood::FieldObject_Wood(){
    className_ = "FieldObject_Wood";
    name_ = "Wood";
    // モデルの初期化
    model_ = std::make_unique<Model>("FieldObject/Wood.obj");
    model_->isRotateWithQuaternion_ = false;
    model_->meshColor_[0] = MyMath::FloatColor(0x8d6400ff);
    model_->meshColor_[1] = leafColor_;
    model_->meshColor_[2] = leafColor_;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}

void FieldObject_Wood::Initialize(){
}

void FieldObject_Wood::Update(){
    model_->meshColor_[1] = leafColor_;
    model_->meshColor_[2] = leafColor_;

    FieldObject::Update();
}

void FieldObject_Wood::Draw(){
    FieldObject::Draw();
}
