#include "CameraManager.h"
#include <cassert>

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
    instance_->cameras_["main"] = std::make_unique<Camera>();
    instance_->cameras_["debug"] = std::make_unique<DebugCamera>();
}

void CameraManager::Update(){

    // カメラの更新
    for(auto& camera : instance_->cameras_){
        camera.second->Update();
        camera.second->UpdateMatrix();
    }
}

CameraManager* CameraManager::GetInstance(){
    if(!instance_){ instance_ = new CameraManager(); }
    return instance_;
}

Camera* CameraManager::GetCamera(std::string name){
    // 指定要素がなければアサート
    if(instance_->cameras_.find(name) == instance_->cameras_.end()){ assert(false); }
    // カメラのポインタ
    return instance_->cameras_[name].get();
}
