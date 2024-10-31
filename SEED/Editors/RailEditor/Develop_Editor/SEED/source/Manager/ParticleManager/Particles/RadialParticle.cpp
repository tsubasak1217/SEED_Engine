#include "RadialParticle.h"

RadialParticle::RadialParticle(
    const Range3D& positionRange, 
    const Range1D& radiusRange, 
    const Range1D& speedRange,
    float lifeTime,
    const std::initializer_list<Vector4>& colors,
    BlendMode blendMode
) : BaseParticle(positionRange, radiusRange, speedRange, lifeTime, colors, blendMode){

    particle_->textureGH_ = TextureManager::LoadTexture("particle.png");

    // パーティクルの向きをランダム決定
    float theta = MyFunc::Random(0.0f, 2.0f * 3.14f);
    float phi = MyFunc::Random(0.0f, 2.0f * 3.14f);
    direction_ = {
        std::cosf(theta),
        std::sinf(theta),
        std::sinf(phi)
    };
}

void RadialParticle::Update(){

    // パーティクルの移動
    particle_->translate_ += direction_ * speed_ * ClockManager::DeltaTime();

    // 色を時間経過で薄くする
    particle_->color_.w = lifeTime_ / kLifeTime_;

    // 寿命、ビルボードの更新
    BaseParticle::Update();
}
