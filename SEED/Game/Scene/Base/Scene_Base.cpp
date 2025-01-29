#include "Scene_Base.h"
#include <../GameSystem.h>

Scene_Base::Scene_Base(){}

void Scene_Base::Update(){
    if(currentState_){
        currentState_->Update();
    }

    if (currentEvent_) {
        currentEvent_->Update();
    }
}

void Scene_Base::Draw(){
    if(currentState_){
        currentState_->Draw();
    }

    if (currentEvent_) {
        currentEvent_->Draw();
    }
}

void Scene_Base::BeginFrame(){
    isStateChanged_ = false;

    if(currentState_){
        currentState_->BeginFrame();
    }
}

void Scene_Base::ManageState(){
    if(currentState_){
        currentState_->ManageState();
    }
}

void Scene_Base::ChangeScene(const std::string& nextSceneName){
    GameSystem::ChangeScene(nextSceneName);
}

void Scene_Base::ChangeState(State_Base* nextState){
    isStateChanged_ = true;
    currentState_.reset(nextState);
}

void Scene_Base::CauseEvent(Event_Base* nextEventState) {
    currentEvent_.reset(nextEventState);
}
