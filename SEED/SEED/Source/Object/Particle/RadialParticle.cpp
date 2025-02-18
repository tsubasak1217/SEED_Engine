#include <SEED/Source/Object/Particle/RadialParticle.h>

RadialParticle::RadialParticle(const Emitter& emitter) : BaseParticle(emitter){
}

void RadialParticle::Update(){

    //
    float t = lifeTime_ / kLifeTime_;

    // パーティクルの移動
    velocity_ = direction_ * speed_ * ClockManager::DeltaTime();

    // 色を時間経過で薄くする
    particle_->color_.w = std::clamp(lifeTime_ / kLifeTime_, 0.0f, 1.0f);

    // 徐々に小さくする
    particle_->scale_ = kScale_ * t;

    // 寿命、ビルボードの更新
    BaseParticle::Update();
}
