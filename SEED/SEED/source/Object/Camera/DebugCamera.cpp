#include "DebugCamera.h"
#include "InputManager.h"
#include "MatrixFunc.h"
#include "ClockManager.h"


DebugCamera::DebugCamera() {
    Initialize();
}

DebugCamera::~DebugCamera() {}

//////////////////////////////////////////////////
//     更新処理
//////////////////////////////////////////////////

void DebugCamera::Update() {

    Move();
    UpdateMatrix();
}

void DebugCamera::Move() {

    //移動方向の取得
    moveDirection_ = {
        Input::GetStickValue(LR::LEFT).x + float(Input::IsPressKey(DIK_D) - Input::IsPressKey(DIK_A)),
        (Input::GetLRTriggerValue(LR::LEFT) - Input::GetLRTriggerValue(LR::RIGHT)) + float(Input::IsPressKey(DIK_Q) - Input::IsPressKey(DIK_E)),
        Input::GetStickValue(LR::LEFT).y + float(Input::IsPressKey(DIK_W) - Input::IsPressKey(DIK_S))
    };

    // 回転量の計算
    Vector3 rotateValue;

    if (!Input::IsPressMouse(MOUSE_BUTTON::LEFT)) {
        rotateValue = {
            -rotateSpeed_ * Input::GetStickValue(LR::RIGHT).y,
            rotateSpeed_ * Input::GetStickValue(LR::RIGHT).x,
            0.0f
        };
    } else {
        Vector2 mousePos = Input::GetMousePosition();
        Vector2 preMousePos = Input::GetPreMousePosition();

        Line ray[2] = {
            GetRay(mousePos),
            GetRay(preMousePos)
        };

        Vector3 rayVec[2] = {
            ray[0].end_ - ray[0].origin_,
            ray[1].end_ - ray[1].origin_
        };

        Quaternion rotateQuat = Quaternion::LookAt(rayVec[1], rayVec[0]);
        rotateValue = Quaternion::ToEuler(rotateQuat);
        rotateValue.z = 0.0f;
        rotateValue *= 2.0f;
    }

    // 移動量の計算
    Vector3 velocity = (moveDirection_ * moveSpeed_) * RotateMatrix({ 0.0f,transform_.rotate_.y,0.0f });

    // トランスフォームの更新
    transform_.rotate_ += rotateValue * ClockManager::TimeRate();
    transform_.translate_ += velocity * ClockManager::DeltaTime();
}
