#include <SEED/Source/Object/Particle/BaseParticle.h>
#include <SEED/Source/SEED.h>

BaseParticle::BaseParticle(Emitter_Base* emitter){
    emitter;
}

void BaseParticle::Update(){

    //////////////////////////////////////
    // ビルボード処理
    //////////////////////////////////////
    if(isBillboard_){
        particle_->transform_.rotate_ = SEED::GetCamera()->GetRotation();
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
        particle_->transform_.rotate_ += rotateAxis_ * rotateSpeed_ * ClockManager::DeltaTime();
    }

    //////////////////////////////////
    // パーティクルのトランスフォーム更新
    //////////////////////////////////
    particle_->transform_.translate_ += velocity_;
    particle_->UpdateMatrix();
}

void BaseParticle::Draw(){
    // パーティクル描画
    particle_->Draw();
}
