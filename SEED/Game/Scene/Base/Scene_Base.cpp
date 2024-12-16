#include "Scene_Base.h"
#include "SceneManager.h"

void IScene::Update(){ currentState_->Update(); }

void IScene::Draw(){ currentState_->Draw(); }

void IScene::ChangeState(State_Base* nextState){
    currentState_.reset(nextState);
}
