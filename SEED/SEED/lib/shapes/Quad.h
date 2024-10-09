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
    Matrix4x4 uvTransform;
};