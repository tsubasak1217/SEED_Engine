#include "Player.h"

/// engine
// module
#include "ImGuiManager.h"
#include "InputManager.h"

// manager
#include "Egg/Manager/EggManager.h"
#include "Player/PredationRange/PredationRange.h"
#include "StageManager.h"
//lib
#include "../adapter/json/JsonCoordinator.h"

// 状態クラスのインクルード
#include "PlayerState/PlayerState_Idle.h"
#include "PlayerState/PlayerState_Jump.h"
#include "PlayerState/PlayerState_Move.h"
#include "PlayerState/PlayerState_Spawn.h"
#include "PlayerState/PlayerStage_ForNextStage.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
Player::Player(): BaseCharacter(){
    className_ = "Player";
    name_ = "Player";
    Initialize();

}

Player::~Player(){}

void Player::Initialize(){

    // 属性の決定
    objectType_ = ObjectType::Player;

    // モデルの初期化
    model_ = std::make_unique<Model>("dinosaur.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;

    // コライダーの初期化
    InitColliders(ObjectType::Player);
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(ObjectType::Player);
        collider->AddSkipPushBackType(ObjectType::Egg);
    }

    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f,3.0f,0.0f);

    // 状態の初期化
    currentState_ = std::make_unique<PlayerState_Idle>("Player_Idle",this);

    JsonCoordinator::LoadGroup("Player");

    // 捕食可能範囲の初期化
    predationRange_ = std::make_unique<PredationRange>();
    predationRange_->Initialize(this);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void Player::Update(){
#ifdef _DEBUG
    ImGui::Begin("Player");
    JsonCoordinator::RenderGroupUI("Player");
    if(ImGui::Button("Save")){
        JsonCoordinator::SaveGroup("Player");
    }
    ImGui::End();
#endif // _DEBUG

    if(enemyManager_){
        predationRange_->Update(enemyManager_);
    }

    BaseCharacter::Update();
}
//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Player::Draw(){
    BaseCharacter::Draw();
}

void Player::Spawn(const Vector3& pos){
    PlayerState_Spawn* state = new PlayerState_Spawn("Player_Spawn",this);
    state->SetSpawnPos(pos);
    ChangeState(state);
}

void Player::ToClearStageState(const Vector3& nextStartPos){
    ChangeState(new PlayerStage_ForNextStage(nextStartPos,this));
}

//////////////////////////////////////////////////////////////////////////
// コライダー関連
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ステート関連
//////////////////////////////////////////////////////////////////////////
void Player::HandleMove(const Vector3& acceleration){
    // 移動
    if(isMovable_){
        model_->translate_ += acceleration;
    }

    // 移動制限
    model_->translate_.y = std::clamp(model_->translate_.y,0.0f,10000.0f);
    model_->UpdateMatrix();
}

bool Player::CanEatEnemy(){
    return !predationRange_->GetPreyList().empty();
}

//////////////////////////////////////////////////////////////////////////
// 衝突時処理
//////////////////////////////////////////////////////////////////////////
void Player::OnCollision(const BaseObject* other,ObjectType objectType){
    other;
    objectType;

    // ゴールに触れている状態で
    if(objectType == ObjectType::GoalField){

        // ステージ遷移ステートへ
        if(Input::IsTriggerPadButton(PAD_BUTTON::A | PAD_BUTTON::B)){
            if(!StageManager::IsLastStage()){
                ToClearStageState(StageManager::GetNextStartPos());
                //2つのステージのコライダーを渡すよう設定
                StageManager::SetIsHandOverColliderNext(true);
            }
        }
    }
}
