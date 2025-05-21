#pragma once
#include <SEED/Source/Object/Camera/BaseCamera.h>
#include <SEED/Lib/Tensor/Vector3.h>

struct DebugCamera : public BaseCamera{
    DebugCamera();
    ~DebugCamera()override;
    void Update()override;

private:
    void Move();
    void MoveByPad();
    void MoveByKeyboard();

private:
    Vector3 moveDirection_;
    float moveSpeed_ = 30.0f;
    float rotateSpeed_ = 0.025f;
};