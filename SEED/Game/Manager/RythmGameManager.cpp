#include "RythmGameManager.h"
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/SEED.h>
#include <Game/GameSystem.h>
#include <Game/Config/PlaySettings.h>
#include <Game/Scene/Scene_Game/State/GameState_Select.h>
#include <Game/Scene/Scene_Game/State/GameState_Pause.h>
#include <Game/Scene/Scene_Clear/Scene_Clear.h>

/////////////////////////////////////////////////////////////////////////////////
// static変数の初期化
/////////////////////////////////////////////////////////////////////////////////
RythmGameManager* RythmGameManager::instance_ = nullptr;

////////////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////////////////
RythmGameManager::RythmGameManager(){
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
void RythmGameManager::Initialize(const SongInfo& songInfo, int32_t difficulty){
    // カメラの初期化
    gameCamera_ = std::make_unique<BaseCamera>();
    gameCamera_->SetTranslation(Vector3(0.0f, 0.0f, 0.0f));
    gameCamera_->UpdateMatrix();

    // カメラの登録,設定
    SEED::RemoveCamera("gameCamera");// 既存の物があることを想定して登録前に削除
    SEED::RegisterCamera("gameCamera", gameCamera_.get());
    SEED::SetMainCamera("gameCamera");

    // gameObjectをクリア
    GameSystem::GetScene()->GetHierarchy()->EraseAllObject();

    // settingsの初期化
    PlaySettings::GetInstance();

    // 曲情報の保存
    songInfo_ = songInfo;
    playDifficulty_ = difficulty;
    const auto& songData = songInfo_.noteDatas[difficulty];

    // リザルトの初期化
    playResult_ = PlayResult();
    playResult_.songData = songData;

    // 譜面データの初期化
    notesData_ = std::make_unique<NotesData>();
    notesData_->Initialize(songData);
    playResult_.totalCombo = notesData_->GetTotalCombo();

    // Inputの初期化
    PlayerInput::GetInstance()->Initialize();
    PlayerInput::GetInstance()->SetNotesData(notesData_.get());

    // エディタの初期化
    if(songData.contains("jsonFilePath")){
        notesEditor_->Initialize(songData["jsonFilePath"]);
    }

    // プレイフィールドの初期化
    PlayField::GetInstance()->SetNoteData(notesData_.get());
    PlayField::GetInstance()->Initialize();

    // コンボオブジェクト
    comboObject_ = std::make_unique<ComboObject>();

    // timer
    playEndTimer_.Initialize(3.0f);

    // 判定の初期化
    Judgement::GetInstance();

    // pause状態の初期化
    isPaused_ = false;

    // チュートリアルマネージャの初期化
    if(songData["songName"] == "都立チュートリアル中学校"){
        tutorialManager_ = std::make_unique<TutorialObjectManager>(notesData_->GetSongTimerPtr());
        tutorialManager_.value()->Initialize();
    }
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

    // escapeでポーズ画面を出す
    if(!isPaused_){
        if(Input::IsTriggerKey(DIK_ESCAPE)){
            auto* scene = GameSystem::GetScene();
            scene->CauseEvent(new GameState_Pause(scene));
            Pause();
        }
    }

    // 譜面が終了している場合の処理
    if(notesData_->GetIsEnd()){

        // 譜面が終了した瞬間の処理
        if(playEndTimer_.GetProgress() == 0.0f){
            // プレイ結果をScene_Clearに渡す
            playResult_.isFullCombo = playResult_.evalutionCount[(int)Judgement::Evalution::MISS] == 0;
            playResult_.isAllPerfect =
                playResult_.isFullCombo &&
                playResult_.evalutionCount[(int)Judgement::Evalution::GREAT] == 0 &&
                playResult_.evalutionCount[(int)Judgement::Evalution::GOOD] == 0;
            Scene_Clear::SetResult(playResult_);

            // クリアエフェクトを出す
            auto* scene = GameSystem::GetScene();
            auto* hierarchy = scene->GetHierarchy();
            hierarchy;
            if(playResult_.isAllPerfect){
                hierarchy->LoadObject2D("ClearEffect/Effect_AP.prefab");
            } else{
                if(playResult_.isFullCombo){
                    hierarchy->LoadObject2D("ClearEffect/Effect_FC.prefab");
                }
            }

            // scoreDataを書き込む(無ければ作成)
            std::string path = "Resources/ScoreDatas/" + songInfo_.folderName + "/scoreData.json";
            nlohmann::json scoreJson = MyFunc::GetJson(path, true);
            static std::string difficultyName[4] = { "Basic", "Expert", "Master", "Parallel" };

            // クリアアイコンの決定
            ClearIcon clearIcon;
            if(playResult_.isAllPerfect){
                clearIcon = ClearIcon::Perfect;
            } else if(playResult_.isFullCombo){
                clearIcon = ClearIcon::FullCombo;
            } else{
                clearIcon = ClearIcon::None;
            }

            // スコアの書き込み(値を更新すれば)
            if(scoreJson.contains("score")){
                float preScore = scoreJson["score"].value(difficultyName[playDifficulty_], 0.0f);
                if(playResult_.score > preScore){
                    scoreJson["score"][difficultyName[playDifficulty_]] = playResult_.score;
                }
            } else{
                scoreJson["score"][difficultyName[playDifficulty_]] = playResult_.score;
            }

            // クリアアイコンの書き込み(値を更新すれば)
            if(scoreJson.contains("clearIcon")){
                int preIcon = scoreJson["clearIcon"].value(difficultyName[playDifficulty_], 0);
                if(static_cast<int>(clearIcon) > preIcon){
                    scoreJson["clearIcon"][difficultyName[playDifficulty_]] = static_cast<int>(clearIcon);
                }
            } else{
                scoreJson["clearIcon"][difficultyName[playDifficulty_]] = static_cast<int>(clearIcon);
            }

            // ファイルを置き換える
            MyFunc::CreateJsonFile(path, scoreJson);

            // チュートリアルマネージャの終了処理
            tutorialManager_.reset();
        }

        // プレイ終了タイマーを更新
        playEndTimer_.Update();

        // タイマーが終了したらシーン遷移
        if(playEndTimer_.IsFinished()){
            // Inputのカーソルを表示状態に戻す
            Input::SetMouseCursorVisible(true);

            // リザルト画面へ移行
            GameSystem::ChangeScene("Clear");
            return;
        }
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

        // チュートリアルマネージャの更新
        if(tutorialManager_){
            tutorialManager_.value()->Update();
        }

        // コンボテキストの更新
        comboObject_->Update();

        // スコア,ランクの計算
        playResult_.score = CalculateScore();
        playResult_.rank = ScoreRankUtils::GetScoreRank(playResult_.score);
        playResult_.ScoreTextUpdate();
    }

#ifdef _DEBUG
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

    // チュートリアルオブジェクトの描画
    if(tutorialManager_){
        tutorialManager_.value()->Draw();
    }

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
// コンボの加算、終了
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::BreakCombo(){
    comboObject_->BreakCombo();
}

void RythmGameManager::AddCombo(){
    comboObject_->AddCombo();
    comboObject_->GetComboCount() > playResult_.maxCombo ? playResult_.maxCombo = comboObject_->GetComboCount() : playResult_.maxCombo;
}

//////////////////////////////////////////////////////////////////////////////////
// スコアの計算
//////////////////////////////////////////////////////////////////////////////////
float RythmGameManager::CalculateScore(){
    float result;
    float scorePerNote = 100.0f / playResult_.totalCombo;
    float scoreSubtractRateGreat = 0.25f;
    float scoreSubtractRateGood = 0.5f;
    float subtractGreat = playResult_.evalutionCount[(int)Judgement::Evalution::GREAT] * scorePerNote * scoreSubtractRateGreat;
    float subtractGood = playResult_.evalutionCount[(int)Judgement::Evalution::GOOD] * scorePerNote * scoreSubtractRateGood;
    float subtractMiss = playResult_.evalutionCount[(int)Judgement::Evalution::MISS] * scorePerNote;
    result = 100.0f - (subtractGreat + subtractGood + subtractMiss);
    return std::clamp(result, 0.0f, 100.0f);
}


///////////////////////////////////////////////////////////////////////////////
// リトライ
///////////////////////////////////////////////////////////////////////////////
void RythmGameManager::Retry(){
    // シーンを再読み込み
    Initialize(songInfo_,playDifficulty_);
}