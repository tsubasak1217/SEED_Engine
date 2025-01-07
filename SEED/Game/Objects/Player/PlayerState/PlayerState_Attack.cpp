#include "PlayerState_Attack.h"
#include "PlayerState_Jump.h"
#include "PlayerState_Idle.h"

//////////////////////////////////////////////////////////////////////////
//　コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
PlayerState_Attack::PlayerState_Attack(const std::string& stateName, BaseCharacter* player){
    Initialize(stateName, player);
    pCharacter_->SetAnimation("punch", true);
    InitColliders(colliderNames_[phase_ - 1],ObjectType::PlayerAttack);
    // アニメーションの時間から攻撃時間を設定
    kAttackTime_ = pCharacter_->GetAnimationDuration();
    attackTime_ = kAttackTime_;
}

PlayerState_Attack::~PlayerState_Attack(){}

void PlayerState_Attack::Initialize(const std::string& stateName, BaseCharacter* player){
    ICharacterState::Initialize(stateName, player);
}

//////////////////////////////////////////////////////////////////////////
//　更新処理
//////////////////////////////////////////////////////////////////////////

void PlayerState_Attack::Update(){
    // コンボ時間を減らす
    attackTime_ -= ClockManager::DeltaTime();

    // コンボ処理
    Combo();

    //コライダーの更新
    UpdateColliders();
}

//////////////////////////////////////////////////////////////////////////
//　描画処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Attack::Draw(){
    // コライダーの描画
    for(auto& collider : colliders_){
        collider->Draw();
    }
}


//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Attack::ManageState(){

    // 途中でジャンプしたら終わる
    if(Input::IsPressPadButton(PAD_BUTTON::A)){
        pCharacter_->ChangeState(new PlayerState_Jump("Player_Jump", pCharacter_));
        isChangeState_ = true;
        return;
    }

    // 移動状態へ
    if(moveAble_){
        if(MyMath::Length(Input::GetStickValue(LR::LEFT))){
            pCharacter_->ChangeState(new PlayerState_Move("Player_Move", pCharacter_));
            isChangeState_ = true;
            return;
        }
    }

    // 時間が来たらアイドル状態へ
    if(attackTime_ < 0.0f){
        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle", pCharacter_));
        isChangeState_ = true;
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

            // コライダーの変更
            InitColliders(colliderNames_[phase_ - 1], ObjectType::PlayerAttack);

            // アニメーションを変更
            pCharacter_->SetAnimation(animationNames_[phase_ - 1], false);
            kAttackTime_ = pCharacter_->GetAnimationDuration();
            attackTime_ = kAttackTime_;
            moveAble_ = false;
        }
    }

}
