#include "Scene_Base.h"
#include <Game/GameSystem.h>
#include <SEED/Source/Basic/Object/GameObject.h>

Scene_Base::Scene_Base(){
    hierarchy_ = std::make_unique<Hierarchy>();
}

void Scene_Base::Finalize(){
    hierarchy_->Finalize();
}

void Scene_Base::Update(){
    if(currentState_){
        currentState_->Update();
    }

    if(currentEventState_){
        currentEventState_->Update();
    }
}

void Scene_Base::Draw(){
    if(currentState_){
        currentState_->Draw();
    }

    if(currentEventState_){
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

void Scene_Base::CauseEvent(EventState_Base* nextEventState){
    currentEventState_.reset(nextEventState);
}

// ヒエラルキーからのゲームオブジェクトの登録・削除
void Scene_Base::RegisterToHierarchy(GameObject* gameObject){
    hierarchy_->RegisterGameObject(gameObject);
}

void Scene_Base::RemoveFromHierarchy(GameObject* gameObject){
    hierarchy_->RemoveGameObject(gameObject);
}
