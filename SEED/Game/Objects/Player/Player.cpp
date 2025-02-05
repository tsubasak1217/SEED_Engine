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
#include "Easing.h"

// 状態クラスのインクルード
#include "PlayerState/PlayerState_Idle.h"
#include "PlayerState/PlayerState_Jump.h"
#include "PlayerState/PlayerState_Move.h"
#include "PlayerState/PlayerState_Spawn.h"
#include "PlayerState/PlayerStage_ForNextStage.h"
#include "PlayerState/PlayerState_Eat.h"

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

    UpdateScaleByGrowLevel();
    BaseCharacter::Update();

    shadow_->Update(StageManager::GetCurrentStage());

    if(enemyManager_){
        predationRange_->Update(enemyManager_);
    }

    // 無敵時間中の色変更
    if(unrivalledTime_ > 0.0f){
        float sin = sinf(ClockManager::TotalTime() * (3.14f * 2.5f));
        model_->color_ = {1.0f,0.5f,0.5f,1.0f};
        model_->color_.w = 0.5f + (0.5f * sin);
    } else{
        model_->color_ = {1.0f,1.0f,1.0f,1.0f};
    }
}
//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Player::Draw(){
    shadow_->Draw();
    BaseCharacter::Draw();

    predationRange_->Draw();
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

    // スイッチ用の重量の更新
    switchPushWeight_ = 1.0f + (1.00f * (growLevel_ - 1));


    BaseCharacter::BeginFrame();

#ifdef _DEBUG
    ImGui::Begin("Player");
    ImGui::Text("dropSpeed_ : %f",dropSpeed_);
    ImGui::End();
#endif
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

void Player::Spawn(Egg* _egg){
    PlayerState_Spawn* state = new PlayerState_Spawn("Player_Spawn",this,_egg);
    growLevel_ = 1;
    ChangeState(state);
}

void Player::GameOver(){
    { // プレイヤーを初期地点に戻す(セーブデータがあればInitializeStatusで上書きされる)
        DiscardPreCollider();
        SetTranslate(StageManager::GetCurrentStage()->GetStartPosition());
        UpdateMatrix();
    }
    {
        growLevel_ = 1;
    }
    { // ステージの初期化
        StageManager::GetCurrentStage()->InitializeStatus(true);
    }

    { // ステートの初期化
        currentState_ = std::make_unique<PlayerState_Idle>("Player_Idle",this);
    }
}

//////////////////////////////////////////////////////////////////////////
// プレイヤーの成長レベルによってスケールを変更
//////////////////////////////////////////////////////////////////////////
void Player::UpdateScaleByGrowLevel(){
    static float aimScale = 1.0f;
    static float preScale = 1.0f;
    static int preGrowLevel = 1;
    static float growUpTime = 1.0f;
    static float kGrowUpTime = 1.0f;

    // 成長レベルが変わったら
    if(preGrowLevel != growLevel_){
        // 時間を初期化して目標スケールを変更する
        growUpTime = 0.0f;
        aimScale = 1.0f + (0.75f * (growLevel_ - 1));
        preScale = GetWorldScale().x;
        preGrowLevel = growLevel_;
    }

    // 時間がまだ残っている場合
    if(growUpTime < 1.0f){

        // 媒介変数を計算
        float t = EaseOutBounce(growUpTime / kGrowUpTime);

        // スケールを変更
        float scale = preScale + (aimScale - preScale) * t;
        SetScale(Vector3(scale,scale,scale));

        // 時間を加算
        growUpTime = std::clamp(growUpTime + ClockManager::DeltaTime(),0.0f,kGrowUpTime);
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
// 基本的な情報をJson形式で出す
//////////////////////////////////////////////////////////////////////////
const nlohmann::json& Player::GetJsonData(){
    static nlohmann::json json;
    json["GrowLevel"] = growLevel_;
    json["Weight"] = weight_;

    // BaseObjectの情報と結合して出力
    json.update(BaseCharacter::GetJsonData());
    return json;
}

//////////////////////////////////////////////////////////////////////////
// 衝突時処理
//////////////////////////////////////////////////////////////////////////
void Player::OnCollision(BaseObject* other,ObjectType objectType){
    other;
    objectType;
    static float kUnrriValledTime = 3.0f;

    BaseObject::OnCollision(other,objectType);

    // 移動床に触れている状態
    if((int32_t)objectType & (int32_t)ObjectType::Move){
        if(!IsJumpable()){
            // 親子付けを行い移動床基準のトランスフォームに変換
            SetParent(other);

            Vector3 preTranslate = GetWorldTranslate();
            Matrix4x4 invParentMat = InverseMatrix(GetParent()->GetWorldMat());
            Vector3 localTranslate = preTranslate * invParentMat;
            localTranslate *= ExtractScale(GetParent()->GetWorldMat());
            SetTranslate(localTranslate);
            UpdateMatrix();
            Vector3 newTranslate = GetWorldTranslate();
        }
    }

    // 敵に触れている状態
    if(objectType == ObjectType::Enemy){
        if(unrivalledTime_ <= 0.0f){
            PlayerState_Eat* state = dynamic_cast<PlayerState_Eat*>(currentState_.get());

            // 食べていない場合
            if(!state){
                // 成長レベルが最低の場合、死亡
                if(growLevel_ <= 1){
                    isGameOver_ = true;
                } else{
                    // 小さくなる
                    StepGrowLevel(-1);
                }
                // 無敵時間を設定
                unrivalledTime_ = kUnrriValledTime;
            }
        }
    }
}
