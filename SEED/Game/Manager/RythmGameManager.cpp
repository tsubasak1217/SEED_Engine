#include "RythmGameManager.h"
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/SEED.h>
#include <Game/GameSystem.h>
#include <Game/Config/PlaySettings.h>
#include <Game/Scene/Scene_Game/State/GameState_Select.h>
#include <Game/Scene/Scene_Game/State/GameState_Pause.h>
#include <Game/Objects/Result/ResultDrawer.h>

/////////////////////////////////////////////////////////////////////////////////
// static変数の初期化
/////////////////////////////////////////////////////////////////////////////////
RythmGameManager* RythmGameManager::instance_ = nullptr;

////////////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////////////////
RythmGameManager::RythmGameManager(){
    // コンボオブジェクト
    comboObject_ = std::make_unique<ComboObject>();
    comboObject_->comboText.transform.translate = { 187.0f,147.0f };
    comboObject_->comboText.size = { 300.0f,200.0f };
    comboObject_->comboText.fontSize = 100.0f;

    // エディタ
    notesEditor_ = std::make_unique<NotesEditor>();
}

RythmGameManager::~RythmGameManager(){
    // カメラの登録解除
    SEED::RemoveCamera("gameCamera");
}

////////////////////////////////////////////////////////////////////////////////
// インスタンスの取得
////////////////////////////////////////////////////////////////////////////////
RythmGameManager* RythmGameManager::GetInstance(){
    if(!instance_){
        instance_ = new RythmGameManager();
    }
    return instance_;
}

//////////////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::Initialize(const nlohmann::json& songData){
    // カメラの初期化
    gameCamera_ = std::make_unique<BaseCamera>();
    gameCamera_->SetTranslation(Vector3(0.0f, 0.0f, 0.0f));
    gameCamera_->UpdateMatrix();

    // カメラの登録,設定
    SEED::RegisterCamera("gameCamera", gameCamera_.get());
    SEED::SetMainCamera("gameCamera");

    // settingsの初期化
    PlaySettings::GetInstance();

    // Inputの初期化
    PlayerInput::GetInstance()->Initialize();

    // リザルトの初期化
    playResult_ = PlayResult();
    playResult_.songData = songData;

    // 譜面データの初期化
    notesData_ = std::make_unique<NotesData>();
    notesData_->Initialize(songData);
    playResult_.totalCombo = notesData_->GetTotalCombo();
    

    // エディタの初期化
    if(songData.contains("jsonFilePath")){
        notesEditor_->Initialize(songData["jsonFilePath"]);
    }

    // プレイフィールドの初期化
    PlayField::GetInstance()->SetNoteData(notesData_.get());

    // 判定の初期化
    Judgement::GetInstance();
}

//////////////////////////////////////////////////////////////////////////////////
// フレーム開始処理
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::BeginFrame(){
    if(isPaused_){
        Input::SetMouseCursorVisible(true);
        return;
    }

    // inputのフレーム開始処理
    //Input::SetMouseCursorVisible(false);
#ifdef _DEBUG
    //Input::RepeatCursor(ImFunc::GetSceneWindowRange("GameWindow"));
#else
    Input::RepeatCursor();
#endif

    PlayerInput::GetInstance()->BeginFrame();
    // 譜面データのフレーム開始処理
    if(notesData_){
        notesData_->BeginFrame();
    }
}

//////////////////////////////////////////////////////////////////////////////////
// フレーム終了処理
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::EndFrame(){

    // 譜面が終わったらクリアシーンへ移行
    // escapeでポーズ画面を出す
    if(!isPaused_){
        if(Input::IsTriggerKey(DIK_ESCAPE)){
            auto* scene = GameSystem::GetScene();
            scene->CauseEvent(new GameState_Pause(scene));
            Pause();
        }
    }

    if(notesData_->GetIsEnd()){

        // スコア,ランクの計算
        playResult_.score = CalculateScore();
        playResult_.rank = ScoreRankUtils::GetScoreRank(playResult_.score);

        // プレイ結果の保存
        ResultDrawer::SetResult(playResult_);

        // Inputのカーソルを表示状態に戻す
        Input::SetMouseCursorVisible(true);

        // リザルト画面へ移行
        GameSystem::ChangeScene("Clear");
        return;
    }

}

//////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::Update(){
    if(!isPaused_){
        // Inputの更新
        PlayerInput::GetInstance()->Update();

        // 譜面データの更新
        notesData_->Update();

        // ノーツの判定
        Judgement::GetInstance()->Judge(notesData_.get());

        // プレイフィールドの更新
        PlayField::GetInstance()->Update();
    }

#ifdef _DEBUG
    if(ImFunc::CustomBegin("ComboText", MoveOnly_TitleBar)){
        comboObject_->comboText.Edit();
    }
    ImGui::End();

    // ノーツの編集ウインドウ
    notesEditor_->Edit();
    notesData_->Edit();
#endif // _DEBUG

}

//////////////////////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::Draw(){
    // Inputのカーソル描画
    PlayerInput::GetInstance()->Draw();

    // プレイフィールドの描画
    PlayField::GetInstance()->Draw();

    // 譜面データの描画
    notesData_->Draw();

    // コンボの描画
    comboObject_->Draw();

    // ゲームカメラ画面の描画
#ifdef _DEBUG
    ImFunc::SceneWindowBegin("GameScene", "gameCamera", MoveOnly_TitleBar);
    ImGui::End();

    PlaySettings::GetInstance()->Edit();

#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////////////
// ポーズ,再開
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::Pause(){
    isPaused_ = true;
    notesData_->Pause();
}

void RythmGameManager::Resume(){
    isPaused_ = false;
    notesData_->Resume();
}

//////////////////////////////////////////////////////////////////////////////////
// スコアの計算
//////////////////////////////////////////////////////////////////////////////////
float RythmGameManager::CalculateScore(){
    float result;
    float scorePerNote = 100.0f / playResult_.totalCombo;
    float scoreSubtractRateGreat = 0.5f;
    float scoreSubtractRateGood = 0.75f;
    float subtractGreat = playResult_.evalutionCount[(int)Judgement::Evaluation::GREAT] * scorePerNote * scoreSubtractRateGreat;
    float subtractGood = playResult_.evalutionCount[(int)Judgement::Evaluation::GOOD] * scorePerNote * scoreSubtractRateGood;
    float subtractMiss = playResult_.evalutionCount[(int)Judgement::Evaluation::MISS] * scorePerNote;
    result = 100.0f - (subtractGreat + subtractGood + subtractMiss);
    return result;
}
