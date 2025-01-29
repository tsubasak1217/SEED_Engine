#include "Player.h"

/// engine
// module
#include "ImGuiManager.h"
#include "InputManager.h"

//object
#include "Egg/Egg.h"

// manager
#include "Egg/Manager/EggManager.h"
#include "Player/PredationRange/PredationRange.h"
#include "StageManager.h"
//lib
#include "../adapter/json/JsonCoordinator.h"
#include "../PlayerInput/PlayerInput.h"

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

    // Json ファイルからの読み込み
    JsonCoordinator::LoadGroup("Player");

    JsonCoordinator::RegisterItem("Player","Weight",weight_);

    // 捕食可能範囲の初期化
    predationRange_ = std::make_unique<PredationRange>();
    predationRange_->Initialize(this);

    lastPosOnGround_ = GetWorldTranslate();
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
    BaseCharacter::Update();
}
//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Player::Draw(){
    BaseCharacter::Draw();
}

void Player::EndFrame(){
    BaseCharacter::EndFrame();
    if(GetWorldTranslate().y <= 0.0f){
        SetTranslate(StageManager::GetStartPos());
        for(auto& collider : this->GetColliders()){
            collider->DiscardPreCollider();
        }
    }
    if(!isDrop_){
        lastPosOnGround_ = GetLocalTranslate();
    }
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
    if(!enemyManager_){
        return false;
    }
    predationRange_->Update(enemyManager_);

    if(predationRange_->GetPreyList().empty()){
        return false;
    }
    return true;
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
        if(PlayerInput::CharacterMove::GoNextStage()){
            // 現在のステートが PlayerStage_ForNextStage でない場合のみ 許可
            PlayerStage_ForNextStage* state = dynamic_cast<PlayerStage_ForNextStage*>(currentState_.get());
            if(!state){
                if(!StageManager::IsLastStage()){
                    ToClearStageState(StageManager::GetNextStartPos());
                    //2つのステージのコライダーを渡すよう設定
                    StageManager::SetIsHandOverColliderNext(true);
                }
            }
        }
    }

    // 敵に触れている状態
    if(objectType == ObjectType::Enemy){
        DiscardPreCollider();
        SetTranslate(StageManager::GetCurrentStage()->GetStartPosition());
        UpdateMatrix();
    }
}
