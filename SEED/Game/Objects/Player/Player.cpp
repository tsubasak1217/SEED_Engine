#include "Player.h"
#include "InputManager.h"
#include "ImGuiManager.h"

// 状態クラスのインクルード
#include "PlayerState/PlayerState_Idle.h"
#include "PlayerState/PlayerState_Attack.h"
#include "PlayerState/PlayerState_Jump.h"
#include "PlayerState/PlayerState_Move.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
Player::Player() : BaseCharacter(){
    className_ = "Player";
    name_ = "Player";
    Initialize();
}

Player::~Player(){}

void Player::Initialize(){

    // 属性の決定
    objectType_ = ObjectType::Player;

    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/man.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;

    // コライダーの初期化
    InitColliders(ObjectType::Player);

    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, model_->GetWorldMatPtr());

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f, 3.0f, 0.0f);

    // 状態の初期化
    currentState_ = std::make_unique<PlayerState_Idle>("Player_Idle",this);
}


//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void Player::Update(){
    BaseCharacter::Update();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Player::Draw(){
    BaseCharacter::Draw();
}

//////////////////////////////////////////////////////////////////////////
// コライダー関連
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// ステート関連
//////////////////////////////////////////////////////////////////////////
void Player::HandleMove(const Vector3& acceleration){
    // 移動
    model_->translate_ += acceleration;

    // 移動制限
    model_->translate_.y = std::clamp(model_->translate_.y, 0.0f, 10000.0f);
    model_->UpdateMatrix();
}


//////////////////////////////////////////////////////////////////////////
// 衝突時処理
//////////////////////////////////////////////////////////////////////////
void Player::OnCollision(const BaseObject* other, ObjectType objectType){
    other;
    objectType;
}
