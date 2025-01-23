#include "FieldObject_MoveFloor.h"

// lib
#include "ClockManager.h"
#include "../adapter/json/JsonCoordinator.h"

/////////////////////////////////////////////////////////////////////////
// static変数宣言
/////////////////////////////////////////////////////////////////////////
uint32_t FieldObject_MoveFloor::nextFieldObjectID_ = 1;

/////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////
FieldObject_MoveFloor::FieldObject_MoveFloor(){
    // 名前の初期化
    className_ = "FieldObject_MoveFloor";
    name_ = "moveFloor";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // 全般の初期化
    Initialize();
    fieldObjectID_ = nextFieldObjectID_++;
}

FieldObject_MoveFloor::FieldObject_MoveFloor(const std::string& modelName)
    : FieldObject(modelName){
    // クラス名の初期化
    className_ = "FieldObject_MoveFloor";
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    Initialize();

    fieldObjectID_ = nextFieldObjectID_++;
}

/////////////////////////////////////////////////////////////////////////
// 初期化関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::Initialize(){
    FieldObject::Initialize();
}

/////////////////////////////////////////////////////////////////////////
// 更新関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::Update(){
    Move();
    FieldObject::Update();
}

/////////////////////////////////////////////////////////////////////////
// ImGui表示関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::ShowImGui(){
    ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);
}

/* private =============================================================*/

/////////////////////////////////////////////////////////////////////////
// 移動関数
/////////////////////////////////////////////////////////////////////////
void FieldObject_MoveFloor::Move(){
    if (routinePoints_.size() < 1){ return; }
    // ルーチンポイントを巡回
    if (MyMath::Length(model_->translate_, routinePoints_[currentMovePointIndex_]) < 0.001f){
        currentMovePointIndex_++;
        currentMovePointIndex_ = currentMovePointIndex_ % routinePoints_.size();
    }
    // ルーチンポイントに向かって移動
    Vector3 moveVec = MyMath::Normalize(routinePoints_[currentMovePointIndex_] - model_->translate_);
    model_->translate_ = model_->translate_ + moveVec * moveSpeed_ * ClockManager::DeltaTime();
}
