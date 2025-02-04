// state
#include "GameState_Title.h"
#include "GameState_Select.h"

//lib 
#include "../adapter/json/JsonCoordinator.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ：デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////
GameState_Title::GameState_Title(Scene_Base* pScene) : State_Base(pScene){
    pGameScene_ = dynamic_cast< Scene_Game* >(pScene);
    Initialize();


}

GameState_Title::~GameState_Title(){
    pGameScene_->Get_pPlayer()->SetIsApplyGravity(true);
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::Initialize(){
    pGameScene_->Get_pStageManager()->GetTitleStage()->InitializeStatus("resources/jsons/Stages/stage_title.json");
    // 操作フラグをfalseにしておく
    Vector3 initializePlayerPos = StageManager::GetTitleStartPos();
    pGameScene_->Get_pPlayer()->SetPosition(StageManager::GetTitleStartPos());
    pGameScene_->Get_pPlayer()->SetIsMovable(false);
    pGameScene_->Get_pPlayer()->SetIsApplyGravity(false);

    // カメラの初期位置
    SEED::SetCamera("main");
    Vector3 cameraPos = pGameScene_->Get_pPlayer()->GetWorldTranslate() + cameraOffset_;
    SEED::GetCamera()->SetTranslation(cameraPos);
    SEED::GetCamera()->SetRotation(cameraRotate_);

    const std::string groupName = "title";
    JsonCoordinator::LoadGroup(groupName);
    JsonCoordinator::RegisterItem(groupName, "cameraPos",cameraOffset_);
    JsonCoordinator::RegisterItem(groupName, "cameraRotate",cameraRotate_);
    JsonCoordinator::RegisterItem(groupName, "playerRotate", playerRotate_);

    // イベントシーンがあれば終了
    pGameScene_->EndEvent();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::Finalize(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// 更新処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::Update(){

#ifdef _DEBUG
    ImGui::Begin("title");
    if (ImGui::Button("save")){
        JsonCoordinator::SaveGroup("title");
    }

    JsonCoordinator::RenderGroupUI("title");
    ImGui::End();
#endif // _DEBUG


    StageManager::GetTitleStage()->Update();
    Vector3 cameraPos = pGameScene_->Get_pPlayer()->GetWorldTranslate() + cameraOffset_;
    SEED::GetCamera()->SetTranslation(cameraPos);
    SEED::GetCamera()->SetRotation(cameraRotate_);
    pGameScene_->Get_pPlayer()->SetRotate(playerRotate_);
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::Draw(){
    // タイトル画面の描画処理
    StageManager::GetTitleStage()->Draw();

}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::BeginFrame(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::EndFrame(){
    ManageState();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// コライダーをCollisionManagerに渡す
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::HandOverColliders(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// ステート管理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::ManageState(){
    // タイトル画面のステート管理
    // 遷移
    if (Input::IsTriggerPadButton(PAD_BUTTON::A)){
        pGameScene_->ChangeState(new GameState_Select(pGameScene_));
    }
}