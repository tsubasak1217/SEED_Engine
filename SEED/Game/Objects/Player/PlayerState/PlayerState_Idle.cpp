#include "PlayerState_Idle.h"
// 状態クラスのインクルード
#include "PlayerState_Jump.h"
#include "PlayerState_Move.h"
#include "PlayerState_ThrowEgg.h"
#include "PlayerState_Eat.h"

#include "Egg/Egg.h"
// EggState
#include "Egg/State/EggState_Thrown.h"

// 状態を表されている主
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

    // 卵 を 投げる状態へ
    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    if(Input::IsTriggerPadButton(PAD_BUTTON::LT)){
        if(pPlayer->GetEggManager()->GetIsEmpty()){
            return;
        }

        // すでに 投げているなら return
        Egg* pEgg = pPlayer->GetEggManager()->GetFrontEgg().get();
        if(pEgg->GetIsThrown()){
            return;
        }

        pCharacter_->ChangeState(new PlayerState_ThrowEgg("Player_ThrowEgg",pPlayer));
        return;
    }

    if(Input::IsReleasePadButton(PAD_BUTTON::RT)){
        if(pPlayer->GetEggManager()->GetIsEmpty()){
            return;
        }

        // すでに 投げているなら return
        Egg* pEgg = pPlayer->GetEggManager()->GetFrontEgg().get();
        if(pEgg->GetIsThrown()){
            return;
        }

        pEgg->SetVelocity({0.0f,8.0f,0.0f});
        // velocity で管理するため, 初期値は基本 0
        pEgg->ChangeState(new EggState_Thrown(pEgg,{0.0f,0.0f,0.0f},0.0f,0.0f));

        return;
    }

    // 捕食
    if(Input::IsReleasePadButton(PAD_BUTTON::B)){
        if(pPlayer->CanEatEnemy()){
            pCharacter_->ChangeState(new PlayerState_Eat(pCharacter_));
            return;
        }
    }
}

