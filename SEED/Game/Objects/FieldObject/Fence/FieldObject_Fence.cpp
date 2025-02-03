#include "FieldObject_Fence.h"

FieldObject_Fence::FieldObject_Fence(){
    className_ = "FieldObject_Fence";
    name_ = "Fence";
    // モデルの初期化
    model_ = std::make_unique<Model>("FieldObject/Fence.obj");
    model_->isRotateWithQuaternion_ = false;
    // 茶色にする
    model_->meshColor_[0] = Vector4(0.5f, 0.3f, 0.1f, 1.0f);
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}

void FieldObject_Fence::Initialize(){
}
