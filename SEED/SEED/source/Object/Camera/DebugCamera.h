#pragma once
#include "BaseCamera.h"
#include "Vector3.h"

struct DebugCamera : public BaseCamera{
    DebugCamera();
    ~DebugCamera()override;
    void Update()override;

private:
    void Move();

private:
    Vector3 moveDirection_;
    float moveSpeed_ = 30.0f;
    float rotateSpeed_ = 0.025f;
};