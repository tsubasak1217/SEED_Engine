#include <SEED/Source/Manager/CameraManager/CameraManager.h>
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
}

void CameraManager::Update(){

    // 全カメラを更新するのではなく、アクティブなカメラのみ更新
    for(auto& camera : instance_->cameras_){
        if(camera.second->isActive_){
            camera.second->Update();
            camera.second->UpdateMatrix();
        }
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

void CameraManager::RegisterCamera(const std::string& name, BaseCamera* camera){
    // 指定要素が既にあるとアサート
    if(instance_->cameras_.find(name) != instance_->cameras_.end()){ assert(false); }
    // カメラを追加
    instance_->cameras_[name] = camera;
}

// カメラの削除
void CameraManager::RemoveCamera(const std::string& name){
    // 指定要素がなければアサート
    if(instance_->cameras_.find(name) == instance_->cameras_.end()){ return; }
    // カメラを削除
    instance_->cameras_.erase(name);
}


void CameraManager::SetIsCameraActive(const std::string& name,bool isActive){
    // cameras_ マップから指定された名前のカメラを探す
    if(instance_->cameras_.find(name) != instance_->cameras_.end()){
        // カメラが見つかった場合、isActiveフラグを設定
        instance_->cameras_[name]->isActive_ = isActive;

    } else{
        // カメラが見つからない場合はアサート
        assert(false);
    }
}