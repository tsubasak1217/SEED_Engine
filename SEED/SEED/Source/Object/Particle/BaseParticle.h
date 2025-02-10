#pragma once
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Object/Particle/Emitter.h>
#include <vector>
#include <memory>

class BaseParticle{
public:
    BaseParticle() = default;
    BaseParticle(const Emitter& emitter);
    virtual ~BaseParticle() = default;

    virtual void Update();
    virtual void Draw();

public:

    bool GetIsAlive()const{return lifeTime_ > 0.0f;}
    void SetAcceleration(const Vector3& acceleration){ acceleration_ = acceleration; }
    const Vector3& GetPos()const{ return particle_->translate_; }

protected:

    // モデル
    std::unique_ptr<Model>particle_;
    // 保存しておくスケール
    Vector3 kScale_;

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
    float kLifeTime_;
    float lifeTime_;

    // 重力関連
    bool isUseGravity_;
    float gravity_;
    float gravityAcceleration_;

    // 回転関連
    bool isUseRotate_;
    float rotateSpeed_;
    Vector3 rotateAxis_;

    // テクスチャ
    uint32_t textureHandle_;
};