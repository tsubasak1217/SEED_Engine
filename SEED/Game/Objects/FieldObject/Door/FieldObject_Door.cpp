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
    FieldObject::Initialize();
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
    FieldObject::Initialize();
}

FieldObject_Door::~FieldObject_Door() = default;


////////////////////////////////////////////////////////////////////////
// 初期化関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::Initialize(){
    FieldObject::Initialize();
    isOpened_ = false;
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


////////////////////////////////////////////////////////////////////////
// getter / setter
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::SetIsOpened(bool isOpened){
    if (isOpened_ != isOpened){
        isOpened_ = isOpened;
        if (isOpened_){
            ChangeState(new OpeningState());
        } else {
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
void FieldObject_Door::OnNotify(const std::string& event,[[maybe_unused]] void* data){
    // data を使用しないので無視する
    if (event == "SwitchActivated"){
        SetIsOpened(true);
    } else if (event == "SwitchDeactivated"){
        SetIsOpened(false);
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
}