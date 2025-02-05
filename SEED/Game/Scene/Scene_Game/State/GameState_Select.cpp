#include "GameState_Select.h"
#include "GameState_Play.h"
#include "Scene_Game.h"
#include "State/GameState_Title.h"

// 遷移可能なステートのインクルード
#include "Pause/GameState_PauseForSelect.h"

// lib
#include "../PlayerInput/PlayerInput.h"


////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ：デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////
GameState_Select::GameState_Select(Scene_Base* pScene): State_Base(pScene){
    pGameScene_ = dynamic_cast<Scene_Game*>(pScene);
    Initialize();
}

GameState_Select::~GameState_Select(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Select::Initialize(){

    // ステージをリセット
    StageManager::SetCurrentStageNo(0);

    // イベントシーンがあれば終了
    pGameScene_->EndEvent();

    // ステージセレクターを生成
    stageSelector_ = std::make_unique<StageSelector>(pGameScene_->Get_pStageManager(),pGameScene_->Get_pCamera());

    // 操作フラグをfalseにしておく
    pGameScene_->Get_pPlayer()->SetIsMovable(false);
    pGameScene_->Get_pPlayer()->SetTranslate(StageManager::GetStartPos());

}

////////////////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Select::Finalize(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// 更新処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Select::Update(){
    stageSelector_->Update();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Select::Draw(){
    stageSelector_->Draw();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Select::BeginFrame(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Select::EndFrame(){

    // ステージが変わったらプレイヤーの位置を初期位置に戻す
    if(StageManager::IsStageChanged()){
        pGameScene_->Get_pPlayer()->SetVelocityY(0.0f);
        pGameScene_->Get_pPlayer()->DiscardPreCollider();
        pGameScene_->Get_pPlayer()->SetTranslate(StageManager::GetStartPos());
        pGameScene_->Get_pPlayer()->UpdateMatrix();
    }

    ManageState();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// コライダーをCollisionManagerに渡す
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Select::HandOverColliders(){}


////////////////////////////////////////////////////////////////////////////////////////
//
// ステート管理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Select::ManageState(){

    // title遷移
    if (Input::IsTriggerPadButton(PAD_BUTTON::START)){
        pGameScene_->ChangeState(new GameState_Title(pScene_));
        return;
    }

    // ステージが決定されたらプレイステートに遷移
    if(stageSelector_->GetIsDecided()){
        // ステージをリセット
        int stageNo = pGameScene_->Get_pStageManager()->GetCurrentStage()->GetStageNo() + 1;
        std::string filePath = "resources/jsons/Stages/stage_" + std::to_string(stageNo) + ".json";
        // pGameScene_->Get_pStageManager()->GetCurrentStage()->LoadFromJson(filePath); <- InitializeStatus を追加したのでコメントアウト
        pGameScene_->Get_pStageManager()->GetCurrentStage()->InitializeStatus(filePath);
        pGameScene_->Get_pPlayer()->SetCorpseManager(pGameScene_->Get_pStageManager()->GetCurrentStage()->GetPlayerCorpseManager());

        // 遷移
        pGameScene_->ChangeState(new GameState_Play(pGameScene_));
    }
}
