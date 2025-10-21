#pragma once
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <optional>
#include <vector>
#include <memory>
#include <concepts>


class Particle2D{
public:
    Particle2D() = default;
    Particle2D(class EmitterBase* emitter);
    ~Particle2D() = default;
    virtual void Update();
    virtual void Draw();

public:
    bool GetIsAlive()const{ return lifeTime_ < kLifeTime_; }
    void SetAcceleration(const Vector2& acceleration){ acceleration_ = acceleration; }
    const Vector2& GetPos() const{ return particleTransform_.translate; }

protected:
    void Enter();
    void Exit();

protected:
    // 頂点情報
    Transform2D particleTransform_;
    std::vector<Vector2> vertices_;
    // 保存変数
    Vector2 kScale_;
    Vector2 emitPos_;
    std::optional<Vector2> goalPos_; // 目標位置(ある場合)

    // 移動関連
    Vector2 baseDirection_;
    float directionRange_;
    Vector2 direction_;
    float speed_;
    Vector2 velocity_;
    Vector2 acceleration_;
    Vector2 totalAcceleration_;
    EasingFunction velocityEaseFunc_;

    // 寿命関連
    float kLifeTime_;
    float lifeTime_ = 0.0f;

    // 重力関連
    bool isUseGravity_;
    float gravity_;
    float gravityAcceleration_;

    // 回転関連
    bool isUseRotate_;
    float rotateSpeed_;
    float localRotate_;
    EasingFunction rotateEaseFunc_;

    // テクスチャ
    uint32_t textureHandle_;

    // 出現・消失の動きに関わるパラメータ
    float maxTimePoint_; // 最大に到達する時間(0.0f~1.0f/lifeTime)
    float maxTimeRate_; // 最大を維持する時間の割合
    float borderTime_[2];// [0]: 出現の終了時間, [1]: 消失の開始時間
    Vector2 kInScale_;
    Vector2 kOutScale_;
    float kInAlpha_;
    float kOutAlpha_;
    EasingFunction enterEaseFunc_;
    EasingFunction exitEaseFunc_;

    // 初回にあらかじめアップデートする秒数(必要であれば)
    float initUpdateTime_ = 0.0f;
};