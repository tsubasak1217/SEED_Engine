#include "SceneManager.h"
#include "Scene_Game.h"
#include "Scene_Title/Scene_Title.h"
#include "Scene_Clear/Scene_Clear.h"
#include "CollisionManaer/CollisionManager.h"

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
    // Colliderのリセット
    CollisionManager::ResetColliderList();
    // シーンの更新
    pScene_->Update();
    // シーン更新終了後にカメラを更新
    CameraManager::Update();
    // 当たり判定
    CollisionManager::CheckCollision();
}

void SceneManager::Draw(){
    pScene_->Draw();
    CollisionManager::Draw();
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
