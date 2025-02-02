#include "FieldObject_Door.h"
#include "State/OpeningState.h"
#include "State/ClosingState.h"
#include "State/OpenedState.h"
#include "State/ClosedState.h"

// local
#include "../FieldObject/Activator/FieldObject_Activator.h"

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
    hasActivator_ = false;
    ChangeState(new ClosingState());
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
    const char* hasSwitchStr = hasActivator_ ? "true" : "false";
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
    if (event != "SwitchActivated" && event != "SwitchDeactivated"
        && event != "LeverActivated" && event != "LeverDeactivated"){
        return;
    }

    if (data != nullptr){
        if (data != static_cast< void* >(this)){
            // ドア自身に向けたイベントでなければ無視
            return;
        }
    }

    // ドアが完全に閉じている状態（ClosedState）なら開く
    if (event == "SwitchActivated" || event == "LeverActivated"){
        if (dynamic_cast< ClosedState* >(currentState_.get())){
            SetIsOpened(true);
        }
    }
    // ドアが完全に開いている状態（OpenedState）なら閉じる
    else if (event == "SwitchDeactivated" || event == "LeverDeactivated"){
        if (dynamic_cast< OpenedState* >(currentState_.get())){
            SetIsOpened(false);
        }
    }
}

////////////////////////////////////////////////////////////////////////
// setter
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::SetActivator(FieldObject_Activator* pActivator){
    FieldObject_Activator* activator = pActivator;
    if (activator){
        activator->RegisterObserver(this);
    }
    hasActivator_ = true;
}

void FieldObject_Door::RemoveActivator(FieldObject_Activator* pActivator){
    FieldObject_Activator* activator = pActivator;
    if (activator){
        activator->UnregisterObserver(this);
    }
    hasActivator_ = false;
}
