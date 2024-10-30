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
    instance_->mainCamera_ = std::make_unique<Camera>();
    instance_->debugCamera_ = std::make_unique<DebugCamera>();

    instance_->cameras_["main"] = instance_->mainCamera_.get();
    instance_->cameras_["debug"] = instance_->debugCamera_.get();
}

void CameraManager::Update(){

    // カメラの更新
    for(auto& camera : instance_->cameras_){
        if(!camera.second->isActive_){ continue; }
        camera.second->Update();
        camera.second->UpdateMatrix();
    }
}

CameraManager* CameraManager::GetInstance(){
    if(!instance_){ instance_ = new CameraManager(); }
    return instance_;
}

Camera* CameraManager::GetCamera(const std::string& name){
    // 指定要素がなければアサート
    if(instance_->cameras_.find(name) == instance_->cameras_.end()){ assert(false); }
    // カメラのポインタ
    return instance_->cameras_[name];
}

void CameraManager::AddCamera(const std::string& name, Camera* camera){
    // 指定要素が既にあるとアサート
    if(instance_->cameras_.find(name) != instance_->cameras_.end()){ assert(false); }
    // カメラを追加
    instance_->cameras_[name] = camera;
}
