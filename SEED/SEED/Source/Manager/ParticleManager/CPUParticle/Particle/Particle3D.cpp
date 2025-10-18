#include "Particle3D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/Emitter3D.h>


Particle3D::Particle3D(EmitterBase* emitter){

    // Emitter3Dにキャスト
    Emitter3D* modelEmitter = dynamic_cast<Emitter3D*>(emitter);
    if(!modelEmitter){
        return;
    }

    ////////////////// パーティクルのモデルを生成 /////////////////////
    particle_ = std::make_unique<Model>(modelEmitter->emitModelFilePath_);
    particle_->isParticle_ = true;
    particle_->cullMode_ = modelEmitter->cullingMode; // カリングモードを設定

    /////////////////////// フラグ類を決定 ///////////////////////////

    isBillboard_ = modelEmitter->isBillboard;
    isUseGravity_ = modelEmitter->isUseGravity;
    isUseRotate_ = modelEmitter->isUseRotate;

    ////////////////////// 座標をランダム決定 ////////////////////////
    Range3D emitRange;
    Vector3 center = modelEmitter->GetCenter();
    emitRange.min = center - modelEmitter->emitRange * 0.5f;
    emitRange.max = center + modelEmitter->emitRange * 0.5f;
    particle_->transform_.translate = MyFunc::Random(emitRange);
    emitPos_ = particle_->transform_.translate;

    ///////////////////// 大きさをランダム決定 ////////////////////////
    float radius = MyFunc::Random(modelEmitter->radiusRange.min, modelEmitter->radiusRange.max);
    particle_->transform_.scale = MyFunc::Random(modelEmitter->scaleRange) * radius;
    kScale_ = particle_->transform_.scale;

    /////////////////////// 進行方向を決定 ////////////////////////

    direction_ = modelEmitter->baseDirection; // 基本方向を設定

    // 目標位置が設定されている場合、方向を目標位置に向ける
    if(modelEmitter->isSetGoalPosition){
        direction_ = MyMath::Normalize(modelEmitter->goalPosition - particle_->transform_.translate);
    }

    // 目標地点で終了するかどうかで処理を分岐
    if(!modelEmitter->isEndWithGoalPosition){
        // ばらけさせるやつ
        float angleRange = 3.14f * std::clamp(modelEmitter->directionRange, 0.0f, 1.0f);
        float theta = MyFunc::Random(-angleRange, angleRange); // 水平回転
        float phi = MyFunc::Random(-angleRange / 2.0f, angleRange / 2.0f); // 垂直回転 (範囲を制限)
        Vector3 randomDirection = {// 球座標から方向ベクトルを計算
            std::cos(phi) * std::cos(theta),
            std::cos(phi) * std::sin(theta),
            std::sin(phi)
        };

        // 基本方向とランダムな方向を組み合わせて最終的な方向を決定
        direction_ = randomDirection * Quaternion::DirectionToDirection({ 1.0f, 0.0f, 0.0f }, direction_); // 回転を適用

    } else{
        // 目標地点を設定
        goalPos_ = modelEmitter->GetCenter() + modelEmitter->goalPosition;
    }

    ///////////////////// 速度を決定 ////////////////////////
    if(!modelEmitter->isEndWithGoalPosition){
        speed_ = MyFunc::Random(modelEmitter->speedRange.min, modelEmitter->speedRange.max);
    }

    //////////////////////// 回転情報を決定 ////////////////////////

    // 回転軸指定がある場合、軸を決定
    if(modelEmitter->useRotateDirection){
        rotateAxis_ = modelEmitter->rotateDirection; // 基本方向を使用
    } else{
        rotateAxis_ = MyFunc::RandomVector();
    }

    // ランダム初期化フラグがある場合
    if(modelEmitter->isRoteteRandomInit_){
        particle_->transform_.rotate = Quaternion::ToQuaternion(rotateAxis_ * MyFunc::Random(-3.14f, 3.14f));
        localRotate_ = particle_->transform_.rotate;
    }

    // 回転速度の決定
    rotateSpeed_ = MyFunc::Random(modelEmitter->rotateSpeedRange.min, modelEmitter->rotateSpeedRange.max);

    //////////////////////// 重力を決定 //////////////////////////
    gravity_ = modelEmitter->gravity;

    ///////////////////// 寿命をランダム決定 ////////////////////////
    kLifeTime_ = MyFunc::Random(modelEmitter->lifeTimeRange.min, modelEmitter->lifeTimeRange.max);
    kLifeTime_ = std::clamp(kLifeTime_, 0.1f, 100000.0f); // 寿命の範囲を制限

    ////////////////////// 色をランダム決定 ////////////////////////
    particle_->masterColor_ = modelEmitter->colors[MyFunc::Random(0, (int)modelEmitter->colors.size() - 1)];

    ///////////////////// ブレンドモードを設定 ////////////////////////
    particle_->blendMode_ = modelEmitter->blendMode;

    //////////////////////// ライト設定 //////////////////////////
    particle_->lightingType_ = (int32_t)modelEmitter->lightingType_;

    //////////////////////// テクスチャを設定 ////////////////////////

    // モデルのテクスチャを取得し、無ければ追加
    if(modelEmitter->useDefaultTexture){
        //ModelData* modelData = ModelManager::GetModelData(modelEmitter->emitModelFilePath_);
        //auto& materials = modelData->materials;
        //for(auto material : materials){
        //    std::string& texturePath = material.textureFilePath_;
        //    modelEmitter->texturePaths.push_back(texturePath);
        //}

    } else{// カスタムテクスチャを使用する場合、メッシュごとにランダムなテクスチャを設定
        for(auto& material : particle_->materials_){
            textureHandle_ = TextureManager::LoadTexture(
                modelEmitter->texturePaths[MyFunc::Random(0, (int)modelEmitter->texturePaths.size() - 1)]
            );
            material.GH = textureHandle_;
        }
    }

    //////////////////// イージング関数を設定 ////////////////////////
    velocityEaseFunc_ = Easing::Ease[modelEmitter->velocityEaseType_];
    rotateEaseFunc_ = Easing::Ease[modelEmitter->rotateEaseType_];
    enterEaseFunc_ = Easing::Ease[modelEmitter->enterEaseType_];
    exitEaseFunc_ = Easing::Ease[modelEmitter->exitEaseType_];

    //////////////////////// 減衰の設定 ////////////////////////
    kInScale_ = modelEmitter->kInScale;
    kOutScale_ = modelEmitter->kOutScale;
    kInAlpha_ = modelEmitter->kInAlpha;
    kOutAlpha_ = modelEmitter->kOutAlpha;
    maxTimePoint_ = modelEmitter->maxTimePoint;
    maxTimeRate_ = modelEmitter->maxTimeRate;
    // 出現の終了時間
    borderTime_[0] = (kLifeTime_ * maxTimePoint_) - (maxTimeRate_ * kLifeTime_ * 0.5f);
    borderTime_[0] = std::clamp(borderTime_[0], 0.0f, kLifeTime_);
    // 消失の開始時間
    borderTime_[1] = (kLifeTime_ * maxTimePoint_) + (maxTimeRate_ * kLifeTime_ * 0.5f);
    borderTime_[1] = std::clamp(borderTime_[1], 0.0f, kLifeTime_);
}

void Particle3D::Update(){

    Enter();
    Exit();

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

void Particle3D::Draw(){
    // パーティクル描画
    particle_->Draw();
}



// 出現の際の動き
void Particle3D::Enter(){
    // もう最大に到達している
    if(lifeTime_ > borderTime_[0]){
        if(particle_->transform_.scale != kScale_){
            particle_->transform_.scale = kScale_;
            particle_->masterColor_.value.w = 1.0f;
        }
        return;
    }

    // まだ最大に到達していない
    float t = lifeTime_ / borderTime_[0];
    float ease = enterEaseFunc_(t);

    // スケールの補間
    particle_->transform_.scale = MyMath::Lerp(kInScale_ * kScale_, kScale_, ease);

    // アルファ値の補間
    particle_->masterColor_.value.w = MyMath::Lerp(kInAlpha_, 1.0f, ease);
}



// 消失の際の動き
void Particle3D::Exit(){
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
    particle_->masterColor_.value.w = MyMath::Lerp(1.0f, kOutAlpha_, ease);
}
