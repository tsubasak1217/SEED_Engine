#include "Scene_Select.h"

//============================================================================
//	Scene_Select classMethods
//============================================================================

Scene_Select::Scene_Select() {
}

Scene_Select::~Scene_Select() {
    // すべてのエフェクトを削除
    EffectSystem::DeleteAll();
}

void Scene_Select::Initialize() {

    //============================================================================
    //	engine
    //============================================================================

    SEED::SetMainCamera("default");
    SEED::SetSkyBox("DefaultAssets/CubeMaps/rostock_laage_airport_4k.dds");

    // エフェクトの追加
    EffectSystem::AddEffectEndless("gj3_selectScene.json", { 0.0f,0.0f,0.0f },nullptr);

    //============================================================================
    //	objects
    //============================================================================

    // 背景初期化
    background_ = std::make_unique<SelectBackground>();
    background_->Initialize();

    // ステージ選択初期化
    selectStage_ = std::make_unique<SelectStage>();
    selectStage_->Initialize(static_cast<uint32_t>(currentStageIndex_));
}

void Scene_Select::Update() {

    //============================================================================
    //	objects
    //============================================================================

    // 背景更新
    background_->Update();

    // ステージ選択更新
    selectStage_->Update();

    //============================================================================
    //	sceneEvenets
    //============================================================================

    if (Input::IsTriggerPadButton(PAD_BUTTON::A)) {

        ChangeScene("Game");
    }

    if (Input::IsTriggerPadButton(PAD_BUTTON::B)) {

        ChangeScene("Title");
    }
}

void Scene_Select::Draw() {

    //============================================================================
    //	objects
    //============================================================================

    // 背景描画
    background_->Draw();

    // ステージ選択描画
    selectStage_->Draw();
}

//============================================================================
//	Scene_Select frame:Begin
//============================================================================

void Scene_Select::BeginFrame() {

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->BeginFrame();
}
//============================================================================
//	Scene_Select frame:End
//============================================================================

void Scene_Select::EndFrame() {

    // ヒエラルキー内のオブジェクトのフレーム終了処理
    hierarchy_->EndFrame();
}