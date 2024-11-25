#include "BaseParticle.h"

BaseParticle::BaseParticle(
    const Range3D& positionRange,
    const Range1D& radiusRange,
    const Range1D& speedRange,
    float lifeTime,
    const std::initializer_list<Vector4>& colors,
    BlendMode blendMode
){
    // パーティクルのモデルを生成
    particle_ = std::make_unique<Model>("plane");

    // 座標をランダム決定
    particle_->translate_ = MyFunc::Random(positionRange);

    //大きさをランダム決定
    float radius = MyFunc::Random(radiusRange.min, radiusRange.max);
    particle_->scale_ = { radius,radius,radius };

    // 速度をランダム決定
    speed_ = MyFunc::Random(speedRange.min, speedRange.max);

    // 寿命をランダム決定
    kLifeTime_ = lifeTime;
    lifeTime_ = kLifeTime_;

    // 色をランダム決定
    std::vector<Vector4>colorList;
    for(auto& color : colors){ colorList.emplace_back(color); }
    particle_->color_ = colorList[MyFunc::Random(0, (int)colors.size() - 1)];

    // ブレンドモードを設定
    particle_->blendMode_ = blendMode;

    // ライトを無効に
    particle_->lightingType_ = (int32_t)LIGHTINGTYPE_NONE;
}

void BaseParticle::Update(){
    // ビルボード処理
    particle_->rotate_ = SEED::GetCamera()->transform_.rotate_;
    // 寿命を減らす
    lifeTime_ -= ClockManager::DeltaTime();
    // 加速度を追加
    totalAcceleration_ += acceleration_ * ClockManager::DeltaTime();
    velocity_ += totalAcceleration_ * ClockManager::DeltaTime();
    // 移動
    particle_->translate_ += velocity_;
    // パーティクルのトランスフォーム更新
    particle_->UpdateMatrix();
}

void BaseParticle::Draw(){
    // パーティクル描画
    particle_->Draw();
}
