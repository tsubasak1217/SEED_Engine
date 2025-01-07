#include "Scene_Title.h"

Scene_Title::Scene_Title(SceneManager* pSceneManager)
{
    pSceneManager_ = pSceneManager;
}

Scene_Title::~Scene_Title()
{
}

void Scene_Title::Initialize()
{
}

void Scene_Title::Finalize()
{
}

void Scene_Title::Update()
{
    currentState_->Update();
}

void Scene_Title::Draw()
{
    currentState_->Draw();
}

void Scene_Title::EndFrame(){}
