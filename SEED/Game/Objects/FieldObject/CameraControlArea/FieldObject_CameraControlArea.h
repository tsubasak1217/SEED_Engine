#pragma once
#include <optional>
#include "FieldObject/FieldObject.h"
#include "FollowCamera.h"
#include "MyFunc.h"
#include "MyMath.h"

class FieldObject_CameraControlArea : public FieldObject{
public:
    FieldObject_CameraControlArea();
    ~FieldObject_CameraControlArea() = default;

public:
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;
    void OnCollision( BaseObject* other, ObjectType objectType)override;

public:
    // 試しにペアレントするかどうかのフラグ
    bool isTestParent_ = false;

    // カメラの位置を固定するかどうか
    bool isPositionFixed_ = false;

    // 固定する場合のトランスフォーム情報
    Vector3 cameraPos_;
    Vector3 cameraRotate_;
    Matrix4x4 cameraMatrix_;
    std::unique_ptr<BaseObject> cameraModel_;

    // 固定しない場合のカメラの角度・距離情報
    float theta_ = 0.0f;
    float phi_ = 0.0f;
    float exitTheta_ = -3.14f * 0.5f;
    float exitPhi_ = 3.14f * 0.45f;
    float distance_ = 0.0f;

private:
    bool isCollidePlayer_ = false;
    bool preIsCollidePlayer_ = false;
    FollowCamera* pCamera_ = nullptr;
    static bool isCollidePlayerAll_;
    std::optional<float> preDistance_;
};