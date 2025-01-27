#include "PlayerState_Jump.h"
//other state
#include "PlayerState_Idle.h"

//lib
#include "../adapter/json/JsonCoordinator.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
PlayerState_Jump::PlayerState_Jump(const std::string& stateName,BaseCharacter* player){
    Initialize(stateName,player);
    pCharacter_->SetAnimation("jump",true);
}

PlayerState_Jump::~PlayerState_Jump(){
    pCharacter_->SetIsJump(false);
    pCharacter_->SetIsApplyGravity(true);
    pCharacter_->SetJumpPower(0.0f);
}

void PlayerState_Jump::Initialize(const std::string& stateName,BaseCharacter* player){
    ICharacterState::Initialize(stateName,player);

    // Jsonから値を取得
    JsonCoordinator::RegisterItem("Player","jumpPower",jumpPower_);
    JsonCoordinator::RegisterItem("Player","MoveSpeedOnJump",moveSpeed_);
    JsonCoordinator::RegisterItem("Player","hoveringTime",hoveringTime_);
    JsonCoordinator::RegisterItem("Player","jumpHoveringAccel",jumpHoveringAccel_);
    // ジャンプの初期化
    pCharacter_->SetIsJump(true);
    pCharacter_->SetJumpPower(jumpPower_);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Jump::Update(){

    // ジャンプ処理
    Hovering();

    // 移動処理(ジャンプしながらも動ける)
    Move();

    // 回転処理
    if(MyMath::Length(Input::GetStickValue(LR::LEFT))){
        Rotate();
    }
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Jump::Draw(){}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Jump::ManageState(){
    if(hoveringState_ == HoveringState::Hovering){
        return;
    }
    // 着地
    if(!pCharacter_->GetIsJump() or !pCharacter_->GetIsDrop()){
        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
    }
}

//////////////////////////////////////////////////////////////////////////
// ジャンプ処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Jump::Hovering(){
    switch(hoveringState_){
        case HoveringState::MoveUp:
            if(pCharacter_->GetVelocity().y < 0.0f){
                hoveringState_ = HoveringState::Hovering;
            }
            break;
        case HoveringState::Hovering:
            hoveringTime_ -= ClockManager::DeltaTime();

            pCharacter_->SetJumpPower(jumpHoveringAccel_);

            pCharacter_->SetIsApplyGravity(false);
            if(!Input::GetInstance()->IsPressPadButton(PAD_BUTTON::A) || hoveringTime_ <= 0.0f){
                hoveringState_ = HoveringState::MoveDown;

                pCharacter_->SetIsApplyGravity(true);
            }
            break;
        case HoveringState::MoveDown:
            break;
        default:
            break;
    }
}
