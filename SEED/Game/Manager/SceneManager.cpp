#include "SceneManager.h"
#include "Scene_Game.h"
#include "Scene_Title/Scene_Title.h"
#include "Scene_Clear/Scene_Clear.h"

SceneManager* SceneManager::instance_ = nullptr;
std::unique_ptr<IScene> SceneManager::currentScene_ = nullptr;

SceneManager::SceneManager(){
    currentScene_.reset(new Scene_Game());
}

SceneManager::~SceneManager(){
    delete instance_;
    instance_ = nullptr;
}

void SceneManager::Initialize(){
    instance_ = GetInstance();
}

void SceneManager::Update(){

    if(currentScene_->GetNextScene() != nullptr){
        ChangeScene(currentScene_->GetNextScene());
    }

    currentScene_->Update();
}

void SceneManager::Draw(){
    currentScene_->Draw();
}

SceneManager* SceneManager::GetInstance(){

    if(!instance_){
        instance_ = new SceneManager();
    }

    return instance_;
}

void SceneManager::ChangeScene(IScene* newScene){
    currentScene_.reset(newScene);
    currentScene_->Initialize();
}
