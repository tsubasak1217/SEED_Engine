// state
#include "GameState_Title.h"
#include "GameState_Select.h"

// lib 
#include "../adapter/json/JsonCoordinator.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ：デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////
GameState_Title::GameState_Title(Scene_Base* pScene): State_Base(pScene){
    pGameScene_ = dynamic_cast<Scene_Game*>(pScene);
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
    pGameScene_->Get_pStageManager()->GetTitleStage()->InitializeStatus(std::string("resources/jsons/Stages/stage_title.json"));

    // 操作フラグをfalseにしておく
    Vector3 initializePlayerPos = StageManager::GetTitleStartPos();
    pGameScene_->Get_pPlayer()->SetPosition(initializePlayerPos);
    pGameScene_->Get_pPlayer()->SetIsMovable(false);
    pGameScene_->Get_pPlayer()->SetIsApplyGravity(false);

    // カメラの初期位置
    SEED::SetCamera("main");
    Vector3 cameraPos = pGameScene_->Get_pPlayer()->GetWorldTranslate() + cameraOffset_;
    SEED::GetCamera()->SetTranslation(cameraPos);
    SEED::GetCamera()->SetRotation(cameraRotate_);

    const std::string groupName = "title";
    JsonCoordinator::LoadGroup(groupName);
    JsonCoordinator::RegisterItem(groupName, "cameraPos", cameraOffset_);
    JsonCoordinator::RegisterItem(groupName, "cameraRotate", cameraRotate_);
    JsonCoordinator::RegisterItem(groupName, "playerRotate", playerRotate_);

    // イベントシーンがあれば終了
    pGameScene_->EndEvent();

    titleLogo_ = std::make_unique<Sprite>("Title/TitleLogo.png");
    titleLogo_->anchorPoint = {0.5f,0.5f};
    titleLogo_->translate = {1090.f,190.f};
  
    // FadeIn用のフェードスプライトの初期化
    fade_ = std::make_unique<Sprite>("SelectScene/fade.png");
    fade_->anchorPoint = Vector2(0.0f, 0.0f);
    fade_->translate = Vector2(0.0f, 0.0f);
    // 初期状態は完全に不透明（フェードインで徐々に透明にする）
    fade_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    fadeInStarted_ = true;
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
    if(ImGui::Button("save")){
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

    // フェードイン処理：フェードが始まっている場合、アルファ値を減少させる
    if (fadeInStarted_){
        fade_->color.w -= fadeSpeed_;
        if (fade_->color.w <= 0.0f){
            fade_->color.w = 0.0f;
            fadeInStarted_ = false; // フェードイン完了
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Title::Draw(){
    // タイトル画面の描画処理
    StageManager::GetTitleStage()->Draw();

    titleLogo_->Draw();

    // フェードイン効果の描画（フェードが完了していればアルファが0なので描画しても影響なし）
    fade_->Draw();
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
    // 遷移：Aボタンを押したらセレクト画面へ
    if (Input::IsTriggerPadButton(PAD_BUTTON::A)){
        pGameScene_->ChangeState(new GameState_Select(pGameScene_));
    }
}
