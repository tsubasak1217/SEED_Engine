#include "PlayerState_Attack.h"
#include "PlayerState_Jump.h"
#include "PlayerState_Idle.h"

//////////////////////////////////////////////////////////////////////////
//　コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
PlayerState_Attack::PlayerState_Attack(BaseCharacter* player){
    Initialize(player);
    pCharacter_->SetAnimation("punch", true);
    // アニメーションの時間から攻撃時間を設定
    kAttackTime_ = pCharacter_->GetAnimationDuration();
    attackTime_ = kAttackTime_;
}

PlayerState_Attack::~PlayerState_Attack(){}

void PlayerState_Attack::Initialize(BaseCharacter* player){
    IState::Initialize(player);
}

//////////////////////////////////////////////////////////////////////////
//　更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Attack::Update(){
    // コンボ時間を減らす
    attackTime_ -= ClockManager::DeltaTime();

    // コンボ処理
    Combo();

    // ステート管理
    ManageState();
}

//////////////////////////////////////////////////////////////////////////
//　描画処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Attack::Draw(){}


//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Attack::ManageState(){

    // 途中でジャンプしたら終わる
    if(Input::IsPressPadButton(PAD_BUTTON::A)){
        pCharacter_->ChangeState(new PlayerState_Jump(pCharacter_));
        return;
    }

    // 移動状態へ
    if(moveAble_){
        if(MyMath::Length(Input::GetStickValue(LR::LEFT))){
            pCharacter_->ChangeState(new PlayerState_Move(pCharacter_));
            return;
        }
    }

    // 時間が来たらアイドル状態へ
    if(attackTime_ < 0.0f){
        pCharacter_->ChangeState(new PlayerState_Idle(pCharacter_));
        return;
    }
}

void PlayerState_Attack::Combo(){

    float t = attackTime_ / kAttackTime_;

    if(t > 0.5f){ return; }

    // 時間がたったら移動可能にする
    moveAble_ = true;

    // 一定時間経過後にボタンを押したら
    if(Input::IsTriggerPadButton(PAD_BUTTON::B)){

        // 次のフェーズへ
        if(phase_ + 1 <= kMaxPhase_){
            phase_++;

            // アニメーションを変更
            pCharacter_->SetAnimation(animationNames_[phase_ - 1], true);
            kAttackTime_ = pCharacter_->GetAnimationDuration();
            attackTime_ = kAttackTime_;
            moveAble_ = false;
        }
    }

}
