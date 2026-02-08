#pragma once
#include <memory>
#include <Game/Objects/Judgement/PlayField.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <Game/Objects/Notes/NotesData.h>
#include <SEED/Source/Basic/Camera/BaseCamera.h>
#include <Game/Objects/Combo/ComboObject.h>
#include <Game/Manager/NotesEditor.h>
#include <Game/Objects/Result/PlayResult.h>
#include <Game/Objects/Tutorial/TutorialObjectManager.h>

/// <summary>
/// リズムゲーム管理クラス
/// </summary>
class RhythmGameManager{
private:
    RhythmGameManager();
    RhythmGameManager(const RhythmGameManager&) = delete;
    void operator=(const RhythmGameManager&) = delete;

public:
    ~RhythmGameManager();
    static RhythmGameManager* GetInstance();
    void Initialize(const SongInfo& songInfo, int32_t difficulty);
    void BeginFrame();
    void EndFrame();
    void Update();
    void Draw();

public:
    // ゲームカメラの取得
    SEED::BaseCamera* GetCamera(){ return gameCamera_.get(); }

    // ポーズ・再開
    void Pause();
    void Resume();

    // コンボの加算や終了
    void BreakCombo();
    void AddCombo();

    // 評価関連のカウントを追加
    void AddEvaluation(Judgement::Evaluation evaluation){playResult_.evaluationCount[(int)evaluation]++;}
    void AddFastCount(int count = 1){playResult_.fastCount += count;}
    void AddLateCount(int count = 1){playResult_.lateCount += count;}

    // リトライ
    void Retry();

private:
    // スコアの計算
    float CalculateScore();

private:
    SongInfo songInfo_;// 曲情報
    int32_t playDifficulty_; // プレイ中の難易度
    std::unique_ptr<SEED::BaseCamera> gameCamera_;// カメラ
    std::unique_ptr<NotesData> notesData_;// 譜面データ
    std::unique_ptr<ComboObject> comboObject_; // コンボオブジェクト
    std::unique_ptr<NotesEditor> notesEditor_; // ノーツエディタ
    PlayResult playResult_; // プレイ結果
    SEED::Timer playEndTimer_;
    bool isPaused_ = false; // ポーズ中かどうか
    std::optional<std::unique_ptr<TutorialObjectManager>> tutorialManager_; // チュートリアルマネージャ
    std::unique_ptr<LivelyEffectsOwner> livelyEffectsOwner_;// 背景盛り上がりエフェクト
    bool isLivelyEffectActive_ = false;// 背景盛り上がりエフェクトが有効かどうか
};
