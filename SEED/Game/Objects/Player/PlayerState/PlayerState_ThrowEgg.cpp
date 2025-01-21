#include "PlayerState_ThrowEgg.h"

// Others Behavior
#include "PlayerState_Idle.h"
//eggState
#include "Egg/State/EggState_Thrown.h"
#include "Egg/State/EggState_Follow.h"
#include "Egg/State/EggState_Idle.h"
// stl
#include <algorithm>

// Engine
#include "SEED.h"
#include "ClockManager.h"
#include "InputManager.h"

// Object
#include "../Player/Player.h"
#include "Egg/Egg.h"
//lib
#include "../adapter/json/JsonCoordinator.h"

// math
#include "Matrix4x4.h"
#include "MatrixFunc.h"
#include "MyMath.h"
#include "MyFunc.h"

PlayerState_ThrowEgg::PlayerState_ThrowEgg(const std::string& stateName,BaseCharacter* player){
    Initialize(stateName,player);
}

PlayerState_ThrowEgg::~PlayerState_ThrowEgg(){}

void PlayerState_ThrowEgg::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    JsonCoordinator::RegisterItem("Player","eggOffset",eggOffset_);
    JsonCoordinator::RegisterItem("Player","throwPower",throwPower_);
    JsonCoordinator::RegisterItem("Player","throwDirection",throwDirection_);
    JsonCoordinator::RegisterItem("Player","pressForcus",pressForcus_);

    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    if(!pPlayer){
        assert(false);
    }
    eggManager_ = pPlayer->GetEggManager();

    throwEgg_ = eggManager_->GetFrontEgg().get();
    throwEgg_->ChangeState(new EggState_Idle(throwEgg_));
    eggWeight_ = dynamic_cast<Egg*>(throwEgg_)->GetWeight();
}

void PlayerState_ThrowEgg::Update(){
    // 移動＆回転
    PlayerState_Move::Move();
    PlayerState_Move::Rotate();

    // 移動状態の更新 & アニメーションの変更
    UpdateMovingState();
    ChangeAnimation();

    throwDirection_= MyMath::Normalize(throwDirection_);

    // 卵 の 位置 を 更新
    throwEgg_->SetTranslate(pCharacter_->GetWorldTranslate() + (eggOffset_ * RotateYMatrix(pCharacter_->GetWorldRotate().y)));
    throwEgg_->HandleRotate(pCharacter_->GetWorldRotate());

}

constexpr float timePerSegment = 0.1f / 32.0f;
void PlayerState_ThrowEgg::Draw(){

    Vector3 eggPos = throwEgg_->GetWorldTranslate();
    Vector3 preSegmentPos = eggPos;
    float index = 0.0f;
    while(true){
        // 地面につくまで描画する
        if(preSegmentPos.y <= 0.0f){
            return;
        }
        index += 1.0f;

        Vector2 parabolic2d =  MyFunc::CalculateParabolic(Vector2(throwDirection_.x,throwDirection_.y),throwPower_,timePerSegment * index,9.8f * eggWeight_);
        Vector3 parabolic3d = Vector3(0.0f,parabolic2d.y,parabolic2d.x) * RotateYMatrix(pCharacter_->GetWorldRotate().y) + eggPos;

        SEED::DrawLine(preSegmentPos,parabolic3d,Vector4(0.3f,1.0f,0.3f,1.0f));

        preSegmentPos = parabolic3d;
    }
}

void PlayerState_ThrowEgg::ManageState(){
    // 卵 を 投げる状態へ
    if(pressForcus_){
        if(Input::IsPressPadButton(PAD_BUTTON::LT)){
            if(Input::IsPressPadButton(PAD_BUTTON::RT)){
                //投げる
                throwEgg_->ChangeState(new EggState_Thrown(throwEgg_,throwDirection_,pCharacter_->GetWorldRotate().y,throwPower_));
                pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
                return;
            }
        } else{
            // もとに戻す
            throwEgg_->ChangeState(new EggState_Follow(throwEgg_,pCharacter_));
            pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
            return;
        }
    } else{ // 切り替え
        if(Input::IsTriggerPadButton(PAD_BUTTON::LT)){
            // もとに戻す
            throwEgg_->ChangeState(new EggState_Follow(throwEgg_,pCharacter_));
            pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
            return;
        }
        if(Input::IsPressPadButton(PAD_BUTTON::RT)){
            throwEgg_->ChangeState(new EggState_Thrown(throwEgg_,throwDirection_,pCharacter_->GetWorldRotate().y,throwPower_));
            pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
            return;
        }
    }
}

void PlayerState_ThrowEgg::UpdateMovingState(){
    preIsMoving_ = isMoving_;
    if(MyMath::LengthSq(pCharacter_->GetPrePos() - pCharacter_->GetWorldTranslate()) >= 0.0001f){
        isMoving_ = true;
    } else{
        isMoving_ = false;
    }
}

void PlayerState_ThrowEgg::ChangeAnimation(){
    // 移動状態が変わったら
    if(preIsMoving_ == isMoving_){
        return;
    }

    // 動いているなら
    if(isMoving_){
        pCharacter_->SetAnimation("running",true);
    } else{
        pCharacter_->SetAnimation("idle",true);
    }
}
