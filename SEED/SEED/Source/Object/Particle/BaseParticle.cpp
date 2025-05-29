#include <SEED/Source/Object/Particle/BaseParticle.h>
#include <SEED/Source/SEED.h>

BaseParticle::BaseParticle(Emitter_Base* emitter){
    emitter;
}

void BaseParticle::Update(){

    float t = lifeTime_ / kLifeTime_;

    //////////////////////////////////////
    // 寿命を減らす
    //////////////////////////////////////
    lifeTime_ += ClockManager::DeltaTime();


    //////////////////////////////////
    // 速度の計算
    //////////////////////////////////

    // 基本速度
    float velocityEase = velocityEaseFunc_(t);
    if(&velocityEaseFunc_ == &Easing::Ease[0]){ velocityEase = 1.0f; }
    velocity_ = direction_ * speed_ * velocityEase * ClockManager::DeltaTime();

    // 加速度の計算
    totalAcceleration_ += acceleration_ * ClockManager::DeltaTime();
    velocity_ += totalAcceleration_ * ClockManager::DeltaTime();

    // 重力処理
    if(isUseGravity_){
        gravityAcceleration_ += gravity_ * ClockManager::DeltaTime();
        velocity_.y += gravityAcceleration_ * ClockManager::DeltaTime();
    }

    // translateの更新
    particle_->transform_.translate_ += velocity_;

    //////////////////////////////////
    // 回転処理
    //////////////////////////////////

    float rotateEase = rotateEaseFunc_(t);
    if(&rotateEaseFunc_ == &Easing::Ease[0]){ rotateEase = 1.0f; }

    if(isBillboard_){
        if(!isUseRotate_){
            // 通常のビルボード
            particle_->transform_.rotateQuat_ = Quaternion::ToQuaternion(SEED::GetCamera()->GetRotation());
        } else{
            // ビルボードしながら任意回転(ずっと動かずに回転を見てる感じになる)
            localRotate_ *= Quaternion::AngleAxis(
                rotateSpeed_ * rotateEase * ClockManager::DeltaTime(),
                rotateAxis_
            );

            // ビルボードの回転と任意回転を合成
            particle_->transform_.rotateQuat_ = Quaternion::ToQuaternion(SEED::GetCamera()->GetRotation()) * localRotate_;
        }
    } else{
        if(isUseRotate_){
            // 通常の回転
            particle_->transform_.rotateQuat_ *= Quaternion::AngleAxis(
                rotateSpeed_ * rotateEase * ClockManager::DeltaTime(),
                rotateAxis_
            );
        }
    }

    //////////////////////////////////
    // パーティクルのトランスフォーム更新
    //////////////////////////////////
    particle_->UpdateMatrix();
}

void BaseParticle::Draw(){
    // パーティクル描画
    particle_->Draw();
}


// 出現の際の動き
void BaseParticle::Enter(){
    // もう最大に到達している
    if(lifeTime_ > borderTime_[0]){
        if(particle_->transform_.scale_ != kScale_){
            particle_->transform_.scale_ = kScale_;
            particle_->masterColor_.w = 1.0f;
        }
        return;
    }

    // まだ最大に到達していない
    float t = lifeTime_ / borderTime_[0];
    float ease = enterEaseFunc_(t);

    // スケールの補間
    particle_->transform_.scale_ = MyMath::Lerp(kInScale_ * kScale_, kScale_, ease);

    // アルファ値の補間
    particle_->masterColor_.w = MyMath::Lerp(kInAlpha_, 1.0f, ease);
}


// 消失の際の動き
void BaseParticle::Exit(){
    // まだ開始地点に到達していない
    if(lifeTime_ < borderTime_[1]){
        return;
    }

    // 消失
    float t = (lifeTime_ - borderTime_[1]) / (kLifeTime_ - borderTime_[1]);
    float ease = exitEaseFunc_(t);

    // スケールの補間
    particle_->transform_.scale_ = MyMath::Lerp(kScale_, kOutScale_ * kScale_, ease);
    // アルファ値の補間
    particle_->masterColor_.w = MyMath::Lerp(1.0f, kOutAlpha_, ease);
}
