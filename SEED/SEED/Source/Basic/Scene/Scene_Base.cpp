#include "Scene_Base.h"
#include <Game/GameSystem.h>

Scene_Base::Scene_Base(){}

void Scene_Base::Update(){
    if(currentState_){
        currentState_->Update();
    }

    if (currentEventState_) {
        currentEventState_->Update();
    }
}

void Scene_Base::Draw(){
    if(currentState_){
        currentState_->Draw();
    }

    if (currentEventState_) {
        currentEventState_->Draw();
    }
}

void Scene_Base::BeginFrame(){
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
    currentState_.reset(nextState);
}

void Scene_Base::CauseEvent(EventState_Base* nextEventState) {
    currentEventState_.reset(nextEventState);
}
