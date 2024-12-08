#include "RadialParticle.h"

RadialParticle::RadialParticle(
    const Range3D& positionRange,
    const Range1D& radiusRange,
    const Vector3& baseDirection,
    float directionRange,
    const Range1D& speedRange,
    const Range1D& lifeTime,
    const std::vector<Vector4>& colors,
    BlendMode blendMode
) : BaseParticle(positionRange, radiusRange, baseDirection, directionRange, speedRange, lifeTime, colors, blendMode){

    particle_->textureGH_[0] = TextureManager::LoadTexture("particle.png");
}

void RadialParticle::Update(){

    // パーティクルの移動
    velocity_ = direction_ * speed_ * ClockManager::DeltaTime();

    // 色を時間経過で薄くする
    particle_->color_.w = std::clamp(lifeTime_ / kLifeTime_, 0.0f, 1.0f);

    // 寿命、ビルボードの更新
    BaseParticle::Update();
}
