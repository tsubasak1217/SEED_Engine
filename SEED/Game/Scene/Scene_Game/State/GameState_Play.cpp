#include <GameState_Play.h>

#include "StageManager.h"
#include <Scene_Game.h>

// 遷移可能なステートのインクルード
#include "Pause/GameState_PauseForPlay.h"
#include "GameState_Goal.h"

// lib
#include "../PlayerInput/PlayerInput.h"

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

    // イベントシーンがあれば終了
    pGameScene_->EndEvent();

    //sprite
    pauseButton = std::make_unique<Sprite>("GameUI/start.png");
    pauseButton->color.w = 0.566f;
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
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Draw(){
    pauseButton->Draw();
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
}
