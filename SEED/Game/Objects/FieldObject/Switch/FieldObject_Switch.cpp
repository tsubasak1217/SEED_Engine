// FieldObject_Switch.cpp
#include "FieldObject_Switch.h"
#include "FieldObject/Door/FieldObject_Door.h"
#include "InputManager.h"

////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////
FieldObject_Switch::FieldObject_Switch(){
    // 名前の初期化
    className_ = "FieldObject_Switch";
    name_ = "switch";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();

    //objectのClear
    associatedDoors_.clear();
    associatedMoveFloors_.clear();
}

////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Update(){
    if (isColliding_ && !isActivated_){
        // 乗っている状態で、まだ起動状態でなければ
        isActivated_ = true;
        Notify("SwitchActivated", this);
    } else if (!isColliding_ && isActivated_){
        // 何も乗っていない状態で、現在起動状態であれば
        isActivated_ = false;
        Notify("SwitchDeactivated", this);
    }

    FieldObject_Activator::Update();
}
