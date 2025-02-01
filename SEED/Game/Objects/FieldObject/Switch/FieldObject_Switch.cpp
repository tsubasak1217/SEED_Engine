// FieldObject_Switch.cpp
#include "FieldObject_Switch.h"
#include "FieldObject/Door/FieldObject_Door.h"
#include "FieldObject/Switch/FieldObject_Switch.h"
#include "InputManager.h"

////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////
FieldObject_Switch::FieldObject_Switch()
    : switchType_(SwitchType::TYPE_TRIGGER){ // デフォルトを Trigger に設定
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

    associatedDoors_.clear();
}

FieldObject_Switch::FieldObject_Switch(const std::string& modelName, SwitchType type)
    : FieldObject(modelName), switchType_(type){ // SwitchType を初期化
    // クラス名の初期化
    className_ = "FieldObject_Switch";
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();

    associatedDoors_.clear();
}

////////////////////////////////////////////////////////////////////
// 初期化関数
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Initialize(){
    // 追加の初期化が必要な場合はここに記述
}

////////////////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Update(){

    FieldObject::Update();
}

////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Draw(){

    // 重さに応じてマテリアルを変更
    int leftrequiredWeight = std::clamp(int((float)requiredWeight_ - currentWeight_),0,100);

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
// 開始時のフレーム処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::BeginFrame(){
    FieldObject::BeginFrame();

    ///////////////////////////////////////////////////////
    // 初期化前にフラグに応じて処理を行う
    ///////////////////////////////////////////////////////
    if(switchType_ == SwitchType::TYPE_TRIGGER){
        // Trigger タイプの処理
        if(isColliding_ && !wasCollidingLastFrame_){
            Toggle();
        }
    } else if(switchType_ == SwitchType::TYPE_HELD){
        // Held タイプの処理
        if(isColliding_ && !isActivated_){
            isActivated_ = true;
            Notify("SwitchActivated");
        } else if(!isColliding_ && isActivated_){
            isActivated_ = false;
            Notify("SwitchDeactivated");
        }
    }

    ///////////////////////////////////////////////////////
    // 開始時の初期化
    ///////////////////////////////////////////////////////
    // 前フレームの衝突状態を更新
    wasCollidingLastFrame_ = isColliding_;
    // 現在の衝突状態をリセット
    isColliding_ = false;
    currentWeight_ = 0.0f;
    model_->color_ = { 1.0f,1.0f,1.0f,1.0f };
}

////////////////////////////////////////////////////////////////////
// 終了時のフレーム処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::EndFrame(){
    FieldObject::EndFrame();

    // 必要重量を満たしていればスイッチをオンにする
    if((int)currentWeight_ >= requiredWeight_){
        isColliding_ = true;
    }
}


////////////////////////////////////////////////////////////////////
// ImGui による表示
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::ShowImGui(){
    ImGui::Text("SwitchType");

    // チェックボックスの状態を判定
    bool isHeld = (switchType_ == SwitchType::TYPE_HELD);

    // チェックボックスの描画
    if(ImGui::Checkbox("Held", &isHeld)){
        // チェックボックスの状態が変更されたらSwitchTypeを更新
        switchType_ = isHeld ? SwitchType::TYPE_HELD : SwitchType::TYPE_TRIGGER;
    }

}

////////////////////////////////////////////////////////////////////
// Observer 用関数
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::RegisterObserver(IObserver* observer){
    observers_.push_back(observer);
}

void FieldObject_Switch::UnregisterObserver(IObserver* observer){
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}

void FieldObject_Switch::Notify(const std::string& event, void* data){
    for(auto& observer : observers_){
        if(observer){
            observer->OnNotify(event, data);
        }
    }
}

////////////////////////////////////////////////////////////////////
// Collision 用関数
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::OnCollision([[maybe_unused]] const BaseObject* other, ObjectType objectType){
    // 重さを加算
    if(objectType == ObjectType::Player or objectType == ObjectType::Egg or objectType == ObjectType::PlayerCorpse){
        currentWeight_ += other->GetSwitchPushWeight();
    }

}

////////////////////////////////////////////////////////////////////
// Toggle 関数
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Toggle(){
    isActivated_ = !isActivated_;
    std::string event = isActivated_ ? "SwitchActivated" : "SwitchDeactivated";
    Notify(event);  // 登録された Observer に通知
}

////////////////////////////////////////////////////////////////////
// ドアへのポインタを設定・取得するメソッド
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::AddAssociatedDoor(FieldObject_Door* door){
    // 重複登録を防ぐ
    if(std::find(associatedDoors_.begin(), associatedDoors_.end(), door) == associatedDoors_.end()){
        associatedDoors_.push_back(door);
    }
}

void FieldObject_Switch::RemoveAssociatedDoor(FieldObject_Door* door){
    auto it = std::remove_if(
        associatedDoors_.begin(),
        associatedDoors_.end(),
        [door](FieldObject_Door* existingDoor){
            return existingDoor == door;
        }
    );
    associatedDoors_.erase(it, associatedDoors_.end());
}

std::vector<FieldObject_Door*>& FieldObject_Switch::GetAssociatedDoors(){
    return associatedDoors_;
}

////////////////////////////////////////////////////////////////////
// 移動する床へのポインタを設定・取得するメソッド
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::AddAssociatedMoveFloor(FieldObject_MoveFloor* moveFloor){
    // 重複登録を防ぐ
    if(std::find(associatedMoveFloors_.begin(), associatedMoveFloors_.end(), moveFloor) == associatedMoveFloors_.end()){
        associatedMoveFloors_.push_back(moveFloor);
    }
}

void FieldObject_Switch::RemoveAssociatedMoveFloor(FieldObject_MoveFloor* moveFloor){
    auto it = std::remove_if(
        associatedMoveFloors_.begin(),
        associatedMoveFloors_.end(),
        [moveFloor](FieldObject_MoveFloor* existingMoveFloor){
            return existingMoveFloor == moveFloor;
        }
    );
    associatedMoveFloors_.erase(it, associatedMoveFloors_.end());
}

std::vector<FieldObject_MoveFloor*>& FieldObject_Switch::GetAssociatedMoveFloors(){
    return associatedMoveFloors_;
}
