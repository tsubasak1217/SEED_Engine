#include "PlayerThrowEggBehavior.h"

// Others Behavior
#include "PlayerRootBehavior.h"

// stl
#include <algorithm>

// Engine
#include "ClockManager.h"
#include "InputManager.h"

// Object
#include "../Player/Player.h"

#include "../Egg/Egg.h"
#include "../Egg/EggManager.h"

// math
#include "Matrix4x4.h"
#include "MatrixFunc.h"
#include "MyMath.h"

PlayerThrowEggBehavior::PlayerThrowEggBehavior(Player* _player)
    :IPlayerBehavior(_player){}

PlayerThrowEggBehavior::~PlayerThrowEggBehavior(){}

void PlayerThrowEggBehavior::Initialize(){
    // 入力された値を 取得 & Player の 方向 を 更新
    player_->RotateUpdate();

    // Player の 現在向いてる方向 v
    throwDirection_ = Vector3(0.0f,0.0f,1.0f) * RotateMatrix(player_->GetRotate());

    // Egg に Velocity を セット
    throwEgg_ = player_->GetEggManager()->GetFrontEgg().get();
}

void PlayerThrowEggBehavior::Update(){
    { // 投げる 方向 を 更新
        // 現在の 向いている方向 を 計算 & 更新 
        player_->RotateUpdate();

        // Player の 現在向いてる方向 v
        throwDirection_ = MyMath::Normalize(Vector3(0.0f,0.0f,1.0f) *
                                            RotateMatrix(player_->GetRotate()));

        { // 上方向の 更新
            currentThrowHeight_ += (maxThrowHeight_ * ClockManager::DeltaTime()) * updateSign_;
            // 上限値 or 下限値 まで 行ったら 反転 & clamp
            if(currentThrowHeight_ >= maxThrowHeight_){
                currentThrowHeight_ = maxThrowHeight_;
                updateSign_ *= -1.0f;
            } else if(currentThrowHeight_ <= minThrowHeight_){
                currentThrowHeight_ = minThrowHeight_;
                updateSign_ *= -1.0f;
            }
            throwDirection_.y = currentThrowHeight_;
        }

        // 方向の 更新が終わったら 正規化
        throwDirection_ = MyMath::Normalize(throwDirection_);
    }

    { // ThrowEgg の offset 計算
        // 固定値 仮Offset
        throwEgg_->SetTranslate(player_->GetWorldPos() + Vector3(0.0f,2.0f,0.0f));
    }


    { // 投げる 速度を 計算
        if(Input::IsPressPadButton(PAD_BUTTON::RB)){
            currentThrowHeight_ += updateThrowSpeedValue_ * ClockManager::DeltaTime();
        } else if(Input::IsPressPadButton(PAD_BUTTON::RT)){
            currentThrowHeight_ -= updateThrowSpeedValue_ * ClockManager::DeltaTime();
        }
        currentThrowSpeed_ = std::clamp(currentThrowSpeed_,minThrowSpeed_,maxThrowSpeed_);
    }

    // 押してなかったら 卵 を 投げる
    if(!Input::IsPressPadButton(PAD_BUTTON::A)){
        throwEgg_->ThrowThis(throwDirection_ * currentThrowSpeed_);
        // 投げたら 元の状態に戻る
        player_->ChangeBehavior(std::make_unique<PlayerRootBehavior>(player_));
    }
}