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

struct Sprite{

public:
    Sprite();
    Sprite(const std::string& filename);
    Sprite(const std::string& filename, const Vector2& leftTop, const Vector2& size);

    void Draw();

public:

    Vector2 leftTop;
    Vector2 size;

    // 色；マテリアル
    uint32_t GH;
    Vector4 color;
    BlendMode blendMode;

    // SRT
    Vector2 scale;
    float rotate;
    Vector2 translate;
    
    // アンカーポイント
    Vector2 anchorPoint;

    // 切り取り範囲
    Vector2 clipLT;
    Vector2 clipSize;

    // UVトランスフォーム
    Matrix4x4 uvTransform;    

    // 解像度の変更を反映するかどうかの設定
    bool isStaticDraw;

    // 描画位置の設定(前景か背景か)
    DrawLocation drawLocation = DrawLocation::Front;
    uint32_t layer = 0;// 描画順。大きいほど手前に描画

public:
    Matrix4x4 GetWorldMatrix()const;

};