#include "Particle2D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/Emitter2D.h>


Particle2D::Particle2D(EmitterBase* emitter){

    // Emitter3Dにキャスト
    Emitter2D* emitter2D = dynamic_cast<Emitter2D*>(emitter);
    if(!emitter2D){
        return;
    }

    ////////////////////// 座標をランダム決定 ////////////////////////
    Range2D emitRange;
    Vector2 center = emitter2D->GetCenter();
    emitRange.min = center - emitter2D->emitRange * 0.5f;
    emitRange.max = center + emitter2D->emitRange * 0.5f;
    particle_.translate = MyFunc::Random(emitRange);
    emitPos_ = particle_.translate;

    ///////////////////// 大きさをランダム決定 ////////////////////////
    float radius = MyFunc::Random(emitter2D->radiusRange.min, emitter2D->radiusRange.max);
    particle_ = MakeEqualQuad2D(radius);
    particle_.scale = MyFunc::Random(emitter2D->scaleRange);
    kScale_ = particle_.scale;

    /////////////////////// 進行方向を決定 ////////////////////////

    // 目標位置が設定されている場合
    if(emitter2D->isSetGoalPosition_){
        endPos_.value() = emitter2D->GetCenter() + emitter2D->goalPosition;
    }

    // 進行方向を範囲内でばらけさせる
    float angleRange = 3.14f * std::clamp(emitter2D->directionRange, 0.0f, 1.0f);
    float theta = MyFunc::Random(-angleRange, angleRange); // 水平回転

    // 基本方向とランダムな方向を組み合わせて最終的な方向を決定
    direction_ = emitter2D->baseDirection * RotateMatrix(theta); // 回転を適用

    ///////////////////// 速度を決定 ////////////////////////

    speed_ = MyFunc::Random(emitter2D->speedRange.min, emitter2D->speedRange.max);

    //////////////////////// 回転情報を決定 ////////////////////////

    // ランダム初期化フラグがある場合
    if(emitter2D->isRoteteRandomInit_){
        particle_.rotate = MyFunc::Random(-3.14f, 3.14f);
        localRotate_ = particle_.rotate;
    }

    // 回転速度の決定
    rotateSpeed_ = MyFunc::Random(emitter2D->rotateSpeedRange.min, emitter2D->rotateSpeedRange.max);

    //////////////////////// 重力を決定 //////////////////////////
    gravity_ = emitter2D->gravity_;

    ///////////////////// 寿命をランダム決定 ////////////////////////
    lifetimer_.Initialize(MyFunc::Random(emitter2D->lifeTimeRange.min, emitter2D->lifeTimeRange.max));

    ////////////////////// 色をランダム決定 ////////////////////////
    masterColor_ = emitter2D->colors_[MyFunc::Random(0, (int)emitter2D->colors_.size() - 1)];

    ///////////////////// ブレンドモードを設定 ////////////////////////
    particle_.blendMode = emitter2D->blendMode_;

    //////////////////////// テクスチャを設定 ////////////////////////

    // モデルのテクスチャを取得し、無ければ追加
    textureHandle_ = TextureManager::LoadTexture(
        emitter2D->texturePaths_[MyFunc::Random(0, (int)emitter2D->texturePaths_.size() - 1)]
    );
    particle_.GH = textureHandle_;

    /////////////////////// カーブの設定 /////////////////////////

    rotateCurve_ = emitter2D->rotateCurve_;
    velocityCurve_ = emitter2D->velocityCurve_;
    colorCurve_ = emitter2D->colorCurve_;
    scaleCurve_ = emitter2D->scaleCurve_;

    ///////////////////// 色操作のモード /////////////////////////

    colorMode_ = emitter2D->colorMode_;

    //////////////////////// 初回更新時間 ////////////////////////
    if(emitter2D->initUpdateTime_ > 0.0f){
        initUpdateTime_ = emitter2D->initUpdateTime_;
    }
}

void Particle2D::Update(){

    float t = lifetimer_.GetProgress();
    float deltaTime = ClockManager::DeltaTime();

    // 初回アップデート時間が設定されている場合はその時間を使用
    if(initUpdateTime_ > 0.0f){
        deltaTime += initUpdateTime_;
        initUpdateTime_ = 0.0f; // 空に戻す
    }

    //////////////////////////////////////
    // 寿命を減らす
    //////////////////////////////////////

    lifetimer_.Update();

    //////////////////////////////////
    // 速度の計算
    //////////////////////////////////

    // 特に目標地点がない場合
    if(endPos_ == std::nullopt){
        // 基本速度
        velocity_ = direction_ * speed_ * velocityCurve_.GetValue(t) * deltaTime;

        // 加速度の計算
        totalAcceleration_ += acceleration_ * deltaTime;
        velocity_ += totalAcceleration_ * deltaTime;

        // 重力処理
        if(isUseGravity_){
            gravityAcceleration_ += gravity_ * deltaTime;
            velocity_.y += gravityAcceleration_ * deltaTime;
        }
        // translateの更新
        particle_.translate += velocity_;

    } else{
        // 明確な目標地点がある場合
        particle_.translate = MyMath::Lerp(
            emitPos_,
            endPos_.value(),
            velocityCurve_.GetValue(t)
        );
    }

    //////////////////////////////////
    // 回転処理
    //////////////////////////////////

    if(isUseRotate_){
        // 通常の回転
        particle_.rotate += rotateSpeed_ * rotateCurve_.GetValue(t) * deltaTime;
    }

    //////////////////////////////////
    // 色の更新
    //////////////////////////////////

    particle_.color = masterColor_ * colorCurve_.GetValue4(t);
}

void Particle2D::Draw(){
    // パーティクル描画
    SEED::DrawQuad2D(particle_);
}