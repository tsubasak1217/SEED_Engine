#include "PlayerState_Jump.h"
//other state
#include "PlayerState_Idle.h"

//lib
#include <SEED/Source/Manager/AudioManager/AudioManager.h>

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

    // ジャンプの初期化
    pCharacter_->SetIsJump(true);
    pCharacter_->SetJumpPower(jumpPower_);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Jump::Update(){

    // 移動処理(ジャンプしながらも動ける)
    Move();

    // 回転処理
    if(MyMath::LengthSq(Input::GetStickValue(LR::LEFT))){
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
    // 着地
    if(!pCharacter_->GetIsJump() && !pCharacter_->GetIsDrop()){
        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
    }
}