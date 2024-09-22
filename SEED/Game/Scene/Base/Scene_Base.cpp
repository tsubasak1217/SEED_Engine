#include "Scene_Base.h"
#include "SceneManager.h"

Scene_Base::Scene_Base(SceneManager* pSceneManager)
{
    pSceneManager_ = pSceneManager;
}

void Scene_Base::Update(){ currentState_->Update(); }

void Scene_Base::Draw(){ currentState_->Draw(); }

void Scene_Base::ChangeScene(Scene_Base* nextScene){
    pSceneManager_->ChangeScene(nextScene);
}

void Scene_Base::ChangeState(State_Base* nextState){
    currentState_.reset(nextState);
}
