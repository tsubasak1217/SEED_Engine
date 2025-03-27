#include "SampleCharacterState_Idle.h"

// 状態クラスのインクルード
#include "SampleCharacterState_Jump.h"
#include "SampleCharacterState_Move.h"

// 状態を表されている主
#include <Game/Objects/Assets/Actor/SampleCharacter/SampleCharacter.h>

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
SampleCharacterState_Idle::SampleCharacterState_Idle(const std::string& stateName,BaseCharacter* player){
    Initialize(stateName,player);
}

SampleCharacterState_Idle::~SampleCharacterState_Idle(){}

void SampleCharacterState_Idle::Initialize(const std::string& stateName,BaseCharacter* player){
    ICharacterState::Initialize(stateName,player);
    pCharacter_->SetAnimation("idle",true);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void SampleCharacterState_Idle::Update(){

    // コライダーの更新
    for(auto& collider : colliders_){
        collider->Update();
    }

    HandOverColliders();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void SampleCharacterState_Idle::Draw(){}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void SampleCharacterState_Idle::ManageState(){

    if(!pCharacter_->GetIsMovable()){ return; }

    // ジャンプ状態へ
    if(Input::IsTriggerPadButton(PAD_BUTTON::A)){
        if(pCharacter_->IsJumpable()){
            pCharacter_->ChangeState(new SampleCharacterState_Jump("SampleCharacter_Jump",pCharacter_));
            return;
        }
    }

    // 移動
    if(MyMath::LengthSq(Input::GetStickValue(LR::LEFT))){
        pCharacter_->ChangeState(new SampleCharacterState_Move("SampleCharacter_Move",pCharacter_));
        return;
    }
}

