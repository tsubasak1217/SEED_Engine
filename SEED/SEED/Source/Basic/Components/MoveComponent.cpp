#include "MoveComponent.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>

MoveComponent::MoveComponent(GameObject* pOwner, const std::string& tagName) : IComponent(pOwner, tagName){
    // テキストボックスの初期化
    if(tagName == ""){
        componentTag_ = "MoveComponent_ID:" + std::to_string(componentID_);
    }
}

void MoveComponent::Initialize(){
}

void MoveComponent::BeginFrame(){
    // 入力の取得
    Vector2 stickValue = Input::GetStickValue(LR::LEFT);

    direction_ = {
        stickValue.x,0.0f,stickValue.y
    };

    if(pCamera_){
        // カメラの向きに合わせて方向を調整
        Quaternion cameraRot = pCamera_->GetRotation();
        // xz平面のみを考慮して方向を調整
        direction_ *= RotateMatrix(cameraRot);
        direction_.y = 0.0f; // y成分を0にすることで水平移動に限定
        MyMath::Normalize(direction_);
    }
}

void MoveComponent::Update(){
    // 移動処理
    owner_->AddWorldTranslate(direction_ * speed_ * ClockManager::DeltaTime());

    // 移動方向を向かせる
    if(MyMath::Length(direction_) > 0.01f){
        owner_->SetLocalRotate(Quaternion::ToQuaternion(MyFunc::CalcRotateVec(direction_)));
        isMoving_ = true;
    } else{
        isMoving_ = false;
    }
}

void MoveComponent::Draw(){
}

void MoveComponent::EndFrame(){
}

void MoveComponent::Finalize(){
}

void MoveComponent::EditGUI(){
}

nlohmann::json MoveComponent::GetJsonData() const{
    return nlohmann::json();
}

void MoveComponent::LoadFromJson(const nlohmann::json& jsonData){
    jsonData;
}
