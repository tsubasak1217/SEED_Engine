#pragma once
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Tensor/Quaternion.h>

struct Transform2D{
    Vector2 scale = { 1.0f,1.0f };
    float rotate = 0.0f; // radians
    Vector2 translate;
};

struct Transform{
    Vector3 scale = {1.0f,1.0f,1.0f};
    Vector3 rotate;
    Quaternion rotateQuat;
    Vector3 translate;
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