#include "PlayerState_Move.h"
#include "PlayerState_Jump.h"
#include "PlayerState_Attack.h"
#include "PlayerState_Idle.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
PlayerState_Move::PlayerState_Move(Player* player){
    Initialize(player);
}

PlayerState_Move::~PlayerState_Move(){}

void PlayerState_Move::Initialize(Player* player){
    IPlayerState::Initialize(player);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Update(){
    // 移動処理
    Move();
    // 回転処理
    Rotate();
    // ステート管理
    ManageState();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Draw(){}

//////////////////////////////////////////////////////////////////////////
// 移動処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Move(){
    DecideStickVelocity();
    pPlayer_->HandleMove(acceleration_);
}

//////////////////////////////////////////////////////////////////////////
// スティックの入力から移動量を決定
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::DecideStickVelocity(){

    // スティックの入力を取得
    stickDirection_ = Input::GetStickValue(LR::LEFT);

    // カメラの回転を考慮して補正
    if(pPlayer_->GetFollowCamera()){
        stickDirection_ *= RotateMatrix(-pPlayer_->GetFollowCamera()->GetRotation().y);
    }

    acceleration_ = Vector3(stickDirection_.x, 0.0f, stickDirection_.y) * moveSpeed_ * ClockManager::DeltaTime();
}

//////////////////////////////////////////////////////////////////////////
// 移動ベクトルから回転適用
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Rotate(){
    // 移動ベクトルを求める
    moveVec_ = pPlayer_->GetWorldTranslate() - pPlayer_->GetPrePos();

    // 移動ベクトルから回転を求める
    if(MyMath::Length(moveVec_) > 0.0f){
        pPlayer_->HandleRotate(Vector3(0.0f,MyFunc::CalcRotateVec(moveVec_).y,0.0f));
    }
}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::ManageState(){

    // ジャンプ状態へ
    if(Input::IsPressPadButton(PAD_BUTTON::A)){
        pPlayer_->ChangeState(new PlayerState_Jump(pPlayer_));
        return;
    }

    // 攻撃状態へ
    if(Input::IsPressPadButton(PAD_BUTTON::B)){
        pPlayer_->ChangeState(new PlayerState_Attack(pPlayer_));
        return;
    }

    // アイドル状態へ
    if(MyMath::Length(moveVec_) == 0.0f){
        pPlayer_->ChangeState(new PlayerState_Idle(pPlayer_));
        return;
    }

}
