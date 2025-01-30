#include "BaseCharacter.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "ICharacterState.h"
#include "TextureManager/TextureManager.h"
#include "SEED.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
BaseCharacter::BaseCharacter(): BaseObject(){
    name_ = "unnamed";
    Initialize();
}

BaseCharacter::~BaseCharacter(){}

void BaseCharacter::Initialize(){
    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/man.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;
}


//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::Update(){

    // 状態に応じた更新処理
    if(currentState_){
        // 更新処理
        currentState_->Update();
    }

    // velocityの更新
    velocity_.y = jumpPower_ + dropSpeed_;
    MoveByVelocity();

    // モデルの更新
    model_->Update();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::Draw(){

    // 状態に応じた描画処理
    if(currentState_){
        currentState_->Draw();
    }

    // 基本モデルの描画
    BaseObject::Draw();
}

//////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////

void BaseCharacter::BeginFrame(){

    // 前フレームの座標の保存
    prePos_ = GetWorldTranslate();

    // フラグの初期化
    isDamaged_ = false;

    BaseObject::BeginFrame();
}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::EndFrame(){

    // 基本クラスの終了処理
    BaseObject::EndFrame();

    // ジャンプフラグの管理
    EndFrameJumpFlagUpdate();


    // 状態に応じた終了処理
    if(currentState_){
        currentState_->EndFrame();
    }

}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時の落下関連の更新処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::EndFrameJumpFlagUpdate(){

    // ジャンプの許容時間
    static const float kJumpAllowableTime = 0.1f;

    if(!isDrop_){
        isJump_ = false;
        jumpPower_ = 0.0f;
        jumpAllowableTime_ = kJumpAllowableTime;
    } else{
        jumpAllowableTime_ -= ClockManager::DeltaTime();
    }

    if(isJump_){
        isDrop_ = true;
    }
}



//////////////////////////////////////////////////////////////////////////
// ステート関連
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::ChangeState(ICharacterState* nextState){
    currentState_.reset(nextState);
}

void BaseCharacter::HandleMove(const Vector3& acceleration){
    // 移動
    model_->translate_ += acceleration;
}

void BaseCharacter::HandleRotate(const Vector3& rotate){
    // 回転
    model_->rotate_ = rotate;
    model_->UpdateMatrix();
}

//////////////////////////////////////////////////////////////////////////
// 衝突判定関連
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::HandOverColliders(){

    // 基本コライダーを渡す
    BaseObject::HandOverColliders();

    // state固有のコライダーを渡す
    if(currentState_){
        currentState_->HandOverColliders();
    }
}

// コライダーの初期化
void BaseCharacter::InitColliders(ObjectType objectType){
    BaseObject::InitColliders(objectType);
    colliders_;
    // state固有のコライダーを初期化
    if(currentState_){
        currentState_->InitColliders(objectType);
    }
}

// 前フレームのコライダーの破棄
void BaseCharacter::DiscardPreCollider(){
    BaseObject::DiscardPreCollider();
    // state固有のコライダーの破棄
    if(currentState_){
        currentState_->DiscardPreCollider();
    }
}

// ダメージを受ける処理
void BaseCharacter::Damage(int32_t damage){
    HP_ -= damage;
    if(HP_ <= 0){
        isAlive_ = false;
    }

    isDamaged_ = true;
}