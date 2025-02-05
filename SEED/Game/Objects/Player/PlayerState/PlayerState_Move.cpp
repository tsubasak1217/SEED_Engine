#include "PlayerState_Move.h"

// parentObject
#include "Player/Player.h"
// Others State
#include "PlayerState_Jump.h"
#include "PlayerState_Idle.h"
#include "PlayerState_ThrowEgg.h"
#include "PlayerState_Eat.h"
//lib
#include "../adapter/json/JsonCoordinator.h"
#include "../PlayerInput/PlayerInput.h"

// Egg
#include "Egg/Egg.h"
// EggState
#include "Egg/State/EggState_Thrown.h"

// Manager
#include "ParticleManager.h"

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

    // Jsonから値を取得
    JsonCoordinator::RegisterItem("Player","moveSpeed",moveSpeed_);
    JsonCoordinator::RegisterItem("Player","lerpRate",lerpRate_);
    JsonCoordinator::RegisterItem("Player","isLerpRotate",isLerpRotate_);
    // effect
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

    //particle
    EmitParticle();
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
    stickDirection_ = PlayerInput::CharacterMove::GetCharacterMoveDirection();

    // カメラの回転を考慮して補正
    Player *pPlayer_ = dynamic_cast<Player *>(pCharacter_);
    if (pPlayer_->GetFollowCamera())
    {
        stickDirection_ *= RotateMatrix(-pPlayer_->GetFollowCamera()->GetRotation().y);
    }

    {
        pCharacter_->SetAnimationSpeedRate(1.0f);
    }

    acceleration_ = Vector3(stickDirection_.x, 0.0f, stickDirection_.y) * moveSpeed_ * ClockManager::DeltaTime();
}

//////////////////////////////////////////////////////////////////////////
// 移動ベクトルから回転適用
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::Rotate()
{
    // 移動ベクトルを求める
    moveVec_ = pCharacter_->GetWorldTranslate() - pCharacter_->GetPrePos();

    // 親オブジェクトがある場合は親の移動分を引く
    if(pCharacter_->GetParent()){
        Vector3 parentWorldPos = pCharacter_->GetParent()->GetWorldTranslate();
        Vector3 parentPreWorldPos = pCharacter_->GetParent()->GetPrePos();
        Vector3 parentMoveVec = parentWorldPos - parentPreWorldPos;
        moveVec_ -= parentMoveVec;
    }

    // 移動ベクトルから回転を求める
    if (MyMath::LengthSq(moveVec_) > 0.0f)
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

void PlayerState_Move::EmitParticle(){
leftEmitTime_ -= ClockManager::DeltaTime();
    if(leftEmitTime_ <= 0.f){
       leftEmitTime_ = emitTime_;
       ParticleManager::AddEffect("dinosaur_run.json",{0.f,0.f,-.4f},pCharacter_->GetWorldMatPtr());
    }
}

//////////////////////////////////////////////////////////////////////////
// ステート管理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Move::ManageState(){

    if(!pCharacter_->GetIsMovable()){ return; }

    // ジャンプ状態へ
    if(PlayerInput::CharacterMove::Jump()){
        if(pCharacter_->IsJumpable()){
            pCharacter_->ChangeState(new PlayerState_Jump("Player_Jump",pCharacter_));
            return;
        }
    }

    // アイドル状態へ
    if(MyMath::LengthSq(moveVec_) == 0.0f){
        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
        return;
    }

    // 卵 を 投げる状態へ
    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    if(PlayerInput::CharacterMove::FocusEggInput()){
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

    // 捕食
    if(PlayerInput::CharacterMove::Eat()){
        if(pPlayer->CanEatEnemy()){
            pCharacter_->ChangeState(new PlayerState_Eat(pCharacter_));
            return;
        }
    }
}
