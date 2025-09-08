#pragma once
#include <SEED/Lib/Tensor/Vector4.h>
#include <stdint.h>
#include <cmath>
#include <numbers>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <SEED/Lib/Structs/Material.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Structs/DrawLocation.h>

struct Sprite{

public:
    Sprite();
    Sprite(uint32_t GH);
    Sprite(const std::string& filename);
    Sprite(const std::string& filename, const Vector2& leftTop, const Vector2& size);

    void Draw();

public:

    Vector2 leftTop;
    Vector2 size;

    // 色；マテリアル
    uint32_t GH;
    std::string texturePath;
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
    bool flipX = false;
    bool flipY = false;

    // 解像度の変更を反映するかどうかの設定
    bool isStaticDraw;

    // 描画位置の設定(前景か背景か)
    DrawLocation drawLocation = DrawLocation::Front;
    int32_t layer = 0;// 描画順。大きいほど手前に描画

    // 2D描画時にビュー行列を適用するかどうか
    bool isApplyViewMat = true;

public:
    Matrix4x4 GetWorldMatrix()const;
    void SetTexture(const std::string& filename);


};