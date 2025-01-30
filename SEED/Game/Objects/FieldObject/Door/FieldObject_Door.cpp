#include "FieldObject_Door.h"
#include "State/OpeningState.h"
#include "State/ClosingState.h"
#include "State/OpenedState.h"
#include "State/ClosedState.h"

// local
#include "../FieldObject/Switch/FieldObject_Switch.h"

// lib
#include "ClockManager.h" 

////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////////
FieldObject_Door::FieldObject_Door(){
    // 名前の初期化
    className_ = "FieldObject_Door";
    name_ = "door";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // 状態の初期化
    currentState_ = std::make_unique<ClosedState>();
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    Initialize();
}

FieldObject_Door::FieldObject_Door(const std::string& modelName)
    : FieldObject(modelName){
    // クラス名の初期化
    className_ = "FieldObject_Door";
    // 状態の初期化
    currentState_ = std::make_unique<ClosedState>();
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    Initialize();
}

FieldObject_Door::~FieldObject_Door() = default;

////////////////////////////////////////////////////////////////////////
// 初期化関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::Initialize(){
    FieldObject::Initialize();
    isOpened_ = false;
    hasSwitch_ = false;
    ChangeState(new ClosedState());
}

////////////////////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::Update(){
    float deltaTime = ClockManager::DeltaTime();
    if (currentState_){
        currentState_->Update(this, deltaTime);
    }
    FieldObject::Update();
}

////////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::Draw(){
    FieldObject::Draw();
}

void FieldObject_Door::ShowImGui(){
    const char* hasSwitchStr = hasSwitch_ ? "true" : "false";
    ImGui::Text("has switch:%s", hasSwitchStr);
    ImGui::DragFloat("OpenSpeed", &openSpeed_, 0.01f);
    ImGui::DragFloat("ClosedPosY", &closedPosY_, 0.01f);
}

////////////////////////////////////////////////////////////////////////
// getter / setter
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::SetIsOpened(bool isOpened){
    if (isOpened_ != isOpened){
        isOpened_ = isOpened;
        if (isOpened_){
            ChangeState(new OpeningState());
        } else{
            ChangeState(new ClosingState());
        }
    }
}

////////////////////////////////////////////////////////////////////////
// 状態変更用メソッド
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::ChangeState(DoorState* newState){
    if (currentState_){
        currentState_->Exit(this);
    }
    currentState_.reset(newState);
    if (currentState_){
        currentState_->Enter(this);
    }
}

////////////////////////////////////////////////////////////////////////
// Observerの関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::OnNotify(const std::string& event, [[maybe_unused]] void* data){
    // SwitchActivated・SwitchDeactivated イベントの場合、データがあるなら自分自身か確認
    if ((event == "SwitchActivated" || event == "SwitchDeactivated") && data != nullptr){
        if (data != static_cast< void* >(this)){
            return; // データがあり、自分自身でない場合は無視
        }
    }

    // ドアが完全に閉じている状態かどうかを確認して開く操作を行う
    if (event == "SwitchActivated"){
        if (dynamic_cast< ClosedState* >(currentState_.get())){
            SetIsOpened(true);
        }
    }
    // ドアが完全に開いている状態かどうかを確認して閉じる操作を行う
    else if (event == "SwitchDeactivated"){
        if (dynamic_cast< OpenedState* >(currentState_.get())){
            SetIsOpened(false);
        }
    }
}

////////////////////////////////////////////////////////////////////////
// setter
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::SetSwitch(FieldObject_Switch* pSwitch){
    FieldObject_Switch* switchObj = pSwitch;
    if (switchObj){
        switchObj->RegisterObserver(this);
    }
    hasSwitch_ = true;
}

void FieldObject_Door::RemoveSwitch(FieldObject_Switch* pSwitch){
    FieldObject_Switch* switchObj = pSwitch;
    if (switchObj){
        switchObj->UnregisterObserver(this);
    }
    hasSwitch_ = false;
}
