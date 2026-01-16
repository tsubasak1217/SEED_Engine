#include "Particle2D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/Emitter2D.h>

namespace SEED{
    Particle2D::Particle2D(EmitterBase* emitter){

        // Emitter3Dにキャスト
        Emitter2D* emitter2D = dynamic_cast<Emitter2D*>(emitter);
        if(!emitter2D){
            return;
        }

        ///////////////////// 寿命をランダム決定 ////////////////////////

        lifetimer_.Initialize(Methods::Rand::Random(emitter2D->lifeTimeRange_.min, emitter2D->lifeTimeRange_.max));

        /////////////////////// フラグ類を決定 ///////////////////////////

        isUseGravity_ = emitter2D->isUseGravity_;
        isUseRotate_ = emitter2D->isUseRotate_;

        ///////////////////// 大きさをランダム決定 ////////////////////////
        float radius = Methods::Rand::Random(emitter2D->radiusRange_.min, emitter2D->radiusRange_.max);
        particle_ = Methods::Shape::MakeEqualQuad2D(1.0f);
        particle_.scale = Methods::Rand::Random(emitter2D->scaleRange_) * radius;
        kScale_ = particle_.scale;

        ////////////////////// 座標をランダム決定 ////////////////////////
        Range2D emitRange;
        emitRange.min = -emitter2D->emitRange_ * 0.5f;
        emitRange.max = emitter2D->emitRange_ * 0.5f;
        emitPos_ = Methods::Rand::Random(emitRange);
        emitPos_ *= Methods::Matrix::RotateMatrix(emitter2D->center_.rotate);
        emitPos_ += emitter2D->GetCenter();
        particle_.translate = emitPos_;


        ///////////////////// 速度を決定 ////////////////////////

        speed_ = Methods::Rand::Random(emitter2D->speedRange_.min, emitter2D->speedRange_.max);

        /////////////////////// 進行方向を決定 ////////////////////////

        // 目標位置が設定されている場合は終了位置を設定
        if(emitter2D->isSetGoalPosition_){
            // 終了位置を設定
            endPos_ = emitter2D->GetCenter() + emitter2D->goalPosition_;
        }

        // 進行方向を範囲内でばらけさせる
        float angleRange = 3.14f * std::clamp(emitter2D->directionRange_, 0.0f, 1.0f);
        float theta = Methods::Rand::Random(-angleRange, angleRange);

        // 基本方向とランダムな方向を組み合わせて最終的な方向を決定
        direction_ = emitter2D->baseDirection_ * Methods::Matrix::RotateMatrix(theta); // 回転を適用


        //////////////////////// 回転情報を決定 ////////////////////////

        // ランダム初期化フラグがある場合
        if(emitter2D->isRoteteRandomInit_){
            particle_.rotate = Methods::Rand::Random(-3.14f, 3.14f);
            localRotate_ = particle_.rotate;
        }

        // 回転速度の決定
        rotateSpeed_ = Methods::Rand::Random(emitter2D->rotateSpeedRange_.min, emitter2D->rotateSpeedRange_.max);

        //////////////////////// 重力を決定 //////////////////////////

        gravity_ = emitter2D->gravity_;

        ////////////////////// 色をランダム決定 ////////////////////////

        masterColor_ = emitter2D->colors_[Methods::Rand::Random(0, (int)emitter2D->colors_.size() - 1)];

        ///////////////////// ブレンドモードを設定 ////////////////////////

        particle_.blendMode = emitter2D->blendMode_;

        //////////////////////// テクスチャを設定 ////////////////////////

        textureHandle_ = TextureManager::LoadTexture(
            emitter2D->texturePaths_[Methods::Rand::Random(0, (int)emitter2D->texturePaths_.size() - 1)]
        );
        particle_.GH = textureHandle_;

        /////////////////////// カーブの設定 /////////////////////////

        rotateCurve_ = emitter2D->rotateCurve_;
        velocityCurve_ = emitter2D->velocityCurve_;
        colorCurve_ = emitter2D->colorCurve_;
        scaleCurve_ = emitter2D->scaleCurve_;
        positionInterpolationCurve_ = emitter2D->positionInterpolationCurve_;

        ///////////////////// 色操作のモード /////////////////////////

        colorMode_ = emitter2D->colorMode_;

        ///////////////////////// 描画設定 //////////////////////////

        particle_.layer = emitter2D->layer_;
        particle_.isApplyViewMat = emitter2D->isApplyViewMatrix_;
        particle_.drawLocation = emitter2D->drawLocation_;

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
        particle_.translate += velocity_;


        // 目標地点がある場合は徐々に近づける
        if(endPos_ != std::nullopt){
            // 明確な目標地点がある場合
            particle_.translate = Methods::Math::Lerp(
                particle_.translate,
                endPos_.value(),
                positionInterpolationCurve_.GetValue(t)
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
        // スケール処理
        //////////////////////////////////

        particle_.scale = kScale_ * scaleCurve_.GetValue2(t);

        //////////////////////////////////
        // 色の更新
        //////////////////////////////////

        // 色の扱いがRGBAかHSVAかで分岐
        if(colorMode_ == Enums::ColorMode::RGBA){
            particle_.color = colorCurve_.GetValue4(t);
        } else{
            particle_.color = Methods::Math::HSV_to_RGB(colorCurve_.GetValue4(t));
        }
    }

    void Particle2D::Draw(){
        // パーティクル描画
        SEED::Instance::DrawQuad2D(particle_, masterColor_);
    }
}