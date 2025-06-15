#pragma once
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Tensor/Quaternion.h>

struct Transform2D{
    Vector2 scale = { 1.0f,1.0f };
    float rotate = 0.0f; // radians
    Vector2 translate;
    // 変換関数
    Matrix3x3 ToMatrix();
    Matrix4x4 ToMatrix4x4();
    void FromMatrix4x4(const Matrix4x4& mat);
};

struct Transform{
    Vector3 scale = {1.0f,1.0f,1.0f};
    Vector3 rotate;
    Quaternion rotateQuat;
    Vector3 translate;

    // 変換関数
    Matrix4x4 ToMatrix(bool isUseQuaternion);
    void FromMatrix(const Matrix4x4& mat);
};

struct EulerTransform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};

struct QuaternionTransform{
    Vector3 scale;
    Quaternion rotate;
    Vector3 translate;
};

struct TransformMatrix {
    Matrix4x4 WVP;
    Matrix4x4 world;
    Matrix4x4 worldInverseTranspose;
};