#pragma once
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <optional>
#include <vector>
#include <memory>

class Particle3D{
public:
    Particle3D() = default;
    Particle3D(class EmitterBase* emitter);
    ~Particle3D() = default;
    virtual void Update();
    virtual void Draw();

public:
    bool GetIsAlive()const{ return lifeTime_ < kLifeTime_; }
    void SetAcceleration(const Vector3& acceleration){ acceleration_ = acceleration; }
    const Vector3& GetPos() const{ return particle_->transform_.translate; }

protected:
    void Enter();
    void Exit();

protected:
    // モデル
    std::unique_ptr<Model>particle_;
    // 保存変数
    Vector3 kScale_;
    Vector3 emitPos_;
    std::optional<Vector3> goalPos_; // 目標位置(ある場合)

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
    Vector3 rotateAxis_;
    Quaternion localRotate_;
    EasingFunction rotateEaseFunc_;

    // テクスチャ
    uint32_t textureHandle_;

    // 出現・消失の動きに関わるパラメータ
    float maxTimePoint_; // 最大に到達する時間(0.0f~1.0f/lifeTime)
    float maxTimeRate_; // 最大を維持する時間の割合
    float borderTime_[2];// [0]: 出現の終了時間, [1]: 消失の開始時間
    Vector3 kInScale_;
    Vector3 kOutScale_;
    float kInAlpha_;
    float kOutAlpha_;
    EasingFunction enterEaseFunc_;
    EasingFunction exitEaseFunc_;

    // 初回にあらかじめアップデートする秒数(必要であれば)
    float initUpdateTime_ = 0.0f;
};