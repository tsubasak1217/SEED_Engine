#include "PlayerState_ThrowEgg.h"

// Others Behavior
#include "PlayerState_Idle.h"
//eggState
#include "Egg/State/EggState_Thrown.h"

// stl
#include <algorithm>

// Engine
#include "ClockManager.h"
#include "InputManager.h"

// Object
#include "../Player/Player.h"
#include "Egg/Egg.h"

// math
#include "Matrix4x4.h"
#include "MatrixFunc.h"
#include "MyMath.h"

PlayerState_ThrowEgg::PlayerState_ThrowEgg(BaseCharacter* _player,EggManager* _eggManager)
    :PlayerState_Move(_player),
    eggManager_(_eggManager){}

PlayerState_ThrowEgg::~PlayerState_ThrowEgg(){}

void PlayerState_ThrowEgg::Initialize(BaseCharacter* character){
    ICharacterState::Initialize(character);

    // Player の 現在向いてる方向 v
    throwDirection_ = Vector3(0.0f,0.0f,1.0f) * RotateMatrix(pCharacter_->GetWorldRotate());

    // Egg に Velocity を セット
    if(eggManager_->GetIsEmpty()){
        pCharacter_->ChangeState(new PlayerState_Idle(pCharacter_));
        return;
    }
    throwEgg_ = eggManager_->GetFrontEgg().get();

}

void PlayerState_ThrowEgg::Update(){
    // 移動＆回転
    PlayerState_Move::Move();
    PlayerState_Move::Rotate();

    // 卵 の 位置 を 更新
    throwEgg_->SetTranslate(pCharacter_->GetWorldTranslate() + (eggOffset_ * RotateYMatrix(pCharacter_->GetWorldRotate().y)));

    // ステート管理
    ManageState();
}

void PlayerState_ThrowEgg::Draw(){}

void PlayerState_ThrowEgg::ManageState(){
    // 卵 を 投げる状態へ
    if(Input::IsReleasePadButton(PAD_BUTTON::RB)){
        // Player の 現在向いてる方向
        throwDirection_ = Vector3(0.0f,0.0f,1.0f) * RotateMatrix(pCharacter_->GetWorldRotate());
        throwDirection_ = MyMath::Normalize(throwDirection_);
        Vector3 throwVelocity = throwDirection_ * throwPower_;
        throwEgg_->ChangeState(new EggState_Thrown(throwEgg_,throwDirection_));
        pCharacter_->ChangeState(new PlayerState_Idle(pCharacter_));
        return;
    }
}
