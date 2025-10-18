#pragma once
#include <SEED/Lib/Tensor/Vector4.h>
#include <stdint.h>
#include <cmath>
#include <numbers>
#include <json.hpp>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Lib/Structs/Material.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Structs/DrawLocation.h>
#include <SEED/Lib/Structs/Color.h>

struct Sprite{

public:
    Sprite();
    Sprite(const std::string& filename);
    Sprite(const std::string& filename, const Vector2& size);
    void Draw(const std::optional<Color>& masterColor = std::nullopt);

public:

    Vector2 size;

    // 色；マテリアル
    uint32_t GH;
    std::string texturePath;
    Color color;
    BlendMode blendMode;

    // SRT
    Transform2D transform;
    Vector2 offset;
    const Matrix3x3* parentMat = nullptr;

    // アンカーポイント
    Vector2 anchorPoint;

    // 切り取り範囲
    Vector2 clipLT;
    Vector2 clipSize;

    // UVトランスフォーム
    Transform2D uvTransform;  
    bool flipX = false;
    bool flipY = false;

    // 解像度の変更を反映するかどうかの設定
    bool isStaticDraw;

    // 描画位置の設定(前景か背景か)
    DrawLocation drawLocation = DrawLocation::Front;
    int32_t layer = 0;// 描画順。大きいほど手前に描画

    // 2D描画時にビュー行列を適用するかどうか
    bool isApplyViewMat = false;

private:
    Vector2 defaultSize_ = {0.0f,0.0f};

public:
    Matrix4x4 GetWorldMatrix()const;
    void SetTexture(const std::string& filename);
    void ToDefaultSize();
    Vector2 GetDefaultSize() const;

    // Json, ImGui
    nlohmann::json ToJson() const;
    void FromJson(const nlohmann::json& data);
    void Edit(const std::string& hash = "");
};