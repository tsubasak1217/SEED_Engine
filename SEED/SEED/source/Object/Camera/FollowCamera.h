#pragma once
#include "BaseCamera.h"
#include "MyMath.h"
#include "MyFunc.h"
#include "Base/BaseObject.h"
#include "InputHangler.h"

struct FollowCamera : public BaseCamera{

public:
    FollowCamera();
    ~FollowCamera()override;
    void Initialize()override;
    void Update()override;

private:
    void UpdateAngle();
    void UpdateDistance();

public:
    void SetTarget(BaseObject* target){ target_ = target; }
    void SetDistance(float distance){ distance_ = distance; }
    void SetRotateSpeed(float speed){ rotateSpeed_ = speed; }

private:

    BaseObject* target_ = nullptr;
    Vector3 defaultOffset_;
    float distance_;
    float theta_;
    float phi_;
    float rotateSpeed_;

    // 限界角度
    float kMaxPhi_;
    float kMinPhi_;

    // 入力ハンドラ
public:/*外部からキーコンフィグ設定可能にするためpublic*/
    InputHandler<Vector2> angleInput_;
    InputHandler<float> distanceInput_;
};