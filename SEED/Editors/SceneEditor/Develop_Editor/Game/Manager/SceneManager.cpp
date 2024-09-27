#include "SceneManager.h"
#include "Scene_Game.h"
#include "Scene_Title/Scene_Title.h"
#include "Scene_Clear/Scene_Clear.h"

SceneManager* SceneManager::instance_ = nullptr;
std::unique_ptr<Scene_Base> SceneManager::pScene_ = nullptr;

SceneManager::SceneManager(){
    pScene_.reset(new Scene_Game(instance_));
}

SceneManager::~SceneManager(){
    delete instance_;
    instance_ = nullptr;
}

void SceneManager::Initialize(){
    instance_ = GetInstance();
}

void SceneManager::Update(){
    pScene_->Update();
}

void SceneManager::Draw(){
    pScene_->Draw();
}

SceneManager* SceneManager::GetInstance(){

    if(!instance_){
        instance_ = new SceneManager();
    }

    return instance_;
}

void SceneManager::ChangeScene(Scene_Base* newScene){
    pScene_.reset(newScene);
}
