#include "PlayerState_Jump.h"
#include "PlayerState_Idle.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
PlayerState_Jump::PlayerState_Jump(const std::string& stateName, BaseCharacter* player){
    Initialize(stateName, player);
    pCharacter_->SetAnimation("jump", true);
}

PlayerState_Jump::~PlayerState_Jump(){}

void PlayerState_Jump::Initialize(const std::string& stateName, BaseCharacter* player){
    ICharacterState::Initialize(stateName, player);

    // ジャンプの初期化
    isJump_ = true;
    isDrop_ = true;
    jumpVelocity_ = jumpPower_;
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Jump::Update(){

    // ジャンプ処理
    Jump();

    // 移動処理(ジャンプしながらも動ける)
    Move();

    // 回転処理
    if(MyMath::Length(Input::GetStickValue(LR::LEFT))){
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
    if(!isJump_ && !isDrop_){
        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle",pCharacter_));
    }
}

//////////////////////////////////////////////////////////////////////////
// ジャンプ処理
//////////////////////////////////////////////////////////////////////////
void PlayerState_Jump::Jump(){

    // 落下速度を加算
    jumpVelocity_ += gravity_ * (isJump_ * isDrop_) * ClockManager::DeltaTime();
    pCharacter_->HandleMove(Vector3(0.0f, jumpVelocity_, 0.0f));

    // ジャンプ終了処理
    if(pCharacter_->GetWorldTranslate().y <= 0.0f){
        isJump_ = false;
        isDrop_ = false;
        jumpVelocity_ = 0.0f;
    }
}
