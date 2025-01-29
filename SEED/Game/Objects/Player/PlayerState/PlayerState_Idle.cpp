#include "PlayerState_Idle.h"

// 状態クラスのインクルード
#include "PlayerState_Jump.h"
#include "PlayerState_Move.h"

// 主
#include "Player/Player.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
PlayerState_Idle::PlayerState_Idle(const std::string& stateName,BaseCharacter* player){
    Initialize(stateName,player);
}

PlayerState_Idle::~PlayerState_Idle(){}

void PlayerState_Idle::Initialize(const std::string& stateName,BaseCharacter* player){
    ICharacterState::Initialize(stateName,player);
    pCharacter_->SetAnimation("idle",true);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Idle::Update(){

    // コライダーの更新
    for(auto& collider : colliders_){
        collider->Update();
    }

    HandOverColliders();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Idle::Draw(){}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Idle::ManageState(){

    if(!pCharacter_->GetIsMovable()){ return; }

    // ジャンプ状態へ
    if(Input::IsTriggerPadButton(PAD_BUTTON::A)){
        if(pCharacter_->IsJumpable()){
            pCharacter_->ChangeState(new PlayerState_Jump("Player_Jump",pCharacter_));
            return;
        }
    }

    // 移動
    if(MyMath::Length(Input::GetStickValue(LR::LEFT))){
        pCharacter_->ChangeState(new PlayerState_Move("Player_Move",pCharacter_));
        return;
    }
}

