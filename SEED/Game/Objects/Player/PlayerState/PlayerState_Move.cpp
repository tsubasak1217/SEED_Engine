#include "PlayerState_Move.h"

// parentObject
#include "Player/Player.h"
// Others State
#include "PlayerState_Jump.h"
#include "PlayerState_Idle.h"
#include "PlayerState_ThrowEgg.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
PlayerState_Move::PlayerState_Move(const std::string &stateName, BaseCharacter *player)
{
    Initialize(stateName, player);
    pCharacter_->SetAnimation("running", true);
}

PlayerState_Move::~PlayerState_Move() {}

void PlayerState_Move::Initialize(const std::string &stateName, BaseCharacter *player)
{
    ICharacterState::Initialize(stateName, player);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Update()
{
    // 移動処理
    Move();
    // 回転処理
    Rotate();
    // state遷移
    ManageState();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Draw() {}

//////////////////////////////////////////////////////////////////////////
// 移動処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Move()
{
    DecideStickVelocity();
    pCharacter_->HandleMove(acceleration_);
}

//////////////////////////////////////////////////////////////////////////
// スティックの入力から移動量を決定
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::DecideStickVelocity()
{

    // スティックの入力を取得
    stickDirection_ = Input::GetStickValue(LR::LEFT);

    // カメラの回転を考慮して補正
    Player *pPlayer_ = dynamic_cast<Player *>(pCharacter_);
    if (pPlayer_->GetFollowCamera())
    {
        stickDirection_ *= RotateMatrix(-pPlayer_->GetFollowCamera()->GetRotation().y);
    }

    // ダッシュ
    bool isDash = Input::IsPressPadButton(PAD_BUTTON::Y);

    if (isDash)
    {
        pCharacter_->SetAnimationSpeedRate(3.0f);
    }
    else
    {
        pCharacter_->SetAnimationSpeedRate(1.0f);
    }

    acceleration_ = Vector3(stickDirection_.x, 0.0f, stickDirection_.y) * moveSpeed_ * ClockManager::DeltaTime() * (1.0f + (isDash * 2.0f));
}

//////////////////////////////////////////////////////////////////////////
// 移動ベクトルから回転適用
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Rotate()
{
    // 移動ベクトルを求める
    moveVec_ = pCharacter_->GetWorldTranslate() - pCharacter_->GetPrePos();

    // 移動ベクトルから回転を求める
    if (MyMath::Length(moveVec_) > 0.0f)
    {
        Vector3 rotateVec_ = MyFunc::CalcRotateVec(moveVec_);
        rotateVec_.x = 0.0f;

        if (isLerpRotate_)
        { // 補間回転する場合

            // 補間後の回転を求める
            Vector3 lerped = Quaternion::ToEuler(
                Quaternion::Slerp(
                    pCharacter_->GetWorldRotate(),
                    rotateVec_,
                    lerpRate_ * ClockManager::TimeRate()));

            pCharacter_->HandleRotate(lerped);
        }
        else
        {
            pCharacter_->HandleRotate(rotateVec_);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::ManageState()
{

    // ジャンプ状態へ
    if (Input::IsPressPadButton(PAD_BUTTON::A))
    {
        pCharacter_->ChangeState(new PlayerState_Jump("Player_Jump", pCharacter_));
        return;
    }

    // アイドル状態へ
    if (MyMath::Length(moveVec_) == 0.0f)
    {
        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle", pCharacter_));
        return;
    }

    // 卵 を 投げる状態へ
    if (Input::IsPressPadButton(PAD_BUTTON::RB))
    {
        Player *pPlayer_ = dynamic_cast<Player *>(pCharacter_);
        if(pPlayer_->GetEggManager()->GetIsEmpty()){
            return;
        }
        pCharacter_->ChangeState(new PlayerState_ThrowEgg("Player_ThrowEgg",pPlayer_));
        return;
    }
}
