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
// 描画関数
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Draw(){
    // 重さに応じてマテリアルを変更
    int leftrequiredWeight = std::clamp(int((float)requiredWeight_ - currentWeight_), 0, 100);
    // 重さに応じて色を変更
    switch(leftrequiredWeight){
    case 0:// 緑
        model_->color_ = { 0.0f,1.0f,0.0f,1.0f };
        break;
    case 1:// 黄
        model_->color_ = { 1.0f,1.0f,0.0f,1.0f };
        break;
    case 2:// オレンジ
        model_->color_ = { 1.0f,0.5f,0.0f,1.0f };
        break;
    default:// 赤
        model_->color_ = { 1.0f,0.0f,0.0f,1.0f };
        break;
    }
    FieldObject::Draw();
}

////////////////////////////////////////////////////////////////////
// フレーム開始時処理
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
    // 基底クラスの BeginFrame() を呼ぶ（重さをリセット＆判定などを行う）
    FieldObject_Activator::BeginFrame();
}

////////////////////////////////////////////////////////////////////
// フレーム終了時処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::EndFrame(){
    FieldObject::EndFrame();
    // 必要重量を満たしていればスイッチをオンにする
    if((int)currentWeight_ >= requiredWeight_){
        isColliding_ = true;
    }
}

////////////////////////////////////////////////////////////////////
// 衝突時処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::OnCollision([[maybe_unused]] const BaseObject* other, ObjectType objectType){
    // 重さを加算
    if(objectType == ObjectType::Player or objectType == ObjectType::Egg or objectType == ObjectType::PlayerCorpse){
        currentWeight_ += other->GetSwitchPushWeight();
    }

}