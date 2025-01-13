#include "FieldObject_Door.h"
#include "State/OpeningState.h"
#include "State/ClosingState.h"
#include "State/OpenedState.h"
#include "State/ClosedState.h"

// lib
#include "ClockManager.h" 

////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////////
FieldObject_Door::FieldObject_Door(){
    className_ = "FieldObject_Door";
    name_ = "door";
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    currentState_ = std::make_unique<ClosedState>();
}

FieldObject_Door::FieldObject_Door(const std::string& modelName)
    : FieldObject(modelName){
    className_ = "FieldObject_Door";
    currentState_ = std::make_unique<ClosedState>();
}

FieldObject_Door::~FieldObject_Door() = default;


////////////////////////////////////////////////////////////////////////
// 初期化関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Door::Initialize(){
    FieldObject::Initialize();
    if (model_){
        model_->translate_.y = 0.0f;
    }
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
void FieldObject_Door::OnNotify(const std::string& event, [[maybe_unused]]void* data){
    if (event == "ToggleDoor"){
        SetIsOpened(!isOpened_);
    }
}