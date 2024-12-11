#include "BaseParticle.h"
#include "SEED.h"

BaseParticle::BaseParticle(const Emitter& emitter){

    ////////////////// パーティクルのモデルを生成 /////////////////////
    particle_ = std::make_unique<Model>("Plane.obj");
    particle_->isRotateWithQuaternion_ = false;

    /////////////////////// フラグ類を決定 ///////////////////////////

    isBillboard_ = emitter.isBillboard;
    isUseGravity_ = emitter.isUseGravity;
    isUseRotate_ = emitter.isUseRotate;

    ////////////////////// 座標をランダム決定 ////////////////////////
    Range3D emitRange;
    emitRange.min = emitter.center - emitter.emitRange * 0.5f;
    emitRange.max = emitter.center + emitter.emitRange * 0.5f;
    particle_->translate_ = MyFunc::Random(emitRange);

    ///////////////////// 大きさをランダム決定 ////////////////////////
    float radius = MyFunc::Random(emitter.radiusRange.min, emitter.radiusRange.max);
    particle_->scale_ = { radius,radius,radius };

    //////////////////// 進行方向をランダム決定 ///////////////////////
    float angleRange = 3.14f * std::clamp(emitter.directionRange, 0.0f, 1.0f);
    float theta = MyFunc::Random(-angleRange, angleRange); // 水平回転
    float phi = MyFunc::Random(-angleRange / 2.0f, angleRange / 2.0f); // 垂直回転 (範囲を制限)

    Vector3 randomDirection = {// 球座標から方向ベクトルを計算
        std::cos(phi) * std::cos(theta),
        std::cos(phi) * std::sin(theta),
        std::sin(phi)
    };

    direction_ = randomDirection * Quaternion::DirectionToDirection({ 1.0f, 0.0f, 0.0f }, emitter.baseDirection); // 回転を適用

    ///////////////////// 速度をランダム決定 ////////////////////////
    speed_ = MyFunc::Random(emitter.speedRange.min, emitter.speedRange.max);

    //////////////////////// 回転情報を決定 ////////////////////////

    particle_->rotate_ = MyFunc::RandomVector() * 3.14f;
    rotateAxis_ = MyFunc::RandomVector();
    rotateSpeed_ = MyFunc::Random(emitter.rotateSpeedRange.min, emitter.rotateSpeedRange.max);

    //////////////////////// 重力を決定 //////////////////////////
    gravity_ = emitter.gravity;

    ///////////////////// 寿命をランダム決定 ////////////////////////
    kLifeTime_ = MyFunc::Random(emitter.lifeTimeRange.min, emitter.lifeTimeRange.max);
    lifeTime_ = kLifeTime_;

    ////////////////////// 色をランダム決定 ////////////////////////
    particle_->color_ = emitter.colors[MyFunc::Random(0, (int)emitter.colors.size() - 1)];

    ///////////////////// ブレンドモードを設定 ////////////////////////
    particle_->blendMode_ = emitter.blendMode;

    //////////////////////// ライトを無効に //////////////////////////
    particle_->lightingType_ = (int32_t)LIGHTINGTYPE_NONE;

    //////////////////////// テクスチャを設定 ////////////////////////
    textureHandle_ = TextureManager::LoadTexture(
        emitter.texturePaths[MyFunc::Random(0, (int)emitter.texturePaths.size() - 1)]
    );
    particle_->textureGH_[0] = textureHandle_;
}

void BaseParticle::Update(){

    //////////////////////////////////////
    // ビルボード処理
    //////////////////////////////////////
    if(isBillboard_){
        particle_->rotate_ = SEED::GetCamera()->transform_.rotate_;
    }

    //////////////////////////////////////
    // 寿命を減らす
    //////////////////////////////////////
    lifeTime_ -= ClockManager::DeltaTime();


    //////////////////////////////////
    // 加速度を追加
    //////////////////////////////////
    totalAcceleration_ += acceleration_ * ClockManager::DeltaTime();
    velocity_ += totalAcceleration_ * ClockManager::DeltaTime();


    //////////////////////////////////
    // 重力処理
    //////////////////////////////////
    if(isUseGravity_){
        gravityAcceleration_ += gravity_ * ClockManager::DeltaTime();
        velocity_.y += gravityAcceleration_ * ClockManager::DeltaTime();
    }

    //////////////////////////////////
    // 回転処理
    //////////////////////////////////
    if(isUseRotate_ && !isBillboard_){
        particle_->rotate_ += rotateAxis_ * rotateSpeed_ * ClockManager::DeltaTime();
    }

    //////////////////////////////////
    // パーティクルのトランスフォーム更新
    //////////////////////////////////
    particle_->translate_ += velocity_;
    particle_->UpdateMatrix();
}

void BaseParticle::Draw(){
    // パーティクル描画
    particle_->Draw();
}
