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
    instance_->mainCamera_ = std::make_unique<Camera>();
    instance_->stageViewCamera_ = std::make_unique<StageViewCamera>();
    instance_->debugCamera_ = std::make_unique<DebugCamera>();

    instance_->cameras_["main"] = instance_->mainCamera_.get();
    instance_->cameras_["stageView"] = instance_->stageViewCamera_.get();
    instance_->cameras_["debug"] = instance_->debugCamera_.get();
}

void CameraManager::Update(){

    ImGui::Begin("camera");

    if (ImGui::CollapsingHeader("stageViewCamera")){ 
        if (instance_->cameras_.find("stageView") != instance_->cameras_.end()){
            instance_->cameras_["stageView"]->ShowGui();
        } 

        if (instance_->cameras_.find("debugCamera") != instance_->cameras_.end()){
            instance_->cameras_["debug"]->ShowGui();
        }
    }

    ImGui::End();


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
