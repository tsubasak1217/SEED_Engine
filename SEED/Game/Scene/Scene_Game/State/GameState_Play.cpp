#include <GameState_Play.h>

#include "StageManager.h"
#include <Scene_Game.h>

// 遷移可能なステートのインクルード
#include "Pause/GameState_PauseForPlay.h"
#include "GameState_Goal.h"

// lib
#include "../PlayerInput/PlayerInput.h"
#include "LocalFunc.h"
#include "Easing.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ：デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////

GameState_Play::GameState_Play(Scene_Base* pScene,bool isPlayerSetStartPos): State_Base(pScene){
    pGameScene_ = dynamic_cast<Scene_Game*>(pScene);
    Initialize(isPlayerSetStartPos);
}

GameState_Play::~GameState_Play(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Initialize(bool isPlayerSetStartPos){
    // FieldEditor
    fieldEditor_ = std::make_unique<FieldEditor>(pGameScene_->Get_StageManager());
    fieldEditor_->Initialize();

    // FieldColliderEditor
    fieldColliderEditor_ = std::make_unique<ColliderEditor>("field",nullptr);

    // プレイヤーの初期位置
    Player* pPlayer = pGameScene_->Get_pPlayer();
    if(isPlayerSetStartPos){
        pPlayer->SetPosition(StageManager::GetStartPos());
        pPlayer->SetTranslateY(pPlayer->GetWorldTranslate().y + 1.f);
        pPlayer->SetIsDrop(false);
        pPlayer->SetIsApplyGravity(true);
        pPlayer->SetGrowLevel(1);
    }
    pPlayer->SetIsMovable(true);

    // プレイヤーにステージの敵情報を渡す
    {
        EnemyManager* enemyManager = pGameScene_->Get_StageManager().GetCurrentStage()->GetEnemyManager();
        pPlayer->SetEnemyManager(enemyManager);
    }

    // プレイヤーにそのステージの死体Managerを渡す
    {
        if(!pPlayer->GetCorpseManager()->GetIsEmpty()){
            pPlayer->GetCorpseManager()->RemoveAll();
        }
        pPlayer->SetCorpseManager(pGameScene_->Get_pStageManager()->GetCurrentStage()->GetPlayerCorpseManager());
    }

    // カメラのターゲット
    pGameScene_->Get_pCamera()->SetTarget(pPlayer);
   
    // ターゲットのワールド回転行列を取得
    Matrix4x4 targetWorldMatrix = pPlayer->GetWorldMat();

    // Y 軸の回転成分を抽出 (Z軸方向のベクトル)
    Vector3 forward = MyMath::Normalize(Vector3(targetWorldMatrix.m[2][0], 0.0f, targetWorldMatrix.m[2][2]));

    // Y 軸の回転角度を求める
    float targetYaw = std::atan2(forward.x, forward.z);

    // カメラの回転をターゲットの反対方向に設定
    pGameScene_->Get_pCamera()->SetTheta(targetYaw + 3.14f);

    float phi = MyMath::Deg2Rad(80.0f);
    pGameScene_->Get_pCamera()->SetPhi(phi);


    // イベントシーンがあれば終了
    pGameScene_->EndEvent();

    //sprite
    pauseButton = std::make_unique<Sprite>("GameUI/start.png");
    pauseButton->color.w = 0.566f;

    // 黒い画面を生成
    fade_ = std::make_unique<Sprite>("Assets/white1x1.png");
    fade_->size = Vector2(1280.f,760.f);
    fade_->color = Vector4(0.f,0.f,0.f,1.0f);
    fade_->translate = Vector2(0.0f,fade_->size.y);
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Finalize(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// 更新処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Update(){
#ifdef _DEBUG
    // ステージのエディター
    fieldEditor_->ShowImGui();

    if(fieldEditor_->GetIsEditing()){
        SEED::SetCamera("debug");
    } else{
        SEED::SetCamera("follow");
    }

    // フィールドのコライダーエディター
    fieldColliderEditor_->Edit();

#endif // _DEBUG

    FadeUpdate();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Draw(){
    pauseButton->Draw();

    if(isFadeIn_ || isFadeOut_){
        fade_->Draw();
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::BeginFrame(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::EndFrame(){
    ManageState();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// コライダーをCollisionManagerに渡す
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::HandOverColliders(){
    fieldColliderEditor_->HandOverColliders();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ステート管理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::ManageState(){

    // ポーズへ遷移
    if(PlayerInput::Pause::Pause()){
        pGameScene_->ChangeState(new GameState_PauseForPlay(pScene_));
        return;
    }

    // ゴールへ遷移
    if(StageManager::GetCurrentStage()->IsGoal()){
        if(!StageManager::IsLastStage()){
            //2つのステージのコライダーを渡すよう設定
            StageManager::SetIsHandOverColliderNext(true);

            pScene_->ChangeState(new GameState_Goal(pScene_));
            return;
        }
    }

    // ゲームオーバー
    if(!isFadeIn_ && !isFadeOut_){
        if(pGameScene_->Get_pPlayer()->GetIsGameOver()){
            pGameScene_->Get_pPlayer()->SetIsMovable(false);
            isFadeIn_ = true;
        }
    }
}

void GameState_Play::FadeUpdate(){
    if(isFadeIn_){
        currentTime_ += ClockManager::DeltaTime();
        fade_->translate.y = MyMath::Lerp(0.f,720.f,1.f - EaseInSine(currentTime_));
        fade_->translate.y = (std::max)(fade_->translate.y,0.f);
        if(currentTime_ >= fadeInTime_){
            isFadeIn_ = false;
            isFadeOut_ = true;
            currentTime_ = 0.0f;

            fade_->translate.y = 0.f;

            // ゲームオーバー処理
            pGameScene_->Get_pPlayer()->GameOver();
        }
    } else if(isFadeOut_){
        currentTime_ += ClockManager::DeltaTime();
        // フェードアウト処理
        fade_->color.w =MyMath::Lerp(1.f,0.f,EaseInSine(currentTime_));
        if(currentTime_ >= fadeOutTime_){
            currentTime_ = 0;

            fade_->size = Vector2(1280.f,760.f);
            fade_->color = Vector4(0.f,0.f,0.f,1.0f);
            fade_->translate = Vector2(0.0f,fade_->size.y);

            isFadeOut_ = false;
            pGameScene_->Get_pPlayer()->SetIsMovable(true);
        }
    }
}
