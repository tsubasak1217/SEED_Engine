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
// 描画前処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::BeginFrame(){

    // 「重さにより押されている（= isColliding_）場合に起動、そうでなければ停止」
    if (isColliding_ && !isActivated_){
        // 押されていて、かつまだ起動していなければ起動する
        isActivated_ = true;
        Notify("SwitchActivated");
    } else if (!isColliding_ && isActivated_){
        // 押されておらず、かつ現在起動しているなら停止する
        isActivated_ = false;
        Notify("SwitchDeactivated");
    }
    // 基底クラスの BeginFrame() を呼ぶ（重さをリセット＆判定などを行う）
    FieldObject_Activator::BeginFrame();
}