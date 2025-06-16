#include <SEED/Source/Basic/Particle/BaseParticle.h>
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

    // 媒介変数の計算
    float velocityEase = velocityEaseFunc_(t);

    // 特に目標地点がない場合
    if(goalPos_ == std::nullopt){
        // 基本速度
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
        particle_->transform_.translate += velocity_;

    } else{
        // 明確な目標地点がある場合
        particle_->transform_.translate = MyMath::Lerp(
            emitPos_,
            goalPos_.value(),
            velocityEase
        );
    }

    //////////////////////////////////
    // 回転処理
    //////////////////////////////////

    float rotateEase = rotateEaseFunc_(t);
    if(&rotateEaseFunc_ == &Easing::Ease[0]){ rotateEase = 1.0f; }

    if(isBillboard_){
        if(!isUseRotate_){
            // 通常のビルボード
            particle_->transform_.rotate = SEED::GetMainCamera()->GetRotation();
        } else{
            // ビルボードしながら任意回転(ずっと動かずに回転を見てる感じになる)
            localRotate_ *= Quaternion::AngleAxis(
                rotateSpeed_ * rotateEase * ClockManager::DeltaTime(),
                rotateAxis_
            );

            // ビルボードの回転と任意回転を合成
            particle_->transform_.rotate = SEED::GetMainCamera()->GetRotation() * localRotate_;
        }
    } else{
        if(isUseRotate_){
            // 通常の回転
            particle_->transform_.rotate *= Quaternion::AngleAxis(
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
        if(particle_->transform_.scale != kScale_){
            particle_->transform_.scale = kScale_;
            particle_->masterColor_.w = 1.0f;
        }
        return;
    }

    // まだ最大に到達していない
    float t = lifeTime_ / borderTime_[0];
    float ease = enterEaseFunc_(t);

    // スケールの補間
    particle_->transform_.scale = MyMath::Lerp(kInScale_ * kScale_, kScale_, ease);

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
    particle_->transform_.scale = MyMath::Lerp(kScale_, kOutScale_ * kScale_, ease);
    // アルファ値の補間
    particle_->masterColor_.w = MyMath::Lerp(1.0f, kOutAlpha_, ease);
}
