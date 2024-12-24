#include "Player.h"
#include "InputManager.h"
#include "ImGuiManager.h"

// 状態クラスのインクルード
#include "PlayerState/IPlayerState.h"
#include "PlayerState/PlayerState_Idle.h"
#include "PlayerState/PlayerState_Attack.h"
#include "PlayerState/PlayerState_Jump.h"
#include "PlayerState/PlayerState_Move.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
Player::Player() : BaseObject(){
    name_ = "Player";
    Initialize();
}

Player::~Player(){}

void Player::Initialize(){

    // モデルの初期化
    model_ = std::make_unique<Model>("suzanne2.obj");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f, 3.0f, 0.0f);

    // 状態の初期化
    currentState_ = std::make_unique<PlayerState_Idle>(this);
}


//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void Player::Update(){

    // 前フレームの座標の保存
    prePos_ = GetWorldTranslate();

    // 状態に応じた更新処理
    if(currentState_){
        currentState_->Update();
    }

    BaseObject::Update();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Player::Draw(){

    // 状態に応じた描画処理
    if(currentState_){
        currentState_->Draw();
    }

    // 基本モデルの描画
    BaseObject::Draw();
}


//////////////////////////////////////////////////////////////////////////
// ステート関連
//////////////////////////////////////////////////////////////////////////
void Player::ChangeState(IPlayerState* nextState){
    currentState_.reset(nextState); 
}

void Player::HandleMove(const Vector3& acceleration){
    // 移動
    model_->translate_ += acceleration;

    // 移動制限
    model_->translate_.y = std::clamp(model_->translate_.y, 0.0f, 10000.0f);
    model_->UpdateMatrix();
}

void Player::HandleRotate(const Vector3& rotate){
    // 回転
    model_->rotate_ = rotate;
    model_->UpdateMatrix();
}
