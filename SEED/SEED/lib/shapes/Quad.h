#pragma once

#include "Vector4.h"
#include <stdint.h>
#include <cmath>
#include <numbers>
#include "MyMath.h"
#include "MatrixFunc.h"
#include "Material.h"
#include "blendMode.h"

struct Quad{

    Quad() = default;
    Quad(
        const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
        const Vector3& scale = { 1.0f,1.0f,1.0f },
        const Vector3& rotate = { 0.0f, 0.0f, 0.0f },
        const Vector3& translate = { 0.0f,0.0f,0.0f },
        const Vector4& color = { 1.0f,1.0f,1.0f,1.0f },
        BlendMode blendMode = BlendMode::NORMAL
    );

    Vector3 localVertex[4];
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
    Vector4 color;
    BlendMode blendMode;
    int32_t lightingType;
    int32_t GH;
    Matrix4x4 uvTransform;
};


struct Quad2D{

    Quad2D() = default;
    Quad2D(
        const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4,
        const Vector2& scale = { 1.0f,1.0f },
        float rotate = 0.0f,
        const Vector2& translate = { 0.0f,0.0f },
        const Vector4& color = { 1.0f,1.0f,1.0f,1.0f },
        BlendMode blendMode = BlendMode::NORMAL
    );

    Vector2 localVertex[4];
    Vector2 scale;
    float rotate;
    Vector2 translate;
    Vector4 color;
    BlendMode blendMode;
    int32_t lightingType = LIGHTINGTYPE_NONE;
    int32_t GH;
    Matrix4x4 uvTransform;
};