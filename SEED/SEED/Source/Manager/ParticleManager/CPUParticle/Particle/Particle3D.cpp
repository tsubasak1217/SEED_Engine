#include "Particle3D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/Emitter3D.h>


namespace SEED{
    Particle3D::Particle3D(EmitterBase* emitter){

        // Emitter3Dにキャスト
        Emitter3D* modelEmitter = dynamic_cast<Emitter3D*>(emitter);
        if(!modelEmitter){
            return;
        }

        ////////////////// パーティクルのモデルを生成 /////////////////////
        particle_ = std::make_unique<Model>(modelEmitter->emitModelFilePath_);
        particle_->isParticle_ = true;
        particle_->cullMode_ = modelEmitter->cullingMode_; // カリングモードを設定

        ///////////////////// 寿命をランダム決定 ////////////////////////

        lifetimer_.Initialize(Methods::Rand::Random(modelEmitter->lifeTimeRange_.min, modelEmitter->lifeTimeRange_.max));

        /////////////////////// フラグ類を決定 ///////////////////////////

        isBillboard_ = modelEmitter->isBillboard_;
        isUseGravity_ = modelEmitter->isUseGravity_;
        isUseRotate_ = modelEmitter->isUseRotate_;

        ////////////////////// 座標をランダム決定 ////////////////////////
        Range3D emitRange;
        emitRange.min = -modelEmitter->emitRange_ * 0.5f;
        emitRange.max = modelEmitter->emitRange_ * 0.5f;
        emitPos_ = Methods::Rand::Random(emitRange);
        emitPos_ *= Methods::Matrix::RotateMatrix(modelEmitter->center_.rotate);
        emitPos_ += modelEmitter->GetCenter();
        particle_->transform_.translate = emitPos_;

        ///////////////////// 大きさをランダム決定 ////////////////////////
        float radius = Methods::Rand::Random(modelEmitter->radiusRange_.min, modelEmitter->radiusRange_.max);
        particle_->transform_.scale = Methods::Rand::Random(modelEmitter->scaleRange_) * radius;
        kScale_ = particle_->transform_.scale;

        ///////////////////// 速度を決定 ////////////////////////

        speed_ = Methods::Rand::Random(modelEmitter->speedRange_.min, modelEmitter->speedRange_.max);

        /////////////////////// 進行方向を決定 ////////////////////////

        direction_ = modelEmitter->baseDirection_; // 基本方向を設定

        // 目標位置が設定されている場合、方向を目標位置に向ける
        if(modelEmitter->isSetGoalPosition_){
            // 終了位置を設定
            endPos_ = modelEmitter->GetCenter() + modelEmitter->goalPosition_;
        }


        // 進行方向を範囲内でばらけさせる
        float angleRange = 3.14f * std::clamp(modelEmitter->directionRange_, 0.0f, 1.0f);
        float theta = Methods::Rand::Random(-angleRange, angleRange); // 水平回転
        float phi = Methods::Rand::Random(-angleRange / 2.0f, angleRange / 2.0f); // 垂直回転 (範囲を制限)
        Vector3 randomDirection = {// 球座標から方向ベクトルを計算
            std::cos(phi) * std::cos(theta),
            std::cos(phi) * std::sin(theta),
            std::sin(phi)
        };

        // 基本方向とランダムな方向を組み合わせて最終的な方向を決定
        direction_ = randomDirection * Quaternion::DirectionToDirection({ 1.0f, 0.0f, 0.0f }, direction_); // 回転を適用


        //////////////////////// 回転情報を決定 ////////////////////////

        // 回転軸指定がある場合、軸を決定
        if(modelEmitter->useRotateDirection_){
            rotateAxis_ = modelEmitter->rotateDirection_; // 基本方向を使用
        } else{
            rotateAxis_ = Methods::Rand::RandomVector();
        }

        // ランダム初期化フラグがある場合
        if(modelEmitter->isRoteteRandomInit_){
            particle_->transform_.rotate = Quaternion::ToQuaternion(rotateAxis_ * Methods::Rand::Random(-3.14f, 3.14f));
            localRotate_ = particle_->transform_.rotate;
        }

        // 回転速度の決定
        rotateSpeed_ = Methods::Rand::Random(modelEmitter->rotateSpeedRange_.min, modelEmitter->rotateSpeedRange_.max);

        //////////////////////// 重力を決定 //////////////////////////
        gravity_ = modelEmitter->gravity_;

        ////////////////////// 色をランダム決定 ////////////////////////
        masterColor_ = modelEmitter->colors_[Methods::Rand::Random(0, (int)modelEmitter->colors_.size() - 1)];

        ///////////////////// ブレンドモードを設定 ////////////////////////
        particle_->blendMode_ = modelEmitter->blendMode_;

        //////////////////////// ライト設定 //////////////////////////
        particle_->lightingType_ = (int32_t)modelEmitter->lightingType_;

        //////////////////////// テクスチャを設定 ////////////////////////

        // モデルのテクスチャを取得し、無ければ追加
        if(!modelEmitter->useDefaultTexture_){
            // カスタムテクスチャを使用する場合、メッシュごとにランダムなテクスチャを設定
            for(auto& material : particle_->materials_){
                textureHandle_ = TextureManager::LoadTexture(
                    modelEmitter->texturePaths_[Methods::Rand::Random(0, (int)modelEmitter->texturePaths_.size() - 1)]
                );
                material.GH = textureHandle_;
            }
        }

        /////////////////////// カーブの設定 /////////////////////////

        rotateCurve_ = modelEmitter->rotateCurve_;
        velocityCurve_ = modelEmitter->velocityCurve_;
        colorCurve_ = modelEmitter->colorCurve_;
        scaleCurve_ = modelEmitter->scaleCurve_;
        positionInterpolationCurve_ = modelEmitter->positionInterpolationCurve_;

        ///////////////////// 色操作のモード /////////////////////////

        colorMode_ = modelEmitter->colorMode_;

        //////////////////////// 初回更新時間 ////////////////////////
        if(modelEmitter->initUpdateTime_ > 0.0f){
            initUpdateTime_ = modelEmitter->initUpdateTime_;
        }
    }

    void Particle3D::Update(){

        float t = lifetimer_.GetProgress();
        float deltaTime = ClockManager::DeltaTime();

        // 初回アップデート時間が設定されている場合はその時間を使用
        if(initUpdateTime_ > 0.0f){
            deltaTime += initUpdateTime_;
            lifetimer_.currentTime += initUpdateTime_;
            initUpdateTime_ = 0.0f; // 空に戻す
        }

        //////////////////////////////////////
        // 寿命を減らす
        //////////////////////////////////////

        lifetimer_.Update();

        //////////////////////////////////
        // 速度の計算
        //////////////////////////////////

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
        particle_->transform_.translate += velocity_;


        // 目標地点がある場合は徐々に近づける
        if(endPos_ != std::nullopt){
            // 明確な目標地点がある場合
            particle_->transform_.translate = Methods::Math::Lerp(
                particle_->transform_.translate,
                endPos_.value(),
                positionInterpolationCurve_.GetValue(t)
            );
        }

        //////////////////////////////////
        // 回転処理
        //////////////////////////////////

        if(isBillboard_){
            if(!isUseRotate_){
                // 通常のビルボード
                particle_->transform_.rotate = SEED::Instance::GetMainCamera()->GetRotation();
            } else{
                // ビルボードしながら任意回転(ずっと動かずに回転を見てる感じになる)
                localRotate_ *= Quaternion::AngleAxis(
                    rotateSpeed_ * rotateCurve_.GetValue(t) * ClockManager::DeltaTime(),
                    rotateAxis_
                );

                // ビルボードの回転と任意回転を合成
                particle_->transform_.rotate = SEED::Instance::GetMainCamera()->GetRotation() * localRotate_;
            }
        } else{
            if(isUseRotate_){
                // 通常の回転
                particle_->transform_.rotate *= Quaternion::AngleAxis(
                    rotateSpeed_ * rotateCurve_.GetValue(t) * ClockManager::DeltaTime(),
                    rotateAxis_
                );
            }
        }


        //////////////////////////////////
        // スケール処理
        //////////////////////////////////

        particle_->transform_.scale = kScale_ * scaleCurve_.GetValue3(t);

        //////////////////////////////////
        // パーティクルのトランスフォーム更新
        //////////////////////////////////
        particle_->UpdateMatrix();

        //////////////////////////////////
        // 色の更新
        //////////////////////////////////

        // 色の扱いがRGBAかHSVAかで分岐
        if(colorMode_ == GeneralEnum::ColorMode::RGBA){
            particle_->masterColor_ = colorCurve_.GetValue4(t);
        } else{
            particle_->masterColor_ = Methods::Math::HSV_to_RGB(colorCurve_.GetValue4(t));
        }
    }

    void Particle3D::Draw(){
        // パーティクル描画
        particle_->Draw(masterColor_);
    }
}