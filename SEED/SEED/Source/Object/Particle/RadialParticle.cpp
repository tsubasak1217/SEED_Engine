#include "RadialParticle.h"
#include "Emitter/Emitter_Plane3D.h"

RadialParticle::RadialParticle(Emitter_Base* emitter) : BaseParticle(emitter){

    Emitter_Plane3D* planeEmitter = dynamic_cast<Emitter_Plane3D*>(emitter);
    if(!planeEmitter){
        return;
    }

    ////////////////// パーティクルのモデルを生成 /////////////////////
    particle_ = std::make_unique<Model>("Assets/Plane.obj");
    particle_->isRotateWithQuaternion_ = false;
    particle_->isParticle_ = true;

    /////////////////////// フラグ類を決定 ///////////////////////////

    isBillboard_ = planeEmitter->isBillboard;
    isUseGravity_ = planeEmitter->isUseGravity;
    isUseRotate_ = planeEmitter->isUseRotate;

    ////////////////////// 座標をランダム決定 ////////////////////////
    Range3D emitRange;
    Vector3 center = planeEmitter->GetCenter();
    emitRange.min = center - planeEmitter->emitRange * 0.5f;
    emitRange.max = center + planeEmitter->emitRange * 0.5f;
    particle_->transform_.translate_ = MyFunc::Random(emitRange);

    ///////////////////// 大きさをランダム決定 ////////////////////////
    float radius = MyFunc::Random(planeEmitter->radiusRange.min, planeEmitter->radiusRange.max);
    particle_->transform_.scale_ = { radius,radius,radius };
    kScale_ = particle_->transform_.scale_;

    //////////////////// 進行方向をランダム決定 ///////////////////////
    float angleRange = 3.14f * std::clamp(planeEmitter->directionRange, 0.0f, 1.0f);
    float theta = MyFunc::Random(-angleRange, angleRange); // 水平回転
    float phi = MyFunc::Random(-angleRange / 2.0f, angleRange / 2.0f); // 垂直回転 (範囲を制限)

    Vector3 randomDirection = {// 球座標から方向ベクトルを計算
        std::cos(phi) * std::cos(theta),
        std::cos(phi) * std::sin(theta),
        std::sin(phi)
    };

    direction_ = randomDirection * Quaternion::DirectionToDirection({ 1.0f, 0.0f, 0.0f }, planeEmitter->baseDirection); // 回転を適用

    ///////////////////// 速度をランダム決定 ////////////////////////
    speed_ = MyFunc::Random(planeEmitter->speedRange.min, planeEmitter->speedRange.max);

    //////////////////////// 回転情報を決定 ////////////////////////

    particle_->transform_.rotate_ = MyFunc::RandomVector() * 3.14f;
    rotateAxis_ = MyFunc::RandomVector();
    rotateSpeed_ = MyFunc::Random(planeEmitter->rotateSpeedRange.min, planeEmitter->rotateSpeedRange.max);

    //////////////////////// 重力を決定 //////////////////////////
    gravity_ = planeEmitter->gravity;

    ///////////////////// 寿命をランダム決定 ////////////////////////
    kLifeTime_ = MyFunc::Random(planeEmitter->lifeTimeRange.min, planeEmitter->lifeTimeRange.max);
    lifeTime_ = kLifeTime_;

    ////////////////////// 色をランダム決定 ////////////////////////
    particle_->masterColor_ = planeEmitter->colors[MyFunc::Random(0, (int)planeEmitter->colors.size() - 1)];

    ///////////////////// ブレンドモードを設定 ////////////////////////
    particle_->blendMode_ = planeEmitter->blendMode;

    //////////////////////// カリングを設定 ////////////////////////
    particle_->cullMode_ = D3D12_CULL_MODE(planeEmitter->cullingMode + 1);

    //////////////////////// ライトを無効に //////////////////////////
    particle_->lightingType_ = (int32_t)LIGHTINGTYPE_NONE;

    //////////////////////// テクスチャを設定 ////////////////////////
    textureHandle_ = TextureManager::LoadTexture(
        planeEmitter->texturePaths[MyFunc::Random(0, (int)planeEmitter->texturePaths.size() - 1)]
    );
    particle_->materials_[0].GH = textureHandle_;

}

void RadialParticle::Update(){

    //
    float t = lifeTime_ / kLifeTime_;

    // パーティクルの移動
    velocity_ = direction_ * speed_ * ClockManager::DeltaTime();

    // 色を時間経過で薄くする
    particle_->masterColor_.w = std::clamp(lifeTime_ / kLifeTime_, 0.0f, 1.0f);

    // 徐々に小さくする
    particle_->transform_.scale_ = kScale_ * t;

    // 寿命、ビルボードの更新
    BaseParticle::Update();
}
