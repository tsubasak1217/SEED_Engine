#include "Scene_Base.h"
#include "SceneManager.h"

Scene_Base::Scene_Base(SceneManager* pSceneManager)
{
    pSceneManager_ = pSceneManager;
}

void Scene_Base::Update(){
    if(currentState_){
        currentState_->Update();
    }
}

void Scene_Base::Draw(){
    if(currentState_){
        currentState_->Draw();
    }
}

void Scene_Base::BeginFrame(){
    isStateChanged_ = false;
}

void Scene_Base::ManageState(){
    if(currentState_){
        currentState_->ManageState();
    }
}

void Scene_Base::ChangeScene(Scene_Base* nextScene){
    pSceneManager_->ChangeScene(nextScene);
}

void Scene_Base::ChangeScene(std::unique_ptr<Scene_Base> nextScene){
    pSceneManager_->ChangeScene(std::move(nextScene));
}

void Scene_Base::ChangeState(State_Base* nextState){
    isStateChanged_ = true;
    currentState_.reset(nextState);
}
