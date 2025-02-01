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
    model_->isParentScale_ = false;

    // コライダーの初期化
    InitColliders(ObjectType::Player);
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(ObjectType::Player);
        collider->AddSkipPushBackType(ObjectType::Egg);
    }

    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f,7.0f,0.0f);

    // 状態の初期化
    currentState_ = std::make_unique<PlayerState_Idle>("Player_Idle",this);

    // Json ファイルからの読み込み
    JsonCoordinator::LoadGroup("Player");

    JsonCoordinator::RegisterItem("Player","Weight",weight_);

    // 捕食可能範囲の初期化
    predationRange_ = std::make_unique<PredationRange>();
    predationRange_->Initialize(this);

    lastPosOnGround_ = GetWorldTranslate();

    // shadow
    shadow_ = std::make_unique<Shadow>(this);
    shadow_->Initialize();
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

    shadow_->Update(StageManager::GetCurrentStage());
}
//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Player::Draw(){
    shadow_->Draw();
    BaseCharacter::Draw();
}

//////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////////////
void Player::BeginFrame(){

    // 落下中かつジャンプ可能でないとき or ジャンプ時
    if((isDrop_ && !IsJumpable()) or isJump_){

        // 親がいれば
        if(GetParent()){
            // 親子付けを解除
            parent_ = nullptr;
            model_->parent_ = nullptr;
            // ワールド基準のトランスフォームに変換
            SetTranslate(GetWorldTranslate());
            SetRotate(GetWorldRotate());
            SetScale(GetWorldScale());
            UpdateMatrix();
        }
    }

    BaseCharacter::BeginFrame();

    ImGui::Begin("Player");
    ImGui::Text("dropSpeed_ : %f",dropSpeed_);
    ImGui::End();
}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
//////////////////////////////////////////////////////////////////////////
void Player::EndFrame(){
    BaseCharacter::EndFrame();
    if(GetWorldTranslate().y <= 0.0f){
        isGameOver_ = true;
    }
    if(!isDrop_){
        lastPosOnGround_ = GetLocalTranslate();
    }

    if(isMovable_){
        if(isGameOver_){
            GameOver();
            isGameOver_ = false;
        }
    } else{
        //isMovable_ が false なら isGameOver_ は false にする
        isGameOver_ = false;
    }
}

void Player::Spawn(const Vector3& pos){
    PlayerState_Spawn* state = new PlayerState_Spawn("Player_Spawn",this);
    state->SetSpawnPos(pos);
    ChangeState(state);
}

void Player::GameOver(){
    { // ステージの初期化
        StageManager::GetCurrentStage()->InitializeStatus();
    }
    { // プレイヤーを初期地点に戻す
        DiscardPreCollider();
        SetTranslate(StageManager::GetCurrentStage()->GetStartPosition());
        UpdateMatrix();
    }
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

    BaseObject::OnCollision(other,objectType);

    /*// ゴールに触れている状態で
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
    }*/

    // 移動床に触れている状態
    if((int32_t)objectType & (int32_t)ObjectType::Move){
        if(!IsJumpable()){
            // 親子付けを行い移動床基準のトランスフォームに変換
            SetParent(other);

            Vector3 preTranslate = GetWorldTranslate();
            Matrix4x4 invParentMat = InverseMatrix(GetParent()->GetWorldMat());
            Vector3 localTranslate = preTranslate * invParentMat;
            SetTranslate(localTranslate);
            UpdateMatrix();
            Vector3 newTranslate = GetWorldTranslate();
            isStop_ = true;
        }
    }

    // 敵に触れている状態
    if(objectType == ObjectType::Enemy){
        isGameOver_ = true;
    }
}
