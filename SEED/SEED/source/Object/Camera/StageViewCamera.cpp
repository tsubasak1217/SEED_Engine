#include "StageViewCamera.h"
#include "MatrixFunc.h"
#include "MyMath.h"
#include "InputManager.h"
#include "Environment.h"
#include "LocalFunc.h"
#include "ClockManager.h"
#include <DebugCamera.h>
#include "SEED.h"

#include <imgui.h>
#include <numbers>
#include <algorithm>
#undef max

StageViewCamera::StageViewCamera(){
    transform_.scale_ = {1.0f,1.0f,1.0f}; // scale
    transform_.rotate_ = {0.0f,0.0f,0.0f}; // rotate
    transform_.translate_ = {0.0f,1.0f,-10.0f}; // translate
    projectionMode_ = PERSPECTIVE;
    clipRange_ = kWindowSize;
    znear_ = 0.1f;
    zfar_ = 1000.0f;
    UpdateMatrix();

}

//////////////////////////////////////////////////////////////////////////
// 毎フレーム更新
//////////////////////////////////////////////////////////////////////////
void StageViewCamera::Update(){
    // スティック入力でピッチ・ヨーを更新
    Move();

    // ピッチ&ヨー から球面座標でカメラ位置を求める
    UpdatePosition();

    // カメラを原点方向に向ける
    LookAtViewPoint();

}

//////////////////////////////////////////////////////////////////////////
// 1) スティック入力でピッチ・ヨーを更新する
//////////////////////////////////////////////////////////////////////////
void StageViewCamera::Move(){
    Vector2 stickValue = Input::GetStickValue(LR::LEFT);

    // 入力が小さい場合は何もしない
    if (MyMath::Length(Vector3 {stickValue.x, 0.0f, stickValue.y}) < 0.1f){
        return;
    }

    // DeltaTime
    float dt = ClockManager::DeltaTime();

    // ------------------------------------------------------------
    // 上下回転(Pitch)、左右回転(Yaw) を計算
    // ------------------------------------------------------------
    float pitchDelta = stickValue.y * rotationSpeed_ * dt;  // X軸回転
    float yawDelta = stickValue.x * rotationSpeed_ * dt;    // Y軸回転

    // ピッチとヨーを加算
    pitch_ += pitchDelta;
    yaw_ += yawDelta;

    // ピッチを 0°～80° の範囲に制限し、地面下へ行かないように
    float minRad = MyMath::Deg2Rad(minPitchDeg_); // 0° (水平以上)
    float maxRad = MyMath::Deg2Rad(maxPitchDeg_); // 80°くらい
    pitch_ = std::clamp(pitch_, minRad, maxRad);

    // Yaw は -π～+π など自由に回してOK。
    // 必要に応じて 2π でラップするなど。
    if (yaw_ > float(std::numbers::pi)){ yaw_ -= float(std::numbers::pi) * 2; }
    if (yaw_ < -float(std::numbers::pi)){ yaw_ += float(std::numbers::pi) * 2; }
}

//////////////////////////////////////////////////////////////////////////
// 2) 球面座標からカメラ位置(translate_)を求める
//    ピッチ(0=水平, +90°=真上) & ヨー で半球上を動く
//////////////////////////////////////////////////////////////////////////
void StageViewCamera::UpdatePosition(){
    float cp = std::cos(pitch_);
    float sp = std::sin(pitch_);
    float cy = std::cos(yaw_);
    float sy = std::sin(yaw_);

    Vector3 newPos;
    newPos.x = radius_* cp * cy;
    newPos.y = radius_* sp;
    newPos.z = radius_* cp * sy;

    transform_.translate_ = newPos;
}

//////////////////////////////////////////////////////////////////////////
// 3) 原点(0,0,0)を注視(常に中心を見続ける)
//////////////////////////////////////////////////////////////////////////
void StageViewCamera::LookAtViewPoint(){
    // カメラ位置
    Vector3 eye = transform_.translate_;
    // 原点（注視点）
    Vector3 target = {0.0f, 0.0f, 0.0f};
    // 上方向
    Vector3 up = {0.0f, 1.0f, 0.0f};

    // カメラZ軸(前方向)
    Vector3 zAxis = MyMath::Normalize(target - eye);

    // カメラX軸(右方向) = up × zAxis
    Vector3 xAxis = MyMath::Normalize(MyMath::Cross(up, zAxis));
    // カメラY軸(上方向) = zAxis × xAxis
    Vector3 yAxis = MyMath::Cross(zAxis, xAxis);

    // 回転行列を構築
    Matrix4x4 mat {
        xAxis.x, yAxis.x, zAxis.x, 0.0f,
        xAxis.y, yAxis.y, zAxis.y, 0.0f,
        xAxis.z, yAxis.z, zAxis.z, 0.0f,
        0.0f,    0.0f,    0.0f,    1.0f
    };

    // 行列 → オイラー角変換
    Vector3 euler = MatrixToEulerAngles(mat);

    // transform_.rotate_ に入れておく (描画 or デバッグ用)
    transform_.rotate_ = euler;
}
