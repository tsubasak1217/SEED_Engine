#pragma once

#include "Vector4.h"
#include <stdint.h>
#include <cmath>
#include <numbers>
#include "MyMath.h"
#include "MatrixFunc.h"
#include "Material.h"

struct Triangle{

    Triangle();
    Triangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale = { 1.0f,1.0f,1.0f },
        const Vector3& rotate = { 0.0f, 0.0f, 0.0f },
        const Vector3& translate = { 0.0f,0.0f,0.0f },
        uint32_t color = 0xffffffff
    );

    Vector3 localVertex[3];
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
    uint32_t color;
    Vector4 colorf;
    int32_t litingType_;
    Matrix4x4 uvTransform_;
};

struct Triangle2D{

    Triangle2D();
    Triangle2D(
        const Vector2& v1, const Vector2& v2, const Vector2& v3,
        const Vector2& scale = { 1.0f,1.0f },
        float rotate = 0.0f,
        const Vector2& translate = { 0.0f,0.0f },
        uint32_t color = 0xffffffff
    );

public:
    Vector2 localVertex[3];
    Vector2 scale;
    float rotate;
    Vector2 translate;
    uint32_t color;
    Vector4 colorf;
    int32_t litingType_;
    uint32_t GH_;
    Matrix4x4 uvTransform_;
    // 解像度の変更を反映するかどうかの設定
    bool isStaticDraw;


public:
    Matrix4x4 GetWorldMatrix()const;
};