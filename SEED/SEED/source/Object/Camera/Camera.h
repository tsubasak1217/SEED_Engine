#pragma once
#include "Transform.h"
#include "Vector2.h"

#include <stdint.h>


struct Camera{

    Camera()= default;
    virtual ~Camera() = default;
    void UpdateMatrix();
    virtual void Update();
    virtual void ShowGui(){}
    //=============================================================================
    //              メンバ変数
    //=============================================================================

    EulerTransform transform_;
    Vector2 clipRange_;
    uint32_t projectionMode_;
    float znear_;
    float zfar_;

    Vector3 normal_;

    Matrix4x4 worldMat_;
    Matrix4x4 viewMat_;
    Matrix4x4 projectionMat_;
    Matrix4x4 projectionMat2D_;
    Matrix4x4 viewProjectionMat_;
    Matrix4x4 viewProjectionMat2D_;
    Matrix4x4 viewportMat_;
    Matrix4x4 vpVp_;



};

enum PROJECTIONMODE{
    PERSPECTIVE,
    ORTHO
};