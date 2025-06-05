#include <SEED/Source/Object/Camera/DebugCamera.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>


DebugCamera::DebugCamera(){
    Initialize();
}

DebugCamera::~DebugCamera(){}

//////////////////////////////////////////////////
//     更新処理
//////////////////////////////////////////////////

void DebugCamera::Update(){

    Move();
    UpdateMatrix();
}

void DebugCamera::Move(){
    MoveByPad();
    MoveByKeyboard();
}

/////////////////////////////////////////////////////////////
// ゲームパッドでの移動
/////////////////////////////////////////////////////////////
void DebugCamera::MoveByPad(){
    //移動方向の取得
    moveDirection_ = {
        Input::GetStickValue(LR::LEFT).x,
        Input::GetLRTriggerValue(LR::LEFT) - Input::GetLRTriggerValue(LR::RIGHT),
        Input::GetStickValue(LR::LEFT).y
    };

    // 回転量の計算
    Vector3 rotateValue = {
        -rotateSpeed_ * Input::GetStickValue(LR::RIGHT).y,
        rotateSpeed_ * Input::GetStickValue(LR::RIGHT).x,
        0.0f
    };

    // 移動量の計算
    Vector3 velocity = (moveDirection_ * moveSpeed_) * RotateMatrix({ 0.0f,transform_.rotate.y,0.0f });

    // トランスフォームの更新
    transform_.rotate += rotateValue * ClockManager::TimeRate();
    transform_.translate += velocity * ClockManager::DeltaTime();
}

//////////////////////////////////////////////////////////////
// キーボードでの移動
//////////////////////////////////////////////////////////////
void DebugCamera::MoveByKeyboard(){

    // シフトキーが押されている時間を計測(加速用)
    static float shiftPressedTime = 0.0f;
    static float doubleSpeedTime = 0.2f;
    float additionalSpeedRate = 1.0f + (shiftPressedTime / doubleSpeedTime);
    if(Input::IsPressKey(DIK_LSHIFT)){
        shiftPressedTime += ClockManager::DeltaTime();
    } else{
        shiftPressedTime = 0.0f;
    }

    // 回転の取得(マウス)
    Vector3 rotateValue = { 0.0f,0.0f,0.0f };
    float basePixelCount = 8.0f;
    if(Input::IsPressMouse(MOUSE_BUTTON::RIGHT)){
        rotateValue = {
            rotateSpeed_ * (Input::GetMouseVector().y/basePixelCount),
            rotateSpeed_ * (Input::GetMouseVector().x/basePixelCount),
            0.0f
        };
    }

    // 移動方向の取得
    moveDirection_ = {
        float(Input::IsPressKey(DIK_D) - Input::IsPressKey(DIK_A)),
        float(Input::IsPressKey(DIK_Q) - Input::IsPressKey(DIK_E)),
        float(Input::IsPressKey(DIK_W) - Input::IsPressKey(DIK_S))
    };

    // 移動量の計算
    Vector3 velocity = (moveDirection_ * moveSpeed_ * additionalSpeedRate) * RotateMatrix(transform_.rotate);

    // トランスフォームの更新
    transform_.rotate += rotateValue * ClockManager::TimeRate();
    transform_.translate += velocity * ClockManager::DeltaTime();
}
