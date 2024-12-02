#pragma once

#include "Vector4.h"
#include <stdint.h>
#include <cmath>
#include <numbers>
#include "MyMath.h"
#include "MatrixFunc.h"
#include "Material.h"
#include "blendMode.h"
#include "DrawLocation.h"

struct Triangle{

    Triangle();
    Triangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale = { 1.0f,1.0f,1.0f },
        const Vector3& rotate = { 0.0f, 0.0f, 0.0f },
        const Vector3& translate = { 0.0f,0.0f,0.0f },
        const Vector4& color = {1.0f,1.0f,1.0f,1.0f},
        BlendMode blendMode = BlendMode::NORMAL
    );

    Vector3 localVertex[3];
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
    Vector4 color;
    int32_t litingType;
    uint32_t GH;
    BlendMode blendMode;
    Matrix4x4 uvTransform;
};

struct Triangle2D{

    Triangle2D();
    Triangle2D(
        const Vector2& v1, const Vector2& v2, const Vector2& v3,
        const Vector2& scale = { 1.0f,1.0f },
        float rotate = 0.0f,
        const Vector2& translate = { 0.0f,0.0f },
        const Vector4& color = { 1.0f,1.0f,1.0f,1.0f },
        BlendMode blendMode = BlendMode::NORMAL
    );

public:
    Vector2 localVertex[3];
    Vector2 scale;
    float rotate;
    Vector2 translate;
    Vector4 color;
    int32_t litingType;
    uint32_t GH;
    BlendMode blendMode;
    Matrix4x4 uvTransform;
    // 解像度の変更を反映するかどうかの設定
    bool isStaticDraw;
    // 描画位置の設定(前景か背景か)
    DrawLocation drawLocation = DrawLocation::Front;
    uint32_t layer = 0;// 描画順。大きいほど手前に描画

public:
    Matrix4x4 GetWorldMatrix()const;
};