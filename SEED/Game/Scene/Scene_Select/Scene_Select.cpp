#include "Scene_Select.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/PostEffectSystem/PostEffectSystem.h>

//============================================================================
//	Scene_Select classMethods
//============================================================================

Scene_Select::Scene_Select(){
}

Scene_Select::~Scene_Select(){

    // すべてのエフェクトを削除
    EffectSystem::DeleteAll();

    // BGMを停止させる
    AudioManager::EndAudio(bgmHandle_);
}

void Scene_Select::Initialize(){

    //============================================================================
    //	engine
    //============================================================================

    SEED::SetMainCamera("default");
    auto* camera = SEED::GetMainCamera();
    camera->SetTranslation({ 0.0f,0.0f,-10.0f });
    camera->SetScale({ 1.0f,1.0f,1.0f });
    SEED::SetSkyBox("DefaultAssets/CubeMaps/rostock_laage_airport_4k.dds");

    // エフェクトの追加
    EffectSystem::AddEffectEndless("gj3_selectScene.json", { 0.0f,0.0f,0.0f }, camera->GetWorldMatPtr());

    // BGMを再生
    bgmHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("セレクトシーン_BGM"), true, kBGMVolume_);

    //============================================================================
    //	objects
    //============================================================================

    // 背景初期化
    background_ = std::make_unique<SelectBackground>();
    background_->Initialize();
    // ステージ選択初期化
    selectStage_ = std::make_unique<SelectStage>();
    selectStage_->Initialize(static_cast<uint32_t>(currentStageIndex_));

    // 色の初期化
    quadColors_.push_back(MyMath::FloatColor(0x006066ff));
    //quadColors_.push_back(MyMath::FloatColor(10, 10, 10, 255, false));
    //quadColors_.push_back(MyMath::FloatColor(255, 13, 86, 255, false));
}

void Scene_Select::Update(){

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

    if(isTransitionToGame_){
        // ゲームシーンに進むタイマーを進める
        toGameSceneTimer_.Update();
        // タイマーが終わったらゲームシーンに進む
        if(toGameSceneTimer_.IsFinished()){
            currentStageIndex_ = selectStage_->GetStageInfo().decideStage;
            ChangeScene("Game");
        }
        return;
    }

    // 次のシーンに進めるかチェック
    if(selectStage_->GetStageInfo().isNextScene){
        // ゲームシーンに進ませる
        isTransitionToGame_ = true;

        // トランジション開始
        HexagonTransition* transition = SceneTransitionDrawer::AddTransition<HexagonTransition>();
        transition->SetHexagonInfo(46.0f, quadColors_);
        transition->StartTransition(toGameSceneTimer_.GetDuration(), gameSceneStartTime_);

        return;
    }
    // タイトルに戻るかチェック
    if(selectStage_->IsReturnScene()){
        if(!selectStage_->IsDecideStage()){
            if(!isBackToTitle_){
                isBackToTitle_ = true;
                BlockSlideTransition* transition = SceneTransitionDrawer::AddTransition<BlockSlideTransition>();
                transition->SetQuadInfo(96.0f, quadColors_);
                transition->StartTransition(backToTitleTimer_.GetDuration(), titleSceneStartTime_);
            }
        }
    }

    if(isBackToTitle_){
        // タイトルに戻るタイマーを進める
        backToTitleTimer_.Update();

        // タイマーが終わったらタイトルシーンに戻す
        if(backToTitleTimer_.IsFinished()){
            ChangeScene("Title");
        }
    }
}

void Scene_Select::Draw(){

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

void Scene_Select::BeginFrame(){

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->BeginFrame();
}
//============================================================================
//	Scene_Select frame:End
//============================================================================

void Scene_Select::EndFrame(){

    // ヒエラルキー内のオブジェクトのフレーム終了処理
    hierarchy_->EndFrame();
}