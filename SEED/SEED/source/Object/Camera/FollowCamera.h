#pragma once
#include "Camera.h"
#include "MyMath.h"
#include "MyFunc.h"
#include "../Game/Objects/BaseObject.h"

struct FollowCamera : public Camera{

public:
    FollowCamera();
    ~FollowCamera()override;
    void Initialize()override;
    void Update()override;

private:
    void UpdateAngle();

public:
    void SetTarget(BaseObject* target){ target_ = target; }
    void SetDistance(float distance){ distance_ = distance; }
    void SetRotateSpeed(float speed){ rotateSpeed_ = speed; }

private:

    BaseObject* target_ = nullptr;
    Vector3 defaultOffset_ = MyMath::Normalize(Vector3(0.0f, 2.0f, -5.0f));
    float distance_ = 20.0f;
    float theta_ = 0.0f;
    float phi_ = 0.0f;
    float rotateSpeed_ = 0.025f;

    // 限界角度
    float kMaxPhi_ = 1.5f;
    float kMinPhi_ = 0.1f;
};