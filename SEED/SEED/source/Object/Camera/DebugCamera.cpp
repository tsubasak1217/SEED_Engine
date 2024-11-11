#include "DebugCamera.h"
#include "InputManager.h"
#include "MatrixFunc.h"

DebugCamera::~DebugCamera(){}

//////////////////////////////////////////////////
//     更新処理
//////////////////////////////////////////////////

void DebugCamera::Update(){

    Move();
    UpdateMatrix();
}

void DebugCamera::Move(){
    
    //移動方向の取得
    moveDirection_ = {
        InputManager::GetStickValue(PAD_STICK::LEFT).x,
        InputManager::GetLRTriggerValue(PAD_TRIGGER::LEFT) - InputManager::GetLRTriggerValue(PAD_TRIGGER::RIGHT),
        InputManager::GetStickValue(PAD_STICK::LEFT).y
    };

    // 回転の加算
    transform_.rotate_.y += 0.025f * InputManager::GetStickValue(PAD_STICK::RIGHT).x;
    transform_.rotate_.x += -0.025f * InputManager::GetStickValue(PAD_STICK::RIGHT).y;

    // 移動の加算
    Vector3 velocity = (moveDirection_ * moveSpeed_) * RotateMatrix({ 0.0f,transform_.rotate_.y,0.0f });
    transform_.translate_ += velocity;

}
