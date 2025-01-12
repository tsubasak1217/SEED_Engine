#include "Door.h"
#include "State/OpeningState.h"
#include "State/ClosingState.h"
#include "State/OpenedState.h"
#include "State/ClosedState.h"

// lib
#include "ClockManager.h" 

Door::Door(const std::string& modelName)
    : FieldObject(modelName){
    currentState_ = std::make_unique<ClosedState>();
}

Door::~Door() = default;

void Door::Initialize(){
    FieldObject::Initialize();
    if (model_){
        model_->translate_.y = 0.0f;
    }
    isOpened_ = false;
    ChangeState(new ClosedState());
}

void Door::Update(){
    float deltaTime = ClockManager::DeltaTime();
    if (currentState_){
        currentState_->Update(this, deltaTime);
    }
    FieldObject::Update();
}

void Door::Draw(){
    FieldObject::Draw();
}

void Door::SetIsOpened(bool isOpened){
    if (isOpened_ != isOpened){
        isOpened_ = isOpened;
        if (isOpened_){
            ChangeState(new OpeningState());
        } else{
            ChangeState(new ClosingState());
        }
    }
}

void Door::ChangeState(DoorState* newState){
    if (currentState_){
        currentState_->Exit(this);
    }
    currentState_.reset(newState);
    if (currentState_){
        currentState_->Enter(this);
    }
}

void Door::OnNotify(const std::string& event, [[maybe_unused]]void* data){
    if (event == "ToggleDoor"){
        SetIsOpened(!isOpened_);
    }
}