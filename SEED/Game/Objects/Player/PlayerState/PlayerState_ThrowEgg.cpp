#include "PlayerState_ThrowEgg.h"

// Others Behavior
#include "PlayerState_Idle.h"
//eggState
#include "Egg/State/EggState_Thrown.h"
#include "Egg/State/EggState_Idle.h"
// stl
#include <algorithm>

// Engine
#include "ClockManager.h"
#include "InputManager.h"

// Object
#include "../Player/Player.h"
#include "Egg/Egg.h"
//lib
#include "JsonManager/JsonCoordinator.h"


// math
#include "Matrix4x4.h"
#include "MatrixFunc.h"
#include "MyMath.h"

PlayerState_ThrowEgg::PlayerState_ThrowEgg(const std::string& stateName,BaseCharacter* player){
    Initialize(stateName,player);
}

PlayerState_ThrowEgg::~PlayerState_ThrowEgg(){}

void PlayerState_ThrowEgg::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    JsonCoordinator::RegisterItem("Player","eggOffset",eggOffset_);
    JsonCoordinator::RegisterItem("Player","throwPower",throwPower_);
    JsonCoordinator::RegisterItem("Player","throwDirectionOffset",throwDirectionOffset_);

    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    if(!pPlayer){
        assert(false);
    }
    eggManager_ = pPlayer->GetEggManager();

    throwEgg_ = eggManager_->GetFrontEgg().get();
    throwEgg_->ChangeState(new EggState_Idle(throwEgg_));

    // Player の 現在向いてる方向 v
    throwDirection_ = throwDirectionOffset_ * RotateMatrix(pCharacter_->GetWorldRotate());

}

void PlayerState_ThrowEgg::Update(){
    // 移動＆回転
    PlayerState_Move::Move();
    PlayerState_Move::Rotate();

    throwDirectionOffset_= MyMath::Normalize(throwDirectionOffset_);

    // 卵 の 位置 を 更新
    throwEgg_->SetTranslate(pCharacter_->GetWorldTranslate() + (eggOffset_ * RotateYMatrix(pCharacter_->GetWorldRotate().y)));

    // ステート管理
    ManageState();
}

void PlayerState_ThrowEgg::Draw(){}

void PlayerState_ThrowEgg::ManageState(){
    // 卵 を 投げる状態へ
    if(Input::IsReleasePadButton(PAD_BUTTON::RB)){
        throwDirectionOffset_ = MyMath::Normalize(throwDirectionOffset_);
        // Player の 現在向いてる方向
        throwDirection_ = throwDirectionOffset_ * RotateMatrix(pCharacter_->GetWorldRotate());
        throwDirection_ = MyMath::Normalize(throwDirection_);
        Vector3 throwVelocity = throwDirection_ * throwPower_;
        throwEgg_->ChangeState(new EggState_Thrown(throwEgg_,throwDirection_));

        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
        return;
    }
}
