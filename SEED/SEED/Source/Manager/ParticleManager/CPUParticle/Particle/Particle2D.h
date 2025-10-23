#pragma once
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <optional>
#include <vector>
#include <memory>
#include <concepts>


/// <summary>
/// 2Dのパーティクル
/// </summary>
class Particle2D{
public:
    Particle2D() = default;
    Particle2D(class EmitterBase* emitter);
    ~Particle2D() = default;
    virtual void Update();
    virtual void Draw();

public:
    bool GetIsAlive()const{ return !lifetimer_.IsFinished(); }
    void SetAcceleration(const Vector2& acceleration){ acceleration_ = acceleration; }
    const Vector2& GetPos() const{ return particle_.translate; }

protected:
    // 頂点情報
    Quad2D particle_;
    // 保存変数
    Vector2 emitPos_;
    Vector2 kScale_;
    std::optional<Vector2> endPos_; // 目標位置

    // 移動関連
    Vector2 baseDirection_;
    float directionRange_;
    Vector2 direction_;
    float speed_;
    Vector2 velocity_;
    Vector2 acceleration_;
    Vector2 totalAcceleration_;

    // 寿命関連
    Timer lifetimer_;

    // カーブ
    Curve scaleCurve_;
    Curve velocityCurve_;// 速度カーブ
    Curve rotateCurve_;// 回転カーブ
    Curve colorCurve_;// 色カーブ

    // 色
    Color masterColor_;
    ColorMode colorMode_ = ColorMode::RGBA;

    // 重力関連
    bool isUseGravity_;
    float gravity_;
    float gravityAcceleration_;

    // 回転関連
    bool isUseRotate_;
    float rotateSpeed_;
    float localRotate_;

    // テクスチャ
    uint32_t textureHandle_;

    // 初回にあらかじめアップデートする秒数(必要であれば)
    float initUpdateTime_ = 0.0f;
};