#pragma once
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Object/Particle/Emitter/Emitter.h>
#include <vector>
#include <memory>

class BaseParticle{
public:
    BaseParticle() = default;
    BaseParticle(Emitter_Base* emitter);
    virtual ~BaseParticle() = default;

    virtual void Update();
    virtual void Draw();

public:

    bool GetIsAlive()const{return lifeTime_ < kLifeTime_;}
    void SetAcceleration(const Vector3& acceleration){ acceleration_ = acceleration; }
    virtual const Vector3& GetPos() = 0;

protected:
    void Enter();
    void Exit();

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
};