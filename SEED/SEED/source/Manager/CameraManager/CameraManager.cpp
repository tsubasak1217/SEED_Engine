#include "CameraManager.h"
#include <cassert>
#include <imgui.h>
// static変数初期化
CameraManager* CameraManager::instance_ = nullptr;

CameraManager::~CameraManager(){
    if(instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

void CameraManager::Initialize(){

    GetInstance();
    instance_->mainCamera_ = std::make_unique<BaseCamera>();
    instance_->stageViewCamera_ = std::make_unique<StageViewCamera>();
    instance_->debugCamera_ = std::make_unique<DebugCamera>();

    instance_->cameras_["main"] = instance_->mainCamera_.get();
    instance_->cameras_["stageView"] = instance_->stageViewCamera_.get();
    instance_->cameras_["debug"] = instance_->debugCamera_.get();
}

void CameraManager::Update(){

    // カメラの更新
    for(auto& camera : instance_->cameras_){
        camera.second->UpdateMatrix();
        camera.second->Update();
    }
}

CameraManager* CameraManager::GetInstance(){
    if(!instance_){ instance_ = new CameraManager(); }
    return instance_;
}

BaseCamera* CameraManager::GetCamera(const std::string& name){
    // 指定要素がなければアサート
    if(instance_->cameras_.find(name) == instance_->cameras_.end()){ assert(false); }
    // カメラのポインタ
    return instance_->cameras_[name];
}

void CameraManager::AddCamera(const std::string& name, BaseCamera* camera){
    // 指定要素が既にあるとアサート
    if(instance_->cameras_.find(name) != instance_->cameras_.end()){ assert(false); }
    // カメラを追加
    instance_->cameras_[name] = camera;
}

// カメラの削除
void CameraManager::DeleteCamera(const std::string& name){
    // 指定要素がなければアサート
    if(instance_->cameras_.find(name) == instance_->cameras_.end()){ assert(false); }
    // カメラを削除
    instance_->cameras_.erase(name);
}
