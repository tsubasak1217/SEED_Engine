#pragma once
#include "BaseCamera.h"
#include "Quaternion.h"
#include "OBB.h"

struct StageViewCamera : public BaseCamera{
    StageViewCamera();
    virtual ~StageViewCamera() = default;

    void Update() override;         // 毎フレーム更新

private:
    void Move();                    // ピッチ&ヨーの更新 (スティック入力)
    void UpdatePosition();          // 球面座標 → (x,y,z)
    void LookAtViewPoint();         // 原点を注視

private:
    float radius_ = 60.0f;          // 中心からの距離
    float rotationSpeed_ = 1.0f;    // 回転スピード

    float pitch_ = 0.0f;            // 上下 (0～+1.57あたりでクランプ: 0°～90°)
    float yaw_ = 0.0f;              // 左右 (-π～+πなど自由に動かす)

    float minPitchDeg_ = 0.0f;      // 0°
    float maxPitchDeg_ = 80.0f;     // 80°

};
