#pragma once
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <optional>
#include <vector>
#include <memory>

/// <summary>
/// 3Dのパーティクル
/// </summary>
class Particle3D{
public:
    Particle3D() = default;
    Particle3D(class EmitterBase* emitter);
    ~Particle3D() = default;
    virtual void Update();
    virtual void Draw();

public:
    bool GetIsAlive()const{ return !lifetimer_.IsFinished();}
    void SetAcceleration(const Vector3& acceleration){ acceleration_ = acceleration; }
    const Vector3& GetPos() const{ return particle_->transform_.translate; }

protected:
    // モデル
    std::unique_ptr<Model>particle_;
    // 保存変数
    Vector3 emitPos_;
    Vector3 kScale_;
    std::optional<Vector3> endPos_; // 目標位置

    // ビルボードを適用するかどうか
    bool isBillboard_;

    // 移動関連
    Vector3 baseDirection_;
    float directionRange_;
    Vector3 direction_;
    float speed_;
    Vector3 velocity_;
    Vector3 acceleration_;
    Vector3 totalAcceleration_;

    // 寿命関連
    Timer lifetimer_;

    // 重力関連
    bool isUseGravity_;
    float gravity_;
    float gravityAcceleration_;

    // 回転関連
    bool isUseRotate_;
    float rotateSpeed_;
    Vector3 rotateAxis_;
    Quaternion localRotate_;

    // カーブ
    Curve scaleCurve_;
    Curve velocityCurve_;// 速度カーブ
    Curve rotateCurve_;// 回転カーブ
    Curve colorCurve_;// 色カーブ

    // 色
    Color masterColor_;
    ColorMode colorMode_ = ColorMode::RGBA;

    // テクスチャ
    uint32_t textureHandle_;

    // 初回にあらかじめアップデートする秒数(必要であれば)
    float initUpdateTime_ = 0.0f;
};