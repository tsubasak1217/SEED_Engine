#include "Scene_Clear.h"

Scene_Clear::Scene_Clear(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
}

Scene_Clear::~Scene_Clear(){}

void Scene_Clear::Initialize(){}

void Scene_Clear::Finalize(){}

void Scene_Clear::Update(){
    currentState_->Update();
}

void Scene_Clear::Draw(){
    currentState_->Draw();
}
