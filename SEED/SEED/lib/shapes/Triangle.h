#pragma once

#include <SEED/Lib/Tensor/Vector4.h>
#include <stdint.h>
#include <cmath>
#include <numbers>
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Lib/Structs/Material.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Structs/DrawLocation.h>

/// <summary>
/// 三次元の三角形
/// </summary>
struct Triangle{

    Triangle();
    Triangle(
        const Vector3& v1, const Vector3& v2, const Vector3& v3,
        const Vector3& scale = { 1.0f,1.0f,1.0f },
        const Vector3& rotate = { 0.0f, 0.0f, 0.0f },
        const Vector3& translate = { 0.0f,0.0f,0.0f },
        const Color& color = {1.0f,1.0f,1.0f,1.0f},
        BlendMode blendMode = BlendMode::NORMAL
    );

    Vector3 localVertex[3];
    Vector3 scale = {1.0f,1.0f,1.0f};
    Vector3 rotate;
    Vector3 translate;
    Color color = {1.0f,1.0f,1.0f,1.0f};
    int32_t lightingType = LIGHTINGTYPE_NONE;
    int32_t GH = -1;
    BlendMode blendMode = BlendMode::NORMAL;
    D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
    Matrix4x4 uvTransform = IdentityMat4();
};


/// <summary>
/// 二次元の三角形
/// </summary>
struct Triangle2D{

    Triangle2D();
    Triangle2D(
        const Vector2& v1, const Vector2& v2, const Vector2& v3,
        const Vector2& scale = { 1.0f,1.0f },
        float rotate = 0.0f,
        const Vector2& translate = { 0.0f,0.0f },
        const Color& color = { 1.0f,1.0f,1.0f,1.0f },
        BlendMode blendMode = BlendMode::NORMAL
    );

public:
    Vector2 localVertex[3];
    Vector2 scale = { 1.0f,1.0f };
    float rotate = 0.0f;
    Vector2 translate = {0.0f,0.0f};
    Color color = { 1.0f,1.0f,1.0f,1.0f };
    int32_t lightingType = LIGHTINGTYPE_NONE;
    int32_t GH = -1;
    BlendMode blendMode = BlendMode::NORMAL;
    Matrix4x4 uvTransform = IdentityMat4();
    // 解像度の変更を反映するかどうかの設定
    bool isStaticDraw = false;
    // 描画位置の設定(前景か背景か)
    DrawLocation drawLocation = DrawLocation::Front;
    bool isApplyViewMat = false;
    int32_t layer = 0;// 描画順。大きいほど手前に描画

public:
    Matrix4x4 GetWorldMatrix()const;
};