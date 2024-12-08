#include "BaseParticle.h"
#include "SEED.h"

BaseParticle::BaseParticle(
    const Range3D& positionRange,
    const Range1D& radiusRange,
    const Vector3& baseDirection,
    float directionRange,
    const Range1D& speedRange,
    const Range1D& lifeTime,
    const std::vector<Vector4>& colors,
    BlendMode blendMode
){
    ////////////////// パーティクルのモデルを生成 /////////////////////
    particle_ = std::make_unique<Model>("plane.obj");
    particle_->isRotateWithQuaternion_ = false;

    ////////////////////// 座標をランダム決定 ////////////////////////
    particle_->translate_ = MyFunc::Random(positionRange);

    ///////////////////// 大きさをランダム決定 ////////////////////////
    float radius = MyFunc::Random(radiusRange.min, radiusRange.max);
    particle_->scale_ = { radius,radius,radius };

    //////////////////// 進行方向をランダム決定 ///////////////////////
    float angleRange = 3.14f * std::clamp(directionRange, 0.0f, 1.0f);
    float theta = MyFunc::Random(-angleRange, angleRange); // 水平回転
    float phi = MyFunc::Random(-angleRange / 2.0f, angleRange / 2.0f); // 垂直回転 (範囲を制限)

    Vector3 randomDirection = {// 球座標から方向ベクトルを計算
        std::cos(phi) * std::cos(theta),
        std::cos(phi) * std::sin(theta),
        std::sin(phi)
    };

    direction_ = randomDirection * Quaternion::DirectionToDirection({ 1.0f, 0.0f, 0.0f }, baseDirection); // 回転を適用

    ///////////////////// 速度をランダム決定 ////////////////////////
    speed_ = MyFunc::Random(speedRange.min, speedRange.max);

    ///////////////////// 寿命をランダム決定 ////////////////////////
    kLifeTime_ = MyFunc::Random(lifeTime.min, lifeTime.max);
    lifeTime_ = kLifeTime_;

    ////////////////////// 色をランダム決定 ////////////////////////
    particle_->color_ = colors[MyFunc::Random(0, (int)colors.size() - 1)];

    ///////////////////// ブレンドモードを設定 ////////////////////////
    particle_->blendMode_ = blendMode;

    //////////////////////// ライトを無効に //////////////////////////
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
