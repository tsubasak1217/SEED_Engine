#pragma once
#include "Camera.h"
#include "Vector3.h"

struct DebugCamera : public Camera{
    DebugCamera() = default;
    ~DebugCamera()override;
    void Update()override;

private:
    void Move();

private:
    Vector3 moveDirection_;
    float moveSpeed_ = 0.5f;
};