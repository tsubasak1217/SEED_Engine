#include "PlayerState_Idle.h"
// 状態クラスのインクルード
#include "PlayerState_Jump.h"
#include "PlayerState_Move.h"
#include "PlayerState_ThrowEgg.h"

//状態を表されている主
#include "Player/Player.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
PlayerState_Idle::PlayerState_Idle(BaseCharacter* player){
    Initialize(player);
}

PlayerState_Idle::~PlayerState_Idle(){}

void PlayerState_Idle::Initialize(BaseCharacter* player){
    ICharacterState::Initialize(player);
    pCharacter_->SetAnimation("idle",true);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Idle::Update(){
    // ステート管理
    ManageState();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Idle::Draw(){}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Idle::ManageState(){

    // ジャンプ状態へ
    if(Input::IsPressPadButton(PAD_BUTTON::A)){
        pCharacter_->ChangeState(new PlayerState_Jump(pCharacter_));
        return;
    }

    // 移動
    if(MyMath::Length(Input::GetStickValue(LR::LEFT))){
        pCharacter_->ChangeState(new PlayerState_Move(pCharacter_));
        return;
    }

    // 卵 を 投げる状態へ
    if(Input::IsTriggerPadButton(PAD_BUTTON::RB)){
        Player* pPlayer_ = dynamic_cast<Player*>(pCharacter_);
        pCharacter_->ChangeState(new PlayerState_ThrowEgg(pPlayer_,pPlayer_->GetEggManager()));
        return;
    }
}
