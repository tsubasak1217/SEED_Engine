#include "FieldObject_Tile.h"
#include "FieldObject_Tile.h"
#include "FieldObject_Tile.h"

FieldObject_Tile::FieldObject_Tile(){
    className_ = "FieldObject_Tile";
    name_ = "tile";
    model_ = std::make_unique<Model>("FieldObject/tile.obj");
    model_->isRotateWithQuaternion_ = false;
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}

void FieldObject_Tile::Initialize(){
}