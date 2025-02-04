#include "FieldObject_Box.h"

FieldObject_Box::FieldObject_Box(){
    className_ = "FieldObject_Box";
    name_ = "box";
    model_ = std::make_unique<Model>("FieldObject/Box.obj");
    model_->isRotateWithQuaternion_ = false;
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::OnFieldObject);
    Initialize();
    isApplyGravity_ = true;
}
